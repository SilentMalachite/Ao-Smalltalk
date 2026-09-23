#include "ao/TestRunner.hpp"

#include "ao/Compile.hpp"
#include "ao/Compiler.hpp"
#include "ao/Context.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Interpreter.hpp"
#include "ao/Send.hpp"
#include "ao/Symbol.hpp"
#include "ao/kernel/Install.hpp"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <system_error>
#include <vector>

namespace ao {
namespace {

Oop ao_AoTest_assert_equals_(CallContext& ctx, const Oop& receiver, const Oop* args,
                             std::uint32_t argc) {
  if (argc != 2 || args == nullptr) {
    return Oop{};
  }
  Root self(ctx.roots, receiver);
  Root actual(ctx.roots, args[0]);
  Root expected(ctx.roots, args[1]);
  const Oop eq = ctx.wk.intern("=");
  const Oop same = send(ctx, actual.slot, eq, &expected.slot, 1, nullptr);
  if (same.isTrue()) {
    return self.slot;
  }
  // SPEC §4.4: 不一致は error: で abort し、ファイルの失敗として runSmalltalkTests が 1 回数える。
  // 1 回目の printString が full GC を起こすと Symbol も動く。セレクタはルートに載せる。
  Root printSel(ctx.roots, ctx.wk.intern("printString"));
  Root left(ctx.roots, send(ctx, actual.slot, printSel.slot, nullptr, 0, nullptr));
  Root right(ctx.roots, send(ctx, expected.slot, printSel.slot, nullptr, 0, nullptr));
  std::string message = Str::toUtf8(ctx.heap, left.slot);
  message.append(" ~= ");
  message += Str::toUtf8(ctx.heap, right.slot);
  const auto n = static_cast<std::uint32_t>(message.size());
  Root text(ctx.roots, allocateRetry(ctx, ctx.wk.stringClass, n, kFlagBytes));
  if (text.slot.isHeap() && n != 0) {
    std::memcpy(ctx.heap.bytes(text.slot), message.data(), n);
  }
  if (!text.slot.isHeap()) {
    return Oop{};
  }
  return send(ctx, self.slot, ctx.wk.selError_, &text.slot, 1, nullptr);
}

// The only subclass: that calls WellKnown::define is the five-keyword Class method.
Oop makeAoTest(CallContext& ctx) {
  Root name(ctx.roots, ctx.wk.intern("AoTest"));
  Root empty(ctx.roots, ctx.wk.intern(""));
  if (!name.slot.isHeap() || !empty.slot.isHeap()) {
    return Oop{};
  }
  Oop args[5] = {name.slot, empty.slot, empty.slot, empty.slot, empty.slot};
  const Oop sel = ctx.wk.intern(
      "subclass:instanceVariableNames:classVariableNames:poolDictionaries:category:");
  return send(ctx, ctx.wk.objectClass, sel, args, 5, nullptr);
}

bool readFile(const std::filesystem::path& path, std::string* out) {
  std::ifstream in(path, std::ios::in | std::ios::binary);
  if (!in) {
    return false;
  }
  std::string body((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
  if (in.bad()) {
    return false;
  }
  *out = std::move(body);
  return true;
}

bool listTests(const std::filesystem::path& dir, std::vector<std::filesystem::path>* files) {
  namespace fs = std::filesystem;
  std::error_code ec;
  fs::directory_iterator it(dir, ec);
  if (ec) {
    return false;
  }
  const fs::directory_iterator end;
  while (it != end) {
    std::error_code entryEc;
    if (it->is_regular_file(entryEc) && !entryEc && it->path().extension() == ".st") {
      files->push_back(it->path());
    }
    it.increment(ec);
    if (ec) {
      return false;
    }
  }
  std::sort(files->begin(), files->end(), [](const fs::path& a, const fs::path& b) {
    return a.filename().string() < b.filename().string();
  });
  return true;
}

// SPEC §4.4: runs one file as AoTest>>doIt sent to a new instance, one outermost evaluation.
// Answers "" when the file passed, or what follows the file name on its stderr line: ": <reason>",
// or ":<start>-<end>: <message>" for a compile error, whose span is in the file's own bytes.
std::string runFile(CallContext& ctx, Root& cls, Root& doIt, const std::filesystem::path& file) {
  std::string body;
  if (!readFile(file, &body)) {
    return ": cannot read";
  }
  constexpr std::string_view kHeader = "doIt\n";
  const std::string source = std::string(kHeader) + body;
  const compiler::CompileResult compiled = compiler::compileMethod(source);
  if (!compiled.ok) {
    const std::uint32_t header = kHeader.size();
    const compiler::SourceSpan span = compiled.error.span;
    const std::uint32_t start = span.start < header ? 0 : span.start - header;
    const std::uint32_t end = span.end < header ? 0 : span.end - header;
    return ":" + std::to_string(start) + "-" + std::to_string(end) + ": " + compiled.error.message;
  }
  bool ran = false;
  Root installed(ctx.roots, installMethod(ctx, cls.slot, compiled.image));
  if (installed.slot.isHeap()) {
    Root instance(ctx.roots, send(ctx, cls.slot, ctx.wk.selNew, nullptr, 0, nullptr));
    ran = instance.slot.isHeap() &&
          !send(ctx, instance.slot, doIt.slot, nullptr, 0, nullptr).isEmpty();
  }
  // SPEC §3.4: the abort's reason (stack overflow, doesNotUnderstand:, a mismatch, ...). SPEC
  // §3.2: out of memory is an evaluation error even when a later statement ran and the file
  // reached its end, so the flag is checked after the run.
  std::string reason;
  if (ctx.aborting) {
    reason = abortReasonText(ctx);
    if (reason.empty()) {
      reason = "evaluation aborted";
    }
  } else if (ctx.heap.outOfMemory()) {
    reason = "out of memory";
  } else if (!ran) {
    reason = "evaluation failed";
  }
  clearUnwinding(ctx);
  ctx.heap.clearOutOfMemory();
  return reason.empty() ? reason : ": " + reason;
}

}  // namespace

int runSmalltalkTests(CallContext& ctx, std::string_view path) {
  namespace fs = std::filesystem;
  std::error_code ec;
  const fs::path dir{std::string(path)};
  std::vector<fs::path> files;
  // SPEC §4.4: an unreadable directory, or one without a .st file, fails.
  if (!fs::is_directory(dir, ec) || ec || !listTests(dir, &files)) {
    std::fprintf(stderr, "ao --test: %s: cannot read directory\n", dir.string().c_str());
    return 1;
  }
  if (files.empty()) {
    std::fprintf(stderr, "ao --test: %s: no .st files\n", dir.string().c_str());
    return 1;
  }
  Root cls(ctx.roots, makeAoTest(ctx));
  if (!cls.slot.isHeap() ||
      !kernel::putNative(ctx.heap, ctx.wk, ctx.cache, cls.slot, "assert:equals:", 2,
                         "ao_AoTest_assert_equals_", ao_AoTest_assert_equals_)) {
    std::fputs("ao --test: cannot define AoTest\n", stderr);
    return 1;
  }
  Root doIt(ctx.roots, ctx.wk.intern("doIt"));
  // SPEC §4.4: every file runs, in name order, whatever the earlier ones did. Each failure is one
  // line on stderr.
  int failures = 0;
  for (const fs::path& file : files) {
    // 前に立ったフラグ（起動時や前のファイルのもの）を、このファイルのせいにしない（sessionEval と同じ）。
    ctx.heap.clearOutOfMemory();
    clearUnwinding(ctx);
    refreshStackLimit(ctx);
    const std::string failure = runFile(ctx, cls, doIt, file);
    if (!failure.empty()) {
      std::fprintf(stderr, "ao --test: %s%s\n", file.string().c_str(), failure.c_str());
      ctx.testFailures += 1;
      failures += 1;
    }
  }
  return failures > 0 ? 1 : 0;
}

}  // namespace ao

#include "ao/TestRunner.hpp"

#include "ao/Compile.hpp"
#include "ao/Compiler.hpp"
#include "ao/Context.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Send.hpp"
#include "ao/Symbol.hpp"
#include "ao/kernel/Install.hpp"

#include <algorithm>
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
  ctx.testFailures += 1;
  const Oop printSel = ctx.wk.intern("printString");
  Root left(ctx.roots, send(ctx, actual.slot, printSel, nullptr, 0, nullptr));
  Root right(ctx.roots, send(ctx, expected.slot, printSel, nullptr, 0, nullptr));
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

}  // namespace

int runSmalltalkTests(CallContext& ctx, std::string_view path) {
  namespace fs = std::filesystem;
  std::error_code ec;
  const fs::path dir{std::string(path)};
  if (!fs::is_directory(dir, ec) || ec) {
    return 1;
  }
  std::vector<fs::path> files;
  if (!listTests(dir, &files)) {
    return 1;
  }
  Root cls(ctx.roots, makeAoTest(ctx));
  if (!cls.slot.isHeap()) {
    return 1;
  }
  kernel::putNative(ctx.heap, ctx.wk, cls.slot, "assert:equals:", 2, "ao_AoTest_assert_equals_",
                    ao_AoTest_assert_equals_);
  Root doIt(ctx.roots, ctx.wk.intern("doIt"));
  for (const fs::path& file : files) {
    std::string body;
    if (!readFile(file, &body)) {
      return 1;
    }
    const std::string source = "doIt\n" + body;
    const compiler::CompileResult compiled = compiler::compileMethod(source);
    if (!compiled.ok) {
      return 1;
    }
    Root installed(ctx.roots, installMethod(ctx, cls.slot, compiled.image));
    if (!installed.slot.isHeap()) {
      return 1;
    }
    Root instance(ctx.roots, send(ctx, cls.slot, ctx.wk.selNew, nullptr, 0, nullptr));
    if (!instance.slot.isHeap()) {
      return 1;
    }
    // installMethod replaces the dictionary slot and leaves the global cache.
    if (ctx.cache != nullptr) {
      ctx.cache->forget(ctx.heap, ctx.wk.classOf(instance.slot), doIt.slot);
    }
    const Oop result = send(ctx, instance.slot, doIt.slot, nullptr, 0, nullptr);
    if (result.isEmpty() || ctx.testFailures > 0) {
      return 1;
    }
  }
  return ctx.testFailures > 0 ? 1 : 0;
}

}  // namespace ao

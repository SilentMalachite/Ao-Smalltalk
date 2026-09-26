// SPEC §3.10「デバッガの読み出し」、§3.13: the ao_debug_* C ABI over the abort snapshot.

#include "ao_abi.h"

// The snapshot's pinned roots (ClearDropsRoots). Not a public header.
#include "../src/Session.hpp"

#include <cstdio>
#include <cstring>
#include <filesystem>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace {

class DebugAbi : public ::testing::Test {
 protected:
  void SetUp() override {
    ao_set_debug_capture(1);
    ASSERT_EQ(AO_OK, ao_runtime_boot());
  }
  // SPEC §3.10: the hooks and the capture setting outlive the session; a test's must not reach the
  // next test.
  void TearDown() override {
    ao_runtime_shutdown();
    ao_set_transcript_hook(nullptr, nullptr);
    ao_set_inspect_hook(nullptr, nullptr);
    ao_set_debug_capture(0);
  }

  int doIt(const char* src) {
    err_ = AoSpan{};
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_DOIT, out_, sizeof out_, &err_);
  }

  void defineClass(const char* name) {
    const std::string def = std::string("Object subclass: #") + name +
                            "\n  instanceVariableNames: ''\n  classVariableNames: ''\n"
                            "  poolDictionaries: ''\n  category: 'P10-Test'\n";
    AoSpan err{};
    ASSERT_EQ(AO_OK, ao_accept_class(def.c_str(), &err)) << err.message;
  }
  void accept(const char* cls, int meta, const char* src) {
    AoSpan err{};
    ASSERT_EQ(AO_OK, ao_accept_method(cls, meta, src, &err)) << src << ": " << err.message;
  }
  // DbgAbi: outer: x → inner: y → error:, a block frame, and a class-side failure.
  void defineDbgAbi() {
    defineClass("DbgAbi");
    accept("DbgAbi", 0, "outer: x\n  | y |\n  y := x.\n  ^self inner: y");
    accept("DbgAbi", 0, "inner: s\n  ^self error: s");
    accept("DbgAbi", 0, "blockTemps\n  | k |\n  k := 7.\n  ^#(1 2) do: [:e | | w | w := e + k. w foo]");
    accept("DbgAbi", 1, "classBoom\n  ^nil zork");
  }

  std::string label(int i) {
    char buf[256] = "unset";
    EXPECT_EQ(AO_OK, ao_debug_frame_label(i, buf, sizeof buf)) << i;
    return buf;
  }
  std::string tempName(int i, int j) {
    char buf[64] = "unset";
    EXPECT_EQ(AO_OK, ao_debug_frame_temp_name(i, j, buf, sizeof buf)) << i << "," << j;
    return buf;
  }
  std::string reason() {
    char buf[256] = "unset";
    EXPECT_EQ(AO_OK, ao_debug_reason(buf, sizeof buf));
    return buf;
  }
  // The source of frame i, its return code, and the highlighted text.
  struct Source {
    int rc = -1;
    std::string text;
    AoSpan span{};
    std::string highlighted() const { return text.substr(span.start, span.end - span.start); }
  };
  Source source(int i) {
    Source s;
    char buf[1024];
    s.span.message[0] = 'x';
    s.rc = ao_debug_frame_source(i, buf, sizeof buf, &s.span);
    s.text = buf;
    return s;
  }
  struct Printed {
    int rc = -1;
    std::string cls;
    std::string print;
  };
  Printed tempPrint(int i, int j) {
    Printed p;
    char cls[64] = "unset";
    char buf[256] = "unset";
    p.rc = ao_debug_frame_temp_print(i, j, cls, sizeof cls, buf, sizeof buf);
    p.cls = cls;
    p.print = buf;
    return p;
  }
  Printed receiverPrint(int i) {
    Printed p;
    char cls[64] = "unset";
    char buf[256] = "unset";
    p.rc = ao_debug_frame_receiver_print(i, cls, sizeof cls, buf, sizeof buf);
    p.cls = cls;
    p.print = buf;
    return p;
  }

  char out_[256] = {};
  AoSpan err_{};
};

// SPEC §3.10, §3.13: frame 0 is the innermost; error: is a native synthesized from inner:'s send.
TEST_F(DebugAbi, EvalErrorFillsFramesInnermostFirst) {
  defineDbgAbi();
  ASSERT_EQ(AO_ERR_EVAL, doIt("DbgAbi new outer: 'ab'"));
  ASSERT_EQ(4, ao_debug_frame_count());
  EXPECT_EQ(4, ao_debug_frame_total());
  EXPECT_EQ(std::string(err_.message), reason());

  EXPECT_EQ(2, ao_debug_frame_kind(0));
  EXPECT_EQ("Object>>error: native ao_Object_error_", label(0));
  EXPECT_EQ(-1, ao_debug_frame_pc(0));
  EXPECT_EQ(0, ao_debug_frame_kind(1));
  EXPECT_EQ("DbgAbi>>inner:", label(1));
  EXPECT_GT(ao_debug_frame_pc(1), 0);
  EXPECT_EQ(0, ao_debug_frame_kind(2));
  EXPECT_EQ("DbgAbi>>outer:", label(2));
  EXPECT_EQ(0, ao_debug_frame_kind(3));
  EXPECT_EQ("doIt", label(3));

  // Temps: the native's send argument, then the names the compiler gave.
  ASSERT_EQ(1, ao_debug_frame_temp_count(0));
  EXPECT_EQ("arg1", tempName(0, 0));
  ASSERT_EQ(1, ao_debug_frame_temp_count(1));
  EXPECT_EQ("s", tempName(1, 0));
  ASSERT_EQ(2, ao_debug_frame_temp_count(2));
  EXPECT_EQ("x", tempName(2, 0));
  EXPECT_EQ("y", tempName(2, 1));
  EXPECT_EQ(0, ao_debug_frame_temp_count(3));

  const Printed y = tempPrint(2, 1);
  EXPECT_EQ(AO_OK, y.rc);
  EXPECT_EQ("String", y.cls);
  EXPECT_EQ("'ab'", y.print);
  const Printed self = receiverPrint(1);
  EXPECT_EQ(AO_OK, self.rc);
  EXPECT_EQ("DbgAbi", self.cls);
  const Printed doItSelf = receiverPrint(3);
  EXPECT_EQ("UndefinedObject", doItSelf.cls);
  EXPECT_EQ("nil", doItSelf.print);

  // Out of range.
  char buf[16];
  EXPECT_EQ(-1, ao_debug_frame_kind(4));
  EXPECT_EQ(-1, ao_debug_frame_kind(-1));
  EXPECT_EQ(AO_ERR, ao_debug_frame_label(4, buf, sizeof buf));
  EXPECT_EQ(-1, ao_debug_frame_pc(4));
  EXPECT_EQ(-1, ao_debug_frame_temp_count(4));
  EXPECT_EQ(AO_ERR, ao_debug_frame_temp_name(1, 1, buf, sizeof buf));
  EXPECT_EQ(AO_ERR, ao_debug_frame_temp_name(1, -1, buf, sizeof buf));
  EXPECT_EQ(AO_ERR, ao_debug_frame_source(4, buf, sizeof buf, nullptr));
  EXPECT_EQ(AO_ERR, tempPrint(1, 1).rc);
  EXPECT_EQ(AO_ERR, receiverPrint(4).rc);
}

// SPEC §3.10: an accepted method's frame shows its source with the send in flight selected.
TEST_F(DebugAbi, FrameSourceHighlightsFailingSend) {
  defineDbgAbi();
  ASSERT_EQ(AO_ERR_EVAL, doIt("DbgAbi new outer: 'ab'"));
  const Source inner = source(1);
  EXPECT_EQ(AO_OK, inner.rc);
  EXPECT_EQ("inner: s\n  ^self error: s", inner.text);
  EXPECT_EQ("self error: s", inner.highlighted());
  EXPECT_EQ('\0', inner.span.message[0]);
  const Source outer = source(2);
  EXPECT_EQ(AO_OK, outer.rc);
  EXPECT_EQ("self inner: y", outer.highlighted());
  // highlight may be NULL.
  char buf[256];
  EXPECT_EQ(AO_OK, ao_debug_frame_source(1, buf, sizeof buf, nullptr));
}

// SPEC §3.10: the doIt's source is the evaluated text; its span leaves the doIt prefix out. A DNU
// frame has no method: its placeholder is "<label> source not available".
TEST_F(DebugAbi, DoItFrameSourceDropsPrefix) {
  const char* src = "3 + 4.\nnil foo";
  ASSERT_EQ(AO_ERR_EVAL, doIt(src));
  ASSERT_EQ(2, ao_debug_frame_count());
  EXPECT_EQ("#foo (doesNotUnderstand:)", label(0));
  EXPECT_EQ("doIt", label(1));
  const Source dnu = source(0);
  EXPECT_EQ(AO_ERR_NOSOURCE, dnu.rc);
  EXPECT_EQ("\"#foo (doesNotUnderstand:) source not available\"", dnu.text);
  EXPECT_EQ(0u, dnu.span.start);
  EXPECT_EQ(0u, dnu.span.end);
  EXPECT_EQ(0, ao_debug_frame_temp_count(0));
  const Source doItSrc = source(1);
  EXPECT_EQ(AO_OK, doItSrc.rc);
  EXPECT_EQ(src, doItSrc.text);
  EXPECT_EQ("nil foo", doItSrc.highlighted());
}

// SPEC §3.13: a block frame is labelled with its home method; its temps are its args, its temps
// and the outer variables it copied. The class side is "<Name> class".
TEST_F(DebugAbi, BlockFrameLabelIsBracketsIn) {
  defineDbgAbi();
  ASSERT_EQ(AO_ERR_EVAL, doIt("DbgAbi new blockTemps"));
  ASSERT_EQ(4, ao_debug_frame_count());
  EXPECT_EQ("#foo (doesNotUnderstand:)", label(0));
  EXPECT_EQ(1, ao_debug_frame_kind(1));
  EXPECT_EQ("[] in DbgAbi>>blockTemps", label(1));
  EXPECT_EQ("DbgAbi>>blockTemps", label(2));
  EXPECT_EQ("doIt", label(3));
  ASSERT_EQ(3, ao_debug_frame_temp_count(1));
  EXPECT_EQ("e", tempName(1, 0));
  EXPECT_EQ("w", tempName(1, 1));
  EXPECT_EQ("k", tempName(1, 2));
  EXPECT_EQ("1", tempPrint(1, 0).print);
  EXPECT_EQ("8", tempPrint(1, 1).print);
  EXPECT_EQ("7", tempPrint(1, 2).print);
  const Source block = source(1);
  EXPECT_EQ(AO_OK, block.rc);
  EXPECT_EQ(0u, block.text.find("blockTemps\n"));
  EXPECT_EQ("w foo", block.highlighted());

  ASSERT_EQ(AO_ERR_EVAL, doIt("#(1) do: [:e | e bar]"));
  EXPECT_EQ("[] in doIt", label(1));
  EXPECT_EQ("e", tempName(1, 0));

  ASSERT_EQ(AO_ERR_EVAL, doIt("DbgAbi classBoom"));
  EXPECT_EQ("DbgAbi class>>classBoom", label(1));
  EXPECT_EQ("DbgAbi class", receiverPrint(1).cls);
}

// SPEC §3.13: a failed native is synthesized with the class it was found in and its symbol; its
// source is the browser's native placeholder, and the next frame selects the send.
TEST_F(DebugAbi, NativeFrameLabelNamesSymbol) {
  const char* src = "#(1 2) at: 5";
  ASSERT_EQ(AO_ERR_EVAL, doIt(src));
  ASSERT_EQ(2, ao_debug_frame_count());
  EXPECT_EQ(2, ao_debug_frame_kind(0));
  EXPECT_EQ("ArrayedCollection>>at: native ao_ArrayedCollection_at_", label(0));
  const Source native = source(0);
  EXPECT_EQ(AO_ERR_NOSOURCE, native.rc);
  EXPECT_EQ("\"ArrayedCollection>>at: native ao_ArrayedCollection_at_\"", native.text);
  EXPECT_EQ(0u, native.span.end);
  ASSERT_EQ(1, ao_debug_frame_temp_count(0));
  EXPECT_EQ("arg1", tempName(0, 0));
  EXPECT_EQ("5", tempPrint(0, 0).print);
  EXPECT_EQ("#(1 2)", receiverPrint(0).print);
  EXPECT_EQ("Array", receiverPrint(0).cls);
  EXPECT_EQ("#(1 2) at: 5", source(1).highlighted());
}

// What a transcript hook saw and got while an ao_debug_* print ran (the runtime busy).
struct Reentry {
  std::vector<int> refused;
  std::vector<int> counts;
  std::vector<std::string> labels;
  std::vector<int> generations;
};

void reenterDebug(const char*, int, int is_clear, void* user) {
  if (is_clear != 0) {
    return;
  }
  auto* r = static_cast<Reentry*>(user);
  char cls[32];
  char buf[32];
  r->refused.push_back(ao_debug_frame_temp_print(0, 0, cls, sizeof cls, buf, sizeof buf));
  r->refused.push_back(ao_debug_frame_receiver_print(0, cls, sizeof cls, buf, sizeof buf));
  r->refused.push_back(ao_debug_inspect(0, -1));
  r->refused.push_back(ao_debug_clear());
  r->counts.push_back(ao_debug_frame_count());
  r->generations.push_back(ao_debug_generation());
  if (ao_debug_frame_label(0, buf, sizeof buf) == AO_OK) {
    r->labels.emplace_back(buf);
  }
}

// SPEC §3.10 再入: the reads work while busy; the print, inspect and clear entries are refused
// (AO_ERR) and leave the snapshot as it was. The printString here writes to the Transcript.
TEST_F(DebugAbi, TempPrintIsRefusedWhileBusy) {
  defineClass("DbgNoisy");
  accept("DbgNoisy", 0, "printString\n  Transcript show: 'p'.\n  ^'noisy'");
  ASSERT_EQ(AO_ERR_EVAL, doIt("DbgNoisy new foo: 3"));
  ASSERT_EQ(2, ao_debug_frame_count());
  const int generation = ao_debug_generation();
  Reentry seen;
  ao_set_transcript_hook(reenterDebug, &seen);
  const Printed p = receiverPrint(0);
  ao_set_transcript_hook(nullptr, nullptr);
  EXPECT_EQ(AO_OK, p.rc);
  EXPECT_EQ("DbgNoisy", p.cls);
  EXPECT_EQ("noisy", p.print);
  EXPECT_EQ((std::vector<int>{AO_ERR, AO_ERR, AO_ERR, AO_ERR}), seen.refused);
  EXPECT_EQ(std::vector<int>{2}, seen.counts);
  EXPECT_EQ(std::vector<int>{generation}, seen.generations);
  EXPECT_EQ(std::vector<std::string>{"#foo: (doesNotUnderstand:)"}, seen.labels);
  EXPECT_EQ(2, ao_debug_frame_count());
  EXPECT_EQ(generation, ao_debug_generation());
}

// SPEC §3.10: a printString that aborts leaves the value's print empty (AO_OK with the class
// only); its abort is not captured, so the snapshot and generation stay. Capture works afterwards.
TEST_F(DebugAbi, TempPrintAbortDoesNotReplaceSnapshot) {
  defineClass("DbgBad");
  accept("DbgBad", 0, "printString\n  ^self zork");
  ASSERT_EQ(AO_ERR_EVAL, doIt("| b | b := DbgBad new. b halt"));
  ASSERT_EQ("halt", reason());
  const int count = ao_debug_frame_count();
  ASSERT_EQ(2, count);
  EXPECT_EQ("Object>>halt native ao_Object_halt", label(0));
  const int generation = ao_debug_generation();
  ASSERT_EQ(1, ao_debug_frame_temp_count(1));
  EXPECT_EQ("b", tempName(1, 0));
  const Printed b = tempPrint(1, 0);
  EXPECT_EQ(AO_OK, b.rc);
  EXPECT_EQ("DbgBad", b.cls);
  EXPECT_EQ("", b.print);
  const Printed self = receiverPrint(0);
  EXPECT_EQ(AO_OK, self.rc);
  EXPECT_EQ("DbgBad", self.cls);
  EXPECT_EQ("", self.print);
  EXPECT_EQ(AO_ERR, ao_debug_inspect(0, -1));
  EXPECT_EQ(generation, ao_debug_generation());
  EXPECT_EQ(count, ao_debug_frame_count());
  EXPECT_EQ("halt", reason());
  EXPECT_EQ("Object>>halt native ao_Object_halt", label(0));

  // Capture still works: the next failure replaces the snapshot.
  ASSERT_EQ(AO_ERR_EVAL, doIt("nil foo"));
  EXPECT_EQ("#foo (doesNotUnderstand:)", label(0));
}

// SPEC §3.13 スナップショットの寿命: ao_eval clears the snapshot when it starts; each capture and
// clear moves the generation.
TEST_F(DebugAbi, NextEvalClearsSnapshotAndBumpsGeneration) {
  const int g0 = ao_debug_generation();
  ASSERT_GE(g0, 0);
  ASSERT_EQ(AO_ERR_EVAL, doIt("nil foo"));
  const int g1 = ao_debug_generation();
  EXPECT_LT(g0, g1);
  EXPECT_EQ(2, ao_debug_frame_count());
  ASSERT_EQ(AO_OK, doIt("3 + 4"));
  EXPECT_LT(g1, ao_debug_generation());
  EXPECT_EQ(0, ao_debug_frame_count());
  EXPECT_EQ(0, ao_debug_frame_total());
  char buf[16] = "unset";
  EXPECT_EQ(AO_ERR, ao_debug_reason(buf, sizeof buf));
  EXPECT_EQ(-1, ao_debug_frame_kind(0));
}

// SPEC §3.10 ao_debug_clear: empties the snapshot, gives its roots back and moves the generation.
TEST_F(DebugAbi, ClearDropsRoots) {
  ao::Session* s = ao::session();
  ASSERT_NE(nullptr, s);
  // The first failure roots one slot for good (with or without capture), and the doIt's source
  // entry stays until the next ao_eval: count after both, with capture off.
  ao_set_debug_capture(0);
  ASSERT_EQ(AO_ERR_EVAL, doIt("nil foo"));
  ASSERT_EQ(AO_OK, doIt("3"));
  ao_set_debug_capture(1);
  const std::size_t pinned = s->roots.counts().pinnedSlots;
  const std::size_t slots = s->roots.counts().slots;
  ASSERT_EQ(AO_ERR_EVAL, doIt("| a | a := 'kept'. a foo"));
  ASSERT_EQ(2, ao_debug_frame_count());
  EXPECT_LT(pinned, s->roots.counts().pinnedSlots);
  const int generation = ao_debug_generation();
  EXPECT_EQ(AO_OK, ao_debug_clear());
  EXPECT_EQ(0, ao_debug_frame_count());
  EXPECT_LT(generation, ao_debug_generation());
  EXPECT_EQ(pinned, s->roots.counts().pinnedSlots);
  // Nothing else is left rooted.
  ASSERT_EQ(AO_OK, doIt("3"));
  EXPECT_EQ(slots, s->roots.counts().slots);
}

// SPEC §3.10 ao_set_debug_capture: off, a failure leaves no frames.
TEST_F(DebugAbi, CaptureOffLeavesNoFrames) {
  ao_set_debug_capture(0);
  ASSERT_EQ(AO_ERR_EVAL, doIt("nil foo"));
  EXPECT_EQ(0, ao_debug_frame_count());
  EXPECT_EQ(0, ao_debug_frame_total());
  char buf[16];
  EXPECT_EQ(AO_ERR, ao_debug_reason(buf, sizeof buf));
  ao_set_debug_capture(1);
  ASSERT_EQ(AO_ERR_EVAL, doIt("nil foo"));
  EXPECT_EQ(2, ao_debug_frame_count());
}

// SPEC §3.10: a method without source (a methodsFor: chunk) shows the browser's placeholder with no
// selection, and its temps are named arg1… / t1….
TEST_F(DebugAbi, NoSourceMethodAnswersPlaceholderAndGenericTempNames) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_class("!Object subclass: #DbgChunk\n  instanceVariableNames: ''\n"
                                   "  classVariableNames: ''\n  poolDictionaries: ''\n"
                                   "  category: 'P10-Test'!\n"
                                   "!DbgChunk methodsFor: 'x'!\n"
                                   "two: a and: b\n  | t |\n  t := a + b.\n  ^t foo! !\n",
                                   &err))
      << err.message;
  ASSERT_EQ(AO_ERR_EVAL, doIt("DbgChunk new two: 1 and: 2"));
  ASSERT_EQ(3, ao_debug_frame_count());
  EXPECT_EQ("DbgChunk>>two:and:", label(1));
  const Source chunk = source(1);
  EXPECT_EQ(AO_ERR_NOSOURCE, chunk.rc);
  EXPECT_EQ("\"DbgChunk>>two:and: source not available\"", chunk.text);
  EXPECT_EQ(0u, chunk.span.start);
  EXPECT_EQ(0u, chunk.span.end);
  ASSERT_EQ(3, ao_debug_frame_temp_count(1));
  EXPECT_EQ("arg1", tempName(1, 0));
  EXPECT_EQ("arg2", tempName(1, 1));
  EXPECT_EQ("t1", tempName(1, 2));
  EXPECT_EQ("3", tempPrint(1, 2).print);
}

// SPEC §3.10 ao_debug_inspect: like Inspect it, the hook gets the class name and the printString;
// j -1 is the receiver.
TEST_F(DebugAbi, InspectFiresHookWithTempValue) {
  defineDbgAbi();
  ASSERT_EQ(AO_ERR_EVAL, doIt("DbgAbi new outer: 'ab'"));
  struct Seen {
    std::vector<std::string> classes;
    std::vector<std::string> prints;
  } seen;
  ao_set_inspect_hook(
      [](const char* cls, const char* print, int len, void* user) {
        auto* s = static_cast<Seen*>(user);
        s->classes.emplace_back(cls);
        s->prints.emplace_back(print, static_cast<std::size_t>(len));
      },
      &seen);
  const int generation = ao_debug_generation();
  EXPECT_EQ(AO_OK, ao_debug_inspect(2, 1));
  EXPECT_EQ(AO_OK, ao_debug_inspect(1, -1));
  EXPECT_EQ(AO_ERR, ao_debug_inspect(9, 0));
  EXPECT_EQ(AO_ERR, ao_debug_inspect(2, 2));
  EXPECT_EQ(AO_ERR, ao_debug_inspect(2, -2));
  ao_set_inspect_hook(nullptr, nullptr);
  EXPECT_EQ((std::vector<std::string>{"String", "DbgAbi"}), seen.classes);
  ASSERT_EQ(2u, seen.prints.size());
  EXPECT_EQ("'ab'", seen.prints[0]);
  EXPECT_EQ(generation, ao_debug_generation());
  EXPECT_EQ(4, ao_debug_frame_count());
}

// SPEC §3.13: a process failing in the drain of an ao_eval that answers AO_OK fills the empty
// snapshot; it is readable afterwards.
TEST_F(DebugAbi, ProcessFailureIsReadableAfterEval) {
  const char* src = "[nil bar] fork. 3";
  ASSERT_EQ(AO_OK, doIt(src));
  ASSERT_EQ(2, ao_debug_frame_count());
  EXPECT_NE(std::string::npos, reason().find("bar"));
  EXPECT_EQ("#bar (doesNotUnderstand:)", label(0));
  EXPECT_EQ("[] in doIt", label(1));
  const Source block = source(1);
  EXPECT_EQ(AO_OK, block.rc);
  EXPECT_EQ(src, block.text);
  EXPECT_EQ("nil bar", block.highlighted());
}

// SPEC §3.10 文字列バッファ: with len > 0 the buffer ends in NUL; a text that does not fit is
// AO_ERR_RANGE (AO_ERR_NOSOURCE wins over it); a NULL buffer or len < 1 is AO_ERR.
TEST_F(DebugAbi, BuffersFollowRangeRule) {
  defineDbgAbi();
  ASSERT_EQ(AO_ERR_EVAL, doIt("DbgAbi new outer: 'ab'"));
  char buf[8];
  std::memset(buf, 'x', sizeof buf);
  EXPECT_EQ(AO_ERR_RANGE, ao_debug_frame_label(1, buf, 4));
  EXPECT_STREQ("Dbg", buf);
  EXPECT_EQ(AO_ERR, ao_debug_frame_label(1, nullptr, 4));
  EXPECT_EQ(AO_ERR, ao_debug_frame_label(1, buf, 0));
  std::memset(buf, 'x', sizeof buf);
  EXPECT_EQ(AO_ERR_RANGE, ao_debug_reason(buf, 1));
  EXPECT_EQ('\0', buf[0]);
  EXPECT_EQ(AO_ERR, ao_debug_reason(nullptr, 8));

  AoSpan span{};
  std::memset(buf, 'x', sizeof buf);
  EXPECT_EQ(AO_ERR_RANGE, ao_debug_frame_source(1, buf, sizeof buf, &span));
  EXPECT_EQ('\0', buf[sizeof buf - 1]);
  EXPECT_LT(0u, span.end);  // the span is still given
  std::memset(buf, 'x', sizeof buf);
  EXPECT_EQ(AO_ERR_NOSOURCE, ao_debug_frame_source(0, buf, sizeof buf, &span));
  EXPECT_EQ('\0', buf[sizeof buf - 1]);
  EXPECT_EQ(AO_ERR, ao_debug_frame_source(1, nullptr, 8, &span));

  std::memset(buf, 'x', sizeof buf);
  EXPECT_EQ(AO_ERR_RANGE, ao_debug_frame_temp_name(2, 0, buf, 1));
  EXPECT_EQ('\0', buf[0]);

  char cls[4];
  char print[8];
  EXPECT_EQ(AO_ERR_RANGE, ao_debug_frame_temp_print(2, 0, cls, sizeof cls, print, sizeof print));
  EXPECT_STREQ("Str", cls);
  EXPECT_STREQ("'ab'", print);
  EXPECT_EQ(AO_ERR, ao_debug_frame_temp_print(2, 0, nullptr, 4, print, sizeof print));
  EXPECT_EQ(AO_ERR, ao_debug_frame_receiver_print(1, cls, sizeof cls, print, 0));
}

// SPEC §3.10 ao_set_debug_capture: the setting stays across shutdown, boot and load; the snapshot
// does not. Without a session the reads answer -1 or AO_ERR.
TEST_F(DebugAbi, CaptureSettingSurvivesBootAndLoad) {
  ASSERT_EQ(AO_OK, ao_runtime_shutdown());
  char buf[16];
  EXPECT_EQ(-1, ao_debug_generation());
  EXPECT_EQ(-1, ao_debug_frame_count());
  EXPECT_EQ(-1, ao_debug_frame_total());
  EXPECT_EQ(AO_ERR, ao_debug_reason(buf, sizeof buf));
  EXPECT_EQ(-1, ao_debug_frame_kind(0));
  EXPECT_EQ(AO_ERR, ao_debug_frame_label(0, buf, sizeof buf));
  EXPECT_EQ(AO_ERR, ao_debug_clear());
  EXPECT_EQ(AO_ERR, ao_debug_inspect(0, -1));

  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_ERR_EVAL, doIt("nil foo"));
  EXPECT_EQ(2, ao_debug_frame_count());

  const std::string path =
      (std::filesystem::temp_directory_path() / "ao-debug-abi-capture.aoimage").string();
  ASSERT_EQ(AO_OK, ao_image_save(path.c_str()));
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_image_load(path.c_str(), &err)) << err.message;
  std::remove(path.c_str());
  EXPECT_EQ(0, ao_debug_frame_count());
  ASSERT_EQ(AO_ERR_EVAL, doIt("nil foo"));
  EXPECT_EQ(2, ao_debug_frame_count());

  ao_set_debug_capture(0);
  ASSERT_EQ(AO_OK, ao_runtime_shutdown());
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_ERR_EVAL, doIt("nil foo"));
  EXPECT_EQ(0, ao_debug_frame_count());
}

}  // namespace

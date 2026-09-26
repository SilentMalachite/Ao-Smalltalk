// SPEC §3.10「デバッガの読み出し」、§3.13: the ao_debug_* C ABI over the abort snapshot.

#include "ao_abi.h"

// The snapshot's pinned roots (ClearDropsRoots). Not a public header.
#include "../src/Session.hpp"
#include "ao/Gc.hpp"

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
    ao_set_debug_mode(AO_DEBUG_POSTMORTEM);
  }

  int doIt(const char* src) {
    err_ = AoSpan{};
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_DOIT, out_, sizeof out_, &err_);
  }
  int printIt(const char* src) {
    err_ = AoSpan{};
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out_, sizeof out_,
                   &err_);
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
  // The first use of a new selector (#foo here) roots one slot for good: WellKnown::internWith
  // roots every Symbol it newly interns. The doIt's source entry stays until the next ao_eval.
  // Count after both, with capture off.
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

// SPEC §3.10: ao_set_debug_capture called from a hook inside a debugger printString takes effect
// when the print is done. A later abort in the same printString is still not captured.
TEST_F(DebugAbi, CaptureTurnedOnDuringTempPrintWaitsForTheEnd) {
  defineClass("DbgToggle");
  accept("DbgToggle", 0, "printString\n  Transcript show: 'p'.\n  ^self zork");
  ASSERT_EQ(AO_ERR_EVAL, doIt("| t | t := DbgToggle new. t halt"));
  ASSERT_EQ("halt", reason());
  ASSERT_EQ(2, ao_debug_frame_count());
  const int generation = ao_debug_generation();
  int calls = 0;
  ao_set_transcript_hook(
      [](const char*, int, int, void* user) {
        ++*static_cast<int*>(user);
        ao_set_debug_capture(1);
      },
      &calls);
  const Printed t = tempPrint(1, 0);
  ao_set_transcript_hook(nullptr, nullptr);
  EXPECT_LE(1, calls);
  EXPECT_EQ(AO_OK, t.rc);
  EXPECT_EQ("DbgToggle", t.cls);
  EXPECT_EQ("", t.print);
  EXPECT_EQ("halt", reason());
  EXPECT_EQ(generation, ao_debug_generation());
  EXPECT_EQ(2, ao_debug_frame_count());
  EXPECT_EQ("Object>>halt native ao_Object_halt", label(0));
  // Capture is on once the print is done.
  ASSERT_EQ(AO_ERR_EVAL, doIt("nil foo"));
  EXPECT_EQ("#foo (doesNotUnderstand:)", label(0));
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

// P10 受け入れ: a failure inside a vendor method (filed in from image/vendor, no source recorded)
// shows the placeholder with no selection; the doIt below it still selects its send.
TEST_F(DebugAbi, VendorMethodFrameAnswersPlaceholder) {
  const std::string order = std::string(AO_SOURCE_DIR) + "/image/vendor/LOAD_ORDER";
  ASSERT_EQ(AO_OK, ao_filein_load_order(order.c_str()));
  const char* src = "Bag new add: 3 withOccurrences: nil";
  ASSERT_EQ(AO_ERR_EVAL, doIt(src));
  const int count = ao_debug_frame_count();
  ASSERT_LE(3, count);
  EXPECT_EQ("Bag>>add:withOccurrences:", label(count - 2));
  EXPECT_EQ(0, ao_debug_frame_kind(count - 2));
  const Source vendor = source(count - 2);
  EXPECT_EQ(AO_ERR_NOSOURCE, vendor.rc);
  EXPECT_EQ("\"Bag>>add:withOccurrences: source not available\"", vendor.text);
  EXPECT_EQ(0u, vendor.span.start);
  EXPECT_EQ(0u, vendor.span.end);
  ASSERT_EQ(2, ao_debug_frame_temp_count(count - 2));
  EXPECT_EQ("arg1", tempName(count - 2, 0));
  EXPECT_EQ("3", tempPrint(count - 2, 0).print);
  EXPECT_EQ("doIt", label(count - 1));
  EXPECT_EQ(src, source(count - 1).highlighted());
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


// ---- P11: the live debugger (SPEC §3.13 ライブデバッガ, §3.10 ライブデバッガの操作) ----

void collectChunks(const char* utf8, int len, int is_clear, void* user) {
  auto* chunks = static_cast<std::vector<std::string>*>(user);
  if (is_clear == 0 && utf8 != nullptr && len >= 0) {
    chunks->emplace_back(utf8, static_cast<std::size_t>(len));
  }
}

class LiveDebug : public DebugAbi {
 protected:
  void SetUp() override {
    ao_set_debug_mode(AO_DEBUG_LIVE);
    DebugAbi::SetUp();
  }
  // DbgLive: haltIn: x | y | y := x + 1. self halt. ^y (and friends).
  void defineDbgLive() {
    defineClass("DbgLive");
    accept("DbgLive", 0, "haltIn: x\n  | y |\n  y := x + 1.\n  self halt.\n  ^y");
    accept("DbgLive", 0, "recurse\n  ^self recurse");
  }
  // The halted pid after an AO_ERR_HALT, selected for the reads.
  std::int64_t selectHalted() {
    const std::int64_t pid = ao_debug_halted_pid();
    EXPECT_GT(pid, 0);
    EXPECT_EQ(AO_OK, ao_debug_select(pid));
    return pid;
  }
};

// SPEC §3.13: halt stops the evaluating process; the frames are its live chain, innermost the
// synthesized halt native, and the temps are readable. The snapshot is left as it was (empty).
TEST_F(LiveDebug, HaltStopsWithHaltCodeAndLiveTemps) {
  defineDbgLive();
  ASSERT_EQ(AO_ERR_HALT, printIt("DbgLive new haltIn: 3"));
  EXPECT_STREQ("halt", err_.message);
  EXPECT_STREQ("", out_);
  EXPECT_EQ(1, ao_debug_halted_count());
  EXPECT_EQ(0, ao_debug_frame_count());  // the snapshot, selected by default
  const std::int64_t pid = selectHalted();
  EXPECT_EQ(1, ao_debug_can_proceed(pid));
  EXPECT_EQ("halt", reason());
  ASSERT_EQ(3, ao_debug_frame_count());
  EXPECT_EQ(3, ao_debug_frame_total());
  EXPECT_EQ("Object>>halt native ao_Object_halt", label(0));
  EXPECT_EQ("DbgLive>>haltIn:", label(1));
  EXPECT_EQ("doIt", label(2));
  ASSERT_EQ(2, ao_debug_frame_temp_count(1));
  EXPECT_EQ("x", tempName(1, 0));
  EXPECT_EQ("y", tempName(1, 1));
  EXPECT_EQ("3", tempPrint(1, 0).print);
  EXPECT_EQ("4", tempPrint(1, 1).print);
  EXPECT_EQ("DbgLive", receiverPrint(1).cls);
  const Source src = source(1);
  EXPECT_EQ(AO_OK, src.rc);
  EXPECT_EQ("self halt", src.highlighted());
  EXPECT_EQ(AO_OK, ao_debug_select(0));
  EXPECT_EQ(0, ao_debug_frame_count());
}

// SPEC §3.13: DNU, error: (a native's failure with a message too) and a failed send halt with
// their reasons, and each can be proceeded.
TEST_F(LiveDebug, DnuAndErrorAndFailedSendStop) {
  ASSERT_EQ(AO_ERR_HALT, doIt("nil foo"));
  EXPECT_STREQ("doesNotUnderstand: #foo", err_.message);
  std::int64_t pid = selectHalted();
  EXPECT_EQ(1, ao_debug_can_proceed(pid));
  EXPECT_EQ("#foo (doesNotUnderstand:)", label(0));
  ASSERT_EQ(AO_ERR_HALT, doIt("self error: 'boom'"));
  EXPECT_STREQ("boom", err_.message);
  pid = selectHalted();
  EXPECT_EQ(1, ao_debug_can_proceed(pid));
  ASSERT_EQ(AO_ERR_HALT, doIt("#(1 2) at: 5"));
  EXPECT_STREQ("basicAt: index out of range", err_.message);
  selectHalted();
  EXPECT_EQ("ArrayedCollection>>at: native ao_ArrayedCollection_at_", label(0));
  ASSERT_EQ(AO_ERR_HALT, doIt("[:a | a] value"));
  EXPECT_STREQ("failed: #value", err_.message);
  pid = selectHalted();
  EXPECT_EQ(1, ao_debug_can_proceed(pid));
  EXPECT_EQ(4, ao_debug_halted_count());
}

// SPEC §3.13: NonBoolean receiver and cannot return halt, but Proceed and Step are refused.
TEST_F(LiveDebug, NonBooleanAndCannotReturnStopWithoutProceed) {
  ASSERT_EQ(AO_ERR_HALT, doIt("3 ifTrue: [4]"));
  EXPECT_STREQ("NonBoolean receiver", err_.message);
  std::int64_t pid = selectHalted();
  EXPECT_EQ(0, ao_debug_can_proceed(pid));
  ASSERT_EQ(AO_OK, doIt("blk := [:x | ^x]"));
  ASSERT_EQ(AO_ERR_HALT, doIt("blk value: 1"));
  EXPECT_STREQ("cannot return", err_.message);
  pid = selectHalted();
  EXPECT_EQ(0, ao_debug_can_proceed(pid));
  EXPECT_EQ(2, ao_debug_halted_count());
}

// SPEC §3.13: stack overflow and out of memory are not halted: they abort and are captured.
TEST_F(LiveDebug, StackOverflowAndOomStillAbort) {
  defineDbgLive();
  ASSERT_EQ(AO_ERR_EVAL, doIt("DbgLive new recurse"));
  EXPECT_STREQ("stack overflow", err_.message);
  EXPECT_GT(ao_debug_frame_count(), 0);
  ASSERT_EQ(AO_ERR_EVAL, printIt("(Array new: 600000000) size"));
  EXPECT_STREQ("out of memory", err_.message);
  EXPECT_EQ(0, ao_debug_halted_count());
  EXPECT_EQ(0, ao_debug_halted_pid());
}

// SPEC §3.13: at most eight halted processes; the ninth failure aborts and is captured.
TEST_F(LiveDebug, NinthHaltAborts) {
  for (int k = 0; k < 8; ++k) {
    ASSERT_EQ(AO_ERR_HALT, doIt("self halt")) << k;
  }
  EXPECT_EQ(8, ao_debug_halted_count());
  ASSERT_EQ(AO_ERR_EVAL, doIt("self halt"));
  EXPECT_STREQ("halt", err_.message);
  EXPECT_EQ(8, ao_debug_halted_count());
  EXPECT_EQ(AO_OK, ao_debug_select(0));
  EXPECT_EQ(2, ao_debug_frame_count());
}

// SPEC §3.10, §3.13: a halted process does not run, so the runtime is not busy: evaluations and
// the outermost ao_debug_* entries go on.
TEST_F(LiveDebug, HaltedProcessIsNotBusy) {
  defineDbgLive();
  ASSERT_EQ(AO_ERR_HALT, printIt("DbgLive new haltIn: 3"));
  const std::int64_t pid = ao_debug_halted_pid();
  ASSERT_EQ(AO_OK, printIt("3 + 4"));
  EXPECT_STREQ("7", out_);
  ASSERT_EQ(AO_OK, ao_debug_select(pid));
  EXPECT_EQ(AO_OK, tempPrint(1, 0).rc);
  EXPECT_EQ(AO_OK, ao_debug_clear());
  EXPECT_EQ(AO_OK, ao_workspace_reset());
  EXPECT_EQ(pid, ao_debug_halted_pid());
}

// SPEC §3.10 ao_debug_select: a pid that is not halted answers AO_ERR and reads as nothing.
TEST_F(LiveDebug, SelectUnknownPidFails) {
  ao_set_debug_mode(AO_DEBUG_POSTMORTEM);
  ASSERT_EQ(AO_ERR_EVAL, doIt("nil foo"));
  ao_set_debug_mode(AO_DEBUG_LIVE);
  ASSERT_GT(ao_debug_frame_count(), 0);
  EXPECT_EQ(AO_ERR, ao_debug_select(987654321));
  EXPECT_EQ(AO_ERR, ao_debug_select(-1));
  EXPECT_EQ(0, ao_debug_frame_count());
  EXPECT_EQ(0, ao_debug_frame_total());
  char buf[16];
  EXPECT_EQ(AO_ERR, ao_debug_reason(buf, sizeof buf));
  EXPECT_EQ(AO_ERR, ao_debug_frame_label(0, buf, sizeof buf));
  EXPECT_EQ(0, ao_debug_can_proceed(987654321));
  EXPECT_EQ(AO_OK, ao_debug_select(0));
  EXPECT_GT(ao_debug_frame_count(), 0);
  ASSERT_EQ(AO_OK, ao_runtime_shutdown());
  EXPECT_EQ(AO_ERR, ao_debug_select(0));
  EXPECT_EQ(-1, ao_debug_halted_count());
  EXPECT_EQ(0, ao_debug_halted_pid());
  ASSERT_EQ(AO_OK, ao_runtime_boot());
}

// SPEC §3.13: the halted frames' values are GC roots; they move and stay through collections.
TEST_F(LiveDebug, HaltedFramesSurviveGc) {
  defineClass("DbgGc");
  accept("DbgGc", 0,
         "hold\n  | a |\n  a := Array new: 2.\n  a at: 1 put: 'kept' copy; at: 2 put: 42.\n"
         "  self halt.\n  ^a");
  ASSERT_EQ(AO_ERR_HALT, doIt("DbgGc new hold"));
  const std::int64_t pid = ao_debug_halted_pid();
  ASSERT_EQ(AO_OK, doIt("junk := (1 to: 300) collect: [:i | i printString]. junk := nil"));
  ao::Session& s = *ao::session();
  ao::Gc gc(s.heap, s.roots);
  gc.collectNursery();
  gc.collectOld();
  gc.collectNursery();
  ASSERT_EQ(AO_OK, ao_debug_select(pid));
  EXPECT_EQ("#('kept' 42)", tempPrint(1, 0).print);
}

// SPEC §3.13: a halted evaluation's doIt keeps its source while the process is halted, even
// after later evaluations.
TEST_F(LiveDebug, HaltedDoItKeepsSourceAcrossEvals) {
  ASSERT_EQ(AO_ERR_HALT, doIt("| a | a := 5. self halt. a"));
  const std::int64_t pid = ao_debug_halted_pid();
  ASSERT_EQ(AO_OK, printIt("3 + 4"));
  ASSERT_EQ(AO_OK, printIt("4 + 5"));
  ASSERT_EQ(AO_OK, ao_debug_select(pid));
  EXPECT_EQ("doIt", label(1));
  const Source src = source(1);
  EXPECT_EQ(AO_OK, src.rc);
  EXPECT_EQ("| a | a := 5. self halt. a", src.text);
  EXPECT_EQ("self halt", src.highlighted());
  EXPECT_EQ("5", tempPrint(1, 0).print);
}

// SPEC §3.13: a load and a shutdown abandon the halted processes: no cleanup runs.
TEST_F(LiveDebug, ShutdownAndLoadAbandonHaltedProcesses) {
  std::vector<std::string> seen;
  ao_set_transcript_hook(collectChunks, &seen);
  const std::string path =
      (std::filesystem::temp_directory_path() / "ao-live-debug-abandon.aoimage").string();
  ASSERT_EQ(AO_OK, ao_image_save(path.c_str()));
  ASSERT_EQ(AO_ERR_HALT, doIt("[self halt] ensure: [Transcript show: 'cleanup']"));
  EXPECT_EQ(1, ao_debug_halted_count());
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_image_load(path.c_str(), &err)) << err.message;
  std::remove(path.c_str());
  EXPECT_EQ(0, ao_debug_halted_count());
  EXPECT_EQ(0, ao_debug_halted_pid());
  ASSERT_EQ(AO_ERR_HALT, doIt("[self halt] ensure: [Transcript show: 'cleanup']"));
  ASSERT_EQ(AO_OK, ao_runtime_shutdown());
  EXPECT_TRUE(seen.empty());
  ASSERT_EQ(AO_OK, ao_runtime_boot());
}

// SPEC §3.11: an image is not saved while a process is halted.
TEST_F(LiveDebug, SaveWithHaltedProcessIsRefused) {
  const std::string path =
      (std::filesystem::temp_directory_path() / "ao-live-debug-save.aoimage").string();
  std::remove(path.c_str());
  ASSERT_EQ(AO_ERR_HALT, doIt("self halt"));
  EXPECT_EQ(AO_ERR, ao_image_save(path.c_str()));
  EXPECT_FALSE(std::filesystem::exists(path));
}

// ---- P11-04: Proceed and Abort (SPEC §3.13 操作) ----

struct ProceedFromHook {
  std::int64_t pid = 0;
  std::vector<int> answers;
};

void proceedFromHook(const char*, int, int is_clear, void* user) {
  if (is_clear != 0) {
    return;
  }
  auto* p = static_cast<ProceedFromHook*>(user);
  char out[16];
  AoSpan err{};
  p->answers.push_back(ao_debug_proceed(p->pid, out, sizeof out, &err));
  p->answers.push_back(ao_debug_abort(p->pid));
  p->answers.push_back(ao_debug_can_proceed(p->pid));
}

class LiveProceed : public LiveDebug {
 protected:
  int proceed(std::int64_t pid) {
    err_ = AoSpan{};
    return ao_debug_proceed(pid, out_, sizeof out_, &err_);
  }
};

// SPEC §3.13: Proceed answers nil from halt and runs on; the Print it's value comes back as
// ao_eval's would, with the result kept, and the process is gone.
TEST_F(LiveProceed, ProceedAnswersNilAndFinishesPrintIt) {
  defineDbgLive();
  ASSERT_EQ(AO_ERR_HALT, printIt("(DbgLive new haltIn: 3) + (self halt) printString size"));
  const std::int64_t pid = ao_debug_halted_pid();
  ASSERT_EQ(AO_ERR_HALT, proceed(pid));  // the second halt, in the doIt
  EXPECT_STREQ("halt", err_.message);
  EXPECT_EQ(pid, ao_debug_halted_pid());
  ASSERT_EQ(AO_OK, proceed(pid)) << err_.message;
  EXPECT_STREQ("7", out_);  // 4 + 'nil' size
  EXPECT_EQ(1, ao_eval_result_length());
  EXPECT_EQ(0, ao_debug_halted_count());
  EXPECT_EQ(0, ao_debug_halted_pid());
  EXPECT_EQ(AO_ERR, ao_debug_select(pid));
  EXPECT_EQ(0u, ao::session()->scheduler->liveFibers());
}

// SPEC §3.13: a proceeded process can halt again, with its new reason.
TEST_F(LiveProceed, ProceedCanHaltAgain) {
  ASSERT_EQ(AO_ERR_HALT, doIt("self halt. nil foo. 3"));
  const std::int64_t pid = ao_debug_halted_pid();
  ASSERT_EQ(AO_ERR_HALT, proceed(pid));
  EXPECT_STREQ("doesNotUnderstand: #foo", err_.message);
  EXPECT_EQ(AO_OK, ao_debug_select(pid));
  EXPECT_EQ("doesNotUnderstand: #foo", reason());
  ASSERT_EQ(AO_OK, proceed(pid));
  EXPECT_STREQ("", out_);
}

// SPEC §3.13: Proceed after a failed send makes nil the send's value.
TEST_F(LiveProceed, ProceedFailedSendPushesNil) {
  ASSERT_EQ(AO_ERR_HALT, printIt("([:a | a] value) isNil"));
  EXPECT_STREQ("failed: #value", err_.message);
  ASSERT_EQ(AO_OK, proceed(ao_debug_halted_pid())) << err_.message;
  EXPECT_STREQ("true", out_);
}

// SPEC §3.13: Abort terminates the halted process; its ensure: blocks run, and nothing more of
// the evaluation does.
TEST_F(LiveProceed, AbortRunsEnsureBlocks) {
  std::vector<std::string> seen;
  ao_set_transcript_hook(collectChunks, &seen);
  ASSERT_EQ(AO_ERR_HALT, doIt("[self error: 'x'. Transcript show: 'after'] ensure: "
                              "[Transcript show: 'done']"));
  EXPECT_STREQ("x", err_.message);
  const std::int64_t pid = ao_debug_halted_pid();
  EXPECT_EQ(AO_OK, ao_debug_abort(pid));
  EXPECT_EQ(std::vector<std::string>{"done"}, seen);
  EXPECT_EQ(0, ao_debug_halted_count());
  EXPECT_EQ(AO_ERR, ao_debug_abort(pid));
  EXPECT_EQ(0u, ao::session()->scheduler->liveFibers());
  EXPECT_EQ(0u, ao::session()->scheduler->processFailures());
}

// SPEC §3.13: a failure in a cleanup while aborting does not halt again.
TEST_F(LiveProceed, ErrorInCleanupDuringAbortDoesNotStop) {
  std::vector<std::string> seen;
  ao_set_transcript_hook(collectChunks, &seen);
  ASSERT_EQ(AO_ERR_HALT,
            doIt("[self halt] ensure: [nil foo. Transcript show: 'never']"));
  EXPECT_EQ(AO_OK, ao_debug_abort(ao_debug_halted_pid()));
  EXPECT_EQ(0, ao_debug_halted_count());
  EXPECT_TRUE(seen.empty());
  EXPECT_EQ(0u, ao::session()->scheduler->liveFibers());
}

// SPEC §3.13: Proceed on a halt that cannot go on is refused; Abort still ends it.
TEST_F(LiveProceed, ProceedOnNonProceedableIsRefused) {
  ASSERT_EQ(AO_ERR_HALT, doIt("3 ifTrue: [4]"));
  const std::int64_t pid = ao_debug_halted_pid();
  std::strcpy(out_, "unset");
  err_.message[0] = 'x';
  EXPECT_EQ(AO_ERR, ao_debug_proceed(pid, out_, sizeof out_, &err_));
  EXPECT_STREQ("", out_);
  EXPECT_STREQ("", err_.message);
  EXPECT_EQ(1, ao_debug_halted_count());
  EXPECT_EQ(AO_ERR, ao_debug_proceed(pid, nullptr, 0, &err_));
}

// SPEC §3.13: aborting a halt that cannot be proceeded runs the cleanups too.
TEST_F(LiveProceed, AbortNonProceedableRunsEnsure) {
  std::vector<std::string> seen;
  ao_set_transcript_hook(collectChunks, &seen);
  ASSERT_EQ(AO_ERR_HALT, doIt("[3 ifTrue: [4]] ensure: [Transcript show: 'done']"));
  EXPECT_EQ(AO_OK, ao_debug_abort(ao_debug_halted_pid()));
  EXPECT_EQ(std::vector<std::string>{"done"}, seen);
  EXPECT_EQ(0, ao_debug_halted_count());
}

// SPEC §3.10: Proceed and Abort are outermost entries: refused while busy (from a hook), and the
// halted process stays as it was.
TEST_F(LiveProceed, ProceedWhileBusyIsRefused) {
  ASSERT_EQ(AO_ERR_HALT, doIt("self halt"));
  ProceedFromHook hook;
  hook.pid = ao_debug_halted_pid();
  ao_set_transcript_hook(proceedFromHook, &hook);
  ASSERT_EQ(AO_OK, doIt("Transcript show: 'x'"));
  ao_set_transcript_hook(nullptr, nullptr);
  EXPECT_EQ((std::vector<int>{AO_ERR, AO_ERR, 1}), hook.answers);
  EXPECT_EQ(1, ao_debug_halted_count());
  EXPECT_EQ(AO_OK, proceed(hook.pid));
}

// SPEC §3.10: an unknown pid (or no session) is AO_ERR for Proceed and Abort.
TEST_F(LiveProceed, ProceedUnknownPidFails) {
  EXPECT_EQ(AO_ERR, proceed(424242));
  EXPECT_EQ(AO_ERR, ao_debug_abort(424242));
  EXPECT_EQ(AO_ERR, proceed(0));
  ASSERT_EQ(AO_OK, ao_runtime_shutdown());
  EXPECT_EQ(AO_ERR, proceed(1));
  EXPECT_EQ(AO_ERR, ao_debug_abort(1));
  ASSERT_EQ(AO_OK, ao_runtime_boot());
}

// SPEC §3.13: terminate from Smalltalk ends a halted process like Abort (its cleanups run);
// resume and suspend leave it halted.
TEST_F(LiveProceed, TerminateFromSmalltalkEndsHaltedProcess) {
  std::vector<std::string> seen;
  ao_set_transcript_hook(collectChunks, &seen);
  ASSERT_EQ(AO_ERR_HALT,
            doIt("p := Processor activeProcess. [self halt] ensure: [Transcript show: 'done']"));
  const std::int64_t pid = ao_debug_halted_pid();
  ASSERT_EQ(AO_OK, doIt("p resume. p suspend"));
  EXPECT_EQ(pid, ao_debug_halted_pid());
  EXPECT_TRUE(seen.empty());
  ASSERT_EQ(AO_OK, doIt("p terminate"));
  EXPECT_EQ(std::vector<std::string>{"done"}, seen);
  EXPECT_EQ(0, ao_debug_halted_count());
  EXPECT_EQ(AO_ERR, ao_debug_select(pid));
}

// ---- P11-05: Step and Debug it (SPEC §3.13 操作) ----

class LiveStep : public LiveProceed {
 protected:
  using StepFn = int (*)(int64_t, char*, int, AoSpan*);
  int step(StepFn fn, std::int64_t pid) {
    err_ = AoSpan{};
    const int rc = fn(pid, out_, sizeof out_, &err_);
    if (rc == AO_ERR_HALT) {
      EXPECT_EQ(AO_OK, ao_debug_select(pid));
    }
    return rc;
  }
  // The text the innermost frame's pc selects.
  std::string at() { return source(0).highlighted(); }
  void defineDbgStep() {
    defineClass("DbgStep");
    accept("DbgStep", 0, "two\n  | t |\n  t := 1.\n  ^t + 1");
    accept("DbgStep", 0, "inner\n  self halt.\n  ^3");
    accept("DbgStep", 0, "outer\n  | r |\n  r := self inner.\n  ^r + 1");
  }
};

// SPEC §3.13: step over stops at the next statement start of the same frame.
TEST_F(LiveStep, StepOverMovesToNextStatement) {
  ASSERT_EQ(AO_ERR_HALT, doIt("| a | self halt. a := 1. a := 2. a"));
  const std::int64_t pid = ao_debug_halted_pid();
  ASSERT_EQ(AO_ERR_HALT, step(ao_debug_step_over, pid));
  EXPECT_STREQ("step", err_.message);
  EXPECT_EQ("step", reason());
  EXPECT_EQ("doIt", label(0));
  EXPECT_EQ("a := 1", at());
  ASSERT_EQ(AO_ERR_HALT, step(ao_debug_step_over, pid));
  EXPECT_EQ("a := 2", at());
  EXPECT_EQ("1", tempPrint(0, 0).print);
}

// SPEC §3.13: step over does not stop in a block the statement runs; the evaluation's value
// comes back when it steps past the end.
TEST_F(LiveStep, StepOverDoesNotEnterBlocks) {
  ASSERT_EQ(AO_ERR_HALT, printIt("| s | self halt. s := 0. #(1 2) do: [:e | s := s + e]. s"));
  const std::int64_t pid = ao_debug_halted_pid();
  ASSERT_EQ(AO_ERR_HALT, step(ao_debug_step_over, pid));
  EXPECT_EQ("s := 0", at());
  ASSERT_EQ(AO_ERR_HALT, step(ao_debug_step_over, pid));
  EXPECT_EQ("#(1 2) do: [:e | s := s + e]", at());
  ASSERT_EQ(AO_ERR_HALT, step(ao_debug_step_over, pid));
  EXPECT_EQ("doIt", label(0));
  EXPECT_EQ(1, ao_debug_frame_count());
  ASSERT_EQ(AO_OK, step(ao_debug_step_over, pid)) << err_.message;
  EXPECT_STREQ("3", out_);
  EXPECT_EQ(0, ao_debug_halted_count());
}

// SPEC §3.13: step into enters an interpreted method at its first instruction.
TEST_F(LiveStep, StepIntoEntersInterpretedMethod) {
  defineDbgStep();
  ASSERT_EQ(AO_ERR_HALT, printIt("self halt. DbgStep new two"));
  const std::int64_t pid = ao_debug_halted_pid();
  ASSERT_EQ(AO_ERR_HALT, step(ao_debug_step_into, pid));
  EXPECT_EQ("DbgStep new two", at());
  ASSERT_EQ(AO_ERR_HALT, step(ao_debug_step_into, pid));
  EXPECT_EQ("DbgStep>>two", label(0));
  EXPECT_EQ(0, ao_debug_frame_pc(0));
  EXPECT_EQ("doIt", label(1));
  ASSERT_EQ(AO_ERR_HALT, step(ao_debug_step_into, pid));
  EXPECT_EQ("DbgStep>>two", label(0));
  EXPECT_EQ("^t + 1", at());
  ASSERT_EQ(AO_OK, proceed(pid));
  EXPECT_STREQ("2", out_);
}

// SPEC §3.13: step into does not enter a native; it stops at the next statement.
TEST_F(LiveStep, StepIntoSkipsNative) {
  ASSERT_EQ(AO_ERR_HALT, printIt("self halt. #(1 2) size. 3"));
  const std::int64_t pid = ao_debug_halted_pid();
  ASSERT_EQ(AO_ERR_HALT, step(ao_debug_step_into, pid));
  EXPECT_EQ("#(1 2) size", at());
  ASSERT_EQ(AO_ERR_HALT, step(ao_debug_step_into, pid));
  EXPECT_EQ("3", at());
  EXPECT_EQ(1, ao_debug_frame_count());
}

// SPEC §3.13: step out stops in the sender once the frame has returned.
TEST_F(LiveStep, StepOutStopsInSender) {
  defineDbgStep();
  ASSERT_EQ(AO_ERR_HALT, printIt("DbgStep new outer"));
  const std::int64_t pid = ao_debug_halted_pid();
  ASSERT_EQ(AO_OK, ao_debug_select(pid));
  EXPECT_EQ("DbgStep>>inner", label(1));
  ASSERT_EQ(AO_ERR_HALT, step(ao_debug_step_out, pid));
  EXPECT_EQ("DbgStep>>outer", label(0));
  EXPECT_EQ("doIt", label(1));
  ASSERT_EQ(AO_ERR_HALT, step(ao_debug_step_over, pid));
  EXPECT_EQ("^r + 1", at());
  EXPECT_EQ("3", tempPrint(0, 0).print);
  ASSERT_EQ(AO_ERR_HALT, step(ao_debug_step_out, pid));
  EXPECT_EQ("doIt", label(0));
  ASSERT_EQ(AO_OK, step(ao_debug_step_out, pid));
  EXPECT_STREQ("4", out_);
}

// SPEC §3.13: a step past the end of the evaluation answers as ao_eval; the non-proceedable halt
// refuses steps.
TEST_F(LiveStep, StepPastEndFinishesEval) {
  ASSERT_EQ(AO_ERR_HALT, printIt("self halt. 3 + 4"));
  const std::int64_t pid = ao_debug_halted_pid();
  ASSERT_EQ(AO_ERR_HALT, step(ao_debug_step_over, pid));
  ASSERT_EQ(AO_OK, step(ao_debug_step_over, pid)) << err_.message;
  EXPECT_STREQ("7", out_);
  ASSERT_EQ(AO_ERR_HALT, doIt("3 ifTrue: [4]"));
  const std::int64_t stuck = ao_debug_halted_pid();
  EXPECT_EQ(AO_ERR, step(ao_debug_step_into, stuck));
  EXPECT_EQ(AO_ERR, step(ao_debug_step_over, stuck));
  EXPECT_EQ(AO_ERR, step(ao_debug_step_out, stuck));
  EXPECT_EQ(AO_OK, ao_debug_abort(stuck));
}

// SPEC §3.13 Debug it: it halts before the doIt's first instruction; it ends like a Do it.
TEST_F(LiveStep, DebugItStopsAtFirstBytecode) {
  err_ = AoSpan{};
  const char* src = "x := 3. x + 1";
  ASSERT_EQ(AO_ERR_HALT, ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_DEBUGIT, out_,
                                 sizeof out_, &err_));
  EXPECT_STREQ("debug it", err_.message);
  const std::int64_t pid = ao_debug_halted_pid();
  ASSERT_EQ(AO_OK, ao_debug_select(pid));
  EXPECT_EQ("debug it", reason());
  ASSERT_EQ(1, ao_debug_frame_count());
  EXPECT_EQ("doIt", label(0));
  EXPECT_EQ(0, ao_debug_frame_pc(0));
  EXPECT_EQ("x := 3", at());
  ASSERT_EQ(AO_ERR_HALT, step(ao_debug_step_over, pid));
  EXPECT_EQ("x + 1", at());
  ASSERT_EQ(AO_OK, proceed(pid));
  EXPECT_STREQ("", out_);
  ASSERT_EQ(AO_OK, printIt("x"));
  EXPECT_STREQ("3", out_);
}

// SPEC §3.10: Debug it outside the live mode evaluates nothing.
TEST_F(DebugAbi, DebugItOutsideLiveModeIsRefused) {
  const char* src = "y := 3";
  err_ = AoSpan{};
  EXPECT_EQ(AO_ERR, ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_DEBUGIT, out_,
                            sizeof out_, &err_));
  ASSERT_EQ(AO_OK, printIt("y"));
  EXPECT_STREQ("nil", out_);
}

// ---- Review fixes (Codex) ----

// SPEC §3.13: the scheduler's own list updates do not halt; a failure there aborts and leaves the
// ready queue consistent, so a later terminate is safe.
TEST_F(LiveProceed, SchedulerListFailureAbortsInsteadOfHalting) {
  ASSERT_EQ(AO_OK, doIt("p := [1] fork. p suspend"));
  ASSERT_EQ(AO_ERR_EVAL, doIt("q := Processor instVarAt: 1. q instVarAt: 2 put: 0. p resume"));
  EXPECT_EQ(0, ao_debug_halted_count());
  ASSERT_EQ(AO_OK, doIt("q instVarAt: 2 put: 1. p terminate"));
  ASSERT_EQ(AO_OK, printIt("3 + 4"));
  EXPECT_STREQ("7", out_);
  EXPECT_EQ(0u, ao::session()->scheduler->liveFibers());
}

// SPEC §3.13: after Proceed from a native's failure with a message, the native's own failure
// mark does not halt again: the send answers nil.
TEST_F(LiveProceed, ProceedFromNativeFailureDoesNotHaltAgain) {
  ASSERT_EQ(AO_ERR_HALT, printIt("s := Semaphore new. s instVarAt: 1 put: 4611686018427387903. "
                                 "s signal"));
  EXPECT_STREQ("signal: excess signals out of range", err_.message);
  ASSERT_EQ(AO_OK, proceed(ao_debug_halted_pid())) << err_.message;
  EXPECT_STREQ("nil", out_);
  EXPECT_EQ(0, ao_debug_halted_count());
}

// SPEC §3.13: Debug it with eight processes halted aborts before running anything.
TEST_F(LiveStep, DebugItPastTheHaltLimitAborts) {
  for (int k = 0; k < 8; ++k) {
    ASSERT_EQ(AO_ERR_HALT, doIt("self halt")) << k;
  }
  err_ = AoSpan{};
  const char* src = "x := 1";
  EXPECT_EQ(AO_ERR_EVAL, ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_DEBUGIT, out_,
                                 sizeof out_, &err_));
  EXPECT_STREQ("debug it", err_.message);
  EXPECT_EQ(8, ao_debug_halted_count());
  ASSERT_EQ(AO_OK, printIt("x"));
  EXPECT_STREQ("nil", out_);
}

// SPEC §3.3: a halt's reason writes a NUL byte as \0, as an abort's does.
TEST_F(LiveDebug, HaltReasonEscapesNul) {
  ASSERT_EQ(AO_ERR_HALT, doIt("nil error: ((String new: 3) at: 1 put: $a; at: 3 put: $b; "
                              "yourself)"));
  EXPECT_STREQ("a\\0b", err_.message);
  selectHalted();
  EXPECT_EQ("a\\0b", reason());
}

}  // namespace

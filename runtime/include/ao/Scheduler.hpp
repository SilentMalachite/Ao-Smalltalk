#pragma once

#include "ao/NativeMethod.hpp"

#include <cstddef>
#include <cstdint>
#include <deque>
#include <memory>
#include <string>
#include <vector>

namespace ao {

// Instance layouts of the Kernel-Processes classes (SPEC §3.6), 0-based slots.
inline constexpr std::uint32_t kProcessSlotNextLink = 0;
inline constexpr std::uint32_t kProcessSlotSuspendedContext = 1;
inline constexpr std::uint32_t kProcessSlotPriority = 2;
inline constexpr std::uint32_t kProcessSlotMyList = 3;
inline constexpr std::uint32_t kSchedulerSlotQuiescent = 0;
inline constexpr std::uint32_t kSchedulerSlotActive = 1;
inline constexpr std::uint32_t kSemaphoreSlotExcess = 0;
inline constexpr std::uint32_t kSemaphoreSlotList = 1;
inline constexpr std::uint32_t kSharedQueueSlotContents = 0;
inline constexpr std::uint32_t kSharedQueueSlotRead = 1;
inline constexpr std::uint32_t kSharedQueueSlotWrite = 2;

// The cooperative scheduler of one session (SPEC §3.4「プロセスと協調スケジューラ」). The base
// process runs on the session's CallContext and the host thread's stack; every forked process
// runs on its own fiber (Fiber.hpp): its own CallContext, machine stack and Roots::Stack. One of
// them runs at a time, on one thread.
//
// The C++ records are the truth; what Smalltalk sees follows them: Processor's activeProcess is
// the running process, its quiescentProcesses (an OrderedCollection) holds the ready queue in
// order, a waiting process sits in its Semaphore's linkedList, and a Process's myList names the
// list it is in (nil in none).
class Scheduler {
 public:
  // SPEC §3.4: live processes other than the base (not started ones included).
  static constexpr std::size_t kMaxFibers = 256;
  // SPEC §3.4 drain, §4.4: the rounds ao_eval and ao --test drain for.
  static constexpr int kDrainRounds = 1000;

  // base is the session's context. Sets base.scheduler to this. Touches no heap object: call
  // adoptImage after boot or load.
  explicit Scheduler(CallContext& base);
  // Abandons the processes left (terminateAll(true)), so the heap must still be there, and clears
  // base.scheduler.
  ~Scheduler();
  Scheduler(const Scheduler&) = delete;
  Scheduler& operator=(const Scheduler&) = delete;

  // SPEC §3.4, §3.11: after boot and after load. Processor's activeProcess becomes the base
  // process (a new Process when it holds none), and the ready queue is emptied. Other Processes
  // in the heap have no record here: they cannot run. Only while the base runs and no fiber
  // lives. May collect.
  void adoptImage();

  // The operations of SPEC §3.4 behind the Kernel-Processes natives. ctx is the running process's
  // context (the native's). Each is a GC point and may switch: other processes run and collect
  // before it returns, so the caller holds no unrooted Oop across it. The bool ones answer false
  // when they failed; ctx is then unwinding, except for a malformed receiver (the native answers
  // the empty Oop and the send fails, SPEC §3.3).
  //
  // The new Process, or the empty Oop (not a 0-argument block: no abort; too many processes: an
  // abort). Does not switch.
  Oop fork(CallContext& ctx, Oop block);
  bool yield(CallContext& ctx);
  bool wait(CallContext& ctx, Oop semaphore);
  bool signal(CallContext& ctx, Oop semaphore);
  bool suspend(CallContext& ctx, Oop process);
  bool resume(CallContext& ctx, Oop process);
  bool terminate(CallContext& ctx, Oop process);
  // SharedQueue (SPEC §3.4): nextPut: never waits; next waits on readSynch while empty.
  bool nextPut(CallContext& ctx, Oop queue, Oop value);
  // The element taken, or the empty Oop on failure.
  Oop next(CallContext& ctx, Oop queue);

  // SPEC §3.4 drain: from the base, Processor yield up to `rounds` times, stopping once the ready
  // queue is empty. A failure during it is not the base's: the base is left not unwinding.
  void drain(int rounds);
  // Ends every process but the base, from the base, which is left not unwinding. abandon (SPEC
  // §3.4 abandon): no ensure:/ifCurtailed: cleanup, no Smalltalk code and no hook runs, and
  // nothing counts as a failure; each fiber's C++ frames still unwind, so its roots go. Otherwise
  // (SPEC §4.4) each is terminated (its cleanups run on it) and the ready queue drained, again for
  // what the cleanups forked, while that makes progress (at most kDrainRounds passes); one that a
  // cleanup left blocked is then abandoned, so none remains.
  void terminateAll(bool abandon);

  // SPEC §3.4 プロセスの失敗: how many processes ended in an abort (not by terminate), and the
  // reason of the last one.
  std::uint64_t processFailures() const { return failures_; }
  const std::string& lastFailureReason() const { return lastFailure_; }
  // A process other than the base is running (SPEC §3.10 busy).
  bool nonBaseRunning() const;
  // Live processes other than the base: not started, ready, waiting, suspended or running.
  std::size_t liveFibers() const;
  // The running process (SPEC §3.4: never nil once adoptImage ran).
  Oop activeProcess() const;

  // SPEC §3.13 評価プロセス (live mode). How the evaluation the base waited for ended.
  enum class EvalEnd { Finished, Failed, Halted };
  // From the base: a new process that will apply method (a 0-argument doIt) to nil. It is not
  // queued; awaitEval runs it. Its pid, or 0 when it cannot be made (ctx is then unwinding:
  // too many processes, out of memory). May collect. mode is the ao_eval mode it answers for.
  std::uint64_t forkEval(CallContext& ctx, Oop method, int mode);
  // From the base: runs the evaluating process pid, and the ready ones whenever it switches away,
  // until it ends (Finished with evalValue, or Failed with evalReason). A wait with nothing left
  // to run aborts it with the deadlock (SPEC §3.13). The base is left not unwinding.
  EvalEnd awaitEval(std::uint64_t pid);
  // The value of the last Finished evaluation (a GC root) until clearEvalValue; the reason of the
  // last Failed one.
  const Oop& evalValue() const { return evalValue_; }
  void clearEvalValue() { evalValue_ = Oop::nil(); }
  const std::string& evalReason() const { return evalReason_; }
  // The running process is an evaluating process.
  bool runningEval() const;

 private:
  struct Record;
  enum class State { Ready, Running, Waiting, Suspended, Dead };

  static void fiberEntry(void* arg);
  void runFiber(Record& me);
  void endEval(Record& me, CallContext& ctx, Oop result);
  Record* addFiber(CallContext& ctx, Oop process, Oop block);
  void runAwaited(Record& r);
  [[noreturn]] void finishFiber(Record& me);
  Record& base() const;
  Record* find(Oop process) const;
  Record* findId(std::uint64_t id) const;
  bool enqueue(CallContext& ctx, Record& r);
  void leaveLists(Record& r);
  Record* takeNext(Record& me);
  bool block(CallContext& ctx, Record& me);
  bool afterResume(CallContext& ctx, Record& me);
  void switchTo(Record& to);
  void reapDead();
  void abandonAll();
  void recordFailure(std::string reason);

  CallContext& base_;
  std::vector<std::unique_ptr<Record>> records_;  // front: the base
  Record* current_ = nullptr;
  std::deque<Record*> ready_;
  std::uint64_t failures_ = 0;
  std::string lastFailure_;
  // SPEC §3.13: the evaluating process the base waits for (0: none), and how the last one ended.
  std::uint64_t awaited_ = 0;
  EvalEnd evalEnd_ = EvalEnd::Failed;
  Oop evalValue_ = Oop::nil();  // a root
  std::string evalReason_;
};

}  // namespace ao

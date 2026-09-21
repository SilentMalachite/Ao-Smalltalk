#include "ao/kernel/Install.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Context.hpp"
#include "ao/MethodDictionary.hpp"
#include "ao/Symbol.hpp"

namespace ao {
namespace kernel {

void putNative(Heap& heap, WellKnown& wk, Oop cls, std::string_view selector, std::uint32_t argc,
               std::string_view name, NativeFn fn) {
  auto dict = heap.slotAt(cls, kClassSlotMethodDict);
  if (!dict.isHeap()) {
    return;
  }
  auto sel = Symbol::intern(wk, selector);
  auto idx = NativeRegistry::add(fn);
  auto meth = NativeMethod::create(heap, wk, sel, argc, name, idx, cls);
  if (!meth.isHeap()) {
    return;
  }
  MethodDictionary::atPut(heap, dict, sel, meth);
}

void installAll(Heap& heap, Roots& /*roots*/, WellKnown& wk) {
  installObject(heap, wk);
  installBoolean(heap, wk);
  installBehavior(heap, wk);
  installMagnitude(heap, wk);
  installInteger(heap, wk);
  installCharacter(heap, wk);
  installFloat(heap, wk);
  installCollection(heap, wk);
  installArray(heap, wk);
  installString(heap, wk);
  installDictionary(heap, wk);
  installStream(heap, wk);
  putNative(heap, wk, wk.blockContextClass, "value", 0, "ao_BlockContext_value",
            ao_BlockContext_value);
  putNative(heap, wk, wk.blockContextClass, "value:", 1, "ao_BlockContext_value_",
            ao_BlockContext_value_);
  putNative(heap, wk, wk.blockContextClass, "value:value:", 2, "ao_BlockContext_value_value_",
            ao_BlockContext_value_value_);
  putNative(heap, wk, wk.blockContextClass, "valueWithArguments:", 1,
            "ao_BlockContext_valueWithArguments_", ao_BlockContext_valueWithArguments_);
}

}  // namespace kernel
}  // namespace ao

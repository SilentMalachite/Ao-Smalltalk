#include "ao/kernel/Install.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Context.hpp"
#include "ao/MethodDictionary.hpp"
#include "ao/Symbol.hpp"

namespace ao {
namespace kernel {

bool putNative(Heap& heap, WellKnown& wk, Oop cls, std::string_view selector, std::uint32_t argc,
               std::string_view name, NativeFn fn) {
  auto dict = heap.slotAt(cls, kClassSlotMethodDict);
  if (!dict.isHeap()) {
    return false;
  }
  auto sel = Symbol::intern(wk, selector);
  if (!sel.isHeap()) {
    return false;
  }
  std::uint32_t idx = 0;
  if (!NativeRegistry::addNamed(name, fn, &idx)) {
    return false;
  }
  auto meth = NativeMethod::create(heap, wk, sel, argc, name, idx, cls);
  if (!meth.isHeap()) {
    return false;
  }
  return MethodDictionary::atPut(heap, dict, sel, meth);
}

void ensureNativeNames() {
  std::uint32_t idx = 0;
  if (NativeRegistry::findName("ao_Object_identityEquals", &idx)) {
    return;
  }
  Heap heap;
  Roots roots;
  WellKnown wk(heap, roots);
  Bootstrap::run(heap, roots, wk);
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
  putNative(heap, wk, wk.blockContextClass, "value:value:value:", 3,
            "ao_BlockContext_value_value_value_", ao_BlockContext_value_value_value_);
  putNative(heap, wk, wk.blockContextClass, "value:value:value:value:", 4,
            "ao_BlockContext_value_value_value_value_", ao_BlockContext_value_value_value_value_);
  putNative(heap, wk, wk.blockContextClass, "numArgs", 0, "ao_BlockContext_numArgs",
            ao_BlockContext_numArgs);
  putNative(heap, wk, wk.blockContextClass, "whileTrue:", 1, "ao_BlockContext_whileTrue_",
            ao_BlockContext_whileTrue_);
  putNative(heap, wk, wk.blockContextClass, "whileFalse:", 1, "ao_BlockContext_whileFalse_",
            ao_BlockContext_whileFalse_);
  putNative(heap, wk, wk.blockContextClass, "whileTrue", 0, "ao_BlockContext_whileTrue",
            ao_BlockContext_whileTrue);
  putNative(heap, wk, wk.blockContextClass, "whileFalse", 0, "ao_BlockContext_whileFalse",
            ao_BlockContext_whileFalse);
  putNative(heap, wk, wk.blockContextClass, "repeat", 0, "ao_BlockContext_repeat",
            ao_BlockContext_repeat);
  putNative(heap, wk, wk.blockContextClass, "ensure:", 1, "ao_BlockContext_ensure_",
            ao_BlockContext_ensure_);
  putNative(heap, wk, wk.blockContextClass, "ifCurtailed:", 1, "ao_BlockContext_ifCurtailed_",
            ao_BlockContext_ifCurtailed_);
  putNative(heap, wk, wk.blockContextClass, "cannotReturn:", 1, "ao_BlockContext_cannotReturn_",
            ao_BlockContext_cannotReturn_);
  installProcess(heap, wk);
  installGeometry(heap, wk);
  installCompiledMethod(heap, wk);
}

}  // namespace kernel
}  // namespace ao

#include "Session.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Compile.hpp"
#include "ao/Image.hpp"
#include "ao/MethodDictionary.hpp"
#include "ao/Send.hpp"
#include "ao/kernel/Install.hpp"

#include "ao_abi.h"

#include <algorithm>
#include <cstring>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace ao {
namespace {

std::unique_ptr<Session> g_session;

void installEmptyCache(Session& session, HostOopHook transcript, HostOopHook inspect) {
  session.cache = std::make_unique<ClassMethodCache>();
  session.cache->addRoots(session.roots);
  session.ctx = std::unique_ptr<CallContext>(new CallContext{
      session.heap, session.roots, session.wk, session.cache.get()});
  session.ctx->transcriptHook = transcript;
  session.ctx->inspectHook = inspect;
}

bool loadedImageProbes(Session& session) {
  Oop arg = Oop::fromSmallInteger(2);
  const Oop three =
      send(*session.ctx, Oop::fromSmallInteger(1), session.wk.intern("+"), &arg, 1, nullptr);
  const Oop isNil = send(*session.ctx, Oop::nil(), session.wk.intern("isNil"), nullptr, 0, nullptr);
  return three.isSmallInteger() && three.smallIntegerValue() == 3 && isNil.isTrue();
}

}  // namespace

Session::Session(bool bootstrap) : wk(heap, roots) {
  if (!bootstrap) {
    return;
  }
  installEmptyCache(*this, nullptr, nullptr);
  Bootstrap::run(heap, roots, wk);
}

Session* session() { return g_session.get(); }

int sessionBoot() {
  if (g_session != nullptr) {
    return 1;
  }
  g_session = std::make_unique<Session>(true);
  return 0;
}

int sessionShutdown() {
  g_session.reset();
  return 0;
}

int sessionImageSave(const char* path) {
  if (g_session == nullptr || path == nullptr) {
    return 1;
  }
  return Image::save(g_session->heap, g_session->roots, g_session->wk, path) ? 0 : 1;
}

int sessionImageLoad(const char* path) {
  if (g_session == nullptr || path == nullptr) {
    return 1;
  }
  auto next = std::make_unique<Session>(false);
  if (!Image::load(next->heap, next->roots, next->wk, path)) {
    return 1;
  }
  HostOopHook transcript = nullptr;
  HostOopHook inspect = nullptr;
  if (g_session->ctx != nullptr) {
    transcript = g_session->ctx->transcriptHook;
    inspect = g_session->ctx->inspectHook;
  }
  installEmptyCache(*next, transcript, inspect);
  g_session = std::move(next);
  ensureTranscriptClassMethods();
  if (!loadedImageProbes(*g_session)) {
    return 1;
  }
  return 0;
}

int sessionFileInLoadOrder(const char* path) {
  if (g_session == nullptr || g_session->ctx == nullptr || path == nullptr) {
    return 1;
  }
  std::vector<compiler::CompileError> errors;
  return fileInLoadOrder(*g_session->ctx, path, errors) ? 0 : 1;
}

Oop ao_Transcript_class_nextPut_(CallContext& ctx, Oop receiver, const Oop* args,
                                 std::uint32_t argc);
Oop ao_Transcript_class_nextPutAll_(CallContext& ctx, Oop receiver, const Oop* args,
                                    std::uint32_t argc);
Oop ao_Transcript_class_show_(CallContext& ctx, Oop receiver, const Oop* args,
                              std::uint32_t argc);
Oop ao_Transcript_class_cr(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Transcript_class_clear(CallContext& ctx, Oop receiver, const Oop* args,
                              std::uint32_t argc);

void ensureTranscriptClassMethods() {
  if (g_session == nullptr) {
    return;
  }
  Session& s = *g_session;
  const Oop meta = s.wk.transcriptMetaclass;
  if (!meta.isHeap()) {
    return;
  }
  Oop dict = s.heap.slotAt(meta, kClassSlotMethodDict);
  if (!dict.isHeap()) {
    dict = MethodDictionary::create(s.heap, s.wk, 8);
    if (!dict.isHeap()) {
      return;
    }
    s.heap.slotAtPut(meta, kClassSlotMethodDict, dict);
  }
  s.roots.add(&dict);
  const Oop show = s.wk.intern("show:");
  const bool present = MethodDictionary::at(s.heap, dict, show).isHeap();
  s.roots.remove(&dict);
  if (present) {
    return;
  }
  kernel::putNative(s.heap, s.wk, meta, "nextPut:", 1, "ao_Transcript_class_nextPut_",
                    ao_Transcript_class_nextPut_);
  kernel::putNative(s.heap, s.wk, meta, "nextPutAll:", 1, "ao_Transcript_class_nextPutAll_",
                    ao_Transcript_class_nextPutAll_);
  kernel::putNative(s.heap, s.wk, meta, "show:", 1, "ao_Transcript_class_show_",
                    ao_Transcript_class_show_);
  kernel::putNative(s.heap, s.wk, meta, "cr", 0, "ao_Transcript_class_cr", ao_Transcript_class_cr);
  kernel::putNative(s.heap, s.wk, meta, "clear", 0, "ao_Transcript_class_clear",
                    ao_Transcript_class_clear);
}

namespace {

struct ClassRow {
  Oop cls;
  std::string name;
  std::string category;
};

struct ListedMethod {
  std::string selector;
  bool native = false;
  Oop method;
};

bool utf8Less(const std::string& a, const std::string& b) {
  const auto* ab = reinterpret_cast<const unsigned char*>(a.data());
  const auto* bb = reinterpret_cast<const unsigned char*>(b.data());
  return std::lexicographical_compare(ab, ab + a.size(), bb, bb + b.size());
}

bool pointerSlots(Heap& heap, Oop obj, std::uint32_t minSlots) {
  return obj.isHeap() && (heap.flags(obj) & kFlagBytes) == 0 && heap.size(obj) >= minSlots;
}

std::string byteText(Heap& heap, Oop obj) {
  if (!obj.isHeap() || (heap.flags(obj) & kFlagBytes) == 0) {
    return {};
  }
  const auto n = heap.size(obj);
  if (n == 0) {
    return {};
  }
  const char* bytes = reinterpret_cast<const char*>(heap.bytes(obj));
  return std::string(bytes, static_cast<std::size_t>(n));
}

int writeBuf(const std::string& text, char* buf, int len) {
  if (buf == nullptr || len <= 0) {
    return AO_ERR;
  }
  const auto room = static_cast<std::size_t>(len - 1);
  const bool fits = text.size() <= room;
  const std::size_t n = fits ? text.size() : room;
  if (n != 0) {
    std::memcpy(buf, text.data(), n);
  }
  buf[n] = '\0';
  return fits ? AO_OK : AO_ERR_RANGE;
}

std::string classNameOf(Heap& heap, Oop cls) {
  if (!pointerSlots(heap, cls, kClassSlotName + 1)) {
    return {};
  }
  return byteText(heap, heap.slotAt(cls, kClassSlotName));
}

std::string categoryHeading(Heap& heap, Oop cls) {
  if (!pointerSlots(heap, cls, kClassSlotCategory + 1)) {
    return "Kernel";
  }
  const std::string text = byteText(heap, heap.slotAt(cls, kClassSlotCategory));
  return text.empty() ? "Kernel" : text;
}

std::string definitionCategory(Heap& heap, Oop cls) {
  if (!pointerSlots(heap, cls, kClassSlotCategory + 1)) {
    return {};
  }
  return byteText(heap, heap.slotAt(cls, kClassSlotCategory));
}

std::string instVarList(Heap& heap, Oop cls) {
  if (!pointerSlots(heap, cls, kClassSlotInstVarNames + 1)) {
    return {};
  }
  const Oop names = heap.slotAt(cls, kClassSlotInstVarNames);
  if (!pointerSlots(heap, names, 0)) {
    return {};
  }
  std::string out;
  const auto n = heap.size(names);
  for (std::uint32_t i = 0; i < n; ++i) {
    const std::string one = byteText(heap, heap.slotAt(names, i));
    if (one.empty()) {
      continue;
    }
    if (!out.empty()) {
      out.push_back(' ');
    }
    out += one;
  }
  return out;
}

std::vector<ClassRow> classRows(Session& s) {
  struct Baton {
    Session* session;
    std::vector<ClassRow>* rows;
  };
  std::vector<ClassRow> rows;
  Baton baton{&s, &rows};
  s.wk.eachClass(
      [](void* p, Oop cls) {
        auto* b = static_cast<Baton*>(p);
        if (!pointerSlots(b->session->heap, cls, kClassSlotCount)) {
          return;
        }
        ClassRow row;
        row.cls = cls;
        row.name = classNameOf(b->session->heap, cls);
        row.category = categoryHeading(b->session->heap, cls);
        b->rows->push_back(std::move(row));
      },
      &baton);
  std::sort(rows.begin(), rows.end(), [](const ClassRow& a, const ClassRow& b) {
    return utf8Less(a.name, b.name);
  });
  return rows;
}

const ClassRow* findClass(const std::vector<ClassRow>& rows, const char* name) {
  if (name == nullptr) {
    return nullptr;
  }
  for (const auto& row : rows) {
    if (row.name == name) {
      return &row;
    }
  }
  return nullptr;
}

Oop sideOf(Session& s, Oop cls, int meta) {
  if (meta == 0 || !cls.isHeap()) {
    return cls;
  }
  return s.heap.klass(cls);
}

std::vector<ListedMethod> methodsOf(Session& s, Oop side) {
  std::vector<ListedMethod> out;
  if (!pointerSlots(s.heap, side, kClassSlotMethodDict + 1)) {
    return out;
  }
  const Oop dict = s.heap.slotAt(side, kClassSlotMethodDict);
  if (!pointerSlots(s.heap, dict, kDictSlotArray + 1)) {
    return out;
  }
  const Oop inner = s.heap.slotAt(dict, kDictSlotArray);
  if (!pointerSlots(s.heap, inner, 0)) {
    return out;
  }
  const auto n = s.heap.size(inner);
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    const Oop key = s.heap.slotAt(inner, i);
    const Oop value = s.heap.slotAt(inner, i + 1);
    if (!key.isHeap() || !value.isHeap()) {
      continue;
    }
    ListedMethod row;
    row.selector = byteText(s.heap, key);
    if (row.selector.empty() && s.heap.size(key) != 0) {
      continue;
    }
    row.native = s.heap.klass(value) == s.wk.nativeMethodClass;
    row.method = value;
    out.push_back(std::move(row));
  }
  std::sort(out.begin(), out.end(), [](const ListedMethod& a, const ListedMethod& b) {
    return utf8Less(a.selector, b.selector);
  });
  return out;
}

std::vector<std::string> protocolsOf(const std::vector<ListedMethod>& methods) {
  bool native = false;
  bool user = false;
  for (const auto& method : methods) {
    if (method.native) {
      native = true;
    } else {
      user = true;
    }
  }
  std::vector<std::string> out;
  if (native) {
    out.emplace_back("native");
  }
  if (user) {
    out.emplace_back("user");
  }
  return out;
}

bool knownProtocol(const char* protocol) {
  return protocol != nullptr &&
         (std::strcmp(protocol, "native") == 0 || std::strcmp(protocol, "user") == 0);
}

std::vector<ListedMethod> selectorsFor(const std::vector<ListedMethod>& methods,
                                       const char* protocol) {
  std::vector<ListedMethod> out;
  if (!knownProtocol(protocol)) {
    return out;
  }
  const bool native = std::strcmp(protocol, "native") == 0;
  for (const auto& method : methods) {
    if (method.native == native) {
      out.push_back(method);
    }
  }
  return out;
}

std::string sourceOf(Session& s, const ListedMethod& method) {
  if (method.native) {
    const std::string_view sym = NativeMethod::nameBytes(s.heap, method.method);
    std::string text = method.selector;
    text += "\n  \"NativeMethod ";
    text.append(sym.data(), sym.size());
    text += "\"\n";
    return text;
  }
  // No session source table yet, so every CompiledMethod uses the template.
  std::string text = method.selector;
  text += "\n  \"CompiledMethod\"\n";
  return text;
}

std::string superclassName(Session& s, Oop cls, int meta) {
  const Oop side = sideOf(s, cls, meta);
  if (!pointerSlots(s.heap, side, kClassSlotSuperclass + 1)) {
    return {};
  }
  const Oop sup = s.heap.slotAt(side, kClassSlotSuperclass);
  if (!sup.isHeap()) {
    return {};
  }
  // A metaclass's superclass is another class object. Name the instance class when it has one.
  if (s.heap.klass(sup) == s.wk.metaclassClass &&
      pointerSlots(s.heap, sup, kClassSlotThisClass + 1)) {
    const Oop thisClass = s.heap.slotAt(sup, kClassSlotThisClass);
    if (thisClass.isHeap()) {
      return classNameOf(s.heap, thisClass);
    }
  }
  return classNameOf(s.heap, sup);
}

std::string definitionOf(Session& s, const ClassRow& row) {
  std::string text;
  if (!pointerSlots(s.heap, row.cls, kClassSlotSuperclass + 1)) {
    text = "nil";
  } else {
    const Oop sup = s.heap.slotAt(row.cls, kClassSlotSuperclass);
    text = sup.isHeap() ? classNameOf(s.heap, sup) : std::string("nil");
  }
  text += " subclass: #";
  text += row.name;
  text += "\n  instanceVariableNames: '";
  text += instVarList(s.heap, row.cls);
  text += "'\n  classVariableNames: ''\n  poolDictionaries: ''\n  category: '";
  text += definitionCategory(s.heap, row.cls);
  text += "'";
  return text;
}

std::vector<std::string> subclassNames(Session& s, const std::string& name,
                                       const std::vector<ClassRow>& rows) {
  std::vector<std::string> out;
  for (const auto& row : rows) {
    if (!pointerSlots(s.heap, row.cls, kClassSlotSuperclass + 1)) {
      continue;
    }
    const Oop sup = s.heap.slotAt(row.cls, kClassSlotSuperclass);
    if (!sup.isHeap()) {
      continue;
    }
    if (classNameOf(s.heap, sup) == name) {
      out.push_back(row.name);
    }
  }
  return out;
}

bool metaOk(int meta) { return meta == 0 || meta == 1; }

}  // namespace

int browserClassCount() {
  Session* s = session();
  if (s == nullptr) {
    return 0;
  }
  return static_cast<int>(classRows(*s).size());
}

int browserClassAt(int index, char* name, int nameLen, char* category, int categoryLen) {
  Session* s = session();
  if (s == nullptr) {
    return AO_ERR;
  }
  const auto rows = classRows(*s);
  if (index < 0 || static_cast<std::size_t>(index) >= rows.size()) {
    return AO_ERR;
  }
  const ClassRow& row = rows[static_cast<std::size_t>(index)];
  const int nameRc = writeBuf(row.name, name, nameLen);
  const int categoryRc = writeBuf(row.category, category, categoryLen);
  if (nameRc == AO_ERR || categoryRc == AO_ERR) {
    return AO_ERR;
  }
  if (nameRc == AO_ERR_RANGE || categoryRc == AO_ERR_RANGE) {
    return AO_ERR_RANGE;
  }
  return AO_OK;
}

int browserProtocolCount(const char* className, int meta) {
  Session* s = session();
  if (s == nullptr || className == nullptr || !metaOk(meta)) {
    return AO_ERR;
  }
  const auto rows = classRows(*s);
  const ClassRow* row = findClass(rows, className);
  if (row == nullptr) {
    return AO_ERR;
  }
  return static_cast<int>(protocolsOf(methodsOf(*s, sideOf(*s, row->cls, meta))).size());
}

int browserProtocolAt(const char* className, int meta, int index, char* buf, int len) {
  Session* s = session();
  if (s == nullptr || className == nullptr || !metaOk(meta)) {
    return AO_ERR;
  }
  const auto rows = classRows(*s);
  const ClassRow* row = findClass(rows, className);
  if (row == nullptr) {
    return AO_ERR;
  }
  const auto protocols = protocolsOf(methodsOf(*s, sideOf(*s, row->cls, meta)));
  if (index < 0 || static_cast<std::size_t>(index) >= protocols.size()) {
    return AO_ERR;
  }
  return writeBuf(protocols[static_cast<std::size_t>(index)], buf, len);
}

int browserSelectorCount(const char* className, int meta, const char* protocol) {
  Session* s = session();
  if (s == nullptr || className == nullptr || protocol == nullptr || !metaOk(meta)) {
    return AO_ERR;
  }
  const auto rows = classRows(*s);
  const ClassRow* row = findClass(rows, className);
  if (row == nullptr) {
    return AO_ERR;
  }
  if (!knownProtocol(protocol)) {
    return 0;
  }
  const auto methods = methodsOf(*s, sideOf(*s, row->cls, meta));
  return static_cast<int>(selectorsFor(methods, protocol).size());
}

int browserSelectorAt(const char* className, int meta, const char* protocol, int index, char* buf,
                      int len) {
  Session* s = session();
  if (s == nullptr || className == nullptr || protocol == nullptr || !metaOk(meta)) {
    return AO_ERR;
  }
  const auto rows = classRows(*s);
  const ClassRow* row = findClass(rows, className);
  if (row == nullptr) {
    return AO_ERR;
  }
  const auto selectors = selectorsFor(methodsOf(*s, sideOf(*s, row->cls, meta)), protocol);
  if (index < 0 || static_cast<std::size_t>(index) >= selectors.size()) {
    return AO_ERR;
  }
  return writeBuf(selectors[static_cast<std::size_t>(index)].selector, buf, len);
}

int browserSource(const char* className, int meta, const char* selector, char* buf, int len) {
  Session* s = session();
  if (s == nullptr || className == nullptr || selector == nullptr || !metaOk(meta)) {
    return AO_ERR;
  }
  const auto rows = classRows(*s);
  const ClassRow* row = findClass(rows, className);
  if (row == nullptr) {
    return AO_ERR;
  }
  const auto methods = methodsOf(*s, sideOf(*s, row->cls, meta));
  for (const auto& method : methods) {
    if (method.selector == selector) {
      return writeBuf(sourceOf(*s, method), buf, len);
    }
  }
  return AO_ERR;
}

int browserClassDefinition(const char* className, char* buf, int len) {
  Session* s = session();
  if (s == nullptr || className == nullptr) {
    return AO_ERR;
  }
  const auto rows = classRows(*s);
  const ClassRow* row = findClass(rows, className);
  if (row == nullptr) {
    return AO_ERR;
  }
  return writeBuf(definitionOf(*s, *row), buf, len);
}

int browserSuperclass(const char* className, int meta, char* buf, int len) {
  Session* s = session();
  if (s == nullptr || className == nullptr || !metaOk(meta)) {
    return AO_ERR;
  }
  const auto rows = classRows(*s);
  const ClassRow* row = findClass(rows, className);
  if (row == nullptr) {
    return AO_ERR;
  }
  return writeBuf(superclassName(*s, row->cls, meta), buf, len);
}

int browserSubclassCount(const char* className) {
  Session* s = session();
  if (s == nullptr || className == nullptr) {
    return AO_ERR;
  }
  const auto rows = classRows(*s);
  if (findClass(rows, className) == nullptr) {
    return AO_ERR;
  }
  return static_cast<int>(subclassNames(*s, className, rows).size());
}

int browserSubclassAt(const char* className, int index, char* buf, int len) {
  Session* s = session();
  if (s == nullptr || className == nullptr) {
    return AO_ERR;
  }
  const auto rows = classRows(*s);
  if (findClass(rows, className) == nullptr) {
    return AO_ERR;
  }
  const auto names = subclassNames(*s, className, rows);
  if (index < 0 || static_cast<std::size_t>(index) >= names.size()) {
    return AO_ERR;
  }
  return writeBuf(names[static_cast<std::size_t>(index)], buf, len);
}

}  // namespace ao

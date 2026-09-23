#include "ao/Codegen.hpp"

#include "ao/Bytecode.hpp"

#include <cstdint>
#include <deque>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ao::compiler {
namespace {

char32_t utf8Scalar(std::string_view s) {
  if (s.empty()) {
    return 0;
  }
  const auto* p = reinterpret_cast<const unsigned char*>(s.data());
  const std::size_t n = s.size();
  const unsigned char b0 = p[0];
  if (b0 <= 0x7F) {
    return static_cast<char32_t>(b0);
  }
  if ((b0 & 0xE0) == 0xC0 && n >= 2 && (p[1] & 0xC0) == 0x80) {
    return (static_cast<char32_t>(b0 & 0x1F) << 6) | static_cast<char32_t>(p[1] & 0x3F);
  }
  if ((b0 & 0xF0) == 0xE0 && n >= 3 && (p[1] & 0xC0) == 0x80 && (p[2] & 0xC0) == 0x80) {
    return (static_cast<char32_t>(b0 & 0x0F) << 12) | (static_cast<char32_t>(p[1] & 0x3F) << 6) |
           static_cast<char32_t>(p[2] & 0x3F);
  }
  if ((b0 & 0xF8) == 0xF0 && n >= 4 && (p[1] & 0xC0) == 0x80 && (p[2] & 0xC0) == 0x80 &&
      (p[3] & 0xC0) == 0x80) {
    return (static_cast<char32_t>(b0 & 0x07) << 18) | (static_cast<char32_t>(p[1] & 0x3F) << 12) |
           (static_cast<char32_t>(p[2] & 0x3F) << 6) | static_cast<char32_t>(p[3] & 0x3F);
  }
  return static_cast<char32_t>(b0);
}

int specialIndex(std::string_view sel) {
  for (std::uint8_t i = 0; i < specialCount(); ++i) {
    const char* s = specialSelector(i);
    if (s != nullptr && sel == s) {
      return static_cast<int>(i);
    }
  }
  return -1;
}

bool internable(LitKind k) {
  return k == LitKind::Int || k == LitKind::Symbol || k == LitKind::Binding;
}

bool sameIntern(const Literal& a, const Literal& b) {
  if (a.kind != b.kind) {
    return false;
  }
  if (a.kind == LitKind::Int) {
    return a.intValue == b.intValue;
  }
  if (a.kind == LitKind::Symbol || a.kind == LitKind::Binding) {
    return a.text == b.text;
  }
  return false;
}

// Capture analysis (SPEC §3.4, §3.8). A real scope is a method or a block that is not inlined; it
// becomes one CompiledMethod. Each variable belongs to one real scope. A temp that a nested real
// scope captures and that is assigned anywhere lives in a temp vector made on entry to the node
// that declares it; every other capture copies the value when the closure is made.
enum class VarKind : std::uint8_t { Arg, LoopVar, Temp, Hidden };

struct RealScope;

struct Var {
  std::string name;
  VarKind kind = VarKind::Temp;
  RealScope* real = nullptr;
  const Ast* decl = nullptr;  // the method or block that declares it
  bool captured = false;
  bool assigned = false;
  bool boxed = false;
  std::uint32_t slot = 0;      // temp index in real, when not boxed
  std::uint32_t vecIndex = 0;  // index in decl's temp vector, when boxed
};

struct LexScope {
  LexScope* parent = nullptr;
  RealScope* real = nullptr;
  std::vector<Var*> vars;
};

// What a real scope copies from the one that makes its closure: a variable's value, or the temp
// vector of a declaring node (owned by `owner`).
struct Capture {
  Var* var = nullptr;
  const Ast* vector = nullptr;
  RealScope* owner = nullptr;
  bool same(const Capture& o) const { return var == o.var && vector == o.vector; }
};

struct TempVector {
  const Ast* decl = nullptr;
  std::uint32_t size = 0;
  std::uint32_t slot = 0;
};

// Temp layout: [args][plain temps][temp vector slots][copied]. Copied stays last, where the
// interpreter puts the closure's copied values.
struct RealScope {
  RealScope* parent = nullptr;
  const Ast* node = nullptr;
  bool isBlock = false;
  std::uint32_t numArgs = 0;
  std::uint32_t numTemps = 0;
  std::uint32_t copiedBase = 0;
  std::vector<Var*> vars;
  std::vector<TempVector> vectors;
  std::vector<Capture> copied;
};

struct Analysis {
  std::deque<Var> vars;
  std::deque<LexScope> lexes;
  std::deque<RealScope> reals;
  std::unordered_map<const Ast*, RealScope*> realOf;             // method and real blocks
  std::unordered_map<const Ast*, Var*> localOf;                  // Variable / Assign nodes
  std::unordered_map<const Ast*, std::vector<Var*>> declared;    // declaring node -> its vars
  std::vector<std::pair<RealScope*, Var*>> outerRefs;            // uses from a nested real scope
  bool failed = false;
  CompileError error;
};

bool hasReceiverChild(const Ast& send) {
  return send.kids.size() == static_cast<std::size_t>(send.argc) + 1;
}

class Analyzer {
 public:
  explicit Analyzer(Analysis& an) : an_(an) {}

  void method(const Ast& m) {
    RealScope* real = newReal(nullptr, &m, false);
    LexScope* lex = newLex(nullptr, real);
    for (const std::string& p : m.params) {
      declare(lex, &m, p, VarKind::Arg);
    }
    for (const std::string& t : m.temps) {
      declare(lex, &m, t, VarKind::Temp);
    }
    for (const Ast& k : m.kids) {
      walk(k, lex);
    }
    layout();
  }

 private:
  Analysis& an_;

  void fail(SourceSpan span, const char* message) {
    if (an_.failed) {
      return;
    }
    an_.failed = true;
    an_.error.span = span;
    an_.error.message = message;
  }

  RealScope* newReal(RealScope* parent, const Ast* node, bool isBlock) {
    RealScope& r = an_.reals.emplace_back();
    r.parent = parent;
    r.node = node;
    r.isBlock = isBlock;
    an_.realOf[node] = &r;
    return &r;
  }

  LexScope* newLex(LexScope* parent, RealScope* real) {
    LexScope& l = an_.lexes.emplace_back();
    l.parent = parent;
    l.real = real;
    return &l;
  }

  Var* declare(LexScope* lex, const Ast* decl, const std::string& name, VarKind kind) {
    Var& v = an_.vars.emplace_back();
    v.name = name;
    v.kind = kind;
    v.real = lex->real;
    v.decl = decl;
    lex->vars.push_back(&v);
    lex->real->vars.push_back(&v);
    an_.declared[decl].push_back(&v);
    return &v;
  }

  // A use of a name. Names that are not local are left to the code generator.
  void reference(const Ast& node, LexScope* lex, bool write) {
    for (LexScope* s = lex; s != nullptr; s = s->parent) {
      for (Var* v : s->vars) {
        if (v->name != node.name) {
          continue;
        }
        an_.localOf[&node] = v;
        if (write) {
          if (v->kind == VarKind::Arg || v->kind == VarKind::LoopVar) {
            fail(node.span, "cannot assign to argument");
            return;
          }
          v->assigned = true;
        }
        if (v->real != lex->real) {
          v->captured = true;
          an_.outerRefs.emplace_back(lex->real, v);
        }
        return;
      }
    }
  }

  void walk(const Ast& n, LexScope* lex) {
    switch (n.kind) {
      case Ast::Kind::Variable:
        reference(n, lex, false);
        return;
      case Ast::Kind::Assign:
        for (const Ast& k : n.kids) {
          walk(k, lex);
        }
        reference(n, lex, true);
        return;
      case Ast::Kind::Cascade:
        // Cascade parts are always real sends; only their receiver and arguments are walked.
        for (const Ast& part : n.kids) {
          for (const Ast& k : part.kids) {
            walk(k, lex);
          }
        }
        return;
      case Ast::Kind::Block:
        realBlock(n, lex);
        return;
      default:
        for (const Ast& k : n.kids) {
          walk(k, lex);
        }
        return;
    }
  }

  void realBlock(const Ast& blk, LexScope* lex) {
    RealScope* real = newReal(lex->real, &blk, true);
    LexScope* inner = newLex(lex, real);
    for (const std::string& p : blk.params) {
      declare(inner, &blk, p, VarKind::Arg);
    }
    for (const std::string& t : blk.temps) {
      declare(inner, &blk, t, VarKind::Temp);
    }
    for (const Ast& k : blk.kids) {
      walk(k, inner);
    }
  }

  void layout() {
    for (Var& v : an_.vars) {
      v.boxed = v.kind == VarKind::Temp && v.captured && v.assigned;
    }
    for (RealScope& r : an_.reals) {
      std::uint32_t slot = 0;
      for (Var* v : r.vars) {
        if (v->kind == VarKind::Arg) {
          v->slot = slot++;
        }
      }
      r.numArgs = slot;
      for (Var* v : r.vars) {
        if (v->kind != VarKind::Arg && !v->boxed) {
          v->slot = slot++;
        }
      }
      for (Var* v : r.vars) {
        if (!v->boxed) {
          continue;
        }
        TempVector* vec = nullptr;
        for (TempVector& each : r.vectors) {
          if (each.decl == v->decl) {
            vec = &each;
          }
        }
        if (vec == nullptr) {
          vec = &r.vectors.emplace_back();
          vec->decl = v->decl;
        }
        v->vecIndex = vec->size++;
      }
      for (TempVector& vec : r.vectors) {
        vec.slot = slot++;
      }
      r.copiedBase = slot;
    }
    // Every real scope between a use and the variable's own scope copies the value or vector,
    // so the closure that needs it can copy it in turn.
    for (const auto& [from, v] : an_.outerRefs) {
      const Capture cap = v->boxed ? Capture{nullptr, v->decl, v->real} : Capture{v, nullptr, v->real};
      for (RealScope* s = from; s != nullptr && s != v->real; s = s->parent) {
        bool present = false;
        for (const Capture& each : s->copied) {
          present = present || each.same(cap);
        }
        if (!present) {
          s->copied.push_back(cap);
        }
      }
    }
    for (RealScope& r : an_.reals) {
      r.numTemps = r.copiedBase + static_cast<std::uint32_t>(r.copied.size());
      bool fits = r.numTemps <= 255;
      for (const TempVector& vec : r.vectors) {
        fits = fits && vec.size <= 255;
      }
      if (!fits) {
        fail(r.node->span, "too many temporaries");
      }
    }
  }
};;

bool nameIn(const std::vector<std::string>& names, std::string_view name) {
  for (const std::string& each : names) {
    if (each == name) {
      return true;
    }
  }
  return false;
}

class Emitter {
 public:
  // Emits the code of one real scope into image, using the capture analysis.
  Emitter(MethodImage& image, const CompileEnv& env, CompileError& error, const Analysis& an,
          const RealScope* real)
      : image_(image), env_(env), error_(error), an_(an), real_(real) {}

  bool failed() const { return failed_; }

  void compileMethod(const Ast& method) {
    image_.selector = method.name;
    image_.numArgs = static_cast<std::uint8_t>(real_->numArgs);
    image_.numTemps = static_cast<std::uint8_t>(real_->numTemps);
    image_.primitive = 0;

    std::size_t body = 0;
    if (!method.kids.empty() && method.kids[0].kind == Ast::Kind::Primitive) {
      const std::int64_t n = method.kids[0].intValue;
      if (n < 0 || n > 65535) {
        fail(method.kids[0].span, "primitive number out of range");
        return;
      }
      image_.primitive = static_cast<std::uint16_t>(n);
      emitU16(Op::Primitive, image_.primitive);
      body = 1;
    }
    emitEntry(method, false);
    if (body < method.kids.size()) {
      compileMethodBody(method.kids[body]);
    } else {
      emit(Op::ReturnReceiver);
    }
  }

  // The CompiledMethod of a block that is not inlined.
  void compileBlockMethod(const Ast& blk) {
    image_.selector = "";
    image_.numArgs = static_cast<std::uint8_t>(real_->numArgs);
    image_.numTemps = static_cast<std::uint8_t>(real_->numTemps);
    emitEntry(blk, false);
    if (blk.kids.empty()) {
      emit(Op::PushNil);
      emit(Op::ReturnTop);
      return;
    }
    compileBlockBody(blk.kids[0]);
  }

 private:
  MethodImage& image_;
  const CompileEnv& env_;
  CompileError& error_;
  const Analysis& an_;
  const RealScope* real_;
  bool failed_ = false;

  void fail(SourceSpan span, const char* message) {
    if (failed_) {
      return;
    }
    failed_ = true;
    error_.span = span;
    error_.message = message;
  }

  void emit(Op op) { image_.bytes.push_back(static_cast<std::uint8_t>(op)); }

  void emitU8(Op op, std::uint8_t a) {
    emit(op);
    image_.bytes.push_back(a);
  }

  void emitU8U8(Op op, std::uint8_t a, std::uint8_t b) {
    emit(op);
    image_.bytes.push_back(a);
    image_.bytes.push_back(b);
  }

  void emitU16(Op op, std::uint16_t a) {
    emit(op);
    image_.bytes.push_back(static_cast<std::uint8_t>(a & 0xFF));
    image_.bytes.push_back(static_cast<std::uint8_t>((a >> 8) & 0xFF));
  }

  void emitI16(Op op, std::int16_t a) { emitU16(op, static_cast<std::uint16_t>(a)); }

  std::size_t emitJump(Op op) {
    const std::size_t at = image_.bytes.size();
    emitI16(op, 0);
    return at;
  }

  void patchJump(std::size_t at) {
    if (at + 2 >= image_.bytes.size()) {
      return;
    }
    const std::size_t next = at + 3;
    const std::int32_t off = static_cast<std::int32_t>(image_.bytes.size()) - static_cast<std::int32_t>(next);
    if (off < -32768 || off > 32767) {
      fail(SourceSpan{}, "jump offset out of range");
      return;
    }
    const auto v = static_cast<std::uint16_t>(static_cast<std::int16_t>(off));
    image_.bytes[at + 1] = static_cast<std::uint8_t>(v & 0xFF);
    image_.bytes[at + 2] = static_cast<std::uint8_t>((v >> 8) & 0xFF);
  }

  bool fitU8(std::size_t n, std::uint8_t* out, SourceSpan span) {
    if (n > 255) {
      fail(span, "index exceeds 255");
      return false;
    }
    *out = static_cast<std::uint8_t>(n);
    return true;
  }

  std::uint8_t intern(Literal lit, SourceSpan span) {
    if (internable(lit.kind)) {
      for (std::size_t i = 0; i < image_.literals.size(); ++i) {
        if (sameIntern(image_.literals[i], lit)) {
          return static_cast<std::uint8_t>(i);
        }
      }
    }
    if (image_.literals.size() >= 256) {
      fail(span, "too many literals");
      return 0;
    }
    image_.literals.push_back(std::move(lit));
    return static_cast<std::uint8_t>(image_.literals.size() - 1);
  }

  int instVarIndex(std::string_view name) const {
    for (std::size_t i = 0; i < env_.instVarNames.size(); ++i) {
      if (env_.instVarNames[i] == name) {
        return static_cast<int>(i);
      }
    }
    return -1;
  }

  // The local variable a Variable or Assign node names, or null for a non-local name.
  Var* local(const Ast& node) const {
    const auto it = an_.localOf.find(&node);
    return it == an_.localOf.end() ? nullptr : it->second;
  }

  // Where this real scope keeps a copied value or vector (the analysis made sure it has it).
  std::uint8_t copiedIndex(const Capture& cap) const {
    for (std::size_t i = 0; i < real_->copied.size(); ++i) {
      if (real_->copied[i].same(cap)) {
        return static_cast<std::uint8_t>(real_->copiedBase + i);
      }
    }
    return 0;
  }

  // The temp that holds decl's temp vector in this real scope: its own slot, or a copy.
  std::uint8_t vectorTemp(const Ast* decl, const RealScope* owner) const {
    if (owner == real_) {
      for (const TempVector& vec : real_->vectors) {
        if (vec.decl == decl) {
          return static_cast<std::uint8_t>(vec.slot);
        }
      }
    }
    return copiedIndex(Capture{nullptr, decl, nullptr});
  }

  // Pushes what the closure of a nested real scope copies (CreateBlock's operands).
  void pushCapture(const Capture& cap) {
    if (cap.var != nullptr) {
      const Var* v = cap.var;
      emitU8(Op::PushTemp, v->real == real_ ? static_cast<std::uint8_t>(v->slot)
                                            : copiedIndex(cap));
      return;
    }
    emitU8(Op::PushTemp, vectorTemp(cap.vector, cap.owner));
  }

  void emitLoad(const Var* v) {
    if (v->boxed) {
      emitU8U8(Op::PushRemoteTemp, static_cast<std::uint8_t>(v->vecIndex),
               vectorTemp(v->decl, v->real));
      return;
    }
    emitU8(Op::PushTemp, v->real == real_ ? static_cast<std::uint8_t>(v->slot)
                                          : copiedIndex(Capture{const_cast<Var*>(v), nullptr,
                                                                nullptr}));
  }

  // Only boxed temps are written from a nested real scope; arguments are never written.
  void emitStore(const Var* v, bool pop) {
    if (v->boxed) {
      emitU8U8(pop ? Op::PopStoreRemoteTemp : Op::StoreRemoteTemp,
               static_cast<std::uint8_t>(v->vecIndex), vectorTemp(v->decl, v->real));
      return;
    }
    emitU8(pop ? Op::PopStoreTemp : Op::StoreTemp, static_cast<std::uint8_t>(v->slot));
  }

  // On entry to a declaring node: fresh temp vectors, and nil in an inlined block's plain temps
  // (a loop body starts each iteration with nil temps; SPEC §3.5).
  void emitEntry(const Ast& decl, bool inlined) {
    const auto it = an_.declared.find(&decl);
    if (inlined && it != an_.declared.end()) {
      for (const Var* v : it->second) {
        if (v->kind == VarKind::Temp && !v->boxed) {
          emit(Op::PushNil);
          emitU8(Op::PopStoreTemp, static_cast<std::uint8_t>(v->slot));
        }
      }
    }
    for (const TempVector& vec : real_->vectors) {
      if (vec.decl == &decl) {
        emitU8(Op::PushNewArray, static_cast<std::uint8_t>(vec.size));
        emitU8(Op::PopStoreTemp, static_cast<std::uint8_t>(vec.slot));
      }
    }
  }

  bool isPseudo(std::string_view name) const {
    return name == "self" || name == "super" || name == "thisContext" || name == "nil" ||
           name == "true" || name == "false";
  }

  bool isKnownGlobal(std::string_view name) const { return nameIn(env_.knownGlobals, name); }

  // A workspace binding (SPEC §3.10): the literal names it; the runtime boxes the Association.
  void emitLitVar(Op op, const std::string& name, SourceSpan span) {
    Literal lit;
    lit.kind = LitKind::Binding;
    lit.text = name;
    const std::uint8_t li = intern(std::move(lit), span);
    if (failed_) {
      return;
    }
    emitU8(op, li);
  }

  void emitPushGlobal(const Ast& n) {
    Literal lit;
    lit.kind = LitKind::Symbol;
    lit.text = n.name;
    const std::uint8_t li = intern(std::move(lit), n.span);
    if (failed_) {
      return;
    }
    emitU8(Op::PushGlobal, li);
  }

  void compileWorkspaceMethodBody(const Ast& body) {
    if (failed_) {
      return;
    }
    if (body.kind == Ast::Kind::Sequence) {
      if (body.kids.empty()) {
        emit(Op::ReturnReceiver);
        return;
      }
      for (std::size_t i = 0; i + 1 < body.kids.size(); ++i) {
        compileStmt(body.kids[i]);
      }
      const Ast& last = body.kids.back();
      if (last.kind == Ast::Kind::Return) {
        compileReturn(last, false);
      } else {
        compileExpr(last);
        if (!failed_) {
          emit(Op::ReturnTop);
        }
      }
      return;
    }
    if (body.kind == Ast::Kind::Return) {
      compileReturn(body, false);
      return;
    }
    compileExpr(body);
    if (!failed_) {
      emit(Op::ReturnTop);
    }
  }

  void compileMethodBody(const Ast& body) {
    if (env_.undeclaredAreBindings) {
      compileWorkspaceMethodBody(body);
      return;
    }
    if (body.kind == Ast::Kind::Sequence) {
      if (body.kids.empty()) {
        emit(Op::ReturnReceiver);
        return;
      }
      for (std::size_t i = 0; i + 1 < body.kids.size(); ++i) {
        compileStmt(body.kids[i]);
      }
      const Ast& last = body.kids.back();
      if (last.kind == Ast::Kind::Return) {
        compileReturn(last, false);
      } else {
        compileStmt(last);
        emit(Op::ReturnReceiver);
      }
      return;
    }
    if (body.kind == Ast::Kind::Return) {
      compileReturn(body, false);
      return;
    }
    compileStmt(body);
    emit(Op::ReturnReceiver);
  }

  void compileBlockBody(const Ast& body) {
    const Ast* seq = &body;
    if (body.kind != Ast::Kind::Sequence) {
      if (body.kind == Ast::Kind::Return) {
        compileReturn(body, true);
        return;
      }
      compileExpr(body);
      emit(Op::ReturnTop);
      return;
    }
    if (seq->kids.empty()) {
      emit(Op::PushNil);
      emit(Op::ReturnTop);
      return;
    }
    for (std::size_t i = 0; i + 1 < seq->kids.size(); ++i) {
      compileStmt(seq->kids[i]);
    }
    const Ast& last = seq->kids.back();
    if (last.kind == Ast::Kind::Return) {
      compileReturn(last, true);
    } else {
      compileExpr(last);
      emit(Op::ReturnTop);
    }
  }

  void compileStmt(const Ast& n) {
    if (failed_) {
      return;
    }
    if (n.kind == Ast::Kind::Return) {
      compileReturn(n, real_->isBlock);
      return;
    }
    if (n.kind == Ast::Kind::Assign) {
      compileAssign(n, true);
      return;
    }
    if (n.kind == Ast::Kind::Sequence) {
      for (const Ast& k : n.kids) {
        compileStmt(k);
      }
      return;
    }
    compileExpr(n);
    emit(Op::Pop);
  }

  void compileExpr(const Ast& n) {
    if (failed_) {
      return;
    }
    switch (n.kind) {
      case Ast::Kind::Assign:
        compileAssign(n, false);
        return;
      case Ast::Kind::Send:
        compileSend(n, false);
        return;
      case Ast::Kind::Cascade:
        compileCascade(n);
        return;
      case Ast::Kind::Variable:
        compileVariable(n);
        return;
      case Ast::Kind::Literal:
        compileLiteral(n);
        return;
      case Ast::Kind::Block:
        compileBlock(n);
        return;
      case Ast::Kind::Sequence:
        if (n.kids.empty()) {
          emit(Op::PushNil);
          return;
        }
        for (std::size_t i = 0; i + 1 < n.kids.size(); ++i) {
          compileStmt(n.kids[i]);
        }
        compileExpr(n.kids.back());
        return;
      case Ast::Kind::Return:
        compileReturn(n, real_->isBlock);
        return;
      case Ast::Kind::Method:
      case Ast::Kind::Primitive:
        fail(n.span, "unexpected node in expression");
        return;
    }
  }

  bool isSelfishReturn(const Ast& expr, Op* op) const {
    if (expr.kind != Ast::Kind::Variable) {
      return false;
    }
    if (expr.name == "self") {
      *op = Op::ReturnReceiver;
      return true;
    }
    if (expr.name == "true") {
      *op = Op::ReturnTrue;
      return true;
    }
    if (expr.name == "false") {
      *op = Op::ReturnFalse;
      return true;
    }
    if (expr.name == "nil") {
      *op = Op::ReturnNil;
      return true;
    }
    return false;
  }

  void compileReturn(const Ast& n, bool inBlock) {
    const Ast* expr = n.kids.empty() ? nullptr : &n.kids[0];
    if (inBlock) {
      if (expr == nullptr) {
        emit(Op::PushNil);
      } else {
        compileExpr(*expr);
      }
      emit(Op::ReturnBlock);
      return;
    }
    if (expr == nullptr) {
      emit(Op::ReturnNil);
      return;
    }
    Op shortRet = Op::ReturnTop;
    if (isSelfishReturn(*expr, &shortRet)) {
      emit(shortRet);
      return;
    }
    compileExpr(*expr);
    emit(Op::ReturnTop);
  }

  void compileAssign(const Ast& n, bool asStmt) {
    if (n.kids.empty()) {
      fail(n.span, "assignment missing value");
      return;
    }
    compileExpr(n.kids[0]);
    if (failed_) {
      return;
    }
    if (const Var* v = local(n)) {
      emitStore(v, asStmt);
      return;
    }
    std::uint8_t idx = 0;
    const int iv = instVarIndex(n.name);
    if (iv >= 0) {
      if (!fitU8(static_cast<std::size_t>(iv), &idx, n.span)) {
        return;
      }
      emitU8(asStmt ? Op::PopStoreInstVar : Op::StoreInstVar, idx);
      return;
    }
    if (env_.undeclaredAreBindings && !isPseudo(n.name) && !isKnownGlobal(n.name)) {
      emitLitVar(asStmt ? Op::PopStoreLitVar : Op::StoreLitVar, n.name, n.span);
      return;
    }
    fail(n.span, "cannot assign");
  }

  void compileSend(const Ast& send, bool skipReceiver) {
    if (!skipReceiver) {
      if (send.isSuper) {
        emit(Op::PushReceiver);
      } else if (hasReceiverChild(send)) {
        compileExpr(send.kids[0]);
      } else {
        fail(send.span, "send missing receiver");
        return;
      }
    }
    const std::size_t arg0 = hasReceiverChild(send) ? 1 : 0;
    for (std::size_t i = arg0; i < send.kids.size(); ++i) {
      compileExpr(send.kids[i]);
    }
    if (failed_) {
      return;
    }
    const int spec = specialIndex(send.name);
    if (spec >= 0 && !send.isSuper) {
      emitU8U8(Op::SendSpecial, static_cast<std::uint8_t>(spec), send.argc);
      return;
    }
    Literal lit;
    lit.kind = LitKind::Symbol;
    lit.text = send.name;
    const std::uint8_t li = intern(std::move(lit), send.span);
    emitU8U8(send.isSuper ? Op::SendSuper : Op::Send, li, send.argc);
  }

  void compileCascade(const Ast& casc) {
    if (casc.kids.empty()) {
      fail(casc.span, "empty cascade");
      return;
    }
    const Ast& first = casc.kids[0];
    if (first.isSuper) {
      emit(Op::PushReceiver);
    } else if (hasReceiverChild(first)) {
      compileExpr(first.kids[0]);
    } else {
      fail(first.span, "cascade missing receiver");
      return;
    }
    for (std::size_t i = 0; i < casc.kids.size(); ++i) {
      const bool last = i + 1 == casc.kids.size();
      if (!last) {
        emit(Op::Dup);
      }
      compileSend(casc.kids[i], true);
      if (!last) {
        emit(Op::Pop);
      }
    }
  }

  void compileVariable(const Ast& n) {
    if (const Var* v = local(n)) {
      emitLoad(v);
      return;
    }
    std::uint8_t idx = 0;
    const int iv = instVarIndex(n.name);
    if (iv >= 0) {
      if (!fitU8(static_cast<std::size_t>(iv), &idx, n.span)) {
        return;
      }
      emitU8(Op::PushInstVar, idx);
      return;
    }
    if (n.name == "self") {
      emit(Op::PushReceiver);
      return;
    }
    if (n.name == "super") {
      fail(n.span, "super must be a send receiver");
      return;
    }
    if (n.name == "thisContext") {
      emit(Op::PushThisContext);
      return;
    }
    if (n.name == "nil") {
      emit(Op::PushNil);
      return;
    }
    if (n.name == "true") {
      emit(Op::PushTrue);
      return;
    }
    if (n.name == "false") {
      emit(Op::PushFalse);
      return;
    }
    if (env_.undeclaredAreBindings && !isKnownGlobal(n.name)) {
      emitLitVar(Op::PushLitVar, n.name, n.span);
      return;
    }
    emitPushGlobal(n);
  }

  Literal fromAstLiteral(const Ast& n) {
    Literal lit;
    if (n.name == "'") {
      lit.kind = LitKind::String;
      lit.text = n.text;
      return lit;
    }
    if (n.name == "nil") {
      lit.kind = LitKind::Nil;
      return lit;
    }
    if (n.name == "true") {
      lit.kind = LitKind::True;
      return lit;
    }
    if (n.name == "false") {
      lit.kind = LitKind::False;
      return lit;
    }
    if (n.name == "#") {
      lit.kind = LitKind::Symbol;
      lit.text = n.text;
      return lit;
    }
    if (n.name == "$") {
      lit.kind = LitKind::Char;
      lit.intValue = static_cast<std::int64_t>(utf8Scalar(n.text));
      lit.text = n.text;
      return lit;
    }
    if (n.name == "#(") {
      lit.kind = LitKind::Array;
      lit.elements.reserve(n.kids.size());
      for (const Ast& k : n.kids) {
        lit.elements.push_back(fromAstLiteral(k));
      }
      return lit;
    }
    if (n.name == "#[") {
      lit.kind = LitKind::ByteArray;
      lit.bytes.reserve(n.kids.size());
      for (const Ast& k : n.kids) {
        lit.bytes.push_back(static_cast<std::uint8_t>(k.intValue));
      }
      return lit;
    }
    if (n.isFloat) {
      lit.kind = LitKind::Float;
      lit.floatValue = n.floatValue;
      return lit;
    }
    lit.kind = LitKind::Int;
    lit.intValue = n.intValue;
    return lit;
  }

  void compileLiteral(const Ast& n) {
    if (n.name.empty() && !n.isFloat) {
      switch (n.intValue) {
        case -1:
          emit(Op::PushMinusOne);
          return;
        case 0:
          emit(Op::PushZero);
          return;
        case 1:
          emit(Op::PushOne);
          return;
        case 2:
          emit(Op::PushTwo);
          return;
        default:
          break;
      }
    }
    const std::uint8_t li = intern(fromAstLiteral(n), n.span);
    emitU8(Op::PushLiteral, li);
  }

  // A block that is not inlined: its own CompiledMethod, and a closure over what it copies.
  void compileBlock(const Ast& blk) {
    const auto it = an_.realOf.find(&blk);
    if (it == an_.realOf.end()) {
      fail(blk.span, "block missing from the analysis");
      return;
    }
    const RealScope* inner = it->second;
    MethodImage image;
    Emitter innerEm(image, env_, error_, an_, inner);
    innerEm.failed_ = failed_;
    innerEm.compileBlockMethod(blk);
    failed_ = innerEm.failed_;
    if (failed_) {
      return;
    }
    for (const Capture& cap : inner->copied) {
      pushCapture(cap);
    }
    Literal lit;
    lit.kind = LitKind::Method;
    lit.method = std::make_unique<MethodImage>(std::move(image));
    const std::uint8_t li = intern(std::move(lit), blk.span);
    emitU8U8(Op::CreateBlock, li, static_cast<std::uint8_t>(inner->copied.size()));
  }
};

const char* opName(Op op) {
  switch (op) {
    case Op::PushReceiver:
      return "PushReceiver";
    case Op::PushTrue:
      return "PushTrue";
    case Op::PushFalse:
      return "PushFalse";
    case Op::PushNil:
      return "PushNil";
    case Op::PushThisContext:
      return "PushThisContext";
    case Op::PushMinusOne:
      return "PushMinusOne";
    case Op::PushZero:
      return "PushZero";
    case Op::PushOne:
      return "PushOne";
    case Op::PushTwo:
      return "PushTwo";
    case Op::PushTemp:
      return "PushTemp";
    case Op::PushInstVar:
      return "PushInstVar";
    case Op::PushLiteral:
      return "PushLiteral";
    case Op::PushGlobal:
      return "PushGlobal";
    case Op::Pop:
      return "Pop";
    case Op::Dup:
      return "Dup";
    case Op::StoreTemp:
      return "StoreTemp";
    case Op::StoreInstVar:
      return "StoreInstVar";
    case Op::PopStoreTemp:
      return "PopStoreTemp";
    case Op::PopStoreInstVar:
      return "PopStoreInstVar";
    case Op::Send:
      return "Send";
    case Op::SendSuper:
      return "SendSuper";
    case Op::SendSpecial:
      return "SendSpecial";
    case Op::Jump:
      return "Jump";
    case Op::JumpTrue:
      return "JumpTrue";
    case Op::JumpFalse:
      return "JumpFalse";
    case Op::ReturnReceiver:
      return "ReturnReceiver";
    case Op::ReturnTrue:
      return "ReturnTrue";
    case Op::ReturnFalse:
      return "ReturnFalse";
    case Op::ReturnNil:
      return "ReturnNil";
    case Op::ReturnTop:
      return "ReturnTop";
    case Op::ReturnBlock:
      return "ReturnBlock";
    case Op::CreateBlock:
      return "CreateBlock";
    case Op::Primitive:
      return "Primitive";
    case Op::PushNewArray:
      return "PushNewArray";
    case Op::PushRemoteTemp:
      return "PushRemoteTemp";
    case Op::StoreRemoteTemp:
      return "StoreRemoteTemp";
    case Op::PopStoreRemoteTemp:
      return "PopStoreRemoteTemp";
    case Op::PushLitVar:
      return "PushLitVar";
    case Op::StoreLitVar:
      return "StoreLitVar";
    case Op::PopStoreLitVar:
      return "PopStoreLitVar";
  }
  return "Unknown";
}

std::string formatLit(const Literal& lit) {
  switch (lit.kind) {
    case LitKind::Nil:
      return "nil";
    case LitKind::True:
      return "true";
    case LitKind::False:
      return "false";
    case LitKind::Int:
      return std::to_string(lit.intValue);
    case LitKind::Float: {
      std::ostringstream os;
      os << lit.floatValue;
      return os.str();
    }
    case LitKind::Char:
      return "$" + lit.text;
    case LitKind::String:
      return "'" + lit.text + "'";
    case LitKind::Symbol:
      return lit.text;
    case LitKind::Array:
      return "#(";
    case LitKind::ByteArray:
      return "#[";
    case LitKind::Method:
      return "[method]";
    case LitKind::Binding:
      return "{" + lit.text + "}";
  }
  return "?";
}

}  // namespace

void codegen(const Ast& method, const CompileEnv& env, CompileResult& out) {
  out.ok = false;
  out.image = MethodImage{};
  out.error = CompileError{};
  if (method.kind != Ast::Kind::Method) {
    out.error.span = method.span;
    out.error.message = "expected method";
    return;
  }
  Analysis an;
  Analyzer(an).method(method);
  if (an.failed) {
    out.error = an.error;
    return;
  }
  Emitter em(out.image, env, out.error, an, an.realOf.at(&method));
  em.compileMethod(method);
  if (!em.failed()) {
    out.ok = true;
  }
}

std::string disassemble(const MethodImage& image) {
  std::ostringstream os;
  os << "method " << image.selector << " args=" << static_cast<unsigned>(image.numArgs)
     << " temps=" << static_cast<unsigned>(image.numTemps)
     << " prim=" << static_cast<unsigned>(image.primitive) << "\n";
  os << "literals:";
  for (const Literal& lit : image.literals) {
    os << ' ' << formatLit(lit);
  }
  os << "\n";
  const std::vector<std::uint8_t>& b = image.bytes;
  std::size_t i = 0;
  while (i < b.size()) {
    const auto op = static_cast<Op>(b[i]);
    const std::uint8_t n = operandBytes(op);
    os << "  " << opName(op);
    if (i + n >= b.size()) {
      os << "\n";
      break;
    }
    if (n == 1) {
      os << ' ' << static_cast<unsigned>(b[i + 1]);
    } else if (n == 2) {
      if (op == Op::Jump || op == Op::JumpTrue || op == Op::JumpFalse) {
        const auto u = static_cast<std::uint16_t>(b[i + 1]) |
                       static_cast<std::uint16_t>(static_cast<std::uint16_t>(b[i + 2]) << 8);
        os << ' ' << static_cast<int>(static_cast<std::int16_t>(u));
      } else if (op == Op::Primitive) {
        const auto u = static_cast<std::uint16_t>(b[i + 1]) |
                       static_cast<std::uint16_t>(static_cast<std::uint16_t>(b[i + 2]) << 8);
        os << ' ' << static_cast<unsigned>(u);
      } else {
        os << ' ' << static_cast<unsigned>(b[i + 1]) << ' ' << static_cast<unsigned>(b[i + 2]);
      }
    }
    os << "\n";
    i += 1u + n;
  }
  return os.str();
}

}  // namespace ao::compiler

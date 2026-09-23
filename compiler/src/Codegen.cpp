#include "ao/Codegen.hpp"

#include "ao/Bytecode.hpp"

#include <cstdint>
#include <sstream>
#include <string>
#include <string_view>
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

bool internable(LitKind k) { return k == LitKind::Int || k == LitKind::Symbol; }

bool sameIntern(const Literal& a, const Literal& b) {
  if (a.kind != b.kind) {
    return false;
  }
  if (a.kind == LitKind::Int) {
    return a.intValue == b.intValue;
  }
  if (a.kind == LitKind::Symbol) {
    return a.text == b.text;
  }
  return false;
}

struct Scope {
  Scope* parent = nullptr;
  bool isBlock = false;
  std::vector<std::string> args;
  std::vector<std::string> temps;
  std::vector<std::string> copied;
};

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
  Emitter(MethodImage& image, const CompileEnv& env, CompileError& error)
      : image_(image), bindingsImage_(&image), env_(env), error_(error) {}

  bool failed() const { return failed_; }

  void compileMethod(const Ast& method) {
    scope_.parent = nullptr;
    scope_.isBlock = false;
    scope_.args = method.params;
    scope_.temps.clear();
    scope_.copied.clear();
    image_.tempBindings.clear();
    if (env_.undeclaredAreTemps) {
      for (const std::string& name : env_.workspaceTemps) {
        if (nameIn(scope_.args, name) || nameIn(scope_.temps, name)) {
          continue;
        }
        scope_.temps.push_back(name);
        image_.tempBindings.push_back(MethodImage::TempBinding{name, true});
      }
      for (const std::string& name : method.temps) {
        if (nameIn(scope_.args, name) || nameIn(scope_.temps, name)) {
          continue;
        }
        scope_.temps.push_back(name);
        image_.tempBindings.push_back(MethodImage::TempBinding{name, false});
      }
    } else {
      scope_.temps = method.temps;
    }
    const std::size_t ntemps = scope_.args.size() + scope_.temps.size();
    if (scope_.args.size() > 255 || scope_.temps.size() > 255 || ntemps > 255) {
      fail(method.span, env_.undeclaredAreTemps ? "too many temporaries"
                                                : "too many arguments or temporaries");
      return;
    }
    image_.selector = method.name;
    image_.numArgs = static_cast<std::uint8_t>(method.params.size());
    image_.numTemps = static_cast<std::uint8_t>(ntemps);
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
    if (body < method.kids.size()) {
      compileMethodBody(method.kids[body]);
    } else {
      emit(Op::ReturnReceiver);
    }
    if (failed_ || !env_.undeclaredAreTemps) {
      return;
    }
    const std::size_t finalCount = scope_.args.size() + scope_.temps.size() + scope_.copied.size();
    if (finalCount > 255) {
      fail(method.span, "too many temporaries");
      return;
    }
    image_.numTemps = static_cast<std::uint8_t>(finalCount);
  }

 private:
  MethodImage& image_;
  MethodImage* bindingsImage_;
  const CompileEnv& env_;
  CompileError& error_;
  Scope scope_{};
  Scope* cur_ = &scope_;
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

  bool localIndex(Scope* s, std::string_view name, std::uint8_t* index) const {
    for (std::size_t i = 0; i < s->args.size(); ++i) {
      if (s->args[i] == name) {
        *index = static_cast<std::uint8_t>(i);
        return true;
      }
    }
    for (std::size_t i = 0; i < s->temps.size(); ++i) {
      if (s->temps[i] == name) {
        *index = static_cast<std::uint8_t>(s->args.size() + i);
        return true;
      }
    }
    for (std::size_t i = 0; i < s->copied.size(); ++i) {
      if (s->copied[i] == name) {
        *index = static_cast<std::uint8_t>(s->args.size() + s->temps.size() + i);
        return true;
      }
    }
    return false;
  }

  bool bindTemp(Scope* s, std::string_view name, std::uint8_t* index, SourceSpan span) {
    if (localIndex(s, name, index)) {
      return true;
    }
    if (s->parent == nullptr) {
      return false;
    }
    std::uint8_t outer = 0;
    if (!bindTemp(s->parent, name, &outer, span)) {
      return false;
    }
    if (s->args.size() + s->temps.size() + s->copied.size() >= 256) {
      fail(span, "too many temporaries");
      return false;
    }
    s->copied.emplace_back(name);
    *index = static_cast<std::uint8_t>(s->args.size() + s->temps.size() + s->copied.size() - 1);
    return true;
  }

  bool isPseudo(std::string_view name) const {
    return name == "self" || name == "super" || name == "thisContext" || name == "nil" ||
           name == "true" || name == "false";
  }

  bool isKnownGlobal(std::string_view name) const { return nameIn(env_.knownGlobals, name); }

  Scope* methodScope() const {
    Scope* s = cur_;
    while (s->parent != nullptr) {
      s = s->parent;
    }
    return s;
  }

  bool addWorkspaceTemp(std::string_view name, SourceSpan span) {
    Scope* method = methodScope();
    std::uint8_t existing = 0;
    if (localIndex(method, name, &existing)) {
      return true;
    }
    const std::size_t n = method->args.size() + method->temps.size() + method->copied.size();
    if (n >= 255) {
      fail(span, "too many temporaries");
      return false;
    }
    method->temps.emplace_back(name);
    bindingsImage_->tempBindings.push_back(MethodImage::TempBinding{std::string(name), true});
    return true;
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
    if (env_.undeclaredAreTemps) {
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
      compileReturn(n, cur_->isBlock);
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
        compileReturn(n, cur_->isBlock);
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
    std::uint8_t idx = 0;
    if (bindTemp(cur_, n.name, &idx, n.span)) {
      emitU8(asStmt ? Op::PopStoreTemp : Op::StoreTemp, idx);
      return;
    }
    const int iv = instVarIndex(n.name);
    if (iv >= 0) {
      if (!fitU8(static_cast<std::size_t>(iv), &idx, n.span)) {
        return;
      }
      emitU8(asStmt ? Op::PopStoreInstVar : Op::StoreInstVar, idx);
      return;
    }
    if (env_.undeclaredAreTemps && !isPseudo(n.name) && !isKnownGlobal(n.name)) {
      if (!addWorkspaceTemp(n.name, n.span) || !bindTemp(cur_, n.name, &idx, n.span)) {
        return;
      }
      emitU8(asStmt ? Op::PopStoreTemp : Op::StoreTemp, idx);
      return;
    }
    fail(n.span, "cannot assign");
  }

  bool hasReceiverChild(const Ast& send) const {
    return send.kids.size() == static_cast<std::size_t>(send.argc) + 1;
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
    std::uint8_t idx = 0;
    if (bindTemp(cur_, n.name, &idx, n.span)) {
      emitU8(Op::PushTemp, idx);
      return;
    }
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
    if (env_.undeclaredAreTemps && !isKnownGlobal(n.name)) {
      if (!addWorkspaceTemp(n.name, n.span) || !bindTemp(cur_, n.name, &idx, n.span)) {
        return;
      }
      emitU8(Op::PushTemp, idx);
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

  void compileBlock(const Ast& blk) {
    if (blk.params.size() > 255 || blk.temps.size() > 255 ||
        blk.params.size() + blk.temps.size() > 255) {
      fail(blk.span, "too many block arguments or temporaries");
      return;
    }
    MethodImage inner;
    inner.selector = "";
    inner.numArgs = static_cast<std::uint8_t>(blk.params.size());
    Scope innerScope;
    innerScope.parent = cur_;
    innerScope.isBlock = true;
    innerScope.args = blk.params;
    innerScope.temps = blk.temps;
    Emitter innerEm(inner, env_, error_);
    innerEm.bindingsImage_ = bindingsImage_;
    innerEm.cur_ = &innerScope;
    innerEm.failed_ = failed_;
    if (blk.kids.empty()) {
      innerEm.emit(Op::PushNil);
      innerEm.emit(Op::ReturnTop);
    } else {
      innerEm.compileBlockBody(blk.kids[0]);
    }
    failed_ = innerEm.failed_;
    if (failed_) {
      return;
    }
    const std::size_t ntemps = innerScope.args.size() + innerScope.temps.size() + innerScope.copied.size();
    if (ntemps > 255) {
      fail(blk.span, "too many temporaries");
      return;
    }
    inner.numTemps = static_cast<std::uint8_t>(ntemps);
    for (const std::string& name : innerScope.copied) {
      std::uint8_t idx = 0;
      if (!bindTemp(cur_, name, &idx, blk.span)) {
        fail(blk.span, "cannot copy outer temporary");
        return;
      }
      emitU8(Op::PushTemp, idx);
    }
    if (innerScope.copied.size() > 255) {
      fail(blk.span, "too many copied values");
      return;
    }
    Literal lit;
    lit.kind = LitKind::Method;
    lit.method = std::make_unique<MethodImage>(std::move(inner));
    const std::uint8_t li = intern(std::move(lit), blk.span);
    emitU8U8(Op::CreateBlock, li, static_cast<std::uint8_t>(innerScope.copied.size()));
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
  Emitter em(out.image, env, out.error);
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

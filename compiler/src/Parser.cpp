#include "ao/Parser.hpp"

#include "ao/Scanner.hpp"

#include <algorithm>
#include <utility>
#include <vector>

namespace ao::compiler {
namespace {

SourceSpan join(SourceSpan a, SourceSpan b) {
  SourceSpan s;
  s.start = a.start;
  s.end = b.end;
  return s;
}

class Parser {
 public:
  explicit Parser(std::string_view src) : scanner_(src), src_(src) { advance(); }

  ParseResult run() {
    ParseResult r;
    r.method = parseMethodAst();
    if (!hadError_ && cur_.kind != Tok::Eof) {
      fail("unexpected token");
    }
    if (hadError_) {
      r.ok = false;
      r.error = error_;
      return r;
    }
    r.ok = true;
    return r;
  }

 private:
  Scanner scanner_;
  std::string_view src_;
  Token cur_;
  Token prev_;
  bool hadError_ = false;
  CompileError error_;
  // SPEC §3.8: the deepest nesting allowed, and the parentheses, blocks, literal arrays and
  // assignments open around the current token (the levels the parser itself recurses on).
  static constexpr std::uint32_t kMaxNesting = 256;
  std::uint32_t depth_ = 0;

  void advance() {
    prev_ = cur_;
    cur_ = scanner_.next();
    if (cur_.kind == Tok::Error) {
      fail(cur_.text.empty() ? "invalid token" : cur_.text.c_str());
    }
  }

  bool check(Tok k) const { return cur_.kind == k; }

  bool match(Tok k) {
    if (!check(k)) {
      return false;
    }
    advance();
    return true;
  }

  bool checkBinary(std::string_view text) const {
    return cur_.kind == Tok::Binary && cur_.text == text;
  }

  bool isPipe() const { return checkBinary("|"); }

  bool isEmptyTemps() const { return checkBinary("||"); }

  SourceSpan errorSpan() const {
    if (cur_.span.end > cur_.span.start) {
      return cur_.span;
    }
    if (prev_.span.end > prev_.span.start) {
      return prev_.span;
    }
    SourceSpan s;
    s.start = 0;
    s.end = src_.empty() ? 1 : static_cast<std::uint32_t>(src_.size());
    return s;
  }

  void fail(const char* message) { failAt(errorSpan(), message); }

  void failAt(SourceSpan span, const char* message) {
    if (hadError_) {
      return;
    }
    hadError_ = true;
    error_.span = span;
    error_.message = message;
  }

  // Opens a level that the parser recurses into, at the token `at`; past the limit it is the
  // error there instead. The caller leaves it once the level is parsed.
  bool enter(SourceSpan at) {
    if (depth_ >= kMaxNesting) {
      failAt(at, "nesting too deep");
      return false;
    }
    ++depth_;
    return true;
  }

  void leave() { --depth_; }

  // n holds `levels` nesting levels; past the limit it is the error at `at`. A message chain is
  // read in a loop, so this check, not enter, bounds how deep its receiver side goes.
  void nest(Ast& n, std::uint32_t levels, SourceSpan at) {
    if (levels > kMaxNesting) {
      failAt(at, "nesting too deep");
    }
    n.nesting = static_cast<std::uint16_t>(levels);
  }

  static std::uint32_t deepest(const std::vector<Ast>& nodes) {
    std::uint32_t levels = 0;
    for (const Ast& n : nodes) {
      levels = std::max<std::uint32_t>(levels, n.nesting);
    }
    return levels;
  }

  Ast make(Ast::Kind kind, SourceSpan span) const {
    Ast n;
    n.kind = kind;
    n.span = span;
    return n;
  }

  // SPEC §3.8: declares the current identifier into `names`, the arguments or the temps of
  // `scope` (a method or a block). A pseudo-variable, or a name the scope already declares, is
  // the error at the declaration.
  void declare(Ast& scope, std::vector<std::string>& names) {
    const std::string& name = cur_.text;
    if (name == "self" || name == "super" || name == "thisContext" || name == "nil" ||
        name == "true" || name == "false") {
      fail(("cannot declare pseudo-variable: " + name).c_str());
      return;
    }
    const auto in = [&](const std::vector<std::string>& declared) {
      return std::find(declared.begin(), declared.end(), name) != declared.end();
    };
    if (in(scope.params) || in(scope.temps)) {
      fail(("duplicate name: " + name).c_str());
      return;
    }
    names.push_back(name);
  }

  Ast parseMethodAst() {
    Ast method = make(Ast::Kind::Method, SourceSpan{0, static_cast<std::uint32_t>(src_.size())});
    parseMessagePattern(method);
    parseTemps(method);
    if (checkBinary("<")) {
      method.kids.push_back(parsePrimitive());
    }
    Ast body = parseStatements();
    method.kids.push_back(std::move(body));
    return method;
  }

  void parseMessagePattern(Ast& method) {
    if (hadError_) {
      return;
    }
    if (check(Tok::Ident)) {
      method.name = cur_.text;
      method.span = cur_.span;
      advance();
      return;
    }
    if (check(Tok::Binary)) {
      method.name = cur_.text;
      method.span = cur_.span;
      advance();
      if (!check(Tok::Ident)) {
        fail("expected parameter name");
        return;
      }
      declare(method, method.params);
      method.span = join(method.span, cur_.span);
      advance();
      return;
    }
    if (check(Tok::Keyword)) {
      SourceSpan start = cur_.span;
      while (check(Tok::Keyword)) {
        method.name += cur_.text;
        advance();
        if (!check(Tok::Ident)) {
          fail("expected parameter name");
          return;
        }
        declare(method, method.params);
        method.span = join(start, cur_.span);
        advance();
      }
      return;
    }
    fail("expected message pattern");
  }

  // An optional temp declaration `| t u |` (or the empty `||`) of a method or a block.
  void parseTemps(Ast& node) {
    if (hadError_) {
      return;
    }
    if (isEmptyTemps()) {
      advance();
      return;
    }
    if (!isPipe()) {
      return;
    }
    advance();
    parseTempNames(node);
  }

  // The names of a temp declaration after its opening `|`, and the closing `|`.
  void parseTempNames(Ast& node) {
    while (check(Tok::Ident)) {
      declare(node, node.temps);
      advance();
    }
    if (!isPipe()) {
      fail("expected '|'");
      return;
    }
    advance();
  }

  Ast parsePrimitive() {
    SourceSpan start = cur_.span;
    advance();
    if (!(check(Tok::Keyword) && cur_.text == "primitive:")) {
      fail("expected primitive:");
      return {};
    }
    advance();
    if (!check(Tok::Number) || cur_.isFloat || !cur_.largeInt.empty()) {
      fail("expected primitive number");
      return {};
    }
    Ast prim = make(Ast::Kind::Primitive, start);
    prim.intValue = cur_.intValue;
    advance();
    if (!checkBinary(">")) {
      fail("expected '>'");
      return prim;
    }
    prim.span = join(start, cur_.span);
    advance();
    return prim;
  }

  std::vector<Ast> collectStatements() {
    std::vector<Ast> stmts;
    while (!hadError_ && !check(Tok::Eof) && !check(Tok::RParen) && !check(Tok::RBracket)) {
      if (match(Tok::Period)) {
        continue;
      }
      if (check(Tok::Caret)) {
        SourceSpan caret = cur_.span;
        advance();
        Ast expr = parseExpression();
        Ast ret = make(Ast::Kind::Return, join(caret, expr.span));
        ret.nesting = expr.nesting;
        ret.kids.push_back(std::move(expr));
        stmts.push_back(std::move(ret));
        match(Tok::Period);
        break;
      }
      stmts.push_back(parseExpression());
      if (!match(Tok::Period)) {
        break;
      }
    }
    return stmts;
  }

  Ast parseStatements() {
    std::vector<Ast> stmts = collectStatements();
    if (stmts.empty()) {
      return make(Ast::Kind::Sequence, methodSpanFallback());
    }
    if (stmts.size() == 1) {
      return std::move(stmts[0]);
    }
    Ast seq = make(Ast::Kind::Sequence, join(stmts.front().span, stmts.back().span));
    seq.kids = std::move(stmts);
    return seq;
  }

  Ast parseStatementsAsSequence() {
    std::vector<Ast> stmts = collectStatements();
    SourceSpan sp = cur_.span;
    if (!stmts.empty()) {
      sp = join(stmts.front().span, stmts.back().span);
    }
    Ast seq = make(Ast::Kind::Sequence, sp);
    seq.nesting = static_cast<std::uint16_t>(deepest(stmts));
    seq.kids = std::move(stmts);
    return seq;
  }

  SourceSpan methodSpanFallback() const {
    SourceSpan s;
    s.start = 0;
    s.end = static_cast<std::uint32_t>(src_.size());
    return s;
  }

  Ast parseExpression() {
    if (hadError_) {
      return {};
    }
    if (check(Tok::Ident)) {
      Token id = cur_;
      advance();
      if (check(Tok::Assign)) {
        if (!enter(id.span)) {
          return {};
        }
        advance();
        Ast rhs = parseExpression();
        leave();
        Ast as = make(Ast::Kind::Assign, join(id.span, rhs.span));
        as.name = std::move(id.text);
        nest(as, rhs.nesting + 1u, id.span);
        as.kids.push_back(std::move(rhs));
        return as;
      }
      return parseKeyword(makeVariable(id));
    }
    return parseKeyword(parsePrimary());
  }

  Ast makeVariable(const Token& id) const {
    Ast v = make(Ast::Kind::Variable, id.span);
    v.name = id.text;
    return v;
  }

  // `at` is the selector (its first keyword), where a chain too deep is reported.
  Ast makeSend(Ast recv, std::string selector, std::vector<Ast> args, SourceSpan at,
               SourceSpan end) {
    Ast s = make(Ast::Kind::Send, join(recv.span, end));
    s.name = std::move(selector);
    s.argc = static_cast<std::uint8_t>(args.size());
    s.isSuper = recv.kind == Ast::Kind::Variable && recv.name == "super";
    // SPEC §3.8: the receiver side of a message chain is one level deeper per message.
    const std::uint32_t recvLevels = recv.nesting + (recv.kind == Ast::Kind::Send ? 1u : 0u);
    nest(s, std::max(recvLevels, deepest(args)), at);
    s.kids.push_back(std::move(recv));
    for (Ast& a : args) {
      s.kids.push_back(std::move(a));
    }
    return s;
  }

  Ast makeBareSend(std::string selector, std::vector<Ast> args, SourceSpan span) {
    Ast s = make(Ast::Kind::Send, span);
    s.name = std::move(selector);
    s.argc = static_cast<std::uint8_t>(args.size());
    s.nesting = static_cast<std::uint16_t>(deepest(args));
    s.kids = std::move(args);
    return s;
  }

  // A binary message's argument list. Not `{std::move(arg)}`: the elements of an initializer list
  // are const, so that would copy arg's whole subtree.
  static std::vector<Ast> single(Ast arg) {
    std::vector<Ast> args;
    args.push_back(std::move(arg));
    return args;
  }

  Ast parseKeyword(Ast recv) { return parseCascade(parseKeywordMessage(std::move(recv))); }

  // The unary → binary → keyword chain on recv.
  Ast parseKeywordMessage(Ast recv) {
    recv = parseBinary(std::move(recv));
    if (check(Tok::Keyword)) {
      std::string selector;
      std::vector<Ast> args;
      const SourceSpan at = cur_.span;
      SourceSpan end = recv.span;
      while (check(Tok::Keyword)) {
        selector += cur_.text;
        advance();
        Ast arg = parseBinary(parsePrimaryFromStart());
        end = arg.span;
        args.push_back(std::move(arg));
      }
      recv = makeSend(std::move(recv), std::move(selector), std::move(args), at, end);
    }
    return recv;
  }

  Ast parseCascade(Ast recv) {
    if (!check(Tok::Semicolon)) {
      return recv;
    }
    if (recv.kind != Ast::Kind::Send) {
      fail("cascade requires a message");
      return recv;
    }
    // SPEC §3.8: each part is a unary → binary → keyword chain. Its first message goes to the
    // cascade receiver (a super send when that is super), the next ones to the previous result.
    const bool toSuper = recv.isSuper;
    Ast casc = make(Ast::Kind::Cascade, recv.span);
    casc.kids.push_back(std::move(recv));
    while (match(Tok::Semicolon)) {
      Ast first = parseCascadeMessage();
      first.isSuper = toSuper;
      Ast extra = parseKeywordMessage(std::move(first));
      casc.span = join(casc.span, extra.span);
      casc.kids.push_back(std::move(extra));
    }
    casc.nesting = static_cast<std::uint16_t>(deepest(casc.kids));
    return casc;
  }

  Ast parseCascadeMessage() {
    if (check(Tok::Ident)) {
      Token sel = cur_;
      advance();
      return makeBareSend(sel.text, {}, sel.span);
    }
    if (check(Tok::Binary)) {
      Token sel = cur_;
      advance();
      Ast arg = parseUnary(parsePrimaryFromStart());
      const SourceSpan end = arg.span;
      return makeBareSend(sel.text, single(std::move(arg)), join(sel.span, end));
    }
    if (check(Tok::Keyword)) {
      std::string selector;
      std::vector<Ast> args;
      SourceSpan start = cur_.span;
      SourceSpan end = start;
      while (check(Tok::Keyword)) {
        selector += cur_.text;
        advance();
        Ast arg = parseBinary(parsePrimaryFromStart());
        end = arg.span;
        args.push_back(std::move(arg));
      }
      return makeBareSend(std::move(selector), std::move(args), join(start, end));
    }
    fail("expected cascade message");
    return {};
  }

  Ast parseBinary(Ast recv) {
    recv = parseUnary(std::move(recv));
    while (!hadError_ && check(Tok::Binary)) {
      Token sel = cur_;
      advance();
      Ast arg = parseUnary(parsePrimaryFromStart());
      const SourceSpan end = arg.span;
      recv = makeSend(std::move(recv), sel.text, single(std::move(arg)), sel.span, end);
    }
    return recv;
  }

  Ast parseUnary(Ast recv) {
    while (!hadError_ && check(Tok::Ident)) {
      Token sel = cur_;
      advance();
      recv = makeSend(std::move(recv), sel.text, {}, sel.span, sel.span);
    }
    return recv;
  }

  Ast parsePrimaryFromStart() {
    if (hadError_) {
      return {};
    }
    if (check(Tok::Ident)) {
      Token id = cur_;
      advance();
      return makeVariable(id);
    }
    return parsePrimary();
  }

  Ast numberLiteral(SourceSpan span, const Token& num, int sign) {
    Ast lit = make(Ast::Kind::Literal, span);
    lit.isFloat = num.isFloat;
    lit.text = num.text;
    if (num.isFloat) {
      lit.floatValue = num.number * static_cast<double>(sign);
      return lit;
    }
    lit.intValue = sign < 0 ? -num.intValue : num.intValue;
    if (!num.largeInt.empty()) {
      lit.largeInt = (sign < 0 ? "-" : "") + num.largeInt;
    }
    return lit;
  }

  Ast stringLiteral() {
    Ast lit = make(Ast::Kind::Literal, cur_.span);
    lit.name = "'";
    lit.text = cur_.text;
    advance();
    return lit;
  }

  Ast symbolLiteral() {
    Ast lit = make(Ast::Kind::Literal, cur_.span);
    lit.name = "#";
    lit.text = cur_.text;
    advance();
    return lit;
  }

  Ast characterLiteral() {
    Ast lit = make(Ast::Kind::Literal, cur_.span);
    lit.name = "$";
    lit.text = cur_.text;
    advance();
    return lit;
  }

  Ast parseBlock() {
    SourceSpan start = cur_.span;
    advance();
    Ast blk = make(Ast::Kind::Block, start);
    while (check(Tok::Colon)) {
      advance();
      if (!check(Tok::Ident)) {
        fail("expected block parameter");
        return blk;
      }
      declare(blk, blk.params);
      advance();
    }
    // SPEC §3.8: after the arguments the `|` is required; `||` is that `|` and the opening of the
    // temp declaration.
    if (blk.params.empty()) {
      parseTemps(blk);
    } else if (isEmptyTemps()) {
      advance();
      parseTempNames(blk);
    } else if (isPipe()) {
      advance();
      parseTemps(blk);
    } else {
      fail("expected '|'");
      return blk;
    }
    if (!enter(start)) {
      return blk;
    }
    Ast body = parseStatementsAsSequence();
    leave();
    if (!check(Tok::RBracket)) {
      fail("expected ']'");
      return blk;
    }
    blk.span = join(start, cur_.span);
    nest(blk, body.nesting + 1u, start);
    advance();
    blk.kids.push_back(std::move(body));
    return blk;
  }

  Ast finishLiteralArray(SourceSpan start, Tok closer, bool bytes) {
    Ast arr = make(Ast::Kind::Literal, start);
    arr.name = bytes ? "#[" : "#(";
    if (!enter(start)) {
      return arr;
    }
    while (!hadError_ && !check(Tok::Eof) && !check(closer)) {
      if (bytes) {
        arr.kids.push_back(parseByteElement());
      } else {
        arr.kids.push_back(parseArrayElement());
      }
    }
    leave();
    if (!check(closer)) {
      fail(bytes ? "expected ']'" : "expected ')'");
      return arr;
    }
    arr.span = join(start, cur_.span);
    nest(arr, deepest(arr.kids) + 1u, start);
    advance();
    return arr;
  }

  Ast parseByteElement() {
    if (!check(Tok::Number) || cur_.isFloat) {
      fail("expected byte 0-255");
      return {};
    }
    if (cur_.intValue < 0 || cur_.intValue > 255 || !cur_.largeInt.empty()) {
      fail("expected byte 0-255");
      return {};
    }
    Token n = cur_;
    advance();
    return numberLiteral(n.span, n, 1);
  }

  Ast parseArrayElement() {
    if (check(Tok::Number)) {
      Token n = cur_;
      advance();
      return numberLiteral(n.span, n, 1);
    }
    if (check(Tok::String)) {
      return stringLiteral();
    }
    if (check(Tok::Symbol)) {
      return symbolLiteral();
    }
    if (check(Tok::Character)) {
      return characterLiteral();
    }
    if (check(Tok::Ident)) {
      Ast lit = make(Ast::Kind::Literal, cur_.span);
      if (cur_.text == "nil" || cur_.text == "true" || cur_.text == "false") {
        lit.name = cur_.text;
      } else {
        lit.name = "#";
        lit.text = cur_.text;
      }
      advance();
      return lit;
    }
    if (check(Tok::Keyword)) {
      Ast lit = make(Ast::Kind::Literal, cur_.span);
      lit.name = "#";
      lit.text = cur_.text;
      advance();
      return lit;
    }
    if (checkBinary("-")) {
      Token minus = cur_;
      advance();
      if (check(Tok::Number)) {
        Token n = cur_;
        advance();
        return numberLiteral(join(minus.span, n.span), n, -1);
      }
      Ast lit = make(Ast::Kind::Literal, minus.span);
      lit.name = "#";
      lit.text = "-";
      return lit;
    }
    if (check(Tok::Binary)) {
      Ast lit = make(Ast::Kind::Literal, cur_.span);
      lit.name = "#";
      lit.text = cur_.text;
      advance();
      return lit;
    }
    if (check(Tok::LParen) || check(Tok::HashLParen)) {
      SourceSpan start = cur_.span;
      advance();
      return finishLiteralArray(start, Tok::RParen, false);
    }
    if (check(Tok::HashLBracket)) {
      SourceSpan start = cur_.span;
      advance();
      return finishLiteralArray(start, Tok::RBracket, true);
    }
    fail("expected array element");
    return {};
  }

  Ast parsePrimary() {
    if (hadError_) {
      return {};
    }
    if (check(Tok::Number)) {
      Token n = cur_;
      advance();
      return numberLiteral(n.span, n, 1);
    }
    if (check(Tok::String)) {
      return stringLiteral();
    }
    if (check(Tok::Symbol)) {
      return symbolLiteral();
    }
    if (check(Tok::Character)) {
      return characterLiteral();
    }
    if (checkBinary("-")) {
      Token minus = cur_;
      advance();
      if (!check(Tok::Number)) {
        fail("expected number");
        return {};
      }
      Token n = cur_;
      advance();
      return numberLiteral(join(minus.span, n.span), n, -1);
    }
    if (check(Tok::LParen)) {
      SourceSpan start = cur_.span;
      if (!enter(start)) {
        return {};
      }
      advance();
      Ast inner = parseExpression();
      leave();
      if (!check(Tok::RParen)) {
        fail("expected ')'");
        return inner;
      }
      inner.span = join(start, cur_.span);
      nest(inner, inner.nesting + 1u, start);
      advance();
      return inner;
    }
    if (check(Tok::LBracket)) {
      return parseBlock();
    }
    if (check(Tok::HashLParen)) {
      SourceSpan start = cur_.span;
      advance();
      return finishLiteralArray(start, Tok::RParen, false);
    }
    if (check(Tok::HashLBracket)) {
      SourceSpan start = cur_.span;
      advance();
      return finishLiteralArray(start, Tok::RBracket, true);
    }
    fail("expected expression");
    return {};
  }
};

}  // namespace

ParseResult parseMethod(std::string_view source) {
  Parser p(source);
  return p.run();
}

}  // namespace ao::compiler

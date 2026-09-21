#include "ao/Parser.hpp"

#include "ao/Scanner.hpp"

#include <deque>
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
  std::deque<Token> queued_;
  bool hadError_ = false;
  CompileError error_;

  void advance() {
    prev_ = cur_;
    if (!queued_.empty()) {
      cur_ = queued_.front();
      queued_.pop_front();
    } else {
      cur_ = scanner_.next();
    }
    if (cur_.kind == Tok::Error) {
      fail("invalid token");
    }
  }

  void replay(const std::vector<Token>& toks) {
    if (toks.empty()) {
      return;
    }
    queued_.push_front(cur_);
    for (std::size_t i = toks.size(); i-- > 1;) {
      queued_.push_front(toks[i]);
    }
    cur_ = toks[0];
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

  void fail(const char* message) {
    if (hadError_) {
      return;
    }
    hadError_ = true;
    error_.span = errorSpan();
    error_.message = message;
  }

  Ast make(Ast::Kind kind, SourceSpan span) const {
    Ast n;
    n.kind = kind;
    n.span = span;
    return n;
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
      method.params.push_back(cur_.text);
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
        method.params.push_back(cur_.text);
        method.span = join(start, cur_.span);
        advance();
      }
      return;
    }
    fail("expected message pattern");
  }

  void parseTemps(Ast& method) {
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
    while (check(Tok::Ident)) {
      method.temps.push_back(cur_.text);
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
    if (!check(Tok::Number) || cur_.isFloat) {
      fail("expected primitive number");
      return {};
    }
    Ast prim = make(Ast::Kind::Primitive, start);
    prim.intValue = static_cast<std::int64_t>(cur_.number);
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
        advance();
        Ast rhs = parseExpression();
        Ast as = make(Ast::Kind::Assign, join(id.span, rhs.span));
        as.name = std::move(id.text);
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

  Ast makeSend(Ast recv, std::string selector, std::vector<Ast> args, SourceSpan end) {
    Ast s = make(Ast::Kind::Send, join(recv.span, end));
    s.name = std::move(selector);
    s.argc = static_cast<std::uint8_t>(args.size());
    s.isSuper = recv.kind == Ast::Kind::Variable && recv.name == "super";
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
    s.kids = std::move(args);
    return s;
  }

  Ast parseKeyword(Ast recv) {
    recv = parseBinary(std::move(recv));
    if (check(Tok::Keyword)) {
      std::string selector;
      std::vector<Ast> args;
      SourceSpan end = recv.span;
      while (check(Tok::Keyword)) {
        selector += cur_.text;
        advance();
        Ast arg = parseBinary(parsePrimaryFromStart());
        end = arg.span;
        args.push_back(std::move(arg));
      }
      recv = makeSend(std::move(recv), std::move(selector), std::move(args), end);
    }
    return parseCascade(std::move(recv));
  }

  Ast parseCascade(Ast recv) {
    if (!check(Tok::Semicolon)) {
      return recv;
    }
    if (recv.kind != Ast::Kind::Send) {
      fail("cascade requires a message");
      return recv;
    }
    Ast casc = make(Ast::Kind::Cascade, recv.span);
    casc.kids.push_back(std::move(recv));
    while (match(Tok::Semicolon)) {
      Ast extra = parseCascadeMessage();
      casc.span = join(casc.span, extra.span);
      casc.kids.push_back(std::move(extra));
    }
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
      return makeBareSend(sel.text, {std::move(arg)}, join(sel.span, end));
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
      recv = makeSend(std::move(recv), sel.text, {std::move(arg)}, arg.span);
    }
    return recv;
  }

  Ast parseUnary(Ast recv) {
    while (!hadError_ && check(Tok::Ident)) {
      Token sel = cur_;
      advance();
      recv = makeSend(std::move(recv), sel.text, {}, sel.span);
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
    const double v = num.number * static_cast<double>(sign);
    lit.floatValue = v;
    if (!num.isFloat) {
      lit.intValue = static_cast<std::int64_t>(v);
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
      blk.params.push_back(cur_.text);
      advance();
    }
    if (isEmptyTemps()) {
      advance();
    } else if (isPipe()) {
      advance();
      if (isPipe()) {
        advance();
      } else {
        std::vector<Token> ids;
        while (check(Tok::Ident)) {
          ids.push_back(cur_);
          advance();
        }
        if (isPipe()) {
          for (const Token& id : ids) {
            blk.temps.push_back(id.text);
          }
          advance();
        } else {
          replay(ids);
        }
      }
    }
    Ast body = parseStatementsAsSequence();
    if (!check(Tok::RBracket)) {
      fail("expected ']'");
      return blk;
    }
    blk.span = join(start, cur_.span);
    advance();
    blk.kids.push_back(std::move(body));
    return blk;
  }

  Ast finishLiteralArray(SourceSpan start, Tok closer, bool bytes) {
    Ast arr = make(Ast::Kind::Literal, start);
    arr.name = bytes ? "#[" : "#(";
    while (!hadError_ && !check(Tok::Eof) && !check(closer)) {
      if (bytes) {
        arr.kids.push_back(parseByteElement());
      } else {
        arr.kids.push_back(parseArrayElement());
      }
    }
    if (!check(closer)) {
      fail(bytes ? "expected ']'" : "expected ')'");
      return arr;
    }
    arr.span = join(start, cur_.span);
    advance();
    return arr;
  }

  Ast parseByteElement() {
    if (!check(Tok::Number) || cur_.isFloat) {
      fail("expected byte 0-255");
      return {};
    }
    const auto v = static_cast<std::int64_t>(cur_.number);
    if (v < 0 || v > 255 || cur_.number != static_cast<double>(v)) {
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
      lit.name = "#";
      lit.text = cur_.text;
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
      advance();
      Ast inner = parseExpression();
      if (!check(Tok::RParen)) {
        fail("expected ')'");
        return inner;
      }
      inner.span = join(start, cur_.span);
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

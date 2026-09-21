#include "ao/Parser.hpp"

#include "ao/Scanner.hpp"

#include <utility>

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

  void advance() {
    prev_ = cur_;
    cur_ = scanner_.next();
    if (cur_.kind == Tok::Error) {
      fail("invalid token");
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

  bool isPipe() const { return cur_.kind == Tok::Binary && cur_.text == "|"; }

  bool isEmptyTemps() const { return cur_.kind == Tok::Binary && cur_.text == "||"; }

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

  Ast parseStatements() {
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

  Ast parseKeyword(Ast recv) {
    recv = parseBinary(std::move(recv));
    if (!check(Tok::Keyword)) {
      return recv;
    }
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
    return makeSend(std::move(recv), std::move(selector), std::move(args), end);
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
      Ast lit = make(Ast::Kind::Literal, cur_.span);
      lit.name = "'";
      lit.text = cur_.text;
      advance();
      return lit;
    }
    if (check(Tok::Symbol)) {
      Ast lit = make(Ast::Kind::Literal, cur_.span);
      lit.name = "#";
      lit.text = cur_.text;
      advance();
      return lit;
    }
    if (check(Tok::Character)) {
      Ast lit = make(Ast::Kind::Literal, cur_.span);
      lit.name = "$";
      lit.text = cur_.text;
      advance();
      return lit;
    }
    if (check(Tok::Binary) && cur_.text == "-") {
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
      fail("blocks not implemented");
      return {};
    }
    if (check(Tok::HashLParen) || check(Tok::HashLBracket)) {
      fail("literal arrays not implemented");
      return {};
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

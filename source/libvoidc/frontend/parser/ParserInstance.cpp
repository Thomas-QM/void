struct ParserInstance {
  Source& source;
  Ast& ast;
  Vector<Token> const& tokens;
  size_t index = 0;

  static ParserInstance create(Source& src, Ast& ast, Vector<Token> const& tokens) { return { src, ast, tokens }; }

  template<typename... Args>
  void error(Token const& token, char const* fmt, Args const&... args) { source.error(token.begin, token.end, fmt, args...); }

  bool is_infix() { 
    return valid() 
      && is_operator()
      && ::voidc::is_infix(current().data.op);
  }

  bool is_prefix() {
    return valid() 
      && is_operator()
      && ::voidc::is_prefix(current().data.op);
  }

  bool is_seperator() {
    return valid() 
      && is_operator()
      && ::voidc::is_seperator(current().data.op);
  }

  bool match(OperatorKind kind) { 
    return valid() 
      && is_operator()
      && current().data.op == kind; 
  }

  bool          is_operator()           { return current().kind == TokenKind::op; }
  bool          match(TokenKind kind)   { return valid() && current().kind == kind; }
  Token const&  consume()               { advance(); return previous(); }
  Token const&  previous()              { return tokens[index == 0 ? 0 : index - 1]; }
  Token const&  last()                  { return tokens.last(); }
  bool          valid()                 { return index < tokens.len; } 
  Token const&  current()               { return tokens[index]; }
  void          advance(size_t cnt = 1) { index += cnt; }
};
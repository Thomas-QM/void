struct LexerInstance {
  static constexpr size_t TOKENS_DEFAULT_CAPACITY = 1024;

  Source& src;
  Vector<Token> tokens;
  char const* it;
  char const* end;
  char const* mark_it;

  static LexerInstance  create(Source& src)     { return { src, Vector<Token>::create(TOKENS_DEFAULT_CAPACITY), src.file, src.file + src.file_len, nullptr }; }
  
  template<typename... Args>
  void error(char const* begin, char const* end, char const* fmt, Args const&... args) { src.error(begin, end, fmt, args...); }

  void                  add(Token const& token) { tokens.add(token); }
  size_t                char_size()             { return valid() ? ::voidc::char_size(it) : 0; }
  bool                  valid()                 { return it < end; } 
  void                  mark()                  { mark_it = it; }
  size_t                mark_distance()         { return distance(mark_it, it); }
  char                  current()               { return valid() ? *it : '\0'; }
  char32_t              u32current()            { return valid() ? to_utf32_char(it) : U'\0'; }
  char                  next()                  { return it + ::voidc::char_size(it) < end ? *(it + ::voidc::char_size(it)) : '\0'; }
  void                  advance(size_t cnt = 1) { utf8_incr(it, cnt); }
  bool                  is_whitespace()         { return valid() ? ::voidc::is_whitespace(*it) : false; }
  bool                  is_alphabetical()       { return valid() ? ::voidc::is_alphabetical(*it) : false; }
  bool                  is_numeric()            { return valid() ? ::voidc::is_numeric(*it) : false; }
  bool                  is_next_numeric()       { return it + ::voidc::char_size(it) < end ? ::voidc::is_numeric(*(it + ::voidc::char_size(it))) : false; }
  bool                  is_alphanumeric()       { return valid() ? ::voidc::is_alphanumeric(*it) : false; }
  bool                  is_newline()            { return valid() ? ::voidc::is_newline(*it) : false; }
  bool                  is_hexadecimal()        { return valid() ? ::voidc::is_hexadecimal(*it) : false; }
  bool                  is_binary()             { return valid() ? ::voidc::is_binary(*it) : false; }
};
Vector<Token> lex(Source& src) {
  LexerInstance instance = LexerInstance::create(src);

  VOID_LEX_REITERATE:
  while(instance.valid()) {
    char const* old = instance.it;

    for(auto const& lexlet : LEXLETS) { 
      LexletResult res = lexlet(instance);

      if(res == LexletResult::reiterate) { goto VOID_LEX_REITERATE; } 
    }
    if(old == instance.it) {
      instance.error(old, old + char_size(old), "Invalid character");
      instance.advance();
    }
  }
  return instance.tokens;
}
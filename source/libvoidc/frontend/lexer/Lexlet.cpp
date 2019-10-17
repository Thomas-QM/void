enum struct LexletResult {
  none = -1,
  reiterate
};

using Lexlet = LexletResult(*)(LexerInstance& instance);

void parse_escape_sequence(LexerInstance& instance, String& str, char term) {
  if(instance.current() == '\\') {
    char const* start = instance.it;

    instance.advance();
    switch(instance.current()) {
      case 'a':
      case 'A':
        str.add('\a');
        instance.advance();
      break;
      case 'b':
      case 'B':
        str.add('\b');
        instance.advance();
      break;
      case 'f':
      case 'F':
        str.add('\f');
        instance.advance();
      break;
      case 'n':
      case 'N':
        str.add('\n');
        instance.advance();
      break;
      case 'r':
      case 'R':
        str.add('\r');
        instance.advance();
      break;
      case 't':
      case 'T':
        str.add('\t');
        instance.advance();
      break;
      case 'v':
      case 'V':
        str.add('\v');
        instance.advance();
      break;
      case '\\':
        str.add('\\');
        instance.advance();
      break;
      case '\'':
        str.add('\'');
        instance.advance();
      break;
      case '\"':
        str.add('\"');
        instance.advance();
      break;
      case '\?':
        str.add('\?');
        instance.advance();
      break;
      case '0':
        str.add('\0');
        instance.advance();
      break;
      case 'U':
      case 'u':
      {
        size_t len = 0;
        bool invalid_char = false;

        instance.advance();
        for(size_t i = 0; i < 8; ++ i) {
          if(instance.current() == term) { break; }
          if(instance.current() == '\\') { instance.advance(); break; }
          if(!instance.is_hexadecimal()) { invalid_char = true; }
          instance.advance();
          ++ len;
        }
        unsigned long long val = 0;

        if(len == 0) { instance.error(start, instance.it, "Expected hexadecimal numbers"); }
        if(invalid_char) { instance.error(start, instance.it, "Invalid character in hexadecimal literal"); }
        if(!hex_str_to_u64(start + 2, len, val)) {
          instance.error(start, instance.it, "Invalid unicode escape sequence");
        }
        char c_cache[5] {};

        to_utf8_str(val, c_cache);
        str.add(c_cache);
      }
      break;
      default:
        instance.advance();
        instance.error(start, instance.it, "Unknown escape sequence");
      break;
    }
  }
}

LexletResult Lexlet_whitespace(LexerInstance& instance) {
  while(instance.is_whitespace()) { instance.advance(); }
  return LexletResult::none;
}

LexletResult Lexlet_comment(LexerInstance& instance) {
  instance.mark();
  if(instance.current() == '#') {
    while(!instance.is_newline() && instance.valid()) { instance.advance(); }
  }
  return instance.mark_it != instance.it ? LexletResult::reiterate : LexletResult::none;
}

LexletResult Lexlet_identifier_keyword(LexerInstance& instance) {
  if(instance.is_alphabetical()) {
    KeywordKind kwkind = KeywordKind::none;

    instance.mark();
    instance.advance();
    while(instance.is_alphanumeric()) { instance.advance(); }
    for(size_t i = 0; i < VOID_ARRAY_LEN(KEYWORDS); ++ i) {
      ReservedToken const& kw = KEYWORDS[i];

      if(instance.mark_distance() == kw.str_len && memcmp(instance.mark_it, kw.str, kw.str_len) == 0) {
        kwkind = KeywordKind(i);
        break;
      }
    }
    if(kwkind == KeywordKind::none) { instance.add(Token::create(instance.mark_it, instance.it)); } 
    else { instance.add(Token::create(kwkind, instance.mark_it, instance.it)); }
  }
  return LexletResult::none;
}

LexletResult Lexlet_hexadecimal(LexerInstance& instance) {
  if(instance.current() == '0' && (instance.next() == 'x' || instance.next() == 'X')) {
    bool invalid_char = false;

    instance.mark();
    instance.advance(2);
    while(instance.is_alphanumeric()) { 
      if(!instance.is_hexadecimal()) { invalid_char = true; }
      instance.advance(); 
    }
    unsigned long long val = 0;

    if(distance(instance.mark_it + 2, instance.it) == 0) {
      instance.error(instance.mark_it, instance.it, "Expected hexadecimal numbers");
    }
    if(invalid_char) { instance.error(instance.mark_it, instance.it, "Invalid hexadecimal character"); }
    if(!hex_str_to_u64(instance.mark_it, distance(instance.mark_it, instance.it), val)) {
      instance.error(instance.mark_it, instance.it, "Invalid hexadecimal literal");
    }
    instance.add(Token::create(val, instance.mark_it, instance.it));
  }
  return LexletResult::none;
}

LexletResult Lexlet_binary(LexerInstance& instance) {
  if(instance.current() == '0' && (instance.next() == 'b' || instance.next() == 'B')) {
    bool invalid_char = false;

    instance.mark();
    instance.advance(2);
    while(instance.is_alphanumeric()) { 
      if(!instance.is_binary()) { invalid_char = true; }
      instance.advance(); 
    }
    unsigned long long val = 0;

    if(distance(instance.mark_it + 2, instance.it) == 0) {
      instance.error(instance.mark_it, instance.it, "Expected binary numbers");
    }
    if(invalid_char) { instance.error(instance.mark_it, instance.it, "Invalid binary character"); }
    if(!bin_str_to_u64(instance.mark_it + 2, distance(instance.mark_it, instance.it) - 2, val)) {
      instance.error(instance.mark_it, instance.it, "Invalid binary literal");
    }
    instance.add(Token::create(val, instance.mark_it, instance.it));
  }
  return LexletResult::none;
}

LexletResult Lexlet_point_integer(LexerInstance& instance) {
  if(instance.is_numeric() || (instance.current() == '.' && instance.is_next_numeric())) {
    int exponent_cnt = 0;
    bool expect_num = false;
    bool is_point = false;
    
    instance.mark();
    while(instance.is_alphanumeric()) {
      if(instance.current() == 'e' || instance.current() == 'E') {
        is_point = true;
        ++ exponent_cnt;
        instance.advance();
        if(instance.current() == '-' || instance.current() == '+') { instance.advance(); }
        if(!instance.is_numeric()) { expect_num = true; }
        continue;
      }
      instance.advance();
    }
    if(instance.current() == '.' && instance.is_next_numeric()) {
      is_point = true;
      instance.advance();
    }
    while(instance.is_alphanumeric()) {
      if(instance.current() == 'e' || instance.current() == 'E') {
        is_point = true;
        ++ exponent_cnt;
        instance.advance();
        if(instance.current() == '-' || instance.current() == '+') { instance.advance(); }
        if(!instance.is_numeric()) { expect_num = true; }
        continue;
      }
      instance.advance();
    }
    if(exponent_cnt > 1) { instance.error(instance.mark_it, instance.it, "Too many exponentials in floating point literal"); }
    if(expect_num) { instance.error(instance.mark_it, instance.it, "Expected number after exponential"); }
    unsigned long long int_val = 0;
    double point_val = 0;

    if(is_point) {
      if(!str_to_f64(instance.mark_it, instance.mark_distance(), point_val)) {
        instance.error(instance.mark_it, instance.it, "Invalid floating point literal");
      }
      instance.add(Token::create(point_val, instance.mark_it, instance.it));
    } else {
      if(!str_to_u64(instance.mark_it, instance.mark_distance(), int_val)) {
        instance.error(instance.mark_it, instance.it, "Invalid integer literal");
      }
      instance.add(Token::create(int_val, instance.mark_it, instance.it));
    }
  }
  return LexletResult::none;
}

LexletResult Lexlet_operator(LexerInstance& instance) {
  for(size_t i = 0; i < VOID_ARRAY_LEN(OPERATORS); ++ i) {
    ReservedToken const& op = OPERATORS[i];

    if(memcmp(instance.it, op.str, op.str_len) == 0) {
      instance.mark();
      instance.advance(op.str_len);
      instance.add(Token::create(OperatorKind(i), instance.mark_it, instance.it));
      break;
    }
  }
  return LexletResult::none;
}

LexletResult Lexlet_string(LexerInstance& instance) {
  if(instance.current() == '"') {
    String str = String::create();
    bool closed = false;

    instance.mark();
    instance.advance();
    while(instance.valid()) {
      if(instance.current() == '"') {
        instance.advance();
        closed = true;
        break;
      }
      if(instance.current() == '\\') {
        parse_escape_sequence(instance, str, '\"');
        continue;
      }
      str.add(instance.it, instance.char_size()); 
      instance.advance();
    }
    if(!closed) { instance.error(instance.mark_it, instance.it, "Expected closing `\"`"); }
    instance.add(Token::create(str, instance.mark_it, instance.it));
  }
  return LexletResult::none;
}

LexletResult Lexlet_character(LexerInstance& instance) {
  if(instance.current() == '\'') {
    String str = String::create();
    bool closed = false;

    instance.mark();
    instance.advance();
    while(instance.valid()) {
      if(instance.current() == '\'') {
        instance.advance();
        closed = true;
        break;
      }
      if(instance.current() == '\\') {
        parse_escape_sequence(instance, str, '\'');
        continue;
      } 
      str.add(instance.it, instance.char_size());
      instance.advance();
    }
    if(str.len == 0) { instance.error(instance.mark_it, instance.it, "Expected character"); }
    if(char_size(str.data) < str.len) {
      instance.error(instance.mark_it, instance.it, "Too many characters in character literal");
    }
    if(!closed) { instance.error(instance.mark_it, instance.it, "Expected closing `'`"); }
    instance.add(Token::create(to_utf32_char(str.data), instance.mark_it, instance.it));
  }
  return LexletResult::none;
}

static Lexlet const LEXLETS[] = {
  Lexlet_whitespace,
  Lexlet_comment,
  Lexlet_identifier_keyword,
  Lexlet_hexadecimal,
  Lexlet_binary,
  Lexlet_point_integer,
  Lexlet_operator,
  Lexlet_string,
  Lexlet_character
};
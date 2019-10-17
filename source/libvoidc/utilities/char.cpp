constexpr bool is_newline(char c) {
  return c == '\n' || c == '\r';
}

constexpr bool is_whitespace(char c) {
  return c == '\n' || c == '\r' || c == '\t' || c == ' ';
}

constexpr bool is_alphabetical(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

constexpr bool is_numeric(char c) {
  return (c >= '0' && c <= '9');
}

constexpr bool is_alphanumeric(char c) {
  return is_alphabetical(c) || is_numeric(c);
}

constexpr bool is_hexadecimal(char c) {
  return (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || is_numeric(c);
}

constexpr bool is_binary(char c) {
  return c == '0' || c == '1';
}

constexpr size_t char_size(char32_t c) {
  if      (c < 128)     { return 1; } 
  else if (c < 2048)    { return 2; } 
  else if (c < 65536)   { return 3; } 
  else if (c < 1114112) { return 4; }
  return 0;
}

constexpr size_t char_size(char const* it) {
  unsigned char c = *it;

  if      (c < 128) { return 1; } 
  else if (c < 224) { return 2; } 
  else if (c < 240) { return 3; } 
  else              { return 4; }
}

constexpr char32_t to_utf32_char(char const* sch) {
  unsigned char const* ch = (unsigned char const*)sch;
  char32_t o = *ch;

	if      (o < 128) { return o; }
	else if (o < 224) { return ((o & 31) << 6) | (ch[1] & 63); }
	else if (o < 240) {
    return ((o & 15) << 12) 
      | ((ch[1] & 63) << 6) 
      | (ch[2] & 63);
  }
	else {
    return ((o & 7) << 18) 
      | ((ch[1] & 63) << 12) 
      | ((ch[2] & 63) << 6) 
      | (ch[3] & 63);
  }
}

constexpr void to_utf8_str(char32_t c, char* str) {
  size_t c_len = char_size(c);

  if (c_len == 1) { str[0] = c; }
  else if (c_len == 2) {
    str[0] = 192 + (c >> 6);
    str[1] = 128 + (c & 63);
  } 
  else if (c_len == 3) {
    str[0] = 224 + (c >> 12);
    str[1] = 128 + ((c >> 6) & 63);
    str[2] = 128 + (c & 63);
  } 
  else if (c_len == 4) {
    str[0] = 240 + (c >> 18);
    str[1] = 128 + ((c >> 12) & 63);
    str[2] = 128 + ((c >> 6) & 63);
    str[3] = 128 + (c & 63);
  }
}
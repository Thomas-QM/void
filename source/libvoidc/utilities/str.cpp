constexpr char const* utf8_incr(char const*& it, size_t cnt = 1) {
  for(size_t i = 0; i < cnt; ++ i) { it += char_size(it); }
  return it;
}

char* str_clone(const char* str) {
  size_t str_len = strlen(str);
  char* nstr = (char*)malloc(str_len + 1);

  return (char*)memcpy(nstr, str, str_len + 1);
}

constexpr char const* previous_line(char const* str, char const* ptr) {
  char const* last_line = str;
  char const* cur_line = str;
  char const* it = str;

  while(it < ptr) {
    char c = *it;

    if(it + char_size(it) < ptr) {
      char next = *(it + char_size(it));

      if((c == '\r' && next == '\n') || (c == '\n' && next == '\r')) {
        utf8_incr(it, 2);
        last_line = cur_line;
        cur_line = it;
        continue;
      }
    }
    utf8_incr(it);
    if(c == '\n' || c == '\r') {
      last_line = cur_line;
      cur_line = it;
    }
  }
  return last_line;
}

constexpr char const* end_of_line(char const* ptr) {
  char const* it = ptr;

  while(*it != '\0' && *it != '\n' && *it != '\r') { ++ it; }
  return it;
}

constexpr char const* next_line(char const* ptr) {
  char const* it = ptr;

  while(*it != '\0') {
    char c = *it;
    char next = *(it + char_size(it));

    if((c == '\n' && next == '\r') || (c == '\r' && next == '\n')) {
      utf8_incr(it, 2);
      return it;
    }
    if(c == '\n' || c == '\r') {
      utf8_incr(it);
      return it;
    }
    utf8_incr(it);
  }
  return it;
}
size_t num_length(size_t n) {
  size_t length = 0;
  do { ++ length; n /= 10; } while(n);
  return length;
}

bool str_to_f64(char const* str, size_t str_len, double& f) {
  char const* end = str;

  f = strtod(str, (char**)&end);
  return end == str + str_len && errno == 0;
}

bool str_to_u64(char const* str, size_t str_len, unsigned long long& i) {
  char const* end = str;

  i = strtoull(str, (char**)&end, 10);
  return end == str + str_len && errno == 0;
}

bool hex_str_to_u64(char const* str, size_t str_len, unsigned long long& i) {
  char const* end = str;

  i = strtoull(str, (char**)&end, 16);
  return end == str + str_len && errno == 0;
}

bool bin_str_to_u64(char const* str, size_t str_len, unsigned long long& i) {
  char const* end = str;
  
  i = strtoull(str, (char**)&end, 2);
  return end == str + str_len && errno == 0;
}
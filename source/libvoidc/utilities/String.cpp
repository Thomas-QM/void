template<typename T>
struct StringImpl {
  T* data;
  size_t len;
  size_t cap;

  static StringImpl create(size_t cap = 32) {
    StringImpl str { (T*)malloc((sizeof(T) * cap) + 1), 0, cap };

    str.terminate();
    return str;
  }

  void resize(size_t cnt) {
    if(len + cnt > cap) {
      while(len + cnt > cap) { cap *= 2; }
      data = (T*)realloc((void*)data, (sizeof(T) * cap) + 1);
    }
  }

  void add(T const* str, long long str_len = -1) {
    if(str_len == -1) { str_len = strlen(str); }
    resize(str_len);
    memcpy(data + len, str, sizeof(T) * str_len);
    len += str_len;
    terminate();
  }

  void add(T c, size_t cnt = 1) {
    resize(cnt);
    memset(data + len, c, sizeof(T) * cnt);
    len += cnt;
    terminate();
  }

  void     terminate()                 { data[len] = '\0'; }
  T&       at(size_t at)               { return data[at]; }
  T        at(size_t at) const         { return data[at]; }
  T&       operator[](size_t at)       { return data[at]; }
  T        operator[](size_t at) const { return data[at]; }
  void     destroy()                   { free((void*)data); }
};

using String = StringImpl<char>;
using U32String = StringImpl<char32_t>;
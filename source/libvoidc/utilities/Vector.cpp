template<typename T>
struct Vector {
  static constexpr size_t VECTOR_DEFAULT_CAPACITY = 32;

  T* data;
  size_t len;
  size_t cap;

  static Vector create(size_t cap = VECTOR_DEFAULT_CAPACITY) {
    return { (T*)malloc(sizeof(T) * cap), 0, cap };
  }

  void resize(size_t cnt) {
    if(len + cnt > cap) {
      while(len + cnt > cap) { cap *= 2; }
      data = (T*)realloc((void*)data, sizeof(T) * cap);
    }
  } 

  void add(T const& v) {
    resize(1);
    memcpy((void*)(data + len), &v, sizeof(T));
    ++ len;
  }
  
  template<typename... Args>
  void add_many(Args const&... args) {
    resize(sizeof...(args));
    (add(args), ...);
  }

  T&        last()                      { return data[len == 0 ? 0 : len - 1]; }
  T const&  last() const                { return data[len == 0 ? 0 : len - 1]; }
  T&        at(size_t at)               { return data[at]; }
  T const&  at(size_t at) const         { return data[at]; }
  T&        operator[](size_t at)       { return data[at]; }
  T const&  operator[](size_t at) const { return data[at]; }
  void      destroy()                   { free((void*)data); }
};
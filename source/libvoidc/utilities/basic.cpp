#define VOID_ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))

template<typename T>
constexpr size_t distance(T const& a, T const& b) {
  if(a > b) { return a - b; } 
  else      { return b - a; }
}
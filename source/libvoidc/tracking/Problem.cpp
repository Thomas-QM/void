enum struct ProblemKind {
  none = -1,
  error,
  warning
};

constexpr char const* ProblemKind_str(ProblemKind kind) {
  switch(kind) {
    case ProblemKind::none:     return "(none)";
    case ProblemKind::error:    return "Error";
    case ProblemKind::warning:  return "Warning";
  }
  return "(null)";
};

constexpr char const* ProblemKind_color(ProblemKind kind) {
  switch(kind) {
    case ProblemKind::none:     return VOID_ANSI_RESET;
    case ProblemKind::error:    return VOID_ANSI_RED;
    case ProblemKind::warning:  return VOID_ANSI_MAGENTA;
  }
  return VOID_ANSI_RESET;
}

struct Problem {
  static constexpr size_t MESSAGE_LENGTH = 256;

  ProblemKind kind;
  char const* begin;
  char const* end;
  char message[MESSAGE_LENGTH + 1];

  template<typename... Args>
  static Problem create(ProblemKind kind, char const* begin, char const* end, char const* fmt, Args const&... args) {
    Problem prob { kind, begin, end };

    if constexpr(sizeof...(args) > 0) { sprintf(prob.message, fmt, args...); } 
    else { 
      #ifdef VOID_TARGET_WINDOWS
      strcpy_s(prob.message, MESSAGE_LENGTH, fmt);
      #else 
      strcpy(prob.message, fmt); 
      #endif
    }
    return prob;
  }
};
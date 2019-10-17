#define VOID_PANIC(fmt, ...) { \
  fprintf(stderr, "voidc: " VOID_ANSI_RED "Panic" VOID_ANSI_RESET ": "); \
  fprintf(stderr, fmt, ##__VA_ARGS__); \
  fputc('\n', stderr); \
}

#define VOID_RAW_PANIC(fmt, ...) { \
  fprintf(stderr, "voidc: Panic: "); \
  fprintf(stderr, fmt, ##__VA_ARGS__); \
  fputc('\n', stderr); \
}
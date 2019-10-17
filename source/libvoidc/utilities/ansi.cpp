#define VOID_ANSI_RED      "\x1b[31m"
#define VOID_ANSI_GREEN    "\x1b[32m"
#define VOID_ANSI_YELLOW   "\x1b[33m"
#define VOID_ANSI_BLUE     "\x1b[34m"
#define VOID_ANSI_MAGENTA  "\x1b[35m"
#define VOID_ANSI_CYAN     "\x1b[36m"
#define VOID_ANSI_BOLD     "\x1b[1m"
#define VOID_ANSI_RESET    "\x1b[0m"

#ifdef VOID_TARGET_WINDOWS
constexpr int STDOUT_ID = -11;
constexpr int ENABLE_VTERM = 0x0004;
#endif

bool enable_ansi() {
  #ifdef VOID_TARGET_WINDOWS
  HANDLE console_handle = GetStdHandle(STDOUT_ID);

  if(console_handle != (void*)-1 && console_handle != nullptr) {
    DWORD original_mode = 0;

    if(GetConsoleMode(console_handle, &original_mode) == 1) {
      return SetConsoleMode(console_handle, original_mode | ENABLE_VTERM) == 1;
    }
  }
  #endif
  return false;
}
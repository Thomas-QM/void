// io in c is the worst thing known to man. *especially on windows*
char* open_txt(char const* path, size_t& buf_len) {
  char* buf = nullptr;
  FILE* file = nullptr;

  #ifdef VOID_TARGET_WINDOWS
  fopen_s(&file, path, "rb");
  #else
  file = fopen(path, "rb");
  #endif
  if(file == nullptr) { return nullptr; }
  fseek(file, 0, SEEK_END);
  buf_len = ftell(file);
  fseek(file, 0, SEEK_SET);
  buf = (char*)malloc(buf_len + 1);
  if(buf == nullptr) { return nullptr; }
  fread(buf, buf_len, 1, file);
  fclose(file);
  buf[buf_len] = '\0';
  return buf;
}
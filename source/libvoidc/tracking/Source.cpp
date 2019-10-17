struct Source {
  static constexpr size_t MAX_PRE_LINE_EXCERPTS = 3;
  static constexpr size_t MAX_POST_LINE_EXCERPTS = 3;
  static constexpr size_t MAX_PROBLEMS = 25;

  char* path;
  char* file;
  size_t file_len;
  Vector<Problem> problems;

  static Source create(char const* path) {
    Source src;
    src.path = str_clone(path);
    src.file = open_txt(path, src.file_len);
    src.problems = Vector<Problem>::create();

    if(src.file == nullptr) { VOID_PANIC("Failed to open file: '%s'", path); }
    return src;
  }

  void destroy() {
    free(path);
    free(file);
    problems.destroy();
  }

  template<typename... Args>
  void error(char const* begin, char const* end, char const* fmt, Args const&... args) {
    problems.add(Problem::create(ProblemKind::error, begin, end, fmt, args...));  
  }

  template<typename... Args>
  void warning(char const* begin, char const* end, char const* fmt, Args const&... args) {
    problems.add(Problem::create(ProblemKind::warning, begin, end, fmt, args...));  
  }

  SourceLocation location(char const* ptr) {
    SourceLocation loc = SourceLocation::create(1, 1);
    char const* it = file;

    while(it < ptr && it < file + file_len) {
      char c = *it;

      if(it + char_size(it) < ptr) {
        char next = *(it + char_size(it));

        if((c == '\r' && next == '\n') || (c == '\n' && next == '\r')) {
          ++ loc.line;
          loc.column = 1;
          utf8_incr(it, 2);
          continue;
        }
      }
      utf8_incr(it);
      ++ loc.column;
      if(c == '\n' || c == '\r') {
        ++ loc.line;
        loc.column = 1;
      }
    }
    return loc;
  }

  void dump() {
    for(size_t i = 0; i < problems.len; ++ i) {
      Problem& problem = problems[i];
      SourceLocation loc = location(problem.begin);
      char const* kind_str = ProblemKind_str(problem.kind);
      char const* color = ProblemKind_color(problem.kind);
      char const* excerpt_begin = next_line(problem.begin);
      size_t pre_line_count = 0;
      size_t max_line_len = 0;

      for(size_t j = 0; j < MAX_PRE_LINE_EXCERPTS; ++ j) {
        if(excerpt_begin == file) { break; } 
        excerpt_begin = previous_line(file, excerpt_begin);
        ++ pre_line_count;
      }
      {
        char const* line = excerpt_begin;

        for(size_t j = 0; j < pre_line_count + MAX_POST_LINE_EXCERPTS; ++ j) {
          SourceLocation cur_loc = location(line);
          
          max_line_len = num_length(cur_loc.line);
          line = next_line(line);
          if(line == file + file_len) { break; }
        }
      }
      fprintf(stderr, "%s%s" VOID_ANSI_RESET " in %s%s:%zu:%zu" VOID_ANSI_RESET "\n  %*s" VOID_ANSI_YELLOW "%s" VOID_ANSI_RESET "\n ",
        color, kind_str, color, path, loc.line, loc.column, (int)max_line_len, "", problem.message);
      for(size_t j = 0; j < pre_line_count + MAX_POST_LINE_EXCERPTS; ++ j) {
        SourceLocation cur_loc = location(excerpt_begin);
        size_t line_len = num_length(cur_loc.line);
        char const* eol = end_of_line(excerpt_begin);
        
        fprintf(stderr, "%*s%s%zu " VOID_ANSI_RESET VOID_ANSI_YELLOW "|" VOID_ANSI_RESET " %.*s\n ", 
          (int)distance(max_line_len, line_len), "", cur_loc.line == loc.line ? color : "", cur_loc.line, (int)distance(eol, excerpt_begin), excerpt_begin);
        if(cur_loc.line == loc.line) {
          fprintf(stderr, "%*s%s " VOID_ANSI_RESET VOID_ANSI_YELLOW "| %s", (int)max_line_len, "", color, color);
          for(char const* it = excerpt_begin; it < eol; utf8_incr(it)) {
            if(it == problem.begin) {
              fputc('^', stderr);
            } else if(it > problem.begin && it < problem.end) {
              fputc('~', stderr);
            } else {
              fputc(' ', stderr);
            }
          }
          fputs(VOID_ANSI_RESET "\n ", stderr);
        }
        excerpt_begin = next_line(excerpt_begin);
        if(excerpt_begin == file + file_len) { break; }
      }
      fputc('\n', stderr);
    }
  }
};
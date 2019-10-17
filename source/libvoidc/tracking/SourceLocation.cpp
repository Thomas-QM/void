struct SourceLocation {
  size_t line;
  size_t column;

  static SourceLocation create(size_t line, size_t column) { return { line, column }; }
};
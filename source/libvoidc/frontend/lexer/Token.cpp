struct ReservedToken {
  char const* str;
  size_t str_len;
};

enum struct KeywordKind {
  none = -1,
  function,
  let
};

static ReservedToken const KEYWORDS[] = {
  { "func", 4 },  // function
  { "let",  3 }   // let
};

enum struct OperatorKind {
  none = -1,

  add,
  subtract,
  multiply,
  divide,

  end_statement,
  paren_open,
  paren_close,
  comma,
};

bool is_prefix(OperatorKind kind) {
  return kind >= OperatorKind::add 
    && kind <= OperatorKind::subtract;
}

bool is_infix(OperatorKind kind) {
  return kind >= OperatorKind::add 
    && kind <= OperatorKind::divide;
}

bool is_seperator(OperatorKind kind) {
  return (kind >= OperatorKind::end_statement 
    && kind <= OperatorKind::end_statement)
    || kind == OperatorKind::paren_close
    || kind == OperatorKind::comma;
}

enum struct OperatorAssociation {
  left,
  right
};

static ReservedToken const OPERATORS[] = {
  { "+", 1 }, // add
  { "-", 1 }, // subtract
  { "*", 1 }, // multiply
  { "/", 1 }, // divide
  { ";", 1 }, // end statement
  { "(", 1 }, // paren open
  { ")", 1 }, // paren close
  { ",", 1 }, // comma
};

static int const OPERATOR_POWERS[] = {
  1, // add
  1, // subtract
  2, // multiply
  2, // divide
};

static OperatorAssociation const OPERATOR_ASSOCIATIONS[] = {
  OperatorAssociation::left, // add
  OperatorAssociation::left, // subtract
  OperatorAssociation::left, // multiply
  OperatorAssociation::left, // divide
};

enum struct TokenKind {
  none = -1,
  identifier,
  integer,
  point,
  string,
  character,
  keyword,
  op
};

char const* TokenKind_str(TokenKind kind) {
  switch(kind) {
    case TokenKind::identifier:
      return "Identifier";
    case TokenKind::integer:
      return "Integer";
    case TokenKind::point:
      return "Point";
    case TokenKind::string:
      return "String";
    case TokenKind::character:
      return "Character";
    case TokenKind::keyword:
      return "Keyword";
    case TokenKind::op:
      return "Operator";
    default: return "(none)";
  }
}

union TokenData {
  unsigned long long integer;
  double point;
  String string;
  char32_t character;
  KeywordKind keyword;
  OperatorKind op;
};

struct Token {
  TokenKind kind;
  char const* begin;
  char const* end;
  TokenData data;

  static Token create(char const* begin, char const* end)                       { return { TokenKind::identifier, begin, end }; }
  static Token create(unsigned long long i, char const* begin, char const* end) { return { TokenKind::integer, begin, end, .data.integer = i }; }
  static Token create(double point, char const* begin, char const* end)         { return { TokenKind::point, begin, end, .data.point = point }; }
  static Token create(String const& str, char const* begin, char const* end)    { return { TokenKind::string, begin, end, .data.string = str }; }
  static Token create(char32_t ch, char const* begin, char const* end)          { return { TokenKind::character, begin, end, .data.character = ch }; }
  static Token create(KeywordKind kw, char const* begin, char const* end)       { return { TokenKind::keyword, begin, end, .data.keyword = kw }; }
  static Token create(OperatorKind op, char const* begin, char const* end)      { return { TokenKind::op, begin, end, .data.op = op }; }

  // This is bad, dont take this as anything useful.
  void print() {
    printf("Kind: " VOID_ANSI_YELLOW "%s" VOID_ANSI_RESET ", Selection: " VOID_ANSI_CYAN "'%.*s'" VOID_ANSI_RESET, TokenKind_str(kind), (int)distance(begin, end), begin);
    switch(kind) {
      case voidc::TokenKind::integer:
        printf(", Content: " VOID_ANSI_GREEN "'%zu'" VOID_ANSI_RESET, data.integer);
      break;
      case voidc::TokenKind::point:
        printf(", Content: " VOID_ANSI_GREEN "'%f'" VOID_ANSI_RESET, data.point);
      break;
      case voidc::TokenKind::keyword:
        printf(", Content: " VOID_ANSI_GREEN "'%i'" VOID_ANSI_RESET, (int)data.keyword);
      break;
      case voidc::TokenKind::op:
        printf(", Content: " VOID_ANSI_GREEN "'%i'" VOID_ANSI_RESET, (int)data.op);
      break;
      case voidc::TokenKind::string:
        printf(", Content: " VOID_ANSI_GREEN "'%s'" VOID_ANSI_RESET, data.string.data);
      break;
      case voidc::TokenKind::character:
      {
        char str[5] {};
        voidc::to_utf8_str(data.character, str);
        printf(", Content: " VOID_ANSI_GREEN "'%s'" VOID_ANSI_RESET, str);
      }
      break;
      default: break;
    }
    putchar('\n');
  }

  void destroy() {
    switch(kind) {
      case TokenKind::string: data.string.destroy(); break;
      default: break;
    }
  }
};
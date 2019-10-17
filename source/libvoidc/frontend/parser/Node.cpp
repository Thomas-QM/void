enum struct NodeKind {
  none = -1,
  identifier, // begin, end
  integer,    // integer
  point,      // point
  string,     // string
  character,  // character
  op,         // op
  call,       // children
  arguments,  // children
};

char const* NodeKind_str(NodeKind kind) {
  switch(kind) {
    case NodeKind::identifier:  return "Identifier";
    case NodeKind::integer:     return "Integer";
    case NodeKind::point:       return "Point";
    case NodeKind::string:      return "String";
    case NodeKind::character:   return "Character";
    case NodeKind::op:          return "Operator";
    case NodeKind::call:        return "Call";
    case NodeKind::arguments:   return "Arguments";
    default:                    return "(none)";
  }
}

// Ugh, c++ >:(
struct Node;

union NodeData {
  Vector<Node> children;
  unsigned long long integer;
  double point;
  String string;
  char32_t character;
  struct NodeOperator {
    OperatorKind kind;
    Vector<Node> children;
  } op;
};

struct Node {
  NodeKind kind;
  char const* begin;
  char const* end;
  NodeData data;

  static Node create_null()                   { return { NodeKind::none, nullptr, nullptr }; }
  static Node create_children(NodeKind kind)  { return { kind, nullptr, nullptr, .data.children = Vector<Node>::create() }; }

  static Node create_literal(Token const& token) {
    switch(token.kind) {
      case TokenKind::identifier: return { NodeKind::identifier, token.begin, token.end };
      case TokenKind::integer:    return { NodeKind::integer, token.begin, token.end, .data.integer = token.data.integer };
      case TokenKind::point:      return { NodeKind::point, token.begin, token.end, .data.point = token.data.point };
      case TokenKind::string:     return { NodeKind::string, token.begin, token.end, .data.string = token.data.string };
      case TokenKind::character:  return { NodeKind::character, token.begin, token.end, .data.character = token.data.character };
      default:                    return create_null();
    }
  }

  template<typename... Args>
  static Node create(NodeKind kind, Token const& token, Args const&... args) {
    Node node { kind, token.begin, token.end, .data.children = Vector<Node>::create() };

    node.data.children.add_many(args...);
    return node;
  }

  template<typename... Args>
  static Node create_op(Token const& token, Args const&... args) {
    Node node { NodeKind::op, token.begin, token.end, .data.op = { token.data.op, Vector<Node>::create() } };

    node.data.op.children.add_many(args...);
    return node;
  }

  // This is all bad code, dont read, dont smell, dont *look*
  void print(int ind = 0) {
    for(size_t i = 0; i < ind; ++ i) { putchar(' '); }
    if(begin == nullptr || end == nullptr) { 
      printf("%s: (null)\n", NodeKind_str(kind));
    } 
    else { printf("%s: %.*s\n", NodeKind_str(kind), (int)distance(begin, end), begin); }
    if(kind == NodeKind::op) {
      for(size_t i = 0; i < data.op.children.len; ++ i) {
        data.op.children[i].print(ind + 2);
      }
    }
    if(kind >= NodeKind::call && kind <= NodeKind::arguments) {
      for(size_t i = 0; i < data.children.len; ++ i) {
        data.children[i].print(ind + 2);
      }
    }
  }
  
  bool is_null() { return kind == NodeKind::none; }
};
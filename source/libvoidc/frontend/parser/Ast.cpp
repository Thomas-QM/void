struct Ast {
  Vector<Node> children;

  static Ast  create()  { return { Vector<Node>::create() }; }
  void        print()   { for(size_t i = 0; i < children.len; ++ i) { children[i].print(); } }
};
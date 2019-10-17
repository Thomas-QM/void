void sync(ParserInstance& instance, SyncState state = {}) {
  while(instance.valid()) { 
    if(instance.is_seperator() 
      && state.paren_count == 0 
      && state.brack_count == 0) { 
      break; 
    }
    if(instance.match(OperatorKind::paren_open)) { ++ state.paren_count; } 
    else if(instance.match(OperatorKind::paren_close)) { -- state.paren_count; }
    instance.advance(); 
  }
}

bool expect(ParserInstance& instance, OperatorKind kind, SyncState state = {}) {
  if(instance.match(kind)) {
    instance.advance();
    return true;
  } else {
    instance.error(instance.valid() ? instance.current() : instance.last(), "Expected operator `%s`", OPERATORS[(int)kind].str);
    sync(instance, state);
    return false;
  }
}

Node subexpression(ParserInstance&);

Node expression(ParserInstance& instance, int pre_bind_power = 0) {
  Node left = subexpression(instance);

  while(true) {
    Token const& cur = instance.current();

    if(!instance.valid() || instance.is_seperator()) { break; }
    if(!instance.is_infix()) {
      instance.error(cur, "Expected infix operator, got %s", TokenKind_str(instance.current().kind));
      sync(instance);
      break;
    }
    int cur_power = OPERATOR_POWERS[(int)cur.data.op];

    if(cur_power <= pre_bind_power) { break; }
    OperatorAssociation cur_assoc = OPERATOR_ASSOCIATIONS[(int)cur.data.op];
    int cur_bind_power = (cur_assoc == OperatorAssociation::left ? cur_power : cur_power - 1);

    instance.advance();
    left = Node::create_op(cur, left, expression(instance, cur_bind_power));
  }
  return left;
}

Node subexpression(ParserInstance& instance) {
  Node head = Node::create_null();

  if(!instance.valid()) {
    instance.error(instance.last(), "Expected expression");
    return head;
  }

  // Prefix
  if(instance.is_prefix()) {
    Token const& prefix = instance.consume();

    return Node::create_op(prefix, subexpression(instance));
  }
  if(instance.match(OperatorKind::paren_open)) {
    Token const& prefix = instance.consume();

    head = expression(instance);
    expect(instance, OperatorKind::paren_close);
  }
  if(head.is_null()) {
    if(instance.match(TokenKind::character)
      || instance.match(TokenKind::identifier)
      || instance.match(TokenKind::integer)
      || instance.match(TokenKind::point)
      || instance.match(TokenKind::string)) {
      head = Node::create_literal(instance.consume());
    }
  }

  // Postfix
  while(true) {
    if(instance.match(OperatorKind::paren_open)) {
      Token const& prefix = instance.consume();
      Node args = Node::create_children(NodeKind::arguments);

      while(instance.valid()) {
        if(instance.match(OperatorKind::paren_close)) { break; }
        args.data.children.add(expression(instance));
        if(instance.match(OperatorKind::comma)) { 
          instance.advance(); 
          continue;
        }
        break;
      }
      expect(instance, OperatorKind::paren_close);
      head = Node::create(NodeKind::call, prefix, head, args);
      continue;
    }
    break;
  }

  // Unknown
  if(head.is_null()) {
    instance.error(instance.current(), "Expected expression");
    sync(instance);
    return head;
  } 
  return head;
}

Node statement(ParserInstance& instance) {
  // Temporary
  return expression(instance);
}

Ast parse(Source& src, Vector<Token> const& tokens) {
  Ast ast = Ast::create();
  ParserInstance instance = ParserInstance::create(src, ast, tokens);

  while(instance.valid()) { 
    ast.children.add(statement(instance)); 
    if(instance.match(OperatorKind::end_statement)) { 
      instance.advance(); 
    }
  }
  return ast;
}
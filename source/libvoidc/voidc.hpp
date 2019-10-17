#ifndef LIBVOIDC_H
#define LIBVOIDC_H

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define VOID_TARGET_WINDOWS
#include <Windows.h>
#endif

namespace voidc {
  #include "utilities/basic.cpp"
  #include "utilities/ansi.cpp"
  #include "utilities/panic.cpp"
  #include "utilities/Vector.cpp"
  #include "utilities/String.cpp"
  #include "utilities/numeric.cpp"
  #include "utilities/char.cpp"
  #include "utilities/str.cpp"
  #include "utilities/io.cpp"

  #include "tracking/Problem.cpp"
  #include "tracking/SourceLocation.cpp"
  #include "tracking/Source.cpp"

  #include "frontend/lexer/Token.cpp"
  #include "frontend/lexer/LexerInstance.cpp"
  #include "frontend/lexer/Lexlet.cpp"
  #include "frontend/lexer/lex.cpp"

  #include "frontend/parser/Node.cpp"
  #include "frontend/parser/Ast.cpp"
  #include "frontend/parser/ParserInstance.cpp"
  #include "frontend/parser/SyncState.cpp"
  #include "frontend/parser/parse.cpp"
}

#endif
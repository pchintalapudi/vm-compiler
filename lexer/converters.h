#ifndef OOPS_COMPILER_LEXER_CONVERTERS_H
#define OOPS_COMPILER_LEXER_CONVERTERS_H

#include <array>
#include <string>
#include <unordered_map>

namespace oops_compiler {
namespace lexer {
enum class operators {
  ACCESS,
  ADD,
  ADDEQ,
  AND,
  BITAND,
  BITANDEQ,
  BITOR,
  BITOREQ,
  BITSLL,
  BITSLLEQ,
  BITSRA,
  BITSRAEQ,
  BITSRL,
  BITSRLEQ,
  BITXOR,
  BITXOREQ,
  BITNOT,
  BLOCK_COMMENT_CLOSE,
  BLOCK_COMMENT_OPEN,
  CAST,
  COLON,
  COMMA,
  CURLY_CLOSE,
  CURLY_OPEN,
  DEC,
  DIV,
  DIVEQ,
  EQ,
  EQUALS,
  FATARROW,
  GEQUALS,
  GREATER,
  INC,
  LEQUALS,
  LESS,
  LINE_COMMENT,
  LNOT,
  MOD,
  MODEQ,
  MUL,
  MULEQ,
  NEQUALS,
  NULLACCESS,
  OR,
  ROUND_CLOSE,
  ROUND_OPEN,
  SCOPE,
  SEMICOLON,
  SQUARE_CLOSE,
  SQUARE_OPEN,
  SUB,
  SUBEQ,
  TERNARY_START,
  __COUNT__
};
enum class keywords {
  AS,
  AUTO,
  BOOLEAN,
  BREAK,
  BYTE,
  CASE,
  CATCH,
  CLASS,
  CONSTRUCTOR,
  CONTINUE,
  DEFAULT,
  DEF,
  DO,
  DOUBLE,
  ELSE,
  ENUM,
  EXTENDS,
  FALSE,
  FINAL,
  FINALLY,
  FLOAT,
  FOR,
  GET,
  GOTO,
  IF,
  IMPORT,
  IMPLEMENTS,
  INT,
  INTERFACE,
  INSTANCEOF,
  INTRINSIC,
  LONG,
  NATIVE,
  NEW,
  NIL,
  OPERATOR,
  PACKAGE,
  PRIVATE,
  PROTECTED,
  PUBLIC,
  RETURN,
  SET,
  SHORT,
  STATIC,
  SUPER,
  SWITCH,
  THIS,
  THROW,
  TRUE,
  TRY,
  USING,
  VOID,
  WHILE,
  __COUNT__
};

struct mappings {
  std::unordered_map<std::string, operators> string_to_operators{};
  std::unordered_map<std::string, keywords> string_to_keywords{};
  std::array<const char*, static_cast<std::size_t>(operators::__COUNT__)>
      operators_to_strings{};
  std::array<const char*, static_cast<std::size_t>(keywords::__COUNT__)>
      keywords_to_strings{};

  mappings();
};
inline mappings all_mappings;
}  // namespace lexer
}  // namespace oops_compiler

#endif
#ifndef OOPS_COMPILER_PARSER_NARY_EXPRESSION_H
#define OOPS_COMPILER_PARSER_NARY_EXPRESSION_H

#include "expression.h"

namespace oops_compiler {
namespace parser {
class binary_expression : public expression {
 public:
  enum class type {
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    EQUALS,
    LESS,
    LESS_EQ,
    GREATER,
    GREATER_EQ,
    NOT_EQUALS,
    IADD,
    ISUB,
    IMUL,
    IDIV,
    SLL,
    SRL,
    SRA,
    AND,
    OR,
    XOR,
    ISLL,
    ISRL,
    ISRA,
    IAND,
    IOR,
    IXOR,
    ASSIGN,
    INDEX,
    INDEXED_ASSIGN
  };

 private:
 public:
  const expression &get_first_expression() const;
  const expression &get_second_expression() const;
  type get_type() const;
  static std::pair<binary_expression, std::size_t> parse(
      std::vector<lexer::token> tokens, std::size_t start);
};
class unary_expression : public expression {
 public:
  enum class type {
    PREINC,
    PREDEC,
    POSITIVE,
    NEGATIVE,
    CALL,
    POSTINC,
    POSTDEC,
    BITNOT,
    LOGICNOT
  };

 private:
 public:
  const expression &get_expression() const;
  type get_type() const;
  static std::pair<unary_expression, std::size_t> parse(
      std::vector<lexer::token> tokens, std::size_t start);
};
}  // namespace parser
}  // namespace oops_compiler

#endif
#ifndef OOPS_COMPILER_PARSER_NARY_EXPRESSION_H
#define OOPS_COMPILER_PARSER_NARY_EXPRESSION_H

#include "expression.h"

namespace oops_compiler {
namespace parser {
class binary_expression : public expression,
                          public parseable<binary_expression> {
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
  type t;
  std::unique_ptr<expression> first, second;

 public:
  const expression &get_first_expression() const { return *first; }
  const expression &get_second_expression() const { return *second; }
  type get_type() const { return t; }
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
  type t;
  std::unique_ptr<expression> expr;

 public:
  const expression &get_expression() const { return *expr; }
  type get_type() const { return t; }
};
}  // namespace parser
}  // namespace oops_compiler

#endif
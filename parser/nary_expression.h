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
  type t;
  std::unique_ptr<expression> first, second;

 public:
  binary_expression(std::unique_ptr<expression> first, type t,
                    std::unique_ptr<expression> second)
      : t(t), first(std::move(first)), second(std::move(second)) {}
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
  unary_expression(type t, std::unique_ptr<expression> expr)
      : t(t), expr(std::move(expr)) {}
  const expression &get_expression() const { return *expr; }
  type get_type() const { return t; }
};

output<expression> parse_nary_expression(const char* filename, const std::vector<lexer::token> &tokens, std::size_t begin, classloader &loader, scope &scope);
}  // namespace parser
}  // namespace oops_compiler

#endif
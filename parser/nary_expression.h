#ifndef OOPS_COMPILER_PARSER_NARY_EXPRESSION_H
#define OOPS_COMPILER_PARSER_NARY_EXPRESSION_H

#include "expression.h"

namespace oops_compiler {
namespace parser {

/*
Expression ordering
ASSIGNMENT/COMPOUND ASSIGNMENT < LOR < LAND < BOR < BXOR < BAND < EQUALS < IS <
INSTANCEOF < RELATIONAL < SHIFT < ADD/SUB < MUL/DIV/MOD < CAST <
LNOT/BNOT/PLUS/MINUS/PREINC/PREDEC < POSTINC/POSTDEC < FUNC/INDEX < ACCESS <
NEW
*/
#define multi_binary_expression(expr_type, ...)                      \
  struct expr_type##_expression : public expression {                \
    enum class type { __VA_ARGS__, NONE };                           \
    type t;                                                          \
    std::unique_ptr<expression> left;                                \
    std::unique_ptr<expression> right;                               \
    expr_type##_expression(type t, std::unique_ptr<expression> left, \
                           std::unique_ptr<expression> right)        \
        : t(t), left(std::move(left)), right(std::move(right)) {}    \
  }
multi_binary_expression(assignment, BASIC, BOR, BXOR, BAND, SLL, SRA, SRL, ADD,
                        SUB, MUL, DIV, MOD);
#define binary_expression(type)                             \
  struct type##_expression : public expression {            \
    std::unique_ptr<expression> left;                       \
    std::unique_ptr<expression> right;                      \
    type##_expression(std::unique_ptr<expression> left,     \
                      std::unique_ptr<expression> right)    \
        : left(std::move(left)), right(std::move(right)) {} \
  }
binary_expression(lor);
binary_expression(land);
binary_expression(bor);
binary_expression(bxor);
binary_expression(band);
multi_binary_expression(equals, SAME, NOT);
binary_expression(is);
binary_expression(instanceof);
multi_binary_expression(relational, LESS, LEQUAL, GREATER, GEQUAL);
multi_binary_expression(shift, SLL, SRA, SRL);
multi_binary_expression(addition, ADD, SUB);
multi_binary_expression(multiplicative, MUL, DIV, MOD);
binary_expression(cast);
#define multi_unary_expression(type, ...)                       \
  struct type##_expression : public expression {                \
    enum class type { __VA_ARGS__, NONE };                      \
    type t;                                                     \
    std::unique_ptr<expression> expr;                           \
    type##_expression(type t, std::unique_ptr<expression> expr) \
        : t(t), expr(std::move(expr)) {}                        \
  }
multi_unary_expression(prefix, LNOT, BNOT, PLUS, MINUS, INC, DEC);
multi_unary_expression(postfix, INC, DEC);
struct call_expression : public expression {
  enum class type { FUNCTION, INDEX, NONE };
  type t;
  std::unique_ptr<expression> callable;
  std::vector<std::unique_ptr<expression>> arguments;
  call_expression(type t, std::unique_ptr<expression> callable,
                  std::vector<std::unique_ptr<expression>> arguments)
      : t(t), callable(std::move(callable)), arguments(std::move(arguments)) {}
};
#undef multi_binary_expression
#undef binary_expression
#undef multi_unary_expression
}  // namespace parser
}  // namespace oops_compiler

#endif
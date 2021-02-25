#include <unordered_set>

#include "access_expression.h"
#include "expression.h"
#include "impl_common.h"
#include "loop_control.h"
#include "nary_expression.h"
#include "type.h"

using namespace oops_compiler::parser;

parse_decl(expression);
parse_decl(parenthetical);
parse_decl(assignment_expression);
parse_decl(lor_expression);
parse_decl(land_expression);
parse_decl(bor_expression);
parse_decl(bxor_expression);
parse_decl(band_expression);
parse_decl(equals_expression);
parse_decl(is_expression);
parse_decl(instanceof_expression);
parse_decl(relational_expression);
parse_decl(shift_expression);
parse_decl(addition_expression);
parse_decl(multiplicative_expression);
parse_decl(cast_expression);
parse_decl(prefix_expression);
parse_decl(postfix_expression);
parse_decl(call_expression);
parse_decl(access_expression);

parse_decl(parenthetical) {
  output<parenthetical> out;
  out.filename = filename;
  out.contexts.push_back(tokens[begin].token_context);
  auto expr = parse<expression>(filename, tokens, begin + 1, classes);
  std::copy(expr.messages.begin(), expr.messages.end(),
            std::back_inserter(out.messages));
  out.next_token = expr.next_token;
  if (!expr.value) {
    return out;
  }
  if (out.next_token == tokens.size()) {
    message_builder builder;
    builder.builder << "Missing closing parenthesis in expession!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[out.next_token - 1].token_context));
    return out;
  }
  if (tokens[out.next_token].token_data.token_type !=
          lexer::token::data::type::OPERATOR_TOKEN ||
      tokens[out.next_token].token_data.as_operator !=
          lexer::operators::ROUND_CLOSE) {
    message_builder builder;
    builder.builder << "Missing closing parenthesis in expression!";
    out.messages.push_back(builder.build_message(
        logger::level::ERROR, tokens[out.next_token].token_context));
  }
  out.value = std::make_unique<parenthetical>(std::move(*expr.value));
  out.contexts.push_back(tokens[out.next_token].token_context);
  out.next_token++;
  return out;
}

parse_decl(break_expression) {
  (void)classes;
  output<break_expression> out;
  out.filename = filename;
  out.contexts.push_back(tokens[begin].token_context);
  out.next_token = begin + 1;
  out.value = std::make_unique<break_expression>();
  return out;
}

parse_decl(continue_expression) {
  (void)classes;
  output<continue_expression> out;
  out.filename = filename;
  out.contexts.push_back(tokens[begin].token_context);
  out.next_token = begin + 1;
  out.value = std::make_unique<continue_expression>();
  return out;
}

parse_decl(identifier_expression) {
  (void)classes;
  output<identifier_expression> out;
  out.filename = filename;
  out.contexts.push_back(tokens[begin].token_context);
  out.next_token = begin + 1;
  out.value = std::make_unique<identifier_expression>(
      tokens[begin].token_data.as_identifier.start,
      tokens[begin].token_data.as_identifier.size);
  return out;
}

parse_decl(literal_expression) {
  (void)classes;
  output<literal_expression> out;
  out.filename = filename;
  out.contexts.push_back(tokens[begin].token_context);
  out.next_token = begin + 1;
  out.value = std::make_unique<literal_expression>(tokens[begin]);
  return out;
}

namespace {
struct expression_tree {
  expression_tree *parent;
  oops_compiler::lexer::token token;
  std::unique_ptr<expression_tree> left, right;
};

}  // namespace

parse_decl(expression) {
  switch (tokens[begin].token_data.token_type) {
    case lexer::token::data::type::KEYWORD_TOKEN:
      switch (tokens[begin].token_data.as_keyword) {
        case lexer::keywords::BREAK:
          return output<expression>::generalize(
              parse<break_expression>(filename, tokens, begin, classes));
        case lexer::keywords::CONTINUE:
          return output<expression>::generalize(
              parse<continue_expression>(filename, tokens, begin, classes));
        default:
          break;
      }
      break;
    default:
      break;
  }
  return output<expression>::generalize(
      parse<assignment_expression>(filename, tokens, begin, classes));
}

parse_decl(access_chain) {
  output<access_chain> out;
  out.filename = filename;
  out.next_token = begin;
  out.contexts.push_back(tokens[out.next_token].token_context);
  std::string identifier(tokens[out.next_token].token_data.as_identifier.start,
                         tokens[out.next_token].token_data.as_identifier.size);
  out.next_token++;
  if (out.next_token < tokens.size() &&
      tokens[out.next_token].token_data.token_type ==
          lexer::token::data::type::OPERATOR_TOKEN &&
      tokens[out.next_token].token_data.as_operator ==
          lexer::operators::ACCESS) {
    out.next_token++;
    if (out.next_token == tokens.size()) {
      message_builder builder;
      builder.builder << "Unexpected end of access expression!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
      return out;
    }
    if (tokens[out.next_token].token_data.token_type !=
        lexer::token::data::type::IDENTIFIER_TOKEN) {
      message_builder builder;
      builder.builder << "Expected identifier after access operator!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
      return out;
    }
    output<access_chain> right =
        parse<access_chain>(filename, tokens, out.next_token, classes);
    std::copy(right.messages.begin(), right.messages.end(),
              std::back_inserter(out.messages));
    out.next_token = right.next_token;
    if (!right.value) {
      return out;
    }
    out.value = std::make_unique<access_chain>(std::move(identifier),
                                               std::move(*right.value));
  } else {
    out.value = std::make_unique<access_chain>(std::move(identifier),
                                               std::unique_ptr<access_chain>{});
  }
  return out;
}
namespace {
assignment_expression::type get_assignment_type(
    oops_compiler::lexer::operators op) {
  switch (op) {
    case oops_compiler::lexer::operators::ADDEQ:
      return assignment_expression::type::ADD;

    case oops_compiler::lexer::operators::BITANDEQ:
      return assignment_expression::type::BAND;

    case oops_compiler::lexer::operators::BITOREQ:
      return assignment_expression::type::BOR;

    case oops_compiler::lexer::operators::BITSLLEQ:
      return assignment_expression::type::SLL;

    case oops_compiler::lexer::operators::BITSRAEQ:
      return assignment_expression::type::SRA;

    case oops_compiler::lexer::operators::BITSRLEQ:
      return assignment_expression::type::SRL;

    case oops_compiler::lexer::operators::BITXOREQ:
      return assignment_expression::type::BXOR;

    case oops_compiler::lexer::operators::DIVEQ:
      return assignment_expression::type::DIV;

    case oops_compiler::lexer::operators::EQ:
      return assignment_expression::type::BASIC;

    case oops_compiler::lexer::operators::MODEQ:
      return assignment_expression::type::MOD;

    case oops_compiler::lexer::operators::MULEQ:
      return assignment_expression::type::MUL;

    case oops_compiler::lexer::operators::SUBEQ:
      return assignment_expression::type::SUB;

    default:
      return assignment_expression::type::NONE;
  }
}
}  // namespace

#define multi_binary_expression(exprtype, nexttype, exprname, opfunc)          \
  parse_decl(exprtype) {                                                       \
    output<exprtype> out;                                                      \
    out.filename = filename;                                                   \
    out.next_token = begin;                                                    \
    out.contexts.push_back(tokens[out.next_token].token_context);              \
    std::unique_ptr<expression> left;                                          \
    output<nexttype> next =                                                    \
        parse<nexttype>(filename, tokens, out.next_token, classes);            \
    std::copy(next.messages.begin(), next.messages.end(),                      \
              std::back_inserter(out.messages));                               \
    out.next_token = next.next_token;                                          \
    if (!next.value) {                                                         \
      return out;                                                              \
    }                                                                          \
    left = std::move(*next.value);                                             \
    if (out.next_token == tokens.size() ||                                     \
        tokens[out.next_token].token_data.token_type !=                        \
            lexer::token::data::type::OPERATOR_TOKEN) {                        \
      out.value = std::make_unique<exprtype>(                                  \
          exprtype::type::NONE, std::move(left), std::unique_ptr<exprtype>{}); \
    } else {                                                                   \
      exprtype::type t =                                                       \
          opfunc(tokens[out.next_token].token_data.as_operator);               \
      if (t != exprtype::type::NONE) {                                         \
        out.next_token++;                                                      \
        if (out.next_token == tokens.size()) {                                 \
          message_builder builder;                                             \
          builder.builder << "Unexpected end of " exprname " expression!";     \
          out.messages.push_back(                                              \
              builder.build_message(logger::level::FATAL_ERROR,                \
                                    tokens[out.next_token].token_context));    \
          return out;                                                          \
        }                                                                      \
        output<expression> right =                                             \
            parse<expression>(filename, tokens, out.next_token, classes);      \
        std::copy(right.messages.begin(), right.messages.end(),                \
                  std::back_inserter(out.messages));                           \
        out.next_token = right.next_token;                                     \
        if (!right.value) {                                                    \
          return out;                                                          \
        }                                                                      \
        out.value = std::make_unique<exprtype>(t, std::move(left),             \
                                               std::move(*right.value));       \
      } else {                                                                 \
        out.value = std::make_unique<exprtype>(t, std::move(left),             \
                                               std::unique_ptr<expression>{}); \
      }                                                                        \
    }                                                                          \
    return out;                                                                \
  }
multi_binary_expression(assignment_expression, lor_expression, "assignment",
                        get_assignment_type);
#define single_binary_expression(exprtype, nexttype, exprname, op)           \
  parse_decl(exprtype) {                                                     \
    output<exprtype> out;                                                    \
    out.filename = filename;                                                 \
    out.next_token = begin;                                                  \
    out.contexts.push_back(tokens[out.next_token].token_context);            \
    std::unique_ptr<expression> left;                                        \
    output<nexttype> next =                                                  \
        parse<nexttype>(filename, tokens, out.next_token, classes);          \
    std::copy(next.messages.begin(), next.messages.end(),                    \
              std::back_inserter(out.messages));                             \
    out.next_token = next.next_token;                                        \
    if (!next.value) {                                                       \
      return out;                                                            \
    }                                                                        \
    left = std::move(*next.value);                                           \
    if (out.next_token == tokens.size() ||                                   \
        tokens[out.next_token].token_data.token_type !=                      \
            lexer::token::data::type::OPERATOR_TOKEN) {                      \
      out.value = std::make_unique<exprtype>(std::move(left),                \
                                             std::unique_ptr<expression>{}); \
    } else {                                                                 \
      bool skip_right = false;                                               \
      switch (tokens[out.next_token].token_data.as_operator) {               \
        case op:                                                             \
          break;                                                             \
        default: {                                                           \
          out.value = std::make_unique<exprtype>(                            \
              std::move(left), std::unique_ptr<expression>{});               \
          skip_right = true;                                                 \
        }                                                                    \
      }                                                                      \
      if (!skip_right) {                                                     \
        out.next_token++;                                                    \
        if (out.next_token == tokens.size()) {                               \
          message_builder builder;                                           \
          builder.builder << "Unexpected end of " exprname " expression!";   \
          out.messages.push_back(                                            \
              builder.build_message(logger::level::FATAL_ERROR,              \
                                    tokens[out.next_token].token_context));  \
          return out;                                                        \
        }                                                                    \
        output<expression> right =                                           \
            parse<expression>(filename, tokens, out.next_token, classes);    \
        std::copy(right.messages.begin(), right.messages.end(),              \
                  std::back_inserter(out.messages));                         \
        out.next_token = right.next_token;                                   \
        if (!right.value) {                                                  \
          return out;                                                        \
        }                                                                    \
        out.value = std::make_unique<exprtype>(std::move(left),              \
                                               std::move(*right.value));     \
      }                                                                      \
    }                                                                        \
    return out;                                                              \
  }

single_binary_expression(lor_expression, land_expression, "logical or",
                         lexer::operators::OR);
single_binary_expression(land_expression, bor_expression, "logical and",
                         lexer::operators::AND);
single_binary_expression(bor_expression, bxor_expression, "bitwise or",
                         lexer::operators::BITOR);
single_binary_expression(bxor_expression, band_expression, "bitwise xor",
                         lexer::operators::BITXOR);
single_binary_expression(band_expression, equals_expression, "bitwise and",
                         lexer::operators::BITAND);

namespace {
equals_expression::type get_equals_type(oops_compiler::lexer::operators op) {
  switch (op) {
    case oops_compiler::lexer::operators::EQUALS:
      return equals_expression::type::SAME;
    case oops_compiler::lexer::operators::NEQUALS:
      return equals_expression::type::NOT;
    default:
      return equals_expression::type::NONE;
  }
}
}  // namespace

multi_binary_expression(equals_expression, is_expression, "equality",
                        get_equals_type);

#define single_binary_keyword_expression(exprtype, nexttype, exprname, op)   \
  parse_decl(exprtype) {                                                     \
    output<exprtype> out;                                                    \
    out.filename = filename;                                                 \
    out.next_token = begin;                                                  \
    out.contexts.push_back(tokens[out.next_token].token_context);            \
    std::unique_ptr<expression> left;                                        \
    output<nexttype> next =                                                  \
        parse<nexttype>(filename, tokens, out.next_token, classes);          \
    std::copy(next.messages.begin(), next.messages.end(),                    \
              std::back_inserter(out.messages));                             \
    out.next_token = next.next_token;                                        \
    if (!next.value) {                                                       \
      return out;                                                            \
    }                                                                        \
    left = std::move(*next.value);                                           \
    if (out.next_token == tokens.size() ||                                   \
        tokens[out.next_token].token_data.token_type !=                      \
            lexer::token::data::type::KEYWORD_TOKEN) {                       \
      out.value = std::make_unique<exprtype>(std::move(left),                \
                                             std::unique_ptr<expression>{}); \
    } else {                                                                 \
      bool skip_right = false;                                               \
      switch (tokens[out.next_token].token_data.as_keyword) {                \
        case op:                                                             \
          break;                                                             \
        default: {                                                           \
          out.value = std::make_unique<exprtype>(                            \
              std::move(left), std::unique_ptr<expression>{});               \
          skip_right = true;                                                 \
        }                                                                    \
      }                                                                      \
      if (!skip_right) {                                                     \
        out.next_token++;                                                    \
        if (out.next_token == tokens.size()) {                               \
          message_builder builder;                                           \
          builder.builder << "Unexpected end of " exprname " expression!";   \
          out.messages.push_back(                                            \
              builder.build_message(logger::level::FATAL_ERROR,              \
                                    tokens[out.next_token].token_context));  \
          return out;                                                        \
        }                                                                    \
        output<expression> right =                                           \
            parse<expression>(filename, tokens, out.next_token, classes);    \
        std::copy(right.messages.begin(), right.messages.end(),              \
                  std::back_inserter(out.messages));                         \
        out.next_token = right.next_token;                                   \
        if (!right.value) {                                                  \
          return out;                                                        \
        }                                                                    \
        out.value = std::make_unique<exprtype>(std::move(left),              \
                                               std::move(*right.value));     \
      }                                                                      \
    }                                                                        \
    return out;                                                              \
  }

single_binary_keyword_expression(is_expression, instanceof_expression, "is",
                                 lexer::keywords::IS);
single_binary_keyword_expression(instanceof_expression, relational_expression,
                                 "instanceof", lexer::keywords::INSTANCEOF);

namespace {
relational_expression::type get_relational_type(
    oops_compiler::lexer::operators op) {
  switch (op) {
    case oops_compiler::lexer::operators::GREATER:
      return relational_expression::type::GREATER;
    case oops_compiler::lexer::operators::GEQUALS:
      return relational_expression::type::GEQUAL;
    case oops_compiler::lexer::operators::LESS:
      return relational_expression::type::LESS;
    case oops_compiler::lexer::operators::LEQUALS:
      return relational_expression::type::LEQUAL;
    default:
      return relational_expression::type::NONE;
  }
}
}  // namespace

multi_binary_expression(relational_expression, shift_expression, "relational",
                        get_relational_type);

namespace {
shift_expression::type get_shift_type(oops_compiler::lexer::operators op) {
  switch (op) {
    case oops_compiler::lexer::operators::BITSLL:
      return shift_expression::type::SLL;
    case oops_compiler::lexer::operators::BITSRA:
      return shift_expression::type::SRA;
    case oops_compiler::lexer::operators::BITSRL:
      return shift_expression::type::SRL;
    default:
      return shift_expression::type::NONE;
  }
}
}  // namespace

multi_binary_expression(shift_expression, addition_expression, "shift",
                        get_shift_type);

namespace {
addition_expression::type get_addition_type(
    oops_compiler::lexer::operators op) {
  switch (op) {
    case oops_compiler::lexer::operators::ADD:
      return addition_expression::type::ADD;
    case oops_compiler::lexer::operators::SUB:
      return addition_expression::type::SUB;
    default:
      return addition_expression::type::NONE;
  }
}
}  // namespace

multi_binary_expression(addition_expression, multiplicative_expression,
                        "addition", get_addition_type);

namespace {
multiplicative_expression::type get_multiplicative_type(
    oops_compiler::lexer::operators op) {
  switch (op) {
    case oops_compiler::lexer::operators::MUL:
      return multiplicative_expression::type::MUL;
    case oops_compiler::lexer::operators::DIV:
      return multiplicative_expression::type::DIV;
    case oops_compiler::lexer::operators::MOD:
      return multiplicative_expression::type::MOD;
    default:
      return multiplicative_expression::type::NONE;
  }
}
}  // namespace

multi_binary_expression(multiplicative_expression, cast_expression,
                        "multiplicative", get_multiplicative_type);
single_binary_keyword_expression(cast_expression, prefix_expression, "cast",
                                 lexer::keywords::AS);

parse_decl(prefix_expression) {
  output<prefix_expression> out;
  out.filename = filename;
  out.next_token = begin;
  out.contexts.push_back(tokens[out.next_token].token_context);
  if (tokens[out.next_token].token_data.token_type ==
      lexer::token::data::type::OPERATOR_TOKEN) {
    prefix_expression::type t;
    switch (tokens[out.next_token].token_data.as_operator) {
      case lexer::operators::ADD:
        t = prefix_expression::type::PLUS;
        break;
      case lexer::operators::SUB:
        t = prefix_expression::type::MINUS;
        break;
      case lexer::operators::LNOT:
        t = prefix_expression::type::LNOT;
        break;
      case lexer::operators::BITNOT:
        t = prefix_expression::type::BNOT;
        break;
      case lexer::operators::INC:
        t = prefix_expression::type::INC;
        break;
      case lexer::operators::DEC:
        t = prefix_expression::type::DEC;
        break;
      default:
        t = prefix_expression::type::NONE;
        break;
    }
    if (t != prefix_expression::type::NONE) {
      out.next_token++;
      if (tokens.size() == out.next_token) {
        message_builder builder;
        builder.builder << "Unexpected end of prefix expression!";
        out.messages.push_back(builder.build_message(
            logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
        return out;
      }
      output<prefix_expression> after =
          parse<prefix_expression>(filename, tokens, out.next_token, classes);
      std::copy(after.messages.begin(), after.messages.end(),
                std::back_inserter(out.messages));
      out.next_token = after.next_token;
      if (!after.value) {
        return out;
      }
      out.value =
          std::make_unique<prefix_expression>(t, std::move(*after.value));
      return out;
    }
  }
  output<postfix_expression> after =
      parse<postfix_expression>(filename, tokens, out.next_token, classes);
  std::copy(after.messages.begin(), after.messages.end(),
            std::back_inserter(out.messages));
  out.next_token = after.next_token;
  if (!after.value) {
    return out;
  }
  out.value = std::make_unique<prefix_expression>(prefix_expression::type::NONE,
                                                  std::move(*after.value));
  return out;
}

parse_decl(postfix_expression) {
  output<postfix_expression> out;
  out.filename = filename;
  out.next_token = begin;
  out.contexts.push_back(tokens[out.next_token].token_context);
  output<access_expression> identifier =
      parse<access_expression>(filename, tokens, out.next_token, classes);
  std::copy(identifier.messages.begin(), identifier.messages.end(),
            std::back_inserter(out.messages));
  out.next_token = identifier.next_token;
  if (!identifier.value) {
    return out;
  }
  if (tokens.size() == out.next_token) {
    out.value = std::make_unique<postfix_expression>(
        postfix_expression::type::NONE, std::move(*identifier.value));
    return out;
  }
  std::unique_ptr<expression> building = std::move(*identifier.value);
  do {
    if (tokens.size() == out.next_token) {
      break;
    }
    if (tokens[out.next_token].token_data.token_type !=
        lexer::token::data::type::OPERATOR_TOKEN) {
      break;
    }
    switch (tokens[out.next_token].token_data.as_operator) {
      case lexer::operators::INC:
        building = std::make_unique<postfix_expression>(
            postfix_expression::type::INC, std::move(building));
        break;
      case lexer::operators::DEC:
        building = std::make_unique<postfix_expression>(
            postfix_expression::type::DEC, std::move(building));
        break;
      case lexer::operators::ROUND_OPEN: {
        std::vector<std::unique_ptr<expression>> arguments;
        do {
          out.next_token++;
          if (tokens.size() == out.next_token) {
            message_builder builder;
            builder.builder << "Unexpected end of function call expression!";
            out.messages.push_back(
                builder.build_message(logger::level::FATAL_ERROR,
                                      tokens[out.next_token].token_context));
            return out;
          }
          output<expression> arg =
              parse<expression>(filename, tokens, out.next_token, classes);
          std::copy(arg.messages.begin(), arg.messages.end(),
                    std::back_inserter(out.messages));
          out.next_token = arg.next_token;
          if (!arg.value) {
            return out;
          }
          arguments.push_back(std::move(*arg.value));
          if (tokens.size() == out.next_token) {
            message_builder builder;
            builder.builder << "Unexpected end of function call expression!";
            out.messages.push_back(
                builder.build_message(logger::level::FATAL_ERROR,
                                      tokens[out.next_token].token_context));
            return out;
          }
          if (tokens[out.next_token].token_data.token_type !=
                  lexer::token::data::type::OPERATOR_TOKEN ||
              (tokens[out.next_token].token_data.as_operator !=
                   lexer::operators::ROUND_CLOSE &&
               tokens[out.next_token].token_data.as_operator !=
                   lexer::operators::COMMA)) {
            message_builder builder;
            builder.builder << "Unexpected token in function call expression!";
            out.messages.push_back(
                builder.build_message(logger::level::FATAL_ERROR,
                                      tokens[out.next_token].token_context));
            return out;
          }
        } while (tokens[out.next_token].token_data.as_operator ==
                 lexer::operators::COMMA);
        out.next_token++;
        building = std::make_unique<call_expression>(
            call_expression::type::FUNCTION, std::move(building),
            std::move(arguments));
        break;
      }
      case lexer::operators::SQUARE_OPEN: {
        std::vector<std::unique_ptr<expression>> arguments;
        do {
          out.next_token++;
          if (tokens.size() == out.next_token) {
            message_builder builder;
            builder.builder << "Unexpected end of index expression!";
            out.messages.push_back(
                builder.build_message(logger::level::FATAL_ERROR,
                                      tokens[out.next_token].token_context));
            return out;
          }
          output<expression> arg =
              parse<expression>(filename, tokens, out.next_token, classes);
          std::copy(arg.messages.begin(), arg.messages.end(),
                    std::back_inserter(out.messages));
          out.next_token = arg.next_token;
          if (!arg.value) {
            return out;
          }
          arguments.push_back(std::move(*arg.value));
          if (tokens.size() == out.next_token) {
            message_builder builder;
            builder.builder << "Unexpected end of index expression!";
            out.messages.push_back(
                builder.build_message(logger::level::FATAL_ERROR,
                                      tokens[out.next_token].token_context));
            return out;
          }
          if (tokens[out.next_token].token_data.token_type !=
                  lexer::token::data::type::OPERATOR_TOKEN ||
              (tokens[out.next_token].token_data.as_operator !=
                   lexer::operators::SQUARE_CLOSE &&
               tokens[out.next_token].token_data.as_operator !=
                   lexer::operators::COMMA)) {
            message_builder builder;
            builder.builder << "Unexpected token in index expression!";
            out.messages.push_back(
                builder.build_message(logger::level::FATAL_ERROR,
                                      tokens[out.next_token].token_context));
            return out;
          }
        } while (tokens[out.next_token].token_data.as_operator ==
                 lexer::operators::COMMA);
        out.next_token++;
        building = std::make_unique<call_expression>(
            call_expression::type::INDEX, std::move(building),
            std::move(arguments));
        break;
      }
      default:
        goto done;
    }
    out.next_token++;
  } while (true);
done:
  if (building) {
    out.value = std::make_unique<postfix_expression>(
        postfix_expression::type::NONE, std::move(building));
  }
  return out;
}

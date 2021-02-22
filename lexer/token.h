#ifndef OOPS_COMPILER_LEXER_TOKEN_H
#define OOPS_COMPILER_LEXER_TOKEN_H

#include <cstdint>

#include "../logger/logging.h"
#include "converters.h"

namespace oops_compiler {
namespace lexer {

struct token {
  struct data {
    enum class type {
      IDENTIFIER_TOKEN,
      KEYWORD_TOKEN,
      OPERATOR_TOKEN,
      LITERAL_TOKEN
    };

    struct string {
      const char *start;
      unsigned size;
    };
    struct literal {
      enum class type { STRING, DOUBLE, FLOAT, INTEGER };
      union {
        string as_string;
        double as_double;
        std::int64_t as_integer;
        float as_float;
        char as_char;
      };
      type literal_type;
    };
    type token_type;
    union {
      string as_identifier;
      literal as_literal;
      operators as_operator;
      keywords as_keyword;
    };
  };

  data token_data;
  logger::context token_context;
  std::string to_string() const;
};
}  // namespace lexer
}  // namespace oops_compiler

#endif
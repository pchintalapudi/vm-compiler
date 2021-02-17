#ifndef OOPS_COMPILER_PARSER_STATEMENT_H
#define OOPS_COMPILER_PARSER_STATEMENT_H

#include <vector>

#include "../lexer/token.h"
#include "symbol_table.h"

namespace oops_compiler {
    namespace parser {
        class statement {
            private:
            public:
            virtual ~statement() = default;
            static std::pair<statement, std::size_t> parse(const std::vector<lexer::token> &tokens, std::size_t start, symbol_table &symbols);
        };
    }
}

#endif
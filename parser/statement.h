#ifndef OOPS_COMPILER_PARSER_STATEMENT_H
#define OOPS_COMPILER_PARSER_STATEMENT_H

#include <vector>

#include "../lexer/token.h"

namespace oops_compiler {
    namespace parser {
        class statement {
            private:
            public:
            virtual ~statement();
            static std::pair<statement, std::size_t> parse(std::vector<lexer::token> tokens, std::size_t start);
        };
    }
}

#endif
#ifndef OOPS_COMPILER_PARSER_STATEMENT_H
#define OOPS_COMPILER_PARSER_STATEMENT_H

#include <vector>

#include "../lexer/token.h"

namespace oops_compiler {
    namespace parser {
        class symbol_table;
        class statement {
            private:
            public:
            virtual ~statement() = default;
        };
    }
}

#endif
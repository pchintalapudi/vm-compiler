#ifndef OOPS_COMPILER_PARSER_STATEMENT_H
#define OOPS_COMPILER_PARSER_STATEMENT_H

#include <vector>

#include "../lexer/token.h"
#include "scope.h"

namespace oops_compiler {
    namespace parser {
        class statement : public parseable<statement> {
            private:
            public:
            virtual ~statement() = default;
        };
    }
}

#endif
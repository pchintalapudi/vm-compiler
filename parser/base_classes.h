#ifndef OOPS_COMPILER_PARSER_BASE_CLASSES_H
#define OOPS_COMPILER_PARSER_BASE_CLASSES_H

#include <vector>

#include "../lexer/token.h"

namespace oops_compiler {
    namespace parser {
        class classloader;
        class scope;
        template<typename node>
        class parseable {
            static node parse(const std::vector<lexer::token> &tokens, std::size_t begin, classloader &loader, scope &scope);
        };
    }
}

#endif
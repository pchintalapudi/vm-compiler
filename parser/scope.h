#ifndef OOPS_COMPILER_PARSER_SCOPE_H
#define OOPS_COMPILER_PARSER_SCOPE_H

#include <string>
#include <unordered_map>

namespace oops_compiler {
    namespace parser {
        class scope {
            public:
            enum class type {
                VARIABLE, METHOD, CLASS, __COUNT__
            };
            private:
            scope* parent;
            std::unordered_map<std::string, type> variables;
            class proxy {
                private:
                std::unordered_map<std::string, type> &variables;
                std::string key;
                public:
                operator type() {
                    if (auto it = variables.find(key); it != variables.end()) {
                        return it->second;
                    } else {
                        return type::__COUNT__;
                    }
                }
            };
            public:

        };
    }
}

#endif
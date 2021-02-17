#ifndef OOPS_COMPILER_PARSER_MODIFIER_H
#define OOPS_COMPILER_PARSER_MODIFIER_H

namespace oops_compiler {
namespace parser {
enum class modifiers { PUBLIC, PROTECTED, PACKAGE, PRIVATE };
enum class storage { STATIC, VIRTUAL };
enum class special { INTRINSIC, NATIVE, NONE };
}  // namespace parser
}  // namespace oops_compiler

#endif
#ifndef OOPS_COMPILER_PARSER_BASE_CLASSES_H
#define OOPS_COMPILER_PARSER_BASE_CLASSES_H

#include <memory>
#include <string>
#include <unordered_set>
#include <variant>
#include <vector>

#include "../lexer/token.h"
#include "../logger/logging.h"

namespace oops_compiler {
namespace parser {
class class_definition;
class classloader;
class type_declaration;
template <typename out_t>
struct output {
  const char *filename;
  std::optional<std::unique_ptr<out_t>> value;
  std::vector<logger::message> messages;
  std::vector<logger::context> contexts;
  std::size_t next_token;
  template <typename in_t>
  static output<out_t> generalize(output<in_t> out) {
    return output<out_t>{.filename = out.filename,
                         .value = std::move(out.value),
                         .messages = std::move(out.messages),
                         .contexts = std::move(out.contexts),
                         .next_token = out.next_token};
  }
  template <typename in_t>
  static output<out_t> reconstruct(output<in_t> out) {
    return output<out_t>{
        .filename = out.filename,
        .value =
            out.value
                ? std::optional<std::unique_ptr<out_t>>{std::make_unique<out_t>(
                      std::move(**out.value))}
                : std::optional<std::unique_ptr<out_t>>{},
        .messages = std::move(out.messages),
        .contexts = std::move(out.contexts),
        .next_token = out.next_token};
  }
};
template <typename node>
output<node> parse(const char *filename, std::vector<lexer::token> &tokens,
                   std::size_t begin, std::unordered_set<std::string> &classes);

typedef std::vector<std::string> package_declaration;

class imported_class {
 private:
  std::string alias;
  package_declaration package;

 public:
  imported_class(std::string alias, package_declaration package)
      : alias(std::move(alias)), package(std::move(package)) {}

  const std::string &get_alias() const { return alias; }
  const package_declaration &get_package() const { return package; }
};
class source_file {
 private:
  const char *filename;
  std::vector<imported_class> imports;
  package_declaration package;
  std::unique_ptr<class_definition> main;

 public:
  source_file(const char *filename, decltype(source_file::imports) imports,
              package_declaration package,
              std::unique_ptr<class_definition> main);
  const char *get_filename() const { return filename; }
  const decltype(source_file::imports) &get_imports() const { return imports; }

  const package_declaration &get_package() const { return package; }
  const class_definition &get_main() const { return *main; }

  output<char> resolve_unparsed_method_definitions();
};
}  // namespace parser
}  // namespace oops_compiler

#endif
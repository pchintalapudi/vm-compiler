#ifndef OOPS_COMPILER_PARSER_BASE_CLASSES_H
#define OOPS_COMPILER_PARSER_BASE_CLASSES_H

#include <string>
#include <variant>
#include <vector>

#include "../lexer/token.h"
#include "../logger/logging.h"

namespace oops_compiler {
namespace parser {
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
};
template <typename node>
output<node> parse(const char *filename,
                   const std::vector<lexer::token> &tokens, std::size_t begin);

typedef std::vector<std::string> package_declaration;
class source_file {
 private:
  const char *filename;
  std::unordered_map<std::string, const type_declaration *> imports;
  package_declaration package;

 public:
  source_file(const char *filename,
              std::unordered_map<std::string, const type_declaration *> imports,
              package_declaration package)
      : filename(filename),
        imports(std::move(imports)),
        package(std::move(package)) {}
  const char *get_filename() const { return filename; }
  const std::unordered_map<std::string, const type_declaration *> &get_imports()
      const {
    return imports;
  }

  const package_declaration &get_package() const { return package; }
};

class imported_class {
 private:
  std::string alias;
  const type_declaration *cls;

 public:
  imported_class(std::string alias, const type_declaration &cls)
      : alias(std::move(alias)), cls(&cls) {}

  const std::string &get_alias() const { return alias; }
  const type_declaration &get_class() const { return *cls; }
};
}  // namespace parser
}  // namespace oops_compiler

#endif
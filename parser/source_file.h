#ifndef OOPS_COMPILER_SOURCE_FILE_H
#define OOPS_COMPILER_SOURCE_FILE_H

#include <memory>
#include <vector>

#include "base.h"
#include "class.h"

namespace oops_compiler {
namespace parser {
struct package_declaration : virtual ast_node,
                             virtual visitable<package_declaration, ast_node> {
  std::vector<std::unique_ptr<identifier>> path;
  logger::context context;
  package_declaration(decltype(path) path, decltype(context) context)
      : path(std::move(path)), context(std::move(context)) {}
};
struct import_declaration : virtual ast_node, virtual visitable<import_declaration, ast_node> {
  std::unique_ptr<identifier> alias;
  std::vector<std::unique_ptr<identifier>> path;
  logger::context context;
  import_declaration(decltype(alias) alias, decltype(path) path,
                     decltype(context) context)
      : alias(std::move(alias)),
        path(std::move(path)),
        context(std::move(context)) {}
};
struct source_file : virtual ast_node, virtual visitable<source_file, ast_node> {
  std::string filename;
  std::unique_ptr<package_declaration> package;
  std::vector<std::unique_ptr<import_declaration>> imports;
  std::unique_ptr<class_definition> cls;
  source_file(decltype(filename) filename, decltype(package) package,
              decltype(imports) imports, decltype(cls) cls)
      : filename(std::move(filename)),
        package(std::move(package)),
        imports(std::move(imports)),
        cls(std::move(cls)) {}
};
}  // namespace parser
}  // namespace oops_compiler

#endif
#include "base.h"
#include "class.h"
#include "output.h"
#include "source_file.h"

using namespace oops_compiler::parser;

namespace {
enum class field_flags {
  STATIC,
  FINAL,
  NATIVE,
  INTRINSIC,
  CONST,
  VAR,
  DEF,
  OPERATOR,
  GET,
  SET,
  CONSTRUCTOR
};
}

template <>
output<std::unique_ptr<package_declaration>>
oops_compiler::parser::parse<package_declaration>(
    token_stream &tokens, std::unordered_set<std::string> &classes) {
  output<std::unique_ptr<package_declaration>> out{{}, {}, true};
  logger::context ctxt = tokens.context();
  if (tokens.pop_specific<lexer::token::data::type::KEYWORD_TOKEN>(
          lexer::keywords::PACKAGE)) {
    std::vector<std::unique_ptr<identifier>> identifiers;
    do {
      output<std::unique_ptr<identifier>> str =
          parse<identifier>(tokens, classes);
      out.merge_messages(str);
      if (str.fatal) {
        return out;
      }
      if (str.value) {
        identifiers.push_back(std::move(*str.value));
      }
    } while (tokens.pop_specific<lexer::token::data::type::OPERATOR_TOKEN>(
        lexer::operators::ACCESS));
    if (identifiers.empty()) {
      out.log(logger::level::ERROR, ctxt,
              "Package declaration must have at least one folder!");
      out.fatal = false;
    }
    out.value = std::make_unique<package_declaration>(std::move(identifiers),
                                                      std::move(ctxt));
    out.fatal = false;
    return out;
  } else {
    out.log(logger::level::ERROR, std::move(ctxt),
            "Class file must begin with a package declaration!");
    out.fatal = false;
    return out;
  }
}

template <>
output<std::unique_ptr<class_variable>>
oops_compiler::parser::parse<class_variable>(
    token_stream &tokens, std::unordered_set<std::string> &classes) {
  output<std::unique_ptr<class_variable>> out{{}, {}, true};
  auto ctxt = tokens.context();
  auto mod = tokens.pop_specific<lexer::token::data::type::KEYWORD_TOKEN>(
      lexer::keywords::PUBLIC, lexer::keywords::PROTECTED,
      lexer::keywords::PACKAGE, lexer::keywords::PRIVATE);
  access modifier;
  if (!mod) {
    out.log(logger::level::ERROR, std::move(ctxt),
            "Expected public, protected, package, or private modifier to "
            "precede variable declaration!");
    modifier = access::PUBLIC;
  } else {
    switch (mod->token_data.as_keyword) {
      case lexer::keywords::PUBLIC:
        modifier = access::PUBLIC;
        break;
      case lexer::keywords::PROTECTED:
        modifier = access::PROTECTED;
        break;
      case lexer::keywords::PACKAGE:
        modifier = access::PACKAGE;
        break;
      case lexer::keywords::PRIVATE:
        modifier = access::PRIVATE;
        break;
      default:
        out.log(logger::level::FATAL_ERROR, mod->token_context,
                "Internal filter error!!!");
        return out;
    }
  }
  bool is_static =
      !!tokens.pop_specific<lexer::token::data::type::KEYWORD_TOKEN>(
          lexer::keywords::STATIC);
  auto const_token =
      tokens.pop_specific<lexer::token::data::type::KEYWORD_TOKEN>(
          lexer::keywords::CONST, lexer::keywords::VAR);
  bool is_const = false;
  if (!const_token) {
    out.log(logger::level::ERROR, tokens.context(),
            "Expected var or const here!");
  } else {
    is_const = const_token->token_data.as_keyword == lexer::keywords::CONST;
  }
  output<std::unique_ptr<identifier>> name = parse<identifier>(tokens, classes);
  out.merge_messages(name);
  if (name.fatal) {
    return out;
  }
  if (!name.value) {
    name.value = std::make_unique<identifier>("", 0, tokens.context());
  }
  std::optional<std::unique_ptr<general_type>> type;
  if (tokens.pop_specific<lexer::token::data::type::OPERATOR_TOKEN>(
          lexer::operators::COLON)) {
    output<std::unique_ptr<general_type>> tp =
        parse<general_type>(tokens, classes);
    out.merge_messages(tp);
    if (tp.fatal) {
      return out;
    }
    if (tp.value) {
      type = std::move(*tp.value);
    } else {
      type.emplace(nullptr);
    }
  } else {
    type.emplace(nullptr);
  }
  std::optional<std::unique_ptr<expression>> default_value;
  if (tokens.pop_specific<lexer::token::data::type::OPERATOR_TOKEN>(
          lexer::operators::EQ)) {
    output<std::unique_ptr<expression>> def =
        parse<expression>(tokens, classes);
    out.merge_messages(def);
    if (def.fatal) {
      return out;
    }
    if (def.value) {
      default_value = std::move(*def.value);
    }
  } else {
    if (!type) {
      out.log(logger::level::ERROR, tokens.context(),
              "Expected variable to have either an initializer or a type!");
    }
  }
  if (!default_value) {
    default_value.emplace(nullptr);
  }
  if (!tokens.pop_specific<lexer::token::data::type::OPERATOR_TOKEN>(
          lexer::operators::SEMICOLON)) {
    out.log(logger::level::ERROR, tokens.context(),
            "Expected semicolon to terminate variable declaration!");
  }
  out.value = std::make_unique<class_variable>(
      modifier, is_static, is_const, std::move(*type), std::move(*name.value),
      std::move(*default_value), ctxt);
  out.fatal = false;
  return out;
}

template <>
output<std::unique_ptr<parameter>> oops_compiler::parser::parse<parameter>(
    token_stream &tokens, std::unordered_set<std::string> &classes) {
  output<std::unique_ptr<parameter>> out{{}, {}, true};
  auto ctxt = tokens.context();
  output<std::unique_ptr<identifier>> name = parse<identifier>(tokens, classes);
  out.merge_messages(name);
  if (name.fatal) {
    return out;
  }
  if (!name.value) {
    return out;
  }
  if (!tokens.pop_specific<lexer::token::data::type::OPERATOR_TOKEN>(
          lexer::operators::COLON)) {
    out.log(logger::level::ERROR, tokens.context(),
            "Expected colon followed by parameter type!");
  }
  output<std::unique_ptr<general_type>> type =
      parse<general_type>(tokens, classes);
  out.merge_messages(type);
  if (type.fatal) {
    return out;
  }
  if (!type.value) {
    return out;
  }
  out.value = std::make_unique<parameter>(
      std::move(*type.value), std::move(*name.value), std::move(ctxt));
  out.fatal = false;
  return out;
}

template <>
output<std::unique_ptr<class_method>>
oops_compiler::parser::parse<class_method>(
    token_stream &tokens, std::unordered_set<std::string> &classes) {
  output<std::unique_ptr<class_method>> out{{}, {}, true};
  auto ctxt = tokens.context();
  auto mod = tokens.pop_specific<lexer::token::data::type::KEYWORD_TOKEN>(
      lexer::keywords::PUBLIC, lexer::keywords::PROTECTED,
      lexer::keywords::PACKAGE, lexer::keywords::PRIVATE);
  access modifier;
  if (!mod) {
    out.log(logger::level::ERROR, std::move(ctxt),
            "Expected public, protected, package, or private modifier to "
            "precede method declaration!");
    modifier = access::PUBLIC;
  } else {
    switch (mod->token_data.as_keyword) {
      case lexer::keywords::PUBLIC:
        modifier = access::PUBLIC;
        break;
      case lexer::keywords::PROTECTED:
        modifier = access::PROTECTED;
        break;
      case lexer::keywords::PACKAGE:
        modifier = access::PACKAGE;
        break;
      case lexer::keywords::PRIVATE:
        modifier = access::PRIVATE;
        break;
      default:
        out.log(logger::level::FATAL_ERROR, mod->token_context,
                "Internal filter error!!!");
        return out;
    }
  }
  bool is_static =
      !!tokens.pop_specific<lexer::token::data::type::KEYWORD_TOKEN>(
          lexer::keywords::STATIC);
  bool is_final =
      !!tokens.pop_specific<lexer::token::data::type::KEYWORD_TOKEN>(
          lexer::keywords::FINAL);
  bool is_native =
      !!tokens.pop_specific<lexer::token::data::type::KEYWORD_TOKEN>(
          lexer::keywords::NATIVE);
  bool is_intrinsic =
      !!tokens.pop_specific<lexer::token::data::type::KEYWORD_TOKEN>(
          lexer::keywords::INTRINSIC);
  if (is_native && is_intrinsic) {
    out.log(logger::level::ERROR, tokens.context(),
            "Method cannot be both native and intrinsic!");
  }
  if (is_static && is_final) {
    out.log(logger::level::WARNING, tokens.context(),
            "Static method is implicitly final!");
  }
  while (tokens.pop_specific<lexer::token::data::type::KEYWORD_TOKEN>(
      lexer::keywords::STATIC, lexer::keywords::FINAL, lexer::keywords::NATIVE,
      lexer::keywords::INTRINSIC)) {
    out.log(
        logger::level::ERROR, tokens.context(),
        "Modifiers must come in the order `public|protected|package|private "
        "static|final? native|intrinsic?`!");
  }
  std::optional<lexer::token> method_type;
  while (tokens.size() &&
         !(method_type =
               tokens.pop_specific<lexer::token::data::type::KEYWORD_TOKEN>(
                   lexer::keywords::CONSTRUCTOR, lexer::keywords::GET,
                   lexer::keywords::OPERATOR, lexer::keywords::SET,
                   lexer::keywords::DEF))) {
    out.log(logger::level::ERROR, tokens.context(),
            "Expected constructor/def/get/operator/set here!");
    tokens.pop();
  }
  if (!method_type) {
    out.log(logger::level::FATAL_ERROR, tokens.context(),
            "Unexpected end of method declaration!");
    return out;
  }
  switch (method_type->token_data.as_keyword) {
    case lexer::keywords::OPERATOR: {
      out.log(logger::level::FATAL_ERROR, method_type->token_context,
              "I'm tired!");
      return out;
    }
    case lexer::keywords::CONSTRUCTOR: {
      if (is_static) {
        out.log(logger::level::ERROR, method_type->token_context,
                "Constructor cannot be static!");
      }
      if (is_final) {
        out.log(logger::level::WARNING, method_type->token_context,
                "Constructor is implicitly final!");
      }
      tokens.push(lexer::token{
          lexer::token::data{
              .token_type = lexer::token::data::type::IDENTIFIER_TOKEN,
              .as_identifier = {"", 0}},
          tokens.context()});
      [[fallthrough]];
    }
    case lexer::keywords::DEF:
    case lexer::keywords::GET:
    case lexer::keywords::SET: {
      output<std::unique_ptr<type_declaration>> method_name =
          parse<type_declaration>(tokens, classes);
      out.merge_messages(method_name);
      if (method_name.fatal) {
        return out;
      }
      if (!method_name.value) {
        out.log(logger::level::FATAL_ERROR, tokens.context(),
                "Unable to parse method name!");
        return out;
      }
      std::vector<std::unique_ptr<parameter>> parameters;
      while (tokens.size() &&
             !tokens.pop_specific<lexer::token::data::type::OPERATOR_TOKEN>(
                 lexer::operators::ROUND_OPEN)) {
        out.log(logger::level::ERROR, tokens.context(),
                "Expected to see start of parameter list here!");
        tokens.pop();
      }
      if (!tokens.size()) {
        out.log(logger::level::FATAL_ERROR, tokens.context(),
                "Unexpected end of method declaration!");
        return out;
      }
      do {
        output<std::unique_ptr<parameter>> param =
            parse<parameter>(tokens, classes);
        out.merge_messages(param);
        if (param.fatal) {
          return out;
        }
        if (param.value) {
          parameters.push_back(std::move(*param.value));
        }
      } while (tokens.pop_specific<lexer::token::data::type::OPERATOR_TOKEN>(
          lexer::operators::COMMA));
      if (!tokens.pop_specific<lexer::token::data::type::OPERATOR_TOKEN>(
              lexer::operators::ROUND_CLOSE)) {
        out.log(logger::level::ERROR, tokens.context(),
                "Expected end of parameter declaration here!");
      }
      std::unique_ptr<general_type> return_type;
      if (method_type->token_data.as_keyword != lexer::keywords::CONSTRUCTOR) {
        if (!tokens.pop_specific<lexer::token::data::type::OPERATOR_TOKEN>(
                lexer::operators::CAST)) {
          out.log(logger::level::ERROR, tokens.context(),
                  "Expected method return type here!");
        } else {
          output<std::unique_ptr<general_type>> ret =
              parse<general_type>(tokens, classes);
          out.merge_messages(ret);
          if (ret.fatal) {
            return out;
          }
          if (ret.value) {
            return_type = std::move(*ret.value);
          }
        }
      }
      std::optional<lexer::token> body_start;
      while (
          tokens.size() &&
          !(body_start =
                tokens.pop_specific<lexer::token::data::type::OPERATOR_TOKEN>(
                    lexer::operators::SEMICOLON,
                    lexer::operators::CURLY_OPEN))) {
        out.log(logger::level::ERROR, tokens.context(),
                "Expected a semicolon or method body beginning here!");
        tokens.pop();
      }
      if (!tokens.size()) {
        out.log(logger::level::FATAL_ERROR, tokens.context(),
                "Unexpected end of method!");
        return out;
      }
      bool is_abstract =
          body_start->token_data.as_operator == lexer::operators::SEMICOLON;
      token_stream method_stream;
      if (!is_abstract) {
        method_stream.enqueue(*body_start);
        std::size_t depth = 1;
        do {
          auto token = tokens.pop();
          if (!token) {
            out.log(logger::level::FATAL_ERROR, tokens.context(),
                    "Unexpected end of method!");
            return out;
          }
          if (token->token_data.token_type ==
              lexer::token::data::type::OPERATOR_TOKEN) {
            depth +=
                token->token_data.as_operator == lexer::operators::CURLY_OPEN;
            depth -=
                token->token_data.as_operator == lexer::operators::CURLY_CLOSE;
          }
          method_stream.enqueue(*body_start);
        } while (depth);
      }
      out.value = std::make_unique<class_method>(
          std::move(return_type), std::move(*method_name.value),
          std::move(parameters), std::move(method_stream), modifier, is_static,
          is_native, is_intrinsic, is_abstract, std::move(ctxt));
      out.fatal = false;
      return out;
    }
    default:
      out.log(logger::level::FATAL_ERROR, mod->token_context,
              "Internal filter error!!!");
      return out;
  }
}

template <>
output<std::unique_ptr<class_definition>>
oops_compiler::parser::parse<class_definition>(
    token_stream &tokens, std::unordered_set<std::string> &classes) {
  output<std::unique_ptr<class_definition>> out{{}, {}, true};
  auto ctxt = tokens.context();
  if (!tokens.pop_specific<lexer::token::data::type::KEYWORD_TOKEN>(
          lexer::keywords::CLASS)) {
    out.log(logger::level::ERROR, std::move(ctxt),
            "Expected class to start here!");
    out.fatal = false;
    return out;
  }
  output<std::unique_ptr<type_declaration>> declaration =
      parse<type_declaration>(tokens, classes);
  out.merge_messages(declaration);
  if (declaration.fatal) {
    return out;
  }
  if (!declaration.value) {
    out.fatal = false;
    return out;
  }
  std::unique_ptr<general_type> superclass;
  if (tokens.pop_specific<lexer::token::data::type::KEYWORD_TOKEN>(
          lexer::keywords::EXTENDS)) {
    output<std::unique_ptr<general_type>> sup =
        parse<general_type>(tokens, classes);
    out.merge_messages(sup);
    if (sup.fatal) {
      return out;
    }
    if (!sup.value) {
      out.fatal = false;
      return out;
    }
    superclass = std::move(*sup.value);
  }
  std::vector<std::unique_ptr<general_type>> implemented;
  if (tokens.pop_specific<lexer::token::data::type::KEYWORD_TOKEN>(
          lexer::keywords::IMPLEMENTS)) {
    do {
      output<std::unique_ptr<general_type>> cls =
          parse<general_type>(tokens, classes);
      out.merge_messages(cls);
      if (cls.fatal) {
        return out;
      }
      if (cls.value) {
        implemented.push_back(std::move(*cls.value));
      }
    } while (tokens.pop_specific<lexer::token::data::type::OPERATOR_TOKEN>(
        lexer::operators::COMMA));
  }
  if (!tokens.pop_specific<lexer::token::data::type::OPERATOR_TOKEN>(
          lexer::operators::CURLY_OPEN)) {
    out.log(logger::level::ERROR, tokens.context(),
            "Expected opening brace after implemented class declaration!");
    if (!tokens.pop_until<lexer::token::data::type::OPERATOR_TOKEN>(
            lexer::operators::CURLY_OPEN)) {
      out.log(logger::level::FATAL_ERROR, tokens.context(),
              "Unexpected end of class declaration!");
    }
  }
  std::vector<std::unique_ptr<class_definition>> inner_classes;
  std::vector<std::unique_ptr<class_variable>> variables;
  std::vector<std::unique_ptr<class_method>> methods;
  std::vector<lexer::token> lookahead;
  do {
    if (!tokens.size()) {
      out.log(logger::level::FATAL_ERROR, tokens.context(),
              "Unexpected end of class definition!");
      return out;
    }
    auto cls = tokens.pop_specific<lexer::token::data::type::KEYWORD_TOKEN>(
        lexer::keywords::CLASS);
    if (cls) {
      tokens.push(std::move(*cls));
      output<std::unique_ptr<class_definition>> cdef =
          parse<class_definition>(tokens, classes);
      out.merge_messages(cdef);
      if (cdef.fatal) {
        return out;
      }
      if (cdef.value) {
        inner_classes.push_back(std::move(*cdef.value));
      }
      continue;
    }
    bool parse_variable = false;
    auto modifier =
        tokens.pop_specific<lexer::token::data::type::KEYWORD_TOKEN>(
            lexer::keywords::PUBLIC, lexer::keywords::PROTECTED,
            lexer::keywords::PACKAGE, lexer::keywords::PRIVATE);
    if (modifier) lookahead.push_back(std::move(*modifier));
    auto is_static =
        tokens.pop_specific<lexer::token::data::type::KEYWORD_TOKEN>(
            lexer::keywords::STATIC);
    if (is_static) lookahead.push_back(std::move(*is_static));
    auto is_final =
        tokens.pop_specific<lexer::token::data::type::KEYWORD_TOKEN>(
            lexer::keywords::FINAL);
    if (!is_final) {
      auto var = tokens.pop_specific<lexer::token::data::type::KEYWORD_TOKEN>(
          lexer::keywords::VAR, lexer::keywords::CONST);
      if (var) {
        lookahead.push_back(std::move(*var));
        parse_variable = true;
      }
    } else {
      lookahead.push_back(std::move(*is_final));
    }
    while (!lookahead.empty()) {
      tokens.push(std::move(lookahead.back()));
      lookahead.pop_back();
    }
    if (parse_variable) {
      output<std::unique_ptr<class_variable>> variable =
          parse<class_variable>(tokens, classes);
      out.merge_messages(variable);
      if (variable.fatal) {
        return out;
      }
      if (variable.value) {
        variables.push_back(std::move(*variable.value));
      }
    } else {
      output<std::unique_ptr<class_method>> method =
          parse<class_method>(tokens, classes);
      out.merge_messages(method);
      if (method.fatal) {
        return out;
      }
      if (method.value) {
        methods.push_back(std::move(*method.value));
      }
    }
  } while (!tokens.pop_specific<lexer::token::data::type::OPERATOR_TOKEN>(
      lexer::operators::CURLY_CLOSE));
  if (tokens.size()) {
    out.log(logger::level::ERROR, tokens.context(),
            "Additional code found after conclusion of class definition!");
  }
  out.value = std::make_unique<class_definition>(
      std::move(*declaration.value), std::move(variables), std::move(methods),
      std::move(inner_classes), ctxt);
  out.fatal = false;
  return out;
}

template <>
output<std::unique_ptr<import_declaration>>
oops_compiler::parser::parse<import_declaration>(
    token_stream &tokens, std::unordered_set<std::string> &classes) {
  output<std::unique_ptr<import_declaration>> out{{}, {}, true};
  auto ctxt = tokens.context();
  if (tokens.pop_specific<lexer::token::data::type::KEYWORD_TOKEN>(
          lexer::keywords::IMPORT)) {
    std::vector<std::unique_ptr<identifier>> identifiers;
    do {
      output<std::unique_ptr<identifier>> str =
          parse<identifier>(tokens, classes);
      out.merge_messages(str);
      if (str.fatal) {
        return out;
      }
      if (str.value) {
        identifiers.push_back(std::move(*str.value));
      }
    } while (tokens.pop_specific<lexer::token::data::type::OPERATOR_TOKEN>(
        lexer::operators::ACCESS));
    if (identifiers.empty()) {
      out.log(logger::level::ERROR, std::move(ctxt),
              "Import must have at least one identifier!");
      out.fatal = false;
      return out;
    }
    std::unique_ptr<identifier> alias;
    if (tokens.pop_specific<lexer::token::data::type::KEYWORD_TOKEN>(
            lexer::keywords::AS)) {
      output<std::unique_ptr<identifier>> ident =
          parse<identifier>(tokens, classes);
      out.merge_messages(ident);
      if (ident.fatal) {
        return out;
      }
      if (ident.value) {
        alias = std::move(*ident.value);
      } else {
        alias = std::make_unique<identifier>(*identifiers.back());
      }
    } else {
      alias = std::make_unique<identifier>(*identifiers.back());
    }
    if (!tokens.pop_specific<lexer::token::data::type::OPERATOR_TOKEN>(
            lexer::operators::SEMICOLON)) {
      out.log(logger::level::ERROR, ctxt,
              "Import statement must end in semicolon!");
      out.fatal = false;
    }
    out.value = std::make_unique<import_declaration>(
        std::move(alias), std::move(identifiers), std::move(ctxt));
    out.fatal = false;
    return out;
  } else {
    out.log(logger::level::ERROR, std::move(ctxt),
            "Failed to find the import keyword at start of token stream!");
    out.fatal = false;
    return out;
  }
}

template <>
output<std::unique_ptr<source_file>> oops_compiler::parser::parse<source_file>(
    token_stream &tokens, std::unordered_set<std::string> &classes) {
  output<std::unique_ptr<source_file>> out{
      .messages = {}, .value = {}, .fatal = true};
  output<std::unique_ptr<package_declaration>> package =
      parse<package_declaration>(tokens, classes);
  out.merge_messages(package);
  if (package.fatal) {
    return out;
  }
  std::vector<std::unique_ptr<import_declaration>> imports;
  std::optional<lexer::token> next;
  do {
    next = tokens.pop_specific<lexer::token::data::type::KEYWORD_TOKEN>(
        lexer::keywords::IMPORT, lexer::keywords::CLASS);
    if (next && next->token_data.as_keyword == lexer::keywords::IMPORT) {
      tokens.push(*next);
      output<std::unique_ptr<import_declaration>> imp =
          parse<import_declaration>(tokens, classes);
      out.merge_messages(imp);
      if (imp.fatal) {
        return out;
      }
      if (imp.value) {
        imports.push_back(std::move(*imp.value));
      }
    }
  } while (next && next->token_data.as_keyword == lexer::keywords::IMPORT);
  if (!next) {
    out.log(logger::level::FATAL_ERROR, tokens.context());
    return out;
  }
  for (const auto &imp : imports) {
    classes.insert(imp->alias->str());
  }
  output<std::unique_ptr<class_definition>> cls =
      parse<class_definition>(tokens, classes);
  out.merge_messages(cls);
  if (cls.fatal || !cls.value) {
    return out;
  }
  if (!package.value) {
    package.value = std::make_unique<package_declaration>(
        std::vector<std::unique_ptr<identifier>>{}, logger::context::NONE());
  }
  out.value = std::make_unique<source_file>(
      "", std::move(*package.value), std::move(imports), std::move(*cls.value));
  return out;
}
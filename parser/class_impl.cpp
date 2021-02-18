#include "basic_block.h"
#include "class.h"
#include "classloader.h"
#include "impl_common.h"
#include "method.h"
#include "variable.h"

using namespace oops_compiler::parser;

std::vector<oops_compiler::logger::message>
class_definition::resolve_definitions(classloader &loader) {
  std::vector<oops_compiler::logger::message> out;
  for (std::size_t i = 0; i < this->mtds.size(); i++) {
    auto &mtd = static_cast<unparsed_method_declaration &>(*this->mtds[i]);
    auto bb = parse<basic_block>(this->source->get_filename(), mtd.get_tokens(),
                                 0, loader, mtd.get_scope());
    std::copy(bb.messages.begin(), bb.messages.end(), std::back_inserter(out));
    if (!bb.value) {
      return out;
    }
    this->mtds[i] = std::make_unique<method_definition>(std::move(mtd),
                                                        std::move(**bb.value));
  }
  return out;
}

parse_decl(class_definition);
parse_decl(unparsed_method_declaration);
parse_decl(variable);
parse_decl(source_file);

parse_decl(variable) {
  output<variable> out;
  out.filename = filename;
  out.contexts.push_back(tokens[begin].token_context);
  modifiers mods;
  storage store;
  bool fin;
  if (tokens.size() - begin < 3) {
    message_builder builder;
    builder.builder << "Insufficient tokens to parse variable declaration!";
    out.messages.push_back(builder.build_message(logger::level::FATAL_ERROR,
                                                 tokens[begin].token_context));
    out.next_token = begin;
    return out;
  }
  switch (tokens[begin].token_data.as_keyword) {
    case lexer::keywords::PUBLIC:
      mods = modifiers::PUBLIC;
      break;
    case lexer::keywords::PROTECTED:
      mods = modifiers::PROTECTED;
      break;
    case lexer::keywords::PACKAGE:
      mods = modifiers::PACKAGE;
      break;
    case lexer::keywords::PRIVATE:
      mods = modifiers::PRIVATE;
      break;
    default:
      return out;
  }
  begin++;
  if (tokens[begin].token_data.token_type ==
          lexer::token::data::type::KEYWORD_TOKEN &&
      tokens[begin].token_data.as_keyword == lexer::keywords::STATIC) {
    store = storage::STATIC;
    begin++;
  } else {
    store = storage::VIRTUAL;
  }
  if (tokens[begin].token_data.token_type ==
          lexer::token::data::type::KEYWORD_TOKEN &&
      tokens[begin].token_data.as_keyword == lexer::keywords::FINAL) {
    store = storage::STATIC;
    begin++;
  } else {
    store = storage::VIRTUAL;
  }
  auto decl = parse<declaration>(filename, tokens, begin, loader, scope);
  std::copy(decl.messages.begin(), decl.messages.end(),
            std::back_inserter(out.messages));
  out.next_token = decl.next_token;
  if (!decl.value) {
    return out;
  }
  out.value =
      std::make_unique<variable>(mods, store, fin, std::move(**decl.value));
  return out;
}

parse_decl(package_declaration) {
  (void)loader;
  (void)scope;
  output<package_declaration> out;
  out.filename = filename;
  out.contexts.push_back(tokens[begin].token_context);
  std::unique_ptr<package_declaration> package =
      std::make_unique<package_declaration>();
  do {
    begin++;
    if (tokens.size() == begin) {
      message_builder builder;
      builder.builder
          << "Unexpected end of file while parsing package declaration!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[begin].token_context));
      return out;
    }
    if (tokens[begin].token_data.token_type !=
        lexer::token::data::type::DEFERRED_TOKEN) {
      message_builder builder;
      builder.builder << "Package folders must not be strings, numbers, "
                         "operators, or keywords!";
      out.messages.push_back(builder.build_message(
          logger::level::ERROR, tokens[begin].token_context));
      begin--;
      break;
    }
    package->push_back(std::string(tokens[begin].token_data.as_deferred.start,
                                   tokens[begin].token_data.as_deferred.size));
    begin++;
    if (tokens.size() == begin) {
      message_builder builder;
      builder.builder
          << "Unexpected end of file while parsing package declaration!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[begin].token_context));
      return out;
    }
    if (tokens[begin].token_data.token_type !=
            lexer::token::data::type::OPERATOR_TOKEN ||
        (tokens[begin].token_data.as_operator != lexer::operators::ACCESS &&
         tokens[begin].token_data.as_operator != lexer::operators::SEMICOLON)) {
      message_builder builder;
      builder.builder << "Package declaration must consist of identifiers "
                         "separated by periods ending in a semicolon!";
      out.messages.push_back(builder.build_message(
          logger::level::ERROR, tokens[begin].token_context));
      begin--;
      break;
    }
  } while (tokens[begin].token_data.as_operator == lexer::operators::ACCESS);
  begin++;
  out.next_token = begin;
  out.value = std::move(package);
  return out;
}

parse_decl(imported_class) {
  output<imported_class> out;
  out.filename = filename;
  out.contexts.push_back(tokens[begin].token_context);
  std::vector<std::string> path;
  std::string alias;
  do {
    begin++;
    if (tokens.size() == begin) {
      message_builder builder;
      builder.builder
          << "Unexpected end of file while parsing package declaration!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[begin].token_context));
      return out;
    }
    if (tokens[begin].token_data.token_type !=
        lexer::token::data::type::DEFERRED_TOKEN) {
      message_builder builder;
      builder.builder << "Import folders must not be strings, numbers, "
                         "operators, or keywords!";
      out.messages.push_back(builder.build_message(
          logger::level::ERROR, tokens[begin].token_context));
      begin--;
      break;
    }
    path.push_back(std::string(tokens[begin].token_data.as_deferred.start,
                               tokens[begin].token_data.as_deferred.size));
    begin++;
    if (tokens.size() == begin) {
      message_builder builder;
      builder.builder
          << "Unexpected end of file while parsing import declaration!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[begin].token_context));
      return out;
    }
    switch (tokens[begin].token_data.token_type) {
      case lexer::token::data::type::OPERATOR_TOKEN: {
        switch (tokens[begin].token_data.as_operator) {
          case lexer::operators::ACCESS:
          case lexer::operators::SEMICOLON:
            break;
          default: {
            goto nonidentifier;
          }
        }
        break;
      }
      case lexer::token::data::type::KEYWORD_TOKEN: {
        switch (tokens[begin].token_data.as_keyword) {
          case lexer::keywords::AS:
            break;
          default: {
            goto nonidentifier;
          }
        }
        break;
      }
      default: {
        goto nonidentifier;
      }
    }
  } while (tokens[begin].token_data.as_operator == lexer::operators::ACCESS);
  {
    if (tokens[begin].token_data.token_type ==
        lexer::token::data::type::KEYWORD_TOKEN) {
      if (tokens.size() - begin < 2) {
        message_builder builder;
        builder.builder
            << "Unexpected end of file while parsing import declaration!";
        out.messages.push_back(builder.build_message(
            logger::level::FATAL_ERROR, tokens[begin].token_context));
        return out;
      }
      begin++;
      if (tokens[begin].token_data.token_type !=
          lexer::token::data::type::DEFERRED_TOKEN) {
        message_builder builder;
        builder.builder << "Import alias must not be a string, number, "
                           "operator, or keyword!";
        out.messages.push_back(builder.build_message(
            logger::level::ERROR, tokens[begin].token_context));
        begin--;
      } else {
        alias = std::string(tokens[begin].token_data.as_deferred.start,
                            tokens[begin].token_data.as_deferred.size);
        begin++;
      }
    }
    if (alias.empty()) {
      alias = path.back();
    }
    begin++;
    if (tokens[begin].token_data.token_type ==
            lexer::token::data::type::OPERATOR_TOKEN &&
        tokens[begin].token_data.as_operator == lexer::operators::SEMICOLON) {
      goto complete;
    }
  }
nonidentifier : {
  message_builder builder;
  builder.builder << "Import declaration must consist of identifiers "
                     "separated by periods ending in a semicolon!";
  out.messages.push_back(
      builder.build_message(logger::level::ERROR, tokens[begin].token_context));
  out.next_token = begin;
  if (path.empty()) {
    return out;
  }
}
complete:
  scope.add_class(std::move(path), alias);
  out.next_token = begin;
  out.value = std::make_unique<imported_class>(
      alias, *loader.lookup_class(scope.get_class(alias)));
  return out;
}

parse_decl(source_file) {
  output<source_file> out;
  out.filename = filename;
  package_declaration package;
  if (tokens[begin].token_data.token_type !=
          lexer::token::data::type::KEYWORD_TOKEN ||
      tokens[begin].token_data.as_keyword != lexer::keywords::PACKAGE) {
    message_builder builder;
    builder.builder
        << "Package declaration must be the first line in source file!";
    out.messages.push_back(builder.build_message(logger::level::ERROR,
                                                 tokens[begin].token_context));
  } else {
    auto pack =
        parse<package_declaration>(filename, tokens, begin, loader, scope);
    std::copy(pack.messages.begin(), pack.messages.end(),
              std::back_inserter(out.messages));
    begin = out.next_token = pack.next_token;
    if (!pack.value) {
      return out;
    }
    package = std::move(**pack.value);
    if (tokens.size() == begin) {
      message_builder builder;
      builder.builder << "No class definition found!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[begin].token_context));
      return out;
    }
  }
  std::unordered_map<std::string, const type_declaration *> imports;
  while (tokens[begin].token_data.token_type ==
             lexer::token::data::type::KEYWORD_TOKEN &&
         tokens[begin].token_data.as_keyword == lexer::keywords::IMPORT) {
    auto imp = parse<imported_class>(filename, tokens, begin, loader, scope);
    std::copy(imp.messages.begin(), imp.messages.end(),
              std::back_inserter(out.messages));
    begin = out.next_token = imp.next_token;
    if (!imp.value) {
      return out;
    }
    imports[(*imp.value)->get_alias()] = &(*imp.value)->get_class();
    if (tokens.size() == begin) {
      message_builder builder;
      builder.builder << "No class definition found!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[begin].token_context));
      return out;
    }
  }
  while (tokens[begin].token_data.token_type !=
             lexer::token::data::type::KEYWORD_TOKEN ||
         tokens[begin].token_data.as_keyword != lexer::keywords::CLASS) {
    message_builder builder;
    builder.builder << "Expected to find class definition here!";
    out.messages.push_back(builder.build_message(logger::level::ERROR,
                                                 tokens[begin].token_context));
    begin++;
    if (tokens.size() == begin) {
      message_builder builder;
      builder.builder << "No class definition found!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[begin].token_context));
      return out;
    }
  }
  auto cls = parse<class_definition>(filename, tokens, begin, loader, scope);
  std::copy(cls.messages.begin(), cls.messages.end(),
            std::back_inserter(out.messages));
  begin = out.next_token = cls.next_token;
  if (!cls.value) {
    return out;
  }
  if (begin < tokens.size()) {
    message_builder builder;
    builder.builder << "Additional tokens exist beyond class definition!";
    out.messages.push_back(builder.build_message(logger::level::FATAL_ERROR,
                                                 tokens[begin].token_context));
  }
  out.value = std::make_unique<source_file>(filename, std::move(imports),
                                            std::move(package));
}
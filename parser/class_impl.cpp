#include "basic_block.h"
#include "class.h"
#include "classloader.h"
#include "impl_common.h"
#include "method.h"
#include "variable.h"

using namespace oops_compiler::parser;

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
  auto decl = parse<declaration>(filename, tokens, begin, classes);
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
  (void)classes;
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
  std::vector<std::string> package;
  do {
    begin++;
    if (tokens.size() == begin) {
      message_builder builder;
      builder.builder
          << "Unexpected end of file while parsing import declaration!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[begin].token_context));
      return out;
    }
    if (tokens[begin].token_data.token_type !=
        lexer::token::data::type::DEFERRED_TOKEN) {
      message_builder builder;
      builder.builder << "import folders must not be strings, numbers, "
                         "operators, or keywords!";
      out.messages.push_back(builder.build_message(
          logger::level::ERROR, tokens[begin].token_context));
      begin--;
      break;
    }
    package.push_back(std::string(tokens[begin].token_data.as_deferred.start,
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
    if ((tokens[begin].token_data.token_type !=
             lexer::token::data::type::OPERATOR_TOKEN ||
         (tokens[begin].token_data.as_operator != lexer::operators::ACCESS &&
          tokens[begin].token_data.as_operator !=
              lexer::operators::SEMICOLON)) &&
        (tokens[begin].token_data.token_type !=
             lexer::token::data::type::KEYWORD_TOKEN ||
         tokens[begin].token_data.as_keyword != lexer::keywords::AS)) {
      message_builder builder;
      builder.builder << "Import declaration must consist of identifiers "
                         "separated by periods ending in a semicolon!";
      out.messages.push_back(builder.build_message(
          logger::level::ERROR, tokens[begin].token_context));
      begin--;
      break;
    }
  } while (tokens[begin].token_data.as_operator == lexer::operators::ACCESS);
  begin++;
  out.next_token = begin;
  if (package.empty()) {
    message_builder builder;
    builder.builder << "Imports must have at least one identifier!";
    out.messages.push_back(builder.build_message(logger::level::FATAL_ERROR,
                                                 tokens[begin].token_context));
    return out;
  }
  std::string alias = package.back();
  if (tokens[begin].token_data.token_type ==
          lexer::token::data::type::KEYWORD_TOKEN &&
      tokens[begin].token_data.as_keyword == lexer::keywords::AS) {
    begin++;
    out.next_token = begin;
    if (tokens.size() == begin) {
      message_builder builder;
      builder.builder
          << "Unexpected end of file while parsing import declaration!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[begin].token_context));
      return out;
    }
    if (tokens[begin].token_data.token_type !=
        lexer::token::data::type::DEFERRED_TOKEN) {
      message_builder builder;
      builder.builder << "Expected import alias to be an identifier!";
      out.messages.push_back(builder.build_message(
          logger::level::ERROR, tokens[begin].token_context));
    } else {
      alias = std::string(tokens[begin].token_data.as_deferred.start,
                          tokens[begin].token_data.as_deferred.size);
      begin++;
      out.next_token = begin;
    }
  }
  if (tokens.size() == begin) {
    message_builder builder;
    builder.builder
        << "Unexpected end of file while parsing import declaration!";
    out.messages.push_back(builder.build_message(logger::level::FATAL_ERROR,
                                                 tokens[begin].token_context));
    return out;
  }
  if (tokens[begin].token_data.token_type !=
          lexer::token::data::type::OPERATOR_TOKEN ||
      tokens[begin].token_data.as_operator != lexer::operators::SEMICOLON) {
    message_builder builder;
    builder.builder << "Import declaration must consist of identifiers "
                       "separated by periods ending in a semicolon!";
    out.messages.push_back(builder.build_message(logger::level::ERROR,
                                                 tokens[begin].token_context));
  } else {
    begin++;
    out.next_token = begin;
  }
  classes.insert(alias);
  out.value =
      std::make_unique<imported_class>(std::move(alias), std::move(package));
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
    auto pack = parse<package_declaration>(filename, tokens, begin, classes);
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
  std::vector<imported_class> imports;
  while (tokens[begin].token_data.token_type ==
             lexer::token::data::type::KEYWORD_TOKEN &&
         tokens[begin].token_data.as_keyword == lexer::keywords::IMPORT) {
    auto imp = parse<imported_class>(filename, tokens, begin, classes);
    std::copy(imp.messages.begin(), imp.messages.end(),
              std::back_inserter(out.messages));
    begin = out.next_token = imp.next_token;
    if (!imp.value) {
      return out;
    }
    imports.push_back(std::move(**imp.value));
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
  if (tokens.size() == begin + 1) {
    message_builder builder;
    builder.builder << "No class definition found!";
    out.messages.push_back(builder.build_message(logger::level::FATAL_ERROR,
                                                 tokens[begin].token_context));
    return out;
  }
  if (tokens[begin + 1].token_data.token_type !=
      lexer::token::data::type::DEFERRED_TOKEN) {
    message_builder builder;
    builder.builder << "Class name must be an identifier!";
    out.messages.push_back(builder.build_message(logger::level::FATAL_ERROR,
                                                 tokens[begin].token_context));
    return out;
  }
  classes.emplace(tokens[begin + 1].token_data.as_deferred.start,
                  tokens[begin + 1].token_data.as_deferred.size);
  auto cls = parse<class_definition>(filename, tokens, begin, classes);
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

parse_decl(unparsed_method_declaration) {
  output<unparsed_method_declaration> out;
  out.filename = filename;
  out.next_token = begin;
  out.contexts.push_back(tokens[out.next_token].token_context);
  modifiers mods;
  switch (tokens[out.next_token].token_data.as_keyword) {
    case lexer::keywords::PRIVATE:
      mods = modifiers::PRIVATE;
      break;
    case lexer::keywords::PACKAGE:
      mods = modifiers::PACKAGE;
      break;
    case lexer::keywords::PROTECTED:
      mods = modifiers::PROTECTED;
      break;
    case lexer::keywords::PUBLIC:
      mods = modifiers::PUBLIC;
      break;
    default: {
      message_builder builder;
      builder.builder << "Unexpected error in type instantiation (unknown "
                         "token appeared)!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
      return out;
    }
  }
  out.next_token++;
  if (tokens.size() == out.next_token) {
    message_builder builder;
    builder.builder << "Unexpected end of method declaration!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
    return out;
  }
  storage store = storage::VIRTUAL;
  bool fin = false;
  if (tokens[out.next_token].token_data.token_type !=
      lexer::token::data::type::KEYWORD_TOKEN) {
    message_builder builder;
    builder.builder << "Expected one of 'static', 'final', 'get', 'set', "
                       "'operator', or 'def' in method declaration!";
    out.messages.push_back(builder.build_message(
        logger::level::ERROR, tokens[out.next_token].token_context));
  }
  switch (tokens[out.next_token].token_data.as_keyword) {
    case lexer::keywords::STATIC:
      store = storage::STATIC;
      out.next_token++;
      break;
    case lexer::keywords::FINAL:
      fin = true;
      out.next_token++;
      break;
    default:
      break;
  }
  out.next_token++;
  if (tokens.size() == out.next_token) {
    message_builder builder;
    builder.builder << "Unexpected end of method declaration!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
    return out;
  }
  special spec = special::NONE;
  if (tokens[out.next_token].token_data.token_type !=
      lexer::token::data::type::KEYWORD_TOKEN) {
    message_builder builder;
    builder.builder << "Expected one of 'static', 'final', 'get', 'set', "
                       "'operator', or 'def' in method declaration!";
    out.messages.push_back(builder.build_message(
        logger::level::ERROR, tokens[out.next_token].token_context));
  }
  switch (tokens[out.next_token].token_data.as_keyword) {
    case lexer::keywords::NATIVE:
      spec = special::NATIVE;
      out.next_token++;
      break;
    case lexer::keywords::INTRINSIC:
      spec = special::INTRINSIC;
      out.next_token++;
      break;
    default:
      break;
  }
  if (tokens.size() == out.next_token) {
    message_builder builder;
    builder.builder << "Unexpected end of method declaration!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
    return out;
  }
  if (tokens[out.next_token].token_data.token_type !=
      lexer::token::data::type::KEYWORD_TOKEN) {
    message_builder builder;
    builder.builder << "Expected one of 'get', 'set', 'operator', or 'def' in "
                       "method declaration!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
    return out;
  }
  method_declaration::type mtype;
  switch (tokens[out.next_token].token_data.as_keyword) {
    case lexer::keywords::GET:
      mtype = method_declaration::type::GET;
      break;
    case lexer::keywords::SET:
      mtype = method_declaration::type::SET;
      break;
    case lexer::keywords::OPERATOR:
      mtype = method_declaration::type::OPERATOR;
      break;
    case lexer::keywords::DEF:
      mtype = method_declaration::type::DEF;
      break;
    case lexer::keywords::CONSTRUCTOR:
      if (store == storage::STATIC) {
        message_builder builder;
        builder.builder << "Constructors cannot be static!";
        out.messages.push_back(builder.build_message(
            logger::level::ERROR, tokens[out.next_token].token_context));
      }
      if (fin) {
        message_builder builder;
        builder.builder << "Constructors are implicitly final!";
        out.messages.push_back(builder.build_message(
            logger::level::WARNING, tokens[out.next_token].token_context));
      }
      mtype = method_declaration::type::CONSTRUCTOR;
      break;
    default: {
      message_builder builder;
      builder.builder
          << "Expected one of 'get', 'set', 'operator', or 'def' in "
             "method declaration!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
      return out;
    }
  }
  out.next_token++;
  if (tokens.size() == out.next_token) {
    message_builder builder;
    builder.builder << "Unexpected end of method declaration!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
    return out;
  }
  if (tokens[out.next_token].token_data.token_type !=
      lexer::token::data::type::DEFERRED_TOKEN) {
    message_builder builder;
    builder.builder << "Expected an identifier for method name!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
    return out;
  }
  output<type_declaration> method_name =
      parse<type_declaration>(filename, tokens, out.next_token, classes);
  std::copy(method_name.messages.begin(), method_name.messages.end(),
            std::back_inserter(out.messages));
  out.next_token = method_name.next_token;
  if (!method_name.value) {
    return out;
  }
  if (tokens.size() == out.next_token) {
    message_builder builder;
    builder.builder << "Unexpected end of method declaration!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
    return out;
  }
  if (tokens[out.next_token].token_data.token_type !=
          lexer::token::data::type::OPERATOR_TOKEN ||
      tokens[out.next_token].token_data.as_operator !=
          lexer::operators::ROUND_OPEN) {
    message_builder builder;
    builder.builder << "Expected parameters to begin with opening parenthesis!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
    return out;
  }
  out.next_token++;
  if (tokens.size() == out.next_token) {
    message_builder builder;
    builder.builder << "Unexpected end of method declaration!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
    return out;
  }
  bool first_argument_correction = true;
  std::vector<argument> arguments;
  while (tokens[out.next_token].token_data.token_type !=
             lexer::token::data::type::OPERATOR_TOKEN &&
         tokens[out.next_token].token_data.as_operator !=
             lexer::operators::ROUND_CLOSE) {
    out.next_token -= first_argument_correction;
    first_argument_correction = false;
    out.next_token++;
    if (tokens.size() == out.next_token) {
      message_builder builder;
      builder.builder << "Unexpected end of method declaration!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
      return out;
    }
    if (tokens[out.next_token].token_data.token_type !=
        lexer::token::data::type::DEFERRED_TOKEN) {
      message_builder builder;
      builder.builder << "Method argument must begin with an identifier!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
      return out;
    }
    output<argument> arg =
        parse<argument>(filename, tokens, out.next_token, classes);
    std::copy(arg.messages.begin(), arg.messages.end(),
              std::back_inserter(out.messages));
    out.next_token = arg.next_token;
    if (!arg.value) {
      return out;
    }
    arguments.push_back(std::move(**arg.value));
    if (tokens.size() == out.next_token) {
      message_builder builder;
      builder.builder << "Unexpected end of method declaration!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
      return out;
    }
    while (tokens[out.next_token].token_data.token_type !=
               lexer::token::data::type::OPERATOR_TOKEN ||
           (tokens[out.next_token].token_data.as_operator !=
                lexer::operators::ROUND_CLOSE &&
            tokens[out.next_token].token_data.as_operator !=
                lexer::operators::COMMA)) {
      message_builder builder;
      builder.builder << "Expected comma or close parenthesis to continue "
                         "argument declarations!";
      out.messages.push_back(builder.build_message(
          logger::level::ERROR, tokens[out.next_token].token_context));
      out.next_token++;
      if (tokens.size() == out.next_token) {
        message_builder builder;
        builder.builder << "Unexpected end of method declaration!";
        out.messages.push_back(builder.build_message(
            logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
        return out;
      }
    }
  }
  out.next_token++;
  if (tokens.size() == out.next_token) {
    message_builder builder;
    builder.builder << "Unexpected end of method declaration!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
    return out;
  }
  type_instantiation ret{"void", {}, false};
  if (mtype != method_declaration::type::CONSTRUCTOR) {
    if (tokens[out.next_token].token_data.token_type !=
            lexer::token::data::type::OPERATOR_TOKEN ||
        tokens[out.next_token].token_data.as_operator !=
            lexer::operators::CAST) {
      message_builder builder;
      builder.builder
          << "Expected parameters to begin with opening parenthesis!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
      return out;
    }
    out.next_token++;
    if (tokens.size() == out.next_token) {
      message_builder builder;
      builder.builder << "Unexpected end of method declaration!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
      return out;
    }
    if (tokens[out.next_token].token_data.token_type !=
        lexer::token::data::type::DEFERRED_TOKEN) {
      message_builder builder;
      builder.builder << "Return type must begin with an identifier!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
      return out;
    }
    output<type_instantiation> rettype =
        parse<type_instantiation>(filename, tokens, begin, classes);
    std::copy(rettype.messages.begin(), rettype.messages.end(),
              std::back_inserter(out.messages));
    out.next_token = rettype.next_token;
    if (!rettype.value) {
      return out;
    }
    ret = std::move(**rettype.value);
  }
  if (tokens.size() == out.next_token) {
    message_builder builder;
    builder.builder << "Unexpected end of method declaration!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
    return out;
  }
  if (tokens[out.next_token].token_data.token_type !=
          lexer::token::data::type::OPERATOR_TOKEN ||
      (tokens[out.next_token].token_data.as_operator !=
           lexer::operators::SEMICOLON &&
       tokens[out.next_token].token_data.as_operator !=
           lexer::operators::CURLY_OPEN)) {
    message_builder builder;
    builder.builder << "Expected either a semicolon or method body here!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
    return out;
  }
  std::vector<lexer::token> subtokens;
  if (tokens[out.next_token].token_data.as_operator ==
      lexer::operators::SEMICOLON) {
    subtokens.push_back(tokens[out.next_token]);
    out.next_token++;
  } else {
    std::size_t nested = 0;
    do {
      if (tokens.size() == out.next_token) {
        message_builder builder;
        builder.builder << "Unexpected end of method body!";
        out.messages.push_back(builder.build_message(
            logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
        return out;
      }
      if (tokens[out.next_token].token_data.token_type ==
          lexer::token::data::type::OPERATOR_TOKEN) {
        if (tokens[out.next_token].token_data.as_operator ==
            lexer::operators::CURLY_OPEN) {
          nested++;
        } else if (tokens[out.next_token].token_data.as_operator ==
                   lexer::operators::CURLY_CLOSE) {
          nested--;
        }
      }
      subtokens.push_back(tokens[out.next_token++]);
    } while (nested);
  }
  out.value = std::make_unique<unparsed_method_declaration>(
      std::move(**method_name.value), mods, store, spec, fin, std::move(ret),
      std::move(arguments), std::move(subtokens));
  return out;
}

parse_decl(argument) {
  output<argument> out;
  out.filename = filename;
  out.next_token = begin;
  out.contexts.push_back(tokens[out.next_token].token_context);
  output<type_instantiation> type =
      parse<type_instantiation>(filename, tokens, out.next_token, classes);
  std::optional<access_expression> qualified_type;
  std::copy(type.messages.begin(), type.messages.end(),
            std::back_inserter(out.messages));
  out.next_token = type.next_token;
  if (!type.value) {
    return out;
  }
  if (out.next_token == tokens.size()) {
    message_builder builder;
    builder.builder << "Unexpected end of declaration statement!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
    return out;
  }
  if (tokens[out.next_token].token_data.token_type !=
      lexer::token::data::type::DEFERRED_TOKEN) {
    if (tokens[out.next_token].token_data.token_type ==
            lexer::token::data::type::OPERATOR_TOKEN &&
        tokens[out.next_token].token_data.as_operator ==
            lexer::operators::ACCESS) {
      output<access_expression> qualified =
          parse<access_expression>(filename, tokens, out.next_token, classes);
      std::copy(qualified.messages.begin(), qualified.messages.end(),
                std::back_inserter(out.messages));
      out.next_token = qualified.next_token;
      if (qualified.value && tokens.size() > out.next_token &&
          tokens[out.next_token].token_data.token_type ==
              lexer::token::data::type::DEFERRED_TOKEN) {
        qualified_type = std::move(**qualified.value);
      }
    }
    if (!qualified_type) {
      message_builder builder;
      builder.builder << "Expected identifier after type in declaration!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
      return out;
    }
  }
  std::string name(tokens[out.next_token].token_data.as_deferred.start,
                   tokens[out.next_token].token_data.as_deferred.size);
  out.next_token++;
  std::optional<std::unique_ptr<expression>> default_value;
  if (tokens.size() > out.next_token &&
      tokens[out.next_token].token_data.token_type ==
          lexer::token::data::type::OPERATOR_TOKEN &&
      tokens[out.next_token].token_data.as_operator == lexer::operators::EQ) {
    out.next_token++;
    if (tokens.size() == out.next_token) {
      message_builder builder;
      builder.builder << "Unexpected end of argument default value!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
      return out;
    }
    output<expression> expr =
        parse<expression>(filename, tokens, out.next_token, classes);
    std::copy(expr.messages.begin(), expr.messages.end(),
              std::back_inserter(out.messages));
    out.next_token = expr.next_token;
    if (!expr.value) {
      return out;
    }
    default_value = std::move(*expr.value);
  }
  if (qualified_type) {
    out.value = std::make_unique<argument>(
        std::move(*qualified_type), std::move(name), std::move(default_value));
  } else {
    out.value = std::make_unique<argument>(
        std::move(**type.value), std::move(name), std::move(default_value));
  }
  return out;
}

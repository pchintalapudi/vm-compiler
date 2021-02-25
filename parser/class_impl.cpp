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
parse_decl(parameter);
parse_decl(type_declaration);
parse_decl(generic_declaration);

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
        lexer::token::data::type::IDENTIFIER_TOKEN) {
      message_builder builder;
      builder.builder << "Package folders must not be strings, numbers, "
                         "operators, or keywords!";
      out.messages.push_back(builder.build_message(
          logger::level::ERROR, tokens[begin].token_context));
      begin--;
      break;
    }
    package->push_back(
        std::string(tokens[begin].token_data.as_identifier.start,
                    tokens[begin].token_data.as_identifier.size));
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
        lexer::token::data::type::IDENTIFIER_TOKEN) {
      message_builder builder;
      builder.builder << "import folders must not be strings, numbers, "
                         "operators, or keywords!";
      out.messages.push_back(builder.build_message(
          logger::level::ERROR, tokens[begin].token_context));
      begin--;
      break;
    }
    package.push_back(std::string(tokens[begin].token_data.as_identifier.start,
                                  tokens[begin].token_data.as_identifier.size));
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
        lexer::token::data::type::IDENTIFIER_TOKEN) {
      message_builder builder;
      builder.builder << "Expected import alias to be an identifier!";
      out.messages.push_back(builder.build_message(
          logger::level::ERROR, tokens[begin].token_context));
    } else {
      alias = std::string(tokens[begin].token_data.as_identifier.start,
                          tokens[begin].token_data.as_identifier.size);
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
      lexer::token::data::type::IDENTIFIER_TOKEN) {
    message_builder builder;
    builder.builder << "Class name must be an identifier!";
    out.messages.push_back(builder.build_message(logger::level::FATAL_ERROR,
                                                 tokens[begin].token_context));
    return out;
  }
  classes.emplace(tokens[begin + 1].token_data.as_identifier.start,
                  tokens[begin + 1].token_data.as_identifier.size);
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
      lexer::token::data::type::IDENTIFIER_TOKEN) {
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
  std::vector<parameter> arguments;
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
        lexer::token::data::type::IDENTIFIER_TOKEN) {
      message_builder builder;
      builder.builder << "Method parameter must begin with an identifier!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
      return out;
    }
    output<parameter> arg =
        parse<parameter>(filename, tokens, out.next_token, classes);
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
                         "parameter declarations!";
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
  general_type ret{
      access_expression(std::make_unique<type_instantiation>(
                            "void", std::vector<general_type>{}, false),
                        {})};
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
        lexer::token::data::type::IDENTIFIER_TOKEN) {
      message_builder builder;
      builder.builder << "Return type must begin with an identifier!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
      return out;
    }
    output<general_type> rettype =
        parse<general_type>(filename, tokens, begin, classes);
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

parse_decl(parameter) {
  output<parameter> out;
  out.filename = filename;
  out.next_token = begin;
  out.contexts.push_back(tokens[out.next_token].token_context);
  output<general_type> type =
      parse<general_type>(filename, tokens, out.next_token, classes);
  std::copy(type.messages.begin(), type.messages.end(),
            std::back_inserter(out.messages));
  out.next_token = type.next_token;
  if (!type.value) {
    return out;
  }
  if (out.next_token == tokens.size()) {
    message_builder builder;
    builder.builder << "Unexpected end of parameter declaration!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
    return out;
  }
  std::string name(tokens[out.next_token].token_data.as_identifier.start,
                   tokens[out.next_token].token_data.as_identifier.size);
  out.next_token++;
  std::optional<std::unique_ptr<expression>> default_value;
  if (tokens.size() > out.next_token &&
      tokens[out.next_token].token_data.token_type ==
          lexer::token::data::type::OPERATOR_TOKEN &&
      tokens[out.next_token].token_data.as_operator == lexer::operators::EQ) {
    out.next_token++;
    if (tokens.size() == out.next_token) {
      message_builder builder;
      builder.builder << "Unexpected end of parameter default value!";
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
  out.value = std::make_unique<parameter>(
      std::move(**type.value), std::move(name), std::move(default_value));
  return out;
}

parse_decl(type_declaration) {
  output<type_declaration> out;
  out.filename = filename;
  out.next_token = begin;
  out.contexts.push_back(tokens[out.next_token].token_context);
  std::string name(tokens[out.next_token].token_data.as_identifier.start,
                   tokens[out.next_token].token_data.as_identifier.size);
  out.next_token++;
  if (out.next_token == tokens.size() ||
      tokens[out.next_token].token_data.token_type !=
          lexer::token::data::type::OPERATOR_TOKEN ||
      tokens[out.next_token].token_data.as_operator != lexer::operators::LESS) {
    out.value = std::make_unique<type_declaration>(
        std::move(name), std::vector<generic_declaration>{});
    return out;
  }
  std::vector<generic_declaration> generics;
  do {
    out.next_token++;
    if (out.next_token == tokens.size()) {
      message_builder builder;
      builder.builder << "Unexpected end of type declaration!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
      return out;
    }
    if (tokens[out.next_token].token_data.token_type !=
        lexer::token::data::type::IDENTIFIER_TOKEN) {
      message_builder builder;
      builder.builder << "Generic declaration must begin with an identifier!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
      return out;
    }
    output<generic_declaration> generic =
        parse<generic_declaration>(filename, tokens, out.next_token, classes);
    std::copy(generic.messages.begin(), generic.messages.end(),
              std::back_inserter(out.messages));
    out.next_token = generic.next_token;
    if (!generic.value) {
      return out;
    }
    if (out.next_token == tokens.size()) {
      message_builder builder;
      builder.builder << "Unexpected end of type declaration!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
      return out;
    }
    if (tokens[out.next_token].token_data.token_type !=
            lexer::token::data::type::OPERATOR_TOKEN ||
        (tokens[out.next_token].token_data.as_operator !=
             lexer::operators::COMMA &&
         tokens[out.next_token].token_data.as_operator !=
             lexer::operators::GREATER)) {
      message_builder builder;
      builder.builder << "Generic declaration must be separated by commas and "
                         "end in a right angle bracket!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
      return out;
    }
  } while (tokens[out.next_token].token_data.as_operator !=
           lexer::operators::GREATER);
  out.next_token++;
  out.value =
      std::make_unique<type_declaration>(std::move(name), std::move(generics));
  return out;
}

parse_decl(generic_declaration) {
  output<generic_declaration> out;
  out.filename = filename;
  out.next_token = begin;
  out.contexts.push_back(tokens[out.next_token].token_context);
  std::string name(tokens[out.next_token].token_data.as_identifier.start,
                   tokens[out.next_token].token_data.as_identifier.size);
  std::vector<std::pair<generic_declaration::bound, general_type>> bounds;
  do {
    out.next_token++;
    if (tokens.size() == out.next_token) {
      out.value = std::make_unique<generic_declaration>(std::move(name),
                                                        std::move(bounds));
      return out;
    }
    generic_declaration::bound bound;
    switch (tokens[out.next_token].token_data.token_type) {
      case lexer::token::data::type::KEYWORD_TOKEN:
        if (tokens[out.next_token].token_data.as_keyword ==
            lexer::keywords::EXTENDS) {
          bound = generic_declaration::bound::EXTENDS;
          break;
        } else {
          out.value = std::make_unique<generic_declaration>(std::move(name),
                                                            std::move(bounds));
          return out;
        }
      case lexer::token::data::type::IDENTIFIER_TOKEN: {
        std::string token(tokens[out.next_token].token_data.as_identifier.start,
                          tokens[out.next_token].token_data.as_identifier.size);
        if (token == "super") {
          bound = generic_declaration::bound::SUPER;
          break;
        } else {
          out.value = std::make_unique<generic_declaration>(std::move(name),
                                                            std::move(bounds));
          return out;
        }
      }
      case lexer::token::data::type::OPERATOR_TOKEN:
      case lexer::token::data::type::LITERAL_TOKEN:
        out.value = std::make_unique<generic_declaration>(std::move(name),
                                                          std::move(bounds));
        return out;
    }
    out.next_token++;
    if (out.next_token == tokens.size()) {
      message_builder builder;
      builder.builder << "Unexpected end of generic declaration!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
      return out;
    }
    if (tokens[out.next_token].token_data.token_type !=
        lexer::token::data::type::IDENTIFIER_TOKEN) {
      message_builder builder;
      builder.builder << "Generic type bound must be a class!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
      return out;
    }
    if (classes.find(std::string(
            tokens[out.next_token].token_data.as_identifier.start,
            tokens[out.next_token].token_data.as_identifier.size)) ==
        classes.end()) {
      message_builder builder;
      builder.builder << "Generic type bound must be a class!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
      return out;
    }
    output<general_type> type =
        parse<general_type>(filename, tokens, out.next_token, classes);
    std::copy(type.messages.begin(), type.messages.end(),
              std::back_inserter(out.messages));
    out.next_token = type.next_token;
    if (!type.value) {
      return out;
    }
    bounds.emplace_back(bound, std::move(**type.value));
  } while (out.next_token < tokens.size() &&
           tokens[out.next_token].token_data.token_type ==
               lexer::token::data::type::OPERATOR_TOKEN &&
           tokens[out.next_token].token_data.as_operator ==
               lexer::operators::BITAND);
  out.value =
      std::make_unique<generic_declaration>(std::move(name), std::move(bounds));
  return out;
}

parse_decl(class_definition) {
  output<class_definition> out;
  out.filename = filename;
  out.next_token = begin;
  out.contexts.push_back(tokens[out.next_token].token_context);
  out.next_token++;
  if (out.next_token == tokens.size()) {
    message_builder builder;
    builder.builder << "Unexpected end of class declaration!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
    return out;
  }
  output<type_declaration> decl =
      parse<type_declaration>(filename, tokens, out.next_token, classes);
  std::copy(decl.messages.begin(), decl.messages.end(),
            std::back_inserter(out.messages));
  out.next_token = decl.next_token;
  if (!decl.value) {
    return out;
  }
  if (out.next_token == tokens.size()) {
    message_builder builder;
    builder.builder << "Unexpected end of class declaration!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
    return out;
  }
  std::optional<general_type> superclass;
  if (tokens[out.next_token].token_data.token_type ==
          lexer::token::data::type::KEYWORD_TOKEN &&
      tokens[out.next_token].token_data.as_keyword ==
          lexer::keywords::EXTENDS) {
    out.next_token++;
    if (out.next_token == tokens.size()) {
      message_builder builder;
      builder.builder << "Unexpected end of class extension declaration!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
      return out;
    }
    output<general_type> superclassret =
        parse<general_type>(filename, tokens, out.next_token, classes);
    std::copy(superclassret.messages.begin(), superclassret.messages.end(),
              std::back_inserter(out.messages));
    out.next_token = superclassret.next_token;
    if (!superclassret.value) {
      return out;
    }
    superclass = std::move(**superclassret.value);
    if (out.next_token == tokens.size()) {
      message_builder builder;
      builder.builder << "Unexpected end of class declaration!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
      return out;
    }
  }
  std::vector<general_type> interfaces;
  if (tokens[out.next_token].token_data.token_type ==
          lexer::token::data::type::KEYWORD_TOKEN &&
      tokens[out.next_token].token_data.as_keyword ==
          lexer::keywords::IMPLEMENTS) {
    do {
      out.next_token++;
      if (out.next_token == tokens.size()) {
        message_builder builder;
        builder.builder << "Unexpected end of interface extension declaration!";
        out.messages.push_back(builder.build_message(
            logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
        return out;
      }
      output<general_type> interface =
          parse<general_type>(filename, tokens, out.next_token, classes);
      std::copy(interface.messages.begin(), interface.messages.end(),
                std::back_inserter(out.messages));
      out.next_token = interface.next_token;
      if (!interface.value) {
        return out;
      }
      interfaces.push_back(std::move(**interface.value));
      if (out.next_token == tokens.size()) {
        message_builder builder;
        builder.builder << "Unexpected end of class declaration!";
        out.messages.push_back(builder.build_message(
            logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
        return out;
      }
      if (tokens[out.next_token].token_data.token_type !=
              lexer::token::data::type::OPERATOR_TOKEN ||
          (tokens[out.next_token].token_data.as_operator !=
               lexer::operators::COMMA &&
           tokens[out.next_token].token_data.as_operator !=
               lexer::operators::CURLY_OPEN)) {
        message_builder builder;
        builder.builder << "Expected comma separated interface declarations!";
        out.messages.push_back(builder.build_message(
            logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
        return out;
      }
    } while (tokens[out.next_token].token_data.as_operator ==
             lexer::operators::COMMA);
  }
  if (tokens[out.next_token].token_data.token_type !=
          lexer::token::data::type::OPERATOR_TOKEN ||
      tokens[out.next_token].token_data.as_operator !=
          lexer::operators::CURLY_OPEN) {
    message_builder builder;
    builder.builder << "Expected class to open with a curly brace!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
    return out;
  }
  std::vector<std::unique_ptr<variable>> vars;
  std::vector<std::unique_ptr<method_declaration>> mtds;
  std::vector<std::unique_ptr<class_definition>> sub_classes;
  out.next_token++;
  if (out.next_token == tokens.size()) {
    message_builder builder;
    builder.builder << "Unexpected end of class definition!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
    return out;
  }
  while (tokens[out.next_token].token_data.token_type ==
         lexer::token::data::type::KEYWORD_TOKEN) {
    if (out.next_token + 4 >= tokens.size()) {
      message_builder builder;
      builder.builder << "Unexpected end of class declaration!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
      return out;
    }
    if (tokens[out.next_token].token_data.token_type !=
        lexer::token::data::type::KEYWORD_TOKEN) {
      message_builder builder;
      builder.builder << "Class member must begin with public, protected, "
                         "package, or private!";
      out.messages.push_back(builder.build_message(
          logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
      return out;
    }
    switch (tokens[out.next_token].token_data.as_keyword) {
      case lexer::keywords::PUBLIC:
      case lexer::keywords::PROTECTED:
      case lexer::keywords::PACKAGE:
      case lexer::keywords::PRIVATE:
        break;
      default: {
        message_builder builder;
        builder.builder << "Class member must begin with public, protected, "
                           "package, or private!";
        out.messages.push_back(builder.build_message(
            logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
        return out;
      }
    }
    bool parse_class = false;
    bool parse_variable = false;
    std::size_t offset = out.next_token;
    offset++;
    if (tokens[offset].token_data.token_type !=
        lexer::token::data::type::KEYWORD_TOKEN) {
      parse_variable = true;
      goto parse;
    }
    switch (tokens[offset].token_data.as_keyword) {
      case lexer::keywords::CLASS:
        parse_class = true;
        goto parse;
      case lexer::keywords::STATIC:
        offset++;
        break;
      case lexer::keywords::FINAL:
        offset++;
        break;
      default:
        break;
    }
    if (tokens[offset].token_data.token_type !=
        lexer::token::data::type::KEYWORD_TOKEN) {
      parse_variable = true;
      goto parse;
    }
    switch (tokens[offset].token_data.as_keyword) {
      case lexer::keywords::STATIC:
        offset++;
        break;
      case lexer::keywords::FINAL:
        offset++;
        break;
      default:
        break;
    }
    if (tokens[offset].token_data.token_type !=
        lexer::token::data::type::KEYWORD_TOKEN) {
      parse_variable = true;
      goto parse;
    }
    switch (tokens[offset].token_data.as_keyword) {
      case lexer::keywords::NATIVE:
      case lexer::keywords::INTRINSIC:
      case lexer::keywords::DEF:
      case lexer::keywords::CONSTRUCTOR:
      case lexer::keywords::GET:
      case lexer::keywords::SET:
      case lexer::keywords::OPERATOR:
        break;
      default:
        parse_variable = true;
        break;
    }
  parse:
    if (parse_class) {
      output<class_definition> cls =
          parse<class_definition>(filename, tokens, out.next_token, classes);
      std::copy(cls.messages.begin(), cls.messages.end(),
                std::back_inserter(out.messages));
      out.next_token = cls.next_token;
      if (!cls.value) {
        return out;
      }
      sub_classes.push_back(std::move(*cls.value));
    } else if (parse_variable) {
      output<variable> var =
          parse<variable>(filename, tokens, out.next_token, classes);
      std::copy(var.messages.begin(), var.messages.end(),
                std::back_inserter(out.messages));
      out.next_token = var.next_token;
      if (!var.value) {
        return out;
      }
      vars.push_back(std::move(*var.value));
    } else {
      output<unparsed_method_declaration> mtd =
          parse<unparsed_method_declaration>(filename, tokens, out.next_token,
                                             classes);
      std::copy(mtd.messages.begin(), mtd.messages.end(),
                std::back_inserter(out.messages));
      out.next_token = mtd.next_token;
      if (!mtd.value) {
        return out;
      }
      mtds.push_back(std::move(*mtd.value));
    }
  }
  while (out.next_token < tokens.size() &&
         (tokens[out.next_token].token_data.token_type !=
              lexer::token::data::type::OPERATOR_TOKEN ||
          tokens[out.next_token].token_data.as_operator !=
              lexer::operators::CURLY_CLOSE)) {
    message_builder builder;
    builder.builder << "Expected class to end here!";
    out.messages.push_back(builder.build_message(
        logger::level::ERROR, tokens[out.next_token].token_context));
    out.next_token++;
  }
  if (out.next_token + 4 >= tokens.size()) {
    message_builder builder;
    builder.builder << "Unexpected end of class declaration!";
    out.messages.push_back(builder.build_message(
        logger::level::FATAL_ERROR, tokens[out.next_token].token_context));
    return out;
  }
  out.next_token++;
  out.value = std::make_unique<class_definition>(
      std::move(**decl.value), std::move(sub_classes), std::move(interfaces),
      std::move(superclass), std::move(vars), std::move(mtds));
  return out;
}
#include "node_type.h"
#include "output.h"
#include "type.h"

using namespace oops_compiler::parser;

template <>
output<std::unique_ptr<type_instantiation>>
oops_compiler::parser::parse<type_instantiation>(
    token_stream &tokens, std::unordered_set<std::string> &classes) {
  output<std::unique_ptr<type_instantiation>> out{{}, {}, true};
  auto name = parse<identifier>(tokens, classes);
  out.merge_messages(name);
  if (name.fatal) {
    return out;
  }
  if (!name.value || classes.find((**name.value).str()) == classes.end()) {
    out.log(logger::level::ERROR, tokens.context(), "Expected a type name!");
    out.fatal = false;
    return out;
  }
  if (tokens.pop_specific<lexer::token::data::type::OPERATOR_TOKEN>(
          lexer::operators::LESS)) {
    std::vector<std::unique_ptr<general_type>> subtypes;
    do {
      output<std::unique_ptr<general_type>> subtype =
          parse<general_type>(tokens, classes);
      out.merge_messages(subtype);
      if (subtype.fatal) {
        return out;
      }
      if (subtype.value) {
        subtypes.push_back(std::move(*subtype.value));
      }
    } while (tokens.pop_specific<lexer::token::data::type::OPERATOR_TOKEN>(
        lexer::operators::COMMA));
    auto next = tokens.pop_specific<lexer::token::data::type::OPERATOR_TOKEN>(
        lexer::operators::BITSRA, lexer::operators::BITSRAEQ,
        lexer::operators::BITSRL, lexer::operators::BITSRLEQ,
        lexer::operators::GEQUALS, lexer::operators::GREATER);
    if (!next) {
      out.log(logger::level::FATAL_ERROR, tokens.context(),
              "Unexpected token in generic declaration!");
      return out;
    }
    switch (next->token_data.as_operator) {
      case lexer::operators::BITSRA: {
        next->token_data.as_operator = lexer::operators::GREATER;
        tokens.push(std::move(*next));
        break;
      }
      case lexer::operators::BITSRAEQ: {
        next->token_data.as_operator = lexer::operators::GEQUALS;
        tokens.push(std::move(*next));
        break;
      }
      case lexer::operators::BITSRL: {
        next->token_data.as_operator = lexer::operators::BITSRA;
        tokens.push(std::move(*next));
        break;
      }
      case lexer::operators::BITSRLEQ: {
        next->token_data.as_operator = lexer::operators::BITSRAEQ;
        tokens.push(std::move(*next));
        break;
      }
      case lexer::operators::GEQUALS: {
        next->token_data.as_operator = lexer::operators::EQ;
        tokens.push(std::move(*next));
        break;
      }
      case lexer::operators::GREATER: {
        break;
      }
      default:
        out.log(logger::level::FATAL_ERROR, tokens.context(),
                "Filter error in token stream!");
        return out;
    }
    out.value = std::make_unique<type_instantiation>(std::move(*name.value),
                                                     std::move(subtypes));
    out.fatal = false;
    return out;
  } else {
    out.value = std::make_unique<type_instantiation>(
        std::move(*name.value), std::vector<std::unique_ptr<general_type>>{});
    out.fatal = false;
    return out;
  }
}

template <>
output<std::unique_ptr<generic_bound>>
oops_compiler::parser::parse<generic_bound>(
    token_stream &tokens, std::unordered_set<std::string> &classes) {
  output<std::unique_ptr<generic_bound>> out{{}, {}, true};
  output<std::unique_ptr<identifier>> name = parse<identifier>(tokens, classes);
  out.merge_messages(name);
  if (name.fatal) {
    return out;
  }
  if (!name.value) {
    out.log(logger::level::ERROR, tokens.context(), "Expected name here!");
    return out;
  }
  std::vector<std::unique_ptr<general_type>> lower_bounds, upper_bounds;
  if (tokens.pop_specific<lexer::token::data::type::KEYWORD_TOKEN>(
          lexer::keywords::EXTENDS)) {
    do {
      if (!tokens.size()) {
        out.log(logger::level::ERROR, tokens.context(),
                "Unexpected end of generic bound here!");
        return out;
      }
      output<std::unique_ptr<general_type>> against =
          parse<general_type>(tokens, classes);
      out.merge_messages(against);
      if (against.fatal) {
        return out;
      }
      if (against.value) {
        upper_bounds.push_back(std::move(*against.value));
      } else {
        out.log(logger::level::ERROR, tokens.context(),
                "Expected a class here!");
      }
    } while (tokens.pop_specific<lexer::token::data::type::OPERATOR_TOKEN>(
        lexer::operators::BITAND));
  }
  if (auto token =
          tokens.pop_type<lexer::token::data::type::IDENTIFIER_TOKEN>()) {
    if (std::string(token->token_data.as_identifier.start,
                    token->token_data.as_identifier.size) == "super") {
      do {
        if (!tokens.size()) {
          out.log(logger::level::ERROR, tokens.context(),
                  "Unexpected end of generic bound here!");
          return out;
        }
        output<std::unique_ptr<general_type>> against =
            parse<general_type>(tokens, classes);
        out.merge_messages(against);
        if (against.fatal) {
          return out;
        }
        if (against.value) {
          lower_bounds.push_back(std::move(*against.value));
        } else {
          out.log(logger::level::ERROR, tokens.context(),
                  "Expected a class here!");
        }
      } while (tokens.pop_specific<lexer::token::data::type::OPERATOR_TOKEN>(
          lexer::operators::BITAND));
    } else {
      tokens.push(std::move(*token));
    }
  }
  out.value = std::make_unique<generic_bound>(
      std::move(*name.value), std::move(lower_bounds), std::move(upper_bounds));
  out.fatal = false;
  return out;
}

template <>
output<std::unique_ptr<type_declaration>>
oops_compiler::parser::parse<type_declaration>(
    token_stream &tokens, std::unordered_set<std::string> &classes) {
  output<std::unique_ptr<type_declaration>> out{{}, {}, true};
  output<std::unique_ptr<identifier>> name = parse<identifier>(tokens, classes);
  out.merge_messages(name);
  if (name.fatal) {
    return out;
  }
  if (!name.value) {
    out.log(logger::level::ERROR, tokens.context(), "Expected name here!");
    return out;
  }
  if (tokens.pop_specific<lexer::token::data::type::OPERATOR_TOKEN>(
          lexer::operators::LESS)) {
    std::vector<std::unique_ptr<generic_bound>> generics;
    do {
      if (!tokens.size()) {
        out.log(logger::level::ERROR, tokens.context(),
                "Unexpected end of type declaration here!");
        return out;
      }
      output<std::unique_ptr<generic_bound>> against =
          parse<generic_bound>(tokens, classes);
      out.merge_messages(against);
      if (against.fatal) {
        return out;
      }
      if (against.value) {
        generics.push_back(std::move(*against.value));
      } else {
        out.log(logger::level::ERROR, tokens.context(),
                "Expected a generic declaration here!");
      }
    } while (tokens.pop_specific<lexer::token::data::type::OPERATOR_TOKEN>(
        lexer::operators::COMMA));
    out.value = std::make_unique<type_declaration>(std::move(*name.value),
                                                   std::move(generics));
    out.fatal = false;
    return out;
  } else {
    out.value = std::make_unique<type_declaration>(
        std::move(*name.value), std::vector<std::unique_ptr<generic_bound>>{});
    out.fatal = false;
    return out;
  }
}
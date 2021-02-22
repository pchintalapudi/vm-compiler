#include "lexer.h"

#include <algorithm>
#include <iostream>
#include <sstream>

#include "converters.h"

using namespace oops_compiler::lexer;
lexer::lexer(const void *file, std::size_t file_size, const char *filename)
    : root{},
      file(static_cast<const char *>(file)),
      file_size(file_size),
      filename(filename) {
  for (unsigned i = 0; i < static_cast<unsigned>(operators::__COUNT__); i++) {
    this->root.insert(all_mappings.operators_to_strings[i],
                      static_cast<operators>(i));
  }
  std::vector<std::string> out;
  std::string base;
  this->root.get_elements(out, base);
  for (const auto &str : out) {
    std::cout << "Operator " << str << "\n";
  }
}

namespace {
struct message_builder {
  std::stringstream builder;
  std::string get_message() {
    std::string out = builder.str();
    builder.clear();
    return out;
  }
};
enum class whitespace_type {
  NOT_WHITESPACE,
  VALID_WHITESPACE,
  NEWLINE,
  INVALID_WHITESPACE
};
whitespace_type get_whitespace_type(char c) {
  constexpr char DEL = 127, TAB = '\t', LF = '\n', CR = '\r', SPACE = ' ';
  if (c == DEL) {
    return whitespace_type::INVALID_WHITESPACE;
  }
  if (c > 32) {
    return whitespace_type::NOT_WHITESPACE;
  }
  switch (c) {
    case LF:
      return whitespace_type::NEWLINE;
    case TAB:
    case CR:
    case SPACE:
      return whitespace_type::VALID_WHITESPACE;
    default:
      return whitespace_type::INVALID_WHITESPACE;
  }
}
std::optional<std::pair<int, oops_compiler::lexer::operators>> parse_operator(
    const char *start, const char *end,
    const oops_compiler::lexer::operator_trie *root) {
  const char *current = start;
  const oops_compiler::lexer::operator_trie *prev_root;
  while (root && current < end) {
    root = (*(prev_root = root))[*current++];
  }
  if (root) {
    prev_root = root;
  }
  while (prev_root->get_operator() == operators::__COUNT__ &&
         prev_root->parent_full()) {
    prev_root = prev_root->get_parent();
  }
  if (prev_root->get_operator() != operators::__COUNT__) {
    return std::pair{prev_root->get_depth(), prev_root->get_operator()};
  } else {
    return {};
  }
}
std::variant<std::int64_t, std::string> parse_int(const char *str,
                                                  std::size_t length,
                                                  int radix = 10) {
  try {
    std::cout << "parse int " << std::string(str, length) << "\n";
    return std::stoll(std::string(str, length), 0, radix);
  } catch (const std::overflow_error &) {
    return std::string("Long literal too large!!!");
  } catch (const std::invalid_argument &) {
    return std::string("Unparseable long literal!!!");
  }
}
std::variant<double, std::string> parse_double(const char *str,
                                               std::size_t length) {
  try {
    return std::stod(std::string(str, length), 0);
  } catch (const std::overflow_error &) {
    return std::string("Double literal too large!!!");
  } catch (const std::invalid_argument &) {
    return std::string("Unparseable double literal!!!");
  }
}
std::variant<float, std::string> parse_float(const char *str,
                                             std::size_t length) {
  try {
    return std::stof(std::string(str, length), 0);
  } catch (const std::overflow_error &) {
    return std::string("Float literal too large!!!");
  } catch (const std::invalid_argument &) {
    return std::string("Unparseable float literal!!!");
  }
}
struct lexer_variables {
  oops_compiler::lexer::lexed_output *out;
  const char *start;
  const char *end;
  oops_compiler::logger::context context;
  message_builder builder;
  oops_compiler::lexer::token identifier_token;
  void build_message(oops_compiler::logger::level level) {
    out->messages.push_back(oops_compiler::logger::message{
        .text = builder.get_message(),
        .location = context,
        .produced_at = oops_compiler::logger::stage::LEXING,
        .log_level = level});
  }
  void set_as_integer_token() {
    int radix = 10;
    std::cout << "INTEGER " << identifier_token.token_data.as_identifier.size
              << " \""
              << std::string(identifier_token.token_data.as_identifier.start,
                             identifier_token.token_data.as_identifier.size)
              << "\""
              << "\n";
    if (identifier_token.token_data.as_identifier.start[0] == '0') {
      switch (identifier_token.token_data.as_identifier.size) {
        case 1:
          identifier_token.token_data.token_type =
              token::data::type::LITERAL_TOKEN;
          identifier_token.token_data.as_literal.literal_type =
              token::data::literal::type::INTEGER;
          identifier_token.token_data.as_literal.as_integer = 0;
          return;
        case 2:
          builder.builder << "Integer literals may not begin with 0, except "
                             "for the literal 0 and when the 0 is followed by "
                             "one of x, X, o, O, b, or B!!!";
          build_message(oops_compiler::logger::level::ERROR);
          return;
        default:
          switch (identifier_token.token_data.as_identifier.start[1]) {
            case 'b':
            case 'B':
              radix = 2;
              break;
            case 'o':
            case 'O':
              radix = 8;
              break;
            case 'x':
            case 'X':
              radix = 16;
              break;
            default:
              builder.builder
                  << "Integer literals may not begin with 0, except "
                     "for the literal 0 and when the 0 is followed by "
                     "one of x, X, o, O, b, or B!!!";
              build_message(oops_compiler::logger::level::ERROR);
              return;
          }
          break;
      }
    }
    std::variant<std::int64_t, std::string> maybe;
    if (radix != 10) {
      identifier_token.token_data.as_identifier.start += 2;
      maybe = parse_int(identifier_token.token_data.as_identifier.start,
                        identifier_token.token_data.as_identifier.size - 2, radix);
      identifier_token.token_data.as_identifier.start -= 2;
    } else {
      maybe = parse_int(identifier_token.token_data.as_identifier.start,
                        identifier_token.token_data.as_identifier.size, radix);
    }
    if (std::holds_alternative<std::int64_t>(maybe)) {
      identifier_token.token_data.token_type = token::data::type::LITERAL_TOKEN;
      identifier_token.token_data.as_literal.literal_type =
          token::data::literal::type::INTEGER;
      identifier_token.token_data.as_literal.as_integer =
          std::get<std::int64_t>(maybe);
    } else {
      builder.builder << std::get<std::string>(maybe);
      build_message(oops_compiler::logger::level::ERROR);
    }
  }
  void token_to_number() {
    switch (identifier_token.token_data.as_identifier
                .start[identifier_token.token_data.as_identifier.size - 1]) {
      case 'f':
      case 'F': {
        if (identifier_token.token_data.as_identifier.size < 3 ||
            identifier_token.token_data.as_identifier.start[0] != '0' ||
            (identifier_token.token_data.as_identifier.start[1] != 'x' &&
             identifier_token.token_data.as_identifier.start[1] != 'X')) {
          auto maybe = parse_float(identifier_token.token_data.as_identifier.start,
                                   identifier_token.token_data.as_identifier.size);
          if (std::holds_alternative<float>(maybe)) {
            identifier_token.token_data.token_type =
                token::data::type::LITERAL_TOKEN;
            identifier_token.token_data.as_literal.literal_type =
                token::data::literal::type::FLOAT;
            identifier_token.token_data.as_literal.as_float =
                std::get<float>(maybe);
          } else {
            builder.builder << std::get<std::string>(maybe);
            build_message(oops_compiler::logger::level::ERROR);
          }
        } else {
          set_as_integer_token();
        }
        break;
      }
      case 'i':
      case 'I': {
        identifier_token.token_data.as_identifier.size--;
        set_as_integer_token();
        break;
      }
      case 'd':
      case 'D': {
        if (identifier_token.token_data.as_identifier.size < 3 ||
            identifier_token.token_data.as_identifier.start[0] != '0' ||
            (identifier_token.token_data.as_identifier.start[1] != 'x' &&
             identifier_token.token_data.as_identifier.start[1] != 'X')) {
          auto maybe = parse_double(identifier_token.token_data.as_identifier.start,
                                    identifier_token.token_data.as_identifier.size);
          if (std::holds_alternative<double>(maybe)) {
            identifier_token.token_data.token_type =
                token::data::type::LITERAL_TOKEN;
            identifier_token.token_data.as_literal.literal_type =
                token::data::literal::type::DOUBLE;
            identifier_token.token_data.as_literal.as_double =
                std::get<double>(maybe);
          } else {
            builder.builder << std::get<std::string>(maybe);
            build_message(oops_compiler::logger::level::ERROR);
          }
        } else {
          set_as_integer_token();
        }
        break;
      }
      default:
        if (identifier_token.token_data.as_identifier.size > 2 &&
            identifier_token.token_data.as_identifier.start[0] == '0' &&
            (identifier_token.token_data.as_identifier.start[1] == 'x' ||
             identifier_token.token_data.as_identifier.start[1] == 'X')) {
          set_as_integer_token();
          break;
        }
        for (std::int64_t i = 1; i < identifier_token.token_data.as_identifier.size;
             i++) {
          if (identifier_token.token_data.as_identifier.start[i] == 'e' ||
              identifier_token.token_data.as_identifier.start[i] == 'E' ||
              identifier_token.token_data.as_identifier.start[i] == '.') {
            auto maybe =
                parse_double(identifier_token.token_data.as_identifier.start,
                             identifier_token.token_data.as_identifier.size);
            if (std::holds_alternative<double>(maybe)) {
              identifier_token.token_data.token_type =
                  token::data::type::LITERAL_TOKEN;
              identifier_token.token_data.as_literal.literal_type =
                  token::data::literal::type::DOUBLE;
              identifier_token.token_data.as_literal.as_double =
                  std::get<double>(maybe);
            } else {
              builder.builder << std::get<std::string>(maybe);
              build_message(oops_compiler::logger::level::ERROR);
            }
            return;
          }
        }
        set_as_integer_token();
        break;
    }
  }
  void flush_token() {
    if (identifier_token.token_context.global_char_number > -1) {
      if (isdigit(identifier_token.token_data.as_identifier.start[0])) {
        token_to_number();
      } else if (auto it = all_mappings.string_to_keywords.find(
                     std::string(identifier_token.token_data.as_identifier.start,
                                 identifier_token.token_data.as_identifier.size));
                 it != all_mappings.string_to_keywords.end()) {
        identifier_token.token_data.token_type = token::data::type::KEYWORD_TOKEN;
        identifier_token.token_data.as_keyword = it->second;
      }
      out->output.push_back(identifier_token);
      std::cout << identifier_token.to_string() << std::endl;
      identifier_token.token_data.token_type = token::data::type::IDENTIFIER_TOKEN;
      identifier_token.token_context.global_char_number = -1;
    }
  }
  void extend_token() {
    if (identifier_token.token_context.global_char_number == -1) {
      identifier_token.token_context = context;
      identifier_token.token_data.as_identifier.size = 0;
      identifier_token.token_data.as_identifier.start =
          start + context.global_char_number - 1;
    }
    identifier_token.token_data.as_identifier.size++;
  }
};

lexer_variables init_lexer_values(oops_compiler::lexer::lexed_output *output) {
  lexer_variables out{};
  out.out = output;
  out.identifier_token.token_context.global_char_number = -1;
  return out;
}
}  // namespace

lexed_output lexer::lex() {
  std::cout << "Lexing"
            << "\n";
  lexed_output out{};
  out.filename = this->filename;
  lexer_variables variables = init_lexer_values(&out);
  variables.start = this->file;
  variables.end = variables.start + this->file_size;
  std::cout << "Initialized"
            << "\n";
  std::cout << variables.start << " " << variables.end << std::endl;
  while (variables.start + variables.context.global_char_number <
         variables.end) {
    std::cout << "Loop context " << variables.context.to_string() << "\n";
    if (auto op = parse_operator(
            variables.start + variables.context.global_char_number,
            variables.end, &this->root)) {
      if (op->second == operators::ACCESS &&
          variables.identifier_token.token_context.global_char_number > -1 &&
          isdigit(variables.identifier_token.token_data.as_identifier.start[0])) {
        variables.context.global_char_number++;
        variables.context.char_number++;
        variables.extend_token();
        continue;
      }
      variables.flush_token();
      switch (op->second) {
        case operators::LINE_COMMENT:
          variables.context.global_char_number += op->first;
          variables.context.char_number += op->first;
          while (variables.start + ++variables.context.global_char_number <
                     variables.end &&
                 variables.start[variables.context.global_char_number] != '\n')
            ;
          break;
        case operators::BLOCK_COMMENT_OPEN: {
          auto context = variables.context;
          variables.context.global_char_number += op->first;
          variables.context.char_number += op->first;
          while (variables.start + variables.context.global_char_number <
                 variables.end - 1) {
            switch (variables.start[variables.context.global_char_number]) {
              case '*':
                if (variables.start[variables.context.global_char_number + 1] ==
                    '/') {
                  variables.context.global_char_number += 2;
                  variables.context.char_number += 2;
                  goto comment_end;
                }
                break;
              case '\n':
                variables.context.global_char_number++;
                variables.context.line_number++;
                variables.context.char_number = -1;
                break;
              default:
                variables.context.global_char_number++;
                variables.context.char_number++;
            }
          }
          variables.builder.builder << "Unclosed comment literal!";
          std::swap(variables.context, context);
          variables.build_message(logger::level::FATAL_ERROR);
          std::swap(variables.context, context);
          break;
        comment_end:
          break;
        }
        default:
          out.output.push_back(token{
              .token_data =
                  token::data{.token_type = token::data::type::OPERATOR_TOKEN,
                              .as_operator = op->second},
              .token_context = variables.context});
          std::cout << out.output[out.output.size() - 1].to_string()
                    << std::endl;
          variables.context.global_char_number += op->first;
          variables.context.char_number += op->first;
      }
    } else {
      switch (get_whitespace_type(
          variables.start[variables.context.global_char_number])) {
        case whitespace_type::INVALID_WHITESPACE:
          variables.builder.builder
              << "Invalid whitespace character (ASCII code "
              << static_cast<int>(
                     variables.start[variables.context.global_char_number])
              << ") detected!!!";
          variables.build_message(logger::level::WARNING);
          [[fallthrough]];
        case whitespace_type::NOT_WHITESPACE:
          variables.context.global_char_number++;
          variables.context.char_number++;
          variables.extend_token();
          break;
        case whitespace_type::NEWLINE:
          variables.context.line_number++;
          variables.context.char_number = -1;
          [[fallthrough]];
        case whitespace_type::VALID_WHITESPACE:
          variables.flush_token();
          variables.context.global_char_number++;
          variables.context.char_number++;
      }
    }
  }
  std::cout << "Out of loop\n";
  if (!out.messages.size() || *std::max_element(out.messages.begin(), out.messages.end()) <
      logger::level::ERROR) {
    variables.builder.builder << "Successfully lexed file " << this->filename
                              << "!";
    variables.build_message(logger::level::DEBUG);
  }
  std::sort(out.messages.begin(), out.messages.end());
  return out;
}
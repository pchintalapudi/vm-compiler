#include "lexer.h"

#include <algorithm>
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
    return std::stol(std::string(str, length), 0, radix);
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
  oops_compiler::lexer::token deferred_token;
  void build_message(oops_compiler::logger::level level) {
    out->messages.push_back(oops_compiler::logger::message{
        .text = builder.get_message(),
        .location = context,
        .produced_at = oops_compiler::logger::stage::LEXING,
        .log_level = level});
  }
  void set_as_integer_token() {
    int radix = 10;
    if (deferred_token.token_data.as_deferred.start[0] == '0') {
      switch (deferred_token.token_data.as_deferred.size) {
        case 1:
          deferred_token.token_data.token_type =
              token::data::type::LITERAL_TOKEN;
          deferred_token.token_data.as_literal.literal_type =
              token::data::literal::type::INTEGER;
          deferred_token.token_data.as_literal.as_integer = 0;
          break;
        case 2:
          builder.builder << "Integer literals may not begin with 0, except "
                             "for the literal 0 and when the 0 is followed by "
                             "one of x, X, o, O, b, or B!!!";
          build_message(oops_compiler::logger::level::ERROR);
          return;
        default:
          switch (deferred_token.token_data.as_deferred.start[1]) {
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
    auto maybe = parse_int(deferred_token.token_data.as_deferred.start,
                           deferred_token.token_data.as_deferred.size, radix);
    if (std::holds_alternative<std::int64_t>(maybe)) {
      deferred_token.token_data.token_type = token::data::type::LITERAL_TOKEN;
      deferred_token.token_data.as_literal.literal_type =
          token::data::literal::type::INTEGER;
      deferred_token.token_data.as_literal.as_integer =
          std::get<std::int64_t>(maybe);
    } else {
      builder.builder << std::get<std::string>(maybe);
      build_message(oops_compiler::logger::level::ERROR);
    }
  }
  void token_to_number() {
    switch (deferred_token.token_data.as_deferred
                .start[deferred_token.token_data.as_deferred.size - 1]) {
      case 'f':
      case 'F': {
        if (deferred_token.token_data.as_deferred.size < 3 ||
            deferred_token.token_data.as_deferred.start[0] != 0 ||
            (deferred_token.token_data.as_deferred.start[1] != 'x' &&
             deferred_token.token_data.as_deferred.start[1] != 'X')) {
          auto maybe = parse_float(deferred_token.token_data.as_deferred.start,
                                   deferred_token.token_data.as_deferred.size);
          if (std::holds_alternative<float>(maybe)) {
            deferred_token.token_data.token_type =
                token::data::type::LITERAL_TOKEN;
            deferred_token.token_data.as_literal.literal_type =
                token::data::literal::type::FLOAT;
            deferred_token.token_data.as_literal.as_float =
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
        deferred_token.token_data.as_deferred.size--;
        set_as_integer_token();
        break;
      }
      case 'd':
      case 'D': {
        if (deferred_token.token_data.as_deferred.size < 3 ||
            deferred_token.token_data.as_deferred.start[0] != 0 ||
            (deferred_token.token_data.as_deferred.start[1] != 'x' &&
             deferred_token.token_data.as_deferred.start[1] != 'X')) {
          auto maybe = parse_double(deferred_token.token_data.as_deferred.start,
                                    deferred_token.token_data.as_deferred.size);
          if (std::holds_alternative<double>(maybe)) {
            deferred_token.token_data.token_type =
                token::data::type::LITERAL_TOKEN;
            deferred_token.token_data.as_literal.literal_type =
                token::data::literal::type::DOUBLE;
            deferred_token.token_data.as_literal.as_double =
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
        if (deferred_token.token_data.as_deferred.size > 2 &&
            deferred_token.token_data.as_deferred.start[0] == '0' &&
            (deferred_token.token_data.as_deferred.start[1] == 'x' ||
             deferred_token.token_data.as_deferred.start[1] == 'X')) {
          set_as_integer_token();
          break;
        }
        for (std::int64_t i = 1; i < deferred_token.token_data.as_deferred.size;
             i++) {
          if (deferred_token.token_data.as_deferred.start[i] == 'e' ||
              deferred_token.token_data.as_deferred.start[i] == 'E') {
            auto maybe =
                parse_double(deferred_token.token_data.as_deferred.start,
                             deferred_token.token_data.as_deferred.size);
            if (std::holds_alternative<double>(maybe)) {
              deferred_token.token_data.token_type =
                  token::data::type::LITERAL_TOKEN;
              deferred_token.token_data.as_literal.literal_type =
                  token::data::literal::type::DOUBLE;
              deferred_token.token_data.as_literal.as_double =
                  std::get<double>(maybe);
            } else {
              builder.builder << std::get<std::string>(maybe);
              build_message(oops_compiler::logger::level::ERROR);
            }
            break;
          }
        }
        set_as_integer_token();
        break;
    }
  }
  void flush_token() {
    if (deferred_token.token_context.global_char_number > -1) {
      if (isdigit(deferred_token.token_data.as_deferred.start[0])) {
        token_to_number();
      } else if (auto it = all_mappings.string_to_keywords.find(
                     std::string(deferred_token.token_data.as_deferred.start,
                                 deferred_token.token_data.as_deferred.size));
                 it != all_mappings.string_to_keywords.end()) {
        deferred_token.token_data.token_type = token::data::type::KEYWORD_TOKEN;
        deferred_token.token_data.as_keyword = it->second;
      }
      out->output.push_back(deferred_token);
      deferred_token.token_data.token_type = token::data::type::DEFERRED_TOKEN;
      deferred_token.token_context.global_char_number = -1;
    }
  }
  void extend_token() {
    if (deferred_token.token_context.global_char_number == -1) {
      deferred_token.token_context = context;
      deferred_token.token_data.as_deferred.size = 0;
      deferred_token.token_data.as_deferred.start =
          start + context.global_char_number;
    }
    deferred_token.token_data.as_deferred.size++;
  }
};

lexer_variables init_lexer_values(oops_compiler::lexer::lexed_output *output) {
  lexer_variables out{};
  out.out = output;
  out.deferred_token.token_context.global_char_number = -1;
  return out;
}
}  // namespace

lexed_output lexer::lex() {
  lexed_output out{};
  out.filename = this->filename;
  lexer_variables variables = init_lexer_values(&out);
  while (variables.start + variables.context.global_char_number <
         variables.end) {
    if (auto op = parse_operator(variables.start, variables.end, &this->root)) {
      switch (op->second) {
        default:
          variables.flush_token();
          out.output.push_back(token{
              .token_data =
                  token::data{.token_type = token::data::type::OPERATOR_TOKEN,
                              .as_operator = op->second},
              .token_context = variables.context});
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
  if (*std::max_element(out.messages.begin(), out.messages.end()) <
      logger::level::ERROR) {
    variables.builder.builder << "Successfully lexed file " << this->filename
                              << "!";
    variables.build_message(logger::level::DEBUG);
  }
  std::sort(out.messages.begin(), out.messages.end());
  return out;
}
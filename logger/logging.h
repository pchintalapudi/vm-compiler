#ifndef OOPS_COMPILER_LOGGER_LOGGING_H
#define OOPS_COMPILER_LOGGER_LOGGING_H

#include <string>
#include <tuple>

namespace oops_compiler {
namespace logger {
enum class level {
  FATAL_ERROR,  // The current stage of compilation cannot proceed because there
                // is no way to figure out what was meant here. Fatal errors are
                // usually caused by lexical failures (ex. mismatched groupers,
                // missing semicolons, etc.)
  ERROR,  // The program cannot proceed to the next stage of compilation due to
          // this unresolved error. However, this stage may continue in an
          // attempt to discover additional errors.
  WARNING,  // The program can proceed to the next stage of compilation; however
            // it is likely that this is not what was intended by the programmer
            // and therefore it should be noted.
  INFO,     // Something useful to know about the current compilation process
  DEBUG     // Debug information about how the compiler is viewing the code
};

enum class stage {
  LEXING,    // Splits into tokens of operators, keywords, literals, and symbols
  PARSING,   // Constructs an abstract syntax tree from the tokens
  COMPILING  // Produces bytecode from the abstract syntax tree
};
struct context {
  const char *class_name;
  const char *method_name;
  std::int64_t line_number;
  std::int64_t char_number;
  std::int64_t global_char_number;

  bool operator==(const context &other) const {
    return std::tie(line_number, char_number, global_char_number, class_name,
                    method_name) ==
           std::tie(other.line_number, other.char_number,
                    other.global_char_number, other.class_name,
                    other.method_name);
  }

  bool operator<(const context &other) const {
    return std::tie(class_name, method_name, line_number, char_number,
                    global_char_number) <
           std::tie(other.class_name, other.method_name, other.line_number,
                    other.char_number, other.global_char_number);
  }
};

struct message {
  std::string text;
  context location;
  stage produced_at;
  level log_level;

  bool operator==(const message &other) const {
    return this->log_level == other.log_level &&
           this->produced_at == other.produced_at &&
           this->location == other.location && this->text == other.text;
  }

  bool operator<(const message &other) const {
    return std::tie(produced_at, log_level, location) <
           std::tie(other.produced_at, other.log_level, other.location);
  }

  bool operator<(level lvl) const { return this->log_level < lvl; }

  bool operator<(stage stg) const { return this->produced_at < stg; }
};

}  // namespace logger
}  // namespace oops_compiler

#endif
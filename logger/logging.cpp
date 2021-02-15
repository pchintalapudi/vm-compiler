#include "logging.h"

#include <sstream>

using namespace oops_compiler::logger;

std::string context::to_string() const {
  std::stringstream builder;
  builder << "{";
  builder << "\"line\": " << this->line_number << ",";
  builder << "\"char\": " << this->char_number << ",";
  builder << "\"global\": " << this->global_char_number;
  if (this->class_name) builder << ", \"class\": \"" << this->class_name << "\"";
  if (this->method_name) builder << ", \"method\": \"" << this->method_name << "\"";
  builder << "}";
  return builder.str();
}

std::string message::to_string() const {
    std::stringstream builder;
    builder << "{";
    builder << "\"text\": \"" << this->text << "\",";
    builder << "\"location\": " << this->location.to_string() << ",";
    builder << "\"level\": ";
    switch (this->log_level) {
        case level::DEBUG:
        builder << "\"debug\"";
        break;
        case level::INFO:
        builder << "\"info\"";
        break;
        case level::WARNING:
        builder << "\"warning\"";
        break;
        case level::ERROR:
        builder << "\"error\"";
        break;
        case level::FATAL_ERROR:
        builder << "\"fatal_error\"";
        break;
    }
    builder << ",";
    builder << "\"stage\": ";
    switch (this->produced_at) {
        case stage::LEXING:
        builder << "\"lexing\"";
        break;
        case stage::PARSING:
        builder << "\"parsing\"";
        break;
        case stage::COMPILING:
        builder << "\"compiling\"";
        break;
    }
    builder << "}";
    return builder.str();
}
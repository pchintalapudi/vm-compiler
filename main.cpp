#include <cstring>
#include <fstream>
#include <iostream>
#include <istream>
#include <ostream>

#include "files/files.h"
#include "lexer/lexer.h"

class simple_file_stream {
 private:
  FILE *file;

 public:
  simple_file_stream(const char *name) { file = fopen(name, "w"); }
  simple_file_stream &operator<<(std::string string) {
    std::string replaced;
    replaced.reserve(string.size());
    for (std::size_t i = 0; i < string.size(); i++) {
      if (string[i] == '%') {
        replaced.push_back('%');
      }
      replaced.push_back(string[i]);
    }
    fprintf(file, "%s", replaced.c_str());
    fflush(file);
    return *this;
  }
  ~simple_file_stream() { fclose(file); }
};

constexpr static auto DEBUG_FILE = "out.temp.json";

int main(int argc, char **argv) {
  for (int i = 0; i < argc; i++) {
    std::cout << argv[i] << "\n";
  }
  if (argc < 2) {
    std::cerr << "Filename to compile not provided!\n";
    return 1;
  }
  auto src = oops_compiler::files::mmap_file::create(argv[1], strlen(argv[1]));
  if (!src) {
    std::cerr << "Filename \"" << argv[1] << "\" does not exist!" << std::endl;
    return 1;
  }
  simple_file_stream debug_file(DEBUG_FILE);
  oops_compiler::lexer::lexer lexer(**src, src->file_size(), argv[1]);
  auto out = lexer.lex();
  std::cout << "Lexed!" << std::endl;
  debug_file << "{\"lexed\": ";
  debug_file << "{";
  debug_file << "\"filename\": \"" << out.filename << "\",";
  debug_file << "\"messages\": [";
  for (const auto &message : out.messages) {
    debug_file << message.to_string() << ",";
  }
  debug_file << "],";
  debug_file << "\"tokens\": [";
  for (const auto &token : out.output) {
    debug_file << token.to_string() << ",";
  }
  debug_file << "]";
  debug_file << "}";
  debug_file << ",";
  debug_file << "\"parsed\":";
  debug_file << "{";
  debug_file << "\"messages\": [";
  debug_file << "]";
  debug_file << "}";
  debug_file << "}";
  std::cout << "Finished!\n";
  return 0;
}

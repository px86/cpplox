#include <fstream>
#include <iostream>
#include <string>

#include "lox.hpp"
#include "lexer.hpp"

int main(int argc, char **argv) {
  if (argc > 2) {
    std::cerr << "Usage: lox [SCRIPT]\n";
    return 1;
  }
  if (argc == 2) {
    try {
      run_file(argv[1]);
    } catch (std::ios_base::failure &e) {
      std::cerr << "Error: can not read file: " << argv[1] << '\n';
      return 1;
    }
  } else { run_prompt(); }

  return 0;
}

void run_file(const char *filepath) {
  std::size_t read_size = 4096;

  auto stream = std::ifstream(filepath);
  stream.exceptions(std::ios_base::badbit);

  auto source = std::string();
  auto buf = std::string(read_size, '\0');
  while (stream.read(&buf[0], read_size)) {
    source.append(buf, 0, stream.gcount());
  }
  source.append(buf, 0, stream.gcount());

  run(source);
}

void run_prompt() {
  std::string source;
  while (std::cin) {
    std::cout << "> ";
    std::getline(std::cin, source);
    if (source.empty()) continue;
    run(source);
  }
}

void run(std::string &source) {
  auto lexer = Lexer(source);
  auto tokens = lexer.lex();
  for (auto token: tokens) {
    std::cout << token.to_string() << std::endl;
  }
  // TODO: next parse
}

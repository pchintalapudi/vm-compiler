#include "basic_block.h"
#include "expression.h"
#include "if.h"
#include "semicolon.h"
#include "statement.h"
#include "while.h"

using namespace oops_compiler::parser;

std::pair<oops_compiler::parser::expression, std::size_t>
expression::parse_parenthetical(std::vector<lexer::token> tokens,
                                std::size_t start) {
  auto subexpr = expression::parse(tokens, start + 1);
  return {subexpr.first, subexpr.second + 1};
}

std::pair<oops_compiler::parser::expression, std::size_t>
expression::parse_indexer(std::vector<lexer::token> tokens, std::size_t start) {
  auto subexpr = expression::parse(tokens, start + 1);
  return {subexpr.first, subexpr.second + 1};
}

std::pair<oops_compiler::parser::while_statement, std::size_t>
while_statement::parse(std::vector<lexer::token> tokens, std::size_t start) {
  auto conditional = expression::parse_parenthetical(tokens, start + 1);
  auto statement = statement::parse(tokens, conditional.second);
  return {while_statement(conditional.first, statement.first),
          statement.second};
}

std::pair<oops_compiler::parser::if_statement, std::size_t> if_statement::parse(
    std::vector<lexer::token> tokens, std::size_t start) {
  auto conditional = expression::parse_parenthetical(tokens, start + 1);
  auto statement = statement::parse(tokens, conditional.second);
  return {if_statement(conditional.first, statement.first), statement.second};
}

std::pair<oops_compiler::parser::else_statement, std::size_t>
else_statement::parse(std::vector<lexer::token> tokens, std::size_t start) {
  auto statement = statement::parse(tokens, start + 1);
  return {else_statement(statement.first), statement.second};
}

std::pair<oops_compiler::parser::basic_block, std::size_t> basic_block::parse(
    std::vector<lexer::token> tokens, std::size_t start) {
  std::vector<statement> statements;
  start += 1;
  while (tokens[start].token_data.token_type !=
             lexer::token::data::type::OPERATOR_TOKEN ||
         tokens[start].token_data.as_operator !=
             lexer::operators::CURLY_CLOSE) {
    auto statement = statement::parse(tokens, start);
    start = statement.second;
    statements.push_back(statement.first);
  }
  return {basic_block(statements), start + 1};
}

std::pair<statement, std::size_t> statement::parse(
    std::vector<lexer::token> tokens, std::size_t start) {
  switch (tokens[start].token_data.token_type) {
    case lexer::token::data::type::KEYWORD_TOKEN:
      switch (tokens[start].token_data.as_keyword) {
        case lexer::keywords::IF:
          return if_statement::parse(tokens, start);
        case lexer::keywords::ELSE:
          return else_statement::parse(tokens, start);
        case lexer::keywords::WHILE:
          return while_statement::parse(tokens, start);
      }
      break;
    case lexer::token::data::type::OPERATOR_TOKEN:
      switch (tokens[start].token_data.as_operator) {
        case lexer::operators::CURLY_OPEN:
          return basic_block::parse(tokens, start);
      }
      break;
  }
  return semicolon_statement::parse(tokens, start);
}
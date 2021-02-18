#include "converters.h"

using namespace oops_compiler::lexer;
#define current_line_number(variable_name) \
  constexpr std::size_t variable_name = __LINE__;

mappings::mappings() {
#define map_operator(operator, string)                                     \
  this->operators_to_strings[static_cast<std::size_t>(operator)] = string; \
  this->string_to_operators[string] = operator;                            \
  static_assert(static_cast<std::size_t>(operator) ==                      \
                __LINE__ - operators_start - 1)

#define map_keyword(keyword, string)                                     \
  this->keywords_to_strings[static_cast<std::size_t>(keyword)] = string; \
  this->string_to_keywords[string] = keyword;                            \
  static_assert(static_cast<std::size_t>(keyword) ==                     \
                __LINE__ - keywords_start - 1)
  // DON'T INTRODUCE EXTRA LINES WITHIN THE BELOW BLOCK
  // The line numbers are used for verification that every operator is covered
  {
    current_line_number(operators_start);
    map_operator(operators::ACCESS, ".");
    map_operator(operators::ADD, "+");
    map_operator(operators::ADDEQ, "+=");
    map_operator(operators::AND, "&&");
    map_operator(operators::BITAND, "&");
    map_operator(operators::BITANDEQ, "&=");
    map_operator(operators::BITOR, "|");
    map_operator(operators::BITOREQ, "|=");
    map_operator(operators::BITSLL, "<<");
    map_operator(operators::BITSLLEQ, "<<=");
    map_operator(operators::BITSRA, ">>");
    map_operator(operators::BITSRAEQ, ">>=");
    map_operator(operators::BITSRL, ">>>");
    map_operator(operators::BITSRLEQ, ">>>=");
    map_operator(operators::BITXOR, "^");
    map_operator(operators::BITXOREQ, "^=");
    map_operator(operators::BITNOT, "~");
    map_operator(operators::BLOCK_COMMENT_CLOSE, "*/");
    map_operator(operators::BLOCK_COMMENT_OPEN, "/*");
    map_operator(operators::CAST, "->");
    map_operator(operators::COLON, ":");
    map_operator(operators::COMMA, ",");
    map_operator(operators::CURLY_CLOSE, "}");
    map_operator(operators::CURLY_OPEN, "{");
    map_operator(operators::DEC, "--");
    map_operator(operators::DIV, "/");
    map_operator(operators::DIVEQ, "/=");
    map_operator(operators::EQ, "=");
    map_operator(operators::EQUALS, "==");
    map_operator(operators::FATARROW, "=>");
    map_operator(operators::GEQUALS, ">=");
    map_operator(operators::GREATER, ">");
    map_operator(operators::INC, "++");
    map_operator(operators::LEQUALS, "<=");
    map_operator(operators::LESS, "<");
    map_operator(operators::LINE_COMMENT, "//");
    map_operator(operators::LNOT, "!");
    map_operator(operators::MOD, "%");
    map_operator(operators::MODEQ, "%=");
    map_operator(operators::MUL, "*");
    map_operator(operators::MULEQ, "*=");
    map_operator(operators::NEQUALS, "!=");
    map_operator(operators::NULLACCESS, "?.");
    map_operator(operators::OR, "||");
    map_operator(operators::ROUND_CLOSE, ")");
    map_operator(operators::ROUND_OPEN, "(");
    map_operator(operators::SCOPE, "::");
    map_operator(operators::SEMICOLON, ";");
    map_operator(operators::SQUARE_CLOSE, "]");
    map_operator(operators::SQUARE_OPEN, "[");
    map_operator(operators::SUB, "-");
    map_operator(operators::SUBEQ, "-=");
    map_operator(operators::TERNARY_START, "?");
    current_line_number(operators_end);
    constexpr std::size_t operator_count = operators_end - operators_start - 1;
    static_assert(operator_count ==
                  static_cast<std::size_t>(operators::__COUNT__));
    //["\\.","\\+","\\+=","&&","&","&=","\\|","\\|=","<<","<<=",">>",">>=",">>>",">>>=","\\^","\\^=","~","\\*/","/\\*","->",":",",","}","{","--","/","/=","=","==","=>",">=",">","\\+\\+","<=","<","//","!","%","%=","\\*","\\*=","!=","\\?\\.","\\|\\|","\\)","\\(","::","]","\\[","-","-=","\\?"]
    //"(?:>>>=)|(?:\\+\\+)|(?:\\?\\.)|(?:\\|\\|)|(?:\\+=)|(?:\\|=)|(?:<<=)|(?:>>=)|(?:>>>)|(?:\\^=)|(?:\\*/)|(?:/\\*)|(?:\\*=)|(?:\\.)|(?:\\+)|(?:&&)|(?:&=)|(?:\\|)|(?:<<)|(?:>>)|(?:\\^)|(?:->)|(?:--)|(?:/=)|(?:==)|(?:=>)|(?:>=)|(?:<=)|(?://)|(?:%=)|(?:\\*)|(?:!=)|(?:\\))|(?:\\()|(?:::)|(?:\\[)|(?:-=)|(?:\\?)|(?:&)|(?:~)|(?::)|(?:,)|(?:})|(?:{)|(?:/)|(?:=)|(?:>)|(?:<)|(?:!)|(?:%)|(?:])|(?:-)"

    current_line_number(keywords_start);
    map_keyword(keywords::AS, "as");
    map_keyword(keywords::AUTO, "auto");
    map_keyword(keywords::BOOLEAN, "bool");
    map_keyword(keywords::BREAK, "break");
    map_keyword(keywords::BYTE, "byte");
    map_keyword(keywords::CASE, "case");
    map_keyword(keywords::CATCH, "catch");
    map_keyword(keywords::CLASS, "class");
    map_keyword(keywords::CONSTRUCTOR, "constructor");
    map_keyword(keywords::CONTINUE, "continue");
    map_keyword(keywords::DEFAULT, "default");
    map_keyword(keywords::DEF, "def");
    map_keyword(keywords::DO, "do");
    map_keyword(keywords::DOUBLE, "double");
    map_keyword(keywords::ELSE, "else");
    map_keyword(keywords::ENUM, "enum");
    map_keyword(keywords::EXTENDS, "extends");
    map_keyword(keywords::FALSE, "false");
    map_keyword(keywords::FINAL, "final");
    map_keyword(keywords::FINALLY, "finally");
    map_keyword(keywords::FLOAT, "float");
    map_keyword(keywords::FOR, "for");
    map_keyword(keywords::GET, "get");
    map_keyword(keywords::GOTO, "goto");
    map_keyword(keywords::IF, "if");
    map_keyword(keywords::IMPORT, "import");
    map_keyword(keywords::IMPLEMENTS, "implements");
    map_keyword(keywords::INT, "int");
    map_keyword(keywords::INTERFACE, "interface");
    map_keyword(keywords::INSTANCEOF, "instanceof");
    map_keyword(keywords::INTRINSIC, "intrinsic");
    map_keyword(keywords::LONG, "long");
    map_keyword(keywords::NATIVE, "native");
    map_keyword(keywords::NEW, "new");
    map_keyword(keywords::NIL, "null");
    map_keyword(keywords::OPERATOR, "operator");
    map_keyword(keywords::PACKAGE, "package");
    map_keyword(keywords::PRIVATE, "private");
    map_keyword(keywords::PROTECTED, "protected");
    map_keyword(keywords::PUBLIC, "public");
    map_keyword(keywords::RETURN, "return");
    map_keyword(keywords::SET, "set");
    map_keyword(keywords::SHORT, "short");
    map_keyword(keywords::STATIC, "static");
    map_keyword(keywords::SUPER, "super");
    map_keyword(keywords::SWITCH, "switch");
    map_keyword(keywords::THIS, "this");
    map_keyword(keywords::THROW, "throw");
    map_keyword(keywords::TRUE, "true");
    map_keyword(keywords::TRY, "try");
    map_keyword(keywords::USING, "using");
    map_keyword(keywords::VOID, "void");
    map_keyword(keywords::WHILE, "while");
    current_line_number(keywords_end);
    constexpr std::size_t keyword_count = keywords_end - keywords_start - 1;
    static_assert(keyword_count ==
                  static_cast<std::size_t>(keywords::__COUNT__));
  }
#undef map_operator
#undef map_keyword
#undef current_line_number
}

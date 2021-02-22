#include "token.h"
#include <sstream>

using namespace oops_compiler::lexer;

std::string token::to_string() const {
    std::stringstream builder;
    builder << "{";
    builder << "\"context\": " << this->token_context.to_string() << ",";
    switch (this->token_data.token_type)
    {
        case token::data::type::OPERATOR_TOKEN:
        builder << "\"type\": " << "\"operator\"" << ",";
        builder << "\"data\": \"" << all_mappings.operators_to_strings[static_cast<unsigned>(this->token_data.as_operator)] << "\"";
        break;
        case token::data::type::KEYWORD_TOKEN:
        builder << "\"type\": " << "\"keyword\"" << ",";
        builder << "\"data\": \"" << all_mappings.keywords_to_strings[static_cast<unsigned>(this->token_data.as_keyword)] << "\"";
        break;
        case token::data::type::IDENTIFIER_TOKEN:
        builder << "\"type\": " << "\"identifier\"" << ",";
        builder << "\"data\": \"" << std::string(token_data.as_identifier.start, token_data.as_identifier.size) << "\"";
        break;
        case token::data::type::LITERAL_TOKEN:
        builder << "\"type\": " << "\"literal\"" << ",";
        switch (token_data.as_literal.literal_type) {
            case token::data::literal::type::INTEGER:
            builder << "\"literal_type\": " << "\"integer\"" << ",";
            builder << "\"data\": " << token_data.as_literal.as_integer;
            break;
            case token::data::literal::type::FLOAT:
            builder << "\"literal_type\": " << "\"float\"" << ",";
            builder << "\"data\": " << token_data.as_literal.as_float;
            break;
            case token::data::literal::type::DOUBLE:
            builder << "\"literal_type\": " << "\"double\"" << ",";
            builder << "\"data\": " << token_data.as_literal.as_double;
            break;
            case token::data::literal::type::STRING:
            builder << "\"literal_type\": " << "\"string\"" << ",";
            std::string literal;
            literal.reserve(token_data.as_literal.as_string.size);
            for (std::size_t i = 0; i < literal.size(); i++) {
                char c = token_data.as_literal.as_string.start[i];
                if (c == '\\') literal.push_back('\\');
                literal.push_back(token_data.as_literal.as_string.start[i]);
            }
            builder << "\"data\": \"" << literal << "\"";
            break;
        }
        break;
    }
    builder << "}";
    return builder.str();
}
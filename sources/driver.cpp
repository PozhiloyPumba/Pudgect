#include "driver.hpp"
#include <stack>

namespace yy {

    parser::token_type Driver::yylex (parser::semantic_type *yylval, parser::location_type *location)
    {
        parser::token_type tokenT = static_cast<parser::token_type> (lexer_->yylex ());

        switch (tokenT) {
            case yy::parser::token_type::ID: {
                yylval->build<std::string> () = lexer_->YYText ();
                break;
            }
            case yy::parser::token_type::LEXERR: {
                throw std::runtime_error ("Unexpected word");
            }
            default:;
        }

        return tokenT;
    }

}
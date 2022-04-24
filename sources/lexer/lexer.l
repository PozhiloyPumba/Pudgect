%option c++

%option yyclass="SATLexer"

%{
    #include <stdio.h>
    #include "grammar.tab.hh"
    
    #include "../sources/lexer/lexer.hpp"
    #define YY_USER_ACTION setLocation ();
    #define yyterminate() return yy::parser::token_type::END
    int yyFlexLexer::yywrap () { return 1; }

%}

TRASH_SYMBS             [ \t\n\r\v]+
DIGIT                   [0-9]

ALPHA                   [a-zA-Z]

WORD                    ({ALPHA}|"_")({ALPHA}|{DIGIT}|"_")*

%%

{TRASH_SYMBS}           {   /*skip*/    }

"&"                     {   return yy::parser::token_type::AND;         }
"|"                     {   return yy::parser::token_type::OR;          }
"->"                    {   return yy::parser::token_type::IMPL;        }
"("                     {   return yy::parser::token_type::LBRAC;       }
")"                     {   return yy::parser::token_type::RBRAC;       }
"~"                     {   return yy::parser::token_type::NOT;         }
"1"                     {   return yy::parser::token_type::TAUT;        }
"0"                     {   return yy::parser::token_type::FALSE;       }
{WORD}                  {   return yy::parser::token_type::ID;          }
";"                     {   return yy::parser::token_type::SEMICOLON;   }
":"                     {   return yy::parser::token_type::COLON;       }
"="                     {   return yy::parser::token_type::EQUAL;       }

.                       {   return yy::parser::token_type::LEXERR;      }

%%
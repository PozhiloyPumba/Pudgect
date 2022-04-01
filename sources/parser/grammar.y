%language "c++"

%skeleton "lalr1.cc"

%defines
%define lr.type ielr
%define api.value.type variant

%param {yy::Form* driver}

%code requires
{
#include <iostream>
#include <string>
#include "include/tree.hpp"
#include "include/node.hpp"
namespace yy {class Form;};

}

%code
{
#include "include/driver.hpp"
#include <string>

namespace yy {

    parser::token_type yylex(parser::semantic_type* yylval, parser::location_type* location, Form *driver);

}
}

%locations

%token <std::string>        ID

%token                      AND      "&"
%token                      OR       "|"
%token                      IMPL     "->"
%token                      LBRAC    "("
%token                      RBRAC    ")"
%token                      NOT      "~"
%token                      TAUT     "1"
%token                      FALSE    "0"
%token                      LEXERR
%token                      END         0   "end of file"

/*PRIORITIES*/
%left IMPL
%left OR
%left AND
%left NOT

/* TREE */
%type <SAT::Node*> expr

%start start

%%
start       :   expr                {
                                        driver->setRoot ($1);
                                    }

expr        :   expr IMPL expr      {
                                        auto node = new SAT::OperNode (SAT::OperNode::OperType::IMPL);
                                        node->left_ = $1;
                                        node->right_ = $3;

                                        $$ = node;
                                    }
            |   expr OR expr        {
                                        auto node = new SAT::OperNode (SAT::OperNode::OperType::OR);
                                        node->left_ = $1;
                                        node->right_ = $3;

                                        $$ = node;
                                    }
            |   expr AND expr       {
                                        auto node = new SAT::OperNode (SAT::OperNode::OperType::AND);
                                        node->left_ = $1;
                                        node->right_ = $3;

                                        $$ = node;
                                    }
            |   NOT expr            {
                                        auto node = new SAT::OperNode (SAT::OperNode::OperType::NOT);
                                        node->left_ = $2;

                                        $$ = node;
                                    }
            |   ID                  {
                                        $$ = new SAT::VarNode ($1);
                                    }
            |   TAUT                {
                                        $$ = new SAT::ConstNode (true);
                                    }
            |   FALSE               {
                                        $$ = new SAT::ConstNode (false);
                                    }
            |   LBRAC expr RBRAC    {
                                        $$ = $2;
                                    }
            |   error               {   /*TODO error handling*/
                                        $$ = nullptr;
                                    };
%%

namespace yy {

parser::token_type yylex (parser::semantic_type* yylval, parser::location_type* location, Form* driver) {
    
    try {

        return driver->yylex (yylval, location);
    
    } catch (std::runtime_error& err) {

        std::cout << err.what () << std::endl;
        throw err; 

    }

}
void parser::error (const parser::location_type& location, const std::string& what) {

}
}
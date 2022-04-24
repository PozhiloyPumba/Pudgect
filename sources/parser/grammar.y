%language "c++"

%skeleton "lalr1.cc"

%defines
%define lr.type ielr
%define api.value.type variant

%param {yy::Driver* driver}

%code requires
{
#include <iostream>
#include <string>
#include <utility>
#include "include/tree.hpp"
#include "include/node.hpp"
namespace yy {class Driver;};

}

%code
{
#include "include/driver.hpp"
#include <string>

namespace yy {

    parser::token_type yylex(parser::semantic_type* yylval, parser::location_type* location, Driver *driver);

}
}

%locations

%token <std::string>        ID

%token                      AND         "&"
%token                      OR          "|"
%token                      IMPL        "->"
%token                      LBRAC       "("
%token                      RBRAC       ")"
%token                      NOT         "~"
%token                      TAUT        "1"
%token                      FALSE       "0"
%token                      SEMICOLON   ";"
%token                      COLON       ":"
%token                      COMMA       ","
%token                      EQUAL       "="
%token                      LEXERR
%token                      END         0   "end of file"

/*PRIORITIES*/
%left IMPL
%left OR
%left AND
%left NOT

/* TREE */
%type <SAT::Node*> expr
%type <void *> eval

%start start

%%
start       :   expr COLON eval SEMICOLON       {
                                                    driver->setRoot ($1);
                                                }
            |   expr SEMICOLON                  {
                                                    driver->setRoot ($1);
                                                };

expr        :   expr IMPL expr      {
                                        if ($1 && $3) {
                                            auto node = new SAT::OperNode (SAT::OperNode::OperType::IMPL);
                                            node->left_ = $1;
                                            node->right_ = $3;
                                            $1->setParent (node);
                                            $3->setParent (node);
                                            $$ = node;
                                        }
                                        else {
                                            delete $1;
                                            delete $3;
                                            $$ = nullptr;
                                        }
                                    }
            |   expr OR expr        {
                                        if ($1 && $3) {
                                            auto node = new SAT::OperNode (SAT::OperNode::OperType::OR);
                                            node->left_ = $1;
                                            node->right_ = $3;
                                            $1->setParent (node);
                                            $3->setParent (node);
                                            $$ = node;
                                        }
                                        else {
                                            delete $1;
                                            delete $3;
                                            $$ = nullptr;
                                        }
                                    }
            |   expr AND expr       {
                                        if ($1 && $3) {
                                            auto node = new SAT::OperNode (SAT::OperNode::OperType::AND);
                                            node->left_ = $1;
                                            node->right_ = $3;
                                            $1->setParent (node);
                                            $3->setParent (node);
                                            $$ = node;
                                        }
                                        else {
                                            delete $1;
                                            delete $3;
                                            $$ = nullptr;
                                        }
                                    }
            |   NOT expr            {
                                        if ($2) {
                                            auto node = new SAT::OperNode (SAT::OperNode::OperType::NOT);
                                            node->left_ = $2;
                                            $2->setParent (node);

                                            $$ = node;
                                        }
                                        else {
                                            delete $2;
                                            $$ = nullptr;
                                        }
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
            |   error SEMICOLON     {
                                        driver->pushError (@1, "Undefined syntax in form");    $$ = nullptr;
                                    };

eval        :   ID EQUAL TAUT    COMMA eval           {   driver->addEvalInfo (std::make_pair ($1, true));  }
            |   ID EQUAL FALSE   COMMA eval           {   driver->addEvalInfo (std::make_pair ($1, false)); }
            |   ID EQUAL FALSE                        {   driver->addEvalInfo (std::make_pair ($1, false)); }
            |   ID EQUAL TAUT                         {   driver->addEvalInfo (std::make_pair ($1, true)); }
            |   error SEMICOLON                       {   driver->pushError (@1, "Undefined syntax in eval");    $$ = nullptr;    };
%%

namespace yy {

parser::token_type yylex (parser::semantic_type* yylval, parser::location_type* location, Driver* driver) {
    
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
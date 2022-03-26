#ifndef DRIVER_HPP__
#define DRIVER_HPP__

#include <FlexLexer.h>
#include <memory>
#include "tree.hpp"
#include "node.hpp"
#include "lexer.hpp"
#include "grammar.tab.hh"

namespace yy {

    class Driver final {
        std::unique_ptr<SATLexer> lexer_;
        Tree::BinaryTree<SAT::Node *> tree_;
    
    public:
        Driver (): lexer_ (std::unique_ptr<SATLexer> {new SATLexer}) {}

        bool parse ()
        {
            parser parser (this);
            bool res = parser.parse ();
            return !res;
        }

        parser::token_type yylex (parser::semantic_type *yylval, parser::location_type *location)
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
            }

            return tokenT;
        }

        void setRoot (SAT::Node *node) { tree_.setRoot (node); }
        SAT::Node *getRoot () { return tree_.getRoot (); }

        void callDump (std::ostream &out) { tree_.dump (out); }
    };
}

#endif
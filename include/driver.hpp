#ifndef DRIVER_HPP__
#define DRIVER_HPP__

#include <FlexLexer.h>

#include <memory>
#include <stack>
#include <utility>
#include <algorithm>
#include <iostream>

#include "grammar.tab.hh"
#include "lexer.hpp"
#include "node.hpp"
#include "tree.hpp"

namespace yy {

    class Form final {
        std::unique_ptr<SATLexer> lexer_;
        Tree::BinaryTree<SAT::Node *> tree_;
        std::vector<std::pair<std::string, bool>> evalInfo_;
        
        void recToString (SAT::Node *curRoot, std::string &str) const;

        void deleteImplication ();
        void deMorgan ();
        void deleteDoubleNeg ();
        void lawOfDistr ();
        
    public:
        Form () : lexer_ (std::unique_ptr<SATLexer>{new SATLexer}) {}

        bool parse ()
        {
            parser parser (this);
            bool res = parser.parse ();
            return !res;
        }

        void toCNF ()
        {
            deleteImplication ();
            deMorgan ();
            deleteDoubleNeg ();
            lawOfDistr ();
        }

        std::string toString () const; // func which cast form to string

        parser::token_type yylex (parser::semantic_type *yylval, parser::location_type *location);

        void setRoot (SAT::Node *node) { tree_.setRoot (node); }
        
        SAT::Node *getRoot () { return tree_.getRoot (); }
        
        void addEvalInfo (const std::pair<std::string, bool> &var) {  evalInfo_.push_back (var);  }
        
        void printEvalInfo () const {
            std::for_each (evalInfo_.begin (), evalInfo_.end (), 
            [] (const std::pair<std::string, bool> &forDump) {std::cout << forDump.first << "=" << forDump.second;});
        }

        void callDump (std::ostream &out) { tree_.dump (out); }
    };
}  // namespace yy

#endif
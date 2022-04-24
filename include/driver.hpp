#ifndef DRIVER_HPP__
#define DRIVER_HPP__

#include <FlexLexer.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <stack>
#include <unordered_map>
#include <utility>

#include "grammar.tab.hh"
#include "lexer.hpp"
#include "node.hpp"
#include "tree.hpp"

namespace yy {

    class Driver {
        std::unique_ptr<SATLexer> lexer_;
        Tree::BinaryTree<SAT::Node *> tree_;
        std::unordered_map<std::string, bool> evalInfo_;
        std::vector<std::string> error_;

    public:
        Driver () : lexer_ (std::unique_ptr<SATLexer>{new SATLexer})
        {
        }

        inline bool parse ()
        {
            parser parser (this);
            bool res = parser.parse ();
            std::for_each (error_.begin (), error_.end (), [] (const std::string &forDump) { std::cout << forDump << std::endl; });

            return !res;
        }

        parser::token_type yylex (parser::semantic_type *yylval, parser::location_type *location);

        void setRoot (SAT::Node *node) { tree_.setRoot (node); }

        SAT::Node *getRoot () const { return tree_.getRoot (); }

        void addEvalInfo (const std::pair<std::string, bool> &var) { evalInfo_.insert (var); }

        inline void pushError (yy::location curLocation, const std::string &err)
        {
            std::string errPos = std::string ("ERROR::#") + std::to_string (curLocation.begin.line) + std::string (": ");

            error_.push_back (errPos + err);
        }

        inline void printEvalInfo () const
        {
            std::for_each (evalInfo_.begin (), evalInfo_.end (), [] (const std::pair<std::string, bool> &forDump) { std::cout << forDump.first << "=" << forDump.second; });
        }

        inline void callDump (std::ostream &out) const { tree_.dump (out); }

        inline void swapEvalInfo (std::unordered_map<std::string, bool> &forSwap) { evalInfo_.swap (forSwap); }
    };

}  // namespace yy

#endif
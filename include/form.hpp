#ifndef Form_HPP__
#define Form_HPP__

#include <stack>
#include <utility>
#include <algorithm>
#include <iostream>
#include <unordered_map>

#include "driver.hpp"
#include "node.hpp"
#include "tree.hpp"

namespace SAT {
    
    class Form final {
        Tree::BinaryTree<SAT::Node *> tree_;
        std::unordered_map<std::string, bool> evalInfo_;
        
        bool isCNF = false;

        void recToString (SAT::Node *curRoot, std::string &str) const;

        void deleteImplication ();
        bool recDeMorgan (SAT::Node *curNode);
        void lambDeMorgan (SAT::Node *curNode, SAT::Node *newOp);
        void deMorgan ();
        void deleteDoubleNeg ();
        void lawOfDistr ();
        void simplifyAllConjuncts ();
        void simplifyOneConjunct (SAT::Node *rootOfConjunct);
        void destructSubNode (std::list<SAT::Node *> &preorder, SAT::Node *curRoot);
        void deleteSubNode (std::list<SAT::Node *> &preorder, SAT::Node *subroot);
    
        public:
        Form () = default;
        
        bool input ();

        void toCNF ();

        void simplify ();
        void evaluate ();   //maybe const

        std::string toString () const; // func which cast form to string
        
        void printEvalInfo () const;

        inline void callDump (std::ostream &out) const { tree_.dump (out); }
    };
}
#endif
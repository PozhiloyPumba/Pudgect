#ifndef Form_HPP__
#define Form_HPP__

#include <algorithm>
#include <iostream>
#include <stack>
#include <unordered_map>
#include <utility>

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
        void deleteSubNode (std::list<SAT::Node *> &preorder, SAT::Node *subroot, SAT::Node *&curRoot);
        void changingEvaluate ();

    public:
        Form () = default;
        Form (const Form &rhs);
        Form (Form &&rhs);

        bool input ();

        void toCNF ();

        void simplify ();
        Form evaluate () const;  // maybe const

        std::string toString () const;  // func which cast form to string

        void printEvalInfo () const;

        inline void callDump (std::ostream &out) const { tree_.dump (out); }
    };
}  // namespace SAT
#endif
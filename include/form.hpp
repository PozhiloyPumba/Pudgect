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

        bool isCNF_ = false;
        bool isSimple_ = false;

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
        Form evaluate () const;

        std::string toString () const;  // func which cast form to string

        void printEvalInfo () const;

        inline bool isCNF () const { return isCNF_; }
        inline bool isSimple () const { return isSimple_; }
        inline void callDump (std::ostream &out) const { tree_.dump (out); }
        inline auto *getRoot () const { return tree_.getRoot (); }
    };

    class CNF_3 final {
        std::unordered_map <std::string, int> converter_;
        std::unordered_map <int, std::string> reverseConverter_;

        std::vector <std::vector<std::pair<int, bool>>> form_;
        int numberOfFakeVars_ = 0;
        void init (const Form &form);
        void conjunctCast (SAT::Node *curNode);

    public:
        CNF_3 (const Form &form);
        std::string toString () const;  // func which cast form to string
    };
}  // namespace SAT
#endif
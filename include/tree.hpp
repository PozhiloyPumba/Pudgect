#ifndef NARYTREE_HPP__
#define NARYTREE_HPP__

#include <iostream>
#include <stack>
#include <vector>

namespace Tree {

    template <typename T>
    class BinaryTree final {
        T root_ = nullptr;

        void PrintNodeIntoGraphviz (T curNode, std::ostream &out) const
        {
            out << "\"" << curNode << "\" [label = \"";
            curNode->nodeDump (out);
            out << "\"]\n";

            if (curNode->left_)
                PrintNodeIntoGraphviz (curNode->left_, out);
            if (curNode->right_)
                PrintNodeIntoGraphviz (curNode->right_, out);
        }

        void BuildConnectionsInGraphviz (T curNode, std::ostream &out) const
        {
            if (curNode->left_)
                out << "\"" << curNode << "\" -> \"" << curNode->left_
                    << "\" [color=\"black\"]\n";

            if (curNode->right_)
                out << "\"" << curNode << "\" -> \"" << curNode->right_
                    << "\" [color=\"black\"]\n";

            if (curNode->getParent ())
                out << "\"" << curNode << "\" -> \"" << curNode->getParent ()
                    << "\" [color=\"red\"]\n";

            if (curNode->left_)
                BuildConnectionsInGraphviz (curNode->left_, out);
            if (curNode->right_)
                BuildConnectionsInGraphviz (curNode->right_, out);
        }

    public:
        BinaryTree (T root = nullptr) : root_ (root) {}

        ~BinaryTree ()
        {
            T curNode = root_;
            if (!root_)
                return;

            std::stack<T> stack;
            std::vector<T> queueOnDelete;
            stack.push (curNode);

            while (stack.size ()) {
                curNode = stack.top ();
                stack.pop ();
                if (curNode) {
                    queueOnDelete.push_back (curNode);

                    auto childrenSt = curNode->left_;
                    auto childrenFin = curNode->right_;

                    stack.push (childrenSt);
                    stack.push (childrenFin);
                }
            }

            for (int i = queueOnDelete.size () - 1; i >= 0; --i)
                delete queueOnDelete[i];
        }

        // Rule of 0
        BinaryTree (const BinaryTree &) = delete;
        BinaryTree (BinaryTree &&) = delete;
        BinaryTree &operator= (const BinaryTree &) = delete;
        BinaryTree &operator= (BinaryTree &&) = delete;

        void dump (std::ostream &out) const
        {
            if (!root_) {
                return;
            }

            out << "digraph {\n"
                   "rankdir = \"TB\"\n"
                   "node [fontsize=10, shape=box, height=0.5]\n"
                   "edge [fontsize=10]\n";

            PrintNodeIntoGraphviz (root_, out);
            BuildConnectionsInGraphviz (root_, out);

            out << "}\n";
        }

        // Setters and getters
        void setRoot (T root) { root_ = root; }

        T getRoot () const { return root_; }
    };
}  // namespace Tree

#endif
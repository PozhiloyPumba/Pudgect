#ifndef NODE_HPP__
#define NODE_HPP__

#include <iostream>
#include <stack>
#include <string>

namespace SAT {

    class Node {
    public:
        enum class NodeT { VARIABLE,
                           OPERATOR,
                           CONST };

    protected:
        Node *parent_;
        const NodeT type_;

    public:
        Node *left_ = nullptr;
        Node *right_ = nullptr;
        Node (const NodeT type, Node *parent = nullptr)
            : parent_ (parent), type_ (type) {}

        Node (const Node &other) = delete;
        Node (Node &&other) = delete;
        Node &operator= (const Node &other) = delete;
        Node &operator= (Node &&other) = delete;
        virtual ~Node () = default;

        virtual Node *clone (Node *parent = nullptr) const = 0;

        NodeT getType () const { return type_; }

        void setParent (Node *parent) { parent_ = parent; }

        Node *getParent () const { return parent_; }
        virtual void nodeDump (std::ostream &out) const = 0;
        virtual std::string getNodeForDump () const = 0;

        Node *copySubTree (Node *parent = nullptr)
        {
            Node *curCopy = clone (parent);

            if (left_)
                curCopy->left_ = left_->copySubTree (curCopy);
            if (right_)
                curCopy->right_ = right_->copySubTree (curCopy);
            return curCopy;
        }
    };

    class VarNode final : public Node {
        std::string name_;

    public:
        VarNode (const std::string &name, Node *parent = nullptr)
            : Node (NodeT::VARIABLE, parent), name_ (name) {}

        void nodeDump (std::ostream &out) const override { out << name_; }

        std::string getName () const { return name_; }
        std::string getNodeForDump () const override { return name_; }

        Node *clone (Node *parent = nullptr) const override
        {
            return new VarNode (name_, parent);
        }
    };

    class OperNode final : public Node {
    public:
        // the members of this enumerate are arranged in order of precedence
        enum class OperType {
            IMPL,  // a -> b
            OR,    // a | b
            AND,   // a & b
            NOT    // ~a
        };

    private:
        OperType opType_;

    public:
        OperNode (const OperType opType, Node *parent = nullptr)
            : Node (NodeT::OPERATOR, parent), opType_ (opType) {}

        OperType getOpType () const { return opType_; }

        void nodeDump (std::ostream &out) const override
        {
            switch (opType_) {
                case OperType::IMPL:
                    out << "IMPL (->)";
                    break;
                case OperType::OR:
                    out << "OR (|)";
                    break;
                case OperType::AND:
                    out << "AND (&)";
                    break;
                case OperType::NOT:
                    out << "NOT (~)";
                    break;
                default:
                    out << "Unexpected operator type!";
            }
        }
        std::string getNodeForDump () const override
        {
            switch (opType_) {
                case OperType::IMPL:
                    return "->";
                case OperType::OR:
                    return "|";
                case OperType::AND:
                    return "&";
                case OperType::NOT:
                    return "~";
                default:
                    return "Unexpected operator type!";
            }
        }

        Node *clone (Node *parent = nullptr) const override
        {
            return new OperNode (opType_, parent);
        }
    };

    class ConstNode final : public Node {
        bool value_;

    public:
        ConstNode (bool value, Node *parent = nullptr)
            : Node (NodeT::CONST, parent), value_ (value) {}

        void nodeDump (std::ostream &out) const override { out << value_; }

        bool getVal () const { return value_; }
        std::string getNodeForDump () const override { return std::to_string (value_); }

        Node *clone (Node *parent = nullptr) const override
        {
            return new ConstNode (value_, parent);
        }
    };
}  // namespace SAT

#endif
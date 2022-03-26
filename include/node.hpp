#ifndef NODE_HPP__
#define NODE_HPP__

#include <iostream>
#include <string>

namespace SAT {

    enum class NodeT {

        VARIABLE,
        OPERATOR,
        CONST
    };

    class Node {
        Node *parent_;

        const NodeT type_;

    public:
        Node *left_ = nullptr;
        Node *right_ = nullptr;
        Node (const NodeT type, Node *parent = nullptr) : parent_ (parent), type_ (type) {}

        Node (const Node &other) = delete;
        Node (Node &&other) = delete;
        Node &operator= (const Node &other) = delete;
        Node &operator= (Node &&other) = delete;
        virtual ~Node () = default;

        NodeT getType () const { return type_; }

        void setParent (Node *parent) { parent_ = parent; }

        Node *getParent () const { return parent_; }
        virtual void nodeDump (std::ostream &out) const = 0;
    };

    class VarNode final : public Node {
        std::string name_;

    public:
        VarNode (const std::string &name, Node *parent = nullptr)
            : Node (NodeT::VARIABLE, parent), name_ (name)
        {
        }

        void nodeDump (std::ostream &out) const override { out << name_; }

        std::string getName () const { return name_; }
    };

    class OperNode final : public Node {
    public:
        enum class OperType;

    private:
        OperType opType_;

    public:
        enum class OperType {
            AND,  // a & b
            OR,   // a | b
            IMPL, // a -> b 
            NOT   // ~a
        };

        OperNode (const OperType opType, Node *parent = nullptr)
            : Node (NodeT::OPERATOR, parent), opType_ (opType)
        {
        }

        OperType getOpType () const { return opType_; }

        void nodeDump (std::ostream &out) const override
        {
            switch (opType_) {
                case OperType::IMPL: out << "IMPL (->)"; break;
                case OperType::OR: out << "OR (|)"; break;
                case OperType::AND: out << "AND (&)"; break;
                case OperType::NOT: out << "NOT (~)"; break;
                default: out << "Unexpected operator type!";
            }
        }
    };

    class ConstNode final : public Node {
        bool value_;

    public:
        ConstNode (bool value, Node *parent = nullptr)
            : Node (NodeT::CONST, parent), value_ (value)
        {
        }

        void nodeDump (std::ostream &out) const override { out << value_; }

        bool getVal () const { return value_; }
    };
}

#endif
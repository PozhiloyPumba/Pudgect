#include "driver.hpp"
#include <stack>

namespace yy {
    void Form::recToString (SAT::Node *curRoot, std::string &str) const
    {
        if (!curRoot)
            return;

        if (curRoot->getType () == SAT::Node::NodeT::OPERATOR) {
            auto curOp = static_cast<SAT::OperNode *>(curRoot);

            if (curOp->getOpType () == SAT::OperNode::OperType::NOT) {
                if (curOp->left_->getType () != SAT::Node::NodeT::OPERATOR) {
                    str += curRoot->getNodeForDump ();
                    recToString (curOp->left_, str);
                }
                else {
                    str += curRoot->getNodeForDump () + "(";
                    recToString (curOp->left_, str);
                    str += ")";
                }
            }
            else {
                auto curPriority = static_cast<std::underlying_type_t<SAT::OperNode::OperType>>(curOp->getOpType ());

                auto printChild = [this, curPriority, curOp, &str] (auto &child) {
                    bool wasBr = false;
                    if (child->getType () == SAT::Node::NodeT::OPERATOR) {
                        auto childOp = static_cast<SAT::OperNode *>(child);
                        if (static_cast<std::underlying_type_t<SAT::OperNode::OperType>>(childOp->getOpType ()) < curPriority) {
                            wasBr = true;
                            str += "(";
                            recToString (childOp, str);
                            str += ")";
                        }
                    }
                    if (!wasBr)
                        recToString (child, str);
                };

                printChild (curOp->left_);

                str += curRoot->getNodeForDump ();

                printChild (curOp->right_);
            }
        }
        else {
            str += curRoot->getNodeForDump ();
        } 
    }

    std::string Form::toString () const // func which cast form to string
    {
        std::string form;
        recToString (tree_.getRoot (), form);
        return form;
    }


    parser::token_type Form::yylex (parser::semantic_type *yylval, parser::location_type *location)
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
            default:;
        }

        return tokenT;
    }


    void Form::deleteImplication () //FIXME:
    {
        std::stack<SAT::Node *> inorder;

        inorder.push (tree_.getRoot ());

        while (!inorder.empty ()) {
            auto curNode = inorder.top ();
            inorder.pop ();
            
            if (!curNode)
                continue;

            if (curNode->getType () == SAT::Node::NodeT::OPERATOR &&
                static_cast<SAT::OperNode *>(curNode)->getOpType () == SAT::OperNode::OperType::IMPL) {
                
                auto *parent = curNode->getParent ();
                auto *leftNode = curNode->left_;
                auto *rightNode = curNode->right_;

                auto *node = new SAT::OperNode (SAT::OperNode::OperType::OR, parent);
                
                if (parent != nullptr) {
                    if (parent->left_ == curNode) parent->left_ = node;
                    else parent->right_ = node;
                }
                else tree_.setRoot (node);

                auto *notNode = new SAT::OperNode (SAT::OperNode::OperType::NOT, node);
                node->left_ = notNode;
                curNode->left_->setParent (notNode);
                notNode->left_ = leftNode;

                node->right_ = rightNode;
                node->right_->setParent (node);
                delete curNode;
                inorder.push (node->left_);
                inorder.push (node->right_);
            }
            else {
                inorder.push (curNode->left_);
                // std::cout << curNode->getNodeForDump () << " parent " << curNode->getParent ()->getNodeForDump () << std::endl;
                inorder.push (curNode->right_);
            }
        }
    }

    void Form::deMorgan ()
    {

    }

    void Form::deleteDoubleNeg ()
    {

    }

    void Form::lawOfDistr ()
    {

    }

}
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


    void Form::deleteImplication ()
    {
        std::stack<SAT::Node *> preorder;

        preorder.push (tree_.getRoot ());

        while (!preorder.empty ()) {
            auto curNode = preorder.top ();
            preorder.pop ();
            
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
                curNode = node;
            }

            preorder.push (curNode->left_);
            preorder.push (curNode->right_);
        }
    }

    void Form::lambDeMorgan (SAT::Node *curNode, SAT::Node *newOp)
    {
        auto *curOp = curNode->left_;
        auto *curRoot = newOp->getParent (); //root

        if (curRoot != nullptr) {
            if (curRoot->left_ == curNode) curRoot->left_ = newOp;
            else curRoot->right_ = newOp;
        }
        else tree_.setRoot (newOp);

        auto *notNodeLeft = new SAT::OperNode (SAT::OperNode::OperType::NOT, newOp);
        newOp->left_ = notNodeLeft;
        curOp->left_->setParent (notNodeLeft);
        notNodeLeft->left_ = curOp->left_;

        auto *notNodeRight = new SAT::OperNode (SAT::OperNode::OperType::NOT, newOp);
        newOp->right_ = notNodeRight;
        curOp->right_->setParent (notNodeRight);
        notNodeRight->left_ = curOp->right_;

        delete curNode->left_;
        delete curNode;
    }

    bool Form::recDeMorgan (SAT::Node *curNode)
    {
        if (!curNode) return false;

        bool wasChL = recDeMorgan (curNode->left_);
        bool wasChR = recDeMorgan (curNode->right_);

        bool wasCh = wasChL || wasChR;

        if (curNode->getType () == SAT::Node::NodeT::OPERATOR &&
            static_cast<SAT::OperNode *>(curNode)->getOpType () == SAT::OperNode::OperType::NOT && curNode->left_ &&
            curNode->left_->getType () == SAT::Node::NodeT::OPERATOR) {

            auto *curOp = static_cast<SAT::OperNode *>(curNode->left_);
            switch (curOp->getOpType ()) {
                case SAT::OperNode::OperType::OR: {
                    auto *newOp = new SAT::OperNode (SAT::OperNode::OperType::AND, curNode->getParent ());
                    lambDeMorgan (curNode, newOp);
                    wasCh = true;
                    break;
                }
                case SAT::OperNode::OperType::AND: {
                    auto *newOp = new SAT::OperNode (SAT::OperNode::OperType::OR, curNode->getParent ());
                    lambDeMorgan (curNode, newOp);
                    wasCh = true;
                    break;
                }
                default:;
            }
        }

        return wasCh;
    }

    void Form::deMorgan ()  //TODO: add it in readme
    {
        while (recDeMorgan (tree_.getRoot ()))
            ;
    }

    void Form::deleteDoubleNeg ()
    {
        std::stack<SAT::Node *> preorder;

        preorder.push (tree_.getRoot ());

        while (!preorder.empty ()) {
            auto curNode = preorder.top ();
            preorder.pop ();
            
            if (!curNode)
                continue;

            if (curNode->getType () == SAT::Node::NodeT::OPERATOR &&
                static_cast<SAT::OperNode *>(curNode)->getOpType () == SAT::OperNode::OperType::NOT &&
                curNode->left_->getType () == SAT::Node::NodeT::OPERATOR &&
                static_cast<SAT::OperNode *>(curNode->left_)->getOpType () == SAT::OperNode::OperType::NOT) {

                auto *parent = curNode->getParent ();
                auto *newCHildNode = curNode->left_->left_;

                newCHildNode->setParent (parent);

                if (parent != nullptr) {
                    if (parent->left_ == curNode) parent->left_ = newCHildNode;
                    else parent->right_ = newCHildNode;
                }
                else tree_.setRoot (parent);

                delete curNode->left_;
                delete curNode;
                curNode = newCHildNode;
                preorder.push (newCHildNode);
            }

            preorder.push (curNode->left_);
            preorder.push (curNode->right_);
        }
    }

    void Form::lawOfDistr ()
    {
        std::stack<SAT::Node *> postorder;
        std::stack<SAT::Node *> queue;

        postorder.push (tree_.getRoot ());

        while (!postorder.empty ()) {
            auto curNode = postorder.top ();
            postorder.pop ();

            if (curNode) {
                queue.push (curNode);
                postorder.push (curNode->left_);
                postorder.push (curNode->right_);
            }
        }
        // std::cout << queue.size() << std::endl;

        while (!queue.empty ()) {
            
            auto *curNode = queue.top ();
            queue.pop ();
            
            if (curNode->getType () == SAT::Node::NodeT::OPERATOR &&
                static_cast<SAT::OperNode *>(curNode)->getOpType () == SAT::OperNode::OperType::OR) {

                if (curNode->left_->getType () == SAT::Node::NodeT::OPERATOR &&
                    static_cast<SAT::OperNode *>(curNode->left_)->getOpType () == SAT::OperNode::OperType::AND) {
                    
                    auto *parent = curNode->getParent ();
                    auto *leftOp = curNode->left_;

                    curNode->left_ = leftOp->right_;
                    curNode->setParent (leftOp);
                    leftOp->setParent (parent);
                    
                    if (parent){
                        if (parent->left_ == curNode) parent->left_ = leftOp;
                        else parent->right_ = leftOp;
                    }
                    else tree_.setRoot (leftOp);

                    auto *newOr = new SAT::OperNode (SAT::OperNode::OperType::OR, leftOp);

                    newOr->left_ = leftOp->left_;
                    newOr->left_->setParent (newOr);
                    newOr->right_ = curNode->right_->copySubTree (newOr);
                    leftOp->left_ = newOr;
                    leftOp->right_ = curNode;
                    curNode->left_->setParent (curNode);
                    queue.push (newOr);
                    queue.push (curNode);
                }
                else if (curNode->right_->getType () == SAT::Node::NodeT::OPERATOR &&
                    static_cast<SAT::OperNode *>(curNode->right_)->getOpType () == SAT::OperNode::OperType::AND) {
                        
                    auto *parent = curNode->getParent ();
                    auto *rightOp = curNode->right_;

                    curNode->right_ = rightOp->left_;
                    curNode->setParent (rightOp);
                    rightOp->setParent (parent);
                    
                    if (parent){
                        if (parent->left_ == curNode) parent->left_ = rightOp;
                        else parent->right_ = rightOp;
                    }
                    else tree_.setRoot (rightOp);
                    
                    auto *newOr = new SAT::OperNode (SAT::OperNode::OperType::OR, rightOp);

                    newOr->right_ = rightOp->right_;
                    newOr->right_->setParent (newOr);
                    newOr->left_ = curNode->left_->copySubTree (newOr);
                    rightOp->right_ = newOr;
                    rightOp->left_ = curNode;
                    curNode->right_->setParent (curNode);
                    queue.push (newOr);
                    queue.push (curNode);
                }
            }
        }
    }

}
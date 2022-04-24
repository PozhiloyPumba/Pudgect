#include "form.hpp"

namespace SAT {
    bool Form::input ()
    {
        yy::Driver driver;
        if (!driver.parse ())
            return 1;

        tree_.setRoot (driver.getRoot ());
        driver.setRoot (nullptr);
        driver.swapEvalInfo (evalInfo_);
        return 0;
    }

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

    void Form::toCNF ()
    {
        deleteImplication ();
        deMorgan ();
        deleteDoubleNeg ();
        lawOfDistr ();
        isCNF = true;
    }

    void Form::destructSubNode (std::list<SAT::Node *> &preorder, SAT::Node *curRoot) //TODO:
    {
        if (!curRoot)
            return;

        std::stack<SAT::Node *> stack;
        std::vector<SAT::Node *> queueOnDelete;
        stack.push (curRoot);

        while (stack.size ()) {
            auto *curNode = stack.top ();
            stack.pop ();
            if (curNode) {
                auto res = std::find (preorder.begin (), preorder.end (), curNode);
                if (res != preorder.end ()) {
                    preorder.erase (res);
                }
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

    void Form::deleteSubNode (std::list<SAT::Node *> &preorder, SAT::Node *subroot)   //TODO: iterators requirements
    {
        if (subroot->getParent () != nullptr) {
            auto *parent = subroot->getParent ();

            if (parent->getParent () != nullptr) { // grandparent us is not root of tree
                auto *grandParent = parent->getParent ();
                if (grandParent->left_ == parent) {    // we are in left subtree of grandparent
                    if (parent->left_ == subroot) { //we are left child
                        grandParent->left_ = parent->right_;
                        parent->right_->setParent (grandParent);
                        parent->right_ = nullptr;
                    }
                    else {    // we are right child
                        grandParent->left_ = parent->left_;
                        parent->left_->setParent (grandParent);
                        parent->left_ = nullptr;
                    }
                }
                else {  // we are in right subtree of grandparent
                    if (parent->left_ == subroot) { //we are left child
                        grandParent->right_ = parent->right_;
                        parent->right_->setParent (grandParent);
                        parent->right_ = nullptr;
                    }
                    else {    // we are right child
                        grandParent->right_ = parent->left_;
                        parent->left_->setParent (grandParent);
                        parent->left_ = nullptr;
                    }
                }

            }
            else {  // grandparent of us is root of tree
                if (parent->left_ == subroot) { //we are left child
                    tree_.setRoot (parent->right_);
                    parent->right_->setParent (nullptr);
                    parent->right_ = nullptr; 
                }
                else {  // we are right child
                    tree_.setRoot (parent->left_);
                    parent->left_->setParent (nullptr);
                    parent->left_ = nullptr;
                }
            }
            destructSubNode (preorder, parent);
        }
        else {
            destructSubNode (preorder, subroot);
            tree_.setRoot (nullptr);
        }

    }

    void Form::simplifyAllConjuncts ()
    {
        std::vector<SAT::Node *> preorder;

        preorder.push_back (tree_.getRoot ());

        while (!preorder.empty ()) {
            auto *curNode = preorder.back ();
            preorder.pop_back ();
            
            if (!curNode)
                continue;

            if (!(curNode->getType () == SAT::Node::NodeT::OPERATOR &&
                static_cast<SAT::OperNode *>(curNode)->getOpType () == SAT::OperNode::OperType::AND)) {
                
                simplifyOneConjunct (curNode);
                continue;
            }

            preorder.push_back (curNode->left_);
            preorder.push_back (curNode->right_);
        }
    }

    void Form::simplifyOneConjunct (SAT::Node *rootOfConjunct)
    {
        std::unordered_map<std::string, SAT::Node *> vars;

        std::list<SAT::Node *> preorder;   // stack -> list because i want delete elements from middle of it

        preorder.push_back (rootOfConjunct);

        while (!preorder.empty ()) {
            auto *curNode = preorder.back ();
            preorder.pop_back ();
            
            if (!curNode)
                continue;
            
            if (curNode->getType () == SAT::Node::NodeT::OPERATOR &&
                static_cast<SAT::OperNode *>(curNode)->getOpType () == SAT::OperNode::OperType::NOT) {
                
                auto *underNotNode = curNode->left_; 
                if (underNotNode->getType () == SAT::Node::NodeT::CONST) {
                    auto *constNode = static_cast <SAT::ConstNode *> (underNotNode);

                    if (constNode->getVal () == true) {
                        deleteSubNode (preorder, curNode);
                    }
                    else {
                        deleteSubNode (preorder, rootOfConjunct);
                        if (tree_.getRoot () == nullptr)
                            tree_.setRoot (new SAT::ConstNode (true));
                        return;
                    }
                }

                if (underNotNode->getType () == SAT::Node::NodeT::VARIABLE) {
                    auto *varNode = static_cast <SAT::VarNode *> (underNotNode);
                    
                    auto res = vars.find (varNode->getName ());

                    if (res != vars.end ()) {
                        if (res->second->getParent () != nullptr && 
                            static_cast <SAT::OperNode *>(res->second->getParent ())->getOpType () == SAT::OperNode::OperType::NOT) {
                            deleteSubNode (preorder, curNode);
                        }
                        else {
                            deleteSubNode (preorder, rootOfConjunct);
                            if (tree_.getRoot () == nullptr)
                                tree_.setRoot (new SAT::ConstNode (true));
                            return;
                        }
                    }
                    else {
                        vars.insert ({varNode->getName (), varNode});
                    }
                }
                continue;
            }
            
            if (curNode->getType () == SAT::Node::NodeT::CONST) {
                auto *constNode = static_cast <SAT::ConstNode *> (curNode);

                if (constNode->getVal () == true) {
                    deleteSubNode (preorder, rootOfConjunct);
                    if (tree_.getRoot () == nullptr)
                        tree_.setRoot (new SAT::ConstNode (true));
                    return;
                }
                else {
                    deleteSubNode (preorder, curNode);
                }
                continue;
            }

            if (curNode->getType () == SAT::Node::NodeT::VARIABLE) {
                auto *varNode = static_cast <SAT::VarNode *> (curNode);
                    
                auto res = vars.find (varNode->getName ());

                if (res != vars.end ()) {
                    if (res->second->getParent () != nullptr && 
                        static_cast <SAT::OperNode *>(res->second->getParent ())->getOpType () == SAT::OperNode::OperType::NOT) {
                        deleteSubNode (preorder, rootOfConjunct);
                        if (tree_.getRoot () == nullptr)
                            tree_.setRoot (new SAT::ConstNode (true));
                        return;
                    }
                    else {
                        deleteSubNode (preorder, curNode);
                    }
                }
                else {
                    vars.insert ({varNode->getName (), varNode});
                }
                continue;
            }

            preorder.push_back (curNode->left_);
            preorder.push_back (curNode->right_);
        }
    }

    void Form::simplify ()
    {
        if (!isCNF)
            toCNF ();

        simplifyAllConjuncts ();
    }

    void Form::evaluate ()  // TODO: copy tree 
    {
        std::stack<SAT::Node *> preorder;

        auto *copy = tree_.getRoot ()->copySubTree ();
        preorder.push (copy);

        while (!preorder.empty ()) {
            auto curNode = preorder.top ();
            preorder.pop ();
            
            if (!curNode)
                continue;

            if (curNode->getType () == SAT::Node::NodeT::VARIABLE) {
                auto *varNode = static_cast <SAT::VarNode *> (curNode);
                    
                auto res = evalInfo_.find (varNode->getName ());

                if (res != evalInfo_.end ()) {
                    auto *parent = varNode->getParent ();
                    auto *evalNode = new SAT::ConstNode (res->second, parent);
                    if (parent){
                        if (parent->left_ == curNode) parent->left_ = evalNode;
                        else parent->right_ = evalNode;
                    }
                    delete curNode;
                }
                continue;
            }

            preorder.push (curNode->left_);
            preorder.push (curNode->right_);
        }
    }

    void Form::printEvalInfo () const {
        std::for_each ( evalInfo_.begin (), evalInfo_.end (), 
                        [] (const std::pair<std::string, bool> &forDump) {std::cout << forDump.first << "=" << forDump.second;});
    }
}
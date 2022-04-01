#ifndef DRIVER_HPP__
#define DRIVER_HPP__

#include <FlexLexer.h>

#include <memory>
#include <stack>
#include <utility>

#include "grammar.tab.hh"
#include "lexer.hpp"
#include "node.hpp"
#include "tree.hpp"

namespace yy {

    class Form final {
        std::unique_ptr<SATLexer> lexer_;
        Tree::BinaryTree<SAT::Node *> tree_;

        void recToString (SAT::Node *curRoot, std::string &str) const //TODO: refactor this func
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
                    bool wasBr = false;
                    auto curPriority = static_cast<std::underlying_type_t<SAT::OperNode::OperType>>(curOp->getOpType ());
                    
                    if (curOp->left_->getType () == SAT::Node::NodeT::OPERATOR) {
                        auto leftOp = static_cast<SAT::OperNode *>(curRoot->left_);
                        if (static_cast<std::underlying_type_t<SAT::OperNode::OperType>>(leftOp->getOpType ()) < curPriority) {
                            wasBr = true;
                            str += "(";
                            recToString (curRoot->left_, str);
                            str += ")";
                        }
                    }
                    if (!wasBr)
                        recToString (curRoot->left_, str);
                    
                    wasBr = false;

                    str += curRoot->getNodeForDump ();

                    if (curOp->right_->getType () == SAT::Node::NodeT::OPERATOR) {
                        auto rightOp = static_cast<SAT::OperNode *>(curRoot->right_);
                        if (static_cast<std::underlying_type_t<SAT::OperNode::OperType>>(rightOp->getOpType ()) <= curPriority) {
                            wasBr = true;
                            str += "(";
                            recToString (rightOp, str);
                            str += ")";        
                        }
                    }
                    if (!wasBr) {
                        recToString (curRoot->right_, str);
                    }
                }
            }
            else {
                str += curRoot->getNodeForDump ();
            } 
        }

    public:
        Form () : lexer_ (std::unique_ptr<SATLexer>{new SATLexer}) {}

        bool parse ()
        {
            parser parser (this);
            bool res = parser.parse ();
            return !res;
        }

        void simplify ()
        {
            
        }

        std::string toString () const // func which cast form to string
        {
            std::string form;
            recToString (tree_.getRoot (), form);
            return form;
        }

        parser::token_type yylex (parser::semantic_type *yylval, parser::location_type *location)
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

        void setRoot (SAT::Node *node) { tree_.setRoot (node); }
        SAT::Node *getRoot () { return tree_.getRoot (); }

        void callDump (std::ostream &out) { tree_.dump (out); }
    };
}  // namespace yy

#endif
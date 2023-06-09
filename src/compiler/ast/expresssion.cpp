#include "compiler/ast.h"
#include "ZParser.h"
#include "compiler/type.h"

namespace zero {

    AtomicExpressionAstNode *createAtom(ZParser::AtomContext *atomContext, string fileName) {
        if (atomContext->function() != nullptr) {
            return FunctionAstNode::from(atomContext->function(), fileName);
        }

        auto atomic = new AtomicExpressionAstNode();
        atomic->expressionType = ExpressionAstNode::TYPE_ATOMIC;
        atomic->data = atomContext->getText();

        atomic->fileName = fileName;
        atomic->line = atomContext->getStart()->getLine();
        atomic->pos = atomContext->getStart()->getCharPositionInLine();

        if (atomContext->IDENT() != nullptr) {
            atomic->atomicType = AtomicExpressionAstNode::TYPE_IDENTIFIER;
            atomic->isLvalue = 1;
        }
        if (atomContext->STRING() != nullptr) {
            atomic->atomicType = AtomicExpressionAstNode::TYPE_STRING;
            atomic->data = atomContext->STRING()->getText();
            atomic->data = atomic->data.substr(1, atomic->data.size() - 2); // to remove " at the beginning and end
        }
        if (atomContext->INT() != nullptr) {
            atomic->atomicType = AtomicExpressionAstNode::TYPE_INT;
        }
        if (atomContext->DECIMAL() != nullptr) {
            atomic->atomicType = AtomicExpressionAstNode::TYPE_DECIMAL;
        }
        if (atomContext->TRUE_() != nullptr || atomContext->FALSE_() != nullptr) {
            atomic->atomicType = AtomicExpressionAstNode::TYPE_BOOLEAN;
        }
        if (atomContext->NULL_() != nullptr) {
            atomic->atomicType = AtomicExpressionAstNode::TYPE_NULL;
        }
        return atomic;
    }

    ExpressionAstNode *ExpressionAstNode::from(ZParser::ExpressionContext *expressionContext, string fileName) {
        ExpressionAstNode *expression;

        if (expressionContext->primaryExpresssion() != nullptr) {
            if (expressionContext->primaryExpresssion()->expression() != nullptr) {
                return from(expressionContext->primaryExpresssion()->expression(), fileName);
            } else {
                return createAtom(expressionContext->primaryExpresssion()->atom(), fileName);
            }
        }

        if (expressionContext->bop != nullptr) {
            auto binary = new BinaryExpressionAstNode();
            binary->expressionType = TYPE_BINARY;
            binary->left = from(expressionContext->expression(0), fileName);
            binary->right = from(expressionContext->expression(1), fileName);
            binary->opName = expressionContext->bop->getText();
            expression = binary;
        } else if (expressionContext->prefix != nullptr) {
            auto unary = new PrefixExpressionAstNode();
            unary->expressionType = TYPE_UNARY;
            unary->right = from(expressionContext->expression(0), fileName);
            unary->opName = expressionContext->prefix->getText();
            expression = unary;
        } else {
            auto functionCall = new FunctionCallExpressionAstNode();
            auto typeParameters = expressionContext->expression(0)->typeParametersBlock();
            auto hasExplicitTypeParameters = typeParameters != nullptr;
            auto leftExpressionContext = expressionContext->expression(0);
            if (hasExplicitTypeParameters) {
                for (const auto &param: typeParameters->typeDescriptor()) {
                    auto typeAst = TypeDescriptorAstNode::from(param, fileName);
                    functionCall->typeParams.push_back(typeAst);
                }
                leftExpressionContext = leftExpressionContext->expression(0);
            }
            functionCall->expressionType = TYPE_FUNCTION_CALL;
            functionCall->left = from(leftExpressionContext, fileName);
            functionCall->params = new vector<ExpressionAstNode *>();
            for (unsigned int i = 1; i < expressionContext->expression().size(); i++) {
                functionCall->params->push_back(from(expressionContext->expression(i), fileName));
            }
            expression = functionCall;
        }

        expression->fileName = fileName;
        expression->line = expressionContext->getStart()->getLine();
        expression->pos = expressionContext->getStart()->getCharPositionInLine();

        return expression;
    }

    string ExpressionAstNode::toString() {
        switch (expressionType) {
            case TYPE_FUNCTION_CALL: {
                auto call = ((FunctionCallExpressionAstNode *) this);
                string paramsStr;
                for (auto &piece: *call->params) {
                    paramsStr += piece->toString() + ",";
                }
                string typeParamsStr;
                for (auto &typeParam: call->resolvedTypeParams) {
                    typeParamsStr += typeParam->toString() + ",";
                }
                return "(" + call->left->toString() + "<" + typeParamsStr + ">(" + paramsStr + "):" +
                       resolvedType->name + ")";
            }
            case TYPE_UNARY: {
                auto prefix = ((PrefixExpressionAstNode *) this);
                return "(" + prefix->opName + "" + prefix->right->toString() + ":" + resolvedType->name +
                       ")";
            }
            case TYPE_BINARY: {
                auto binary = ((BinaryExpressionAstNode *) this);
                return "(" + binary->left->toString() + "" + binary->opName + "" +
                       binary->right->toString() + ":" + resolvedType->name + ")";
            }
            case TYPE_ATOMIC: {
                auto atom = ((AtomicExpressionAstNode *) this);
                if (atom->atomicType == AtomicExpressionAstNode::TYPE_FUNCTION) {
                    auto function = ((FunctionAstNode *) atom);
                    string argsStr;
                    for (const auto &piece : *function->arguments) {
                        argsStr += piece.first + ":" + piece.second->toString() + ",";
                    }
                    string typeParametersStr;
                    for (const auto &piece : function->typeArguments) {
                        typeParametersStr += piece.first + ":" + piece.second->toString() + ",";
                    }
                    return "(fun <" + typeParametersStr + ">(" + argsStr + "){\n" + function->program->toString() +
                           "}: " +
                           resolvedType->toString() + ")";
                } else {
                    return "(" + atom->data + ":" + resolvedType->toString() + ")";
                }
            }
            default: {
                return BaseAstNode::toString();
            }
        }
    }
}
//
// Created by user on 14.10.2019.
//
#include <iostream>
#include "expressionNodes.h"

void IntegerLiteralNode::print(std::string ident) {
    std::cout << ident << "integer literal: " << value << std::endl;
}

void RealLiteralNode::print(std::string ident) {
    std::cout << ident << "real literal: " << value << std::endl;
}

void BooleanLiteralNode::print(std::string ident) {
    std::cout << ident << "boolean literal: " << value << std::endl;
}

void WhileNode::print(std::string ident) {
    std::cout << ident << "while condition" << std::endl;
    condition->print(ident + " ");
    std::cout << ident << "while body" << std::endl;
    body->print(ident + " ");
}

void IfNode::print(std::string ident) {
    std::cout << ident << "if condition" << std::endl;
    condition->print(ident + " ");
    std::cout << ident << "if trueBody" << std::endl;
    trueBody->print(ident + " ");
    if (falseBody) {
        std::cout << ident << "if falseBody" <<
                  std::endl;
        falseBody->
                print(ident
                      + " ");
    }
}

void FieldDeclarationNode::print(std::string ident) {
    std::cout << ident << "field declaration varName: " << varName << std::endl;
    initializer->print(ident + " ");
}

void VarDeclarationNode::print(std::string ident) {
    std::cout << ident << "var declaration varName: " << varName << std::endl;
    initializer->print(ident + " ");
}

void VarAssignmentNode::print(std::string ident) {
    std::cout << ident << "var assignment varName: " << varName << std::endl;
    initializer->print(ident + " ");
}

void FieldAssignmentNode::print(std::string ident) {
    std::cout << ident << "field assignment varName: " << varName << " field: " << fieldName << std::endl;
    initializer->print(ident + " ");
}

void ReturnNode::print(std::string ident) {
    std::cout << ident << "return: " << std::endl;
    expression->print(ident + " ");
}

void VariableNode::print(std::string ident) {
    std::cout << ident << "variable: " << varName << std::endl;
}

void FieldAccessNode::print(std::string ident) {
    std::cout << ident << "field access, varName: " << varName << " field: " << field << std::endl;
}

void MethodInvokeNode::print(std::string ident) {
    std::cout << ident << "invoke method, varName: " << varName << " methodName: " << methodName << std::endl;
    for (auto it = args.begin(); it != args.end(); ++it)
        (*it)->print(ident + " ");
}

void ConstructorInvokeNode::print(std::string ident) {
    std::cout << ident << "invoke constructor, className: " << className << std::endl;
    for (auto it = args.begin(); it != args.end(); ++it)
        (*it)->print(ident + " ");
}

void BodyNode::print(std::string ident) {
    std::cout << ident << "body:" << std::endl;
    for (auto it = statements.begin(); it != statements.end(); ++it)
        (*it)->print(ident + " ");
}

void ClassBodyNode::print(std::string ident) {
    if (constructor != nullptr) {
        std::cout << ident << "ctor:" << std::endl;
        constructor->print(ident + " ");
    }
    std::cout << ident << "VarDeclarations:" << std::endl;
    for (auto it = varDeclarations.begin(); it != varDeclarations.end(); ++it)
        (*it)->print(ident + " ");
    std::cout << ident << "MethodDeclarations:" << std::endl;
    for (auto it = methods.begin(); it != methods.end(); ++it)
        (*it)->print(ident + " ");
}

void ParamNode::print(std::string ident) {
    std::cout << ident << "parameter " << paramName << ", " << paramType << std::endl;
}

void MethodDeclarationNode::print(std::string ident) {
    std::cout << ident << "method " << methodName << ", returns " << returnTypeName << std::endl;
    for (auto it = params.begin(); it != params.end(); ++it)
        (*it)->print(ident + " ");
    body->print(ident + " ");
}

void ConstructorDeclarationNode::print(std::string ident) {
    for (auto it = params.begin(); it != params.end(); ++it)
        (*it)->print(ident + " ");
    body->print(ident + " ");
}

void ClassNode::print(std::string ident) {
    std::cout << ident << "Class " << className;
    if (classTemplate != "")
        std::cout << " template " << classTemplate;
    if (baseClass != "")
        std::cout << "baseClass " << baseClass;
    std::cout << std::endl;
    classBody->print(ident + " ");
}


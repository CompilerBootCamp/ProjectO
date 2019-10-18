//
// Created by alex on 06.10.19.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include <vector>
#include <unordered_map>
#include <exception>

#include "tokens.h"
#include "expressionNodes.h"


typedef std::vector<Tokens*>::iterator TokensIt;


class Parser {
    TokensIt currentToken;
    TokensIt startToken;
    TokensIt endToken;

    int getNextToken() {
        ++currentToken;
        return (*currentToken)->getType();
    }

    bool checkCurrentToken(int id) {
        return (*currentToken)->getType() == id;
    }

    bool checkNextToken(int id) {
        ++currentToken;
        return checkCurrentToken(id);
    }

    void expectCurrentToken(int id) {
        if (!checkCurrentToken(id)) {
            std::string error = "expected " + convertTokenToString(id) + ", received " + (*currentToken)->toString();
            throwError(error);
        }
    }

    void expectNextToken(int id) {
        ++currentToken;
        expectCurrentToken(id);
    }

    void throwError(std::string error) {
        std::cout << error << std::endl;
        throw std::exception();
    }

    std::vector<TreeNode*> classes;

    TreeNode* parseWhileStatement();
    TreeNode* parseIfStatement();
    TreeNode* parseAssignmentStatement();
    TreeNode* parseStatement();
    TreeNode* parseIdentifier();
    TreeNode* parseExpression();
    TreeNode* parseBody();
    ParamNode* parseParameter();
    std::vector<ParamNode*> parseParameters();
    ClassBodyNode* parseClassBody();
    TreeNode* parseClass();

public:


    Parser(const TokensIt &start, const TokensIt &end) : currentToken(start), startToken(start), endToken(end) { }

    std::vector<TreeNode*> parseTokens() {
        while(currentToken != endToken)
            classes.push_back(parseClass());
        return classes;
    }

    void print() {
        for (auto it = classes.begin(); it != classes.end(); ++it)
            (*it)->print("");
    }
};

#endif //COMPILER_PARSER_H

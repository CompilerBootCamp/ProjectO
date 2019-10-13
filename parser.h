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
    TreeNode* baseNode;

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

    TreeNode* parseWhileStatement();
    TreeNode* parseIfStatement();
    TreeNode* parseAssignmentStatement();
    TreeNode* parseStatement();
    TreeNode* parseExpression();
    TreeNode* parseBody();
    TreeNode* parseParameter();
    std::vector<TreeNode*> parseParameters();
    TreeNode* parseClassBody();
    TreeNode* parseClass();

public:
    Parser(const TokensIt &start, const TokensIt &end) : currentToken(start), startToken(start), endToken(end) {
        baseNode = parseClass();
    }

    void print() {
        baseNode->print("");
    }
};

#endif //COMPILER_PARSER_H

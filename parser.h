//
// Created by alex on 06.10.19.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include <vector>
#include <unordered_map>
#include <exception>

#include "tokens.h"


typedef std::vector<Tokens*>::iterator TokensIt;


class Node {
protected:
    TokensIt currentToken;
    TokensIt startToken;
    TokensIt endToken;
    std::unordered_map<std::string, Node*> symbols;
public:
    Node(const TokensIt &start, const TokensIt &end) : currentToken(start), startToken(start), endToken(end) {}

    Node* findSymbol(std::string name) {
        auto classIt = symbols.find(name);
        if (classIt == symbols.end())
            return nullptr;
        else
            return (*classIt).second;
    }

    bool parseKeyword(int id) {
        if (!(*currentToken)->checkType(KeywordToken))
            return false;

        Keyword* keyword = static_cast<Keyword*>(*currentToken);
        if(!keyword->checkId(id))
            return false;
        return true;
    }

    void throwError(std::string error, Tokens* token) {
        std::cout << error << std::endl;
        throw std::exception();
    }

    virtual void print()
    {

    }

    const TokensIt &getCurrentToken() const {
        return currentToken;
    }
};


class ClassNode : public Node {
    std::string name;
    std::unordered_map<std::string, Node*> fields;
    std::unordered_map<std::string, Node*> methods;
    Node* parent = nullptr;
    Node* baseClass = nullptr;
    Node* templateClass = nullptr;
public :
    ClassNode(Node* parentNode, const TokensIt &start, const TokensIt &end): Node(start, end), parent(parentNode) {
        if (currentToken == endToken)
            return;

        parseClassName();
        parseTemplateConstruction();
        parseExtendsConstruction();
        parseClassBody();
    }

    const std::string &getName() const {
        return name;
    }


    void parseClassName() {
        if (!parseKeyword(Class))
            throwError("'class' keyword expected", *currentToken);
        currentToken++;

        if ((currentToken != endToken) && (*currentToken)->checkType(IdentificatorToken)) {
            name = (*currentToken)->getImage();
        }
        else
            throwError("identifier expected", *currentToken);
        ++currentToken;
    }

    Node* parseClassIdentifier() {
        if ((currentToken == endToken) || !(*currentToken)->checkType(IdentificatorToken))
            throwError("identifier expected", *currentToken);

        Node* classId = parent->findSymbol((*currentToken)->getImage());
        if (classId == nullptr)
            throwError("base class not found", *currentToken);

        ++currentToken;
        return classId;
    }

    void parseTemplateConstruction() {
        if ((currentToken == endToken) || !(*currentToken)->checkType(DelimiterToken))
            return;
        Delimiter* delimiter = static_cast<Delimiter*>(*currentToken);
        if (!delimiter->checkCode(LeftRectBracket))
            return;
        currentToken++;

        templateClass = parseClassIdentifier();

        if ((currentToken == endToken) || !(*currentToken)->checkType(DelimiterToken))
            throwError("']' expected", *currentToken);
        delimiter = static_cast<Delimiter*>(*currentToken);
        if (!delimiter->checkCode(LeftRectBracket))
            throwError("']' expected", *currentToken);
        currentToken++;
    }

    void parseExtendsConstruction() {
        if (!parseKeyword(Extends))
            return;
        ++currentToken;
        baseClass = parseClassIdentifier();
    }

    void parseClassBody() {
        if (!parseKeyword(Is))
            throwError("'is' keyword expected", *currentToken);
        currentToken++;

        if (parseKeyword(End)) {
            currentToken++;
            return;
        }
        else if (parseKeyword(Var)) {

        }
        else if (parseKeyword(Method)) {

        }
        else
            throwError("'end', 'var', 'method' keywords expected", *currentToken);

    }

    void print() {
        std::cout << "Class, name = " << name << std::endl;
    }

    virtual ~ClassNode() {
        for(auto it = fields.begin(); it!= fields.end(); ++it)
            delete((*it).second);
        for(auto it = methods.begin(); it!= methods.end(); ++it)
            delete((*it).second);
    }
};

class ProgramNode : public Node {
public:
    ProgramNode(const TokensIt &start, const TokensIt &end): Node(start, end) {
        while (currentToken != endToken){
            ClassNode* node = new ClassNode(this, currentToken, end);
            symbols[node->getName()] = node;
            currentToken = node->getCurrentToken();
        }
    }

    virtual ~ProgramNode() {
        for(auto it = symbols.begin(); it!= symbols.end(); ++it)
            delete((*it).second);
    }

    void print() {
        for(auto it = symbols.begin(); it!= symbols.end(); ++it)
            (*it).second->print();
    }
};


#endif //COMPILER_PARSER_H

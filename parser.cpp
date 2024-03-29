#include "parser.h"
#include "expressionNodes.h"

TreeNode* Parser::parseWhileStatement() {
    TreeNode* condition = parseExpression();
    expectNextToken(Loop);
    TreeNode* body = parseBody();
    return new WhileNode(condition, body);
}

TreeNode* Parser::parseIfStatement() {
    TreeNode *condition = parseExpression();
    expectNextToken(Then);
    TreeNode *trueBody = parseBody();
    if (checkCurrentToken(Else)) {
        TreeNode *falseBody = parseBody();
        expectCurrentToken(End);
        return new IfNode(condition, trueBody, falseBody);
    } else if (checkCurrentToken(End))
        return new IfNode(condition, trueBody);
    else
        throwError("'end' or 'else' expected");
}

TreeNode* Parser::parseAssignmentStatement() {
    std::string varName = (*currentToken)->getImage();
    int token = getNextToken();
    if (token == Dot) {
        expectNextToken(Identifier);
        std::string fieldName = (*currentToken)->getImage();
        expectNextToken(Assignment);
        TreeNode* initializer = parseExpression();
        return new FieldAssignmentNode(varName, fieldName, initializer);
    } else if (token == Assignment) {
        TreeNode* initializer = parseExpression();
        return new VarAssignmentNode(varName, initializer);
    } else
        throwError("':=' expected");


}

TreeNode* Parser::parseStatement() {
    switch ((*currentToken)->getType()) {
        case While:
            return parseWhileStatement();
        case If:
            return parseIfStatement();
        case Return:
            return new ReturnNode(parseExpression());
        case Identifier:
            return parseAssignmentStatement();
        default:
            throwError("unknown expression");
    }
}

TreeNode* Parser::parseIdentifier() {
    std::string varName = (*currentToken)->getImage();
    int token = getNextToken();
    if(token == Dot) {
        expectNextToken(Identifier);
        std::string memberName = (*currentToken)->getImage();

        if (!checkNextToken(LeftRoundBracket)) {
            --currentToken;
            return new FieldAccessNode(varName, memberName);
        }

        std::vector<TreeNode*> args;
        if (getNextToken()==RightRoundBracket)
            return new MethodInvokeNode(varName, memberName, args);
        --currentToken;
        do{
            args.push_back(parseExpression());
        } while (checkNextToken(Comma));
        expectCurrentToken(RightRoundBracket);
        return new MethodInvokeNode(varName, memberName, args);
    }
    else if (token == LeftRoundBracket){
        std::vector<TreeNode*> args;
        if (getNextToken()==RightRoundBracket)
            return new ConstructorInvokeNode(varName, args);
        --currentToken;
        do{
            args.push_back(parseExpression());
        } while (checkNextToken(Comma));
        expectCurrentToken(RightRoundBracket);
        return new ConstructorInvokeNode(varName, args);
    }{
        --currentToken;
        return new VariableNode(varName);
    }
}

TreeNode* Parser::parseExpression(){
    switch (getNextToken()) {
        case RealLiteral:
            return new RealLiteralNode((*currentToken)->getImage());
        case IntegerLiteral:
            return new IntegerLiteralNode((*currentToken)->getImage());
        case BooleanLiteral:
            return new BooleanLiteralNode((*currentToken)->getImage());
        case Identifier:
            return parseIdentifier();
        default:
            throwError("literal or identifier expected");
    }
}

TreeNode* Parser::parseBody() {
    std::unordered_map <std::string, std::string> symbols;
    std::vector <TreeNode*> statements;
    int token;
    while ( (token = getNextToken()) != End && token != Else) {
        if(token == Var) {
            expectNextToken(Identifier);
            std::string varName = (*currentToken)->getImage();
            symbols[(*currentToken)->getImage()] = varName;

            expectNextToken(Colon);
            TreeNode* newValueExpression = parseExpression();
            statements.push_back(new VarDeclarationNode(varName, newValueExpression));
        }
        else
            statements.push_back(parseStatement());
    }
    return new BodyNode(symbols, statements);
}

ParamNode* Parser::parseParameter() {
    expectNextToken(Identifier);
    std::string paramName = (*currentToken)->getImage();

    expectNextToken(Colon);
    expectNextToken(Identifier);
    std::string paramType = (*currentToken)->getImage();
    return new ParamNode(paramName, paramType);
}

std::vector<ParamNode*> Parser::parseParameters() {
    std::vector<ParamNode*> params;
    expectNextToken(LeftRoundBracket);
    if (getNextToken()==RightRoundBracket)
        return params;
    --currentToken;
    do{
        params.push_back(parseParameter());
    } while (checkNextToken(Comma));

    expectCurrentToken(RightRoundBracket);
    return params;
}

ClassBodyNode* Parser::parseClassBody() {
    std::unordered_map <std::string, std::string> symbols;
    std::vector <TreeNode*> methods;
    std::vector <FieldDeclarationNode*> varDeclarations;
    TreeNode* constructor = nullptr;

    while (!checkNextToken(End)) {
        if(checkCurrentToken(Var)) {
            expectNextToken(Identifier);
            std::string varName = (*currentToken)->getImage();
            symbols[(*currentToken)->getImage()] = varName;

            expectNextToken(Colon);
            TreeNode* newValueExpression = parseExpression();
            varDeclarations.push_back(new FieldDeclarationNode(varName, newValueExpression));
        }
        else if (checkCurrentToken(Method)) {
            expectNextToken(Identifier);
            std::string methodName = (*currentToken)->getImage();

            std::vector<ParamNode*> params = parseParameters();
            std::string returnTypeName = "";
            if (checkNextToken(Colon)) {
                expectNextToken(Identifier);
                returnTypeName = (*currentToken)->getImage();
            }
            expectNextToken(Is);
            TreeNode* body = parseBody();
            methods.push_back(new MethodDeclarationNode(methodName, returnTypeName, params, body));
        } else if (checkCurrentToken(This)) {
            std::vector<ParamNode*> params = parseParameters();
            expectNextToken(Is);
            TreeNode* body = parseBody();
            constructor = new ConstructorDeclarationNode(params, body);
        } else
            throwError("'var', 'method', 'this' expected");

    }
    currentToken++;
    return new ClassBodyNode(symbols, varDeclarations, methods, constructor);
}

TreeNode* Parser::parseClass(){
    expectCurrentToken(Class);
    expectNextToken(Identifier);
    std::string className = (*currentToken)->getImage();

    std::string classTemplate = "";
    ++currentToken;
    if (checkCurrentToken(LeftRectBracket)) {
        expectNextToken(Identifier);
        classTemplate = (*currentToken)->getImage();
        expectNextToken(RightRectBracket);
        ++currentToken;
    }

    std::string baseClass = "";
    if (checkCurrentToken(Extends)) {
        expectNextToken(Identifier);
        baseClass = (*currentToken)->getImage();
        ++currentToken;
    }
    expectCurrentToken(Is);
    ClassBodyNode* classBody = parseClassBody();
    return new ClassNode(className, classTemplate, baseClass, classBody);
}
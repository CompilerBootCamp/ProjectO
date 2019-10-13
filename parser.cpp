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
    expectNextToken(Assignment);
    TreeNode* initializer = parseExpression();
    return new AssignmentNode(varName, initializer);
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

TreeNode* Parser::parseExpression(){
    int token = getNextToken();
    if (token == Literal){
        return new LiteralNode((*currentToken)->getImage());
    } else if (token == Identifier) {
        // поиск идентификатора в таблице символов
        std::string varName = (*currentToken)->getImage();

        token = getNextToken();
        if(token == Dot) {
            expectNextToken(Identifier);
            std::string memberName = (*currentToken)->getImage();

            if (!checkNextToken(LeftRoundBracket)) {
                --currentToken;
                return new FieldAccessNode(varName, memberName);
            }

            std::vector<TreeNode*> args;
            do{
                args.push_back(parseExpression());
            } while (checkNextToken(Comma));
            expectCurrentToken(RightRoundBracket);
            return new MethodInvokeNode(varName, memberName, args);
        }
        else if (token == LeftRoundBracket){
            std::vector<TreeNode*> args;
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
    else
        throwError("literal or identifier expected");
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
            statements.push_back(new AssignmentNode(varName, newValueExpression));
        }
        else
            statements.push_back(parseStatement());
    }
    return new BodyNode(symbols, statements);
}

TreeNode* Parser::parseParameter() {
    expectNextToken(Identifier);
    std::string paramName = (*currentToken)->getImage();

    expectNextToken(Colon);
    expectNextToken(Identifier);
    std::string paramType = (*currentToken)->getImage();
    return new ParamNode(paramName, paramType);
}

std::vector<TreeNode*> Parser::parseParameters() {
    std::vector<TreeNode*> params;
    expectNextToken(LeftRoundBracket);
    do{
        params.push_back(parseParameter());
    } while (checkNextToken(Comma));

    expectCurrentToken(RightRoundBracket);
    return params;
}

TreeNode* Parser::parseClassBody() {
    std::unordered_map <std::string, std::string> symbols;
    std::vector <TreeNode*> methods;
    std::vector <TreeNode*> varDeclarations;
    TreeNode* constructor;

    while (!checkNextToken(End)) {
        if(checkCurrentToken(Var)) {
            expectNextToken(Identifier);
            std::string varName = (*currentToken)->getImage();
            symbols[(*currentToken)->getImage()] = varName;

            expectNextToken(Colon);
            TreeNode* newValueExpression = parseExpression();
            varDeclarations.push_back(new AssignmentNode(varName, newValueExpression));
        }
        else if (checkCurrentToken(Method)) {
            expectNextToken(Identifier);
            std::string methodName = (*currentToken)->getImage();

            std::vector<TreeNode*> params = parseParameters();
            std::string returnTypeName = "";
            if (checkNextToken(Colon)) {
                expectNextToken(Identifier);
                returnTypeName = (*currentToken)->getImage();
            }
            expectNextToken(Is);
            TreeNode* body = parseBody();
            methods.push_back(new MethodDeclarationNode(methodName, returnTypeName, params, body));
        } else if (checkCurrentToken(This)) {
            std::vector<TreeNode*> params = parseParameters();
            expectNextToken(Is);
            TreeNode* body = parseBody();
            constructor = new ConstructorDeclarationNode(params, body);
        } else
            throwError("'var', 'method', 'this' expected");

    }
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
    TreeNode* classBody = parseClassBody();
    return new ClassNode(className, classTemplate, baseClass, classBody);

}
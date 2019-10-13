//
// Created by alex on 06.10.19.
//

#ifndef COMPILER_EXPRESSIONNODES_H
#define COMPILER_EXPRESSIONNODES_H
#include <string>
#include <vector>

/**
 *
 */
class VariableInfo {
    std::string name;
public:
    VariableInfo(const std::string &name) : name(name) {}
};

/**
 *
 */
class TreeNode {
public:
    virtual void print(std::string ident) {}
};

/**
 *
 */
class LiteralNode : public TreeNode {
    std::string value;
public:
    LiteralNode(const std::string &value) : value(value) {}
    void print(std::string ident) {
        std::cout<< ident << "literal: " << value << std::endl;
    }
};

/**
 *
 */
class WhileNode : public TreeNode {
    TreeNode* condition;
    TreeNode* body;
public:
    WhileNode(TreeNode *condition, TreeNode *body) : condition(condition), body(body) {}
    void print(std::string ident) {
        std::cout << ident << "while condition" << std::endl;
        condition->print(ident + " ");
        std::cout << ident << "while body" << std::endl;
        body->print(ident + " ");
    }
};

/**
 *
 */
class IfNode : public TreeNode {
    TreeNode* condition;
    TreeNode* trueBody;
    TreeNode* falseBody;
public:
    IfNode(TreeNode *condition, TreeNode *trueBody) : condition(condition), trueBody(trueBody), falseBody(nullptr) {}

    IfNode(TreeNode *condition, TreeNode *trueBody, TreeNode *falseBody) : condition(condition), trueBody(trueBody),
                                                                           falseBody(falseBody) {}

    void print(std::string ident) {
        std::cout << ident << "if condition" << std::endl;
        condition->print(ident + " ");
        std::cout << ident << "if trueBody" << std::endl;
        trueBody->print(ident + " ");
        if (falseBody) {
            std::cout << ident << "if falseBody" << std::endl;
            falseBody->print(ident + " ");
        }
    }
};

/**
 *
 */
class AssignmentNode : public TreeNode {
    std::string varName;
    TreeNode* initializer;
public:
    AssignmentNode(const std::string &varName, TreeNode *initializer) : varName(varName), initializer(initializer) {}
    void print(std::string ident) {
        std::cout<< ident<< "assignment varName: " << varName << std::endl;
        initializer->print(ident + " ");
    }
};

/**
 *
 */
class ReturnNode : public TreeNode {
    TreeNode* expression;
public:
    ReturnNode(TreeNode *expression) : expression(expression) {}
    void print(std::string ident) {
        std::cout<< ident<< "return: " << std::endl;
        expression->print(ident + " ");
    }
};

/**
 *
 */
class VariableNode  : public TreeNode {
    std::string varName;
public:
    VariableNode(const std::string &varName) : varName(varName) {}
    void print(std::string ident) {
        std::cout<< ident<< "variable: " << varName << std::endl;
    }
};

/**
 *
 */
class FieldAccessNode  : public TreeNode  {
    std::string varName;
    std::string field;
public:
    FieldAccessNode(const std::string &varName, const std::string &field) : varName(varName), field(field) {}
    void print(std::string ident) {
        std::cout<< ident<< "field access, varName: " << varName << " field: " << field<< std::endl;
    }
};

/**
 *
 */
class MethodInvokeNode  : public TreeNode  {
    std::string varName;
    std::string methodName;
    std::vector<TreeNode*> args;
public:
    MethodInvokeNode(const std::string &varName, const std::string &methodName, const std::vector<TreeNode *> &args)
            : varName(varName), methodName(methodName), args(args) {}
    void print(std::string ident) {
        std::cout<< ident<< "invoke method, varName: " << varName << " methodName: " << methodName<< std::endl;
        for (auto it = args.begin(); it != args.end(); ++it)
            (*it)->print(ident + " ");
    }
};

/**
 *
 */
class ConstructorInvokeNode  : public TreeNode  {
    std::string className;
    std::vector<TreeNode*> args;
public:
    ConstructorInvokeNode(const std::string &className, const std::vector<TreeNode *> &args) : className(className),
                                                                                               args(args) {}

    void print(std::string ident) {
        std::cout<< ident<< "invoke constructor, className: " << className <<  std::endl;
        for (auto it = args.begin(); it != args.end(); ++it)
            (*it)->print(ident + " ");
    }
};

/**
 *
 */
class BodyNode  : public TreeNode {
    std::unordered_map <std::string, std::string> symbols;
    std::vector <TreeNode*> statements;
public:
    BodyNode(const std::unordered_map<std::string, std::string> &symbols, const std::vector<TreeNode *> &statements)
            : symbols(symbols), statements(statements) {}
    void print(std::string ident) {
        std::cout<< ident<< "body:" << std::endl;
        for (auto it = statements.begin(); it != statements.end(); ++it)
            (*it)->print(ident + " ");
    }
};

/**
 *
 */
class ClassBodyNode  : public TreeNode {
    std::unordered_map <std::string, std::string> symbols;
    std::vector <TreeNode*> varDeclarations;
    std::vector <TreeNode*> methods;
    TreeNode* constructor;
public:
    ClassBodyNode(const std::unordered_map<std::string, std::string> &symbols,
                  const std::vector<TreeNode *> &varDeclarations, const std::vector<TreeNode *> &methods,
                  TreeNode *constructor) : symbols(symbols), varDeclarations(varDeclarations), methods(methods),
                                           constructor(constructor) {}

    void print(std::string ident) {
        if (constructor != nullptr) {
            std::cout << ident << "ctor:" << std::endl;
//            constructor->print(ident + " ");
        }
        std::cout<< ident<< "VarDeclarations:" << std::endl;
        for (auto it = varDeclarations.begin(); it != varDeclarations.end(); ++it)
            (*it)->print(ident + " ");
        std::cout<< ident<< "MethodDeclarations:" << std::endl;
        for (auto it = methods.begin(); it != methods.end(); ++it)
            (*it)->print(ident + " ");
    }
};

/**
 *
 */
class MethodDeclarationNode  : public TreeNode {
    std::string methodName;
    std::string returnTypeName;
    std::vector<TreeNode*> params;
    TreeNode* body;
public:
    MethodDeclarationNode(const std::string &methodName, const std::string &returnTypeName,
                          const std::vector<TreeNode *> &params, TreeNode *body) : methodName(methodName),
                                                                                   returnTypeName(returnTypeName),
                                                                                   params(params), body(body) {}
    void print(std::string ident) {
        std::cout << ident << "method " << methodName << ", returns " << returnTypeName << std::endl;
        for(auto it = params.begin(); it!=params.end(); ++it)
            (*it)->print(ident + " ");
        body->print(ident + " ");
    }
};


/**
 *
 */
class ConstructorDeclarationNode  : public TreeNode {
    std::vector<TreeNode*> params;
    TreeNode* body;
public:
    ConstructorDeclarationNode(const std::vector<TreeNode *> &params, TreeNode *body) : params(params), body(body) {}
};

/**
 *
 */
class ClassNode  : public TreeNode {
    std::string className;
    std::string classTemplate;
    std::string baseClass;
    TreeNode* classBody;
public:
    ClassNode(const std::string &className, const std::string &classTemplate, const std::string &baseClass,
              TreeNode *classBody) : className(className), classTemplate(classTemplate), baseClass(baseClass),
                                     classBody(classBody) {}
    void print(std::string ident) {

        std::cout << ident << "Class " << className;
        if (classTemplate != "")
            std::cout << " template " << classTemplate;
        if (baseClass != "")
            std::cout << "baseClass " << baseClass;
        std::cout << std::endl;
        classBody->print(ident + " ");
    }
};

/**
 *
 */
class ParamNode  : public TreeNode {
    std::string paramName;
    std::string paramType;
public:
    ParamNode(const std::string &paramName, const std::string &paramType) : paramName(paramName),
                                                                            paramType(paramType) {}

    void print(std::string ident) {
        std::cout << ident << "parameter " << paramName << ", " << paramType << std::endl;
    }
};






#endif //COMPILER_EXPRESSIONNODES_H

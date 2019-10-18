//
// Created by alex on 06.10.19.
//

#ifndef COMPILER_EXPRESSIONNODES_H
#define COMPILER_EXPRESSIONNODES_H
#include <string>
#include <vector>
#include "llvm/ADT/APFloat.h"
#include "llvm/IR/Verifier.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
/// Обязательно добавляем данный заголовочный файл для JIT-компиляции!
#include "llvm/ExecutionEngine/Interpreter.h"

#include <map>
#include <stack>
#include <set>


typedef std::unordered_map<std::string, llvm::Value*> SymbolTable;

struct FieldInfo {
    int number;
    llvm::Type* type;
    llvm::Value* initializer;
};

struct ClassInfo {
    llvm::Type* type;
    std::unordered_map<std::string, FieldInfo> fields;
    std::unordered_map<std::string, llvm::Function*> methods;
    ClassInfo() {}
    ClassInfo(llvm::Type *type) : type(type) {}
};

struct CodeGenParams{
    llvm::LLVMContext& context;
    llvm::IRBuilder<>& builder;
    std::unique_ptr<llvm::Module>& module;
    std::vector<SymbolTable>& symbolTables;
    std::map<std::string, ClassInfo>& classes;
};

/**
 *
 */
class TreeNode {
public:
    virtual void print(std::string ident)  {}
    virtual llvm::Value* generateCode(const CodeGenParams& params) {}
    virtual ~TreeNode() {}
};

/**
 *
 */
class IntegerLiteralNode : public TreeNode {
    int value;
public:
    IntegerLiteralNode(const std::string &image) : value(std::stoi(image)) {}
    void print(std::string ident) override;
    llvm::Value* generateCode(const CodeGenParams& params) override {
        return llvm::ConstantInt::get(params.context, llvm::APInt(32, value, true));
    }
};

/**
 *
 */
class RealLiteralNode : public TreeNode {
    double value;
public:
    RealLiteralNode(const std::string &image) : value(std::stod(image)) {}
    void print(std::string ident) override;
    llvm::Value* generateCode(const CodeGenParams& params) override {
        return llvm::ConstantFP::get(params.context, llvm::APFloat(value));
    }
};

/**
 *
 */
class BooleanLiteralNode : public TreeNode {
    bool value;
public:
    BooleanLiteralNode(const std::string &image) {
        value = image == "true" ? true : false;
    }
    void print(std::string ident) override;
    llvm::Value* generateCode(const CodeGenParams& params) override {
        return llvm::ConstantInt::get(params.context, llvm::APInt(1, value, true));
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
    void print(std::string ident) override;

    virtual ~WhileNode() {
        delete condition;
        delete body;
    }
    llvm::Value* generateCode(const CodeGenParams& params) override;
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
    void print(std::string ident) override;
    llvm::Value* generateCode(const CodeGenParams& params) override;

    virtual ~IfNode() {
        delete condition;
        delete trueBody;
        delete falseBody;
    }
};


/**
 *
 */
class FieldDeclarationNode{
    std::string varName;
    TreeNode* initializer;
public:
    FieldDeclarationNode(const std::string &varName, TreeNode *initializer) : varName(varName), initializer(initializer) {}
    void print(std::string ident);
    FieldInfo generateCode(const CodeGenParams &params, std::unordered_map<std::string, FieldInfo> &fields);

    virtual ~FieldDeclarationNode() {
        delete initializer;
    }
};

/**
 *
 */
class VarDeclarationNode : public TreeNode {
    std::string varName;
    TreeNode* initializer;
public:
    VarDeclarationNode(const std::string &varName, TreeNode *initializer) : varName(varName), initializer(initializer) {}
    void print(std::string ident) override;
    llvm::Value *generateCode(const CodeGenParams &params) override;

    virtual ~VarDeclarationNode() {
        delete initializer;
    }
};

/**
 *
 */
class VarAssignmentNode : public TreeNode {
    std::string varName;
    TreeNode* initializer;
public:
    VarAssignmentNode(const std::string &varName, TreeNode *initializer) : varName(varName), initializer(initializer) {}
    void print(std::string ident) override;
    llvm::Value *generateCode(const CodeGenParams &params) override;

    virtual ~VarAssignmentNode() {
        delete initializer;
    }
};

/**
 *
 */
class FieldAssignmentNode : public TreeNode {
    std::string varName;
    std::string fieldName;
    TreeNode* initializer;
public:
    FieldAssignmentNode(const std::string &varName, const std::string &fieldName,  TreeNode *initializer) : varName(varName), fieldName(fieldName), initializer(initializer) {}
    void print(std::string ident) override;
    llvm::Value *generateCode(const CodeGenParams &params) override;

    virtual ~FieldAssignmentNode() {
        delete initializer;
    }
};

/**
 *
 */
class ReturnNode : public TreeNode {
    TreeNode* expression;
public:
    ReturnNode(TreeNode *expression) : expression(expression) {}
    void print(std::string ident) override;
    llvm::Value* generateCode(const CodeGenParams& params) override;

    virtual ~ReturnNode() {
        delete expression;
    }

};

/**
 *
 */
class VariableNode  : public TreeNode {
    std::string varName;
public:
    VariableNode(const std::string &varName) : varName(varName) {}
    void print(std::string ident) override;
    llvm::Value *generateCode(const CodeGenParams &params) override;
};

/**
 *
 */
class FieldAccessNode  : public TreeNode  {
    std::string varName;
    std::string field;
public:
    FieldAccessNode(const std::string &varName, const std::string &field) : varName(varName), field(field) {}
    void print(std::string ident) override;
    llvm::Value *generateCode(const CodeGenParams &params) override;
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
    void print(std::string ident) override;

    llvm::Value *generateCode(const CodeGenParams &params) override;

    virtual ~MethodInvokeNode() {
        for (auto it = args.begin(); it != args.end(); ++it)
            delete (*it);
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

    void print(std::string ident) override;
    llvm::Value *generateCode(const CodeGenParams &params) override;

    virtual ~ConstructorInvokeNode() {
        for (auto it = args.begin(); it != args.end(); ++it)
            delete (*it);
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
    void print(std::string ident) override;
    llvm::Value *generateCode(const CodeGenParams &params) override;

    virtual ~BodyNode() {
        for (auto it = statements.begin(); it != statements.end(); ++it)
            delete (*it);
    }
};

/**
 *
 */
class ClassBodyNode  : public TreeNode {
public:
    std::unordered_map <std::string, std::string> symbols;
    std::vector <FieldDeclarationNode*> varDeclarations;
    std::vector <TreeNode*> methods;
    TreeNode* constructor;
    ClassBodyNode(const std::unordered_map<std::string, std::string> &symbols,
                  const std::vector<FieldDeclarationNode *> &varDeclarations, const std::vector<TreeNode *> &methods,
                  TreeNode *constructor) : symbols(symbols), varDeclarations(varDeclarations), methods(methods),
                                           constructor(constructor) {}
    void print(std::string ident) override;

    virtual ~ClassBodyNode() {
        delete constructor;
        for (auto it = varDeclarations.begin(); it != varDeclarations.end(); ++it)
            delete (*it);
        for (auto it = methods.begin(); it != methods.end(); ++it)
            delete (*it);
    }
};

/**
 *
 */
class ParamNode  : public TreeNode {
public:
    std::string paramName;
    std::string paramType;
    ParamNode(const std::string &paramName, const std::string &paramType) : paramName(paramName),
                                                                            paramType(paramType) {}
    void print(std::string ident) override;
};

/**
 *
 */
class MethodDeclarationNode  : public TreeNode {
    std::string methodName;
    std::string returnTypeName;
    std::vector<ParamNode*> params;
    TreeNode* body;
public:
    MethodDeclarationNode(const std::string &methodName, const std::string &returnTypeName,
                          const std::vector<ParamNode *> &params, TreeNode *body) : methodName(methodName),
                                                                                   returnTypeName(returnTypeName),
                                                                                   params(params), body(body) {}
    void print(std::string ident);

    virtual ~MethodDeclarationNode() {
        delete body;
        for (auto it = params.begin(); it != params.end(); ++it)
            delete (*it);
    }
};

/**
 *
 */
class ConstructorDeclarationNode  : public TreeNode {
public:
    std::vector<ParamNode*> params;
    TreeNode* body;
    ConstructorDeclarationNode(const std::vector<ParamNode *> &params, TreeNode *body) : params(params), body(body) {}
    void print(std::string ident) override;

    virtual ~ConstructorDeclarationNode() {
        delete body;
        for (auto it = params.begin(); it != params.end(); ++it)
            delete (*it);
    }
};

/**
 *
 */
class ClassNode  : public TreeNode {
    std::string className;
    std::string classTemplate;
    std::string baseClass;
    ClassBodyNode* classBody;

public:
    ClassNode(const std::string &className, const std::string &classTemplate, const std::string &baseClass,
              ClassBodyNode *classBody) : className(className), classTemplate(classTemplate), baseClass(baseClass),
                                     classBody(classBody) {}
    void print(std::string ident) override;
    llvm::Value *generateCode(const CodeGenParams &params) override;

    virtual ~ClassNode() {
        delete classBody;
    }
};


#endif //COMPILER_EXPRESSIONNODES_H

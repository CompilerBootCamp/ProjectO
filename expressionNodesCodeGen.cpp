//
// Created by user on 14.10.2019.
//
#include <iostream>
#include "expressionNodes.h"

static void throwError(std::string error) {
    std::cout << error << std::endl;
    throw std::exception();
}

static llvm::Value* findVariable(const CodeGenParams &params, const std::string& name) {
    for (int i = params.symbolTables.size()-1; i >= 0; --i) {
        auto symbolIt = params.symbolTables[i].find(name);
        if (symbolIt != params.symbolTables[i].end())
            return symbolIt->second;
    }
    return nullptr;
}

static ClassInfo* findClass(const CodeGenParams &params, const llvm::Type* type) {
    for(auto it = params.classes.begin(); it != params.classes.end(); ++it)
        if (llvm::PointerType::getUnqual(it->second.type) == type)
            return &it->second;
}

static FieldInfo* findField(const CodeGenParams &params, ClassInfo* classInfo, const std::string& name) {
    auto it = classInfo->fields.find(name);
    if (it != classInfo->fields.end())
        return &it->second;
    return nullptr;
}

static const std::string& getTypeName(const CodeGenParams &params, const llvm::Type* type) {
    for(auto it = params.classes.begin(); it != params.classes.end(); ++it)
        if (llvm::PointerType::getUnqual(it->second.type) == type)
            return it->first;
    throwError("Type not found");
}

llvm::Value* VariableNode::generateCode(const CodeGenParams &params) {
    // ищем обычную переменную
    llvm::Value* var = findVariable(params, varName);
    if (var != nullptr)
        return params.builder.CreateLoad(var, "load"+varName);

    // смотрим, не является ли переменная полем текущего класса
    llvm::Value* thisInstance = findVariable(params, "strct");
    ClassInfo* thisClass = findClass(params, thisInstance->getType());
    FieldInfo* fieldInfo = findField(params, thisClass, varName);
    if (fieldInfo != nullptr) {
        // является полем, обращаемся к соответствующему полю структуры
        std::vector<llvm::Value *> indices(2);
        indices[0] = llvm::ConstantInt::get(params.context, llvm::APInt(32, 0, true));
        indices[1] = llvm::ConstantInt::get(params.context, llvm::APInt(32, fieldInfo->number, true));;

        llvm::Value *member_ptr = params.builder.CreateGEP(thisClass->type, thisInstance, indices, "membptr"+varName);
        return params.builder.CreateLoad(member_ptr, "load"+varName);
    }
    throwError("Variable not found");
}

llvm::Value* FieldAccessNode::generateCode(const CodeGenParams &params) {
    llvm::Value* classInstance = findVariable(params, varName);             ///< ищем экземпляр класса по имени переменной
    if (classInstance == nullptr)
        throwError("Variable not found " + varName);
    ClassInfo* classInfo = findClass(params, classInstance->getType());     ///< ищем информацию о классе переменной
    FieldInfo* fieldInfo = findField(params, classInfo, field);           ///< ищем информацию о поле в классе
    if (fieldInfo != nullptr) {
        // является полем, обращаемся к соответствующему полю структуры
        std::vector<llvm::Value *> indices(2);
        indices[0] = llvm::ConstantInt::get(params.context, llvm::APInt(32, 0, true));
        indices[1] = llvm::ConstantInt::get(params.context, llvm::APInt(32, fieldInfo->number, true));;

        llvm::Value *member_ptr = params.builder.CreateGEP(classInfo->type, classInstance, indices, "membptr" + varName);
        return params.builder.CreateLoad(member_ptr, "load"+field);
    }
    throwError("Field not found" + field);
}

llvm::Value *MethodInvokeNode::generateCode(const CodeGenParams &params) {
    llvm::Value* classInst = findVariable(params, varName);
    if (classInst != nullptr) {
        std::string typeName = getTypeName(params, classInst->getType());
        std::string fullMethodName = typeName + methodName;

        llvm::Function* method = params.module->getFunction(fullMethodName);
        if (!method)
            throwError("Unknown method referenced " + methodName);

        // If argument mismatch error.
        if (method->arg_size() != (args.size() + 1))
            throwError("Incorrect # arguments passed " + methodName);

       std::vector<llvm::Value*> ArgsV;
        ArgsV.push_back(classInst);
        for (unsigned i = 0, e = args.size(); i != e; ++i) {
            llvm::Value* paramValue = args[i]->generateCode(params);
            llvm::AllocaInst* paramTemp = params.builder.CreateAlloca(paramValue->getType(), 0, "allocParam");
            params.builder.CreateStore(paramValue, paramTemp);
            ArgsV.push_back(paramTemp);
        }
        return params.builder.CreateCall(method, ArgsV, "call" + methodName);
    }
    throwError("Variable not found " + varName);
}

llvm::Value* ConstructorInvokeNode::generateCode(const CodeGenParams &params){
    auto classIt = params.classes.find(className);
    if (classIt == params.classes.end())
        throwError("Class not found " + className);
    ClassInfo* classInfo = &classIt->second;


    std::string ctorName = className + "Ctor";
    llvm::Function* ctor = params.module->getFunction(ctorName);
    if (!ctor)
        throwError("Unknown ctor referenced " + className);
    if (ctor->arg_size() != (args.size() + 1))
        throwError("Incorrect # arguments passed");

    std::vector<llvm::Value*> ArgsV;
    llvm::AllocaInst* alloc = params.builder.CreateAlloca(classInfo->type, 0, "alloc" + className);
    ArgsV.push_back(alloc);
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
        llvm::Value* paramValue = args[i]->generateCode(params);
        llvm::AllocaInst* paramTemp = params.builder.CreateAlloca(paramValue->getType(), 0, "allocParam");
        params.builder.CreateStore(paramValue, paramTemp);
        ArgsV.push_back(paramTemp);
    }
    params.builder.CreateCall(ctor, ArgsV, "callctor");
    return params.builder.CreateLoad(alloc, "instLoad");
}


llvm::Value* IfNode::generateCode(const CodeGenParams& params) {
    llvm::Value* conditionValue = condition->generateCode(params);
    if (!conditionValue)
        return nullptr;

    llvm::Function* function = params.builder.GetInsertBlock()->getParent();

    llvm::BasicBlock* thenBlock = llvm::BasicBlock::Create(params.context, "then", function);
    llvm::BasicBlock* elseBlock = llvm::BasicBlock::Create(params.context, "else");
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(params.context, "ifcont");

    params.builder.CreateCondBr(conditionValue, thenBlock, elseBlock);

    // Генерируем код для правдивого условия
    params.builder.SetInsertPoint(thenBlock);
    llvm::Value* thenValue = trueBody->generateCode(params);
    if (!thenValue)
        return nullptr;
    params.builder.CreateBr(mergeBlock);
    thenBlock = params.builder.GetInsertBlock();

    // Генерируем код для ложного условия
    function->getBasicBlockList().push_back(elseBlock);
    params.builder.SetInsertPoint(elseBlock);
    llvm::Value* elseValue = falseBody->generateCode(params);
    if (!elseValue)
        return nullptr;
    params.builder.CreateBr(mergeBlock);
    elseBlock = params.builder.GetInsertBlock();

    // объединяем блоки
    function->getBasicBlockList().push_back(mergeBlock);
    params.builder.SetInsertPoint(mergeBlock);
//    llvm::PHINode* PN = params.builder.CreatePHI(thenValue->getType(), 2, "iftmp");
//
//    PN->addIncoming(thenValue, thenBlock);
//    PN->addIncoming(elseValue, elseBlock);
    return conditionValue;
}

llvm::Value* WhileNode::generateCode(const CodeGenParams& params) {
    llvm::Function* function = params.builder.GetInsertBlock()->getParent();
    llvm::BasicBlock* conditionBlock = llvm::BasicBlock::Create(params.context, "loopCond", function);
    llvm::BasicBlock* loopStartBlock = llvm::BasicBlock::Create(params.context, "loopStart");
    llvm::BasicBlock* loopEndBlock = llvm::BasicBlock::Create(params.context, "loopEnd");

    params.builder.CreateBr(conditionBlock);
    params.builder.SetInsertPoint(conditionBlock);
    llvm::Value* conditionValue = condition->generateCode(params);
    if (!conditionValue)
        return nullptr;
    params.builder.CreateCondBr(conditionValue, loopStartBlock, loopEndBlock);

    // тело цикла
    function->getBasicBlockList().push_back(loopStartBlock);
    params.builder.SetInsertPoint(loopStartBlock);
    llvm::Value* bodyValue = body->generateCode(params);
    if (!bodyValue)
        return nullptr;
    params.builder.CreateBr(conditionBlock);
    loopStartBlock = params.builder.GetInsertBlock();

    // выход из цикла
    function->getBasicBlockList().push_back(loopEndBlock);
    params.builder.SetInsertPoint(loopEndBlock);
    return conditionValue;

}


FieldInfo FieldDeclarationNode::generateCode(const CodeGenParams &params, std::unordered_map<std::string, FieldInfo> &fields){
    llvm::Value* fieldInit = initializer->generateCode(params);
    llvm::Type* fieldType = fieldInit->getType();
    int index = fields.size();
    FieldInfo fieldInfo = {index, fieldType, fieldInit};
    fields[varName] = fieldInfo;
    return fieldInfo;
}

llvm::Value* VarDeclarationNode::generateCode(const CodeGenParams &params) {
    llvm::Value* varInit = initializer->generateCode(params);
    llvm::Type* varType = varInit->getType();
    llvm::AllocaInst *alloc = params.builder.CreateAlloca(varType, 0, "alloc"+varName);
    params.builder.CreateStore(varInit, alloc);
    // сохраняем в таблице симовлов для текущей области видимости
    params.symbolTables[params.symbolTables.size()-1][varName] = alloc;
    return alloc;
}

static llvm::Value* storeValueToStructField(const CodeGenParams &params, llvm::Value* classInstance, const std::string fieldName, llvm::Value* initValue) {
    ClassInfo* classInfo = findClass(params, classInstance->getType());     ///< ищем информацию о классе переменной
    FieldInfo* fieldInfo = findField(params, classInfo, fieldName);           ///< ищем информацию о поле в классе
    if (fieldInfo != nullptr) {
        // является полем, обращаемся к соответствующему полю структуры
        std::vector<llvm::Value *> indices(2);
        indices[0] = llvm::ConstantInt::get(params.context, llvm::APInt(32, 0, true));
        indices[1] = llvm::ConstantInt::get(params.context, llvm::APInt(32, fieldInfo->number, true));;

        llvm::Value *member_ptr = params.builder.CreateGEP(classInfo->type, classInstance, indices, "membptr" + fieldName);
        return params.builder.CreateStore(initValue, member_ptr);
    }
    throwError("Field not found");
}

llvm::Value* VarAssignmentNode::generateCode(const CodeGenParams &params) {
    llvm::Value *initValue = initializer->generateCode(params);
    llvm::Value* var = findVariable(params, varName);
    if (var != nullptr) {
        params.builder.CreateStore(initValue, var);
        return initValue;
    }

    // смотрим, не является ли переменная полем текущего класса
    llvm::Value* thisInstance = findVariable(params, "strct");
    return storeValueToStructField(params, thisInstance, varName, initValue);
}

llvm::Value* FieldAssignmentNode::generateCode(const CodeGenParams &params) {
    llvm::Value* initValue = initializer->generateCode(params);
    llvm::Value* classInstance = findVariable(params, varName);             ///< ищем экземпляр класса по имени переменной
    if (classInstance == nullptr)
        throwError("Variable not found");
    return storeValueToStructField(params, classInstance, fieldName, initValue);
}

llvm::Value* ReturnNode::generateCode(const CodeGenParams &params) {
    params.builder.CreateRet(expression->generateCode(params));
}


llvm::Value* BodyNode::generateCode(const CodeGenParams &params) {
    for (auto it = statements.begin(); it != statements.end(); ++it)
        (*it)->generateCode(params);
}

llvm::Value *ClassNode::generateCode(const CodeGenParams &params) {
    // создаем структуру для класса
    ClassInfo classInfo;
    llvm::StructType* classType = llvm::StructType::create(params.context, className);
    std::vector<llvm::Type*> members;
    std::vector<llvm::Value*> memberInits;
    for (auto it = classBody->varDeclarations.begin(); it != classBody->varDeclarations.end(); ++it) {
        FieldInfo fieldInfo = (*it)->generateCode(params, classInfo.fields);
        members.push_back(fieldInfo.type);
        memberInits.push_back(fieldInfo.initializer);
    }
    classType->setBody(members);
    classInfo.type = classType;
    params.classes[className] = classInfo;

    // создаем конструктор
    // создаем прототип конструктора
    ConstructorDeclarationNode *constructor = (ConstructorDeclarationNode *) classBody->constructor;
    std::vector<llvm::Type *> args;
    args.push_back(llvm::PointerType::getUnqual(classType));
    for (auto it = constructor->params.begin(); it != constructor->params.end(); ++it) {
        args.push_back(llvm::PointerType::getUnqual(params.classes[(*it)->paramType].type));
    }
    llvm::FunctionType *funcType = llvm::FunctionType::get(llvm::PointerType::getUnqual(classType), args, false);
    llvm::Function *function = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, className + "Ctor",
                                                      params.module.get());

    function->arg_begin()->setName("strct");
    int index = 0;
    for (auto it = function->arg_begin() + 1; it != function->arg_end(); ++it) {
        (*it).setName(constructor->params[index++]->paramName);
    }

    SymbolTable ctorVars;
    for (auto &Arg : function->args())
        ctorVars[Arg.getName()] = &Arg;
    params.symbolTables.push_back(ctorVars);

    params.builder.SetInsertPoint(llvm::BasicBlock::Create(params.context, "entry", function));
    // инициализируем в конструторе все поля
    for (int index = 0; index < memberInits.size(); index++) {
        std::vector<llvm::Value *> indices(2);
        indices[0] = llvm::ConstantInt::get(params.context, llvm::APInt(32, 0, true));
        indices[1] = llvm::ConstantInt::get(params.context, llvm::APInt(32, index, true));;

        llvm::Value *member_ptr = params.builder.CreateGEP(classType, ctorVars["strct"], indices, "membptr");
        params.builder.CreateStore(memberInits[index], member_ptr);
    }
    // генерируем код для тела конструктора
    constructor->body->generateCode(params);
    params.builder.CreateRet(ctorVars["strct"]);
}

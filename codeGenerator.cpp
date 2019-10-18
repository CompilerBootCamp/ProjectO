//
// Created by user on 14.10.2019.
//
#include <iostream>
#include <llvm/ADT/APInt.h>
#include "codeGenerator.h"
#include "expressionNodes.h"

enum {Math, RelationInt, RelationFloat};

static llvm::LLVMContext context;
static llvm::IRBuilder<> builder(context);
static std::unique_ptr<llvm::Module> module = llvm::make_unique<llvm::Module>("top", context);
static std::vector<SymbolTable> symbolTables;

static llvm::Function* generateBinaryFunction(int operation, llvm::FunctionType *funcType, std::string name, int type) {
    llvm::Function *function = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, name, module.get());
    auto firstArg = function->arg_begin();
    auto secondArg = firstArg+1;

    firstArg->setName("classPtr");
    secondArg->setName("value");
    builder.SetInsertPoint(llvm::BasicBlock::Create(context, "entry", function));

    switch (type) {
        case Math:
            builder.CreateRet(builder.CreateBinOp(static_cast<llvm::Instruction::BinaryOps>(operation), builder.CreateLoad(function->arg_begin(),"deref1"), builder.CreateLoad(secondArg,"deref2"), "result"));
            break;
        case RelationInt:
            builder.CreateRet(builder.CreateICmp(static_cast<llvm::CmpInst::Predicate>(operation), builder.CreateLoad(function->arg_begin(),"deref1"), builder.CreateLoad(secondArg,"deref2"), "result"));
            break;
        case RelationFloat:
            builder.CreateRet(builder.CreateFCmp(static_cast<llvm::CmpInst::Predicate>(operation), builder.CreateLoad(function->arg_begin(),"deref1"), builder.CreateLoad(secondArg,"deref2"), "result"));
            break;
    }
    return function;
}

static llvm::Function* generateBinaryMathFunction(int operation, llvm::FunctionType *funcType, std::string name) {
    return generateBinaryFunction(operation, funcType, name, Math);
}

static llvm::Function* generateIntRelationFunction(int predicate, llvm::FunctionType *funcType, std::string name) {
    return generateBinaryFunction(predicate, funcType, name, RelationInt);
}

static llvm::Function* generateFloatRelationFunction(int predicate, llvm::FunctionType *funcType, std::string name) {
    return generateBinaryFunction(predicate, funcType, name, RelationFloat);
}

static void generateCodeForIntegerBinary(std::unordered_map<std::string, llvm::Function*> &methods) {
    std::vector<llvm::Type *> args;
    args.push_back(llvm::Type::getInt32Ty(context)->getPointerTo());
    args.push_back(llvm::Type::getInt32Ty(context)->getPointerTo());
    llvm::FunctionType* funcType = llvm::FunctionType::get(builder.getInt32Ty(), args, false);

    methods["IntegerPlusInteger"] = generateBinaryMathFunction(llvm::Instruction::BinaryOps::Add, funcType,"IntegerPlusInteger");
    methods["IntegerMinusInteger"] = generateBinaryMathFunction(llvm::Instruction::BinaryOps::Sub, funcType, "IntegerMinusInteger");
    methods["IntegerMulInteger"] = generateBinaryMathFunction(llvm::Instruction::BinaryOps::Mul, funcType,"IntegerMulInteger");
    methods["IntegerDivInteger"] = generateBinaryMathFunction(llvm::Instruction::BinaryOps::SDiv, funcType, "IntegerDivInteger");

    funcType = llvm::FunctionType::get(builder.getInt1Ty(), args, false);
    methods["IntegerEqInteger"] = generateIntRelationFunction(llvm::CmpInst::Predicate::ICMP_EQ, funcType, "IntegerEqInteger");
    methods["IntegerLessInteger"] = generateIntRelationFunction(llvm::CmpInst::Predicate::ICMP_SLT, funcType, "IntegerLessInteger");
    methods["IntegerLessEqualInteger"] = generateIntRelationFunction(llvm::CmpInst::Predicate::ICMP_SLE, funcType, "IntegerLessEqualInteger");
    methods["IntegerGreaterInteger"] = generateIntRelationFunction(llvm::CmpInst::Predicate::ICMP_SGT, funcType, "IntegerGreaterInteger");
    methods["IntegerGreaterEqualInteger"] = generateIntRelationFunction(llvm::CmpInst::Predicate::ICMP_SGE, funcType, "IntegerGreaterEqualInteger");
}

static void generateCodeForDoubleBinary(std::unordered_map<std::string, llvm::Function*> &methods) {
    std::vector<llvm::Type *> args;
    args.push_back(llvm::Type::getDoubleTy(context)->getPointerTo());
    args.push_back(llvm::Type::getDoubleTy(context)->getPointerTo());
    llvm::FunctionType *funcType = llvm::FunctionType::get(builder.getDoubleTy(), args, false);

    methods["RealRealPlus"] = generateBinaryMathFunction(llvm::Instruction::BinaryOps::FAdd, funcType,"RealPlusReal");
    methods["RealRealMinus"] = generateBinaryMathFunction(llvm::Instruction::BinaryOps::FSub, funcType, "RealMinusReal");
    methods["RealRealMul"] = generateBinaryMathFunction(llvm::Instruction::BinaryOps::FMul, funcType,"RealMulReal");
    methods["RealRealDiv"] = generateBinaryMathFunction(llvm::Instruction::BinaryOps::FDiv, funcType, "RealDivReal");

    funcType = llvm::FunctionType::get(builder.getInt1Ty(), args, false);
    methods["RealRealEq"] = generateFloatRelationFunction(llvm::CmpInst::Predicate::FCMP_OEQ, funcType, "RealEqReal");
    methods["RealRealLess"] = generateFloatRelationFunction(llvm::CmpInst::Predicate::FCMP_OLT, funcType, "RealLessReal");
    methods["RealRealLessEqual"] = generateFloatRelationFunction(llvm::CmpInst::Predicate::FCMP_OLE, funcType, "RealLessEqualReal");
    methods["RealRealGreater"] = generateFloatRelationFunction(llvm::CmpInst::Predicate::FCMP_OGT, funcType, "RealGreaterReal");
    methods["RealRealGreaterEqual"] = generateFloatRelationFunction(llvm::CmpInst::Predicate::FCMP_OGE, funcType, "RealGreaterEqualReal");
}

struct ProgramStruct {
    int result;
};

void generateCode(std::vector<TreeNode*> &baseNode) {

    llvm::InitializeNativeTarget();

    static std::map<std::string, ClassInfo> classes;
    classes["Integer"] = ClassInfo(llvm::Type::getInt32Ty(context));
    classes["Real"] = ClassInfo(llvm::Type::getDoubleTy(context));
    classes["Boolean"] = ClassInfo(llvm::Type::getInt1Ty(context));

    CodeGenParams params = {context, builder, module, symbolTables, classes};
    generateCodeForIntegerBinary(classes["Integer"].methods);
    generateCodeForDoubleBinary(classes["Real"].methods);

    for (auto it = baseNode.begin(); it != baseNode.end(); ++it)
        (*it)->generateCode(params);

    module->print(llvm::errs(), nullptr);

    llvm::Function* startFunction = module->getFunction("FibCtor");
    llvm::ExecutionEngine* executionEngine = llvm::EngineBuilder(std::move(module)).create();

    ProgramStruct programStruct;
    int arg = 0;

    std::vector<llvm::GenericValue> args(2);
    args[0].PointerVal = &programStruct;
    args[1].PointerVal = &arg;
    executionEngine->runFunction(startFunction, args);


    std::cout << std::endl << "Calculating Fibonacci:" << std::endl;
    std::cout << "0 = " << programStruct.result << std::endl;

    arg = 1;
    executionEngine->runFunction(startFunction, args);
    std::cout << "1 = " << programStruct.result << std::endl;

    arg = 2;
    executionEngine->runFunction(startFunction, args);
    std::cout << "2 = " << programStruct.result << std::endl;

    arg = 20;
    executionEngine->runFunction(startFunction, args);
    std::cout << "20 = " << programStruct.result << std::endl;

    arg = 30;
    executionEngine->runFunction(startFunction, args);
    std::cout << "30 = " << programStruct.result << std::endl;

    arg = 40;
    executionEngine->runFunction(startFunction, args);
    std::cout << "40 = " << programStruct.result << std::endl;

    arg = 46;
    executionEngine->runFunction(startFunction, args);
    std::cout << "46 = " << programStruct.result << std::endl;

};

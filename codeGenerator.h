//
// Created by user on 14.10.2019.
//

#ifndef COMPILER_CODEGENERATOR_H
#define COMPILER_CODEGENERATOR_H

#include "expressionNodes.h"

void generateCode(std::vector<TreeNode*> &baseNode, std::string cmdCtor, std::vector<int> cmdArgs);

#endif //COMPILER_CODEGENERATOR_H

//
// Created by user on 03.10.2019.
//

#ifndef COMPILER_LEXANALIZER_H
#define COMPILER_LEXANALIZER_H

#include <vector>
#include <cstdio>
#include "tokens.h"

std::vector<Tokens*> makeLexAnalysis(const char* str, const char* endStr);

void deleteVector(std::vector<Tokens*> & v);

#endif //COMPILER_LEXANALIZER_H

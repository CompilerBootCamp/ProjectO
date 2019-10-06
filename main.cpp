#include <iostream>
#include "lexAnalizer.h"
#include "parser.h"
#include <string.h>
#include <fstream>

static std::ifstream::pos_type getFileSize(const char* fileName) {
    std::ifstream in(fileName, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

int main() {
    // Предыдущий main с лексером перенес в файл lexx2.cpp
    // main_lexx2();

    size_t size = getFileSize("testClasses.txt");
    char* fileData = new char[size];
    FILE* file = fopen("testClasses.txt", "rb");
    if (file == NULL)
        return -1;
    fread(fileData, size, 1, file);
    auto v = makeLexAnalysis(fileData, fileData + size);

    std::cout << "Lexer results:" << std::endl;
    for (auto it = v.begin(); it != v.end(); ++it)
        (*it)->printInformation();

    std::cout << std::endl << "Parser results:" << std::endl;
    try {
        ProgramNode program(v.begin(), v.end());
        program.print();
    }
    catch(std::exception const&e) {

    }

    delete[] fileData;
    deleteVector(v);
}

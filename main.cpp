#include <iostream>
#include "lexAnalizer.h"
#include "parser.h"
#include <string.h>
#include <fstream>
#include "codeGenerator.h"

static std::ifstream::pos_type getFileSize(const char* fileName) {
    std::ifstream in(fileName, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

int main(int argc, char* argv[]) {
    if (argc <= 1)
        return -1;

    std::string ctorName(argv[1]);
    std::vector<int> args;
    for (int i=2; i<argc; i++)
        args.push_back(std::atoi(argv[i]));

    size_t size = getFileSize("testClasses.txt");
    char* fileData = new char[size];
    FILE* file = fopen("testClasses.txt", "rb");
    if (file == NULL)
        return -1;
    fread(fileData, size, 1, file);
    auto v = makeLexAnalysis(fileData, fileData + size);

    std::cout << "Lexer results:" << std::endl;
//    for (auto it = v.begin(); it != v.end(); ++it)
//        (*it)->printInformation();

    std::vector<TreeNode*> classes;
    try {
        std::cout << std::endl << "Parser results:" << std::endl;
        Parser parser(v.begin(), v.end());
        std::vector<TreeNode*> classes = parser.parseTokens();
        parser.print();
        generateCode(classes, ctorName, args);
        for (auto it = classes.begin(); it != classes.end(); ++it)
            delete(*it);
    }
    catch(std::exception const&e) {

    }

    for (auto it = classes.begin(); it != classes.end(); ++it)
        delete(*it);
    deleteVector(v);
    delete[] fileData;
    return 0;
}

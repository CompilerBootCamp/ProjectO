#include <iostream>
#include "lexAnalizer.h"
#include <string.h>
#include <fstream>

static std::ifstream::pos_type getFileSize(const char* fileName) {
    std::ifstream in(fileName, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

int main() {
    size_t size = getFileSize("test.txt");
    char* fileData = new char[size];
    FILE* file = fopen("test.txt", "rb");
    if (file == NULL)
        return -1;
    fread(fileData, size, 1, file);
    auto v = makeLexAnalysis(fileData, fileData + size);

    for (auto it = v.begin(); it != v.end(); ++it)
        std::cout << (*it)->image << std::endl;

    delete[] fileData;
    deleteVector(v);
}
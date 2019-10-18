cmake_minimum_required(VERSION 3.10)
project(compiler)
set(CMAKE_CXX_STANDARD 17)

find_package(LLVM REQUIRED CONFIG PATHS D:/SamsungCompilerBootCamp/llvm9/llvm9/cmake-build-debug/lib/cmake/llvm)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(compiler main.cpp lexAnalizer.cpp lexAnalizer.h tokens.h parser.h parser.cpp expressionNodes.h tokens.cpp codeGenerator.cpp codeGenerator.h expressionNodesPrint.cpp expressionNodesCodeGen.cpp)

llvm_map_components_to_libnames(llvm_libs support core irreader executionengine interpreter nativecodegen)
target_link_libraries(compiler ${llvm_libs})

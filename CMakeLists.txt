cmake_minimum_required(VERSION 3.0)
project(hzip)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pipe -flto -march=native -Ofast")                
add_library(shared SHARED shared.cpp)   
add_executable(hzip main.cpp)          
include (GenerateExportHeader)          
GENERATE_EXPORT_HEADER(shared
    BASE_NAME shared
    EXPORT_MACRO_NAME shared_EXPORTS
    EXPORT_FILE_NAME shared_EXPORTS.h
    STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC)
target_link_libraries(hzip shared) 

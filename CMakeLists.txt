cmake_minimum_required(VERSION 3.0)
project(hzip)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pipe -flto -march=native -Ofast")                
add_library(codec SHARED codec.cpp)   

set (HEADERS 
	tester.h)
 
set (SOURCES 
	tester.cpp
	main.cpp
	codec.cpp)

add_executable(hzip ${PROJECT} ${HEADERS} ${SOURCES})           
include (GenerateExportHeader)          
GENERATE_EXPORT_HEADER(codec
    BASE_NAME codec
    EXPORT_MACRO_NAME codec_EXPORTS
    EXPORT_FILE_NAME codec_EXPORTS.h
    STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC)

target_link_libraries(hzip codec) 

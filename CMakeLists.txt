cmake_minimum_required(VERSION 3.1)
include(ExternalProject)

project(loop_extractor.cpp)

find_package(LLVM REQUIRED CONFIG)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
include_directories(include)

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)

# JsonCPP
find_package(Jsoncpp QUIET)
if(NOT Jsoncpp_FOUND)
  ExternalProject_Add(jsoncpp
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    CMAKE_ARGS -DJSON_BuildTests=Off -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}/jsoncpp/jsoncpp
    PREFIX ${EXTERNAL_INSTALL_LOCATION}/jsoncpp/build
    UPDATE_DISCONNECTED 1
    BUILD_ALWAYS 1
    INSTALL_DIR ${EXTERNAL_INSTALL_LOCATION}/jsoncpp/jsoncpp
  )
  set(JSONCPP_INCLUDE_DIRS ${EXTERNAL_INSTALL_LOCATION}/jsoncpp/jsoncpp/include)
endif()
include_directories(${JSONCPP_INCLUDE_DIRS})

add_library(LLVMLoopTools STATIC lib/io/SCEVAnalyzer.cpp lib/LoopCounters.cpp
        lib/io/ValueToString.cpp lib/LoopClassification.cpp lib/LoopIV.cpp
        lib/util.cpp)
set_property(TARGET LLVMLoopTools PROPERTY CXX_STANDARD 14)
set_target_properties(LLVMLoopTools PROPERTIES COMPILE_FLAGS "-fno-rtti")
set_target_properties(LLVMLoopTools PROPERTIES POSITION_INDEPENDENT_CODE On)
add_dependencies(LLVMLoopTools jsoncpp)

add_library(LLVMLoopExtractor MODULE lib/LoopExtractorPass.cpp lib/LoopExtractor.cpp)
add_library(LLVMLoopStatistics MODULE lib/LoopStatisticsPass.cpp)
target_link_libraries(LLVMLoopExtractor PRIVATE LLVMLoopTools)
target_link_libraries(LLVMLoopStatistics PRIVATE LLVMLoopTools)
set_property(TARGET LLVMLoopExtractor PROPERTY CXX_STANDARD 14)
set_property(TARGET LLVMLoopStatistics PROPERTY CXX_STANDARD 14)
set_target_properties(LLVMLoopExtractor PROPERTIES COMPILE_FLAGS "-fno-rtti")
set_target_properties(LLVMLoopStatistics PROPERTIES COMPILE_FLAGS "-fno-rtti")


cmake_minimum_required(VERSION 3.15)
project(database_implementation_3_0)

set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

include_directories(.)

configure_file(catalog ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(test.cat ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

add_executable(database_implementation_3_0_test
        test.cc
        test.h
        BigQ.h
        BigQ.cc
        Pipe.cc
        Pipe.h
        Comparison.cc
        Comparison.h
        ComparisonEngine.cc
        ComparisonEngine.h
        DBFile.cc
        DBFile.h
        GenericDBFile.cc
        GenericDBFile.h
        Defs.h
        File.cc
        File.h
        Record.cc
        Record.h
        Schema.cc
        Schema.h
        TwoWayList.cc
        TwoWayList.h
        DBFileMetaData.cc
        DBFileMetaData.h
        HeapFile.cc
        HeapFile.h
        RelOp.cc
        RelOp.h
        Function.cc
        Function.h
        SortedFile.cc
        SortedFile.h
        y.tab.o
        yyfunc.tab.o
        lex.yy.c
        lex.yyfunc.c
        )
add_executable(database_implementation_3_0
        a1-test.cc
        a1-test.h
        a2-test.cc
        a2-test.h
        a2_2-test.cc
        a2_2-test.h
        BigQ.cc
        BigQ.h
        BigQTests.cc
        Comparison.cc
        Comparison.h
        ComparisonEngine.cc
        ComparisonEngine.h
        DBFile.cc
        DBFile.h
        DBFileMetaData.cc
        DBFileMetaData.h
        Defs.h
        File.cc
        File.h
        Function.cc
        Function.h
        GenericDBFile.cc
        GenericDBFile.h
        HeapFile.cc
        HeapFile.h
        HeapFileGTests.cc
        main.cc
        ParseFunc.h
        ParseTree.h
        Pipe.cc
        Pipe.h
        Record.cc
        Record.h
        RelOp.cc
        RelOp.h
        Schema.cc
        Schema.h
        SortedFile.cc
        SortedFile.h
        test.cc
        test.h
        TwoWayList.cc
        TwoWayList.h)

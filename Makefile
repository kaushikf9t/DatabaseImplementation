GTEST_CFLAGS = `pkg-config --cflags Google_tests`
GTEST_LIBS = `pkg-config --libs Google_tests`
CC = g++ -Wno-deprecated -Wall

tag = -i
test_out_tag = -ll

ifdef linux
tag = -n
test_out_tag = -lfl
endif

HeapFileGTests.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o GenericDBFile.o y.tab.o lex.yy.o HeapFileGTests.o
	$(CC) -o HeapFileGTests.out Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o GenericDBFile.o y.tab.o lex.yy.o HeapFileGTests.o $(test_out_tag) -lpthread -lgtest

BigQTests.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o GenericDBFile.o Pipe.o y.tab.o lex.yy.o BigQTests.o
	$(CC) -o BigQTests.out Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o GenericDBFile.o SortedFile.o HeapFile.o Pipe.o y.tab.o lex.yy.o BigQTests.o $(test_out_tag) -lpthread -lgtest

test.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o GenericDBFile.o HeapFile.o SortedFile.o DBFileMetaData.o Pipe.o RelOp.o Function.o y.tab.o lex.yy.o yyfunc.tab.o lex.yyfunc.o test.o
	$(CC) -o test.out Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o GenericDBFile.o HeapFile.o SortedFile.o DBFileMetaData.o Pipe.o RelOp.o Function.o y.tab.o lex.yy.o yyfunc.tab.o lex.yyfunc.o test.o $(test_out_tag) -lpthread

a22test.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o GenericDBFile.o SortedFile.o HeapFile.o DBFileMetaData.o Pipe.o y.tab.o lex.yy.o a2_2-test.o
	$(CC) -o a22test.out Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o GenericDBFile.o SortedFile.o HeapFile.o DBFileMetaData.o Pipe.o y.tab.o lex.yy.o a2_2-test.o $(test_out_tag) -lpthread

a2test.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o GenericDBFile.o SortedFile.o HeapFile.o DBFileMetaData.o Pipe.o y.tab.o lex.yy.o a2test.o
	$(CC) -o a2test.out Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o GenericDBFile.o SortedFile.o HeapFile.o DBFileMetaData.o Pipe.o y.tab.o lex.yy.o a2test.o $(test_out_tag) -lpthread

a1test.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o GenericDBFile.o SortedFile.o HeapFile.o Pipe.o y.tab.o DBFileMetaData.o lex.yy.o a1-test.o
	$(CC) -o a1test.out Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o GenericDBFile.o SortedFile.o HeapFile.o DBFileMetaData.o Pipe.o y.tab.o lex.yy.o a1-test.o $(test_out_tag) -lpthread

main: Record.o Comparison.o ComparisonEngine.o Schema.o File.o y.tab.o lex.yy.o main.o
	$(CC) -o main Record.o Comparison.o ComparisonEngine.o Schema.o File.o y.tab.o lex.yy.o main.o $(test_out_tag)

HeapFileGTests.o: HeapFileGTests.cc
	$(CC) -g -c HeapFileGTests.cc

BigQTests.o: BigQTests.cc
	$(CC) -g -c BigQTests.cc

test.o : test.cc
	$(CC) -g -c test.cc

RelOp.o : RelOp.cc
	$(CC) -g -c RelOp.cc

Function.o : Function.cc
	$(CC) -g -c Function.cc

a2_2-test.o: a2_2-test.cc
	$(CC) -g -c a2_2-test.cc

a2test.o: a2-test.cc
	$(CC) -g -c a2-test.cc

a1-test.o: a1-test.cc
	$(CC) -g -c a1-test.cc

main.o: main.cc
	$(CC) -g -c main.cc
	
Comparison.o: Comparison.cc
	$(CC) -g -c Comparison.cc
	
ComparisonEngine.o: ComparisonEngine.cc
	$(CC) -g -c ComparisonEngine.cc
	
DBFile.o: DBFile.cc
	$(CC) -g -c DBFile.cc

GenericDBFile.o: GenericDBFile.cc
	$(CC) -g -c GenericDBFile.cc

HeapFile.o: HeapFile.cc
	$(CC) -g -c HeapFile.cc

SortedFile.o: SortedFile.cc
	$(CC) -g -c SortedFile.cc

DBFileMetaData.o: DBFileMetaData.cc
	$(CC) -g -c DBFileMetaData.cc

File.o: File.cc
	$(CC) -g -c File.cc

Pipe.o: Pipe.cc
	$(CC) -g -c Pipe.cc

BigQ.o: BigQ.cc
	$(CC) -g -c BigQ.cc

Record.o: Record.cc
	$(CC) -g -c Record.cc

Schema.o: Schema.cc
	$(CC) -g -c Schema.cc
	
y.tab.o: Parser.y
	yacc -d Parser.y
	# sed $(tag) -e "s/  __attribute__ ((__unused__))$$/# ifndef __cplusplus\n  __attribute__ ((__unused__));\n# endif/" y.tab.c
	g++ -c y.tab.c

yyfunc.tab.o: ParserFunc.y
	yacc -p "yyfunc" -b "yyfunc" -d ParserFunc.y
	#sed $(tag) yyfunc.tab.c -e "s/  __attribute__ ((__unused__))$$/# ifndef __cplusplus\n  __attribute__ ((__unused__));\n# endif/"
	g++ -c yyfunc.tab.c

lex.yy.o: Lexer.l
	lex  Lexer.l
	gcc  -c lex.yy.c

lex.yyfunc.o: LexerFunc.l
	lex -Pyyfunc LexerFunc.l
	gcc  -c lex.yyfunc.c

clean: 
	rm -f *.o
	rm -f *.out
	rm -f y.tab.c
	rm -f y.tab.c-e
	rm -f lex.yy.c
	rm -f y.tab.h
	rm -f main
	rm -f *.bin.bigq
	rm -f tmp*.bin
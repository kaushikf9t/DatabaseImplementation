#ifndef REL_OP_H
#define REL_OP_H

#include "Pipe.h"
#include "DBFile.h"
#include "Record.h"
#include "Function.h"

struct InternalData{
    DBFile *inFile;
    Pipe *outPipe;
    Pipe *inPipe;
    CNF *selOp;
    Record *literal;
    int *keepMe;
    int numAttsInput;
    int numAttsOutput;
    Function *computeMe;
    OrderMaker *groupAtts;
    int runLen;
    Schema *mySchema;
    FILE *outFile;

};

class RelationalOp {
protected:
    pthread_t thread;
    int runLen;
public:
	// blocks the caller until the particular relational operator 
	// has run to completion
	void WaitUntilDone (){
        pthread_join(thread, nullptr);
	}

	// tell us how much internal memory the operation can use
    void Use_n_Pages(int n) {
        runLen = n;
    }
};

class SelectFile : public RelationalOp { 

private:
    static void* executeOp(void* data);


public:
	void Run(DBFile &inFile, Pipe &outPipe, CNF &selOp, Record &literal){
	    InternalData *data = static_cast<struct InternalData *>(malloc(sizeof(struct InternalData)));
        data->inFile = &inFile;
        data->outPipe = &outPipe;
        data->selOp = &selOp;
        data->literal = &literal;
        if(pthread_create(&thread, nullptr, executeOp, (void *) data)){
            cout<<"Problem with thread creation";
        }
	}
};

class SelectPipe : public RelationalOp {
private:
    static void* executeOp(void* data);

public:
	void Run (Pipe &inPipe, Pipe &outPipe, CNF &selOp, Record &literal){
        InternalData *data = static_cast<struct InternalData *>(malloc(sizeof(struct InternalData)));
        data->inPipe = &inPipe;
        data->outPipe = &outPipe;
        data->selOp = &selOp;
        data->literal = &literal;
        if(pthread_create(&thread, NULL, executeOp, (void *) data)){
            cout<<"Problem with thread creation"<<endl;
        }
	}
};

class Project : public RelationalOp { 
private:
    static void* executeOp(void* data);

public:
	void Run (Pipe &inPipe, Pipe &outPipe, int *keepMe, int numAttsInput, int numAttsOutput){
        InternalData *data = static_cast<struct InternalData *>(malloc(sizeof(struct InternalData)));
	    data->inPipe = &inPipe;
	    data->keepMe = keepMe;
	    data->numAttsInput = numAttsInput;
	    data->numAttsOutput = numAttsOutput;
        if(pthread_create(&thread, NULL, executeOp, (void *) data)){
            cout<<"Problem with thread creation"<<endl;
        }
	}
};

class Join : public RelationalOp { 
	public:
	void Run (Pipe &inPipeL, Pipe &inPipeR, Pipe &outPipe, CNF &selOp, Record &literal){}
};

class DuplicateRemoval : public RelationalOp {
private:
    static void* executeOp(void* data);
public:
	void Run (Pipe &inPipe, Pipe &outPipe, Schema &mySchema) {
	    InternalData *data;
	    data->inPipe = &inPipe;
	    data->outPipe = &outPipe;
	    data->mySchema = &mySchema;
	    if(pthread_create(&thread, NULL, executeOp, (void *)data)){
	        cout<<"Problem with thread creation"<<endl;
	    }

	}
};

class Sum : public RelationalOp {
private:
    static void* executeOp(void* data);
public:
	void Run (Pipe &inPipe, Pipe &outPipe, Function &computeMe) {
        InternalData *data = static_cast<struct InternalData *>(malloc(sizeof(struct InternalData)));
	    data->inPipe = &inPipe;
	    data->outPipe = &outPipe;
	    data->computeMe = &computeMe;
	    if(pthread_create(&thread, NULL, executeOp, (void*) data)){
	        cout<<"Problem with thread creation"<<endl;
	    }

	}
};

class GroupBy : public RelationalOp {
private:
    static void* executeOp(void* data);
public:
	void Run (Pipe &inPipe, Pipe &outPipe, OrderMaker &groupAtts, Function &computeMe) {
        InternalData *data = static_cast<struct InternalData *>(malloc(sizeof(struct InternalData)));
	    data->inPipe = &inPipe;
	    data->outPipe = &outPipe;
        data->groupAtts = &groupAtts;
        data->computeMe = &computeMe;
        if(pthread_create(&thread, NULL, executeOp, (void*) data)){
            cout<<"Problem with thread creation"<<endl;
        }
	}

    static void writeRecordToFile(InternalData *internalData, int *mergedAtts, Schema *resultSchema,
                                  Record *firstRecord, double GroupResult, Type &groupType);
};

class WriteOut : public RelationalOp {
private:
    static void* executeOp(void* data);
public:
	void Run (Pipe &inPipe, FILE *outFile, Schema &mySchema) {
        InternalData *data = static_cast<struct InternalData *>(malloc(sizeof(struct InternalData)));
	    data->inPipe = &inPipe;
	    data->mySchema = &mySchema;
	    data->outFile = outFile;
	    if(pthread_create(&thread, NULL, executeOp, (void*) data)){
	        cout<<"Problem with thread creation"<<endl;
	    }
	}
};
#endif

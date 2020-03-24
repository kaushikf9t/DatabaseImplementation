#include <iostream>
#include "GenericDBFile.h"
#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "Defs.h"
#include <fstream>
#include <map>
#include <cstring>
#include "BigQ.h"

class SortedFile: public GenericDBFile{
    public:
    SortedFile();
    ~SortedFile();
    
    BigQ *bigQ;
    Pipe *in, *out;
    bool isBigQCreated;

    int createMetaData(const char *fpath, fType file_type, void *startup);
    int Create(const char *fpath, const char *metDataPath, fType file_type, void *startup);
    //int Open(const char *fpath, const char *metadataPath);
    void Add(Record &addme);
    int Open(const char *fpath, const char *metadataPath);
    void initBigQ();
    void MoveFirst();
    int GetNext(Record &fetchme);
    int GetNext(Record &fetchme, CNF &cnf, Record &literal);
    void flushPage();
};
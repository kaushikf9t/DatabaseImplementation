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

    int createMetaData(const char *fpath, fType file_type, void *startup) override;
    int Create(const char *fpath, const char *metDataPath, fType file_type, void *startup)override;
    void Add(Record &addme) override;
    int Open(const char *fpath, const char *metadataPath)override;
    void initBigQ();
    void MoveFirst()override;
    int GetNext(Record &fetchme)override;
    int GetNext(Record &fetchme, CNF &cnf, Record &literal)override;
    void flushPage()override;
};
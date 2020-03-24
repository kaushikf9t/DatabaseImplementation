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

class HeapFile: public GenericDBFile{
    public:
    HeapFile();
    ~HeapFile();

    int createMetaData(const char *fpath, fType file_type, void *startup);
    int Create(const char *fpath, const char *metDataPath, fType file_type, void *startup);
    int Open(const char *fpath, const char *metadataPath);
    void Add (Record &addme);
    void MoveFirst();
    int GetNext(Record &fetchme);
    int GetNext(Record &fetchme, CNF &cnf, Record &literal);
    void flushPage();
    
};
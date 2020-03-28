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

    int createMetaData(const char *fpath, fType file_type, void *startup) override;

    int Create(const char *fpath, const char *metDataPath, fType file_type, void *startup) override;

    int Open(const char *fpath, const char *metadataPath) override;

    void Add (Record &addme) override;

    void MoveFirst() override;

    int GetNext(Record &fetchme) override;

    int GetNext(Record &fetchme, CNF &cnf, Record &literal) override;

    void flushPage() override;
    
};
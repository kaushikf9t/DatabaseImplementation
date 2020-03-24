#ifndef DBFILE_H
#define DBFILE_H

#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "Defs.h"
#include "GenericDBFile.h"
#include "HeapFile.h"
#include "SortedFile.h"
#include <iostream>

class DBFile {

private:
    GenericDBFile *myInternalFile;

public:
    DBFile();

    ~DBFile();

    //int createMetaData(const char *fpath, fType file_type, void *startup);

    int Create(const char *fpath, const char *metDatapath, fType file_type, void *startup);

    int Open(const char *fpath, const char *metadataPath);

    int Close();

    void Load(Schema &myschema, const char *loadpath);

    void MoveFirst();

    void Add(Record &addme);

    int GetNext(Record &fetchme);

    int GetNext(Record &fetchme, CNF &cnf, Record &literal);

    static basic_string<char> getFileType(const char *metaDataPath);

};


#endif

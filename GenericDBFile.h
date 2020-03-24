#ifndef DATABASE_IMPLEMENTATION_GENERICDBFILE_H
#define DATABASE_IMPLEMENTATION_GENERICDBFILE_H


#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "Defs.h"
#include<fstream>
#include <map>
#include <cstring>
#include <unistd.h>
#include "BigQ.h"
#include "Pipe.h"
#include <pthread.h>
#include "DBFileMetaData.h"

class GenericDBFile {
protected:
    /**
     * Stores the file type of DBFile
     
    fType fileType;
    */

    /**
     * Object of a file.
     */
    File *file;

    /**
     * Page used for reads and writes.
     */
    Page *page;

    /**
     * The comparator engine is used by GetNext method which filters records based on CNF passed to it.
     */
    ComparisonEngine comp;

    // We keep separate cursor for reading and writing the page, so we don't need to remember any of them.

    /**
     * Current page where we will append the records
     */
    off_t writePage;

    /**
     * Current page from which we will read the data.
     */
    off_t readPage = 0;

    /**
     * Used as a flag to determine if page need to be flushed while switching from writes to read.
     */
    bool needFlush = false;


    /**
     * Write File Object
     */
    
    ofstream *outfile;

    /**
     * Read File object
     */
    ifstream *infile; 

    /**
     * Hold the OrderMaker and the RunLength needed for the BigQ instance
     * TO-DO Candidates to be moved to the Sorted File
     */
    
    DBFileMetaData *metadata;

    OrderMaker *om;
    
    int runLength;

public:

    GenericDBFile();

    ~GenericDBFile();

    virtual int createMetaData(const char *fpath, fType file_type, void *startup);

    virtual int Create(const char *fpath, const char *metDataPath, fType file_type, void *startup);

    virtual int Open(const char *fpath, const char *metadataPath);

    int Close();

    void Load(Schema &myschema, const char *loadpath);

    virtual void MoveFirst();

    virtual void Add(Record &addme);

    virtual int GetNext(Record &fetchme);

    virtual int GetNext(Record &fetchme, CNF &cnf, Record &literal);

    /**
     * Flush the page if it have some records while switching from writes to read.
     */
    void flushPageIfNeeded();

    /**
     * Flush the page into the file.
     * The page won't have any records after it.
     */
    virtual void flushPage();

};

struct SortInfo { 
    OrderMaker *myOrder; 
    int runLength;
    
};

#endif //DATABASE_IMPLEMENTATION_GENERICDBFILE_H

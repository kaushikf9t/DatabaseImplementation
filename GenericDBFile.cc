#include <iostream>
#include "GenericDBFile.h"


GenericDBFile::GenericDBFile() {
    page = new Page();
    file = new File();
    outfile = new ofstream();
    infile = new ifstream();
}

GenericDBFile::~GenericDBFile() {
    delete page;
    delete file;
}

int GenericDBFile::Create(const char *fpath, const char *metDataPath, fType file_type, void *startup) {}

int GenericDBFile::createMetaData(const char *f_path, fType f_type, void *startup) {
    return 1;
}

int GenericDBFile::Open(const char *f_path, const char *metadataPath) {}

void GenericDBFile::Add(Record &addme) {}

void GenericDBFile::MoveFirst() {}

int GenericDBFile::Close() {
    this->flushPageIfNeeded();

    page->EmptyItOut();
    return file->Close();
}

int GenericDBFile::GetNext(Record &fetchme) {}


int GenericDBFile::GetNext(Record &fetchme, CNF &cnf, Record &literal) {}

void GenericDBFile::flushPageIfNeeded() {
    if (needFlush) {
        this->flushPage();
    }
}

void GenericDBFile::Load(Schema &f_schema, const char *loadpath) {
    FILE *tableFile = fopen(loadpath, "r");

    Record temp;
    int counter = 0;

    while (temp.SuckNextRecord(&f_schema, tableFile) == 1) {
        counter++;
        Add(temp);

        if (counter % 10000 == 0) std::cerr << counter << "\n";
    }

    flushPage();
}

void GenericDBFile::flushPage() {}


#include <iostream>
#include "GenericDBFile.h"


GenericDBFile::GenericDBFile() {
    page = new Page();
    file = new File();
}

GenericDBFile::~GenericDBFile() {
    delete page;
    delete file;
}

int GenericDBFile::Close() {
    this->flushPageIfNeeded();

    page->EmptyItOut();
    return file->Close();
}

void GenericDBFile::flushPageIfNeeded() {
    if (needFlush) {
        this->flushPage();
        this->MoveFirst();

    }
    else return;
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
    MoveFirst();
}


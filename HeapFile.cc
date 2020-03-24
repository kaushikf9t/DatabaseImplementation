#include "HeapFile.h"

HeapFile::HeapFile() {

}

HeapFile::~HeapFile() {

}

int HeapFile::createMetaData(const char *fpath, fType file_type, void *startup) {
    cout << "In the Heap File method of create meta data" << endl;

    outfile->open(fpath, ios::out);

    if (!outfile) {
        cout << "Error Creating file" << endl;
    }

    char *fileType = "heap";
    outfile->write(fileType, sizeof(fileType));
    outfile->close();

}

int HeapFile::Create(const char *f_path, const char *metDataPath, fType f_type, void *startup) {
    cout << "In the Heap File method of create" << endl;
    //Create Metadata file first
    createMetaData(metDataPath, f_type, startup);

    this->file->Open(0, (char *) f_path);
    writePage = 0;
    cout << "Get Write Page count " << writePage << '\n';
    return 1;
}

int HeapFile::Open(const char *f_path, const char *metadataPath) {
    file->Open(1, (char *) f_path);

    return 1;
}

void HeapFile::Add(Record &rec) {
    if (page->Append(&rec) == 0) {
        this->flushPage();
        page->Append(&rec);
    }

    needFlush = true;
}

void HeapFile::MoveFirst() {
    this->flushPageIfNeeded();
    readPage = 0;
    //page->EmptyItOut();
    file->GetPage(page, readPage++);

}

int HeapFile::GetNext(Record &fetchme) {
    this->flushPageIfNeeded();

    if (page->GetFirst(&fetchme) == 1) return 1;
    if (readPage < writePage) {
        file->GetPage(page, readPage++);
    }

    return page->GetFirst(&fetchme);
}

void HeapFile::flushPage() {
    cout<<"In super-class method of flushPage"<<endl;
    file->AddPage(page, writePage++);
    page->EmptyItOut();
    needFlush = false;
}

int HeapFile::GetNext(Record &fetchme, CNF &cnf, Record &literal) {

    this->flushPageIfNeeded();

    int foundFilteredValue = 0;

    while ((foundFilteredValue = page->GetFirst(&fetchme)) == 1 || readPage < writePage) {
        if (foundFilteredValue == 1) {
            if (comp.Compare(&fetchme, &literal, &cnf)) {
                return 1;
            }
        } else {
            file->GetPage(page, readPage++);
        }
    }

    return 0;
}
    

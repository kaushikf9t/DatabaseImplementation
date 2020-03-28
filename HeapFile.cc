#include "HeapFile.h"

HeapFile::HeapFile() = default;

HeapFile::~HeapFile() = default;

int HeapFile::createMetaData(const char *fpath, fType file_type, void *startup) {
    cout << "In the Heap File method of create meta data" << endl;
    outfile = new ofstream();
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
    page->EmptyItOut();

}

int HeapFile::GetNext(Record &fetchme) {
    this->flushPageIfNeeded();

    if (page->GetFirst(&fetchme) == 1) return 1;

    if (readPage < file->GetLength() -1) {
        file->GetPage(page, readPage++);
    }

    return page->GetFirst(&fetchme);
}

void HeapFile::flushPage() {
    file->AddPage(page, writePage++);
    page->EmptyItOut();
    needFlush = false;
}

int HeapFile::GetNext(Record &fetchme, CNF &cnf, Record &literal) {
    this->flushPageIfNeeded();
    int foundFilteredValue = 0;
    int count = 0;
    while ((foundFilteredValue = page->GetFirst(&fetchme)) == 1 || readPage < writePage) {
        if (foundFilteredValue == 1) {
            if (comp.Compare(&fetchme, &literal, &cnf)) {
                cout<<++count;
                return 1;
            }
        } else {
            file->GetPage(page, readPage++);
        }
    }

    return 0;
}
    

#include "DBFile.h"

DBFile::DBFile() {

}

DBFile::~DBFile() {
    delete myInternalFile;
}


int DBFile::Create(const char *fpath, const char *metDatapath, fType file_type, void *startup) {
    if(file_type == heap){
        myInternalFile = new HeapFile();

    } else if(file_type == sorted){

        myInternalFile = new SortedFile();

    }
    return myInternalFile->Create(fpath, metDatapath, file_type, startup);
    // } else {
    //     //Placeholder for B+ tree
    // }

}

int DBFile::Open(const char *fpath, const char *metadataPath) {
    string fileType = getFileType(metadataPath);

    if(fileType.compare("heap")){
        myInternalFile = new HeapFile();
    } else if(fileType.compare("sorted")){
        myInternalFile = new SortedFile();
    }

    return myInternalFile->Open(fpath, metadataPath);
}

int DBFile::Close() {
    return myInternalFile->Close();
}

void DBFile::Load(Schema &myschema, const char *loadpath) {
    myInternalFile->Load(myschema, loadpath);
}

void DBFile::MoveFirst() {
    myInternalFile->MoveFirst();
}

void DBFile::Add(Record &addme) {
    myInternalFile->Add(addme);
}

int DBFile::GetNext(Record &fetchme) {
    return myInternalFile->GetNext(fetchme);
}

int DBFile::GetNext(Record &fetchme, CNF &cnf, Record &literal) {
    return myInternalFile->GetNext(fetchme, cnf, literal);
}

basic_string<char> DBFile::getFileType(const char *metaDataPath) {
    string dbFileType;
    ifstream mdfile(metaDataPath);
    string line;
    if(mdfile.is_open())
        getline(mdfile, line);

    dbFileType = line;
    return dbFileType;
}








#include "SortedFile.h"

SortedFile::SortedFile() = default;

SortedFile::~SortedFile() = default;

int SortedFile::createMetaData(const char *fpath, fType file_type, void *startup) {
    cout << "In the Sorted File method of create meta data" << endl;
    SortInfo *ptr;
    ptr = (SortInfo *) startup;
    char *fileType = "sorted";


    metadata = new DBFileMetaData((*ptr).runLength, (*ptr).myOrder, fileType);

    OrderMaker *myorder = metadata->getOrderMaker();
    ofstream mdfile(fpath);

    if (mdfile.is_open()) {
        mdfile << metadata->getFileType() << "\n";
        mdfile << metadata->getRunLength() << "\n";
        mdfile << *myorder << "\n";

    } else {
        cout << "Unable to open the file for write" << endl;
    }

    mdfile.close();
}

int SortedFile::Create(const char *f_path, const char *metDataPath, fType f_type, void *startup) {
    cout << "In the Sorted File method of create" << endl;
    createMetaData(metDataPath, f_type, startup);
    file->Open(0, (char *) f_path);
    writePage = 0;
    cout << "Get Write Page count " << writePage << '\n';

}

int SortedFile::Open(const char *f_path, const char *metadataPath) {
    cerr << "In the Open() method GenericDBFile " << endl;
    ifstream mdfile(metadataPath);
    string line;

    OrderMaker *orderMaker = new OrderMaker();

    if (mdfile.is_open()) {
        metadata = new DBFileMetaData();
        //Set the file type
        getline(mdfile, line);
        metadata->setFileType(line);
        //Set the run length
        getline(mdfile, line);
        metadata->setRunLength(stoi(line));

        //Set OrderMaker
        mdfile >> *orderMaker;
        metadata->setOrderMaker(orderMaker);

        metadata->getOrderMaker()->Print();
    }

    file->Open(1, (char *) f_path);
    writePage = file->GetLength() - 1;
    return 1;
}

void SortedFile::initBigQ() {

    OrderMaker *myorder = metadata->getOrderMaker();
    in = new Pipe(100);
    out = new Pipe(100);
    bigQ = new BigQ(*in, *out, *myorder, metadata->getRunLength());
    isBigQCreated = true;

}

//Should have initiated the BigQ by this time
void SortedFile::Add(Record &addme) {
    if (!isBigQCreated)
        initBigQ();

    in->Insert(&addme);
    needFlush = true;
}

//This merges the in-pipe records when the write operation is preempted by some reads
void SortedFile::flushPage() {
    //Shut down the input pipe
    in->ShutDown();
    //Get the records from the output file
    Record *outPipeTempRecord = new Record();
    Record *dbFileTempRecord = new Record();

    //The file and the page instance of the DB File
    bool didDBRecordGoThrough = false;

    //The file and the page instance of the merged DB File
    Page *mergedPage = new Page();
    File *mergedFile = new File();
    char tempPath[100];
    sprintf(tempPath, "%s.tmp", "temp");
    mergedFile->Open(0, (char *) tempPath);
    int mergedPagesCount = 0;

    bool onlyOutPipeAvailable = true;

    for (int whichPage = 0; whichPage < file->GetLength() - 1; whichPage++) {
        onlyOutPipeAvailable = false;
        file->GetPage(page, whichPage);
        page->GetFirst(dbFileTempRecord);
        //compare page -> GetFirst()
        while (didDBRecordGoThrough || out->Remove(outPipeTempRecord)) {
            //Fetch the ordermaker and compare these two records
            //If the current page has the record
            int result = comp.Compare(outPipeTempRecord, dbFileTempRecord, metadata->getOrderMaker());
            if (result < 1) {
                if (mergedPage->Append(outPipeTempRecord) == 0) {
                    mergedFile->AddPage(mergedPage, mergedPagesCount++);
                    mergedPage->Append(outPipeTempRecord);
                }

                didDBRecordGoThrough = false;
                outPipeTempRecord = new Record();
            } else {
                //Advance temp 2
                //TO-DO Move this code to a method
                if (mergedPage->Append(dbFileTempRecord) == 0) {
                    mergedFile->AddPage(mergedPage, mergedPagesCount++);
                    mergedPage->EmptyItOut();
                    mergedPage->Append(dbFileTempRecord);
                }
                didDBRecordGoThrough = true;
                if (page->GetFirst(dbFileTempRecord) == 0) {
                    break;
                }
            }
        }

        if (!didDBRecordGoThrough) {
            if (mergedPage->Append(dbFileTempRecord) == 0) {
                mergedFile->AddPage(mergedPage, mergedPagesCount++);
                mergedPage->EmptyItOut();
                mergedPage->Append(dbFileTempRecord);
            }
        }
    }

    onlyOutPipeAvailable = true;
    int numberOfRecords = 0;
if ((onlyOutPipeAvailable && out->Remove(outPipeTempRecord)) || didDBRecordGoThrough) {
        do {
            if (mergedPage->Append(outPipeTempRecord) == 0) {
                mergedFile->AddPage(mergedPage, mergedPagesCount++);
                mergedPage->EmptyItOut();
                mergedPage->Append(outPipeTempRecord);
            }
            numberOfRecords++;
        } while (out->Remove(outPipeTempRecord));
    }


    cout<< "Number of records : " << numberOfRecords << '\n';

    mergedFile->AddPage(mergedPage, mergedPagesCount++);

    for (int page = 0; page < mergedFile->GetLength() - 1; page++) {
        mergedFile->GetPage(mergedPage, page);
        file->AddPage(mergedPage, page);
    }
    cout << "Remove the temp file now : " << tempPath << endl;
    remove(tempPath);
}

void SortedFile::MoveFirst() {
    if (needFlush) {
        this->flushPage();
    }

    needFlush = false;
    readPage = 0;
    page->EmptyItOut();
    file->GetPage(page, readPage++);

}

int SortedFile::GetNext(Record &fetchme) {
    if (needFlush) {
        this->flushPage();
    }

    needFlush = false;
    if (page->GetFirst(&fetchme) == 1) return 1;
    if (readPage < file->GetLength() - 1) {
        file->GetPage(page, readPage++);
    }

    return page->GetFirst(&fetchme);
}

int SortedFile::GetNext(Record &fetchme, CNF &cnf, Record &literal){
    if (needFlush) {
        cout << "Flushing while switching from writes to read\n";
        flushPage();
    }

    OrderMaker *sortOrder = this -> metadata -> getOrderMaker();
    OrderMaker queryOrder;
    queryOrder.numAtts = 0;

    ComparisonEngine::buildQueryOrderMaker(sortOrder, &queryOrder, &cnf);
    if(queryOrder.numAtts == 0){
        //do a Linear Search similar to heap file

    } else {
        comp.Compare(&fetchme, &literal, &queryOrder);

    }

}



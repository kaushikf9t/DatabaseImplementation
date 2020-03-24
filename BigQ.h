#ifndef BIGQ_H
#define BIGQ_H

#include <pthread.h>
#include <sstream>
#include <iostream>
#include "Pipe.h"
#include "File.h"
#include "Record.h"
#include <vector>

using namespace std;

class BigQ {

private:
public:
    BigQ(Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen);

    ~BigQ();
};

/**
 * Structure which we pass for TPMMS into the worker thread of BigQ
 */
struct thread_data {
    thread_data(Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen) : in(in), out(out), sortorder(sortorder),
                                                                          runlen(runlen) {
        this->in = in;
        this->out = out;
        this->sortorder = sortorder;
        this->runlen = runlen;
    }

    Pipe &in;
    Pipe &out;
    OrderMaker &sortorder;
    int runlen;
};

/**
 * Entry method for the thread generated in BigQ constructors
 * @param data Contains thread_data struct data
 */
void *TPMMS(void *data);

/**
 * Initializes and Open the file for the TPMMS
 */
std::string Init(File *file);

/**
 * It consumes the records passed through in pipe and complets phase 1 of TPMMS which is converting records
 * into different sorted runs of each runlen which can be merged later.
 */
void Phase1(File *file, Pipe &in, OrderMaker &sortorder, int runlen);

/**
 * It uses a priority queue, which points to top of each run and move the top record into the pipe
 * according to sortorder.
 */
void Phase2(File *file, Pipe &out, OrderMaker &sortorder, int runlen);

void Finish(File *file, std::string fileName, Pipe &out);

/**
 * Once we have runlen pages in Phase1, we sort the run and write it into the file.
 * This method sorts the single run and holds the overflown record for next run.
 */
int SortSingleRunData(File *file, vector<Page *> &pages, OrderMaker *sortorder, int runlen, int writePage);

class RecordWrapper;

/**
 * It is wrapper for maintaining run for the phase 2 priority queue.
 */
class SingleRun {
private:
    int startPage;
    int endPage;

    Page *page = new Page();
    File *file;
public:
    SingleRun(File *file, int startPage, int endPage);

    ~SingleRun();

    int GetFirst(RecordWrapper *firstOne);
};

/**
 * It wraps the record and have a pointer to the run which will be used to fetch next records if this record
 * is pushed into output pipe.
 */
class RecordWrapper {
public:
    Record *firstOne = new Record();
    int runArrayIndex;

    RecordWrapper(int runArrayIndex) {
        this->runArrayIndex = runArrayIndex;
    }
};

/**
 * Custom comparator for priority queue which compares records based on SortOrder
 */
struct CustomRecordCompare {
    OrderMaker *orderMaker;
    ComparisonEngine comp;

    CustomRecordCompare(OrderMaker *orderMaker) {
        this->orderMaker = orderMaker;
    }

    bool operator()(Record *lhs, Record *rhs) {
        return comp.Compare(lhs, rhs, this->orderMaker) > 0;
    }

    bool operator()(RecordWrapper *lhs, RecordWrapper *rhs) {
        return comp.Compare(lhs->firstOne, rhs->firstOne, this->orderMaker) > 0;
    }
};

static std::string randomFileName() {
    time_t result = time(nullptr);
    std::stringstream ss;
    ss << result;
    return "tmp" + ss.str() + ".bin";
}

#endif

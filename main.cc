
#include <iostream>
#include "Record.h"
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <vector>
//#include <gtest/gtest.h>

using namespace std;

extern "C" {
int yyparse(void);   // defined in y.tab.c
}

extern struct AndList *final;

bool fileExists();

const bool runningOnServer(fileExists());

// ****************************************************************** //
// Code to check distinguish to pick files from CISE server or local machine.
// It can be remove if needed depending on how the tests will be running.

const std::string g_serverPath("/cise/homes/msodhani/mock_data/");
const std::string g_localPath("/Users/madhavsodhani/projects/cpp/mock_data/");


int sizeOfFilePath(const std::string &tableName) {
    return (runningOnServer ? g_serverPath : g_localPath).size() + tableName.size();
}

char *getFilePath(const std::string &tableName, char *cstr) {
    std::string filePath((runningOnServer ? g_serverPath : g_localPath) + tableName);
    strcpy(cstr, filePath.c_str());
    return cstr;
}

// ****************************************************************** //


// ****************************************************************** //


void flushDataIntoFile();

void readDataFromFile();

int main() {
//    flushDataIntoFile();
    readDataFromFile();
    return 0;
}

void readDataFromFile() {
    Record temp;
    Schema mySchema("catalog", "nation");
    File file;
    file.Open(1, "tmp1581932790.bin");

    cout << "File length is : " << file.GetLength() << '\n';
    Page page;

    int total = 0;
    for (int whichPage = 0; whichPage < file.GetLength() - 1; whichPage++) {
        int numberOfRecords = 0;
        file.GetPage(&page, whichPage);
        while (page.GetFirst(&temp) == 1) {
            numberOfRecords++;
            total++;
            temp.Print(&mySchema);
        }
    }

    file.Close();
}

void flushDataIntoFile() {
    char cstr[sizeOfFilePath("nation.tbl") + 1];
    FILE *tableFile = fopen(getFilePath("nation.tbl", cstr), "r");

    Record temp;
    Schema mySchema("catalog", "nation");

    int counter = 0;
    int whichPage = 0;
    File file;
    file.Open(0, "/Users/madhavsodhani/projects/cpp/mock_data/testing");
    Page page;
    bool flushedAtEnd = false;
    int numberOfRecordsInThePage = 0;

    while (temp.SuckNextRecord(&mySchema, tableFile) == 1) {
        flushedAtEnd = false;
        counter++;
        if (counter % 10000 == 0) {
            cerr << counter << "\n";
        }

        if (page.Append(&temp) == 0) {
            flushedAtEnd = true;
            file.AddPage(&page, whichPage++);
            page.EmptyItOut();
            numberOfRecordsInThePage = 0;
            page.Append(&temp);
        }
        numberOfRecordsInThePage++;
    }

    if (!flushedAtEnd) {
        file.AddPage(&page, file.GetLength());
        page.EmptyItOut();
    }

    file.Close();

    cout << "Number of pages  : " << file.GetLength() << '\n';
    cout << "Total number of records are  : " << counter << '\n';
}

// ****************************************************************** //

int main2() {
    // try to parse the CNF
    cout << "Enter in your CNF: ";
    if (yyparse() != 0) {
        cout << "Can't parse your CNF.\n";
        exit(1);
    }

    // suck up the schema from the file
    Schema lineitem("catalog", "nation");

    // grow the CNF expression from the parse tree
    CNF myComparison;
    Record literal;
    myComparison.GrowFromParseTree(final, &lineitem, literal);

    // print out the comparison to the screen
    myComparison.Print();

    // now open up the text file and start processing it
    char cstr[sizeOfFilePath("orders.tbl") + 1];
    //FILE *tableFile = fopen(getFilePath(tablename + ".tbl", cstr), "r");
    FILE *tableFile = fopen("/home/kaushik/Desktop/tpch/tpch-dbgen/lineitem.tbl", "r");

    Record temp;
    Schema mySchema("catalog", "nation");

    //char *bits = literal.GetBits ();
    //cout << " numbytes in rec " << ((int *) bits)[0] << endl;
    //literal.Print (&supplier);

    int counter = 0;
    int numberOfRecordsForTheQuery = 0;
    // read in all of the records from the text file and see if they match
    // the CNF expression that was typed in
    ComparisonEngine comp;
    while (temp.SuckNextRecord(&mySchema, tableFile) == 1) {
        counter++;
        if (counter % 10000 == 0) {
            cerr << counter << "\n";
        }

        if (comp.Compare(&temp, &literal, &myComparison)) {
            temp.Print(&mySchema);
            numberOfRecordsForTheQuery++;
        }
    }

    cout << "Total number of records are  : " << counter << '\n';
    cout << "Number of records in the query are : " << numberOfRecordsForTheQuery << '\n';
    return 0;
}

bool fileExists() {
    std::string fileName = "/cise/homes/msodhani/server.ignore";
    ifstream f(fileName.c_str());
    return f.good();
}


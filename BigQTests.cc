#include "gtest/gtest.h"
#include "BigQ.h"
#include "File.h"
#include "DBFile.h"
#include "Pipe.h"
#include <fstream>
#include <iostream>
#include <cstring>


char *catalog_path = "catalog";
char *nation_bin_file_name = "nation.bin";
char *tpch_dir = "/Users/madhavsodhani/projects/cpp/mock_data/";

bool fileExists(std::string fileName) {
    ifstream f(fileName.c_str());
    return f.good();
}


// Checks successful generation of the file and initialization steps BigQ
TEST(BigQTests, BigQInit) {
// Setup
File *file = new File();

// Act
std::string fileName = Init(file);


// Assert
EXPECT_EQ(fileExists(fileName), true);

// Cleanup
char cstr[fileName.size() + 1];
strcpy(cstr, fileName.c_str());
remove(cstr);
}


// Checks successful deletion of the file and shutdown of output pipe
TEST(BigQTests, BigQFinish) {
// Setup
File *file = new File();
Pipe output (100);
std::string fileName = Init(file);

// Act
Finish(file, fileName, output);

// Assert
EXPECT_EQ(fileExists(fileName), false);
EXPECT_EQ(output.isDone(), true);
}


// It tests the number of records present from a single run, which should be equal to number of records inserted
TEST(BigQTests, RunExtractionTest) {
// Setup
DBFile dbFile;
dbFile.Create(nation_bin_file_name, heap, NULL);
Schema *rschema = new Schema(catalog_path, "nation");
char tbl_path[100];
sprintf(tbl_path, "%s%s.tbl", tpch_dir, "nation");
dbFile.Load(*rschema, tbl_path);

Pipe output (100);

Page* page = new Page();
int numberOfRecordsAdded = 0;
Record temp;
while(dbFile.GetNext(temp)) {
numberOfRecordsAdded++;
page->Append(&temp);
}

vector<Page *> pages;
pages.push_back(page);

File *file = new File();
std::string fileName = Init(file);
OrderMaker *orderMaker = new OrderMaker(rschema);


SortSingleRunData(file, pages, orderMaker, 10, 0);

SingleRun *run = new SingleRun(file, 0, 0);

// Act
RecordWrapper *wrapper = new RecordWrapper(0);
int numberOfRecordsFound = 0;
while (run->GetFirst(wrapper)) {
numberOfRecordsFound++;
}

// Assert
EXPECT_EQ(numberOfRecordsAdded, numberOfRecordsFound);

// Cleanup
Finish(file, fileName, output);
}

// It tests the sorting of the records in the single run and writing the run into the file
TEST(BigQTests, BigQSortSingleRun) {
// Setup
DBFile dbFile;
dbFile.Create(nation_bin_file_name, heap, NULL);
Schema *rschema = new Schema(catalog_path, "nation");
char tbl_path[100];
sprintf(tbl_path, "%s%s.tbl", tpch_dir, "nation");
dbFile.Load(*rschema, tbl_path);

Pipe output (100);

Page* page = new Page();
int numberOfRecordsAdded = 0;
Record temp;
while(dbFile.GetNext(temp)) {
    numberOfRecordsAdded++;
    page->Append(&temp);
}

vector<Page *> pages;
pages.push_back(page);

File *file = new File();
std::string fileName = Init(file);
OrderMaker *orderMaker = new OrderMaker(rschema);

// Act
SortSingleRunData(file, pages, orderMaker, 10, 0);

Page newPage;
file->GetPage(&newPage, 0);

int numberOfRecordsFound = 0;
while (newPage.GetFirst(&temp)) {
    numberOfRecordsFound++;
}

// Assert
EXPECT_EQ(numberOfRecordsAdded, numberOfRecordsFound);

// Cleanup
Finish(file, fileName, output);
}



int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

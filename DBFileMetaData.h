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


class DBFileMetaData{
    private : 
        string fileType;
        int runLength;
        OrderMaker *om;
    
    public: 
        DBFileMetaData();
        ~DBFileMetaData();

        DBFileMetaData (int runLength, OrderMaker *o, string fileType);

        string getFileType(){
            return fileType;
        }

        int getRunLength(){
            return runLength;
        }

        void setRunLength(int runLen){
            this->runLength = runLen;
        }

        void setOrderMaker(OrderMaker *o){
            om = o;
        }

        OrderMaker* getOrderMaker(){
            return om;
        }

        void setFileType(string ft){
            this -> fileType = ft;
        }
};
#include "DBFileMetaData.h"

DBFileMetaData::DBFileMetaData(){}
DBFileMetaData::DBFileMetaData (int runLength, OrderMaker *o, string fileType) {
    setRunLength(runLength);
    setOrderMaker(o);
    setFileType(fileType);
   
}

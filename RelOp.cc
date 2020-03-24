#include "RelOp.h"
/**
 * Select File Operation
 * @param data
 * @return
 */
void *SelectFile::executeOp(void* data){
    InternalData *internalData = (InternalData *) data;
    internalData->inFile->MoveFirst();
    Record *temp = new Record();
    while(internalData->inFile->GetNext(*temp, *internalData->selOp, *internalData->literal)){
        internalData->outPipe->Insert(temp);
        temp = new Record();
    }

    internalData->outPipe->ShutDown();
    return nullptr;
}

/**
 * Select Pipe Operation
 * @param inPipe
 * @param outPipe
 * @param selOp
 * @param literal
 */

void *SelectPipe::executeOp(void *data) {
    Record *temp = new Record();
    InternalData *internalData = (InternalData *) data;
    ComparisonEngine *comp = new ComparisonEngine();
    while(internalData->inPipe->Remove(temp)){
        if(comp->Compare(temp, internalData->literal, internalData->selOp) == 0){
            //Do nothing
        } else {
            internalData->outPipe->Insert(temp);
        }
        temp = new Record();
    }
    internalData->outPipe->ShutDown();
    return nullptr;
}

/**
 * Project Operation
 * @param inPipe
 * @param outPipe
 * @param keepMe
 * @param numAttsInput
 * @param numAttsOutput
 */

void *Project::executeOp(void *data) {
    InternalData *internalData = (InternalData *) data;
    Record *temp = new Record();
    while(internalData->inPipe->Remove(temp)){
        temp->Project(internalData->keepMe, internalData->numAttsOutput, internalData->numAttsInput);
        internalData->outPipe->Insert(temp);
        temp = new Record();
    }
    internalData->outPipe->ShutDown();
    return nullptr;
}


void *Sum::executeOp(void *data) {
    InternalData *internalData = (InternalData *) data;
    Record *rec = new Record();
    int intResult = 0;
    double dblResult = 0.0;

    double finalResult;
    Type finalType = Int;
    while(internalData -> inPipe->Remove(rec)){
        Type resultType = internalData->computeMe->Apply(*rec, intResult, dblResult);
        if(resultType == Double){
            finalType = Double;
            finalResult += dblResult;

        } else{
            finalResult += intResult;
        }
        rec = new Record();
    }

    Attribute att = {"att1", finalType};
    //Append the final result and a pipe to the file
    Schema *resultSchema = new Schema("sum_result", 1, &att);
    stringstream sstream;
    //Check this out
    std::string resStr = finalType == Double ? std::to_string(finalResult) : std::to_string((int)finalResult);
    sstream<<resStr<<"|";
    Record *finalRec = new Record();
    finalRec->ComposeRecord(resultSchema, sstream.str().c_str());
    internalData ->outPipe->Insert(finalRec);
    internalData->outPipe->ShutDown();

    return nullptr;
}

void *GroupBy::executeOp(void *data) {
    ComparisonEngine *comp = new ComparisonEngine();
    InternalData *internalData = (InternalData *) data;

    //First sort the records providing the ordermaker using the BigQ
    Pipe sortedPipe(100);
    BigQ bigQ(*internalData->inPipe, sortedPipe, *internalData->groupAtts, internalData->runLen);

    OrderMaker *groupAtts = internalData->groupAtts;
    Attribute att;
    Type type;
    att.name = (char *)"sum";
    att.myType = type;

    int *mergedAtts = new int[internalData->groupAtts->numAtts+1];
    mergedAtts[0] = 0;

    for(int i = 1; i < internalData->groupAtts->numAtts + 1; i++)
        mergedAtts[i] = internalData->groupAtts->whichAtts[i-1];

    Schema *resultSchema = new Schema("sum", 1, &att);
    //Fetch the first record from the output pipe

    Record *firstRecord = new Record();
    if(sortedPipe.Remove(firstRecord) == 0){
        internalData->outPipe->ShutDown();
        return nullptr;
    }

    //Group accumulator
    double GroupResult = 0.0;
    Type groupType = Int;

    //Per-record result
    int intRecordResult = 0;
    double dblRecordResult = 0.0;

    Record *nextRecord = new Record();
    while(internalData->outPipe->Remove(nextRecord)){
        int comparison = comp->Compare(firstRecord, nextRecord, internalData->groupAtts);
        Type resultType = internalData->computeMe->Apply(*firstRecord, intRecordResult, dblRecordResult);

        if(resultType == Double){
            groupType = Double;
        }

        if(comparison == 0){
            //Keep accumulating as long as the next record belongs to the same group
            GroupResult += intRecordResult;
            GroupResult += dblRecordResult;

        } else {
            //New grouping - write the existing group and reset the file
            writeRecordToFile(internalData, mergedAtts, resultSchema, firstRecord, GroupResult, groupType);
            GroupResult = 0.0;
        }

        firstRecord->Consume(nextRecord);
        intRecordResult = 0;
        dblRecordResult = 0.0;
    }

    //process the last record

    Type resultType = internalData->computeMe->Apply(*firstRecord, intRecordResult, dblRecordResult);

    GroupResult += intRecordResult;
    GroupResult += dblRecordResult;
    writeRecordToFile(internalData, mergedAtts, resultSchema, firstRecord, GroupResult, groupType);
    internalData->outPipe->ShutDown();

    return nullptr;
}

void GroupBy::writeRecordToFile(InternalData *internalData, int *mergedAtts, Schema *resultSchema,
                                Record *firstRecord, double GroupResult, Type &groupType) {
    stringstream sstream;
    string resStr = groupType == Double ? to_string(GroupResult) : to_string((int)GroupResult);
    sstream<<resStr<<"|";
    Record *finalRec = new Record();
    finalRec->ComposeRecord(resultSchema, sstream.str().c_str());
    Record *mergedRecord = new Record();
    mergedRecord->MergeRecords(finalRec, firstRecord, 1, internalData->groupAtts->numAtts, mergedAtts, internalData->groupAtts->numAtts+1, 1);
    internalData->outPipe->Insert(mergedRecord);
}

void *WriteOut::executeOp(void *data) {
    InternalData *internalData = (InternalData *) data;
    Attribute *atts = internalData->mySchema->GetAtts();
    int n = internalData->mySchema->GetNumAtts();
    Record temp;
    int count = 1;
    while(internalData->inPipe->Remove(&temp)){
        fprintf(internalData->outFile, "%d: ", count++);
        char *bits = temp.bits;
        for (int i = 0; i < n; i++) {
            fprintf(internalData->outFile, "%s",atts[i].name);
            int pointer = ((int *) bits)[i + 1];
            fprintf(internalData->outFile, "[");
            if (atts[i].myType == Int) {
                int *myInt = (int *) &(bits[pointer]);
                fprintf(internalData->outFile, "%d",*myInt);
            } else if (atts[i].myType == Double) {
                double *myDouble = (double *) &(bits[pointer]);
                fprintf(internalData->outFile, "%f", *myDouble);
            } else if (atts[i].myType == String) {
                char *myString = (char *) &(bits[pointer]);
                fprintf(internalData->outFile, "%s", myString);
            }
            fprintf(internalData->outFile, "]");
            if (i != n - 1) {
                fprintf(internalData->outFile, ", ");
            }
        }
        fprintf(internalData->outFile, "\n");
    }
    return nullptr;
}

void *DuplicateRemoval::executeOp(void *data) {
    InternalData *internalData = (InternalData *) data;
    OrderMaker *o = new OrderMaker(internalData->mySchema);
    ComparisonEngine *comp = new ComparisonEngine();

    Pipe sortedPipe(100);
    BigQ bigQ(*internalData->inPipe, sortedPipe, *o, internalData->runLen);

    Record *firstRec = new Record();
    Record *nextRec = new Record();
    if(sortedPipe.Remove(firstRec) == 0){
        internalData->outPipe->ShutDown();
        return nullptr;
    }

    internalData->outPipe->Insert(firstRec);
    while(sortedPipe.Remove(nextRec)){
        if(comp->Compare(firstRec, nextRec, o) == 0){
            firstRec->Consume(nextRec);
            continue;
        } else {
            internalData->outPipe->Insert(nextRec);
            firstRec->Consume(nextRec);
        }
    }
    internalData->outPipe->ShutDown();

    return nullptr;
}


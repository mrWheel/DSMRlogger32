#ifndef FSYSSTUFF_H
#define FSYSSTUFF_H

//============ Includes ====================
#include "arduinoGlue.h"

//=========== Prototypes ==================
void fillRecord(char *record, int8_t maxLen);               
int32_t freeSpace();                                        
bool createRingFile(const char *fileName, timeStruct useTime, int8_t ringType);
bool createRingFile(const char *fileName, timeStruct useTime, int8_t ringType, uint16_t noSlots);
int sortListFiles(const void *cmp1, const void *cmp2);      

//============ Added by Convertor ==========
#endif // FSYSSTUFF_H

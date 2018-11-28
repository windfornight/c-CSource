#include "PEStruct.h"

#ifndef _BUFFER_OPERATION_H_
#define _BUFFER_OPERATION_H_

//以二进制的形式读取文件
LPVOID readPEFile(const char* inFile);

//打印一些相关的PE解构
void printPEStruct(const char* filePath);

//fileBuff->imageBuff
LPVOID extendToImageBuff(LPVOID fileBuff);

//imageBuff->fileBuff
LPVOID reduceToFileBuff(LPVOID imageBuff);

//以二进制形式写文件
void writePEFile(const char* outFile, LPVOID fileBuff);

#endif
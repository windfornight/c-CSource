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

//本地模式，加一个壳（messagebox）在程序运行前
void addShellCode(LPVOID fileBuff, int sectionIdx);

//RVA->FOA
DWORD convRVAtoFOA(LPVOID fileBuff, DWORD rva);

//打印目录
void printDirectory(LPVOID fileBuff);

//打印导出表
void printExportDirectory(LPVOID fileBuff);

//增加一个节
void addSection(const char* inFile, const char* outFile);

#endif
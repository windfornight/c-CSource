#include "PEStruct.h"

#ifndef _BUFFER_OPERATION_H_
#define _BUFFER_OPERATION_H_

//以二进制的形式读取文件
LPVOID readFile(const char* inFile);

bool isValidPEFile(LPVOID fileBuff);

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
LPVOID addSection(LPVOID pFileBuffer, int addSize);

//对齐的大小
DWORD getAlignSize(DWORD size, DWORD alignSize);

//遍历获取pe文件的大小(可能会和真实的大小有出入)
size_t getPEFileSize(LPVOID pFileBuffer);

void dataCopy(LPVOID des, LPVOID src, size_t size);

//移动NT头文件到DOS头之后
void moveNTHead(LPVOID pFileBuffer);

//合并所有的节成为一个节
LPVOID mergeAllSections(LPVOID pFileBuffer);

//扩大最后一个节
LPVOID extendLastSection(LPVOID pFileBuffer, size_t size);

//失败，修改改用dtdebug查查看
void addShellCodeTest(const char* inFile, const char* outFile);

//根据导出表查找函数地址，返回的是RVA
DWORD getFuncAddrByName(LPVOID pFileBuffer, const char* funcName);

//根据导出表查找函数地址， 返回的是RVA
DWORD getFuncAddrByOrdinal(LPVOID pFileBuffer, int ordinal);

//打印重定位表
void printRelocationDiretory(LPVOID pFileBuffer);

//修改imagebase
void changeImageBase(LPVOID pFileBuffer, DWORD newBase);

//移动导出表到新加的节
LPVOID moveExpDirToNewSec(LPVOID pFileBuffer);

DWORD calSizeForMoveExpDir(LPVOID pFileBuffer);

//移动重定位表到新加的节
LPVOID moveRelocDieToNewSec(LPVOID pFileBuffer);

DWORD calSizeForMoveRelocDir(LPVOID pFileBuffer);

//打印导入表
void printImportDescriptor(LPVOID pFileBuffer);

//是否结束标志
bool isEndImportDescriptor(PIMAGE_IMPORT_DESCRIPTOR pImageImportDescriptor);

void printImportFuncInfo(LPVOID pFileBuffer, PIMAGE_THUNK_DATA32 pImageThunkData);

#endif
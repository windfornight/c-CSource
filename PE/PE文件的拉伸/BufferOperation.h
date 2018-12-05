#include "PEStruct.h"

#ifndef _BUFFER_OPERATION_H_
#define _BUFFER_OPERATION_H_

//�Զ����Ƶ���ʽ��ȡ�ļ�
LPVOID readPEFile(const char* inFile);

//��ӡһЩ��ص�PE�⹹
void printPEStruct(const char* filePath);

//fileBuff->imageBuff
LPVOID extendToImageBuff(LPVOID fileBuff);

//imageBuff->fileBuff
LPVOID reduceToFileBuff(LPVOID imageBuff);

//�Զ�������ʽд�ļ�
void writePEFile(const char* outFile, LPVOID fileBuff);

//����ģʽ����һ���ǣ�messagebox���ڳ�������ǰ
void addShellCode(LPVOID fileBuff, int sectionIdx);

//RVA->FOA
DWORD convRVAtoFOA(LPVOID fileBuff, DWORD rva);

//��ӡĿ¼
void printDirectory(LPVOID fileBuff);

//��ӡ������
void printExportDirectory(LPVOID fileBuff);

//����һ����
void addSection(const char* inFile, const char* outFile);

#endif
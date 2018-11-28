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

#endif
#include "PEStruct.h"

#ifndef _BUFFER_OPERATION_H_
#define _BUFFER_OPERATION_H_

//�Զ����Ƶ���ʽ��ȡ�ļ�
LPVOID readFile(const char* inFile);

bool isValidPEFile(LPVOID fileBuff);

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
LPVOID addSection(LPVOID pFileBuffer, int addSize);

//����Ĵ�С
DWORD getAlignSize(DWORD size, DWORD alignSize);

//������ȡpe�ļ��Ĵ�С(���ܻ����ʵ�Ĵ�С�г���)
size_t getPEFileSize(LPVOID pFileBuffer);

void dataCopy(LPVOID des, LPVOID src, size_t size);

//�ƶ�NTͷ�ļ���DOSͷ֮��
void moveNTHead(LPVOID pFileBuffer);

//�ϲ����еĽڳ�Ϊһ����
LPVOID mergeAllSections(LPVOID pFileBuffer);

//�������һ����
LPVOID extendLastSection(LPVOID pFileBuffer, size_t size);

//ʧ�ܣ��޸ĸ���dtdebug��鿴
void addShellCodeTest(const char* inFile, const char* outFile);

//���ݵ�������Һ�����ַ�����ص���RVA
DWORD getFuncAddrByName(LPVOID pFileBuffer, const char* funcName);

//���ݵ�������Һ�����ַ�� ���ص���RVA
DWORD getFuncAddrByOrdinal(LPVOID pFileBuffer, int ordinal);

//��ӡ�ض�λ��
void printRelocationDiretory(LPVOID pFileBuffer);

//�޸�imagebase
void changeImageBase(LPVOID pFileBuffer, DWORD newBase);

//�ƶ��������¼ӵĽ�
LPVOID moveExpDirToNewSec(LPVOID pFileBuffer);

DWORD calSizeForMoveExpDir(LPVOID pFileBuffer);

//�ƶ��ض�λ���¼ӵĽ�
LPVOID moveRelocDieToNewSec(LPVOID pFileBuffer);

DWORD calSizeForMoveRelocDir(LPVOID pFileBuffer);

//��ӡ�����
void printImportDescriptor(LPVOID pFileBuffer);

//�Ƿ������־
bool isEndImportDescriptor(PIMAGE_IMPORT_DESCRIPTOR pImageImportDescriptor);

void printImportFuncInfo(LPVOID pFileBuffer, PIMAGE_THUNK_DATA32 pImageThunkData);

#endif
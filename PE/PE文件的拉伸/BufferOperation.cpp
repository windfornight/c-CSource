#include "stdafx.h"
//#include "stdio.h"
#include "malloc.h"
#include "string.h"

#include "BufferOperation.h"

LPVOID readPEFile(const char* inFile)
{
	FILE* file = fopen(inFile, "rb");
	if(!file){
		printf("file open error!");
		return NULL;
	}
	fseek(file, 0, SEEK_END); //文件指针移动到末尾，正常返回0
	long int cnt = ftell(file); //字节个数
	LPVOID ptBuff = malloc(cnt);
	if(!ptBuff){
		printf("assign buff error!");
		fclose(file);
		file = NULL;
		return NULL;
	}
	fseek(file, 0, SEEK_SET);

	//一次性读取
	size_t n = fread(ptBuff, cnt, 1, file);	
	if(!n)	
	{	
		printf("Fail to read file! ");
		free(ptBuff);
		fclose(file);
		return NULL;
	}	

	fclose(file);
	file = NULL;

	return ptBuff;
}

void printPEStruct(const char* filePath)
{
	LPVOID pFileBuffer = NULL;
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;

	pFileBuffer = readPEFile(filePath);
	if(!pFileBuffer)
	{
		printf("Error to read file!\n");
		return ;
	}

	//判断是否有效的MZ标志
	if(*((PWORD)pFileBuffer) != IMAGE_DOS_SIGNATURE)
	{
		printf("invalid MZ signature!\n");
		free(pFileBuffer);
		return ;
	}

	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;

	//打印DOS头
	printf("*************DOS**************\n");
	printf("MZ signature:%x\n", pDosHeader->e_magic);
	printf("PE offset:%x\n", pDosHeader->e_lfanew);

	//判断是否有效的PE标志
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);  //char *
	if (pNTHeader->Signature != IMAGE_NT_SIGNATURE)
	{
		printf("invalid PE signature!\n");
		free(pFileBuffer);
		return;
	}

	//打印NT头
	printf("*****************NT*******************\n");
	printf("NT:%x\n", pNTHeader->Signature);

	//实际上，结构体对齐有可能会造成问题的，所以最好还是直接地址偏转（这里不会造成影响）
	pPEHeader = &(pNTHeader->FileHeader); //(PIMAGE_FILE_HEADER)((DWORD)pNTHeader + 4);
	printf("********************PE*****************\n");
	printf("PE: %x\n", pPEHeader->Machine);
	printf("number of section:%x\n", pPEHeader->NumberOfSections);
	printf("SizeOfOptionalHeader:%x\n", pPEHeader->SizeOfOptionalHeader);
	

	//可选PE头, 结构体对齐问题，这里没有问题
	pOptionHeader = /*&(pNTHeader->OptionalHeader);*/(PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader+IMAGE_SIZEOF_FILE_HEADER);
	printf("**************OPTION_PE***************\n");
	printf("OPTION_PE: %x\n", pOptionHeader->Magic);
	printf("sizeOfHeader:%x\n", pOptionHeader->SizeOfHeaders);
	printf("sizeOfImage:%x\n", pOptionHeader->SizeOfImage);

	//节表
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	for(int i = 0; i < pPEHeader->NumberOfSections; ++i)
	{
		char name[9] = {0};
		strcpy(name, (pSectionHeader+i)->Name1);
		printf("section Name: %s\n", name);

	}

	//释放内存
	free(pFileBuffer);
}

LPVOID extendToImageBuff(LPVOID pFileBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);;
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);;
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);


	size_t size = pOptionHeader->SizeOfImage;
	LPVOID pImageBuffer = malloc(size);
	if(!pImageBuffer){
		printf("assign buffer failed!\n");
		return NULL;
	}

	memset(pImageBuffer, 0, size);

	size_t headerSize = pOptionHeader->SizeOfHeaders;

	//所有头+节表的大小
	for(int i = 0; i < headerSize; ++i)
	{
		char ch = *((char *)pFileBuffer+i);
		*((char *)pImageBuffer + i) = ch;

	}

	int sectionCnt = pPEHeader->NumberOfSections;

	//拷贝每个节的数据
	for(int i = 0; i < sectionCnt; ++i)
	{
		PIMAGE_SECTION_HEADER curPSHeader = (pSectionHeader + i);
		char* pFSection = (char*)pFileBuffer + curPSHeader->PointerToRawData;
		char* pISection = (char*)pImageBuffer + curPSHeader->VirtualAddress;
		for(int j = 0; j < curPSHeader->SizeOfRawData; ++j)
		{
			char ch =  *(pFSection + j);
			*(pISection + j) = ch;
		}
	}

	return pImageBuffer;
}

LPVOID reduceToFileBuff(LPVOID pImageBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pImageBuffer + pDosHeader->e_lfanew);;
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);;
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	//size_t size = pOptionHeader->SizeOfImage; //一般会扩大内存
	size_t size = 0;
	int sectionCnt = pPEHeader->NumberOfSections;
	//遍历节表并且计算内存的大小（不一定完全还原怎么办？）
	for(int i = 0; i < sectionCnt; ++i)
	{
		PIMAGE_SECTION_HEADER curPSHeader = (pSectionHeader + i);
		if(size < curPSHeader->PointerToRawData + curPSHeader->SizeOfRawData)
		{
			size = curPSHeader->PointerToRawData + curPSHeader->SizeOfRawData;
		}
	}
	if(size % pOptionHeader->FileAlignment)
	{
		size = size / pOptionHeader->FileAlignment + pOptionHeader->FileAlignment;
	}

	LPVOID pFileBuffer = malloc(size);
	if(!pFileBuffer){
		printf("assign buffer failed!\n");
		return NULL;
	}

	memset(pFileBuffer, 0, size);

	size_t headerSize = pOptionHeader->SizeOfHeaders;
	//所有头+节表的大小
	for(int i = 0; i < headerSize; ++i)
	{
		char ch = *((char *)pImageBuffer+i);
		*((char *)pFileBuffer + i) = ch;

	}

	//拷贝每个节的数据
	for(int i = 0; i < sectionCnt; ++i)
	{
		PIMAGE_SECTION_HEADER curPSHeader = (pSectionHeader + i);
		char* pFSection = (char*)pFileBuffer + curPSHeader->PointerToRawData;
		char* pISection = (char*)pImageBuffer + curPSHeader->VirtualAddress;
		for(int j = 0; j < curPSHeader->SizeOfRawData; ++j)
		{
			char ch =  *(pISection + j);
			*(pFSection + j) = ch;
		}
	}

	return pFileBuffer;
}


void writePEFile(const char* outFile, LPVOID pFileBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);;
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);;
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	size_t size = 0;
	int sectionCnt = pPEHeader->NumberOfSections;
	//遍历并计算文件的大小
	for(int i = 0; i < sectionCnt; ++i)
	{
		PIMAGE_SECTION_HEADER curPSHeader = (pSectionHeader + i);
		if(size < curPSHeader->PointerToRawData + curPSHeader->SizeOfRawData)
		{
			size = curPSHeader->PointerToRawData + curPSHeader->SizeOfRawData;
		}
	}
	if(size % pOptionHeader->FileAlignment)
	{
		size = size / pOptionHeader->FileAlignment + pOptionHeader->FileAlignment;
	}


	FILE* fileWrite = fopen(outFile, "wb");
	
	int res = fwrite(pFileBuffer, size, 1, fileWrite);
	if(!res)
	{
		printf("write data failed\n");
		fclose(fileWrite);
		return ; 
	}
	
	fclose(fileWrite);
	fileWrite = NULL;
}






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


//是否一定要到节中去找数据？？？
DWORD convRVAtoFOA(LPVOID pFileBuffer, DWORD rva)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);;
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);;
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	DWORD foa = 0;

	for(int i = 0; i < pPEHeader->NumberOfSections; ++i)
	{
		PIMAGE_SECTION_HEADER curPSH = pSectionHeader + i;
		if(rva >= curPSH->VirtualAddress && (rva <= curPSH->VirtualAddress + curPSH->SizeOfRawData))
		{
			foa = curPSH->PointerToRawData +(rva - curPSH->VirtualAddress);
			break;
		}
	}

	return foa;
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
	printf("entry address:%x\n", pOptionHeader->AddressOfEntryPoint);
	printf("image base:%x\n", pOptionHeader->ImageBase);

	printf("*********************SECTION-INFO****************\n");
	//节表
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	for(int i = 0; i < pPEHeader->NumberOfSections; ++i)
	{
		char name[9] = {0};
		strncpy(name, (pSectionHeader+i)->Name1, 8);
		printf("section Name: %s, virtual size:%x, rawData size:%x \n", name, (pSectionHeader+i)->Misc.VirtualSize, (pSectionHeader+i)->SizeOfRawData);
		printf("virtual address:%x, pointer of rawData:%x\n", (pSectionHeader+i)->VirtualAddress, (pSectionHeader+i)->PointerToRawData);
		printf("\n");
	}

	//printDirectory(pFileBuffer);
	//printExportDirectory(pFileBuffer);
	
	
	//释放内存
	free(pFileBuffer);
}

void printDirectory(LPVOID pFileBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);;
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);;
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	//数据目录表
	printf("*******************IMAGE-DATA-DIRECTORY************************\n");
	IMAGE_DATA_DIRECTORY *pImageDataDirectory = pOptionHeader->DataDirectory;
	char* nameList[16] = {"导出表", "", "", "", "", 
		"重定位表", "", "", "", "",
		"", "", "", "", "",
		""};
	for(int i = 0; i < 16; ++i)
	{
		printf("Name_%02d:%s\n", i+1, nameList[i]);
		printf("virtual address:%x\n", (pImageDataDirectory+i)->VirtualAddress);
		printf("size:%x\n", (pImageDataDirectory+i)->isize);
	}
}

void printExportDirectory(LPVOID pFileBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);;
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);;
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	IMAGE_DATA_DIRECTORY *pImageDataDirectory = pOptionHeader->DataDirectory;

	//导出表
	DWORD foa = convRVAtoFOA(pFileBuffer, (pImageDataDirectory+0)->VirtualAddress);
	if(foa)
	{
		PIMAGE_EXPORT_DIRECTORY pImageExoprtDirectory = (PIMAGE_EXPORT_DIRECTORY)((DWORD)pFileBuffer+foa);
		printf("********************EXPORT-DIRECTORY******************\n");
		printf("name:%s\n", (char*)((DWORD)pFileBuffer + convRVAtoFOA(pFileBuffer, pImageExoprtDirectory->Name)));
		printf("base:%x\n", pImageExoprtDirectory->Base);
		printf("NumberOfFunctions:%x\n", pImageExoprtDirectory->NumberOfFunctions);
		printf("NumberOfNames:%x\n", pImageExoprtDirectory->NumberOfNames);
		DWORD* funcAddressArray = (DWORD *)((DWORD)pFileBuffer + convRVAtoFOA(pFileBuffer, pImageExoprtDirectory->AddressOfFunctions));

		//打印的是FOA
		printf("*********func address**********\n");
		for(int i = 0; i < pImageExoprtDirectory->NumberOfFunctions; ++i)
		{
			DWORD funcFOA = convRVAtoFOA(pFileBuffer, funcAddressArray[i]);
			printf("idx:%d, FOA:%x, RVA:%x\n", i, funcFOA, funcAddressArray[i]);
		}

		//打印函数名字
		DWORD* funcNameAddress = (DWORD*)((DWORD)pFileBuffer + convRVAtoFOA(pFileBuffer, pImageExoprtDirectory->AddressOfNames));
		printf("********func name**************\n");
		for(int i = 0; i < pImageExoprtDirectory->NumberOfNames; ++i)
		{
			DWORD funcFOA = convRVAtoFOA(pFileBuffer, funcNameAddress[i]);
			printf("idx:%d, func name:%s\n", i, (char *)((DWORD)pFileBuffer + funcFOA));

		}

		WORD* funcNameOrdAddress = (WORD*)((DWORD)pFileBuffer + convRVAtoFOA(pFileBuffer, pImageExoprtDirectory->AddressOfNameOrdinals));
		printf("**********func name ord**************\n");
		for(int i = 0; i < pImageExoprtDirectory->NumberOfNames; ++i)
		{
			printf("idx:%d, func address idx:%d\n", i, *(funcNameOrdAddress+i));
		}
	}
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

	printf("size:%x, %x\n", size, pPEHeader->NumberOfSections);


	FILE* fileWrite = fopen(outFile, "wb");
	
	int res = fwrite(pFileBuffer, size, 1, fileWrite);
	if(!res)
	{
		printf("write data failed\n");
		fclose(fileWrite);
		return ; 
	}

	/*for(int i = 0; i < size; ++i)
	{
		if(*((char*)pFileBuffer + i) != *((char *)orgFileBuff + i))
		{
			printf("idx: %d\n", i);
		}
	}*/
	
	fclose(fileWrite);
	fileWrite = NULL;
}

char shellCode[] = {
	0x6A, 0x00, 0x6A, 0x00, 0x6A, 0x00, 0x6A, 0x00,
	0xE8, 0x00, 0x00, 0x00, 0x00,
	0xE9, 0x00, 0x00, 0x00, 0x00
};

#define CODE_SIZE 18
#define FIRST_OFFSET 13
#define SECOND_OFFSET 18


//本计算机入口地址
#define MESSAGE_BOX_ADDRESS 0x7474FDAE

//计算好像没有什么问题，执行却失败，原因待查
void addShellCode(LPVOID pFileBuffer, int sectionIdx)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);;
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);;
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	int sectionCnt = pPEHeader->NumberOfSections;
	if((sectionIdx < 1) || (sectionIdx > sectionCnt))
	{
		printf("no such section!\n");
		return ;
	}

	PIMAGE_SECTION_HEADER pSHToAdd = pSectionHeader + (sectionIdx - 1);

	if (pSHToAdd->SizeOfRawData - pSHToAdd->Misc.VirtualSize < CODE_SIZE)
	{
		printf("not enough free memory!\n");
		return ;
	}

	//拷贝数据
	DWORD pCodeBegin = (DWORD)pFileBuffer+pSHToAdd->PointerToRawData + pSHToAdd->Misc.VirtualSize;
	for(int idx = 0; idx < CODE_SIZE; ++idx)
	{
		*((char *)(pCodeBegin + idx)) = shellCode[idx];
	}


	//修正messageBox的跳转地址
	DWORD pFOffSet1 = pSHToAdd->Misc.VirtualSize + FIRST_OFFSET;
	DWORD jumpValue1 = MESSAGE_BOX_ADDRESS - (pSHToAdd->VirtualAddress + pFOffSet1 + pOptionHeader->ImageBase);
	//printf("virtualAddress:%x, imagebase:%x, offset:%x\n", pSHToAdd->VirtualAddress, pOptionHeader->ImageBase, pFOffSet1);
	//printf("jumpValue1:%x, curValue: %x\n", jumpValue1, (pSHToAdd->VirtualAddress + pFOffSet1 + pOptionHeader->ImageBase));
	*(int *)(pCodeBegin+9) = jumpValue1;

	//修正跳转到原来入口地址的偏移
	DWORD orgEP = pOptionHeader->AddressOfEntryPoint + pOptionHeader->ImageBase;
	DWORD pFOffSet2 = pFOffSet1 + 5;
	DWORD jumpValue2 = orgEP - (pSectionHeader->VirtualAddress + pFOffSet2 + pOptionHeader->ImageBase);
	//printf("jumpValue2:%x\n", jumpValue2);
	*(int *)(pCodeBegin+14) = jumpValue2;


	//修正入口地址
	DWORD curEP = pSHToAdd->VirtualAddress + pSHToAdd->Misc.VirtualSize;// + 13;
	pOptionHeader->AddressOfEntryPoint = curEP;


	//修正节表的属性
	PIMAGE_SECTION_HEADER fPSH = pSectionHeader;
	pSHToAdd->Characteristics = (pSHToAdd->Characteristics | fPSH->Characteristics);
}


//文件对齐和内存对齐不一致的时候会出问题了
void addSection(const char* inFile, const char* outFile)
{
	int addSize = 0x100;
	FILE* file = fopen(inFile, "rb");
	if(!file){
		printf("file open error!");
		return ;
	}
	fseek(file, 0, SEEK_END);
	long int cnt = ftell(file); //字节个数
	LPVOID ptBuff = malloc(cnt + addSize);
	if(!ptBuff){
		printf("assign buff error!");
		fclose(file);
		return ;
	}

	fseek(file, 0, SEEK_SET);
	memset(ptBuff, 0, cnt + addSize);
	printf("cnt----->%x\n", cnt);
	//一次性读取
	size_t n = fread(ptBuff, cnt, 1, file);	
	if(!n)	
	{	
		printf("Fail to read file! ");
		free(ptBuff);
		fclose(file);
		return ;
	}	

	fclose(file);

	LPVOID pFileBuffer = ptBuff;
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);;
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);;
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	DWORD sizeToLastSection = (DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader + pPEHeader->NumberOfSections * sizeof(IMAGE_SECTION_HEADER);
	if(pOptionHeader->SizeOfHeaders - sizeToLastSection < 2 * sizeof(IMAGE_SECTION_HEADER))
	{
		printf("add section failed!\n");
		return ;
	}

	PIMAGE_SECTION_HEADER lastPSHE = pSectionHeader + pPEHeader->NumberOfSections - 1;  //最后一个节
	PIMAGE_SECTION_HEADER addPSHE = lastPSHE + 1;  //增加的节
	PIMAGE_SECTION_HEADER endPSHE = addPSHE + 1; //结尾的节 

	//结尾的街区置0
	/*for(int i = 0; i < 40; ++i)
	{
	*((char *)endPSHE + i) = 0;
	}
	*/

	for(int i = 0; i < 8; ++i)
	{
		addPSHE->Name1[i] = 'A';
	}
	addPSHE->Misc.VirtualSize = addSize;
	addPSHE->VirtualAddress = pOptionHeader->SizeOfImage;
	addPSHE->SizeOfRawData = addSize;

	DWORD pointer = lastPSHE->SizeOfRawData + lastPSHE->PointerToRawData;
	//printf("align:%x\n", pOptionHeader->FileAlignment);
	if(pointer % pOptionHeader->FileAlignment)
	{
		pointer /= pOptionHeader->FileAlignment;
		pointer += pOptionHeader->FileAlignment;
	}

	addPSHE->PointerToRawData = pointer;
	addPSHE->Characteristics = lastPSHE->Characteristics;  //直接复制其属性
	pPEHeader->NumberOfSections += 1;
	pOptionHeader->SizeOfImage += addSize;
	
	writePEFile(outFile, pFileBuffer);

	free(pFileBuffer);
}


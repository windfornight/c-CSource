#include "stdafx.h"
#include <malloc.h>
#include <string.h>

#include "BufferOperation.h"

DWORD getAlignSize(DWORD size, DWORD alignSize)
{
	DWORD newSize = (size / alignSize * alignSize) + (size % alignSize ? alignSize : 0);
	return newSize;
}

LPVOID readFile(const char* inFile)
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

bool isValidPEFile(LPVOID pFileBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	
	//判断是否有效的MZ标志
	if(*((PWORD)pFileBuffer) != IMAGE_DOS_SIGNATURE)
	{
		printf("invalid MZ signature!\n");
		return false ;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);  //char *
	if (pNTHeader->Signature != IMAGE_NT_SIGNATURE)
	{
		printf("invalid PE signature!\n");
		free(pFileBuffer);
		return false;
	}
	return true;
}

//是否一定要到节中去找数据
DWORD convRVAToOffset(LPVOID pFileBuffer, DWORD rva)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);;
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	if (rva >= 0 && rva <= pOptionHeader->SizeOfHeaders)
	{
		//printf("rva in header:%d\n", rva);
		return rva;
	}
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

	if (foa == 0)
	{
		printf("foa is 0, RVA is %x\n", rva);
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

	pFileBuffer = readFile(filePath);
	if(!pFileBuffer)
	{
		printf("Error to read file!\n");
		return ;
	}

	if (!isValidPEFile(pFileBuffer))
	{
		free(pFileBuffer);
		return ;
	}

	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;

	//打印DOS头
	printf("*************DOS**************\n");
	printf("MZ signature:%x\n", pDosHeader->e_magic);
	printf("PE offset:%x\n", pDosHeader->e_lfanew);

	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);  //char *

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
	char* nameList[16] = {"导出表", "导入表", "", "", "", 
		"重定位表", "", "", "", "",
		"", "绑定导入表", "IAT", "", "",
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
	DWORD foa = convRVAToOffset(pFileBuffer, (pImageDataDirectory+0)->VirtualAddress);
	if(foa)
	{
		PIMAGE_EXPORT_DIRECTORY pImageExoprtDirectory = (PIMAGE_EXPORT_DIRECTORY)((DWORD)pFileBuffer+foa);
		printf("********************EXPORT-DIRECTORY******************\n");
		printf("name:%s\n", (char*)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, pImageExoprtDirectory->Name)));
		printf("base:%x\n", pImageExoprtDirectory->Base);
		printf("NumberOfFunctions:%x\n", pImageExoprtDirectory->NumberOfFunctions);
		printf("NumberOfNames:%x\n", pImageExoprtDirectory->NumberOfNames);
		DWORD* funcAddressArray = (DWORD *)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, pImageExoprtDirectory->AddressOfFunctions));

		//打印的是FOA
		printf("*********func address**********\n");
		for(int i = 0; i < pImageExoprtDirectory->NumberOfFunctions; ++i)
		{
			DWORD funcFOA = convRVAToOffset(pFileBuffer, funcAddressArray[i]);
			printf("idx:%d, FOA:%x, RVA:%x\n", i, funcFOA, funcAddressArray[i]);
		}

		//打印函数名字
		DWORD* funcNameAddress = (DWORD*)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, pImageExoprtDirectory->AddressOfNames));
		printf("********func name**************\n");
		for(int i = 0; i < pImageExoprtDirectory->NumberOfNames; ++i)
		{
			DWORD funcFOA = convRVAToOffset(pFileBuffer, funcNameAddress[i]);
			printf("idx:%d, func name:%s\n", i, (char *)((DWORD)pFileBuffer + funcFOA));

		}

		WORD* funcNameOrdAddress = (WORD*)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, pImageExoprtDirectory->AddressOfNameOrdinals));
		printf("**********func name ord**************\n");
		for(int i = 0; i < pImageExoprtDirectory->NumberOfNames; ++i)
		{
			printf("idx:%d, func address idx:%d\n", i, *(funcNameOrdAddress+i));
		}
	}
}

void printRelocationDiretory(LPVOID pFileBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);;
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);;
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	IMAGE_DATA_DIRECTORY *pImageDataDirectory = pOptionHeader->DataDirectory;

	printf("*******************RELOCATION-DIRECTORY*************************\n");
	//重定位表
	PIMAGE_BASE_RELOCATION pImageBaseRelocation = (PIMAGE_BASE_RELOCATION)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, (pImageDataDirectory+5)->VirtualAddress));
	while(pImageBaseRelocation->VirtualAddress)
	{
		int size = (pImageBaseRelocation->Size - sizeof(IMAGE_BASE_RELOCATION)) / 2;
		PWORD relocTbl = (PWORD)(pImageBaseRelocation+1);
		printf("relocation virtual address:%x, cnt:%d\n", pImageBaseRelocation->VirtualAddress, size);
		int needNewLine = 0;
		for(int idx = 0; idx < size; ++idx)
		{
			needNewLine = 1;
			printf("%04x  ", relocTbl[idx]);

			//16个字节，高4位是3的时候需要重定位
			int value = relocTbl[idx];
			if((value >> 12) == 3)
			{
				DWORD addrReloc = pImageBaseRelocation->VirtualAddress + (value & 0xFFF);
				printf("addrReoc:%x\n", addrReloc);
			}

			if((idx + 1) % 8 == 0)
			{
				needNewLine = 0;
				printf("\n");
			}
		}
		if(needNewLine)
		{
			printf("\n");
		}
		pImageBaseRelocation = (PIMAGE_BASE_RELOCATION)((DWORD)pImageBaseRelocation + pImageBaseRelocation->Size);
	}
}

bool isEndImportDescriptor(PIMAGE_IMPORT_DESCRIPTOR pImageImportDescriptor)
{
	for (int i = 0; i < sizeof(IMAGE_IMPORT_DESCRIPTOR); ++i)
	{
		if(*((char*)pImageImportDescriptor+i) != 0)
		{
			return false;
		}
	}
	return true;
}

void printImportFuncInfo(LPVOID pFileBuffer, PIMAGE_THUNK_DATA32 pImageThunkData)
{
	while(*(DWORD*)pImageThunkData != 0)
	{
		//最高位是1，按照序号导出
		if(pImageThunkData->ul.Ordinal & 0x80000000)
		{
			//除去最高位就是序号
			int ordinal = (unsigned)pImageThunkData->ul.Ordinal & (~0x80000000);
			//printf("import by ordinal:%x\n", ordinal);
		}else  //是0的话就是按照名字导出
		{
			PIMAGE_IMPORT_BY_NAME pImageImportByName = (PIMAGE_IMPORT_BY_NAME)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, (DWORD)pImageThunkData->ul.AddressOfData));
			printf("import by name, Hint:%x, Name:%s, address:%x\n", pImageImportByName->Hint, pImageImportByName->Name, (DWORD)pImageImportByName); //注意，这里填充的就是一个名字，结构体已经加上了基值
		}
		++pImageThunkData;
	}
}

void printImportDescriptor(LPVOID pFileBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);;
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	IMAGE_DATA_DIRECTORY *pImageDataDirectory = pOptionHeader->DataDirectory;
	//printf("IAT address:%x\n", (pImageDataDirectory+12)->VirtualAddress);

	printf("*******************IMPORT-DESCRIPTOR*************************\n");
	//导入表
	PIMAGE_IMPORT_DESCRIPTOR pImageDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, (pImageDataDirectory+1)->VirtualAddress));
	while(!isEndImportDescriptor(pImageDescriptor))
	{
		char* nameStr = (char *)pFileBuffer + convRVAToOffset(pFileBuffer, pImageDescriptor->Name);
		printf("dll name:%s, timeDateStamp:%d\n", nameStr, pImageDescriptor->TimeDateStamp);
		//printImportFuncInfo(pFileBuffer, (PIMAGE_THUNK_DATA32)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, pImageDescriptor->OriginalFirstThunk)));
		printImportFuncInfo(pFileBuffer, (PIMAGE_THUNK_DATA32)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, pImageDescriptor->FirstThunk)));

		/*{if (pImageDescriptor->FirstThunk == (pImageDataDirectory+12)->VirtualAddress)
		{
			printf("IAT item:%x\n", pImageDescriptor->FirstThunk);
		}*/
		++pImageDescriptor;
	}
}

void printIATInfo(LPVOID pFileBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);;
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	IMAGE_DATA_DIRECTORY *pImageDataDirectory = pOptionHeader->DataDirectory;
	printf("IAT address:%x\n", (pImageDataDirectory+12)->VirtualAddress);
}

void printBoundImportTbl(LPVOID pFileBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);;
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	IMAGE_DATA_DIRECTORY *pImageDataDirectory = pOptionHeader->DataDirectory;
	
	PIMAGE_BOUND_IMPORT_DESCRIPTOR pImageBoundImpDesc = (PIMAGE_BOUND_IMPORT_DESCRIPTOR)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, (pImageDataDirectory+11)->VirtualAddress));
	char *pImgBoundHeader = (char *)pImageBoundImpDesc;
	while(pImageBoundImpDesc->TimeDateStamp)
	{
		printf("Bound TimeDateStamp:%d, DLL Name:%s\n", pImageBoundImpDesc->TimeDateStamp, pImgBoundHeader + pImageBoundImpDesc->OffsetModuleName);
		for(int idx = 1; idx <= pImageBoundImpDesc->NumberOfModuleForwarderRefs; ++idx)
		{
			PIMAGE_BOUND_FORWARDER_REF pImageBoundForRef = (PIMAGE_BOUND_FORWARDER_REF)(pImageBoundImpDesc + idx);
			printf("REF TimeDateStam:%d, DLL Name:%s\n", pImageBoundForRef->TimeDateStamp, pImgBoundHeader+pImageBoundForRef->OffsetModuleName);
		}
		printf("\n");
		pImageBoundImpDesc = pImageBoundImpDesc + pImageBoundImpDesc->NumberOfModuleForwarderRefs + 1;
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

	free(pFileBuffer);

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

	free(pImageBuffer);
	return pFileBuffer;
}

size_t getPEFileSize(LPVOID pFileBuffer)
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
		printf("error size!\n");
		return 0;
	}
	return size;
}

void writePEFile(const char* outFile, LPVOID pFileBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);;
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);;
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	size_t size =  getPEFileSize(pFileBuffer);
	//printf("write size: %x\n", size);
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
	printf("data has been written!\n");
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
#define MESSAGE_BOX_ADDRESS 0x7681FDAE

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

	/*if (pSHToAdd->SizeOfRawData - pSHToAdd->Misc.VirtualSize < CODE_SIZE)
	{
		printf("not enough free memory!\n");
		return ;
	}*/

	//拷贝数据
	DWORD pCodeBegin = (DWORD)pFileBuffer+pSHToAdd->PointerToRawData + pSHToAdd->Misc.VirtualSize - 20;
	for(int idx = 0; idx < CODE_SIZE; ++idx)
	{
		*((char *)(pCodeBegin + idx)) = shellCode[idx];
	}


	//修正messageBox的跳转地址
	DWORD pFOffSet1 = pSHToAdd->Misc.VirtualSize - 20 + FIRST_OFFSET;
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
	DWORD curEP = pSHToAdd->VirtualAddress + pSHToAdd->Misc.VirtualSize - 20;// + 13;
	pOptionHeader->AddressOfEntryPoint = curEP;


	//修正节表的属性
	PIMAGE_SECTION_HEADER fPSH = pSectionHeader;
	pSHToAdd->Characteristics = (pSHToAdd->Characteristics | fPSH->Characteristics);

}

void dataCopy(LPVOID des, LPVOID src, size_t size)
{
	for(int i = 0; i < size; ++i)
	{
		*((char *)((DWORD)des + i)) = *((char *)((DWORD)src + i)); 
	}
}

//如果不做处理直接在最后一节加数据其实是不一定完全正确的（并不能保证节表之后的数据没有作用）
LPVOID addSection(LPVOID pFileBuffer, int addSize)
{
	if(!isValidPEFile(pFileBuffer))
	{
		free(pFileBuffer);
		return NULL;
	}

	size_t orgSize = getPEFileSize(pFileBuffer);

	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);;
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);;
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	DWORD sizeToLastSection = (DWORD)pOptionHeader - (DWORD)pDosHeader + pPEHeader->SizeOfOptionalHeader + pPEHeader->NumberOfSections * sizeof(IMAGE_SECTION_HEADER);
	if(pOptionHeader->SizeOfHeaders - sizeToLastSection < 2 * sizeof(IMAGE_SECTION_HEADER))  //只做空间的检查，并不保证一定能执行
	{
		printf("add section failed!\n");
		return NULL;
	}

	PIMAGE_SECTION_HEADER lastPSHE = pSectionHeader + pPEHeader->NumberOfSections - 1;  //最后一个节
	PIMAGE_SECTION_HEADER addPSHE = lastPSHE + 1;  //增加的节
	PIMAGE_SECTION_HEADER endPSHE = addPSHE + 1; //结尾的节 

	//结尾的街区置0
	for(int i = 0; i < sizeof(IMAGE_SECTION_HEADER); ++i)
	{
		*((char *)endPSHE + i) = 0;
	}
	
	strncpy(addPSHE->Name1, "ADDSEC", 8);
	addPSHE->Misc.VirtualSize = addSize;//getAlignSize(addSize, pOptionHeader->FileAlignment);
	addPSHE->VirtualAddress = pOptionHeader->SizeOfImage;
	addPSHE->SizeOfRawData = getAlignSize(addSize, pOptionHeader->FileAlignment);
	addPSHE->PointerToRawData = lastPSHE->SizeOfRawData + lastPSHE->PointerToRawData;
	addPSHE->Characteristics = lastPSHE->Characteristics;  //直接复制其属性
	
	//其他成员直接致空
	addPSHE->PointerToRelocations = 0;
	addPSHE->PointerToLinenumbers = 0;
	addPSHE->NumberOfRelocations = 0;
	addPSHE->NumberOfLinenumbers = 0;

	pPEHeader->NumberOfSections += 1;
	pOptionHeader->SizeOfImage += addSize;

	size_t newSize = orgSize + addPSHE->SizeOfRawData;
	LPVOID newPFBuff = malloc(newSize);
	memset(newPFBuff, 0, newSize);
	dataCopy(newPFBuff, pFileBuffer, orgSize);
	free(pFileBuffer);

	return newPFBuff;
}

void moveNTHead(LPVOID pFileBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);;
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	if(pDosHeader->e_lfanew == sizeof(IMAGE_DOS_HEADER))
	{
		return ;
	}
	PIMAGE_SECTION_HEADER endPSH = pSectionHeader + pPEHeader->NumberOfSections;
	size_t size = (DWORD)endPSH - (DWORD)pNTHeader + sizeof(IMAGE_SECTION_HEADER);
	LPVOID pDes = (LPVOID)((DWORD)pFileBuffer + sizeof(IMAGE_DOS_HEADER));
	dataCopy(pDes, (LPVOID)pNTHeader, size);
	pDosHeader->e_lfanew = sizeof(IMAGE_DOS_HEADER);
}

LPVOID mergeAllSections(LPVOID pFileBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);;
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	//printf("size of image:%x\n", pOptionHeader->SizeOfImage);

	if(pPEHeader->NumberOfSections <= 1)
	{
		return pFileBuffer;
	}

	LPVOID pImageBuffer = extendToImageBuff(pFileBuffer);
	pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pImageBuffer + pDosHeader->e_lfanew);;
	pPEHeader =  &(pNTHeader->FileHeader);
	pOptionHeader = &(pNTHeader->OptionalHeader);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	for(int i = 1; i < pPEHeader->NumberOfSections; ++i)
	{
		pSectionHeader->Characteristics |= (pSectionHeader + i)->Characteristics;
	}

	PIMAGE_SECTION_HEADER lastPSH = pSectionHeader + pPEHeader->NumberOfSections - 1;
	DWORD max = (lastPSH->SizeOfRawData > lastPSH->Misc.VirtualSize)?lastPSH->SizeOfRawData:lastPSH->Misc.VirtualSize;
	//printf("%x, %x, %x\n", max, lastPSH->SizeOfRawData, lastPSH->Misc.VirtualSize);
	pSectionHeader->SizeOfRawData = getAlignSize(lastPSH->VirtualAddress + max - pSectionHeader->VirtualAddress, pOptionHeader->FileAlignment);
	pSectionHeader->Misc.VirtualSize = pSectionHeader->SizeOfRawData;  //这个属性很重要（为什么会用virtualsize大于sizeofrawdata的情况）

	//printf("size of raw data:%x\n", pSectionHeader->SizeOfRawData);
	//printf("size of header:%x, first address:%x, first raw pointer:%x\n", pOptionHeader->SizeOfHeaders, pSectionHeader->VirtualAddress, pSectionHeader->PointerToRawData);
	memset((pSectionHeader+1), 0, sizeof(IMAGE_SECTION_HEADER));
	pPEHeader->NumberOfSections = 1;
	return reduceToFileBuff(pImageBuffer);
}


//拉伸后再扩大，避免virtuasize大于sizeofrawdata可能带来的错误
LPVOID extendLastSection(LPVOID pFileBuffer, size_t size)
{
	LPVOID pImageBuffer = extendToImageBuff(pFileBuffer);
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pImageBuffer + pDosHeader->e_lfanew);;
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	size_t alignExSize = getAlignSize(size, pOptionHeader->SectionAlignment);
	DWORD orgSize = pOptionHeader->SizeOfImage;

	LPVOID newPImgBuffer = malloc(orgSize + alignExSize);
	memset(newPImgBuffer, 0, orgSize+alignExSize);
	dataCopy(newPImgBuffer, pImageBuffer, orgSize);

	free(pImageBuffer);

	pDosHeader = (PIMAGE_DOS_HEADER)newPImgBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)newPImgBuffer + pDosHeader->e_lfanew);;
	pPEHeader =  &(pNTHeader->FileHeader);
	pOptionHeader = &(pNTHeader->OptionalHeader);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	PIMAGE_SECTION_HEADER lastPSH = pSectionHeader + pPEHeader->NumberOfSections - 1;
	DWORD max = (lastPSH->SizeOfRawData > lastPSH->Misc.VirtualSize)?lastPSH->SizeOfRawData:lastPSH->Misc.VirtualSize;
	//printf("%x, %x, %x\n", max, lastPSH->SizeOfRawData, lastPSH->Misc.VirtualSize);
	//lastPSH->Misc.VirtualSize = max + size;  --错误的
	lastPSH->SizeOfRawData = getAlignSize(max, pOptionHeader->SectionAlignment)+alignExSize;  //这个属性值得注意
	lastPSH->Misc.VirtualSize = lastPSH->SizeOfRawData;
	pOptionHeader->SizeOfImage += alignExSize;

	return reduceToFileBuff(newPImgBuffer);
}

void addShellCodeTest(const char* inFile, const char* outFile)
{
	LPVOID pFileBuffer = readFile(inFile);
	pFileBuffer = mergeAllSections(pFileBuffer);

	size_t size = 100;
	LPVOID pImageBuffer = extendToImageBuff(pFileBuffer);
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pImageBuffer + pDosHeader->e_lfanew);;
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	size_t alignExSize = getAlignSize(size, pOptionHeader->SectionAlignment);
	DWORD orgSize = pOptionHeader->SizeOfImage;

	LPVOID newPImgBuffer = malloc(orgSize + alignExSize);
	memset(newPImgBuffer, 0, orgSize+alignExSize);
	dataCopy(newPImgBuffer, pImageBuffer, orgSize);

	free(pImageBuffer);

	pDosHeader = (PIMAGE_DOS_HEADER)newPImgBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)newPImgBuffer + pDosHeader->e_lfanew);;
	pPEHeader =  &(pNTHeader->FileHeader);
	pOptionHeader = &(pNTHeader->OptionalHeader);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	PIMAGE_SECTION_HEADER lastPSH = pSectionHeader + pPEHeader->NumberOfSections - 1;
	DWORD max = (lastPSH->SizeOfRawData > lastPSH->Misc.VirtualSize)?lastPSH->SizeOfRawData:lastPSH->Misc.VirtualSize;
	lastPSH->SizeOfRawData = getAlignSize(max, pOptionHeader->SectionAlignment)+alignExSize;  //这个属性值得注意
	lastPSH->Misc.VirtualSize = lastPSH->SizeOfRawData;
	pOptionHeader->SizeOfImage += alignExSize;

	int addSecIdx = pPEHeader->NumberOfSections;
	newPImgBuffer = reduceToFileBuff(newPImgBuffer);

	addShellCode(newPImgBuffer, addSecIdx);
	writePEFile(outFile, newPImgBuffer);
	free(newPImgBuffer);
}

DWORD getFuncAddrByName(LPVOID pFileBuffer, const char* funcName)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);;
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);;
	IMAGE_DATA_DIRECTORY *pImageDataDirectory = pOptionHeader->DataDirectory;

	DWORD exportFOA = convRVAToOffset(pFileBuffer, (pImageDataDirectory+0)->VirtualAddress);
	if (!exportFOA)
	{
		printf("no export table!\n");
		return 0;
	}

	//导出表
	PIMAGE_EXPORT_DIRECTORY pImageExportDirectory = (PIMAGE_EXPORT_DIRECTORY)((DWORD)pFileBuffer+exportFOA);
	DWORD funcCnt = pImageExportDirectory->NumberOfFunctions;
	DWORD nameCnt = pImageExportDirectory->NumberOfNames;
	DWORD* funcAddr = (DWORD*)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, pImageExportDirectory->AddressOfFunctions));
	DWORD* funcNamesAddr = (DWORD*)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, pImageExportDirectory->AddressOfNames));
	WORD* funcNameOrdAddr = (WORD*)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, pImageExportDirectory->AddressOfNameOrdinals));
	
	for(int idx = 0; idx < nameCnt; ++idx)
	{
		char* pFuncName =  (char*)((DWORD)pFileBuffer+ convRVAToOffset(pFileBuffer, funcNamesAddr[idx]));
		if(strcmp(pFuncName, funcName) == 0)
		{
			int eIdx = funcNameOrdAddr[idx];
			return funcAddr[eIdx];
		}
	}

	return 0;
}

DWORD getFuncAddrByOrdinal(LPVOID pFileBuffer, int ordinal)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);;
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);;
	IMAGE_DATA_DIRECTORY *pImageDataDirectory = pOptionHeader->DataDirectory;

	DWORD exportFOA = convRVAToOffset(pFileBuffer, (pImageDataDirectory+0)->VirtualAddress);
	if (!exportFOA)
	{
		printf("no export table!\n");
		return 0;
	}

	//导出表
	PIMAGE_EXPORT_DIRECTORY pImageExportDirectory = (PIMAGE_EXPORT_DIRECTORY)((DWORD)pFileBuffer+exportFOA);
	DWORD funcCnt = pImageExportDirectory->NumberOfFunctions;
	DWORD* funcAddr = (DWORD*)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, pImageExportDirectory->AddressOfFunctions));
	DWORD base = pImageExportDirectory->Base;
	int eIdx = ordinal - base;
	if(eIdx < funcCnt)
	{
		return funcAddr[eIdx];
	}
	return 0;
}

void changeImageBase(LPVOID pFileBuffer, DWORD newBase)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);;
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);;
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	IMAGE_DATA_DIRECTORY *pImageDataDirectory = pOptionHeader->DataDirectory;

	DWORD delta = newBase - pOptionHeader->ImageBase;

	//重定位表
	PIMAGE_BASE_RELOCATION pImageBaseRelocation = (PIMAGE_BASE_RELOCATION)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, (pImageDataDirectory+5)->VirtualAddress));
	while(pImageBaseRelocation->VirtualAddress)
	{
		int size = (pImageBaseRelocation->Size - sizeof(IMAGE_BASE_RELOCATION)) / 2;
		PWORD relocTbl = (PWORD)(pImageBaseRelocation+1);
		for(int idx = 0; idx < size; ++idx)
		{
			//16个字节，高4位是3的时候指向的RVA需要重定位
			int value = relocTbl[idx];
			if((value >> 12) == 3)
			{
				DWORD addrReloc = pImageBaseRelocation->VirtualAddress + (value & 0xFFF);
				DWORD* modifyAddr = (DWORD*)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, addrReloc));
				*modifyAddr += delta;
			}
		}
		pImageBaseRelocation = (PIMAGE_BASE_RELOCATION)((DWORD)pImageBaseRelocation + pImageBaseRelocation->Size);
	}

	pOptionHeader->ImageBase = newBase;
}

DWORD calSizeForMoveExpDir(LPVOID pFileBuffer)
{
	DWORD size = 0;

	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);;
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);;
	IMAGE_DATA_DIRECTORY *pImageDataDirectory = pOptionHeader->DataDirectory;


	DWORD exportFOA = convRVAToOffset(pFileBuffer, (pImageDataDirectory+0)->VirtualAddress);
	if (!exportFOA)
	{
		printf("no export table!\n");
		return 0;
	}

	//导出表
	PIMAGE_EXPORT_DIRECTORY pImageExportDirectory = (PIMAGE_EXPORT_DIRECTORY)((DWORD)pFileBuffer+exportFOA);
	DWORD funcCnt = pImageExportDirectory->NumberOfFunctions;
	DWORD nameCnt = pImageExportDirectory->NumberOfNames;
	DWORD* funcAddr = (DWORD*)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, pImageExportDirectory->AddressOfFunctions));
	DWORD* funcNamesAddr = (DWORD*)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, pImageExportDirectory->AddressOfNames));
	WORD* funcNameOrdAddr = (WORD*)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, pImageExportDirectory->AddressOfNameOrdinals));

	size += sizeof(IMAGE_EXPORT_DIRECTORY);
	size += 4 * funcCnt;
	size += (4 + 2) * nameCnt;

	for(int idx = 0; idx < nameCnt; ++idx)
	{
		char* pFuncName =  (char*)((DWORD)pFileBuffer+ convRVAToOffset(pFileBuffer, funcNamesAddr[idx]));
		size += (strlen(pFuncName) + 1);
	}

	return  size; 
}

//节的属性？
LPVOID moveExpDirToNewSec(LPVOID pFileBuffer)
{
	DWORD size = calSizeForMoveExpDir(pFileBuffer);
	pFileBuffer = addSection(pFileBuffer, size);

	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);;
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);;
	IMAGE_DATA_DIRECTORY *pImageDataDirectory = pOptionHeader->DataDirectory;
	PIMAGE_SECTION_HEADER pImageSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	PIMAGE_SECTION_HEADER newPSH = pImageSectionHeader + pPEHeader->NumberOfSections - 1;
	DWORD addrOffset = newPSH->PointerToRawData; //FOA

	DWORD exportFOA = convRVAToOffset(pFileBuffer, (pImageDataDirectory+0)->VirtualAddress);
	if (!exportFOA)
	{
		return pFileBuffer;
	}

	//导出表
	PIMAGE_EXPORT_DIRECTORY pImageExportDirectory = (PIMAGE_EXPORT_DIRECTORY)((DWORD)pFileBuffer+exportFOA);

	//复制导出表,并修改导出表的相关属性
	dataCopy((LPVOID)((DWORD)pFileBuffer + addrOffset), (LPVOID)pImageExportDirectory, sizeof(IMAGE_EXPORT_DIRECTORY));
	pImageExportDirectory = (PIMAGE_EXPORT_DIRECTORY)((DWORD)pFileBuffer+addrOffset);
	(pImageDataDirectory+0)->VirtualAddress = newPSH->VirtualAddress + (addrOffset - newPSH->PointerToRawData);
	addrOffset += sizeof(IMAGE_EXPORT_DIRECTORY);


	DWORD funcCnt = pImageExportDirectory->NumberOfFunctions;
	DWORD nameCnt = pImageExportDirectory->NumberOfNames;

	DWORD* funcAddr = (DWORD*)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, pImageExportDirectory->AddressOfFunctions));
	DWORD* funcNamesAddr = (DWORD*)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, pImageExportDirectory->AddressOfNames));
	WORD* funcNameOrdAddr = (WORD*)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, pImageExportDirectory->AddressOfNameOrdinals));

	//复制函数地址
	pImageExportDirectory->AddressOfFunctions = newPSH->VirtualAddress + (addrOffset-newPSH->PointerToRawData);
	DWORD* newFuncAddr = (DWORD*)((DWORD)pFileBuffer + addrOffset);
	for(int idx = 0; idx < funcCnt; ++idx)
	{
		newFuncAddr[idx] = funcAddr[idx];
		addrOffset += 4;
	}
	
	//复制函数名地址
	pImageExportDirectory->AddressOfNames = newPSH->VirtualAddress + (addrOffset - newPSH->PointerToRawData);
	DWORD* newFuncNamesAddr = (DWORD*)((DWORD)pFileBuffer + addrOffset);
	for(int idx = 0; idx < nameCnt; ++idx)
	{
		newFuncNamesAddr[idx] = funcNamesAddr[idx];
		addrOffset += 4;
	}

	//复制函数名序号(考虑一下对齐)
	pImageExportDirectory->AddressOfNameOrdinals = newPSH->VirtualAddress + (addrOffset - newPSH->PointerToRawData);
	WORD* newFuncNameOrdAddr = (WORD*)((DWORD)pFileBuffer + addrOffset);
	for(int idx = 0; idx < nameCnt; ++idx)
	{
		newFuncNameOrdAddr[idx] = funcNameOrdAddr[idx];
		addrOffset += 2;
	}
	
	//复制函数名字
	for(int idx = 0; idx < nameCnt; ++idx)
	{
		char* pFuncName =  (char*)((DWORD)pFileBuffer+ convRVAToOffset(pFileBuffer, newFuncNamesAddr[idx]));
		char* res = strcpy((char*)pFileBuffer+addrOffset, pFuncName);
		newFuncNamesAddr[idx] = newPSH->VirtualAddress + (addrOffset - newPSH->PointerToRawData);
		addrOffset += (strlen(res) + 1);
	}

	return pFileBuffer;
}


DWORD calSizeForMoveRelocDir(LPVOID pFileBuffer)
{
	size_t size = 0;

	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);;
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);;
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	IMAGE_DATA_DIRECTORY *pImageDataDirectory = pOptionHeader->DataDirectory;

	//重定位表
	PIMAGE_BASE_RELOCATION pImageBaseRelocation = (PIMAGE_BASE_RELOCATION)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, (pImageDataDirectory+5)->VirtualAddress));
	while(pImageBaseRelocation->VirtualAddress)
	{
		size += pImageBaseRelocation->Size;
		int cnt = (pImageBaseRelocation->Size - sizeof(IMAGE_BASE_RELOCATION)) / 2;
		size += (4 * cnt);
		pImageBaseRelocation = (PIMAGE_BASE_RELOCATION)((DWORD)pImageBaseRelocation + pImageBaseRelocation->Size);
	}
	size += sizeof(IMAGE_BASE_RELOCATION);

	return size;
}

LPVOID moveRelocDirToNewSec(LPVOID pFileBuffer)
{
	DWORD size = calSizeForMoveRelocDir(pFileBuffer);
	pFileBuffer = addSection(pFileBuffer, size);

	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);;
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);;
	IMAGE_DATA_DIRECTORY *pImageDataDirectory = pOptionHeader->DataDirectory;
	PIMAGE_SECTION_HEADER pImageSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	PIMAGE_SECTION_HEADER newPSH = pImageSectionHeader + pPEHeader->NumberOfSections - 1;
	DWORD addrOffset = newPSH->PointerToRawData; //FOA

	//重定位表
	PIMAGE_BASE_RELOCATION pImageBaseRelocation = (PIMAGE_BASE_RELOCATION)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, (pImageDataDirectory+5)->VirtualAddress));
	while(pImageBaseRelocation->VirtualAddress)
	{
		dataCopy((LPVOID)((DWORD)pFileBuffer+addrOffset), (LPVOID)pImageBaseRelocation, pImageBaseRelocation->Size);
		addrOffset += pImageBaseRelocation->Size;
		pImageBaseRelocation = (PIMAGE_BASE_RELOCATION)((DWORD)pImageBaseRelocation + pImageBaseRelocation->Size);
	}

	//结尾标志
	dataCopy((LPVOID)((DWORD)pFileBuffer+addrOffset), (LPVOID)pImageBaseRelocation, sizeof(IMAGE_BASE_RELOCATION));
	addrOffset += sizeof(IMAGE_BASE_RELOCATION);

	//修改导出表目录
	(pImageDataDirectory+5)->VirtualAddress = newPSH->VirtualAddress;

	//如何拷贝重定位表指向的具体地址？（内存对齐？？？）

	return pFileBuffer;
}

DWORD calSizeForMoveImportDir(LPVOID pFileBuffer)
{
	size_t size = 0;

	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);;
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);;
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	IMAGE_DATA_DIRECTORY *pImageDataDirectory = pOptionHeader->DataDirectory;

	PIMAGE_IMPORT_DESCRIPTOR pImageDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, (pImageDataDirectory+1)->VirtualAddress));
	int index = 0;
	while(!isEndImportDescriptor(pImageDescriptor))
	{
		size += sizeof(IMAGE_IMPORT_DESCRIPTOR);

		//INT表的计算
		PIMAGE_THUNK_DATA32 pImageThunkData = (PIMAGE_THUNK_DATA32)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, pImageDescriptor->OriginalFirstThunk)); 
		while(*(DWORD*)pImageThunkData != 0)
		{
			size += sizeof(IMAGE_THUNK_DATA32);
			if(!(pImageThunkData->ul.Ordinal & 0x80000000)) //按照名字导出
			{
				PIMAGE_IMPORT_BY_NAME pImageImportByName = (PIMAGE_IMPORT_BY_NAME)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, (DWORD)pImageThunkData->ul.AddressOfData));
				size += sizeof(IMAGE_IMPORT_BY_NAME);
				size += strlen(pImageImportByName->Name);  //名字的长度，注意，这里由于结构的长度对齐占了两个，所以就不加上结束标志的占位空间了
			}
			++pImageThunkData;
		}
		size += sizeof(IMAGE_THUNK_DATA32);

		++pImageDescriptor;
		index ++;
	}

	//结尾部分
	size += sizeof(IMAGE_IMPORT_DESCRIPTOR);

	return size;
}

LPVOID expendImportDirToNewSec(LPVOID pFileBuffer, LPVOID inFileBuffer)
{
	DWORD size = calSizeForMoveImportDir(pFileBuffer);

	//新增的导入表结构大小
	size += sizeof(IMAGE_IMPORT_DESCRIPTOR);

	//新增的导入表大小

	pFileBuffer = addSection(pFileBuffer, size);
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);;
	PIMAGE_FILE_HEADER pPEHeader =  &(pNTHeader->FileHeader);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = &(pNTHeader->OptionalHeader);;
	IMAGE_DATA_DIRECTORY *pImageDataDirectory = pOptionHeader->DataDirectory;
	PIMAGE_SECTION_HEADER pImageSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	PIMAGE_SECTION_HEADER newPSH = pImageSectionHeader + pPEHeader->NumberOfSections - 1;
	DWORD addrOffset = newPSH->PointerToRawData; //FOA

	//原来的导入表
	PIMAGE_IMPORT_DESCRIPTOR pImageDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, (pImageDataDirectory+1)->VirtualAddress));

	//移动后的导入表
	PIMAGE_IMPORT_DESCRIPTOR pNewImageDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)pFileBuffer + newPSH->PointerToRawData);

	int index = 0;
	//拷贝导入表
	while(!isEndImportDescriptor(pImageDescriptor))
	{
		//拷贝数据
		memcpy((char*)pFileBuffer + addrOffset, pImageDescriptor, sizeof(IMAGE_IMPORT_DESCRIPTOR));
		addrOffset += sizeof(IMAGE_IMPORT_DESCRIPTOR);	
		++pImageDescriptor;
		index++;
	}

	memset((char*)pFileBuffer + addrOffset, 0, 2*sizeof(IMAGE_IMPORT_DESCRIPTOR));
	addrOffset += 2*sizeof(IMAGE_IMPORT_DESCRIPTOR);
	
	//修改目录属性
	(pImageDataDirectory+1)->VirtualAddress = newPSH->VirtualAddress;
	//(pImageDataDirectory+1)->isize = addrOffset;

	while(!isEndImportDescriptor(pNewImageDescriptor))
	{
		//拷贝INT表
		PIMAGE_THUNK_DATA32 pImageThunkData = (PIMAGE_THUNK_DATA32)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, pNewImageDescriptor->OriginalFirstThunk)); 

		//修改属性
		pNewImageDescriptor->OriginalFirstThunk = (addrOffset - newPSH->PointerToRawData) + newPSH->VirtualAddress;  //本节区的文件偏移加本节区的RVA

		while(*(DWORD*)pImageThunkData != 0)
		{
			memcpy((char*)pFileBuffer+addrOffset, pImageThunkData, sizeof(IMAGE_THUNK_DATA32));
			addrOffset += sizeof(IMAGE_THUNK_DATA32);
			++pImageThunkData;
		}

		//结尾标志
		memset((char*)pFileBuffer+addrOffset, 0, sizeof(IMAGE_THUNK_DATA32));
		addrOffset += sizeof(IMAGE_THUNK_DATA32);

		//拷贝INT表指向的名字导入部分数据
		pImageThunkData = (PIMAGE_THUNK_DATA32)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, pNewImageDescriptor->OriginalFirstThunk)); 
		while(*(DWORD*)pImageThunkData != 0)
		{
			
			if(!(pImageThunkData->ul.Ordinal & 0x80000000)) //按照名字导出
			{
				PIMAGE_IMPORT_BY_NAME pImageImportByName = (PIMAGE_IMPORT_BY_NAME)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, (DWORD)pImageThunkData->ul.AddressOfData));
				//修改属性
				pImageThunkData->ul.AddressOfData = (PIMAGE_IMPORT_BY_NAME)(addrOffset - newPSH->PointerToRawData + newPSH->VirtualAddress);  //本节区的文件偏移加本节区的RVA
				//拷贝结构
				memcpy((char*)pFileBuffer+addrOffset, pImageImportByName, sizeof(IMAGE_IMPORT_BY_NAME));
				//拷贝名字
				PIMAGE_IMPORT_BY_NAME pNewImageImportByName = (PIMAGE_IMPORT_BY_NAME)((DWORD)pFileBuffer + convRVAToOffset(pFileBuffer, (DWORD)pImageThunkData->ul.AddressOfData));   /////
				strcpy(pNewImageImportByName->Name, pImageImportByName->Name);
				
				addrOffset += sizeof(IMAGE_IMPORT_BY_NAME);
				addrOffset += strlen(pImageImportByName->Name);  //名字的长度，注意，这里由于结构的长度对齐占了两个，所以就不加上结束标志的占位空间了
			}
			++pImageThunkData;
		}

		//注意，这里不能拷贝IAT表（想想重定位表，间接call的时候）
		++pNewImageDescriptor;
	}


	//根据dll的导出表来填充导入表


	return pFileBuffer;
}







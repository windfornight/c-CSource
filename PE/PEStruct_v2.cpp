#define IMAGE_DOS_SIGNATURE 0x5a4d
#define IMAGE_NT_SIGNATURE 0x4550
#define IMAGE_SIZEOF_FILE_HEADER 0x14

typedef short int WORD;
typedef int DWORD;
typedef long int LONG;
typedef char BYTE;

typedef WORD* PWORD;
typedef void* LPVOID;

typedef struct _IMAGE_DOS_HEADER
{
	WORD e_magic;
	WORD e_cblp;
	WORD e_cp;
	WORD e_crlc;
	WORD e_cparhdr;
	WORD e_minalloc;
	WORD e_maxalloc;
	WORD e_ss;
	WORD e_sp;
	WORD e_csum;
	WORD e_ip;
	WORD e_cs;
	WORD e_lfarlc;
	WORD e_ovno;
	WORD e_res[4];
	WORD e_oemid;
	WORD e_oeminfo;
	WORD e_res2[10];
	LONG e_lfanew;
}IMAGE_DOS_HEADER;

typedef struct IMAGE_FILE_HEADER
{
	WORD Machine;  
	WORD NumberOfSections;  
	DWORD TimeDateStamp; 
	DWORD PointerToSymbolTable;  
	DWORD NumberOfSymbols;
	WORD SizeOfOptionalHeader;
	WORD Characteristics;
}IMAGE_FILE_HEADER;

typedef struct IMAGE_DATA_DIRECTORY
{
	DWORD VirtualAddress;
	DWORD isize;
}IMAGE_DATA_DIRECTORY;

typedef struct IMAGE_OPTIONAL_HEADER32
{
	WORD Magic;
	BYTE MajorLinkerVersion;
	BYTE MinorLinkerVersion;
	DWORD SizeOfCode;
	DWORD SizeOfInitializedData;
	DWORD SizeOfUninitializedData;
	DWORD AddressOfEntryPoint;
	DWORD BaseOfCode;
	DWORD BaseOfData;
	DWORD ImageBase;
	DWORD SectionAlignment;
	DWORD FileAlignment;
	WORD MajorOperatingSystemVersion;
	WORD MinorOperatingSystemVersion;
	WORD MajorImageVersion;
	WORD MinorImageVersion;
	WORD MajorSubsystemVersion;
	WORD MinorSubsystemVersion;
	DWORD Win32VersionValue;
	DWORD SizeOfImage;
	DWORD SizeOfHeaders;
	DWORD CheckSum;
	WORD Subsystem;
	WORD DllCharacteristics;
	DWORD SizeOfStackReserve;
	DWORD SizeOfStackCommit;
	DWORD SizeOfHeapReserve;
	DWORD SizeOfHeapCommit;
	DWORD LoaderFlags;
	DWORD NumberOfRvaAndSizes;
	IMAGE_DATA_DIRECTORY DataDirectory[16];
}IMAGE_OPTIONAL_HEADER32;

typedef struct IMAGE_NT_HEADERS
{
	DWORD Signature;
	IMAGE_FILE_HEADER FileHeader;
	IMAGE_OPTIONAL_HEADER32 OptionalHeader;
}IMAGE_NT_HEADERS;

typedef struct IMAGE_SECTION_HEADER
{
	BYTE Name1[8];
	union{
		DWORD PhysicalAddress;
		DWORD VirtualSize;
	} Misc; 
	DWORD VirtualAddress;  
	DWORD SizeOfRawData; 
	DWORD PointerToRawData; 
	DWORD PointerToRelocations;  
	DWORD PointerToLinenumbers;  
	WORD NumberOfRelocations;  
	WORD NumberOfLinenumbers;  
	DWORD Characteristics;
}IMAGE_SECTION_HEADER;

typedef IMAGE_DOS_HEADER* PIMAGE_DOS_HEADER;
typedef IMAGE_NT_HEADERS* PIMAGE_NT_HEADERS;
typedef IMAGE_FILE_HEADER* PIMAGE_FILE_HEADER;
typedef IMAGE_OPTIONAL_HEADER32* PIMAGE_OPTIONAL_HEADER32;
typedef IMAGE_SECTION_HEADER* PIMAGE_SECTION_HEADER;

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
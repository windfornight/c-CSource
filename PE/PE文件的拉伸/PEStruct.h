#ifndef _PE_STRUCT_H_
#define _PE_STRUCT_H_

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
}IMAGE_FILE_HEADER;  //20

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
	DWORD Signature;  //4
	IMAGE_FILE_HEADER FileHeader;  //20
	IMAGE_OPTIONAL_HEADER32 OptionalHeader;
}IMAGE_NT_HEADERS;

typedef struct IMAGE_SECTION_HEADER
{
	BYTE Name1[8];  //8
	union{
		DWORD PhysicalAddress;
		DWORD VirtualSize;
	} Misc;   //4
	DWORD VirtualAddress;  //4  
	DWORD SizeOfRawData;   //4
	DWORD PointerToRawData;   //4
	DWORD PointerToRelocations;  //4
	DWORD PointerToLinenumbers;    //4
	WORD NumberOfRelocations;  //2
	WORD NumberOfLinenumbers;  //2
	DWORD Characteristics;  //4
}IMAGE_SECTION_HEADER;  //40


//������ṹ
typedef struct _IMAGE_EXPORT_DIRECTORY
{
	DWORD Characteristics;  //δʹ��
	DWORD TimeDateStamp;  //ʱ���
	WORD MajorVersion;   //δʹ��
	WORD MinorVersion;   //δʹ��
	DWORD Name;  //ָ��õ������ļ������ַ�����RVA
	DWORD Base;  //����������ʼ���
	DWORD NumberOfFunctions; //���е��������ĸ���
	DWORD NumberOfNames;  //�Ժ������ֵ����ĺ�������
	DWORD AddressOfFunctions;  //����������ַ��RVA
	DWORD AddressOfNames;  //�����������Ʊ�RVA
	DWORD AddressOfNameOrdinals; //����������ű�RVA(ע�⣬�����һ��ָ��WORD���͵�����)
}IMAGE_EXPORT_DIRECTORY;

typedef struct _IMAGE_BASE_RELOCATION
{
	DWORD VirtualAddress; //�ض�λ�ڴ�ҳ����ʼRVA
	DWORD Size; //�ض�λ��ĳ���
}IMAGE_BASE_RELOCATION;



typedef IMAGE_DOS_HEADER* PIMAGE_DOS_HEADER;
typedef IMAGE_NT_HEADERS* PIMAGE_NT_HEADERS;
typedef IMAGE_FILE_HEADER* PIMAGE_FILE_HEADER;
typedef IMAGE_OPTIONAL_HEADER32* PIMAGE_OPTIONAL_HEADER32;
typedef IMAGE_SECTION_HEADER* PIMAGE_SECTION_HEADER;
typedef IMAGE_EXPORT_DIRECTORY* PIMAGE_EXPORT_DIRECTORY;
typedef IMAGE_BASE_RELOCATION* PIMAGE_BASE_RELOCATION;

#endif
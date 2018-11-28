// disassembly.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "string.h"
#include "malloc.h"

#include "PEStruct.h"
#include "BufferOperation.h"





int _tmain(int argc, _TCHAR* argv[])
{
	char* inFile = "D:\\CRACKME.EXE";
	//char* inFile = "C:\\Windows\\System32\\notepad.exe";
	char* outFile = "D:\\CRACKME1.EXE";
	//printPEStruct(inFile);

	
	LPVOID fileBuff = readPEFile(inFile);
	LPVOID imageBuff = extendToImageBuff(fileBuff);
	LPVOID fileBuff2 = reduceToFileBuff(imageBuff);
	writePEFile(outFile, fileBuff2);

	free(fileBuff);
	free(imageBuff);
	free(fileBuff2);
	

	getchar();
	return 0;
}


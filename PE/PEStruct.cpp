LPVOID ReadPEFile(LPSTR lpszFile)		
{		
	FILE *pFile = NULL;	
	DWORD fileSize = 0;	
	LPVOID pFileBuffer = NULL;	
		
	//打开文件	
    pFile = fopen(lpszFile, "rb");		
	if(!pFile)	
	{	
		printf(" 无法打开 EXE 文件! ");
		return NULL;
	}	
    //读取文件大小		
	fseek(pFile, 0, SEEK_END);	
    fileSize = ftell(pFile);		
    fseek(pFile, 0, SEEK_SET);		
	//分配缓冲区	
	pFileBuffer = malloc(fileSize);	
		
	if(!pFileBuffer)	
	{	
		printf(" 分配空间失败! ");
		fclose(pFile);
		return NULL;
	}	
	//将文件数据读取到缓冲区	
	size_t n = fread(pFileBuffer, fileSize, 1, pFile);	
	if(!n)	
	{	
		printf(" 读取数据失败! ");
		free(pFileBuffer);
		fclose(pFile);
		return NULL;
	}	
	//关闭文件	
	fclose(pFile);	
    return pFileBuffer;		
}		
		
VOID PrintNTHeaders()		
{		
	LPVOID pFileBuffer = NULL;	
	PIMAGE_DOS_HEADER pDosHeader = NULL;	
	PIMAGE_NT_HEADERS pNTHeader = NULL;	
	PIMAGE_FILE_HEADER pPEHeader = NULL;	
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;	
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;	
		
	pFileBuffer = ReadPEFile(FILEPATH);	
	if(!pFileBuffer)	
	{	
		printf("文件读取失败\n");
		return ; 
	}	
		
	//判断是否是有效的MZ标志	
	if(*((PWORD)pFileBuffer) != IMAGE_DOS_SIGNATURE)	
	{	
		printf("不是有效的MZ标志\n");
		free(pFileBuffer);
		return ; 
	}	
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;	
	//打印DOC头	
	printf("********************DOC头********************\n");	
	printf("MZ标志：%x\n",pDosHeader->e_magic);	
	printf("PE偏移：%x\n",pDosHeader->e_lfanew);	
	//判断是否是有效的PE标志	
	if(*((PDWORD)((DWORD)pFileBuffer+pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE)	
	{	
		printf("不是有效的PE标志\n");
		free(pFileBuffer);
		return ;
	}	
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer+pDosHeader->e_lfanew);	
	//打印NT头	
	printf("********************NT头********************\n");	
	printf("NT：%x\n",pNTHeader->Signature);	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);	
	printf("********************PE头********************\n");	
	printf("PE：%x\n",pPEHeader->Machine);	
	printf("节的数量：%x\n",pPEHeader->NumberOfSections);	
	printf("SizeOfOptionalHeader：%x\n",pPEHeader->SizeOfOptionalHeader);	
	//可选PE头	
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader+IMAGE_SIZEOF_FILE_HEADER);	
	printf("********************OPTIOIN_PE头********************\n");	
	printf("OPTION_PE：%x\n",pOptionHeader->Magic);	
	//释放内存	
	free(pFileBuffer);	
}		

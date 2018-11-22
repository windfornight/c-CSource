//二进制读写文件（exe）
char * readAndWriteFile(const char* inFile, const char* outFile)
{
	FILE* file = fopen(inFile, "rb");
	if(!file){
		printf("file open error!");
		return NULL;
	}
	fseek(file, 0, SEEK_END); //文件指针移动到末尾，正常返回0
	long int cnt = ftell(file); //字节个数
	char *ptBuff = (char *)malloc(sizeof(char) * cnt);
	if(!ptBuff){
		printf("assign buff error!");
		return NULL;
	}
	fseek(file, 0, SEEK_SET);
	
	char* pt = ptBuff;
	char ch;
	//二进制数据，不能以文本的形式读取,一次读取一个字节
	while(fread(pt, sizeof(char), 1, file)){
		pt++;
	}

	/*
	for(int i = 0; i < cnt; ++i)
	{
		if(i % 16 == 0){
			printf("\n");
			printf("%08x ", i);
		}
		unsigned char ch = *(ptBuff+i);
		printf("%02x ", ch);
	}
	*/

	fclose(file);
	file = NULL;

	FILE* fileWrite = fopen(outFile, "wb");
	//必须以二进制的形式写"wb", 否则，fwrite函数写是有问题的，0A（换行的时候）会变成0D 0A，
	for(int i = 0; i < cnt; ++i)
	{	
		char ch = *(ptBuff+i);
		int res = fwrite(&ch, sizeof(char), 1, fileWrite);
		//fflush(fileWrite);
	}
	fclose(fileWrite);
	fileWrite = NULL;
	return ptBuff;
}
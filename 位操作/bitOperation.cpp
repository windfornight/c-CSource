//将数值某一位值设为0， 个位算作第一位
unsigned char setZero(unsigned char a, int flag)
{
	unsigned res = 1;
	res = (res << (flag - 1));
	res = (~res);
	a = (a & res);
	return a;
}

//将数值某一位设为1， 个位算作第二位
unsigned char setOne(unsigned char a, int flag)
{
	unsigned res = 1;
	res = (res << (flag - 1));
	a = (a | res);
	return a;
}
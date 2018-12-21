// CreateDllDemo2.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "CreateDllDemo2.h"


// This is an example of an exported variable
CREATEDLLDEMO2_API int nCreateDllDemo2=0;

// This is an example of an exported function.
CREATEDLLDEMO2_API int fnCreateDllDemo2(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see CreateDllDemo2.h for the class definition
CCreateDllDemo2::CCreateDllDemo2()
{
	return;
}

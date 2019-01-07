#ifndef _TOOL_H_
#define _TOOL_H_

void __cdecl OutputDebugStringF(const char *format, ...); 

#ifdef _DEBUG  
#define DbgPrintf   OutputDebugStringF  
#else  
#define DbgPrintf  
#endif 

#endif
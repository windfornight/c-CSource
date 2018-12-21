// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CREATEDLLDEMO2_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CREATEDLLDEMO2_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef CREATEDLLDEMO2_EXPORTS
#define CREATEDLLDEMO2_API __declspec(dllexport)
#else
#define CREATEDLLDEMO2_API __declspec(dllimport)
#endif

// This class is exported from the CreateDllDemo2.dll
class CREATEDLLDEMO2_API CCreateDllDemo2 {
public:
	CCreateDllDemo2(void);
	// TODO: add your methods here.
};

extern CREATEDLLDEMO2_API int nCreateDllDemo2;

extern "C" CREATEDLLDEMO2_API int fnCreateDllDemo2(void);

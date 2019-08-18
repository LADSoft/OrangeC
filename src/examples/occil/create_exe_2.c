#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMALIGN  0x1000 	
#define RAWALIGN  0x0200 	

typedef struct _EXE {
	int typeApp;
	int sizeImptArea;
	int sizeDataArea;
	int sizeCodeSection;
	int sizeDataSection;	// sizeImptArea + sizeDataArea
	int posCodeSection;
	int posDataSection;
	int entryPoint;
	int useDLL;			
	int useFunc;		
} EXE;
EXE exe;

typedef struct _EXE_HEADER {
	IMAGE_DOS_HEADER      DosHeader;	// 64
	BYTE 		  DosStub[64];
	DWORD                 Signature;	// "PE\0\0"
	IMAGE_FILE_HEADER     FileHeader;
	IMAGE_OPTIONAL_HEADER OptionalHeader;
	IMAGE_SECTION_HEADER  SectionHeaders[3];
} EXE_HEADER;	// 0x1F0 ( < RAWALIGN )

typedef struct _DLL {
	char *dllname;	
	char *FuncName[16];	
	int  nFunc;		
	int  FuncAddr[16];  
} DLL;

DLL dll[] = {
	{ "user32.dll", { "MessageBoxA" }, 1 },
	{ "kernel32.dll", { "ExitProcess" }, 1 },
};

void error(char *msg)
{
	printf("%s", msg);
	exit(1);
}

BYTE *getImport() {
	int nLookup, nImptAddr, nName;	
	int n, k, sizeImpt = 0;
	DWORD *pImpt;

	exe.useFunc = 0;
	exe.useDLL = sizeof(dll) / sizeof(DLL);
	for (n = 0; n < exe.useDLL; n++) {
		sizeImpt += strlen(dll[n].dllname) + 1;			// DLL
		for (k = 0; k < dll[n].nFunc; k++)
			sizeImpt += strlen(dll[n].FuncName[k]) + 3;	
		exe.useFunc += dll[n].nFunc;
	}
	sizeImpt += (exe.useDLL + 1) * 20 + (exe.useDLL + exe.useFunc) * 4 * 2;
	sizeImpt = ((sizeImpt - 1) / 16 + 1) * 16;	

	pImpt = calloc(sizeImpt, 1);
	if (pImpt == NULL) 
		error("Fail to allocate memory for pImpt");
	nLookup = (exe.useDLL + 1) * 5;					// DWORD
	nImptAddr = nLookup + (exe.useDLL + exe.useFunc);		// DWORD
	nName = (nImptAddr + (exe.useDLL + exe.useFunc)) * 4;	// BYTE
	for (n = 0; n < exe.useDLL; n++) {
		pImpt[n * 5] = exe.posDataSection + nLookup * 4;		// LookupTable
		pImpt[n * 5 + 4] = exe.posDataSection + nImptAddr * 4;	// ImptAddrTable
		for (k = 0; k < dll[n].nFunc; k++) {
			dll[n].FuncAddr[k] = pImpt[n * 5 + 4] + k * 4;
			pImpt[nLookup++] = exe.posDataSection + nName;
			pImpt[nImptAddr++] = exe.posDataSection + nName;
			strcpy((char*)pImpt + nName + 2, dll[n].FuncName[k]);	
			nName += strlen(dll[n].FuncName[k]) + 3;
		}
		nLookup++;	
		nImptAddr++;
		pImpt[n * 5 + 3] = exe.posDataSection + nName;		
		strcpy((char*)pImpt + nName, dll[n].dllname);		
		nName += strlen(dll[n].dllname) + 1;
	}
	exe.sizeImptArea = sizeImpt;
	return (BYTE*)pImpt;
}

void genExe(BYTE *bufImport) {
	BYTE *bufEXE;
	FILE *fpDst;

	int posImptAddrTable = exe.posDataSection
		+ (exe.useDLL + 1) * 20 + (exe.useDLL + exe.useFunc) * 4;
	int sizeImptAddrTable = (exe.useDLL + exe.useFunc) * 4;

	
	int IMPT = exe.sizeImptArea;
	int MSG = 0x4C;
	int EXIT = 0x54;
	BYTE CodeBuffer[] = {
		0x6A,0x00,                        // push 0
		0x68,  IMPT + 0x0D,0x20,0x40,0x00,  // push dword ptr["exetest"]
		0x68,  IMPT,0x20,0x40,0x00,       // push dword ptr["Hello World!"]
		0x6A,0x00,                        // push 0
		0xFF,0x15,  MSG,0x20,0x40,0x00,   // call MessageBoxA  ;user32.dll
		0x6A,0x00,                        // push 0
		0xFF,0x15,  EXIT,0x20,0x40,0x00,  // call ExitProcess  ;kernel32.dll
	};
	
	BYTE DataBuffer[] = "Hello World!\0exe test\0";

	int lenDataSection = exe.sizeImptArea + sizeof(DataBuffer);
	int sizeImage = MEMALIGN * 3;
	EXE_HEADER exeHeader = {
		{ 	// IMAGE_DOS_HEADER
			0x5A4D, 0x90, 3, 0, 4,                // "MZ", ...
		0, 0xFFFF, 0, 0xB8,	                  // min-, maxalloc, ss, sp
			0,0,0, 0x40, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0x0080,                              
		},
	"\x0E\x1F\xBA\x0E\x00\xB4\x09\xCD\x21\xB8\x01\x4C\xCD\x21"
		  "This program cannot be run in DOS mode.\r\r\n$",	// DosStub
		0x00004550,				  // "PE\0\0"
	{   // IMAGE_FILE_HEADER;
		0x014c, 2, 0, 0, 0,			  // Intel 386
			sizeof(IMAGE_OPTIONAL_HEADER),0x030F  // 0x00E0, Characteristics
		}, {   // IMAGE_OPTIONAL_HEADER
		0x010B, 0x06, 0x00,			  // Magic, Linker Version
		0, 0, 0, exe.entryPoint, 		  // AddressOfEntryPoint
		exe.posCodeSection, exe.posDataSection,	  // Base of Code, Data.
		0x00400000,				  // ImageBase
		MEMALIGN, RAWALIGN,			  // Section-, File-Alignment
		4, 0,  0, 0,  4, 0,	 0,		  // OS, Image, Subsystem Version
		sizeImage, MEMALIGN, 0,		  // Image size, Header size, ...
		exe.typeApp, 0,			  // 2/3: gui/console, DllCharacteristics
		0x100000, 0x1000, 0x100000, 0x1000,	  // stack & heap reserve, commit
		0, 0x10,
			{ {0,0}, {exe.posDataSection, exe.sizeDataSection},                     //  1
			{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
		{posImptAddrTable, sizeImptAddrTable},{0,0},{0,0},{0,0} }	    // 12
		}, { 	// IMAGE_SECTION_HEADER
		{        
				".text\0\0", {sizeof(CodeBuffer)}, exe.posCodeSection, RAWALIGN,
			RAWALIGN, 0, 0, 0, 0, 0x60000020  // Code | Executable | Readable
			}, {    
				".data\0\0", {lenDataSection}, exe.posDataSection, RAWALIGN,
			RAWALIGN * 2, 0,0,0,0, 0xC0000040   // InitializedData|Readable|Writeable
		}
	}
	};
	bufEXE = calloc(RAWALIGN * 3, sizeof(BYTE));
	if (bufEXE == NULL)
		error("Fail to alloc memory for bufEXE");
	memcpy(bufEXE, &exeHeader, sizeof(exeHeader));
	memcpy(bufEXE + RAWALIGN, &CodeBuffer, sizeof(CodeBuffer));
	memcpy(bufEXE + RAWALIGN * 2, bufImport, exe.sizeImptArea);
	memcpy(bufEXE + RAWALIGN * 2 + exe.sizeImptArea, DataBuffer, sizeof(DataBuffer));

	if ((fpDst = fopen("created_exe_2.exe", "wb")) == NULL)
		error("Fail to open 'created_exe2.exe' for write");
	fwrite(bufEXE, RAWALIGN * 3, 1, fpDst);
	free(bufImport);
	fclose(fpDst);
	free(bufEXE);
}

int main() {
	int  n, k;
	BYTE *pImport;

	exe.typeApp = 2;
	exe.sizeCodeSection = MEMALIGN;
	exe.sizeDataSection = MEMALIGN;
	exe.posCodeSection = MEMALIGN;
	exe.posDataSection = MEMALIGN * 2;
	exe.entryPoint = exe.posCodeSection;
	pImport = getImport();
	genExe(pImport);
//	free(pImport);
	for (n = 0; n < exe.useDLL; n++)
		for (k = 0; k < dll[n].nFunc; k++)
			printf("%X %s\n", dll[n].FuncAddr[k], dll[n].FuncName[k]);
	return 0;
}

#include <windows.h>
#include <stdio.h>
#include <string.h>

#define ALIGNMENT  0x1000 

BYTE NativeBuffer[]={
    0x6A,0x00,                         // push 0
    0x68,  0x0D,0x30,0x40,0x00,        // push dword ptr["exetest"]
    0x68,  0x00,0x30,0x40,0x00,        // push dword ptr["Hello World!"]
    0x6A,0x00,                         // push 0
    0xFF,0x15,  0x4E,0x20,0x40,0x00,   // call MessageBoxA
    0xC3                               // ret
};

#define USE_DLL 1
char szDllName[16] = "user32.dll";
DWORD LookupTable[2];
char HintTable[] = "\0\0MessageBoxA";

BYTE InitBuffer[]="Hello World!\0exe test\0";

BYTE bufEXE[ALIGNMENT*4];

int main() {
    FILE *fpDst;
    int offset;

    int sizeCodeSection   = ALIGNMENT;         // sizeof(NativeBuffer) <= ALIGNMENT
    int sizeImportSection = ALIGNMENT;
    int sizeDataSection   = ALIGNMENT;         // sizeof(InitBuffer) <= ALIGNMENT
    int posCodeSection    = ALIGNMENT;
    int posImportSection  = posCodeSection + sizeCodeSection;
    int posDataSection    = posImportSection + sizeImportSection;

    int sizeIID = (USE_DLL+1)*sizeof(IMAGE_IMPORT_DESCRIPTOR);
    int originalFirstThunk = posImportSection + sizeIID + sizeof(szDllName);
    IMAGE_IMPORT_DESCRIPTOR imptDesc[2] = {
        { {originalFirstThunk}, 0, 0, posImportSection + sizeIID,
        (originalFirstThunk + sizeof(LookupTable) + sizeof(HintTable)) }
    };

    IMAGE_DOS_HEADER dosHeader = { 	// 0x0000..0x003F   64 byte
    	0x5A4D,	0x0090, 0x0003, 0, 4,	// "MZ"
	0x0000, 0xFFFF, 0x0000, 0x00B8,	// minalloc, maxalloc, ss, sp
    	0, 0, 0, 0x40, 0, 0,0,0,0, 0, 0, 0,0,0,0,0, 0,0,0,0,0,
    	0x0080, 	// PE
    };
    IMAGE_NT_HEADERS peHeader = {	// 0x0080..0x0177
        0x00004550,				// "PE\0\0"
	{   // IMAGE_FILE_HEADER;
	    0x014c, 3, 0, 0, 0,			// Intel 386
            sizeof(IMAGE_OPTIONAL_HEADER),	// 0x00E0
    	    0x030F				// Characteristics
        }, 
	{   // IMAGE_OPTIONAL_HEADER32
	    0x010B, 0x06, 0x00,			// Magic, Linker Version
	    0, 0, 0, posCodeSection, 		// AddressOfEntryPoint
	    posCodeSection, posDataSection,	// Base of Code, Data
	    0x00400000,				// ImageBase
	    ALIGNMENT, ALIGNMENT,		// Section-, File-Alignment
	    4, 0,  0, 0,  4, 0,	 0,		// OS, Image, Subsystem Version
	    posDataSection + sizeDataSection,   // Image size
            ALIGNMENT, 0,		        // Header size
	    2, 0,				// 2/3: gui/console, DllCharacteristics
	    0x100000, 0x1000, 0x100000, 0x1000,	// stack & heap reserve, commit
	    0, 0x10,
            { {0,0}, {posImportSection, sizeImportSection},                        // 1
            {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	    {posImportSection+(USE_DLL+1)*sizeof(IMAGE_IMPORT_DESCRIPTOR)+
    	     16*USE_DLL+sizeof(LookupTable)+sizeof(HintTable),sizeof(LookupTable)},// 12
            {0,0}, {0,0}, {0,0} }
        }
    };  //-------------- peHeader----------------

    IMAGE_SECTION_HEADER header[] = {
	{        
            ".text\0\0", {sizeof(NativeBuffer)}, posCodeSection, sizeCodeSection, 
	    posCodeSection, 0, 0, 0, 0, 0x60000020  // Code | Executable | Readable
        }, {    
            ".idata\0\0", {sizeImportSection}, posImportSection, sizeImportSection, 
	    posImportSection, 0, 0, 0, 0, 0x40000040  // Initialized Data | Readable
        }, {    
            ".sdata\0\0", {sizeof(InitBuffer)}, posDataSection, sizeDataSection, 
	    posDataSection, 0,0,0,0, 0xC0000040  // InitializedData|Readable|Writeable
	}
    };

    LookupTable[0] = posImportSection + sizeIID+sizeof(szDllName)+sizeof(LookupTable);

    memcpy(bufEXE, &dosHeader, sizeof(dosHeader));
    memcpy(bufEXE+0x80, &peHeader, sizeof(peHeader));
    memcpy(bufEXE+0x80+sizeof(peHeader), header, sizeof(header));
    memcpy(bufEXE+ALIGNMENT, &NativeBuffer, sizeof(NativeBuffer));
    memcpy(bufEXE+ALIGNMENT*2, imptDesc, sizeof(imptDesc));
    offset = ALIGNMENT*2+sizeof(imptDesc);
    memcpy(bufEXE+offset, szDllName, sizeof(szDllName));
    offset += sizeof(szDllName);
    memcpy(bufEXE+offset, LookupTable, sizeof(LookupTable));
    offset += sizeof(LookupTable);
    memcpy(bufEXE+offset, HintTable, sizeof(HintTable));
    offset += sizeof(HintTable);
    memcpy(bufEXE+offset, LookupTable, sizeof(LookupTable));
    memcpy(bufEXE+ALIGNMENT*3, InitBuffer, sizeof(InitBuffer));
    if ((fpDst = fopen("created_exe_1.exe","wb")) == NULL) {
        printf("Fail to create the file lala.exe");
        exit(1);
    }
    fwrite(bufEXE, sizeof(bufEXE), 1, fpDst);
    fclose(fpDst);
    return 0;
}

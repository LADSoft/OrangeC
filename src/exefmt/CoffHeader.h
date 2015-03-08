/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/
#ifndef CoffHeader_h
#define CoffHeader_h

#define IMAGE_FILE_MACHINE_UNKNOWN 0
#define IMAGE_FILE_MACHINE_I386 0x14c

#define IMAGE_FILE_EXECUTABLE_IMAGE 2
#define IMAGE_FILE_32BIT_MACHINE 0x100
#define IMAGE_FILE_DLL 0x2000
#pragma pack(1)
struct CoffHeader
{
    short Machine;
    short NumberOfSections;
    int TimeDateStamp;
    int PointerToSymbolTable;
    int NumberOfSymbols;
    short SizeOfOptionalHeader;
    short Characteristics;
};

#define IMAGE_SCN_TYPE_NO_PAD               0x00000008
#define IMAGE_SCN_CNT_CODE	                0x00000020
#define IMAGE_SCN_CNT_INITIALIZED_DATA	    0x00000040
#define IMAGE_SCN_CNT_UNINITIALIZED_DATA	0x00000080
#define IMAGE_SCN_LNK_OTHER	                0x00000100
#define IMAGE_SCN_LNK_INFO	                0x00000200
#define IMAGE_SCN_LNK_REMOVE	            0x00000800
#define IMAGE_SCN_LNK_COMDAT	            0x00001000
#define IMAGE_SCN_GPREL	                    0x00008000
#define IMAGE_SCN_MEM_PURGEABLE	            0x00020000
#define IMAGE_SCN_MEM_16BIT	                0x00020000
#define IMAGE_SCN_MEM_LOCKED	            0x00040000
#define IMAGE_SCN_MEM_PRELOAD	            0x00080000
#define IMAGE_SCN_ALIGN_1BYTES	            0x00100000
#define IMAGE_SCN_ALIGN_2BYTES	            0x00200000
#define IMAGE_SCN_ALIGN_4BYTES	            0x00300000
#define IMAGE_SCN_ALIGN_8BYTES	            0x00400000
#define IMAGE_SCN_ALIGN_16BYTES	            0x00500000
#define IMAGE_SCN_ALIGN_32BYTES	            0x00600000
#define IMAGE_SCN_ALIGN_64BYTES	            0x00700000
#define IMAGE_SCN_ALIGN_128BYTES	        0x00800000
#define IMAGE_SCN_ALIGN_256BYTES	        0x00900000
#define IMAGE_SCN_ALIGN_512BYTES	        0x00A00000
#define IMAGE_SCN_ALIGN_1024BYTES	        0x00B00000
#define IMAGE_SCN_ALIGN_2048BYTES	        0x00C00000
#define IMAGE_SCN_ALIGN_4096BYTES	        0x00D00000
#define IMAGE_SCN_ALIGN_8192BYTES	        0x00E00000
#define IMAGE_SCN_ALIGN(x)                  (1 << (((x) >> 20)-1))
#define IMAGE_SCN_LNK_NRELOC_OVFL	        0x01000000
#define IMAGE_SCN_MEM_DISCARDABLE	        0x02000000
#define IMAGE_SCN_MEM_NOT_CACHED	        0x04000000
#define IMAGE_SCN_MEM_NOT_PAGED	            0x08000000
#define IMAGE_SCN_MEM_SHARED	            0x10000000
#define IMAGE_SCN_MEM_EXECUTE	            0x20000000
#define IMAGE_SCN_MEM_READ	                0x40000000
#define IMAGE_SCN_MEM_WRITE	                0x80000000

struct CoffSection
{
    char Name[8];
    int VirtualSize;
    int VirtualAddress;
    int SizeOfRawData;
    int PointerToRawData;
    int PointerToRelocations;
    int PointerToLinenumbers;
    short NumberOfRelocations;
    short NumberOfLinenumbers;
    int Characteristics;
} ;
#define IMAGE_COMDAT_SELECT_NODUPLICATES	1	
#define IMAGE_COMDAT_SELECT_ANY	            2
#define IMAGE_COMDAT_SELECT_SAME_SIZE	    3	
#define IMAGE_COMDAT_SELECT_EXACT_MATCH	    4	
#define IMAGE_COMDAT_SELECT_ASSOCIATIVE	    5
#define IMAGE_COMDAT_SELECT_LARGEST	        6	
struct CoffSectionAux
{
    int Length;
    short NumberOfRelocations;
    short NumberOfLinenumbers;
    int CheckSum;
    short Number;
    char Selection;
    char unused[3];
};

#define IMAGE_REL_I386_ABSOLUTE	0x0000	
#define IMAGE_REL_I386_DIR32	0x0006	
#define IMAGE_REL_I386_DIR32NB	0x0007	
#define IMAGE_REL_I386_SECTION	0x000A	
#define IMAGE_REL_I386_SECREL	0x000B	
#define IMAGE_REL_I386_TOKEN	0x000C	
#define IMAGE_REL_I386_SECREL7	0x000D	
#define IMAGE_REL_I386_REL32	0x0014	

struct CoffReloc
{
    int VirtualAddress;
    int SymbolTableIndex;
    short Type;
};

#define IMAGE_SYM_UNDEFINED	        0
#define IMAGE_SYM_ABSOLUTE	        -1
#define IMAGE_SYM_DEBUG	            -2

#define IMAGE_SYM_CLASS_EXTERNAL	2
#define IMAGE_SYM_CLASS_STATIC	    3
#define IMAGE_SYM_CLASS_LABEL       6
#define IMAGE_SYM_CLASS_FUNCTION	101
#define IMAGE_SYM_CLASS_FILE	    103
#define IMAGE_SYM_CLASS_SECTION     104

struct CoffSymbol
{
    char Name[8];
    int Value;
    short SectionNumber;
    short Type;
    char StorageClass;
    char NumberOfAuxSymbols;
};

#define COFF_LIB_SIGNATURE "!<arch>\n"
#define COFF_LIB_END_OF_HEADER "`\n"

struct CoffLinkerMemberHeader
{
    char Name[16];
    char Seconds[12];
    char User[6];
    char Group[6];
    char Mode[8];
    char Size[10];
    char EndOfHeader[2];
};
struct CoffLinkerSectionOneHeader
{
    unsigned  NumberOfSymbols;
    unsigned  OffsetArray[1];
};
#define IMPORT_ORDINAL	        0
#define IMPORT_NAME	            1
#define IMPORT_NAME_NOPREFIX	2
#define IMPORT_NAME_UNDECORATE	3

struct CoffImportHeader
{
    unsigned short Sig1;
    unsigned short Sig2;
    unsigned short Version;
    unsigned short MachineType;
    unsigned Date;
    unsigned Size;
    unsigned short OrdinalOrHint;
    unsigned short ImportType : 2;
    unsigned short ImportNameType : 3;
    unsigned short :11;
};
#pragma pack()
#endif
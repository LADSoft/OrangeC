/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
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

*/
#ifndef PEHeader_H
#define PEHeader_H

#define PESIG 'PE'

#define PE_BASE_HEADER_SIZE     0x18
#define PE_OPTIONAL_HEADER_SIZE 0xe0
#define PE_OBJECTENTRY_SIZE     0x28
#define PE_HEADBUF_SIZE         (PE_BASE_HEADER_SIZE+PE_OPTIONAL_HEADER_SIZE)
#define PE_IMPORTDIRENTRY_SIZE  0x14
#define PE_NUM_VAS              0x10
#define PE_EXPORTHEADER_SIZE    0x28
#define PE_RESENTRY_SIZE        0x08
#define PE_RESDIR_SIZE          0x10
#define PE_RESDATAENTRY_SIZE    0x10
#define PE_SYMBOL_SIZE          0x12
#define PE_RELOC_SIZE           0x0a

#define PE_ORDINAL_FLAG    0x80000000
#define PE_INTEL386        0x014c
#define PE_MAGICNUM        0x010b
#define PE_FILE_EXECUTABLE 0x0002
#define PE_FILE_32BIT      0x0100
#define PE_FILE_LIBRARY    0x2000
#define PE_FILE_REVERSE_BITS_HIGH 0x8000
#define PE_FILE_REVERSE_BITS_LOW  0x80
#define PE_FILE_LOCAL_SYMBOLS_STRIPPED 8
#define PE_FILE_LINE_NUMBERS_STRIPPED 4

#define PE_REL_LOW16 0x2000
#define PE_REL_OFS32 0x3000

#define PE_SUBSYS_NATIVE  1
#define PE_SUBSYS_WINDOWS 2
#define PE_SUBSYS_CONSOLE 3
#define PE_SUBSYS_POSIX   7

#define WINF_UNDEFINED   0x00000000
#define WINF_CODE        0x00000020
#define WINF_INITDATA    0x00000040
#define WINF_UNINITDATA  0x00000080
#define WINF_DISCARDABLE 0x02000000
#define WINF_NOPAGE      0x08000000
#define WINF_SHARED      0x10000000
#define WINF_EXECUTE     0x20000000
#define WINF_READABLE    0x40000000
#define WINF_WRITEABLE   0x80000000
#define WINF_ALIGN_NOPAD 0x00000008
#define WINF_ALIGN_BYTE  0x00100000
#define WINF_ALIGN_WORD  0x00200000
#define WINF_ALIGN_DWORD 0x00300000
#define WINF_ALIGN_8     0x00400000
#define WINF_ALIGN_PARA  0x00500000
#define WINF_ALIGN_32    0x00600000
#define WINF_ALIGN_64    0x00700000
#define WINF_ALIGN       (WINF_ALIGN_64)
#define WINF_COMMENT     0x00000200
#define WINF_REMOVE      0x00000800
#define WINF_COMDAT      0x00001000
#define WINF_NEG_FLAGS   (WINF_DISCARDABLE | WINF_NOPAGE)
#define WINF_IMAGE_FLAGS 0xfa0008e0

#define WIN32_DEFAULT_BASE              0x00400000
#define WIN32_DEFAULT_FILEALIGN         0x00000200
#define WIN32_DEFAULT_OBJECTALIGN       0x00001000
#define WIN32_DEFAULT_STACKSIZE         0x00100000
#define WIN32_DEFAULT_STACKCOMMITSIZE   0x00001000
#define WIN32_DEFAULT_HEAPSIZE          0x00100000
#define WIN32_DEFAULT_HEAPCOMMITSIZE    0x00001000
#define WIN32_DEFAULT_SUBSYS            PE_SUBSYS_WINDOWS
#define WIN32_DEFAULT_SUBSYSMAJOR       4
#define WIN32_DEFAULT_SUBSYSMINOR       0
#define WIN32_DEFAULT_OSMAJOR           1
#define WIN32_DEFAULT_OSMINOR           0

#define PE_FIXUP_ABSOLUTE 0
#define PE_FIXUP_HIGH 1
#define PE_FIXUP_LOW 2
#define PE_FIXUP_HIGHLOW 3
#define PE_FIXUP HIGHADJUST 4

struct PEHeader
{
    int signature;
    short cpu_type;
    short num_objects;
    int time;
    int symbol_ptr;
    int num_symbols;
    short nt_hdr_size;
    short flags;
    short magic;
    unsigned char linker_minor_version;
    unsigned char linker_major_version;
    int code_size;
    int data_size;
    int bss_size;
    int entry_point;
    int code_base;
    int data_base;
    int image_base;
    int object_align;
    int file_align;
    short os_major_version;
    short os_minor_version;
    short user_major_version;
    short user_minor_version;
    short subsys_major_version;
    short subsys_minor_version;
    int uu_1;
    int image_size;
    int header_size;
    int chekcsum;
    short subsystem;
    short dll_flags;
    int stack_size;
    int stack_commit;
    int heap_size;
    int heap_commit;
    int loader_flags;
    int num_rvas;
    int export_rva;
    int export_size;
    int import_rva;
    int import_size;
    int resource_rva;
    int resource_size;
    int exception_rva;
    int exception_size;
    int security_rva;
    int security_size;
    int fixup_rva;
    int fixup_size;
    int debug_rva;
    int debug_size;
    int desc_rva;
    int desc_size;
    int mspec_rva;
    int mspec_size;
    int tls_rva;
    int tls_size;
    int loadconfig_rva;
    int loadconfig_size;
    int boundimp_rva;
    int boundimp_size;
    int iat_rva;
    int iat_size;
    int res1_rva, res1_size;
    int res2_rva, res2_size;
    int res3_rva, res3_size;
};
struct PEImport {
    unsigned thunkPos2 ;
    unsigned time ;
    unsigned version ;
    unsigned dllName ;
    unsigned thunkPos ;
} ;

#endif
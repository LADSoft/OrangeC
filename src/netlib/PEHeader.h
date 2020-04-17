/* Software License Agreement
 * 
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#ifndef PEHeader_H
#define PEHeader_H

#define PESIG 0x4550 // PE

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

namespace DotNetPELib {

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
        unsigned char linker_major_version;
        unsigned char linker_minor_version;
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
        int delay_imports_rva, delay_imports_size;
        int com_rva, com_size;
        int res3_rva, res3_size;
    };

#ifndef PEHEADER_ONLY
    struct PEObject
    {
        char name[8];
        int virtual_size;
        int virtual_addr;
        int raw_size;
        int raw_ptr;
        int reserved[3];
        int flags;
    };

    struct PEResourceHeader
    {
        int datasize;
        int hdrsize;
    };

    struct PEResourceDirTable
    {
        int resource_flags;
        int time;
        int version;
        short name_entry;
        short ident_entry;
    };

    struct PEResourceDirEntry
    {
        int rva_or_id;
        int subdir_or_data : 31;
        int escape : 1;
    };

    struct PEResourceDataEntry
    {
        int rva;
        int size;
        int codepage;
        int reserved;
    };

    struct PEExportHeader
    {
        int flags;
        int time;
        int version;
        int exe_name_rva;
        int ord_base;
        int n_eat_entries;
        int n_name_ptrs;
        int address_rva;
        int name_rva;
        int ordinal_rva;
    };

    struct PEFixupHold
    {
        int offset;
        unsigned char type;
    };

    struct PEFixupBlock
    {
        int rva;
        int size;
        short data[2048];
    };

    struct PEImportDir
    {
        int thunkPos2; // address thunk
        int time;
        int version;
        int dllName;
        int thunkPos; // name thunk
    };

    struct PEImportLookup
    {
        int ord_or_rva : 31;
        int import_by_ordinal : 1;
    };

    struct DotNetCOR20Header
    {
        unsigned cb;
        unsigned short MajorRuntimeVersion;
        unsigned short MinorRuntimeVersion;
        unsigned MetaData[2];
        unsigned Flags;
        unsigned EntryPointToken;
        unsigned Resources[2];
        unsigned StrongNameSignature[2];
        unsigned CodeManagerTable[2];
        unsigned VTableFixup[2];
        unsigned ExportAddressTableJumps[2];
        unsigned ManagedNativeHeader[2];
    };
    struct DotNetMetaHeader
    {
#define META_SIG 0x424A5342
        unsigned Signature;
        unsigned short Major;
        unsigned short Minor;
        unsigned Reserved;
    };
    struct DotNetMetaTablesHeader
    {
        unsigned Reserved1;
        unsigned char MajorVersion;
        unsigned char MinorVersion;
        unsigned char HeapOffsetSizes;
        unsigned char Reserved2;
        longlong MaskValid;
        longlong MaskSorted;
    };

    struct FixedVersionInfo
    {
        unsigned Signature;
        unsigned StrucVersion;
        unsigned FileVersionMS;
        unsigned FileVersionLS;
        unsigned ProductVersionMS;
        unsigned ProductVersionLS;
        unsigned FileFlagsMask;
        unsigned FileFlags;
        unsigned FileOS;
        unsigned FileType;
        unsigned FileSubtype;
        unsigned FileDateMS;
        unsigned FileDateLS;
    };
#endif
} // namespace
#endif

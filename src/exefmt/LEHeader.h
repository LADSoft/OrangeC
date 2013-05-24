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
#ifndef LEHeader_h
#define LEHeader_h

#define LE_SIGNATURE 0x454C /* LE */
#define LX_SIGNATURE 0x584C /* LX */

#define LX_CPU_286 1
#define LX_CPU_386 2
#define LX_CPU_486 3

#define LX_OS_OS2 1
#define LX_OS_DOS 3

#define LX_MF_INTERNALFIXUPSAPPLIED   0x10
#define LX_MF_EXTERNALFIXUPSAPPLIED   0x20
#define LX_MF_PMWINDOWINGCOMPATIBLE   0x200

#define LX_OF_READABLE   1
#define LX_OF_WRITEABLE  2
#define LX_OF_EXECUTABLE 4
#define LX_OF_RESOURCE 8
#define LX_OF_DISCARDABLE 0x10
#define LX_OF_SHARED 0x20
#define LX_OF_PRELOAD 0x40
#define LX_OF_INVALID 0x80
#define LX_OF_ZEROFILL 0x100
#define LX_OF_ALIASNEEDED 0x1000
#define LX_OF_BIGDEFAULT 0x2000
#define LX_OF_CONFORMINGCODE 0x4000
#define LX_OF_IOPRIV 0x8000


#define LE_OPF_ENUMERATED 0
#define LE_OPF_ITERATED 0x40
#define LE_OPF_INVALID 0x80
#define LE_OPF_ZERO 0xc0
#define LE_OPF_LASTPAGE 0x3

#define LX_OPF_ENUMERATED 0
#define LX_OPF_ITERATED 1
#define LX_OPF_INVALID 2
#define LX_OPF_ZERO 3
#define LX_OPF_RANGE 4

#define LX_FM_SOURCE_MASK 15
#define LX_FM_BYTE 0
#define LX_FM_SEL16 2
#define LX_FM_SEL16_PTR16 3
#define LX_FM_OFFSET16 5
#define LX_FM_SEL16_PTR32 6
#define LX_FM_OFFSET32 7
#define LX_FM_SELFREL32 8

#define LX_FM_FIXUPTOALIAS 0x10
#define LX_FM_SOURCELIST 0x20

#define LX_FF_TARGETTYPE_MASK 3

#define LX_FT_INTERNAL 0
#define LX_FT_IMPORTORDINAL 1
#define LX_FT_IMPORTNAME 2
#define LX_FT_IMPORTENTRY 3

#define LX_FF_ADDITIVE 4
#define LX_FF_TARGET32 0x10
#define LX_FF_ADDITIVE32 0x20
#define LX_FF_OBJMODORD16 0x40
#define LX_FF_ORD16 0x80

struct LEHeader
{
   unsigned short sig ;
   unsigned char  bbyte ;
   unsigned char  wword ;
   unsigned format_level ;
   unsigned short cpu_level ;
   unsigned short os_type ;
   unsigned module_version ;
   unsigned module_flags ;
   unsigned module_page_count ;
   unsigned eip_object ;
   unsigned eip ;
   unsigned esp_object ;
   unsigned esp ;
   unsigned page_size ;
   unsigned page_offset_shift ; /* for LE, this is bytes on last page */
   unsigned fixup_section_size ;
   unsigned fixup_section_checksum ;
   unsigned loader_section_size ;
   unsigned loader_section_checksum ;
   unsigned object_table_offset ;
   unsigned object_count ;
   unsigned object_page_table_offset ;
   unsigned object_iter_pages_offset ;
   unsigned resource_table_offset ;
   unsigned resource_table_entries ;
   unsigned resident_name_table_offset ;
   unsigned resident_name_table_entries ; /* le - entry table offset */
   unsigned module_directives_offset ;
   unsigned module_directives_count ;
   unsigned fixup_page_table_offset ;
   unsigned fixup_record_table_offset ;
   unsigned import_module_table_offset ;
   unsigned import_module_table_entries ;
   unsigned import_proc_table_offset ;
   unsigned per_page_checksum_offset ;
   unsigned data_pages_offset ;
   unsigned preload_pages_count ;
   unsigned nonresident_name_table_offset ;
   unsigned nonresident_name_table_length ;
   unsigned nonresident_name_table_checksum ;
   unsigned auto_ds_object ;
   unsigned debug_info_offset ;
   unsigned debug_info_length ;
   unsigned instance_preload_count ;
   unsigned instance_demand ;
   unsigned heapsize ;
   unsigned reserved[6] ;
};
#endif

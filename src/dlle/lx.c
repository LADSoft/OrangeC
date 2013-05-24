
/* 
VALX linker
Copyright 1997-2006 David Lindauer.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Original 16-bit linker written by:
David Troendle

You may contact the author of this derivative at:
    mailto::camille@bluegrass.net
 */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <dos.h>

#include "langext.h"
#include "defines.h"
#include "types.h"
#include "subs.h"
#include "globals.h"
/*+-------------------------------------------------------------------------+
  |                                                                         |
  |                             lx_frame_address                           |
  |                                                                         |
  +-------------------------------------------------------------------------+*/
bit_32 lx_frame_address(public_entry_ptr pub)
BeginDeclarations
bit_32                                 address;
bit_32                                 i;
bit_32                                 seg;
#define Pub                            (*pub)
EndDeclarations
BeginCode
  address = public_target_address(pub);
  seg = -1;
  For i=0; i < n_lx_sections; i++
   BeginFor
    If address GreaterThanOrEqualTo lx_outList[i]->base
     Then
      seg++;
     Else
      ExitLoop ;
     EndIf
   EndFor ;
  return seg;
EndCode

/*+-------------------------------------------------------------------------+
  |                                                                         |
  |                             lx_target_address                           |
  |                                                                         |
  +-------------------------------------------------------------------------+*/
bit_32 lx_target_address(public_entry_ptr pub)
BeginDeclarations
bit_32                                 address;
bit_32                                 i;
bit_32                                 base;
#define Pub                            (*pub)
EndDeclarations
BeginCode
  address = public_target_address(pub);
  For i=0; i < n_lx_sections; i++
   BeginFor
    If address GreaterThanOrEqualTo lx_outList[i]->base
     Then
      base = lx_outList[i]->base;
     Else
      ExitLoop ;
     EndIf
   EndFor ;
   return address - base;
EndCode

/*+-------------------------------------------------------------------------+
  |                                                                         |
  |                             enter_lx_Fixup                              |
  |                                                                         |
  +-------------------------------------------------------------------------+*/
bit_32 enter_lx_fixup(bit_32 target, bit_32 offset, bit_32 address, loc_type location) 
BeginDeclarations
  bit_32                target_object = 0 ;
  bit_32                i ;
EndDeclarations
BeginCode
   If lxfile.val IsTrue OrIf lefile.val IsTrue
    Then
        If location IsNotEqualTo offset32_location AndIf location IsNotEqualTo secondary_offset32_location
                AndIf location IsNotEqualTo pointer32_location
            Then
             linker_error(4,"LX Fixup error:\n"
                        " Error:  segment-relative fixups in LX files must be 32 bits\n");
                return ;
            EndIf ;
      If n_lx_fixups IsEqualTo max_lx_fixups
            Then
            max_lx_fixups += 256 ;
            lx_fixup_array = (lx_fixup_hold_ptr_array)reallocate_memory(lx_fixup_array, max_lx_fixups*sizeof(lx_fixup_hold_ptr)) ;
            EndIf ;
      
      For i=1; i < n_lx_sections; i++
       BeginFor
        If target GreaterThanOrEqualTo lx_outList[i]->base
         Then
          target_object++ ;
         Else
          ExitLoop ;
         EndIf
       EndFor ;
      lx_fixup_array[n_lx_fixups] = (lx_fixup_hold_ptr)allocate_memory(sizeof(lx_fixup_hold_type));
      lx_fixup_array[n_lx_fixups]->offset = address ;
      lx_fixup_array[n_lx_fixups]->target = target  + offset;
      lx_fixup_array[n_lx_fixups]->target_object = target_object ;
      lx_fixup_array[n_lx_fixups++]->type = LX_FM_OFFSET32 ;
     If lefile.val IsTrue
      Then
       target = target - lx_outList[target_object]->base;
      EndIf
    EndIf ;	
   return target ;
EndCode

/*+-------------------------------------------------------------------------+
  |                                                                         |
  |                                SortFixups                               |
  |                                                                         |
  +-------------------------------------------------------------------------+*/
static void SortLXFixups(void)
BeginDeclarations
  bit_32 i,j;
  lx_fixup_hold_ptr temp ;
EndDeclarations
BeginCode
  For i=0 ; i < n_lx_fixups; i++
    BeginFor
      For j=i+1; j < n_lx_fixups; j++
        BeginFor
          If lx_fixup_array[i]->offset > lx_fixup_array[j]->offset
            Then
              temp = lx_fixup_array[i] ;
              lx_fixup_array[i] = lx_fixup_array[j] ;
              lx_fixup_array[j] = temp ;
            EndIf ;
        EndFor ;
    EndFor ;
EndCode

/*+-------------------------------------------------------------------------+
  |                                                                         |
  |                           createLXOutputSection                           |
  |                                                                         |
  +-------------------------------------------------------------------------+*/
bit_32 createLXOutputSection(bit_32 flags)
BeginDeclarations
  bit_32 rv = n_lx_sections++ ;
EndDeclarations
BeginCode
   lx_outList = (lx_section_ptr_array)reallocate_memory(lx_outList,sizeof(lx_section_ptr) * n_lx_sections) ;
   lx_outList[rv] = (lx_section_ptr)allocate_memory(sizeof(lx_section_type)) ;
   lx_outList[rv]->Flags = flags ;
    return rv ;
EndCode

/*+-------------------------------------------------------------------------+
  |                                                                         |
  |                               WriteLXImage                              |
  |                                                                         |
  +-------------------------------------------------------------------------+*/
void write_lx_image(void)
BeginDeclarations
    bit_32 i,j,k,l ;
    lx_header_ptr lxhead ;
    byte_ptr residentTable;
    byte_ptr entryTable;
    bit_32 residentSize;
    bit_32 entrySize;
#define LXHead (*lxhead)
    lx_object_ptr lxobject ;
#define LXObject (*lxobject)
    lx_object_page_ptr lxobjectPage,p ;
#define LXObjectPage (*lxobjectPage)
    le_object_page_ptr leobjectPage ;
#define LEObjectPage (*leobjectPage)
    lx_fixup_page_ptr lxfixupPage ;
    byte_ptr lxfixupRecord =0 ;
    byte_ptr headbuf ;
    bit_32 stubSize;
    byte_ptr stubData;
    bit_32 headerStart ;
    bit_32 headerSize ;
    bit_32 objpagestart ;
    bit_32 fixuppagestart ;
    bit_32 currentPage = 0 ;
    bit_32 size ;
    bit_32 xsize ;
    bit_32 asize ;
    bit_32 current_section = -1 ;
    byte   fixupbuf[32] ;
    bit_32 fixuplen ;
    public_entry_ptr import ;
    string_ptr stubname ;
    segment_entry_ptr seg;
    lseg_ptr lseg ;
#define Seg (*seg )
#define Lseg (*lseg)
    char buf[260] ;
    FILE *fil;
EndDeclarations
BeginCode
    ReturnIf (!n_lx_sections) ;

    // So we don't write the BSS to the file
    If PE_bss_seg IsNotZero
     Then
      lx_outList[PE_bss_seg]->initlength = 0 ;
     EndIf ;
 
    /* create a stack object */
    If lx_stack_seg IsZero
     Then
      i = createLXOutputSection(LX_OF_READABLE | LX_OF_WRITEABLE | LX_OF_ZEROFILL | LX_OF_BIGDEFAULT ) ;
      lx_outList[i]->length = stackSize.val ;
      next_available_address += 
        lx_outList[i]->virtualSize = (stackSize.val + lx_page_size.val -1)  & ~(lx_page_size.val -1);
      lx_outList[i]->base = first_pe_section_address ;
     Else
      i = lx_stack_seg ;
     EndIf
    lx_outList[i]->initlength = 0 ;
    first_pe_section_address = next_available_address = 
            (next_available_address + lx_page_size.val - 1) & ~(lx_page_size.val -1) ;
//    LXHead.stack_size = stackSize.val ;

    /* load stub32a.exe or whatever they specified */
    If stub_file_list.first IsNull
     Then
      stubname = allocate_string(260) ;
      copy_string(stubname, default_lx_stub_string) ;
      If lib_directory.val IsNotNull
       Then
         strcpy(buf,String(stubname)) ;
         fil = SearchPath(buf, String(lib_directory.val),"r") ;
         If fil IsNotNull
            Then
               fclose(fil) ;
               strcpy(String(stubname),buf) ;
               Length(stubname) = strlen(buf) ;
            EndIf
       EndIf
      process_filename(stubname) ;
      add_files_to_list(&stub_file_list, stubname);
     EndIf ;
    load_stub(&stubData,&stubSize);
    
    /* create the basic header */
    headerStart = stubSize ;
    headerStart+=0x0f;
    headerStart&=0xfffffff0; /* align lx header to 8 byte boundary */
    headerSize=sizeof(lx_header_type)+n_lx_sections*sizeof(lx_object_type)+headerStart;
    

    headbuf=allocate_memory(headerSize);
    memset(headbuf,0,headerSize);

    memcpy(headbuf,stubData,stubSize ) ;

    *(bit_32_ptr)( headbuf + 0x3c) = headerStart ; /* Pointer to PE header */
    lxhead = (lx_header_ptr)(headbuf + headerStart) ;

    If lxfile.val
     Then
      LXHead.sig = LX_SIGNATURE ;
     Else
      LXHead.sig = LE_SIGNATURE ;
     EndIf
    LXHead.cpu_level = LX_CPU_386 ;
    LXHead.os_type = LX_OS_OS2 ;
    LXHead.module_version = lx_version.val ;
    LXHead.module_flags = 0x200; /* compatible with PM Windowing, needed by UPX */
    LXHead.page_size = lx_page_size.val ;
    LXHead.auto_ds_object = 2; /* data seg, causeway needs it */
    If lxfile.val
     Then
      LXHead.page_offset_shift = lx_page_shift.val ;
     EndIf;
    LXHead.object_table_offset =  sizeof(LXHead) ;
    LXHead.object_count = n_lx_sections ;
    LXHead.object_page_table_offset  = LXHead.object_table_offset + n_lx_sections * sizeof(LXObject);

    If start_address_found IsTrue
     Then
      fixup                 = start_address;
      LXHead.eip_object = 1 ;
      LXHead.eip = target(&import) - lx_outList[0]->base ; /* RVA */
     Else  /* No start address found. */
      linker_error(4,"No start address.\n");
     EndIf;

    LXHead.esp_object = i+1 ;
    LXHead.esp = stackSize.val ; // highest address

    lxobject = headbuf + headerStart + sizeof(LXHead) ;

    /* make the objects */
    k = 0;
    For i=0; i < n_lx_sections; i++, lxobject++
     BeginFor
      lx_outList[i]->page_table_entries = (lx_outList[i]->length + lx_page_size.val-1)/ lx_page_size.val;	    
      LXObject.page_table_entries = (lx_outList[i]->initlength + lx_page_size.val-1)/ lx_page_size.val ;
      LXObject.virtual_size = lx_outList[i]->length ;
      LXObject.reloc_base_addr = lx_outList[i]->base ;
      LXObject.object_flags = lx_outList[i]->Flags ;
      LXObject.page_table_index = k + 1;
      currentPage += lx_outList[i]->page_table_entries;
      k += LXObject.page_table_entries;
     EndFor
    
    /* make the object pages */
    If lxfile.val
     Then
      p = (byte_ptr)lxobjectPage = (lx_object_page_ptr)allocate_memory(sizeof(LXObjectPage) * currentPage) ;

      k = 1 ;

      For i=0; i < n_lx_sections; i++
       BeginFor
        size = 0 ;
        For j=0; j < lx_outList[i]->page_table_entries; j++,lxobjectPage++
         BeginFor
          If size GreaterThanOrEqualTo lx_outList[i]->initlength
           Then
            LXObjectPage.flags = LX_OPF_ZERO ;
            LXObjectPage.data_offset = 0 ;
            LXObjectPage.data_size = lx_page_size.val ;
           Else
            LXObjectPage.flags = LX_OPF_ENUMERATED ;
            LXObjectPage.data_offset = k++ ;
            size +=
             LXObjectPage.data_size = lx_page_size.val ;
            LXHead.module_page_count++;
           EndIf
         EndFor
       EndFor 
     Else
      p = (byte_ptr)leobjectPage = (le_object_page_ptr)allocate_memory(sizeof(LEObjectPage) * k) ;

      k = 1 ;
      
      For i=0; i < n_lx_sections; i++
       BeginFor
        size = 0 ;
        For j=0; j < lx_outList[i]->page_table_entries; j++
         BeginFor
          If size LessThan lx_outList[i]->initlength
           Then
            LEObjectPage.flags = LE_OPF_ENUMERATED ;
            LEObjectPage.high_offs = k/65536 ;
            LEObjectPage.med_offs = k/256 ;
            LEObjectPage.low_offs = k++ ;
            size += lx_page_size.val ;
            // for the le file, page_offset_shift is number of bytes in last object
            LXHead.page_offset_shift = lx_outList[i]->length % lx_page_size.val ;
            leobjectPage++;
           EndIf
         EndFor
       EndFor 
//       leobjectPage[-1].flags |= LE_OPF_LASTPAGE ;
      EndIf

    
    lxobjectPage = leobjectPage = p ;
    LXHead.module_page_count = k-1 ;
    If lxfile.val IsTrue
     Then
      LXHead.instance_preload_count = currentPage ;
      LXHead.preload_pages_count = k-1 ;
     EndIf

    If Not lxfile.val
     Then
      char *p = exe_file_list.first->filename;
      p = strrchr(p, '\\');
      If p IsNull
       Then
        p = exe_file_list.first->filename;
       Else
        p++;
       EndIf
       residentSize = strlen(p) + 2;
       residentTable = (byte_ptr)allocate_memory(residentSize);
       residentTable[0] = residentSize - 2;
       strcpy(residentTable + 1, p); /* the null is really disassociated from the string, copy for convenience */
       LXHead.resident_name_table_offset = LXHead.object_page_table_offset + (k-1) * sizeof(LEObjectPage);
       LXHead.resident_name_table_entries = LXHead.resident_name_table_offset + residentSize;
       entryTable = (byte_ptr)allocate_memory(1);
       entrySize = 1;
       entryTable[0] = 0;
       LXHead.loader_section_size = LXHead.resident_name_table_entries + entrySize - LXHead.object_table_offset;
     Else	
      LXHead.loader_section_size = LXHead.object_page_table_offset + currentPage * sizeof (LXObjectPage) - LXHead.object_table_offset;
     EndIf ;
    
    /* fixups */
    If n_lx_fixups IsNotZero
     Then

      int n ;
      lxfixupPage = (lx_fixup_page_ptr)allocate_memory(sizeof(lx_fixup_page_type) * (lxfile.val ? 1 + currentPage : k)) ;

      If lxfile.val
       Then
        LXHead.fixup_page_table_offset = LXHead.object_page_table_offset + currentPage * sizeof (LXObjectPage);
       Else
        LXHead.fixup_page_table_offset = LXHead.resident_name_table_entries + entrySize;
       EndIf
      LXHead.fixup_record_table_offset = LXHead.fixup_page_table_offset + sizeof(lx_fixup_page_type) * (lxfile.val ? (1 + currentPage) : k);

      SortLXFixups() ;
      size = 0 ;
      asize = 0 ;
      j=0 ;
      l=0 ;
      n= 0;
      For i=0; i < n_lx_sections ; i++
       BeginFor
        xsize = lx_outList[i]->base ;
        For k = 0; k < lx_outList[i]->page_table_entries; k++,leobjectPage++, n++
         BeginFor
          If lxfile.val IsTrue OrIf n <= LXHead.module_page_count
                AndIf LEObjectPage.flags Is LX_OPF_ENUMERATED
           Then
            lxfixupPage[l].offset = size ;
            If j < n_lx_fixups AndIf (int)(lx_fixup_array[j]->offset - xsize) < (int)lx_page_size.val 
             Then
              For ; j < n_lx_fixups AndIf (int)(lx_fixup_array[j]->offset - xsize) < (int)lx_page_size.val ;j++
               BeginFor
                unsigned n;
                n = lx_fixup_array[j]->target - lx_outList[lx_fixup_array[j]->target_object]->base ;
                // embed the fixup - only doing 32-bit offsets
                fixuplen = 0 ;
                fixupbuf[fixuplen++] = LX_FM_OFFSET32 ;
                If n < 65536
                 Then
                  fixupbuf[fixuplen++] = LX_FT_INTERNAL ;
                 Else
                  fixupbuf[fixuplen++] = LX_FT_INTERNAL + LX_FF_TARGET32 ;
                 EndIf
                *(bit_16_ptr)(fixupbuf+fixuplen) = lx_fixup_array[j]->offset - xsize ;
                fixuplen += sizeof(bit_16);
                fixupbuf[fixuplen++] = lx_fixup_array[j]->target_object+1 ;
                n = lx_fixup_array[j]->target - lx_outList[lx_fixup_array[j]->target_object]->base ;
                If n < 65536
                 Then
                  *(bit_16 *)(fixupbuf + fixuplen) = n;
                  fixuplen += sizeof(bit_16) ;
                 Else
                  *(bit_32 *)(fixupbuf + fixuplen) = n;
                  fixuplen += sizeof(bit_32) ;
                 EndIf

                If asize - size < fixuplen 
                 Then
                  asize += 2048 ;
                  lxfixupRecord = (byte *)reallocate_memory(lxfixupRecord, asize) ;
                 EndIf
                memcpy(lxfixupRecord+size, fixupbuf, fixuplen) ;
                size += fixuplen ;
                // Check for fixup crossing a page boundary
                If (int)(lx_fixup_array[j]->offset + sizeof(bit_32) - xsize) > (int)lx_page_size.val
                 Then
                  ExitLoop ;
                 EndIf 
               EndFor
             EndIf
            l++ ;
           EndIf
          xsize += lx_page_size.val ;
         EndFor
       EndFor

      lxfixupPage[l].offset = size ;
      LXHead.fixup_section_size = size + LXHead.fixup_record_table_offset - LXHead.fixup_page_table_offset ;
      LXHead.loader_section_size = LXHead.fixup_page_table_offset + LXHead.fixup_section_size  - LXHead.object_table_offset;
     EndIf

    LXHead.import_module_table_offset = LXHead.loader_section_size + sizeof(LXHead) ;
    LXHead.import_proc_table_offset = LXHead.import_module_table_offset ;
    LXHead.data_pages_offset =  LXHead.import_proc_table_offset + 3 + headerStart;

    lxobjectPage = leobjectPage = p ;

    far_set(BytePtr(object_file_element), 0, MAX_ELEMENT_SIZE);

    file_open_for_write(exe_file_list.first) ;
    file_write(headbuf,headerSize) ;
    file_write(lxobjectPage, (lxfile.val ? sizeof(lx_object_page_type) * currentPage : sizeof(le_object_page_type) * LXHead.module_page_count )) ;
    If Not lxfile.val
     Then
      file_write(residentTable, residentSize) ;
      file_write(entryTable, entrySize) ;
     EndIf
    If n_lx_fixups IsNotZero
     Then
      file_write(lxfixupPage,(lxfile.val ? (currentPage+1) : LXHead.module_page_count + 1) * sizeof(lx_fixup_page_type)) ;
      file_write(lxfixupRecord,size) ;
     EndIf 

 next_available_address = 0 ;

 /* IMPORT TABLES */ 
 file_write(&next_available_address, 3);
// file_write(&next_available_address, 3);

 TraverseList(segment_list, seg)
  BeginTraverse
   LoopIf((*Seg.lsegs.first).align Is absolute_segment);
   LoopIf(Seg.pe_section_number Is PE_bss_seg);
   LoopIf(Seg.pe_section_number Is LXHead.esp_object-1) ;

   If current_section IsNotEqualTo Seg.pe_section_number
    Then
       current_section = Seg.pe_section_number ;
    EndIf ;

   TraverseList(Seg.lsegs, lseg)
    BeginTraverse
       If Lseg.address - next_available_address IsNotZero
        Then
         write_gap(Lseg.address - next_available_address) ;
        EndIf ;
       next_available_address = Lseg.address + Lseg.length ;
       If Seg.combine Is blank_common_combine
        Then
         write_gap(Lseg.length) ;
        Else
         file_write(Addr(Lseg.data[0]), Lseg.length) ;
        EndIf ;
    EndTraverse;
   
  EndTraverse ;
  file_close_for_write() ;
EndCode

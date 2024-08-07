/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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
 * 
 */

#include "MZHeader.h"
#include "LEHeader.h"
#include "LEObjectPage.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <cstdlib>

void dump_mz(std::fstream& file, MZHeader& mzh)
{
    unsigned headerSize = mzh.n_header_paragraphs * 16;
    unsigned size = mzh.image_length_MOD_512 + mzh.image_length_DIV_512 * 512;
    if (size & 511)
        size -= 512;
    std::cout << "Header Size:        " << std::hex << std::setw(4) << std::setfill('0') << headerSize << std::endl;
    std::cout << "Image Size:         " << std::hex << std::setw(4) << std::setfill('0') << size - headerSize << std::endl;
    std::cout << "Checksum:           " << std::hex << std::setw(4) << std::setfill('0') << mzh.checksum << std::endl;
    std::cout << "Overlay:            " << std::hex << std::setw(4) << std::setfill('0') << mzh.overlay << std::endl;
    std::cout << "Min Paragraphs:     " << std::hex << std::setw(4) << std::setfill('0') << mzh.min_paragraphs_above << std::endl;
    std::cout << "Min Paragraphs:     " << std::hex << std::setw(4) << std::setfill('0') << mzh.max_paragraphs_above << std::endl;
    std::cout << "CS:IP:              " << std::hex << std::setw(4) << std::setfill('0') << mzh.initial_CS << ":" << std::hex
              << std::setw(4) << std::setfill('0') << mzh.initial_IP << std::endl;
    std::cout << "SS:SP:              " << std::hex << std::setw(4) << std::setfill('0') << mzh.initial_SS << ":" << std::hex
              << std::setw(4) << std::setfill('0') << mzh.initial_SP << std::endl;
    std::cout << "Relocation items:   " << std::dec << mzh.n_relocation_items << "(" << std::hex << std::setw(4)
              << std::setfill('0') << mzh.offset_to_relocation_table << ")";
    file.seekg(mzh.offset_to_relocation_table);
    for (int i = 0; i < mzh.n_relocation_items; i++)
    {
        if (i % 4 == 0)
            std::cout << std::endl << "        ";
        unsigned short ss[2];
        file.read((char*)ss, sizeof(ss));
        std::cout << std::hex << std::setw(4) << std::setfill('0') << (int)ss[1] << ":" << std::setw(4) << std::setfill('0')
                  << (int)ss[0] << "  ";
    }
    std::cout << std::endl << std::endl;
}
void DumpHeader(LEHeader& leh)
{
    bool lx = false;
    if (leh.sig == LX_SIGNATURE)
        lx = true;
    std::cout << "File Type: ";
    if (lx)
        std::cout << "LX";
    else
        std::cout << "LE";
    std::cout << std::endl;
    std::cout << "CPU type:            " << std::hex << std::setw(4) << std::setfill('0') << leh.cpu_level << std::endl;
    std::cout << "OS type:             " << std::hex << std::setw(4) << std::setfill('0') << leh.os_type << std::endl;
    std::cout << "Module version:      " << std::hex << std::setw(4) << std::setfill('0') << leh.module_version << std::endl;
    std::cout << "Module flags:        " << std::hex << std::setw(4) << std::setfill('0') << leh.module_flags << std::endl;
    std::cout << "CS:EIP:              " << std::hex << std::setw(4) << std::setfill('0') << leh.eip_object << ":" << std::hex
              << std::setw(4) << std::setfill('0') << leh.eip << std::endl;
    std::cout << "SS:ESP:              " << std::hex << std::setw(4) << std::setfill('0') << leh.esp_object << ":" << std::hex
              << std::setw(4) << std::setfill('0') << leh.esp << std::endl;
    std::cout << "DS                   " << std::hex << std::setw(4) << std::setfill('0') << leh.auto_ds_object << std::endl;
    std::cout << "Page size:           " << std::hex << std::setw(4) << std::setfill('0') << leh.page_size << std::endl;
    if (lx)
        std::cout << "Page shift:          " << std::hex << std::setw(4) << std::setfill('0') << leh.page_offset_shift << std::endl;
    std::cout << "Module Pages:        " << std::hex << std::setw(4) << std::setfill('0') << leh.module_page_count << std::endl;
    if (lx)
    {
        std::cout << "Instance Preload:    " << std::hex << std::setw(4) << std::setfill('0') << leh.instance_preload_count
                  << std::endl;
        std::cout << "Preload Pages:       " << std::hex << std::setw(4) << std::setfill('0') << leh.preload_pages_count
                  << std::endl;
    }
    std::cout << "Objects table:       " << std::dec << std::setw(1) << std::setfill(' ') << leh.object_count << "(" << std::hex
              << std::setw(4) << std::setfill('0') << leh.object_table_offset << ")" << std::endl;
    std::cout << "Loader size:         " << std::hex << std::setw(4) << std::setfill('0') << leh.loader_section_size << std::endl;
    std::cout << "Object Pagetable:    " << std::hex << std::setw(4) << std::setfill('0') << leh.object_page_table_offset
              << std::endl;
    if (!lx)
    {
        std::cout << "Resident Names offset:" << std::hex << std::setw(4) << std::setfill('0') << leh.resident_name_table_offset
                  << std::endl;
        std::cout << "Resident Names entries:" << std::hex << std::setw(4) << std::setfill('0') << leh.resident_name_table_entries
                  << std::endl;
    }
    std::cout << "Fixup Size:          " << std::hex << std::setw(4) << std::setfill('0') << leh.fixup_section_size << std::endl;
    std::cout << "Fixup Page Table:    " << std::hex << std::setw(4) << std::setfill('0') << leh.fixup_page_table_offset
              << std::endl;
    std::cout << "Fixup Record Table:  " << std::hex << std::setw(4) << std::setfill('0') << leh.fixup_record_table_offset
              << std::endl;
    std::cout << "Import Module Table: " << std::hex << std::setw(4) << std::setfill('0') << leh.import_module_table_offset
              << std::endl;
    std::cout << "Import Proc Table:   " << std::hex << std::setw(4) << std::setfill('0') << leh.import_proc_table_offset
              << std::endl;
    std::cout << "Data Pages:          " << std::hex << std::setw(4) << std::setfill('0') << leh.data_pages_offset << std::endl;
}
void dump_le(std::fstream& file, int hdrOfs, LEHeader& leh)
{
    DumpHeader(leh);
    std::cout << "Object table: " << leh.object_count << std::endl;
    file.seekg(hdrOfs + leh.object_table_offset);
    unsigned buf[6];
    int initPages = 0;
    for (int i = 0; i < leh.object_count; i++)
    {
        file.read((char*)buf, sizeof(buf));
        std::cout << "  "
                  << "Object " << i + 1 << ":" << std::endl;
        std::cout << "    "
                  << "Flags:   " << std::hex << std::setw(8) << std::setfill('0') << buf[2] << std::endl;
        std::cout << "    "
                  << "Base:    " << std::hex << std::setw(8) << std::setfill('0') << buf[1] << std::endl;
        std::cout << "    "
                  << "Size:    " << std::hex << std::setw(8) << std::setfill('0') << buf[0] << std::endl;
        std::cout << "    "
                  << "InitSize:" << std::hex << std::setw(8) << std::setfill('0') << buf[4] << std::endl;
        std::cout << "    "
                  << "PageOffs:" << std::hex << std::setw(8) << std::setfill('0') << buf[3] << std::endl;
        initPages += buf[4];
    }
    std::cout << "Object page table: " << initPages;
    LEObjectPage::PageData p;
    file.seekg(hdrOfs + leh.object_page_table_offset);
    for (int i = 0; i < initPages; i++)
    {
        if (i % 8 == 0)
            std::cout << std::endl;
        file.read((char*)&p, sizeof(p));
        std::cout << "    " << i + 1 << ": " << std::hex << std::setw(2) << std::setfill('0') << (int)p.flags << "  ";
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)p.high_offs;
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)p.med_offs;
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)p.low_offs << std::endl;
    }
    std::cout << std::endl << std::endl;
    std::cout << "Fixups: " << std::endl;
    file.seekg(hdrOfs + leh.fixup_page_table_offset);
    unsigned* pt = new unsigned[initPages + 1];
    file.read((char*)pt, (initPages + 1) * sizeof(unsigned));
    file.seekg(hdrOfs + leh.fixup_record_table_offset);
    for (int i = 1; i <= initPages; i++)
    {
        int offs = pt[i - 1];
        std::cout << "  Page: " << i << ":" << pt[i - 1] << ":" << pt[i] << std::endl;
        while (pt[i] > offs)
        {
            // only handling the types of fixups genned by dlle
            unsigned char bf[9];
            file.read((char*)bf, 7);
            offs += 7;
            if (bf[1] & LX_FF_TARGET32)
            {
                offs += 2;
                file.read((char*)bf + 7, 2);
            }
            else
            {
                bf[7] = bf[8] = 0;
            }
            std::cout << "    " << std::hex << std::setw(2) << std::setfill('0') << (int)bf[0] << " ";
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)bf[1] << " ";
            std::cout << std::hex << std::setw(4) << std::setfill('0') << (int)((unsigned short*)(bf + 2))[0] << " ";
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)bf[4] << " ";
            std::cout << std::hex << std::setw(8) << std::setfill('0') << (int)((unsigned*)(bf + 5))[0] << " " << std::endl;
        }
    }

    delete[] pt;
}
void dump_lx(std::fstream& file, int hdrOfs, LEHeader& leh)
{
    DumpHeader(leh);
    std::cout << "Object table: " << leh.object_count << std::endl;
    file.seekg(hdrOfs + leh.object_table_offset);
    unsigned buf[6];
    int initPages = 0;
    for (int i = 0; i < leh.object_count; i++)
    {
        file.read((char*)buf, sizeof(buf));
        std::cout << "  "
                  << "Object " << i + 1 << ":" << std::endl;
        std::cout << "    "
                  << "Flags:   " << std::hex << std::setw(8) << std::setfill('0') << buf[2] << std::endl;
        std::cout << "    "
                  << "Base:    " << std::hex << std::setw(8) << std::setfill('0') << buf[1] << std::endl;
        std::cout << "    "
                  << "Size:    " << std::hex << std::setw(8) << std::setfill('0') << buf[0] << std::endl;
        std::cout << "    "
                  << "InitSize:" << std::hex << std::setw(8) << std::setfill('0') << buf[4] << std::endl;
        std::cout << "    "
                  << "PageOffs:" << std::hex << std::setw(8) << std::setfill('0') << buf[3] << std::endl;
        initPages += buf[4];
    }
    std::cout << "Object page table: " << leh.object_count;
    LXObjectPage::PageData p;
    file.seekg(hdrOfs + leh.object_page_table_offset);
    for (int i = 0; i < leh.object_count; i++)
    {
        if (i % 8 == 0)
            std::cout << std::endl;
        file.read((char*)&p, sizeof(p));
        std::cout << "    " << i + 1 << ": " << std::hex << std::setw(2) << std::setfill('0') << (int)p.flags << "  ";
        std::cout << std::hex << std::setw(8) << std::setfill('0') << (int)p.data_offset << " ";
        std::cout << std::hex << std::setw(4) << std::setfill('0') << (int)p.data_size << std::endl;
    }
    std::cout << std::endl << std::endl;
    std::cout << "Fixups: " << std::endl;
    file.seekg(hdrOfs + leh.fixup_page_table_offset);
    unsigned* pt = new unsigned[initPages + 1];
    file.read((char*)pt, (initPages + 1) * sizeof(unsigned));
    file.seekg(hdrOfs + leh.fixup_record_table_offset);
    for (int i = 1; i <= initPages; i++)
    {
        int offs = pt[i - 1];
        std::cout << "  Page: " << i << ":" << pt[i - 1] << ":" << pt[i] << std::endl;
        while (pt[i] > offs)
        {
            // only handling the types of fixups genned by dlle
            unsigned char bf[9];
            file.read((char*)bf, 7);
            offs += 7;
            if (bf[1] & LX_FF_TARGET32)
            {
                offs += 2;
                file.read((char*)bf + 7, 2);
            }
            else
            {
                bf[7] = bf[8] = 0;
            }
            std::cout << "    " << std::hex << std::setw(2) << std::setfill('0') << (int)bf[0] << " ";
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)bf[1] << " ";
            std::cout << std::hex << std::setw(4) << std::setfill('0') << (int)((unsigned short*)(bf + 2))[0] << " ";
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)bf[4] << " ";
            std::cout << std::hex << std::setw(8) << std::setfill('0') << (int)((unsigned*)(bf + 5))[0] << " " << std::endl;
        }
    }

    delete[] pt;
}
int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "Usage:  ledump file";
        exit(1);
    }
    std::fstream in(argv[1], std::ios::in | std::ios::binary);
    if (!in.is_open())
    {
        std::cout << "Invalid input file";
        exit(1);
    }
    MZHeader mzh;
    in.read((char*)&mzh, sizeof(mzh));
    if (mzh.signature != MZ_SIGNATURE)
    {
        std::cout << "Not an EXE file";
        exit(1);
    }
    in.seekg(0x3c);
    unsigned leOffs;
    in.read((char*)&leOffs, sizeof(leOffs));
    in.seekg(leOffs);
    LEHeader leh;
    in.read((char*)&leh, sizeof(leh));
    dump_mz(in, mzh);
    if (leh.sig == LE_SIGNATURE)
        dump_le(in, leOffs, leh);
    else if (leh.sig == LX_SIGNATURE)
        dump_lx(in, leOffs, leh);
    else
        std::cout << "Can only dump LE and LX files";
}
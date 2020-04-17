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

#ifndef MZHeader_h
#define MZHeader_h

#define MZ_SIGNATURE 0x5A4D /* MZ */

struct MZHeader
{
    unsigned short signature;
    unsigned short image_length_MOD_512;
    unsigned short image_length_DIV_512;
    unsigned short n_relocation_items;
    unsigned short n_header_paragraphs;
    unsigned short min_paragraphs_above;
    unsigned short max_paragraphs_above;
    unsigned short initial_SS;
    unsigned short initial_SP;
    unsigned short checksum;
    unsigned short initial_IP;
    unsigned short initial_CS;
    unsigned short offset_to_relocation_table;
    unsigned short overlay;
    unsigned short always_one;
};
#endif

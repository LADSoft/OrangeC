/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

#include "LEObjectPage.h"
#include "LEObject.h"
#include "LEHeader.h"
#include <cstring>
unsigned LEObjectPage::CountPages()
{
    int n = 0;
    for (auto obj : objects)
    {
        n += ObjectAlign(4096, obj->GetInitSize()) / 4096;
    }
    return n;
}
unsigned LXObjectPage::CountPages()
{
    int n = 0;
    for (auto obj : objects)
    {
        n += ObjectAlign(4096, obj->GetSize()) / 4096;
    }
    return n;
}
void ObjectPage::Write(std::fstream& stream) { stream.write((char*)data.get(), size); }
void LEObjectPage::Setup()
{
    pages = CountPages();
    size = pages * sizeof(PageData);
    data = std::make_unique<unsigned char[]>(size);
    memset(data.get(), 0, size);
    PageData* p = (PageData*)data.get();
    for (int i = 1; i <= pages; i++)
    {
        p->flags = LE_OPF_ENUMERATED;
        p->high_offs = i / 65536;
        p->med_offs = i / 256;
        p->low_offs = i;
        p++;
    }
}
void LXObjectPage::Setup()
{
    pages = CountPages();
    size = pages * sizeof(PageData);
    data = std::make_unique<unsigned char[]>(size);

    PageData* p = (PageData*)data.get();
    int k = 1;
    for (auto obj : objects)
    {
        for (int size = 0; size < obj->GetSize(); size += 4096)
        {
            if (size >= obj->GetInitSize())
            {
                p->flags = LX_OPF_ZERO;
                p->data_offset = 0;
            }
            else
            {
                p->flags = LX_OPF_ENUMERATED;
                p->data_offset = k++;
            }
            p->data_size = 4096;
            p++;
        }
    }
}

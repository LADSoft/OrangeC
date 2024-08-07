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

#include "ObjIndexManager.h"
#include "ObjFile.h"
#include "ObjSection.h"
#include "ObjSymbol.h"
#include "ObjType.h"
#include "ObjSourceFile.h"
void ObjIndexManager::ResetIndexes()
{
    Section = 0;
    Public = 0;
    Local = 0;
    External = 0;
    Type = 0;
    File = 0;
    Auto = 0;
    Reg = 0;
}
void ObjIndexManager::LoadIndexes(ObjFile* file)
{
    ResetIndexes();
    for (auto it = file->PublicBegin(); it != file->PublicEnd(); ++it)
    {
        if ((*it)->GetIndex() >= Public)
            Public = (*it)->GetIndex() + 1;
    }
    for (auto it = file->ExternalBegin(); it != file->ExternalEnd(); ++it)
    {
        if ((*it)->GetIndex() >= External)
            External = (*it)->GetIndex() + 1;
    }
    for (auto it = file->LocalBegin(); it != file->LocalEnd(); ++it)
    {
        if ((*it)->GetIndex() >= Local)
            Local = (*it)->GetIndex() + 1;
    }
    for (auto it = file->AutoBegin(); it != file->AutoEnd(); ++it)
    {
        if ((*it)->GetIndex() >= Auto)
            Auto = (*it)->GetIndex() + 1;
    }
    for (auto it = file->RegBegin(); it != file->RegEnd(); ++it)
    {
        if ((*it)->GetIndex() >= Reg)
            Reg = (*it)->GetIndex() + 1;
    }
    for (auto it = file->SectionBegin(); it != file->SectionEnd(); ++it)
    {
        if ((*it)->GetIndex() >= Section)
            Section = (*it)->GetIndex() + 1;
    }
    for (auto it = file->TypeBegin(); it != file->TypeEnd(); ++it)
    {
        if ((*it)->GetIndex() >= Type)
            Type = (*it)->GetIndex() + 1;
    }
    for (auto it = file->SourceFileBegin(); it != file->SourceFileEnd(); ++it)
    {
        if ((*it)->GetIndex() >= File)
            File = (*it)->GetIndex() + 1;
    }
}

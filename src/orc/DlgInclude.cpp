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

#include "DlgInclude.h"
#include "RCFile.h"
#include "ResFile.h"
#include "ResourceData.h"
#include <stdexcept>

DlgInclude::~DlgInclude() {}
void DlgInclude::SetData(ResourceData* rdata) { data.reset(rdata); }
void DlgInclude::WriteRes(ResFile& resFile)
{
    Resource::WriteRes(resFile);
    if (data)
        data->WriteRes(resFile);
    resFile.Release();
}
void DlgInclude::ReadRC(RCFile& rcFile)
{
    resInfo.SetFlags(resInfo.GetFlags() | ResourceInfo::Pure);
    resInfo.ReadRC(rcFile, false);
    if (!rcFile.IsString())
        throw std::runtime_error("Expected quoted string");
    std::wstring wname = rcFile.GetString();
    std::string name = rcFile.CvtString(wname);
    data = std::make_unique<ResourceData>((unsigned char*)name.c_str(), name.size() + 1);
}

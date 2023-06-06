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

#include "RCData.h"
#include "RCFile.h"
#include "ResFile.h"
#include "ResourceData.h"
#include <cstring>
RCData::~RCData() {}
void RCData::Add(ResourceData* rdata)
{
    std::unique_ptr<ResourceData> temp(rdata);
    data.push_back(std::move(temp));
}

void RCData::WriteRes(ResFile& resFile)
{
    Resource::WriteRes(resFile);
    for (auto& res : *this)
    {
        res->WriteRes(resFile);
    }
    resFile.Release();
}
void RCData::ReadRC(RCFile& rcFile)
{
    resInfo.SetFlags((resInfo.GetFlags() & ~ResourceInfo::Discardable) | ResourceInfo::Pure);
    resInfo.ReadRC(rcFile, false);

    const Token* t = rcFile.GetToken();
    if (t->GetKeyword() != kw::BEGIN)
    {
        data.push_back(std::make_unique<ResourceData>());
        data.back()->ReadRC(rcFile);
    }
    else
    {
        rcFile.NeedBegin();
        bool done = false;
        bool concatwide = false;
        while (!done)
        {
            t = rcFile.GetToken();
            if (t->IsNumeric())
            {
                int n = t->GetInteger();
                if (t->GetNumericType() == Token::t_int || t->GetNumericType() == Token::t_unsignedint)
                {
                    data.push_back(std::make_unique<ResourceData>((unsigned char*)&n, 2));
                }
                else
                {
                    ResourceData* rd = new ResourceData((unsigned char*)&n, 4);
                    data.push_back(std::make_unique<ResourceData>((unsigned char*)&n, 4));
                }
            }
            else if (t->IsString())
            {
                if (t->IsWide() || concatwide)
                {
                    // this is being done for portability
                    int n = t->GetString().size();
                    std::unique_ptr<wchar_t[]> p = std::make_unique<wchar_t[]>(n);
                    for (int i = 0; i < n; i++)
                        p[i] = t->GetString()[i];
                    data.push_back(std::make_unique<ResourceData>((unsigned char*)p.get(), n * sizeof(wchar_t)));
                    concatwide = true;
                }
                else
                {
                    std::string name = rcFile.CvtString(t->GetString());
                    data.push_back(std::make_unique<ResourceData>((unsigned char*)name.c_str(), t->GetString().size()));
                }
            }
            else
                done = true;
            if (!done)
            {
                rcFile.NextToken();
                if (rcFile.IsKeyword())
                    if (rcFile.GetToken()->GetKeyword() == kw::comma)
                        concatwide = false;
                rcFile.SkipComma();
            }
        }
        rcFile.NeedEnd();
    }
}

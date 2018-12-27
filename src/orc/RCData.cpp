/* Software License Agreement
 *
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the
 *     Orange C "Target Code" exception.
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
RCData::~RCData()
{
    for (auto it = begin(); it != end(); ++it)
    {
        ResourceData* r = (*it);
        delete r;
    }
    data.clear();
}
void RCData::WriteRes(ResFile& resFile)
{
    Resource::WriteRes(resFile);
    for (auto res : *this)
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
    if (t->GetKeyword() != Lexer::BEGIN)
    {
        ResourceData* rd = new ResourceData;
        rd->ReadRC(rcFile);
        data.push_back(rd);
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
                    ResourceData* rd = new ResourceData((unsigned char*)&n, 2);
                    data.push_back(rd);
                }
                else
                {
                    ResourceData* rd = new ResourceData((unsigned char*)&n, 4);
                    data.push_back(rd);
                }
            }
            else if (t->IsString())
            {
                if (t->IsWide() || concatwide)
                {
                    // this is being done for portability
                    int n = t->GetString().size();
                    wchar_t* p = new wchar_t[n];
                    for (int i = 0; i < n; i++)
                        p[i] = t->GetString()[i];
                    ResourceData* rd = new ResourceData((unsigned char*)p, n * sizeof(wchar_t));
                    data.push_back(rd);
                    delete[] p;
                    concatwide = true;
                }
                else
                {
                    std::string name = rcFile.CvtString(t->GetString());
                    ResourceData* rd = new ResourceData((unsigned char*)name.c_str(), t->GetString().size());
                    data.push_back(rd);
                }
            }
            else
                done = true;
            if (!done)
            {
                rcFile.NextToken();
                if (rcFile.IsKeyword())
                    if (rcFile.GetToken()->GetKeyword() == Lexer::comma)
                        concatwide = false;
                rcFile.SkipComma();
            }
        }
        rcFile.NeedEnd();
    }
}

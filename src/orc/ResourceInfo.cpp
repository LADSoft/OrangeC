/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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

#include "ResourceInfo.h"
#include "ResFile.h"
#include "RCFile.h"

void ResourceInfo::WriteRes(ResFile& resFile)
{
    resFile.WriteDWord(0);
    resFile.WriteWord(memoryFlags);
    resFile.WriteWord(language);
    resFile.WriteDWord(version);
    resFile.WriteDWord(characteristics);
}
void ResourceInfo::ReadRC(RCFile& rcFile, bool secondary)
{
    bool done = false;
    while (!done)
    {
        switch (rcFile.GetToken()->GetKeyword())
        {
            case kw::DISCARDABLE:
                memoryFlags |= Discardable;
                break;
            case kw::PURE:
                memoryFlags |= Pure;
                break;
            case kw::PRELOAD:
                memoryFlags |= Preload;
                break;
            case kw::MOVEABLE:
                memoryFlags |= Moveable;
                break;
            case kw::LOADONCALL:
                memoryFlags |= LoadOnCall;
                break;
            case kw::NONDISCARDABLE:
                memoryFlags &= ~Discardable;
                break;
            case kw::IMPURE:
                memoryFlags &= ~Pure;
                break;
            case kw::FIXED:
                memoryFlags &= ~Moveable;
                break;
            default:
                done = true;
                break;
        }
        if (!done)
            rcFile.NextToken();
    }
    if (secondary)
    {
        done = false;
        language = rcFile.GetLanguage();
        while (!done)
        {
            switch (rcFile.GetToken()->GetKeyword())
            {
                case kw::LANGUAGE:
                    rcFile.NextToken();
                    language = rcFile.GetNumber();
                    rcFile.GetNumber();
                    rcFile.NeedEol();
                    break;
                case kw::VERSION:
                    rcFile.NextToken();
                    version = rcFile.GetNumber();
                    rcFile.NeedEol();
                    break;
                case kw::CHARACTERISTICS:
                    rcFile.NextToken();
                    characteristics = rcFile.GetNumber();
                    rcFile.NeedEol();
                    break;
                default:
                    done = true;
                    break;
            }
        }
    }
}

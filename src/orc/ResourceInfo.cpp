/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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
            case Lexer::DISCARDABLE:
                memoryFlags |= Discardable;
                break;
            case Lexer::PURE:
                memoryFlags |= Pure;
                break;
            case Lexer::PRELOAD:
                memoryFlags |= Preload;
                break;
            case Lexer::MOVEABLE:
                memoryFlags |= Moveable;
                break;
            case Lexer::LOADONCALL:
                memoryFlags |= LoadOnCall;
                break;
            case Lexer::NONDISCARDABLE:
                memoryFlags &= ~Discardable;
                break;
            case Lexer::IMPURE:
                memoryFlags &= ~Pure;
                break;
            case Lexer::FIXED:
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
                case Lexer::LANGUAGE:
                    rcFile.NextToken();
                    language = rcFile.GetNumber();
                    rcFile.GetNumber();
                    rcFile.NeedEol();
                    break;
                case Lexer::VERSION:
                    rcFile.NextToken();
                    version = rcFile.GetNumber();
                    rcFile.NeedEol();
                    break;
                case Lexer::CHARACTERISTICS:
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

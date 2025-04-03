/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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

#include "LinkRegionFileSpec.h"
#include <cstring>
#include <vector>
#include <fstream>
LinkRegionFileSpecContainer::LinkRegionFileSpecContainer(const ObjString& Spec)
{
    const char* data = Spec.c_str();
    int start = 0, end;
    while (data[start])
    {
        if (data[start] == '*')
            specs.push_back(std::make_unique<LinkRegionFileSpec>(LinkRegionFileSpec::eStar)), start++;
        else if (data[start] == '?')
            specs.push_back(std::make_unique<LinkRegionFileSpec>(LinkRegionFileSpec::eQuestionMark)), start++;
        else
        {
            end = start;
            while (data[end] && data[end] != '*' && data[end] != '?')
            {
                end++;
            }
            specs.push_back(std::make_unique<LinkRegionFileSpec>(LinkRegionFileSpec::eSpan, Spec.substr(start, end)));
            start = end;
        }
    }
}
LinkRegionFileSpecContainer::~LinkRegionFileSpecContainer() {}
bool LinkRegionFileSpecContainer::Matches(const ObjString& Spec)
{
    ObjString working = Spec;
    for (auto& spec : specs)
        for (auto it = specs.begin(); it != specs.end(); ++it)
        {
            switch ((*it)->GetType())
            {
                case LinkRegionFileSpec::eStar: {
                    ++it;
                    if (it != specs.end())
                    {
                        if ((*it)->GetType() != LinkRegionFileSpec::eSpan)
                            return false;
                        size_t pos = working.find((*it)->GetSpan());
                        if (pos == ObjString::npos)
                            return false;
                        working = working.substr(pos + (*it)->GetSpan().size());
                    }
                    else
                        return true;
                    break;
                }
                case LinkRegionFileSpec::eQuestionMark:
                    if (working.size() < 1)
                        return false;
                    working = working.substr(1);
                    break;
                default: {
                    int pos = working.find((*it)->GetSpan());
                    if (pos != 0)
                        return false;
                    working = working.substr((*it)->GetSpan().size());
                    break;
                }
            }
        }
    return true;
}

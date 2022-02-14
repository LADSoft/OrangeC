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

#include <fstream>
#include "MakeStubs.h"
#include "Errors.h"
#include "PreProcessor.h"

void MakeStubs::Run(std::ostream* out)
{
    bool toClose = false;
    if (!outputFile.empty())
    {
        out = new std::fstream(outputFile, std::ios::out);
        if (!out->good())
            Utils::fatal("Cannot open '%s' for write", outputFile.c_str());
        toClose = true;
    }
    else if (!out)
        return;
    int count;
    std::string separator = " ";
    if (userOnly)
        count = preProcessor.GetUserIncludes().size();
    else
        count = preProcessor.GetUserIncludes().size() + preProcessor.GetSysIncludes().size();
    if (count > 7)
        separator = " \\\n    ";

    std::string outTargets;
    if (targets.size() || quotedTargets.size())
    {
        auto splitTargets = Utils::split(targets);
        auto splitQuotedTargets = Utils::split(quotedTargets);
        for (auto t : splitTargets)
            outTargets += t + " ";
        for (auto t : splitQuotedTargets)
        {
            auto t1 = t;
            int end = t1.find_first_of('$');
            while (end != std::string::npos)
            {
                t1 = t1.substr(0, end) + "$" + t1.substr(end);
                end = t1.find_first_of('$', end + 2);
            }
            outTargets += t1 + " ";
        }
    }
    else
    {
        outTargets = inputFile;
        int end = outTargets.find_last_of('.');
        if (end != std::string::npos)
            outTargets = outTargets.substr(0, end);
        outTargets += ".o";
    }
    *out << outTargets << ": ";
    if (!userOnly)
        for (auto t : preProcessor.GetSysIncludes())
            *out << t << separator;
    for (auto t : preProcessor.GetUserIncludes())
        *out << t << separator;
    if (phonyTargets)
    {
        *out << std::endl;
        if (!userOnly)
            for (auto t : preProcessor.GetSysIncludes())
                *out << t << ":\n";
        for (auto t : preProcessor.GetUserIncludes())
            *out << t << ":\n";
    }
    if (toClose)
    {
        delete out;
    }
}

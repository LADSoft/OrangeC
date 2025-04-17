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

#include "GrepMain.h"
#include "CmdFiles.h"
#include "Utils.h"
#include "ToolChain.h"
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <algorithm>
#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#else
#    include <io.h>
#endif
CmdSwitchParser GrepMain::SwitchParser;
CmdSwitchBool GrepMain::recurseDirs(SwitchParser, 'd');
CmdSwitchBool GrepMain::caseInSensitive(SwitchParser, 'i', false);
CmdSwitchBool GrepMain::completeWords(SwitchParser, 'w');
CmdSwitchBool GrepMain::regularExpressions(SwitchParser, 'r', true);
CmdSwitchBool GrepMain::displayMatchCount(SwitchParser, 'c');
CmdSwitchBool GrepMain::displayFileNamesOnly(SwitchParser, 'l');
CmdSwitchBool GrepMain::displayLineNumbers(SwitchParser, 'n');
CmdSwitchBool GrepMain::unixMode(SwitchParser, 'o');
CmdSwitchBool GrepMain::displayNonMatching(SwitchParser, 'v');
CmdSwitchBool GrepMain::verboseMode(SwitchParser, 'z');
// not actual parameters
CmdSwitchBool GrepMain::displayFileNames(SwitchParser, '#');
CmdSwitchBool GrepMain::displayHeaderFileName(SwitchParser, '#');
CmdSwitchInt GrepMain::showAfter(SwitchParser, 'A', 0, 0, INT_MAX);
CmdSwitchInt GrepMain::showBefore(SwitchParser, 'B', 0, 0, INT_MAX);
CmdSwitchInt GrepMain::showBoth(SwitchParser, 'C', 0, 0, INT_MAX);
CmdSwitchInt GrepMain::maxMatches(SwitchParser, 'm', INT_MAX, 0, INT_MAX);
CmdSwitchBool GrepMain::quiet(SwitchParser, 'q');

const char* GrepMain::usageText = "[options] searchstring file[s]";
const char* GrepMain::helpText =
R"help([options] searchstring file[s]
    
This program is a standalone grep program
grep programs perform a search for words or phrases
within a file or set of files.

    -c             Show Match Count only      -d   Recurse Subdirectories
    -i             Case Insensitive           -l   Show File Names only
    -n             Show Line Numbers          -o   UNIX Output Format
    -r-            No Regular Expressions     -v   Non Matching Lines
    -w             Complete Words Only        -z   Verbose
    -A:#           Show Lines After           -B:# Show Lines Before
    -C:#           Show Lines Both            -m:# Set Max Matches
    -V, --version  Show version and date      -? or --help  This help
    
Regular expressions special characters:
    .  Match any character   \\  Quote next character
    *  Match zero or more    +  Match one or more   ?  Match zero or one
    ^  Start of line         $  End of line         
    \\b Word break            \\B Inside word         \\w Word beginning
    \\W Word end              \\< Word constituent    \\> Word non-constituent
    
Use brackets to match one characters out of a set:
    [aeiou4-7]   match vowels or the numbers from 4 to 7 inclusive
    [^aeiou4-7]  match anything but vowels or the numbers from 4 to 7
Use \\( and \\) to set a match region, \\x where x is a digit to access it
Use \\{ and \\} to set an interval:
    a\\{2,4\\} matches from two to four 'a' characters

)help"
"Time: " __TIME__ "  Date: " __DATE__;

int main(int argc, char** argv)
MAINTRY
{
    GrepMain grepMain;
    return grepMain.Run(argc, argv);
}
MAINCATCH
void GrepMain::SetModes(void)
{

    if (verboseMode.GetValue())
    {
        unixMode.SetValue(false);
        displayHeaderFileName.SetValue(true);
        displayFileNames.SetValue(false);
        displayLineNumbers.SetValue(true);
        displayFileNamesOnly.SetValue(false);
    }
    else if (displayMatchCount.GetValue())
    {
        unixMode.SetValue(false);
        displayFileNamesOnly.SetValue(true);
        displayLineNumbers.SetValue(false);
        displayFileNames.SetValue(false);
        displayHeaderFileName.SetValue(true);
    }
    else if (displayFileNamesOnly.GetValue())
    {
        displayLineNumbers.SetValue(false);
        displayFileNames.SetValue(false);
        displayHeaderFileName.SetValue(true);
    }
    else if (unixMode.GetValue())
    {
        displayFileNames.SetValue(true);
        displayHeaderFileName.SetValue(false);
    }
    else
    {
        displayHeaderFileName.SetValue(true);
    }
}
void GrepMain::DisplayMatch(const std::string& fileName, int& matchCount, int lineno, const char* startpos, const char* text)
{
    if (quiet.GetValue())
    {
        return;
    }
    if (matchCount == 0 && displayHeaderFileName.GetValue())
    {
        std::cout << "FILE: " << fileName;
        if (verboseMode.GetValue() || !displayMatchCount.GetValue())
            std::cout << std::endl;
    }
    if (!displayFileNamesOnly.GetValue())
    {
        const char* p = text;
        while (p[-1] && p[-1] != '\n')
            p--;
        const char* q = strchr(p, '\n');
        if (!q)
            q = p + strlen(p);
        if (showBefore.GetValue() || showAfter.GetValue())
            std::cout << "--" << std::endl;
        for (int i = 0; i < showBefore.GetValue() + 1 && p > startpos;)
        {
            if (p[-1] == '\n')
                i++, lineno--;
            p--;
        }
        for (int i = 0; i < showAfter.GetValue() && *q; i++)

        {
            const char* s = strchr(q + 1, '\n');
            if (!s)
                s = q + strlen(q);
            q = s;
        }
        while (p != q)
        {
            const char* s = strchr(p + 1, '\n');
            if (!s)
                s = p + strlen(p);
            if (displayFileNames.GetValue())
            {
                int n = fileName.size();
                n = ((n + 8) / 8) * 8;
                std::cout << std::setfill(' ') << std::setw(n) << std::left << fileName;
            }
            if (displayLineNumbers.GetValue())
            {
                std::cout << std::setfill(' ') << std::setw(8) << std::left << lineno++;
            }
            std::string buf(p + 1, s - p - 1);
            std::cout << buf;

            p = s;
            std::cout << std::endl;
        }
    }
    matchCount++;
    std::cout.clear();
}
void GrepMain::FindLine(const std::string fileName, int& matchCount, int& matchLine, char** matchPos, char* startpos, char* curpos,
                        bool matched)
{
    char* p = *matchPos;
    do
    {
        p = (char*)strchr(p, '\n');
        if (!p)
            p = *matchPos + strlen(*matchPos);
        if (p <= curpos)
        {
            if (displayNonMatching.GetValue())
            {
                DisplayMatch(fileName, matchCount, matchLine, startpos, p);
            }
            if (*p)
            {
                p++;
                matchLine++;
            }
        }
    } while (p < curpos);
    if (matched && !displayNonMatching.GetValue())
    {
        DisplayMatch(fileName, matchCount, matchLine, startpos, curpos);
    }
    if (*p)
    {
        p++;
        matchLine++;
    }
    *matchPos = p;
}
int GrepMain::OneFile(RegExpContext& regexp, const std::string fileName, std::istream& fil, int& openCount)
{
    openCount++;
    // couldn't do this as a straight definition as MSVC decided to treat it as a func
    std::unique_ptr<std::string> str =
        std::make_unique<std::string>(std::istreambuf_iterator<char>(fil), std::istreambuf_iterator<char>());
    str->erase(std::remove(str->begin(), str->end(), '\r'), str->end());
    std::string bufs = " " + *str;
    bufs[0] = 0;
    int matchCount = 0;
    int lineno = 0;
    int length = bufs.size() - 1;
    if (!fil.fail())
    {
        char* buf = const_cast<char*>(bufs.c_str());
        char* start = buf;
        char* str = buf + 1;
        char* matchPos = buf + 1;
        int matchLine = 1;
        bool matched = true;
        while (matched && matchCount < maxMatches.GetValue())
        {
            char* p = nullptr;
            matched = regexp.Match(str - buf, length, buf);
            if (matched)
            {
                FindLine(fileName, matchCount, matchLine, &matchPos, start, buf + regexp.GetStart(), true);
                p = (char*)strchr((char*)buf + regexp.GetEnd(), '\n');
            }
            else
            {
                FindLine(fileName, matchCount, matchLine, &matchPos, start, str + strlen(str), false);
            }
            if (!p)
                p = str + strlen(str);
            else
                p++;
            str = p;
        }
        if (matchCount && verboseMode.GetValue() && !quiet.GetValue())
        {
            if (displayNonMatching.GetValue())
            {
                std::cout << matchCount << " Non-matching lines" << std::endl;
            }
            else
            {
                std::cout << matchCount << " Matching lines" << std::endl;
            }
        }
        else if (matchCount && displayMatchCount.GetValue() && !quiet.GetValue())
        {
            std::cout << ": " << matchCount << std::endl;
        }
    }
    return matchCount;
}
int GrepMain::Run(int argc, char** argv)
{
    auto files =
        ToolChain::StandardToolStartup(SwitchParser, argc, argv, usageText, helpText, [this]() { return !verboseMode.GetValue(); });

    if (showBoth.GetExists())
    {
        showAfter.SetValue(showBoth.GetValue());
        showBefore.SetValue(showBoth.GetValue());
    }
    if (files.size() < 3)
    {
        if (isatty(fileno(stdin)) || files.size() < 2)
            ToolChain::Usage(usageText, 2);
    }
    SetModes();
    RegExpContext regexp(argv[1], regularExpressions.GetValue(), !caseInSensitive.GetValue(), completeWords.GetValue());

    if (!regexp.IsValid())
    {
        fprintf(stderr, "Invalid regular expression");
        return 2;
    }

    int openCount = 0;
    int matchCount = 0;
    if (!isatty(fileno(stdin)))
    {
        matchCount += OneFile(regexp, "STDIN", std::cin, openCount);
    }
    else
        for (int i = 1; i < files.size(); i++)
        {
            std::string name = files[i];
            std::fstream fil(name, std::ios::in | std::ios::binary);
            if (fil.is_open())
                matchCount += OneFile(regexp, name, fil, openCount);
        }
    if (openCount == 0)
    {
        if (!quiet.GetValue())
        {
            std::cout << "Nothing to do." << std::endl;
        }
        return 2;
    }
    return matchCount ? 0 : 1;
}

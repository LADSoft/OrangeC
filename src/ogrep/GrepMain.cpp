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

#include "GrepMain.h"
#include "CmdFiles.h"
#include "Utils.h"
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
CmdSwitchBool GrepMain::ShowHelp(SwitchParser, '?', false, {"help"});
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
    "[options] searchstring file[s]"
    "\n"
    "   -c             Show Match Count only      -d   Recurse Subdirectories\n"
    "   -i             Case Insensitive           -l   Show File Names only\n"
    "   -n             Show Line Numbers          -o   UNIX Output Format\n"
    "   -r-            No Regular Expressions     -v   Non Matching Lines\n"
    "   -w             Complete Words Only        -z   Verbose\n"
    "   -A:#           Show Lines After           -B:# Show Lines Before\n"
    "   -C:#           Show Lines Both            -m:# Set Max Matches\n"
    "   -V, --version  Show version and date      -? or --help  This help\n"
    "\n"
    "Regular expressions special characters:\n"
    "   .  Match any character   \\  Quote next character\n"
    "   *  Match zero or more    +  Match one or more   ?  Match zero or one\n"
    "   ^  Start of line         $  End of line         \n"
    "   \\b Word break            \\B Inside word         \\w Word beginning\n"
    "   \\W Word end              \\< Word constituent    \\> Word non-constituent\n"
    "\n"
    "Use brackets to match one characters out of a set:\n"
    "   [aeiou4-7]   match vowels or the numbers from 4 to 7 inclusive\n"
    "   [^aeiou4-7]  match anything but vowels or the numbers from 4 to 7\n"
    "Use \\( and \\) to set a match region, \\x where x is a digit to access it\n"
    "Use \\{ and \\} to set an interval:\n"
    "   a\\{2,4\\} matches from two to four 'a' characters\n"
    "Time: " __TIME__ "  Date: " __DATE__;

int main(int argc, char** argv)
{
    GrepMain grepMain;
    return grepMain.Run(argc, argv);
}
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
void GrepMain::usage(const char* prog_name, const char* text, int retcode)
{
    fprintf(stderr, "\nusage: %s %s", Utils::ShortName(prog_name), text);
    exit(retcode);
}
int GrepMain::Run(int argc, char** argv)
{
    // handle /V switch
    for (int i = 1; i < argc; i++)
        if (argv[i] && (argv[i][0] == '/' || argv[i][0] == '-'))
            if ((argv[i][1] == 'V' && argv[i][2] == 0) || !strcmp(argv[i], "--version"))
            {
                Utils::banner(argv[0]);
                exit(0);
            }
    if (!SwitchParser.Parse(&argc, argv))
    {
        usage(argv[0], usageText, 2);
    }
    if (showBoth.GetExists())
    {
        showAfter.SetValue(showBoth.GetValue());
        showBefore.SetValue(showBoth.GetValue());
    }
    if (ShowHelp.GetValue() || (argc >= 2 && !strcmp(argv[1], "?")))
    {
        Utils::banner(argv[0]);
        usage(argv[0], helpText, 0);
    }
    if (argc < 3)
    {
        if (isatty(fileno(stdin)) || argc < 2)
            usage(argv[0], usageText, 2);
    }
    if (verboseMode.GetValue())
    {
        Utils::banner(argv[0]);
    }
    SetModes();
    RegExpContext regexp(argv[1], regularExpressions.GetValue(), !caseInSensitive.GetValue(), completeWords.GetValue());

    if (!regexp.IsValid())
    {
        fprintf(stderr, "Invalid regular expression");
        return 2;
    }

    CmdFiles files(argv + 2, recurseDirs.GetValue());

    int openCount = 0;
    int matchCount = 0;
    if (!isatty(fileno(stdin)))
    {
        matchCount += OneFile(regexp, "STDIN", std::cin, openCount);
    }
    else
        for (auto it = files.FileNameBegin(); it != files.FileNameEnd(); ++it)
        {
            std::fstream fil((*it), std::ios::in | std::ios::binary);
            if (fil.is_open())
                matchCount += OneFile(regexp, (*it), fil, openCount);
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

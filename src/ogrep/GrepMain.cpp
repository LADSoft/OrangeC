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

#include "GrepMain.h"
#include "CmdFiles.h"
#include "Utils.h"
#include <stdlib.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <Stdio.h>
#ifdef OPENWATCOM
namespace std
{
    ios &left(ios &in) { return in; }
}
#endif
CmdSwitchParser GrepMain::SwitchParser;

CmdSwitchBool GrepMain::recurseDirs(SwitchParser, 'd');
CmdSwitchBool GrepMain::caseInSensitive(SwitchParser, 'i', false);
CmdSwitchBool GrepMain::completeWords(SwitchParser, 'w');
CmdSwitchBool GrepMain::regularExpressions(SwitchParser, 'r',true);
CmdSwitchBool GrepMain::displayMatchCount(SwitchParser, 'c');
CmdSwitchBool GrepMain::displayFileNamesOnly(SwitchParser,'l');
CmdSwitchBool GrepMain::displayLineNumbers(SwitchParser, 'n');
CmdSwitchBool GrepMain::unixMode(SwitchParser,'o');
CmdSwitchBool GrepMain::displayNonMatching(SwitchParser, 'v');
CmdSwitchBool GrepMain::verboseMode(SwitchParser,'z');
CmdSwitchBool GrepMain::showHelp(SwitchParser, '?');
// not actual parameters
CmdSwitchBool GrepMain::displayFileNames(SwitchParser,'#');
CmdSwitchBool GrepMain::displayHeaderFileName(SwitchParser,'#');


char *GrepMain::usageText = "[-rxlcnvidzwo?] searchstring file[s]\n";
char *GrepMain::helpText = "[options] searchstring file[s]"
                    "\n"
                    "   -c  Show Match Count only        -d  Recurse Subdirectories\n"
                    "   -i  Case Insensitive             -l  Show File Names only\n"
                    "   -n  Show Line Numbers            -o  UNIX Output Format\n"
                    "   -r- No Regular Expressions       -v  Non Matching Lines\n"
                    "   -w  Complete Words Only          -z  Verbose\n"
                    "   -V  Show version and date        -?  This help\n"
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

int main(int argc, char **argv)
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
void GrepMain::DisplayMatch(const std::string &fileName, int &matchCount, int lineno, const char *text)
{
    if (matchCount == 0 && displayHeaderFileName.GetValue())
    {
        std::cout << "FILE: " << fileName.c_str();
        if (verboseMode.GetValue() || !displayMatchCount.GetValue())
            std::cout << std::endl;
    }
    if (!displayFileNamesOnly.GetValue())
    {
        const char *p = text;
        while (p[-1] && p[-1] != '\n')
            p--;
        const char *q = strchr(p, '\n');
        if (!q)
            q = p + strlen(p);
        char buf[5000];
        strncpy(buf, p, q-p);
        buf[q-p] = 0;
        if (displayFileNames.GetValue())
        {
            int n = fileName.size();
            n = ((n + 8) /8) * 8;
            std::cout << std::setfill(' ') << std::setw(n) << std::left << fileName.c_str();
        }
        if (displayLineNumbers.GetValue())
        {
            std::cout << std::setfill(' ') << std::setw(8) << std::left << lineno;
        }
        std::cout << buf << std::endl;
    }
    matchCount++;
}
void GrepMain::FindLine(const std::string fileName, int &matchCount, int &matchLine, char **matchPos, char *curpos, bool matched)
{
    char *p = *matchPos;
    do
    {
        p = strchr(p, '\n');
        if (!p)
            p = *matchPos + strlen(*matchPos);
        if (p <= curpos)
        {
            if (displayNonMatching.GetValue())
            {
                DisplayMatch(fileName, matchCount, matchLine, p);
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
        DisplayMatch(fileName, matchCount, matchLine, curpos);
    }
    if (*p)
    {
        p++;
        matchLine++;
    }
    *matchPos = p;
}
void GrepMain::OneFile(RegExpContext &regexp, const std::string &fileName, int &openCount)
{
    std::fstream fil(fileName.c_str(), std::ios::in | std::ios::binary);
    if (!fil.fail())
    {
        openCount ++;
        fil.seekg(0, std::ios::end);
        int length = fil.tellg();
        fil.seekg(0);
        char *buf = new char[length + 2];
        //if (buf)
        {
            buf[0] = 0;
            buf[length+1] = 0;
            int matchCount = 0;
            int lineno = 0;
            fil.read((char *)buf + 1, length);
            char *p = buf+1;
            for (int i=1; i < length+1; i++)
                if (buf[i] != '\r')
                    *p++ = buf[i];
            *p = 0;
            length = strlen(buf+1);
            if (!fil.fail())
            {
                char *str = buf + 1;
                char *matchPos = buf + 1;
                int matchLine = 1;
                bool matched = true;
                while (matched)
                {
                    char *p = nullptr;
                    matched = regexp.Match(str-buf, length, buf);
                    if (matched)
                    {
                        FindLine(fileName, matchCount, matchLine, &matchPos, buf +regexp.GetStart(), true);
                        p = strchr((char *)buf + regexp.GetEnd(), '\n');
                    }
                    else
                    {
                        FindLine(fileName, matchCount, matchLine, &matchPos, str + strlen(str), false);
                    }
                    if (!p)
                        p = str + strlen(str);
                    else
                        p++;
                    str = p;
                }
                if (matchCount && verboseMode.GetValue())
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
                else if (matchCount && displayMatchCount.GetValue())
                {
                        std::cout << ": " << matchCount << std::endl;
                }
            }
            delete[] buf;
        }
    }
}
int GrepMain::Run(int argc, char **argv)
{
    // handle /V switch
    for (int i = 1; i < __argc; i++)
        if (__argv[i] && (__argv[i][0] == '/' || __argv[i][0] == '-'))
            if (__argv[i][1] == 'V' && __argv[i][2] == 0)
            {
                Utils::banner(argv[0]);
                exit(0);
            }
    if (!SwitchParser.Parse(&argc, argv))
    {
        Utils::usage(argv[0], usageText);
    }
    if (showHelp.GetValue() || (argc >= 2 && !strcmp(argv[1], "?")))
    {
        Utils::banner(argv[0]);
        Utils::usage(argv[0], helpText);
        return 1;
    }
    if (argc < 3)
    {
        Utils::usage(argv[0], usageText);
    }
    if (verboseMode.GetValue())
    {
        Utils::banner(argv[0]);
    }
    SetModes();
    RegExpContext regexp(argv[1], regularExpressions.GetValue(), !caseInSensitive.GetValue(), completeWords.GetValue());

    if (!regexp.IsValid())
        Utils::fatal("Invalid regular expression");

    CmdFiles files(argv + 2, recurseDirs.GetValue());

    int openCount = 0;
    for (CmdFiles::FileNameIterator it = files.FileNameBegin(); it !=files.FileNameEnd(); ++it)
        OneFile(regexp, *(*it), openCount);
    if (openCount == 0)
    {
        std::cout << "Nothing to do." << std::endl;
    }
    return 0 ;
}

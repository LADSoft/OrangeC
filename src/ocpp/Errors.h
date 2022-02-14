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

#ifndef Errors_H
#define Errors_H

#include <string>

class ppInclude;

class Errors
{
  public:
    Errors() {}
    virtual ~Errors() {}

    static void ErrorWithLine(const std::string& msg, std::string& filname, int lineno);
    static void Error(const std::string& msg);
    static void Warning(const std::string& msg);
    static void WarningWithLine(const std::string& msg, std::string& filname, int lineno);
    static void TrivialWarning(const std::string& msg);
    static void TrivialWarningWithLine(const std::string& msg, std::string& filname, int lineno);
    static void Previous(const std::string& name, int lineNo, const std::string& file);
    static void SetShowWarnings(bool flag)
    {
        showWarnings = flag;
        if (!flag)
            showTrivialWarnings = false;
    }
    static void SetShowTrivialWarnings(bool flag)
    {
        if (showWarnings)
            showTrivialWarnings = flag;
    }
    static void WarningsAsErrors(bool flag) { warningsAsErrors = flag; }
    static bool ErrorCount();
    static void SetMaxErrors(int count) { maxErrors = count; }
    static void SetInclude(ppInclude* inc) { include = inc; }
    static std::string ToNum(long long num);
    static int GetErrorLine();
    static std::string GetFileName();
    static void IncrementCount() { errorCount++; }

    static bool GetErrorCount() { return errorCount; }
    static void Reset() { errorCount = warningCount = 0; }

  protected:
    static void FileName();

  private:
    static int maxErrors;
    static int errorCount;
    static int warningCount;
    static ppInclude* include;
    static bool showWarnings;
    static bool showTrivialWarnings;
    static bool warningsAsErrors;
};
#endif
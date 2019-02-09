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

#define _CRT_SECURE_NO_WARNINGS

#include "CmdSwitch.h"
#include "Utils.h"
#include <cctype>
#include <fstream>
#include <climits>
#include <cstdlib>
#include <cstring>

CmdSwitchBase::CmdSwitchBase(CmdSwitchParser& parser, char SwitchChar, std::string LongName) : exists(false), switchChar(SwitchChar), longName(LongName) { parser += this; }
int CmdSwitchBool::Parse(const char* data)
{
    if (data[0] != '-')
        value = true;
    else
        value = false;
    return (data[0] == '-' || data[0] == '+');
}
int CmdSwitchInt::Parse(const char* data)
{
    if (!*data)
        return INT_MAX;
    int cnt = 0;
    char number[256];
    if (data[0] == ':')
        data++, cnt++;
    strncpy(number, data, 255);
    number[255] = 0;

    char* p = number;
    while (isdigit(*p))
        p++;
    if (*p)
        return -1;
    value = Utils::StringToNumber(number);
    if (value < lowLimit || value > hiLimit)
        return -1;
    return p - number + cnt;
}
int CmdSwitchHex::Parse(const char* data)
{
    if (!*data)
        return INT_MAX;
    int cnt = 0;
    char number[256];
    if (data[0] == ':')
        data++, cnt++;
    strncpy(number, data + 1, 255);
    number[255] = 0;

    char* p = number;
    while (isxdigit(*p))
        p++;
    if (*p)
        return -1;
    value = Utils::StringToNumberHex(number);
    if (value < lowLimit || value > hiLimit)
        return -1;
    return p - number + cnt;
}
int CmdSwitchString::Parse(const char* data)
{
    int rv = strlen(data);
    if (data[0] == ':')
        data++;
    if (concat)
    {
        if (concat != '+' && !value.empty())
            value += concat;
        value += data;
    }
    else
    {
        value = data;
    }
    return rv;
}
int CmdSwitchCombineString::Parse(const char* data)
{
    if (data[0] == 0)
        return INT_MAX;  // go on to next arg
    return CmdSwitchString::Parse(data);
}
int CmdSwitchCombo::Parse(const char* data)
{
    int rv = CmdSwitchString::Parse(data);
    for (int i = 0; i < value.size(); i++)
        if (valid.find_first_of(value[i]) == std::string::npos)
            return -1;
    selected = true;
    return rv;
}
int CmdSwitchOutput::Parse(const char* data)
{
    int rv = CmdSwitchCombineString::Parse(data);
    if (rv != INT_MAX)
    {
        const char* p = value.c_str();
        const char* q = strrchr(p, '.');
        if (!q || q[-1] == '.')
            value += extension;
    }
    return rv;
}
CmdSwitchDefine::~CmdSwitchDefine()
{
    for (auto define : defines)
    {
        delete define;
    }
    defines.clear();
}
int CmdSwitchDefine::Parse(const char* data)
{
    int rv = strlen(data);
    char name[10000], *p = name;
    if (!isalpha(*data) && *data != '_')
        return -1;
    while (*data && (isalnum(*data) || *data == '_'))
        *p++ = *data++;
    *p = 0;
    if (*data && *data != '=')
    {
        return -1;
    }
    define* newDefine = new define;
    newDefine->name = name;
    if (*data == '=')
    {
        data++;
        p = name;
        while (*data)
        {
            *p++ = *data++;
        }
        *p = 0;
        newDefine->value = name;
    }
    else
    {
        newDefine->value = "1";
    }
    defines.push_back(newDefine);
    return rv;
}
CmdSwitchDefine::define* CmdSwitchDefine::GetValue(int index)
{
    if (defines.size() > (size_t)index)
        return defines[index];
    return nullptr;
}
int CmdSwitchFile::Parse(const char* data)
{
    int n = CmdSwitchString::Parse(data);
    if (n < 0 || argv)
        return n;
    std::fstream in(CmdSwitchString::GetValue(), std::ios::in);
    if (!in.fail())
    {
        in.seekg(0, std::ios::end);
        size_t size = in.tellg();
        in.seekg(0, std::ios::beg);
        char* data1 = new char[size + 1];
        memset(data1, 0, size + 1);
        in.read(data1, size);
        data1[size] = 0;
        in.close();
        Dispatch(data1);
        delete[] data1;
        return n;
    }
    return -1;
}
void CmdSwitchFile::Dispatch(char* data)
{
    int max = 10;
    argc = 1;
    argv = new char*[max];
    argv[0] = (char *)"";
    while (*data)
    {
        data = GetStr(data);
        if (argc == max)
        {
            max += 10;
            char** p = new char*[max + 1];
            memcpy(p, argv, argc * sizeof(char*));
            delete[] argv;
            argv = p;
        }
    }
    argv[argc] = 0;
    Parser->Parse(&argc, argv);
}
char* CmdSwitchFile::GetStr(char* data)
{
    char buf[10000], *p = buf;
    bool quote = false;
    while (isspace(*data))
        data++;
    if (data[0] == '\0')
        return data;
    if (*data == '"')
        quote = true, data++;
    while (*data && ((quote && *data != '"') || (!quote && !isspace(*data))))
        *p++ = *data++;
    *p = 0;
    if (quote && *data)
        data++;
    while ((p = strstr(buf, "%")))
    {
        char* q = strchr(p + 1, '%');
        if (q)
        {
            int len = q + 1 - p;
            char* name = new char[len - 1];
            memcpy(name, p + 1, len - 2);
            name[len - 2] = 0;
            char* env = getenv(name);
            delete[] name;
            if (env)
            {
                int len2 = strlen(env);
                if (len > len2)
                {
                    strcpy(p + len2, p + len);
                }
                else if (len < len2)
                {
                    memmove(p + len2, p + len, strlen(p + len) + 1);
                }
                memcpy(p, env, strlen(env));
            }
            else
            {
                strcpy(p, q + 1);
            }
        }
        else
            break;
    }
    char* x = new char[strlen(buf) + 1];
    strcpy(x, buf);
    argv[argc++] = x;
    return data;
}
CmdSwitchBase* CmdSwitchParser::Find(const char *name, bool useLongName)
{

    if (useLongName)
    {
        std::string bigmatch = "";
        int max = strlen(name);
        for (int i = max; i >= 1; i--)
        {
            for (auto s : switches)
            {
                if (!strncmp(name, s->GetLongName().c_str(), i))
                {
                    if (i == max && s->GetLongName().size() == max)
                        return s;
                    else if (!bigmatch.size())
                        bigmatch = s->GetLongName();
                }
            }
        }
        std::cerr << "Unknown switch '--" << name << "'" << std::endl;
        if (bigmatch.size())
        {
            std::cerr << "   Did you mean '--" << bigmatch << "'?" << std::endl;

        }
    }
    else
    {
        for (auto s : switches)
        {
            if (s->GetSwitchChar() == name[0])
                return s;
        }
        std::cerr << "Unknown switch '-" << name[0] << "'" << std::endl;
    }
    return nullptr;
}
bool CmdSwitchParser::Parse(int* argc, char* argv[])
{
    for (int i = 0, count = 0; *argv;)
    {
        // special casing '-' alone in an argv
        if (argv[0][0] == '@')  // meant to be a file loader
        {
            auto b = Find(&argv[0][0], false);
            if (!b)
                return false;
            b->Parse(&argv[0][1]);
            memmove(argv, argv + 1, (*argc + 1 - i) * sizeof(char*));
            (*argc)--;
        }
        else if ((argv[0][0] == '-' || argv[0][0] == '/') && argv[0][1] && (argv[0][1] != '-' || argv[0][2]))
        {
            const char *data = &argv[0][0];
            bool longName = data[1] == '-';
            data += 1 + longName;
            bool shifted = false;
            while (data[0] && !shifted)
            {
                CmdSwitchBase *b;
                if (longName)
                {
                    b = Find(data, true);
                    data += strlen(data);
                }
                else
                {
                    b = Find(data++, false);
                }
                if (!b)
                    return false;
                int n = b->Parse(data);
                b->SetArgNum(count++);
                b->SetExists();
                while (n == INT_MAX)
                {
                    if (!argv[1])
                    {
                        if (longName)
                        {
                            std::cerr << "switch '--" << b->GetLongName();
                        }
                        else
                        {
                            std::cerr << "switch '-" << b->GetSwitchChar();
                        }
                        std::cerr << "' requires argument" << std::endl;
                        return false;
                    }
                    shifted = true;
                    // use next arg as the value
                    memmove(argv, argv + 1, (*argc - i) * sizeof(char*));
                    (*argc)--;
                    data = &argv[0][0];
                    n = b->Parse(data);
                }
                if (n < 0)
                    return false;

                data += n;
            }
            // use next arg as the value
            memmove(argv, argv + 1, (*argc - i) * sizeof(char*));
            (*argc)--;
        }
        else
        {
            ++argv;
            ++i;
        }
    }
    return true;
}

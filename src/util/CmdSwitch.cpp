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

#define _CRT_SECURE_NO_WARNINGS

#include "CmdSwitch.h"
#include "Utils.h"
#include <ctype.h>
#include <fstream>
#include <limits.h>
#include <stdlib.h>

#include <string.h>

CmdSwitchBase::CmdSwitchBase(CmdSwitchParser& parser, char SwitchChar) : exists(false), switchChar(SwitchChar) { parser += this; }
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
    char number[256];
    if (data[0] != ':')
        return 0;
    strncpy(number, data + 1, 255);
    number[255] = 0;

    char* p = number;
    while (isdigit(*p))
        p++;
    if (*p)
        return -1;
    value = Utils::StringToNumber(number);
    if (value < lowLimit || value > hiLimit)
        return -1;
    return p - number + 1;
}
int CmdSwitchHex::Parse(const char* data)
{
    char number[256];
    if (data[0] != ':')
        return 0;
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
    return p - number + 1;
}
int CmdSwitchString::Parse(const char* data)
{
    int rv = strlen(data);
    if (data[0] == ':')
        data++;
    if (concat)
    {
        if (concat != '+' && value.size())
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
    //	if (!newDefine) // new return not nullptr or exception!
    //        return -1;
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
    std::fstream in(CmdSwitchString::GetValue().c_str(), std::ios::in);
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
    argv[0] = "";
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
bool CmdSwitchParser::Parse(int* argc, char* argv[])
{
    for (int i = 0; *argv;)
    {
        // special casing '-' alone in an argv
        if (argv[0][0] == '@')  // meant to be a file loader
        {
            CmdSwitchBase temp('@');
            auto it = switches.find(&temp);
            if (it == switches.end())
                return false;
            (*it)->Parse(&argv[0][1]);
            memcpy(argv, argv + 1, (*argc + 1 - i) * sizeof(char*));
            (*argc)--;
        }
        else if ((argv[0][0] == '-' || argv[0][0] == '/') && argv[0][1])
        {
            if (argv[0][1] == '!' || !strcmp(argv[0], "--nologo"))
            {
                // skip the banner nondisplay arg
            }
            else
            {
                const char* data = &argv[0][1];
                const char* end = data + strlen(data);
                while (data < end)
                {
                    CmdSwitchBase temp(*data);
                    auto it = switches.find(&temp);
                    if (it == switches.end())
                        return false;
                    data++;
                    int n = (*it)->Parse(data);
                    while (n == INT_MAX && argv[1])
                    {
                        // use next arg as the value
                        memcpy(argv, argv + 1, (*argc - i) * sizeof(char*));
                        (*argc)--;
                        data = &argv[0][0];
                        end = data + strlen(data);
                        n = (*it)->Parse(data);
                    }
                    if (n < 0)
                        return false;
                    int t = strlen(data);
                    if (t < n)
                        return false;
                    (*it)->SetExists();
                    data += n;
                }
            }
            memcpy(argv, argv + 1, (*argc - i) * sizeof(char*));
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

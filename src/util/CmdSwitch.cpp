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

CmdSwitchDefine::~CmdSwitchDefine() {}
int CmdSwitchDefine::Parse(const char* data)
{
    if (!data[0])
        return INT_MAX;
    int str=0;
    int rv = strlen(data);
    char name[10000], *p = name;
    if (data[0] == '"')
        str = *data++;
    if (!isalpha(*data) && *data != '_')
        return -1;
    while (*data && (isalnum(*data) || *data == '_'))
        *p++ = *data++;
    *p = 0;
    if (*data && *data != '=')
    {
        return -1;
    }
    std::unique_ptr<define> newDefine = std::make_unique<define>();
    newDefine->name = name;
    if (*data == '=')
    {
        data++;
        p = name;
        while (*data && *data != str)
        {
            *p++ = *data++;
        }
        *p = 0;
        if (*data)
            data++;
        newDefine->value = name;
    }
    else
    {
        newDefine->value = "1";
    }
    defines.push_back(std::move(newDefine));
    return rv;
}
CmdSwitchDefine::define* CmdSwitchDefine::GetValue(int index)
{
    if (defines.size() > (size_t)index)
        return defines[index].get();
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
        std::unique_ptr<char[]> data1 = std::make_unique<char[]>(size + 1);
        memset(data1.get(), 0, size + 1);
        in.read(data1.get(), size);
        data1[size] = 0;
        in.close();
        Dispatch(data1.get());
        return n;
    }
    return -1;
}
void CmdSwitchFile::Dispatch(char* data)
{
    int max = 10;
    argc = 1;
    argv = std::make_unique<char* []>(max);
    argv[0] = (char*)"";
    while (*data)
    {
        data = GetStr(data);
        if (argc == max)
        {
            max += 10;
            std::unique_ptr<char* []> p = std::move(argv);
            argv = std::make_unique<char* []>(max);
            memcpy(argv.get(), p.get(), argc * sizeof(char*));
        }
    }
    argv[argc] = 0;
    Parser->Parse(&argc, argv.get());
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
        const char *s = strchr(name,'=');
        if (s && s - name < max)
            max = s - name;
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
        else if ((argv[0][0] == '-' || argv[0][0] == '/' || (argv[0][0] == '+' && argv[0][1] != '-')) && argv[0][1] && (argv[0][1] != '-' || argv[0][2]))
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
                    const char *p = strchr(data, '=');
                    if (p)
                        data = p+1;
                    else
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
void CmdSwitchParser::ScanEnv(char* output, const char* string)
{
    char name[256], *p;
    while (*string)
    {
        if (*string == '%')
        {
            p = name;
            string++;
            while (*string && *string != '%')
                *p++ = *string++;
            if (*string)
                string++;
            *p = 0;
            p = getenv(name);
            if (p)
            {
                strcpy(output, p);
                output += strlen(output);
            }
        }
        else
            *output++ = *string++;
    }
    *output = 0;
}

bool CmdSwitchParser::Parse(const std::string &val, int *argc, char *argv[])
{
    char output[1024], *string = output;
    int rv, i;
    if (val.size() == 0)
        return true;
    ScanEnv(output, val.c_str());
    while (1)
    {
        int quoted = ' ';
        while (*string == ' ')
            string++;
        if (!*string)
            break;
        if (*string == '\"')
            quoted = *string++;
        argv[(*argc)++] = string;
        while (*string && *string != quoted)
            string++;
        if (!*string)
            break;
        *string = 0;
        string++;
    }
    argv[*argc] = nullptr;
    return Parse(argc, argv);

}

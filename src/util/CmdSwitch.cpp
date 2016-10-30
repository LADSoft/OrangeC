/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include "CmdSwitch.h"
#include "Utils.h"
#include <ctype.h>
#include <fstream>

CmdSwitchBase::CmdSwitchBase(CmdSwitchParser &parser, char SwitchChar) : switchChar(SwitchChar)
{
    parser += this;
}
int CmdSwitchBool::Parse(const char *data)
{
    if (data[0] != '-')
        value = true;
    else
        value = false;
    return (data[0] == '-' || data[0] == '+');
}
int CmdSwitchInt::Parse(const char *data)
{
    char number[256];
    if (data[0] != ':')
        return -1;
    strncpy(number, data+1, 255);
    number[255] = 0;
    
    char *p = number;
    while (isdigit(*p))
        p++;
    if (*p)
        return -1;
    value = Utils::StringToNumber(number);
    if (value < lowLimit || value > hiLimit)
        return -1;
    return p - number;
}
int CmdSwitchHex::Parse(const char *data)
{
    char number[256];
    if (data[0] != ':')
        return -1;
    strncpy(number, data+1, 255);
    number[255] = 0;
    
    char *p = number;
    while (isxdigit(*p))
        p++;
    if (*p)
        return -1;
    value = Utils::StringToNumberHex(number);
    if (value < lowLimit || value > hiLimit)
        return -1;
    return p - number+1;
}
int CmdSwitchString::Parse(const char *data)
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
int CmdSwitchCombo::Parse(const char *data)
{
    int rv = CmdSwitchString::Parse(data);
    for (int i=0; i < value.size(); i++)
        if (valid.find_first_of(value[i]) == std::string::npos)
            return -1;
    selected = true;
    return rv;
}
int CmdSwitchOutput::Parse(const char *data)
{
    int rv = CmdSwitchString::Parse(data);
    const char *p = value.c_str();
    const char *q = strrchr(p, '.');
    if (!q || q[-1] == '.')
        value += extension;
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
int CmdSwitchDefine::Parse(const char *data)
{
    int rv = strlen(data);
    char name[512],*p = name;
    if (!isalpha(*data) && *data != '_')
        return -1;
    while (*data && (isalnum(*data) || *data == '_'))
        *p++ = *data++;
    *p = 0;
    if (*data && *data != '=')
    {
        return -1;
    }
    define *newDefine = new define;
//	if (!newDefine) // new return not nullptr or exception!
//        return -1;
    newDefine->name = name;
    if (*data == '=')
    {
        data ++;
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
CmdSwitchDefine::define *CmdSwitchDefine::GetValue(int index)
{
    if (defines.size() > (size_t)index)
        return defines[index];
    return nullptr;		
}
int CmdSwitchFile::Parse(const char *data)
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
        char *data = new char[size + 1];
        memset(data, 0, size + 1);
        in.read(data, size);
        data[size] = 0;
        in.close();
        Dispatch(data);
        delete[] data;
        return n;
    }
    else
    {
        return -1;
    }
}
void CmdSwitchFile::Dispatch(char *data)
{
    int max = 10;
    argc = 1;
    argv = new char *[max];
    argv[0] = "";
    while (*data)
    {
        data = GetStr(data);
        if (argc == max)
        {
            max += 10;
            char **p = new char *[max];
            memcpy(p, argv, argc * sizeof(char *));
            delete [] argv;
            argv = p;
        }
    }
    argv[argc] = 0;
    Parser->Parse(&argc, argv);
}
char * CmdSwitchFile::GetStr(char *data)
{
    char buf[10000], *p = buf;
    bool quote = false;
    while (isspace(*data))
        data++;
    if (data[0] == '\0')
        return data;
    if (*data == '"')
        quote= true, data++;
    while (*data && ((quote && *data != '"') || (!quote && !isspace(*data))))
        *p++ = *data++;
    *p = 0;
    if (quote && *data)
        data ++;
    while ((p = strstr(buf, "%")))
    {
        char *q = strchr(p+1, '%');
        if (q)
        {
            int len = q +1 - p;
            char *name = new char[len-1];
            memcpy(name, p + 1, len - 2);
            name[len-2] = 0;
            char *env = getenv(name);
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
                strcpy(p, q+1);
            }
        }
        else
            break;
    }
    char *x = new char [strlen(buf) + 1];
    strcpy(x,buf);
    argv[argc++] = x;
    return data;
}
bool CmdSwitchParser::Parse(int *argc, char *argv[])
{
    for (int i =0 ; *argv; )
    {
        // special casing '-' alone in an argv
        if (argv[0][0] == '@') // meant to be a file loader
        {
            const char *data = &argv[0][1];
            CmdSwitchBase temp('@');
            auto it = switches.find(&temp);
            if (it == switches.end())
                return false;
            (*it)->Parse(&argv[0][1]);
            memcpy(argv, argv+1, (*argc + 1 -i) * sizeof(char *));
            (*argc)--;
        }
        else if ((argv[0][0] == '-' || argv[0][0] == '/') && argv[0][1])
        {
            if (argv[0][1] == '!')
            {
                // skip the banner nondisplay arg
            }
            else
            {
                const char *data = &argv[0][1];
                const char *end = data + strlen(data);
                while (data < end)
                {
                    CmdSwitchBase temp(*data);
                    auto it = switches.find(&temp);
                    if (it == switches.end())
                        return false;
                    data++;
                    int n = (*it)->Parse(data);
                    if (n < 0)
                        return false;
                    int t = strlen(data);
                    if (t < n)
                        return false;
                    data += n;
                }
            }
            memcpy(argv, argv+1, (*argc + 1 -i) * sizeof(char *));
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

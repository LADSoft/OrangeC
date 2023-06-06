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

#include "xml.h"
#include <cctype>
#include <cstring>

int xmlNode::lineno = 1;

inline bool isspaceline(char ch)
{
    if (ch == '\n')
        xmlNode::IncLine();
    return isspace(ch);
}
bool xmlAttrib::Read(std::fstream& stream)
{
    char t;
    stream >> t;
    if (stream.fail())
        return false;
    while (isspaceline(t))
    {
        stream >> t;
        if (stream.fail())
            return false;
    }
    if (isalpha(t) || t == '_')
    {
        char buf[512];
        char* p = buf;
        buf[0] = 0;
        while (isalnum(t) || t == '_')
        {
            *p++ = t;
            stream >> t;
            if (stream.fail())
                return false;
        }
        *p = 0;
        name = buf;
        while (isspaceline(t))
        {
            stream >> t;
            if (stream.fail())
                return false;
        }
        if (t != '=')
            return false;
        stream >> t;
        if (stream.fail())
            return false;
        while (isspaceline(t))
        {
            stream >> t;
            if (stream.fail())
                return false;
        }
        if (t != '\"')
            return false;
        if (!ReadTextString(stream, value))
            return false;
    }
    return true;
}
bool xmlAttrib::Write(std::fstream& stream)
{
    stream << ' ' << name << " = \"";
    const char* p = value.c_str();
    while (*p)
        WriteTextChar(stream, *p++);
    stream << "\"";
    return !stream.fail();
}
bool xmlAttrib::IsSpecial(char t) { return t == '>' || t == '<' || t == '=' || t == '&' || t == '\'' || t == '\"'; }
void xmlAttrib::WriteTextChar(std::fstream& stream, char t)
{
    switch (t)
    {
        case '"':
            stream << "&quot;";
            break;
        case '\'':
            stream << "&apos;";
            break;
        case '&':
            stream << "&amp;";
            break;
        case '<':
            stream << "&lt;";
            break;
        case '>':
            stream << "&gt;";
            break;
        default:
            stream << t;
            break;
    }
}
bool xmlAttrib::ReadTextString(std::fstream& stream, std::string& str)
{
    char buf[512], *p = buf;
    while (!stream.fail())
    {
        char t;
        stream >> t;
        if (t == '&')
        {
            t = ReadTextChar(stream);
            if (!t)
                return false;
        }
        else if (t == '"')
        {
            *p = 0;
            str = buf;
            return true;
        }
        *p++ = t;
    }
    return false;
}
char xmlAttrib::ReadTextChar(std::fstream& stream)
{
    char buf[10];
    char* p = buf;
    for (int i = 0; i < 10; i++)
    {
        stream >> *p++;
        if (stream.fail())
            return 0;
        if (p[-1] == ';')
        {
            *--p = '\0';
            if (!strcmp(buf, "amp"))
                return '&';
            if (!strcmp(buf, "lt"))
                return '<';
            if (!strcmp(buf, "gt"))
                return '>';
            if (!strcmp(buf, "apos"))
                return '\'';
            if (!strcmp(buf, "quot"))
                return '"';
            return 0;
        }
    }
    return 0;
}

xmlNode::~xmlNode() {}
bool xmlNode::Read(std::fstream& stream, char v)
{
    char t;
    stream.unsetf(std::ios::skipws);
    if (v)
        t = v;
    else
    {
        stream >> t;
        if (stream.fail())
            return false;
        while (isspaceline(t))
        {
            stream >> t;
            if (stream.fail())
                return false;
        }
        if (t != '<')
            return false;
        stream >> t;
        if (stream.fail())
            return false;
        while (isspaceline(t))
        {
            stream >> t;
            if (stream.fail())
                return false;
        }
    }
    if (!isalpha(t) && t != '_')
        return false;
    char buf[512], *p = buf;
    buf[0] = 0;
    while (isalnum(t) || t == '_')
    {
        *p++ = t;
        stream >> t;
        if (stream.fail())
            return false;
    }
    *p = 0;
    elementType = buf;
    while (!stream.fail())
    {
        while (isspaceline(t))
        {
            stream >> t;
            if (stream.fail())
                return false;
        }
        if (isalpha(t) || t == '_')
        {
            stream.putback(t);
            auto attrib = std::make_unique<xmlAttrib>();
            // if (!attrib)
            //    return false;
            if (!attrib->Read(stream))
            {
                return false;
            }
            InsertAttrib(attrib);
            stream >> t;
            if (stream.fail())
                return false;
        }
        else
            break;
    }
    if (t == '/')
    {
        stream >> t;
        if (stream.fail())
            return false;
        return (t == '>');
    }
    else if (t == '>')
    {
        while (!stream.fail())
        {
            stream >> t;
            while (isspaceline(t))
            {
                stream >> t;
                if (stream.fail())
                    return false;
            }
            stream.putback(t);
            if (!ReadTextString(stream, text))
                return false;
            stream >> t;
            if (stream.fail())
                return false;
            if (t == '/')
            {
                stream >> t;
                if (stream.fail())
                    return false;
                while (isspaceline(t))
                {
                    stream >> t;
                    if (stream.fail())
                        return false;
                }
                if (!isalpha(t) && t != '_')
                    return false;
                p = buf;
                buf[0] = 0;
                while (isalnum(t) || t == '_')
                {
                    *p++ = t;
                    stream >> t;
                    if (stream.fail())
                        return false;
                }
                *p = 0;
                if (elementType != buf)
                    return false;
                while (isspaceline(t))
                {
                    stream >> t;
                    if (stream.fail())
                        return false;
                }
                Strip();
                return t == '>';
            }
            else
            {
                if (t == '!')
                {
                    // handle comments;
                    stream >> t;
                    if (t != '-')
                        return false;
                    stream >> t;
                    if (t != '-')
                        return false;
                    int ct = 0;
                    while (!stream.fail())
                    {
                        stream >> t;
                        if (t == '-')
                        {
                            if (++ct >= 2)
                            {
                                stream >> t;
                                if (t == '>')
                                    break;
                            }
                        }
                        else
                            ct = 0;
                    }
                    if (stream.fail())
                        return false;
                }
                else
                {
                    auto node = std::make_unique<xmlNode>();
                    // if (!node)
                    //    return false;
                    if (!node->Read(stream, t))
                    {
                        return false;
                    }
                    InsertChild(node);
                    if (!ReadTextString(stream, text))
                        return false;
                    stream.putback('<');
                }
            }
        }
    }
    return false;
}
void xmlNode::Strip()
{
    if (stripSpaces)
    {
        const char* p = text.c_str();
        const char *q = p, *r = p + strlen(p);
        while (*q && isspaceline(*q))
            q++;
        while (r > p && isspaceline(*(r - 1)))
            r--;
        if (r > q)
            text = text.substr(q - p, r - p);
        else
            text = "";
    }
}
bool xmlNode::ReadTextString(std::fstream& stream, std::string& str)
{
    char buf[512], *p = buf;
    while (!stream.fail())
    {
        char t;
        stream >> t;
        if (t == '&')
        {
            t = ReadTextChar(stream);
            if (!t)
                return false;
            *p++ = t;
        }
        else if (t == '<')
        {
            *p = 0;
            str += buf;
            return true;
        }
        else if (t == '\n')
        {
            *p++ = ' ';
            lineno++;
        }
    }
    return false;
}
bool xmlNode::Write(std::fstream& stream, int indent)
{
    for (int i = 0; i < indent; i++)
        stream << "  ";
    stream << '<' << elementType;
    if (!attribs.empty())
    {
        for (auto& attrib : attribs)
            attrib->Write(stream);
    }
    if (!children.empty() || !text.empty())
    {
        stream << '>' << std::endl;
        for (auto& child : children)
            child->Write(stream, indent + 1);
        if (!text.empty())
        {
            const char* p = text.c_str();
            while (*p)
                WriteTextChar(stream, *p++);
            stream << std::endl;
        }
        for (int i = 0; i < indent; i++)
            stream << "  ";
        stream << "</" << elementType << '>' << std::endl;
    }
    else
    {
        stream << "/>" << std::endl;
    }
    return !stream.fail();
}
void xmlNode::WriteTextChar(std::fstream& stream, char t)
{
    switch (t)
    {
        case '&':
            stream << "&amp;";
            break;
        case '<':
            stream << "&lt;";
            break;
        case '>':
            stream << "&gt;";
            break;
        default:
            stream << t;
            break;
    }
}
void xmlNode::RemoveAttrib(const xmlAttrib* attrib)
{
    for (auto it = attribs.begin(); it != attribs.end(); ++it)
    {
        if ((*it).get() == attrib)
        {
            attribs.erase(it);
        }
    }
}
void xmlNode::RemoveChild(const xmlNode* child)
{
    for (auto it = children.begin(); it != children.end(); ++it)
    {
        if ((*it).get() == child)
        {
            children.erase(it);
        }
    }
}
bool xmlNode::Visit(xmlVisitor& v, void* userData)
{
    for (auto& attrib : attribs)
    {
        if (!v.VisitAttrib(*this, attrib.get(), userData))
            break;
    }
    for (auto& child : children)
    {
        if (!v.VisitNode(*this, child.get(), userData))
            return false;
    }
    return true;
}

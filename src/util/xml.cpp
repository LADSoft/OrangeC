/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
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
#include "xml.h"
#include "ctype.h"
#include <string.h>
bool xmlAttrib::Read(std::fstream &stream)
{
    char t;
    stream >> t;
    if (stream.fail()) return false;
    while (isspace(t))
    {
        stream >> t;
        if (stream.fail()) return false;
    }
    if (isalpha(t)|| t=='_')
    {
        char buf[512];
        char *p = buf;
        buf[0] = 0;
        while (isalnum(t)|| t=='_')
        {
            *p++ = t;
            stream >> t;
            if (stream.fail()) return false;
        }
        *p = 0;
        name = buf;
        while (isspace(t))
        {
            stream >> t;
            if (stream.fail()) return false;
        }
        if (t != '=')
            return false;		
        stream >> t;
        if (stream.fail()) return false;
        while (isspace(t))
        {
            stream >> t;
            if (stream.fail()) return false;
        }
        if (t != '\"')
            return false;
        if (!ReadTextString(stream, value))
            return false;
    }
    return true;
}
bool xmlAttrib::Write(std::fstream &stream)
{
    stream << ' ' << name.c_str() << " = \"";
    const char *p = value.c_str();
    while(*p)
        WriteTextChar(stream, *p++);
    stream << "\"";
    return !stream.fail();
}
bool xmlAttrib::IsSpecial(char t)
{
    return t == '>' || t == '<' || t == '=' || t == '&' || t == '\'' || t == '\"' ;
}
void xmlAttrib::WriteTextChar(std::fstream &stream, char t)
{
    switch(t) {
        case '"' :
            stream << "&quot;";
            break ;
        case '\'':
            stream << "&apos;";
            break ;
        case '&':
            stream << "&amp;";
            break ;
        case '<':
            stream << "&lt;";
            break ;
        case '>':
            stream << "&gt;";
            break ;
        default:
            stream << t;
            break ;
    }
}
bool xmlAttrib::ReadTextString(std::fstream &stream, std::string &str)
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
char xmlAttrib::ReadTextChar(std::fstream &stream)
{
    char buf[10];
    char *p = buf;
    for (int i=0; i < 10; i++)
    {
        stream >> *p++;
        if (stream.fail())
            return 0;
        if (p[-1] == ';')
        {
            *--p= '\0';
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

xmlNode::~xmlNode()
{
    for (std::deque<xmlAttrib *>::iterator it = attribs.begin(); it != attribs.end(); ++it)
    {
        xmlAttrib *a = (*it);
        delete a;
    }
    attribs.clear();
    for (std::deque<xmlNode *>::iterator it = children.begin(); it != children.end(); ++it)
    {
        xmlNode *n = (*it);
        delete n;
    }
    children.clear();
}
bool xmlNode::Read(std::fstream &stream, char v)
{
    char t;
    stream.unsetf(std::ios::skipws);
    if (v)
        t = v;
    else
    {
        stream >> t;
        if (stream.fail()) return false;
        while (isspace(t))
        {
            stream >> t;
            if (stream.fail()) return false;
        }
        if (t != '<')
            return false;
        stream >> t;
        if (stream.fail()) return false;
        while (isspace(t))
        {
            stream >> t;
            if (stream.fail()) return false;
        }
    }
    if (!isalpha(t)&& t!='_')
        return false;
    char buf[512], *p = buf;
    buf[0] = 0;
    while (isalnum(t)|| t=='_')
    {
        *p ++ = t;
        stream >> t;
        if (stream.fail()) return false;
    }
    *p = 0;
    elementType = buf;
//	std::cout << elementType << std::endl;
    while (!stream.fail())
    {
        while (isspace(t))
        {
            stream >> t;
            if (stream.fail()) return false;
        }
        if (isalpha(t)|| t=='_')
        {
            stream.putback(t);
            xmlAttrib *attrib = new xmlAttrib();
            //if (!attrib)
            //    return false;
            if (!attrib->Read(stream))
            {
                delete attrib;
                return false;
            }
            InsertAttrib(attrib);
//			std::cout << attrib->GetName() << ": " << attrib->GetValue() << std::endl;
            stream >> t;
            if (stream.fail()) return false;		
        }
        else
            break;
    }	
    if (t == '/')
    {
        stream >> t;
        if (stream.fail()) return false;
        return (t == '>');
    }
    else if (t == '>')
    {
        while (!stream.fail())
        {		
            stream >> t;
            while (isspace(t))
            {
                stream >> t;
                if (stream.fail()) return false;
            }
            stream.putback(t);
            if (!ReadTextString(stream, text))
                return false;
            stream >> t;
            if (stream.fail()) return false;
            if (t == '/')
            {
                stream >> t;
                if (stream.fail()) return false;
                while (isspace(t))
                {
                    stream >> t;
                    if (stream.fail()) return false;
                }
                if (!isalpha(t)&& t!='_')
                    return false;
                p = buf;
                buf[0] = 0;
                while (isalnum(t)|| t=='_')
                {
                    *p ++ = t;
                    stream >> t;
                    if (stream.fail()) return false;
                }
                *p = 0;
                if (elementType != buf)
                    return false;
                while (isspace(t))
                {
                    stream >> t;
                    if (stream.fail()) return false;
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
                    if (t != '-') return false;
                    stream >> t;
                    if (t != '-') return false;
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
                    xmlNode *node = new xmlNode();
                    //if (!node)
                    //    return false;
                    if (!node->Read(stream, t))
                    {
                        delete node;
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
        const char *p = text.c_str();
        const char *q = p, *r = p + strlen(p);
        while (*q && isspace(*q))
            q++;
        while (r > p && isspace(*(r-1)))
            r--;
        if (r > q)
            text = text.substr(q - p, r - p);
        else
            text = "";
    }
}
bool xmlNode::ReadTextString(std::fstream &stream, std::string &str)
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
            *p++ = ' ';
    }
    return false;
}
bool xmlNode::Write(std::fstream &stream, int indent)
{
    for (int i=0; i < indent; i++)
        stream << "  " ;
    stream << '<' << elementType.c_str();
    if (attribs.size())
    {
        std::deque<xmlAttrib *>::iterator it;
        for (it= attribs.begin(); it != attribs.end(); ++it)
            (*it)->Write(stream);
    }
    if (children.size() || text.size())
    {
        stream << '>' << std::endl;
        std::deque<xmlNode *>::iterator it;
        for (it= children.begin(); it != children.end(); ++it)
            (*it)->Write(stream, indent + 1);
        if (text.size())
        {
            const char *p = text.c_str();
            while (*p)
                WriteTextChar(stream, *p++);
            stream << std::endl;
        }
        for (int i=0; i < indent; i++)
            stream << "  ";
        stream << "</" << elementType.c_str() << '>' << std::endl;
    }
    else
    {
        stream << "/>" << std::endl;
    }
    return !stream.fail();
}
void xmlNode::WriteTextChar(std::fstream &stream, char t)
{
    switch(t) {
        case '&':
            stream << "&amp;";
            break ;
        case '<':
            stream << "&lt;";
            break ;
        case '>':
            stream << "&gt;";
            break ;
        default:
            stream << t;
            break ;
    }
}
void xmlNode::RemoveAttrib(const xmlAttrib *attrib)
{
    for (std::deque<xmlAttrib *>::iterator it = attribs.begin(); it != attribs.end(); ++it)
    {
        if (*it == attrib)
        {
            delete *it;
            attribs.erase(it);
        }
    }
}
void xmlNode::RemoveChild(const xmlNode *child)
{
    for (std::deque<xmlNode *>::iterator it = children.begin(); it != children.end(); ++it)
    {
        if (*it == child)
        {
            delete *it;
            children.erase(it);
        }
    }
}
bool xmlNode::Visit(xmlVisitor &v, void *userData)
{
    for (std::deque<xmlAttrib *>::iterator it = attribs.begin(); it != attribs.end(); ++it)
    {
        if (!v.VisitAttrib(*this, *it, userData))
            break;
    }
    for (std::deque<xmlNode *>::iterator it = children.begin(); it != children.end(); ++it)
    {
        if (!v.VisitNode(*this, *it, userData))
            return false;
    }
    return true;
}

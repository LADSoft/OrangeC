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
#ifndef XML_H
#define XML_H

#include <string>
#include <deque>
#include <fstream>
class xmlAttrib
{
public:
    xmlAttrib() { }
    xmlAttrib(const char *Name, const char *Value) : name(Name), value(Value) { }
    xmlAttrib(const std::string &Name, const std::string &Value) : name(Name), value(Value) { }
    bool operator ==(const char *Name) const { return name == Name; }
    bool operator ==(const std::string &Name) const { return name == Name; }
    bool operator !=(const char *Name) const { return name != Name; }
    bool operator !=(const std::string &Name) const { return name != Name; }
    const std::string &GetValue() const { return value; }
    const std::string &GetName() const { return name; }
    bool Read(std::fstream &stream);
    bool Write(std::fstream &stream);
    static bool IsSpecial(char t);
    static void WriteTextChar(std::fstream &stream, char t);
    static bool ReadTextString(std::fstream &stream, std::string &name);
    static char ReadTextChar(std::fstream &stream);
private:
    std::string name;
    std::string value;
};
class xmlNode;
class xmlVisitor
{
public:
    virtual bool VisitAttrib(xmlNode &node, xmlAttrib *attrib, void *userData) { return false; };
    virtual bool VisitNode(xmlNode &node, xmlNode *child, void *userData) { return false; };
} ;
class xmlNode
{
public:
    xmlNode() : stripSpaces(true) { }
    xmlNode(const std::string &Type) : elementType(Type), stripSpaces(true) { }
    virtual ~xmlNode();
    bool Read(std::fstream &stream, char v = 0);
    bool Write(std::fstream &tream, int indent = 0);
    
    void InsertAttrib( xmlAttrib *attrib) { attribs.push_back(attrib); }
    void InsertChild(xmlNode *child) { children.push_back(child); }
    void RemoveAttrib(const xmlAttrib *attrib);
    void RemoveChild(const xmlNode *child);
    bool operator ==(const char *Name) const { return elementType == Name; }
    bool operator ==(const std::string &Name) const { return elementType == Name; }
    bool operator !=(const char *Name) const { return elementType != Name; }
    bool operator !=(const std::string &Name) const { return elementType != Name; }
    void SetText(const std::string &Text) { text = Text; }
    const std::string &GetText() const { return text; }
    const std::string &GetName() const { return elementType; }
    bool Visit(xmlVisitor &v, void *userData=NULL);
    void SetStripSpaces(bool flag) { stripSpaces = flag; }
    bool GetStripSpaces() const { return stripSpaces; }
    void Strip();

    static bool IsSpecial(char t) { return xmlAttrib::IsSpecial(t); }
    static bool ReadTextString(std::fstream &stream, std::string &str);
    static void WriteTextChar(std::fstream &stream, char t);
    static char ReadTextChar(std::fstream &stream) { return xmlAttrib::ReadTextChar(stream); }
private:
    std::string elementType;
    std::string text;
    std::deque<xmlAttrib *> attribs;
    std::deque<xmlNode *> children;
    bool stripSpaces;
};
#endif
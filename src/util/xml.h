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

#ifndef XML_H
#define XML_H

#include <string>
#include <deque>
#include <fstream>
#include <memory>
class xmlAttrib
{
  public:
    xmlAttrib() {}
    xmlAttrib(const char* Name, const char* Value) : name(Name), value(Value) {}
    xmlAttrib(const std::string& Name, const std::string& Value) : name(Name), value(Value) {}
    bool operator==(const char* Name) const { return name == Name; }
    bool operator==(const std::string& Name) const { return name == Name; }
    bool operator!=(const char* Name) const { return name != Name; }
    bool operator!=(const std::string& Name) const { return name != Name; }
    const std::string& GetValue() const { return value; }
    const std::string& GetName() const { return name; }
    bool Read(std::fstream& stream);
    bool Write(std::fstream& stream);
    static bool IsSpecial(char t);
    static void WriteTextChar(std::fstream& stream, char t);
    static bool ReadTextString(std::fstream& stream, std::string& name);
    static char ReadTextChar(std::fstream& stream);

  private:
    std::string name;
    std::string value;
};
class xmlNode;
class xmlVisitor
{
  public:
    virtual bool VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData) { return false; };
    virtual bool VisitNode(xmlNode& node, xmlNode* child, void* userData) { return false; };
};
class xmlNode
{
  public:
    xmlNode() : stripSpaces(true) {}
    xmlNode(const std::string& Type) : elementType(Type), stripSpaces(true) {}
    virtual ~xmlNode();
    bool Read(std::fstream& stream, char v = 0);
    bool Write(std::fstream& tream, int indent = 0);

    void InsertAttrib(std::unique_ptr<xmlAttrib>& attrib) { attribs.push_back(std::move(attrib)); }
    void InsertChild(std::unique_ptr<xmlNode>& child) { children.push_back(std::move(child)); }
    void RemoveAttrib(const xmlAttrib* attrib);
    void RemoveChild(const xmlNode* child);
    bool operator==(const char* Name) const { return elementType == Name; }
    bool operator==(const std::string& Name) const { return elementType == Name; }
    bool operator!=(const char* Name) const { return elementType != Name; }
    bool operator!=(const std::string& Name) const { return elementType != Name; }
    void SetText(const std::string& Text) { text = Text; }
    const std::string& GetText() const { return text; }
    const std::string& GetName() const { return elementType; }
    bool Visit(xmlVisitor& v, void* userData = nullptr);
    void SetStripSpaces(bool flag) { stripSpaces = flag; }
    bool GetStripSpaces() const { return stripSpaces; }
    void Strip();
    static int Line() { return lineno; }
    static void Reset() { lineno = 1; }
    static void IncLine() { lineno++; }
    static bool IsSpecial(char t) { return xmlAttrib::IsSpecial(t); }
    static bool ReadTextString(std::fstream& stream, std::string& str);
    static void WriteTextChar(std::fstream& stream, char t);
    static char ReadTextChar(std::fstream& stream) { return xmlAttrib::ReadTextChar(stream); }

  private:
    std::string elementType;
    std::string text;
    std::deque<std::unique_ptr<xmlAttrib>> attribs;
    std::deque<std::unique_ptr<xmlNode>> children;
    bool stripSpaces;
    static int lineno;
};
#endif
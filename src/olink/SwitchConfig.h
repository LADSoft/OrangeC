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

#ifndef SWITCHCONFIG_H
#define SWITCHCONFIG_H

#include <vector>
#include <deque>
#include <string>
#include "LinkManager.h"
#include "CmdSwitch.h"
#include "xml.h"

class SwitchConfig;
class ConfigData : public xmlVisitor
{
    friend class SwitchConfig;

  public:
    ConfigData(xmlNode* node) : selected(false), currentDefine(nullptr), relFile(false), mapMode(0), debugPassThrough(false)
    {
        node->Visit(*this);
    }
    virtual ~ConfigData();
    void parse(const xmlNode* node);
    static void AddDefine(LinkManager& linker, const std::string& name, const std::string& value);
    void SetDefines(LinkManager& linker);
    virtual bool VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData);
    virtual bool VisitNode(xmlNode& node, xmlNode* child, void* userData);
    std::string name;
    std::string app;
    std::string appFlags;
    std::string specFile;
    std::deque<std::string> extensions;
    std::vector<std::unique_ptr<CmdSwitchDefine::define>> defines;
    CmdSwitchDefine::define* currentDefine;
    bool selected;
    bool relFile;
    bool debugPassThrough;
    int mapMode;
};
class SwitchConfig : public xmlVisitor, public CmdSwitchString
{
  public:
    SwitchConfig(CmdSwitchParser& parser, char SwitchCmd) : CmdSwitchString(parser, SwitchCmd) {}
    ~SwitchConfig();

    bool Validate();
    bool ReadConfigFile(const std::string& file);
    static void AddDefine(LinkManager& linker, const std::string& name, const std::string& value)
    {
        ConfigData::AddDefine(linker, name, value);
    }
    virtual int Parse(const char* data);
    void SetDefines(LinkManager& linker);
    bool GetRelFile();
    bool GetDebugPassThrough();
    int GetMapMode();
    bool InterceptFile(const std::string& file);
    int RunApp(const std::string& path, const std::string& file, const std::string& debugFile, bool verbose,
               std::string outDefFile, std::string outImportLibrary);
    std::string GetSpecFile();
    virtual bool VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData);
    virtual bool VisitNode(xmlNode& node, xmlNode* child, void* userData);

  private:
    std::vector<std::unique_ptr<ConfigData>> configData;
    std::deque<std::string> files;
};
#endif

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

#include "SwitchConfig.h"
#include "ObjUtil.h"
#include "ObjExpression.h"
#include "LinkExpression.h"
#include "Utils.h"
#include <fstream>
#include <cstdlib>
#include <cctype>
#include <cstring>

ConfigData::~ConfigData() {}
bool ConfigData::VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData)
{
    if (node == "Switch")
    {
        if (*attrib == "Application")
        {
            app = attrib->GetValue();
        }
        else if (*attrib == "Flags")
        {
            appFlags = attrib->GetValue();
        }
        else if (*attrib == "Name")
        {
            name = attrib->GetValue();
        }
        else if (*attrib == "SpecFile")
        {
            specFile = attrib->GetValue();
        }
        else if (*attrib == "Rel")
        {
            relFile = std::atoi(attrib->GetValue().c_str());
        }
        else if (*attrib == "MapMode")
        {
            mapMode = std::atoi(attrib->GetValue().c_str());
        }
        else if (*attrib == "DebugPassThrough")
        {
            debugPassThrough = std::atoi(attrib->GetValue().c_str());
        }
    }
    else if (node == "Define")
    {
        if (*attrib == "Name")
        {
            currentDefine->name = attrib->GetValue();
        }
        else if (*attrib == "Value")
        {
            currentDefine->value = attrib->GetValue();
        }
    }
    else if (node == "Extension")
    {
        if (*attrib == "Value")
        {
            extensions.push_back(attrib->GetValue());
        }
    }
    return true;
}
bool ConfigData::VisitNode(xmlNode& node, xmlNode* child, void* userData)
{
    // buggy, defines can have defines as children :)
    if (*child == "Define")
    {
        defines.push_back(std::make_unique<CmdSwitchDefine::define>());
        currentDefine = defines.back().get();
        child->Visit(*this);
    }
    else if (*child == "Extension")
    {
        child->Visit(*this);
    }
    return true;
}
void ConfigData::AddDefine(LinkManager& linker, const std::string& name, const std::string& value)
{
    int n = std::stoi(value, nullptr, 0);
    LinkExpression* lexp = new LinkExpression(n);
    LinkExpressionSymbol* lsym = new LinkExpressionSymbol(name, lexp);
    (void)LinkExpression::EnterSymbol(lsym, true);
}
void ConfigData::SetDefines(LinkManager& linker)
{
    for (auto& define : defines)
        AddDefine(linker, define->name, define->value);
}
SwitchConfig::~SwitchConfig() {}
int SwitchConfig::Parse(const char* data)
{
    CmdSwitchString::Parse(data);
    const char* p = GetValue().c_str();
    char name[256], *q = name;
    while (*p && *p != ';')
        *q++ = *p++;
    *q = 0;
    std::string swname = name;
    q = name;
    while (*p)
        if (*p == ';')
            *q++ = '/', p++;
        else
            *q++ = *p++;
    *q = 0;
    std::string flags = name;
    bool found = false;
    for (auto& data : configData)
    {
        if (data->name == swname)
        {
            data->selected = true;
            data->appFlags += " " + flags + " ";
            found = true;
        }
    }
    if (!found)
    {
        LinkManager::LinkError("Invalid target name");
        return -1;
    }
    return strlen(data);
}
bool SwitchConfig::ReadConfigFile(const std::string& file)
{
    std::fstream in(file, std::ios::in);
    if (!in.fail())
    {
        xmlNode* node = new xmlNode();
        if (!node->Read(in) || *node != "LinkerConfig")
        {
            delete node;
            return false;
        }
        node->Visit(*this);
        delete node;
        return true;
    }
    return true;  // it is valid to run without a config file
}
bool SwitchConfig::Validate()
{
    std::string name;
    std::string spec;
    for (auto& data : configData)
    {
        if (data->selected)
        {
            if (!name.empty())
            {
                if (name != data->app)
                    return false;
            }
            else
                name = data->app;
            if (!spec.empty())
            {
                if (spec != data->specFile)
                    return false;
            }
            else
                spec = data->specFile;
        }
    }
    return true;
}
std::string SwitchConfig::GetSpecFile()
{
    for (auto& data : configData)
    {
        if (data->selected)
        {
            return data->specFile;
        }
    }
    return std::string("");
}
bool SwitchConfig::GetRelFile()
{
    bool rel = false;
    for (auto& data : configData)
    {
        if (data->selected)
        {
            if (data->relFile)
            {
                rel = true;
                break;
            }
        }
    }
    return rel;
}
bool SwitchConfig::GetDebugPassThrough()
{
    bool passThrough = false;
    for (auto& data : configData)
    {
        if (data->selected)
        {
            if (data->debugPassThrough)
            {
                passThrough = true;
                break;
            }
        }
    }
    return passThrough;
}
int SwitchConfig::GetMapMode()
{
    int mode = 0;
    for (auto& data : configData)
    {
        if (data->selected)
        {
            mode = data->mapMode;
        }
    }
    return mode;
}
void SwitchConfig::SetDefines(LinkManager& linker)
{
    for (auto& data : configData)
    {
        if (data->selected)
        {
            data->SetDefines(linker);
        }
    }
}
bool SwitchConfig::InterceptFile(const std::string& file)
{
    int npos = file.find_last_of(".");
    if (npos != std::string::npos)
    {
        std::string ext = file.substr(npos);
        // by convention the extensions in the APP file are lower case
        for (int i = 0; i < ext.size(); i++)
            ext[i] = tolower(ext[i]);
        for (auto& data : configData)
        {
            if (data->selected)
            {
                for (auto extension : data->extensions)
                {
                    if (ext == extension)
                    {
                        files.push_back(file);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
int SwitchConfig::RunApp(const std::string& path, const std::string& file, const std::string& debugFile, bool verbose,
                         std::string outDefFile, std::string outImportLibrary)
{
    std::string flags;
    std::string name;
    for (auto& data : configData)
    {
        if (data->selected)
        {
            name = data->app;
            flags = flags + " " + data->appFlags;
        }
    }
    if (name.empty())
        return 0;  // nothing to do, all ok
    std::string sverbose = verbose ? "/y" : "/!";
    std::string sdebug = debugFile.empty() ? "" : "\"/v" + debugFile + "\"";
    std::string sfiles;
    for (auto name : files)
        sfiles = sfiles + " \"" + name + "\"";
    std::string outdef;
    if (!outDefFile.empty())
        outdef = " --output-def \"" + outDefFile + "\"";
    std::string outimp;
    if (!outImportLibrary.empty())
        outimp = " --out-implib \"" + outImportLibrary + "\"";
    return Utils::ToolInvoke(name, verbose ? "" : nullptr, "%s %s %s %s %s \"%s\"%s", flags.c_str(), outdef.c_str(), outimp.c_str(),  sverbose.c_str(),
                             sdebug.c_str(), file.c_str(), sfiles.c_str());
}
bool SwitchConfig::VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData) { return false; }
bool SwitchConfig::VisitNode(xmlNode& node, xmlNode* child, void* userData)
{
    if (node == "LinkerConfig")
    {
        if (*child == "Switch")
        {
            configData.push_back(std::make_unique<ConfigData>(child));
        }
    }
    return true;
}

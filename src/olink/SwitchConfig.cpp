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
#include "SwitchConfig.h"
#include "ObjUtil.h"
#include "ObjExpression.h"
#include "LinkExpression.h"
#include <fstream>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#if defined(WIN32) || defined( MICROSOFT)
#define system(x) winsystem(x)
extern "C" int winsystem(const char *);
#endif

ConfigData::~ConfigData()
{
    for (auto define : defines)
        delete define;
    defines.clear();
}
bool ConfigData::VisitAttrib(xmlNode &node, xmlAttrib *attrib, void *userData)
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
            relFile = atoi(attrib->GetValue().c_str());
        }
        else if (*attrib == "MapMode")
        {
            mapMode = atoi(attrib->GetValue().c_str());
        }
        else if (*attrib == "DebugPassThrough")
        {
            debugPassThrough = atoi(attrib->GetValue().c_str());
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
bool ConfigData::VisitNode(xmlNode &node, xmlNode *child, void *userData)
{
    // buggy, defines can have defines as children :)
    if (*child == "Define")
    {
        currentDefine = new CmdSwitchDefine::define;
        //if (currentDefine)
        {
            defines.push_back(currentDefine);
            child->Visit(*this);
        }
    }
    else if (*child == "Extension")
    {
        child->Visit(*this);
    }
    return true;
}
void ConfigData::AddDefine(LinkManager &linker, const std::string &name, const std::string &value)
{
    int n = strtoul(value.c_str(), nullptr, 0);
    LinkExpression *lexp = new LinkExpression(n);
    LinkExpressionSymbol *lsym = new LinkExpressionSymbol(name, lexp);
    LinkExpression::EnterSymbol(lsym, true);
}
void ConfigData::SetDefines(LinkManager &linker)
{
    for (auto define : defines)
        AddDefine(linker, define->name, define->value);
}
SwitchConfig::~SwitchConfig()
{
    for (auto data : configData)
        delete data;
    configData.clear();
}
int SwitchConfig::Parse(const char *data)
{
    CmdSwitchString::Parse(data);
    const char *p = GetValue().c_str();
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
    for (auto data : configData)
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
    std::fstream in(file.c_str(), std::ios::in);
    if (!in.fail())
    {
        xmlNode *node = new xmlNode();
        if (!node->Read(in) || *node != "LinkerConfig")
        {
            delete node;
            return false;
        }
        node->Visit(*this);
        delete node;			
        return true;
    }
    return true; // it is valid to run without a config file
    
}
bool SwitchConfig::Validate()
{
    std::string name;
    std::string spec;
    for (auto data : configData)
    {
        if (data->selected)
        {
            if (name.size() != 0)
            {
                if (name != data->app)
                    return false;
            }
            else
                name = data->app;
            if (spec.size() != 0)
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
    for (auto data : configData)
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
    for (auto data : configData)
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
    for (auto data : configData)
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
    for (auto data : configData)
    {
        if (data->selected)
        {
            mode = data->mapMode;
        }
    }
    return mode;
}
void SwitchConfig::SetDefines(LinkManager &linker)
{
    for (auto data : configData)
    {
        if (data->selected)
        {
            data->SetDefines(linker);
        }
    }
}
bool SwitchConfig::InterceptFile(const std::string &file)
{
    int npos = file.find_last_of(".");
    if (npos != std::string::npos)
    {
        std::string ext = file.substr(npos);
        // by convention the extensions in the APP file are lower case
        for (int i=0; i < ext.size(); i++)
            ext[i] = tolower(ext[i]);
        for (auto data : configData)
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
int SwitchConfig::RunApp(const std::string &path, const std::string &file, const std::string &debugFile, bool verbose)
{
    std::string flags;
    std::string name;
    for (auto data : configData)
    {
        if (data->selected)
        {
            name = data->app;
            flags = flags + " " + data->appFlags;
        }
    }
    if (name.size() == 0)
        return 0; // nothing to do, all ok
    std::string cmd = std::string("\"") + path + name + "\" -! " ;
    if (debugFile.size())
        cmd = cmd + "\"/v"+debugFile + "\" " ;
    cmd = cmd + flags + "\"" + file + "\"";
    for (auto name : files)
        cmd = cmd + " \"" + name + "\"";
    if (verbose)
	    std::cout << "Running App: " << cmd << std::endl;
    return system(cmd.c_str());
}
bool SwitchConfig::VisitAttrib(xmlNode &node, xmlAttrib *attrib, void *userData)
{
    return false;
}
bool SwitchConfig::VisitNode(xmlNode &node, xmlNode *child, void *userData)
{
    if (node == "LinkerConfig")
    {
        if (*child == "Switch")
        {
            ConfigData *config = new ConfigData(child);
            //if (config)
            {
                configData.push_back(config);
            }
        }
    }
    return true;
}

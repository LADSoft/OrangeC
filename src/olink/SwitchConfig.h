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
    ConfigData(xmlNode *node) : selected(false), currentDefine(NULL), relFile(false), mapMode(0),
        debugPassThrough(false)
        { node->Visit(*this); }
    virtual ~ConfigData();
    void parse(const xmlNode *node);
    static void AddDefine(LinkManager &linker, const std::string &name, const std::string &value);
    void SetDefines(LinkManager &linker);
    virtual bool VisitAttrib(xmlNode &node, xmlAttrib *attrib, void *userData);
    virtual bool VisitNode(xmlNode &node, xmlNode *child, void *userData);
    std::string name;
    std::string app;
    std::string appFlags;
    std::string specFile;
    std::deque<std::string> extensions;
    std::vector<CmdSwitchDefine::define *> defines;
    CmdSwitchDefine::define *currentDefine;
    bool selected;
    bool relFile;
    bool debugPassThrough;
    int mapMode;
};
class SwitchConfig : public xmlVisitor, public CmdSwitchString
{
public:
    SwitchConfig(CmdSwitchParser &parser, char SwitchCmd) : CmdSwitchString(parser, SwitchCmd) { }
    ~SwitchConfig() ;

    bool Validate();
    bool ReadConfigFile(const std::string& file);
    static void AddDefine(LinkManager &linker, const std::string &name, const std::string &value)
    { ConfigData::AddDefine(linker, name, value); }
    virtual int Parse(const char *data);
    void SetDefines(LinkManager &linker);
    bool GetRelFile();
    bool GetDebugPassThrough();
    int GetMapMode();
    bool InterceptFile(const std::string &file);
    int RunApp(const std::string &path, const std::string &file, 
               const std::string &debugFile);
    std::string GetSpecFile();
    virtual bool VisitAttrib(xmlNode &node, xmlAttrib *attrib, void *userData);
    virtual bool VisitNode(xmlNode &node, xmlNode *child, void *userData);
private:
    std::vector<ConfigData *> configData;
    std::deque<std::string> files;
} ;
#endif


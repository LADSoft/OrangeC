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
#ifndef dlPmMAIN_H
#define dlPmMAIN_H

#include <string>
#include <vector>
#include "CmdSwitch.h"
#include "ObjFile.h"
#include "ObjFactory.h"

class dlPmMain
{
    struct Section
    {
        Section(ObjInt Address = 0, ObjInt Size = 0) : address(Address), size(Size), data(NULL) { }
        ~Section() {  delete data; }
        ObjInt address;
        ObjInt size;
        char *data;
    };
public:
    dlPmMain() { }
    ~dlPmMain();
    
    int Run(int argc, char **argv);
private:
    void GetSectionNames(std::vector<std::string> &names, ObjFile *file);
    void GetInputSections(const std::vector<std::string> &names, ObjFile *file, ObjFactory *factory);
    void LoadVars(ObjFile *file);
    bool ReadSections(const std::string &path);
    std::string GetOutputName(char *infile) const;
    bool LoadStub(const std::string &name);
    int stubSize;
    char *stubData;
    int memSize;
    int initSize;
    int uninitBase;
    int uninitSize;
    static CmdSwitchParser SwitchParser;
    static CmdSwitchString outputFileSwitch;
    static CmdSwitchString DebugFile;
    static char *usageText;
    ObjInt startAddress;
    ObjFile *file;
    std::vector<Section *> sections;	
} ;
#endif
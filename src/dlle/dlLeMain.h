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
#ifndef DLHEXMAIN_H
#define DLHEXMAIN_H

#include <string>
#include <fstream>
#include <deque>
#include "ObjTypes.h"
#include "CmdSwitch.h"
#include "LEHeader.h"
class ObjFile;
class ObjFactory;
class LEObject;
class LEFixup;
class ObjectPage;
class ResidentNameTable;

class dlLeMain
{
public:
    dlLeMain() : mode(eLe), rnt(NULL) { }
    ~dlLeMain();
    
    int Run(int argc, char **argv);
    enum Mode { UNKNOWN, eLe, eLx };
protected:
    bool GetMode();
    void ReadValues();
    bool ReadSections(const std::string &path, const std::string &exeName);
    std::string GetOutputName(char *infile) const;
    void InitHeader();
    bool LoadStub(const std::string &exeName);
    void WriteStub(std::fstream &out);
private:
    static CmdSwitchParser SwitchParser;
    static CmdSwitchString modeSwitch;
    static CmdSwitchString stubSwitch;
    static CmdSwitchString outputFileSwitch;
    static CmdSwitchString DebugFile;
    static char *usageText;

    ObjInt stackSize;
    std::string outputName;
    std::deque<LEObject *> objects;
    LEFixup *fixups;
    ObjectPage *objectPages;
    ResidentNameTable *rnt;
    enum Mode mode;
    int stubSize;
    char *stubData;
    LEHeader header;
    static unsigned fileVersion;
    ObjFactory *factory;
    ObjFile *file;
    ObjInt startAddress;
} ;
#endif
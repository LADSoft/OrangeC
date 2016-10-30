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
#ifndef CMDSWITCH_H
#define CMDSWITCH_H

// next lines are for watcom
#undef min
#undef max
#include <limits.h>
#include <string>
#include <set>
#include <vector>
#include <fstream>
class CmdSwitchParser;

class CmdSwitchBase
{
    public:
        CmdSwitchBase() : switchChar(-1) { }
        CmdSwitchBase(CmdSwitchParser &parser, char SwitchChar) ;
        CmdSwitchBase(char SwitchChar) : switchChar(SwitchChar) { }
        CmdSwitchBase(const CmdSwitchBase &orig) { switchChar = orig.switchChar; }
        virtual int Parse(const char *data) { return 0; } 
        
        char GetSwitchChar() const { return switchChar; }
    private:
        char switchChar;
} ;

class CmdSwitchBool : public CmdSwitchBase
{
    public:
        CmdSwitchBool(CmdSwitchParser &parser, char SwitchChar, bool State = false) : CmdSwitchBase(parser, SwitchChar), value(State) {}
        CmdSwitchBool(const CmdSwitchBool &orig) : CmdSwitchBase(orig) { value = orig.value; }
        virtual int Parse(const char *data);
        bool GetValue() const { return value; }
        void SetValue(bool flag) { value = flag; }
    private:
        bool value;
};
class CmdSwitchInt : public CmdSwitchBase
{
    public:
        CmdSwitchInt(CmdSwitchParser &parser, char SwitchChar, int Value = 0, int LowLimit = 0, int HiLimit = INT_MAX ) 
            : CmdSwitchBase(parser, SwitchChar), value(Value), lowLimit(LowLimit), hiLimit(HiLimit) {}
        CmdSwitchInt(const CmdSwitchInt &orig) : CmdSwitchBase(orig) 
        {
            value = orig.value;
            lowLimit = orig.lowLimit;
            hiLimit = orig.hiLimit;
        }
        
        virtual int Parse(const char *data);
        int GetValue() const { return value; }
    private:
        int value;
        int lowLimit;
        int hiLimit;
};
class CmdSwitchHex : public CmdSwitchBase
{
    public:
        CmdSwitchHex(CmdSwitchParser &parser, char SwitchChar, int Value = 0, int LowLimit = 0, int HiLimit = INT_MAX ) 
            : CmdSwitchBase(parser, SwitchChar), value(Value), lowLimit(LowLimit), hiLimit(HiLimit) {}
        CmdSwitchHex(const CmdSwitchHex &orig) : CmdSwitchBase(orig) 
        {
            value = orig.value;
            lowLimit = orig.lowLimit;
            hiLimit = orig.hiLimit;
        }
        
        virtual int Parse(const char *data);
        int GetValue() const { return value; }
    private:
        int value;
        int lowLimit;
        int hiLimit;
};
class CmdSwitchString : public CmdSwitchBase
{
    public:
        CmdSwitchString(CmdSwitchParser &parser, char SwitchChar, char Concat = '\0') : CmdSwitchBase(parser, SwitchChar), value(""), concat(Concat) {}
        CmdSwitchString(const CmdSwitchString &orig) : CmdSwitchBase(orig), concat(0) { value = orig.value;}
        CmdSwitchString() : value(""), concat(0) { }
        virtual int Parse(const char *data);
        const std::string &GetValue() const {return value; }
    protected:
        std::string value;
    private:
        char concat;
};
class CmdSwitchCombo : public CmdSwitchString
{
    public:
        CmdSwitchCombo(CmdSwitchParser &parser, char SwitchChar, char *Valid) : CmdSwitchString(parser, SwitchChar), valid(Valid), selected(false) {}
        CmdSwitchCombo(const CmdSwitchCombo &orig) : CmdSwitchString(orig) { valid = orig.valid; selected = orig.selected; }
        CmdSwitchCombo() : valid(""), selected(false) { }
        virtual int Parse(const char *data);
        bool GetValue() { return selected; }
        bool GetValue(char selector) { return value.find_first_of(selector) != std::string::npos; }
    private:
        bool selected;
        std::string valid;
} ;
class CmdSwitchOutput : public CmdSwitchString
{
    public:
        CmdSwitchOutput(CmdSwitchParser &parser, char SwitchChar, const char *Extension, bool concat = false) : CmdSwitchString(parser, SwitchChar, concat), extension(Extension) {}
        CmdSwitchOutput(const CmdSwitchOutput &orig) : CmdSwitchString(orig) { extension = orig.extension;}
        virtual int Parse(const char *data);
    private:
        const char *extension;
};
class CmdSwitchDefine : public CmdSwitchBase
{
    public:
        CmdSwitchDefine(CmdSwitchParser &parser, char SwitchChar) : CmdSwitchBase(parser, SwitchChar) {}
        CmdSwitchDefine(const CmdSwitchDefine &orig) : CmdSwitchBase(orig) { defines = orig.defines; }
        virtual ~CmdSwitchDefine();
        virtual int Parse(const char *data);
        struct define {
            std::string name;
            std::string value;
        };
        int GetCount() const { return defines.size(); }
        define *GetValue(int index);
    private:
        std::vector<define *> defines;
};
class CmdSwitchFile : public CmdSwitchString
{
    public:
        CmdSwitchFile(CmdSwitchParser &parser, char SwitchChar) : CmdSwitchString(parser, SwitchChar), Parser(&parser), argc(0), argv(nullptr) {}
        CmdSwitchFile(CmdSwitchParser &parser) : CmdSwitchString(), Parser(&parser), argc(0), argv(nullptr) { }

        virtual int Parse(const char *data);
        int GetCount() const { return argc; }
        char ** const GetValue() { return argv; }	
    protected:
        void Dispatch(char *data);
        char *GetStr(char *);
    private:
        int argc;
        char **argv;
        CmdSwitchParser *Parser;
} ;
class CmdSwitchParser
{
    public:
        CmdSwitchParser() {}
        ~CmdSwitchParser() {}
        
        bool Parse(int *argc, char **argv);
        
        CmdSwitchParser &operator +=(CmdSwitchBase *Switch)
        {
            switches.insert(Switch);
            return *this;
        }
    private:
        struct plt
        {
            bool operator ()(const CmdSwitchBase *left, const CmdSwitchBase *right) const
            {
                return left->GetSwitchChar() < right->GetSwitchChar();
            }
        } ;
        std::set<CmdSwitchBase *, plt> switches;
} ;
#endif
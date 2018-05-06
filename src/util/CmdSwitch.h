/* Software License Agreement
 * 
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the 
 *     Orange C "Target Code" exception.
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
#include <iostream>
class CmdSwitchParser;

class CmdSwitchBase
{
    public:
        CmdSwitchBase() : switchChar(-1), exists(false) { }
        CmdSwitchBase(CmdSwitchParser &parser, char SwitchChar) ;
        CmdSwitchBase(char SwitchChar) : switchChar(SwitchChar), exists(false) { }
        CmdSwitchBase(const CmdSwitchBase &orig) { 
            switchChar = orig.switchChar; 
            exists = orig.exists;
        }
        virtual int Parse(const char *data) { return 0; } 
        
        char GetSwitchChar() const { return switchChar; }
        void SetExists() { exists = true; }
        bool GetExists() const { return exists; }
    private:
        bool exists;
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
class CmdSwitchCombineString : public CmdSwitchString
{
    public:
        CmdSwitchCombineString(CmdSwitchParser &parser, char SwitchChar, char Concat = '\0') 
            : CmdSwitchString(parser, SwitchChar, Concat) { }
        CmdSwitchCombineString() { }
        virtual int Parse(const char *data);
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
class CmdSwitchOutput : public CmdSwitchCombineString
{
    public:
        CmdSwitchOutput(CmdSwitchParser &parser, char SwitchChar, const char *Extension, bool concat = false) : CmdSwitchCombineString(parser, SwitchChar, concat), extension(Extension) {}
        CmdSwitchOutput(const CmdSwitchOutput &orig) : CmdSwitchCombineString(orig) { extension = orig.extension;}
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
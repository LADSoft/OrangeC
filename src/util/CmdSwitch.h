/* Software License Agreement
 * 
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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

#ifndef CMDSWITCH_H
#define CMDSWITCH_H

#include <climits>
#include <string>
#include <set>
#include <vector>
#include <fstream>
#include <iostream>
#include <memory>
class CmdSwitchParser;

class CmdSwitchBase
{
  public:
    CmdSwitchBase() : switchChar(-1), exists(false) {}
    CmdSwitchBase(CmdSwitchParser& parser, char SwitchChar, std::string longName);
    CmdSwitchBase(char SwitchChar, std::string LongName) : switchChar(SwitchChar), exists(false), longName(LongName) {}
    CmdSwitchBase(const CmdSwitchBase& orig) = default;
    virtual int Parse(const char* data) { return 0; }

    char GetSwitchChar() const { return switchChar; }
    const std::string& GetLongName() const { return longName; }
    void SetExists() { exists = true; }
    bool GetExists() const { return exists; }
    virtual void SetArgNum(int an) { }
  private:
    bool exists;
    char switchChar;
    std::string longName;
};

class CmdSwitchBool : public CmdSwitchBase
{
  public:
    CmdSwitchBool(CmdSwitchParser& parser, char SwitchChar, bool State = false, std::string LongName = "") : CmdSwitchBase(parser, SwitchChar, LongName), value(State) {}
    CmdSwitchBool(const CmdSwitchBool& orig) = default;
    virtual int Parse(const char* data) override;
    bool GetValue() const { return value; }
    void SetValue(bool flag) { value = flag; }

  private:
    bool value;
};
class CmdSwitchInt : public CmdSwitchBase
{
  public:
    CmdSwitchInt(CmdSwitchParser& parser, char SwitchChar, int Value = 0, int LowLimit = 0, int HiLimit = INT_MAX, std::string LongName = "") :
        CmdSwitchBase(parser, SwitchChar, LongName),
        value(Value),
        lowLimit(LowLimit),
        hiLimit(HiLimit)
    {
    }
    CmdSwitchInt(const CmdSwitchInt& orig) = default;

    virtual int Parse(const char* data) override;
    int GetValue() const { return value; }
    void SetValue(int val) { value = val; }
  private:
    int value;
    int lowLimit;
    int hiLimit;
};
class CmdSwitchHex : public CmdSwitchBase
{
  public:
    CmdSwitchHex(CmdSwitchParser& parser, char SwitchChar, int Value = 0, int LowLimit = 0, int HiLimit = INT_MAX, std::string LongName = "") :
        CmdSwitchBase(parser, SwitchChar, LongName),
        value(Value),
        lowLimit(LowLimit),
        hiLimit(HiLimit)
    {
    }
    CmdSwitchHex(const CmdSwitchHex& orig) = default;

    virtual int Parse(const char* data) override;
    int GetValue() const { return value; }

  private:
    int value;
    int lowLimit;
    int hiLimit;
};
class CmdSwitchString : public CmdSwitchBase
{
  public:
    CmdSwitchString(CmdSwitchParser& parser, char SwitchChar, char Concat = '\0', std::string LongName = "") :
        CmdSwitchBase(parser, SwitchChar, LongName),
        value(""),
        concat(Concat)
    {
    }
    CmdSwitchString(const CmdSwitchString& orig) = default;
    CmdSwitchString() : value(""), concat(0) {}
    virtual int Parse(const char* data) override;
    const std::string& GetValue() const { return value; }
    std::string operator+=(const char* c)
    {
        value += c;
        return value;
    }

  protected:
    std::string value;

  private:
    char concat;
};
class CmdSwitchCombineString : public CmdSwitchString
{
  public:
    CmdSwitchCombineString(CmdSwitchParser& parser, char SwitchChar, char Concat = '\0', std::string LongName = "") :
        CmdSwitchString(parser, SwitchChar, Concat, LongName)
    {
    }
    CmdSwitchCombineString() {}
    virtual int Parse(const char* data) override;
};
class CmdSwitchCombo : public CmdSwitchString
{
  public:
    CmdSwitchCombo(CmdSwitchParser& parser, char SwitchChar, const char* Valid, std::string LongName = "") :
        CmdSwitchString(parser, SwitchChar, '\0', LongName),
        valid(Valid),
        selected(false)
    {
    }
    CmdSwitchCombo(const CmdSwitchCombo& orig) = default;
    CmdSwitchCombo() : valid(""), selected(false) {}
    virtual int Parse(const char* data) override;
    bool GetValue() { return selected; }
    bool GetValue(char selector) { return value.find_first_of(selector) != std::string::npos; }

  private:
    bool selected;
    std::string valid;
};
class CmdSwitchOutput : public CmdSwitchCombineString
{
  public:
    CmdSwitchOutput(CmdSwitchParser& parser, char SwitchChar, const char* Extension, bool concat = false, std::string LongName = "") :
        CmdSwitchCombineString(parser, SwitchChar, concat, LongName),
        extension(Extension)
    {
    }
    CmdSwitchOutput(const CmdSwitchOutput& orig) = default;
    virtual int Parse(const char* data) override;

  private:
    const char* extension;
};
class CmdSwitchDefine : public CmdSwitchBase
{
  public:
    CmdSwitchDefine(CmdSwitchParser& parser, char SwitchChar, std::string LongName = "") : CmdSwitchBase(parser, SwitchChar, LongName) {}
    CmdSwitchDefine(const CmdSwitchDefine& orig) = default;
    virtual ~CmdSwitchDefine();
    virtual int Parse(const char* data) override;
    struct define
    {
        std::string name;
        std::string value;
        int argnum;
    };
    int GetCount() const { return defines.size(); }
    define* GetValue(int index);
    virtual void SetArgNum(int an) override { if (defines.size()) defines.back()->argnum = an; }
  private:
    std::vector<std::unique_ptr<define>> defines;
};
class CmdSwitchFile : public CmdSwitchString
{
  public:
      CmdSwitchFile(CmdSwitchParser& parser, char SwitchChar, std::string LongName = "") :
          CmdSwitchString(parser, SwitchChar, '\0', LongName),
        Parser(&parser),
        argc(0),
        argv(nullptr)
    {
    }
    CmdSwitchFile(CmdSwitchParser &parser) : CmdSwitchString(), Parser(&parser), argc(0), argv(nullptr) { }

    virtual int Parse(const char* data) override;

    int GetCount() const { return argc; }
    char** const GetValue() { return argv.get(); }

  protected:
    void Dispatch(char* data);
    char* GetStr(char*);

  private:
    int argc;
    std::unique_ptr<char*[]> argv;
    CmdSwitchParser* Parser;
};
class CmdSwitchParser
{
  public:
    CmdSwitchParser() : nologo(*this, '!', false, "nologo") {}
    ~CmdSwitchParser() {}

    bool Parse(int* argc, char** argv);

    CmdSwitchParser& operator+=(CmdSwitchBase* Switch)
    {
        switches.insert(Switch);
        return *this;
    }

    CmdSwitchBase *Find(const char *sw, bool useLongName);

  private:
    std::set<CmdSwitchBase*> switches;
    CmdSwitchBool nologo;
};
#endif
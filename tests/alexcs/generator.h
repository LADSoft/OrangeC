#ifndef GENERATOR
#define GENERATOR

#include "asm_commands.h"
#include <list>
#include <iostream>
#include <stdio.h>
#include <sstream>
using namespace std;

class AsmOperand;
class AsmStrImmediate;

extern const string ASM_DATA_TYPE_TO_STR[];

class AsmCmd{
public:
    virtual void Print(ostream& o) const;
};

class AsmLabel: public AsmCmd{
    AsmStrImmediate* label;
public:
    AsmLabel(AsmStrImmediate* label);
    AsmLabel(AsmStrImmediate label);
    AsmLabel(string label);
    virtual void Print(ostream& o) const;
};

class AsmRawCmd: public AsmCmd{
private:
    string str;
public:
    AsmRawCmd(string cmd);
    virtual void Print(ostream& o) const;
};

class AsmCmd0: public AsmCmd{
protected:
    AsmCmdName command;
    CmdSize size;
public:
    AsmCmd0(AsmCmdName cmd, CmdSize cmd_size = SIZE_LONG);
    virtual void Print(ostream& o) const;
};

class AsmData{
private:
    string name;
    string value;
    AsmDataType type;
public:
    AsmData(string name_, string value, AsmDataType type = DATA_UNTYPED);
    virtual void Print(ostream& o) const;
};

class AsmCmd1: public AsmCmd0{
private:
    AsmOperand* oper;
public:
    AsmCmd1(AsmCmdName cmd, AsmOperand* oper_, CmdSize size = SIZE_LONG);
    virtual void Print(ostream& o) const;
};

class AsmCmd2: public AsmCmd0{
private:
    AsmOperand* src;
    AsmOperand* dest;
public:
    AsmCmd2(AsmCmdName cmd, AsmOperand* src_, AsmOperand* dest_, CmdSize size = SIZE_LONG);
    virtual void Print(ostream& o) const;
};

class AsmOperand{
public:
    virtual void Print(ostream& o) const;
    virtual void PrintBase(ostream& o) const;
};

class AsmOperandBase: public AsmOperand{
public:
    virtual void Print(ostream& o) const;
    virtual void PrintBase(ostream& o) const;
};

class AsmRegister: public AsmOperandBase{
private:
    RegisterName reg;
public:
    AsmRegister(RegisterName reg_);
    virtual void Print(ostream& o) const;
    virtual void PrintBase(ostream& o) const;
};

class AsmImmediate: public AsmOperandBase{
public:
    virtual void Print(ostream& o) const;
    virtual void PrintBase(ostream& o) const;
    virtual string GetStrValue() const;
    virtual int GetIntValue() const;
};

class AsmIntImmediate: public AsmOperandBase{
private:
    int value;
public:
    AsmIntImmediate(int value_);
    AsmIntImmediate(const AsmIntImmediate& src);
    virtual int GetIntValue() const;
    virtual void Print(ostream& o) const;
    virtual void PrintBase(ostream& o) const;
};

class AsmStrImmediate: public AsmImmediate{
private:
    string value;
public:
    AsmStrImmediate();
    AsmStrImmediate(const string& value_);
    AsmStrImmediate(const AsmStrImmediate& src);
    virtual string GetStrValue() const;
    virtual void Print(ostream& o) const;
    virtual void PrintBase(ostream& o) const;
};

class AsmMemory: public AsmOperand{
private:
    AsmOperandBase* base;
    int disp;
    int index;
    unsigned scale;
public:
    AsmMemory(AsmOperandBase* base_, int disp_ = 0, int index_ = 0, unsigned scale_ = 0);
    AsmMemory(AsmStrImmediate base, int disp_ = 0, int index_ = 0, unsigned scale_ = 0);
    AsmMemory(AsmIntImmediate base, int disp_ = 0, int index_ = 0, unsigned scale_ = 0);
    AsmMemory(RegisterName reg, int disp_ = 0, int index_ = 0, unsigned scale_ = 0);
    virtual void Print(ostream& o) const;
};

class AsmCode{
private:
    AsmStrImmediate format_str_real;
    AsmStrImmediate format_str_int;
    AsmStrImmediate format_str_str;
    AsmStrImmediate format_str_new_line;
    bool was_real;
    bool was_int;
    bool was_str;
    bool was_new_line;
    AsmMemory funct_write;
    list<AsmCmd*> commands;
    list<AsmData*> data;
    string ChangeName(string str);
    unsigned label_counter;
public:
    AsmCode();
    string GenStrLabel();
    AsmStrImmediate GenLabel(string prefix);
    string GenStrLabel(string prefix);
    AsmStrImmediate LabelByStr(string str);
    void AddCmd(AsmCmd* cmd);
    void AddCmd(AsmLabel cmd);
    void AddCmd(AsmCmdName cmd, CmdSize size = SIZE_LONG);
    void AddCmd(string raw_cmd);
    void AddCmd(AsmCmdName cmd, AsmOperand* oper, CmdSize size = SIZE_LONG);
    void AddCmd(AsmCmdName cmd, RegisterName reg, CmdSize size = SIZE_LONG);
    void AddCmd(AsmCmdName cmd, AsmMemory* mem, CmdSize size = SIZE_LONG);
    void AddCmd(AsmCmdName cmd, AsmMemory mem, CmdSize size = SIZE_LONG);
    void AddCmd(AsmCmdName cmd, AsmImmediate* imm, CmdSize size = SIZE_LONG);
    void AddCmd(AsmCmdName cmd, AsmStrImmediate str_imm, CmdSize size = SIZE_LONG);
    void AddCmd(AsmCmdName cmd, AsmIntImmediate int_imm, CmdSize size = SIZE_LONG);
    void AddCmd(AsmCmdName cmd, int int_imm, CmdSize size = SIZE_LONG);
    void AddCmd(AsmCmdName cmd, AsmOperand* oper1, AsmOperand* oper2, CmdSize size = SIZE_LONG);
    void AddCmd(AsmCmdName cmd, RegisterName src, RegisterName dest, CmdSize size = SIZE_LONG);
    void AddCmd(AsmCmdName cmd, RegisterName reg, AsmImmediate* dest, CmdSize size = SIZE_LONG);
    void AddCmd(AsmCmdName cmd, RegisterName reg, AsmStrImmediate dest, CmdSize size = SIZE_LONG);
    void AddCmd(AsmCmdName cmd, RegisterName reg, AsmIntImmediate dest, CmdSize size = SIZE_LONG);
    void AddCmd(AsmCmdName cmd, int int_imm, RegisterName reg, CmdSize size = SIZE_LONG);
    void AddCmd(AsmCmdName cmd, AsmImmediate* src, RegisterName oper1, CmdSize size = SIZE_LONG);
    void AddCmd(AsmCmdName cmd, AsmStrImmediate src, RegisterName oper1, CmdSize size = SIZE_LONG);
    void AddCmd(AsmCmdName cmd, AsmIntImmediate src, RegisterName oper1, CmdSize size = SIZE_LONG);
    void AddCmd(AsmCmdName cmd, AsmMemory* mem, RegisterName reg, CmdSize size = SIZE_LONG);
    void AddCmd(AsmCmdName cmd, AsmMemory mem, RegisterName reg, CmdSize size = SIZE_LONG);
    void AddCmd(AsmCmdName cmd, RegisterName reg, AsmMemory* mem, CmdSize size = SIZE_LONG);
    void AddCmd(AsmCmdName cmd, RegisterName reg, AsmMemory mem, CmdSize size = SIZE_LONG);
    void AddCmd(AsmCmdName cmd, AsmImmediate* src, AsmMemory* mem, CmdSize size = SIZE_LONG);
    void AddCmd(AsmCmdName cmd, AsmStrImmediate str_imm, AsmMemory mem, CmdSize size = SIZE_LONG);
    void AddCmd(AsmCmdName cmd, AsmIntImmediate int_imm, AsmMemory mem, CmdSize size = SIZE_LONG);
    void AddData(AsmData* new_data);
    AsmStrImmediate AddData(string label, string value, AsmDataType type = DATA_UNTYPED);
    AsmStrImmediate AddData(string label, unsigned size);
    AsmStrImmediate AddData(string value, AsmDataType type = DATA_UNTYPED);
    AsmStrImmediate AddData(unsigned size);
    void AddLabel(AsmStrImmediate* label);
    void AddLabel(AsmStrImmediate label);
    void AddLabel(string label);
    virtual void Print(ostream& o) const;
    void GenCallWriteForInt();
    void GenCallWriteForReal();
    void GenCallWriteForStr();
    void GenWriteNewLine();
    void PushMemory(unsigned size);
    void MoveToMemoryFromStack(unsigned size);
    void MoveMemory(unsigned size);
    void AddMainFunctionLabel();
};

#endif

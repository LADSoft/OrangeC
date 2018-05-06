#ifndef ASM_COMMANDS
#define ASM_COMMANDS

#include <string>
using namespace std;

enum CmdSize{
    SIZE_NONE,
    SIZE_BYTE,
    SIZE_SHORT,
    SIZE_WORD,
    SIZE_LONG,
    SIZE_QUARD
};

extern const string SIZE_TO_STR[];

enum RegisterName{
    REG_AL,
    REG_BL,
    REG_CL,
    REG_DL,
    REG_AH,
    REG_BH,
    REG_CH,
    REG_DH,
    REG_AX,
    REG_BX,
    REG_CX,
    REG_DX,    
    REG_DI,
    REG_SI,
    REG_EAX,
    REG_EBX,
    REG_ECX,
    REG_EDX,
    REG_EDI,
    REG_ESI,
    REG_EBP,
    REG_ESP,
    REG_ST,
    REG_ST0,
    REG_ST1,
    REG_ST2,
    REG_ST3,
    REG_ST4,
    REG_ST5,
    REG_ST6,
    REG_ST7
};

extern const string REG_TO_STR[];

enum AsmCmdName{
    ASM_ADD,
    ASM_AND,
    ASM_CALL,
    ASM_CMP,
    ASM_DIV,
    ASM_FADDP,
    ASM_FCH,
    ASM_FCOMPP,
    ASM_FDIVRP,
    ASM_FILD,
    ASM_FLD,
    ASM_FMULP,
    ASM_FNSTSW,
    ASM_FSTP,
    ASM_FSUBRP,
    ASM_FXCH,
    ASM_IDIV,
    ASM_IMUL,
    ASM_JMP,
    ASM_JNE,
    ASM_JNG,
    ASM_JNL,
    ASM_JNZ,
    ASM_JZ,
    ASM_LEA,
    ASM_MOV,
    ASM_MOVZB,
    ASM_MUL,
    ASM_NEG ,
    ASM_NOT,
    ASM_OR,
    ASM_POP,
    ASM_PUSH,
    ASM_RET,
    ASM_SAHF,
    ASM_SAR,
    ASM_SAL,
    ASM_SETA,
    ASM_SETAE,
    ASM_SETB,
    ASM_SETBE,
    ASM_SETG,
    ASM_SETGE,
    ASM_SETL,
    ASM_SETLE,
    ASM_SETE,
    ASM_SETNE, 
    ASM_SUB,
    ASM_TEST,
    ASM_XOR
};

extern const string ASM_CMD_TO_STR[];

enum AsmDataType{
    DATA_UNTYPED,
    DATA_INT,
    DATA_REAL,
    DATA_STR
};

#endif

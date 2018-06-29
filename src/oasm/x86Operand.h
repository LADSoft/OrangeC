#ifndef x86Operand_h
#define x86Operand_h

class x86Parser;
class Coding;
static const int OPARRAY_SIZE = 27;
class x86Operand
{
  public:
    x86Operand() : opcode(-1), addressCoding(-1), operandCoding(-1) { Clear(); }
    void Clear() { memset(values, 0, sizeof(values)); }
    int opcode;
    int addressCoding;
    int operandCoding;
    Coding* values[OPARRAY_SIZE];
};

struct x86Token
{
    enum
    {
        EOT,
        TOKEN,
        REGISTER,
        REGISTERCLASS,
        ADDRESSCLASS,
        NUMBER,
        EMPTY
    } type;
    unsigned char id;
    unsigned char eos;
    unsigned char level;
    unsigned char* addrClass;
    void (x86Parser::*tokenFunc)(x86Operand& operand, int tokenPos);
    x86Token* next;
};

#endif

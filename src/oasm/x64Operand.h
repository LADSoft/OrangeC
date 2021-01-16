#ifndef x64Operand_h
#define x64Operand_h

class x64Parser;
class Coding;
static const int OPARRAY_SIZE = 39;
class x64Operand
{ 
public:
	x64Operand() : opcode(-1), addressCoding(-1), operandCoding(-1) { Clear(); }
	void Clear() { memset(values, 0, sizeof(values)); }
	int opcode;
	int addressCoding;
	int operandCoding;
	Coding *values[OPARRAY_SIZE];
}; 

struct x64Token
{
	enum
	{
		EOT, TOKEN, REGISTER, REGISTERCLASS, ADDRESSCLASS, NUMBER, EMPTY
	} type;
	unsigned char id;
	unsigned char eos;
	unsigned char level;
	unsigned char *addrClass;
	void (x64Parser::*tokenFunc)(x64Operand& operand, int tokenPos);
	x64Token *next;
};

#endif

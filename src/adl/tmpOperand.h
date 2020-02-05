#ifndef tmpOperand_h
#define tmpOperand_h

class tmpParser;
class Coding;
static const int OPARRAY_SIZE = 6;
class tmpOperand
{ 
public:
	tmpOperand() : opcode(-1), addressCoding(-1), operandCoding(-1) { Clear(); }
	void Clear() { memset(values, 0, sizeof(values)); }
	int opcode;
	int addressCoding;
	int operandCoding;
	Coding *values[OPARRAY_SIZE];
}; 

struct tmpToken
{
	enum
	{
		EOT, TOKEN, REGISTER, REGISTERCLASS, ADDRESSCLASS, NUMBER, EMPTY
	} type;
	unsigned char id;
	unsigned char eos;
	unsigned char level;
	unsigned char *addrClass;
	void (tmpParser::*tokenFunc)(tmpOperand& operand, int tokenPos);
	tmpToken *next;
};

#endif

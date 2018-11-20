#ifndef x86Parser_h
#define x86Parser_h

class Section;
class AsmFile;

class x86Parser : public InstructionParser 
{ 
public:
	x86Parser() { Init(); }
	void Setseg32(int v) { stateVars[0] = v; }
	int Getseg32() { return stateVars[0]; }
	virtual void Setup(Section *sect);
	virtual void Init();
	virtual bool ParseSection(AsmFile *fil, Section *sect);
	virtual bool IsBigEndian() { return 0; }

protected:
	bool ParseAddresses(x86Operand &operand, int addrClass, int &tokenPos);
	bool ParseOperands2(x86Token *tokenList, x86Operand &operand, int tokenPos, int level);
	bool ParseOperands(x86Token *tokenList, x86Operand &operand);
	bool ProcessCoding(CodingHelper &base, x86Operand &operand, Coding *coding);
	bool MatchesToken(int token, int tokenPos);
	bool MatchesRegister(int reg, int tokenPos);
	bool MatchesRegisterClass(int cclass, int tokenPos);
	virtual bool DispatchOpcode(int opcode);

	typedef bool (x86Parser::*DispatchType)(x86Operand &);
	static DispatchType DispatchTable[];

	typedef Coding * (x86Parser::*StateFuncDispatchType)();
	static StateFuncDispatchType stateFuncs[];
	Coding *StateFunc1();
	Coding *StateFunc1();
	Coding *StateFunc1();
	Coding *StateFunc1();

private:
	int stateVars[1];

	static Coding stateCoding_eot[];
	static Coding stateCoding1_1[];
	static Coding stateCoding1_2[];
	static Coding stateCoding2_1[];
	static Coding stateCoding2_2[];
	static Coding stateCoding3_1[];
	static Coding stateCoding3_2[];
	static Coding stateCoding4_1[];
	static Coding stateCoding4_2[];

	typedef bool (x86Parser::*NumberDispatchType)(int tokenPos);
	static NumberDispatchType numberFuncs[];
	bool Number1(int tokenPos);
	bool Number2(int tokenPos);
	bool Number3(int tokenPos);
	bool Number4(int tokenPos);
	bool Number5(int tokenPos);
	bool Number6(int tokenPos);
	bool Number7(int tokenPos);
	bool Number8(int tokenPos);
	bool Number9(int tokenPos);
	bool Number10(int tokenPos);
	bool Number11(int tokenPos);
	bool Number12(int tokenPos);
	bool Number13(int tokenPos);
	bool Number14(int tokenPos);
	bool Number15(int tokenPos);
	bool Number16(int tokenPos);
	bool Number17(int tokenPos);
	bool Number18(int tokenPos);

	static unsigned char RegClassData1[];
	static unsigned char RegClassData2[];

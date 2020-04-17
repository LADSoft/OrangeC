#ifndef tmpParser_h
#define tmpParser_h

class Section;
class AsmFile;

class tmpParser : public InstructionParser 
{ 
public:
	tmpParser() { Init(); }
	void Setprocessorbits(int v) { stateVars[0] = v; }
	int Getprocessorbits() { return stateVars[0]; }
	virtual void Setup(Section *sect);
	virtual void Init();
	virtual bool ParseSection(AsmFile *fil, Section *sect);
	virtual bool IsBigEndian() { return 0; }

protected:
	bool ParseAddresses(tmpOperand &operand, int addrClass, int &tokenPos);
	bool ParseOperands2(tmpToken *tokenList, tmpOperand &operand, int tokenPos, int level);
	bool ParseOperands(tmpToken *tokenList, tmpOperand &operand);
	bool ProcessCoding(tmpOperand &operand, Coding *coding);
	bool ProcessCoding(tmpOperand &operand, Coding *coding, int bits, int* arr, char* bitcounts, char *func, int &index);
	bool MatchesToken(int token, int tokenPos);
	bool MatchesRegister(int reg, int tokenPos);
	bool MatchesRegisterClass(int cclass, int tokenPos);
	int DoMath(char op, int left, int right);
	virtual bool DispatchOpcode(int opcode);

	typedef bool (tmpParser::*DispatchType)(tmpOperand &);
	static DispatchType DispatchTable[];

	typedef Coding * (tmpParser::*StateFuncDispatchType)();
	static StateFuncDispatchType stateFuncs[];
	Coding *StateFunc1();
	Coding *StateFunc2();
	Coding *StateFunc3();
	Coding *StateFunc4();
	Coding *StateFunc5();
	Coding *StateFunc6();
	Coding *StateFunc7();

private:
	int stateVars[1];

	static Coding stateCoding_eot[];
	static Coding stateCoding1_1[];
	static Coding stateCoding1_2[];
	static Coding stateCoding2_1[];
	static Coding stateCoding2_2[];
	static Coding stateCoding2_3[];
	static Coding stateCoding3_1[];
	static Coding stateCoding4_1[];
	static Coding stateCoding4_2[];
	static Coding stateCoding5_1[];
	static Coding stateCoding5_2[];
	static Coding stateCoding5_3[];
	static Coding stateCoding6_1[];
	static Coding stateCoding7_1[];

	typedef bool (tmpParser::*NumberDispatchType)(int tokenPos);
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
	bool Number19(int tokenPos);
	bool Number20(int tokenPos);

	static unsigned char RegClassData1[];

	static unsigned char registerData22[];
	static unsigned char registerData7[];
	static unsigned char registerData10[];
	static unsigned char registerData13[];
	static unsigned char registerData16[];
	static unsigned char registerData31[];
	static unsigned char registerData32[];
	static unsigned char registerData30[];
	static unsigned char registerData1[];
	static unsigned char registerData5[];
	static unsigned char registerData24[];
	static unsigned char registerData8[];
	static unsigned char registerData11[];
	static unsigned char registerData14[];
	static unsigned char registerData17[];
	static unsigned char registerData4[];
	static unsigned char registerData21[];
	static unsigned char registerData6[];
	static unsigned char registerData20[];
	static unsigned char registerData9[];
	static unsigned char registerData12[];
	static unsigned char registerData15[];
	static unsigned char registerData18[];
	static unsigned char registerData19[];
	static unsigned char registerData2[];
	static unsigned char registerData3[];
	static unsigned char registerData26[];
	static unsigned char registerData25[];
	static unsigned char registerData27[];
	static unsigned char registerData29[];
	static unsigned char registerData28[];
	static unsigned char registerData23[];
	static unsigned char registerData33[];
	static unsigned char *registerDataIndirect[];

	static int registerValues[][6];

	static tmpToken tokenBranches1[];
	void TokenFunc2(tmpOperand &operand, int tokenPos);
	void TokenFunc3(tmpOperand &operand, int tokenPos);
	static tmpToken tokenBranches4[];
	static Coding tokenCoding5_0[];
	static Coding tokenCoding5_1[];
	static Coding tokenCoding5_2[];
	static Coding tokenCoding5_3[];
	void TokenFunc5(tmpOperand &operand, int tokenPos);
	static tmpToken tokenBranches6[];
	static Coding tokenCoding7_1[];
	static Coding tokenCoding7_2[];
	static Coding tokenCoding7_3[];
	void TokenFunc7(tmpOperand &operand, int tokenPos);

	static Coding OpcodeCodings0_5[];
	bool Opcode0(tmpOperand &operand);
	static Coding OpcodeCodings1_5[];
	bool Opcode1(tmpOperand &operand);

	static Coding Coding1[];
	static Coding Coding2[];
	static Coding *Codings[];

	static Coding prefixCoding1[];
	static Coding prefixCoding2[];
	static Coding prefixCoding3[];
	static Coding prefixCoding4[];
	static Coding prefixCoding5[];
	static Coding prefixCoding6[];
	static Coding prefixCoding7[];
	static Coding prefixCoding8[];
	static Coding prefixCoding9[];
	static Coding prefixCoding10[];
	static Coding *prefixCodings[];

}; 

#endif

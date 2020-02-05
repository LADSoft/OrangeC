#include "InstructionParser.h"
#include "tmpOperand.h"
#include "tmpParser.h"

InstructionParser *InstructionParser::GetInstance()
{
	return static_cast<InstructionParser *>(new tmpParser());
}
Coding tmpParser::stateCoding_eot[] = { { Coding::eot } };
Coding tmpParser::stateCoding1_1[] = {
	{ Coding::eot },
};
Coding tmpParser::stateCoding1_2[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 103, 8, -1, 0, 0 }, 
	{ Coding::eot },
};
Coding tmpParser::stateCoding2_1[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 103, 8, -1, 0, 0 }, 
	{ Coding::eot },
};
Coding tmpParser::stateCoding2_2[] = {
	{ Coding::eot },
};
Coding tmpParser::stateCoding2_3[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 103, 8, -1, 0, 0 }, 
	{ Coding::eot },
};
Coding tmpParser::stateCoding3_1[] = {
	{ Coding::eot },
};
Coding tmpParser::stateCoding4_1[] = {
	{ Coding::eot },
};
Coding tmpParser::stateCoding4_2[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 102, 8, -1, 0, 0 }, 
	{ Coding::eot },
};
Coding tmpParser::stateCoding5_1[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 102, 8, -1, 0, 0 }, 
	{ Coding::eot },
};
Coding tmpParser::stateCoding5_2[] = {
	{ Coding::eot },
};
Coding tmpParser::stateCoding5_3[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 102, 8, -1, 0, 0 }, 
	{ Coding::eot },
};
Coding tmpParser::stateCoding6_1[] = {
	{ Coding::eot },
};
Coding tmpParser::stateCoding7_1[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 4, -1, 0, 0 }, 
	{ Coding::eot },
};
Coding *tmpParser::StateFunc1()
{
	if (stateVars[0]==16)
	{
		return stateCoding1_1;
	}
	if (stateVars[0]==32)
	{
		return stateCoding1_2;
	}
	return stateCoding_eot;
}
Coding *tmpParser::StateFunc2()
{
	if (stateVars[0]==16)
	{
		return stateCoding2_1;
	}
	if (stateVars[0]==32)
	{
		return stateCoding2_2;
	}
	if (stateVars[0]==64)
	{
		return stateCoding2_3;
	}
	return stateCoding_eot;
}
Coding *tmpParser::StateFunc3()
{
	if (true)
	{
		return stateCoding3_1;
	}
	return stateCoding_eot;
}
Coding *tmpParser::StateFunc4()
{
	if (stateVars[0]==16)
	{
		return stateCoding4_1;
	}
	if (stateVars[0]==32)
	{
		return stateCoding4_2;
	}
	return stateCoding_eot;
}
Coding *tmpParser::StateFunc5()
{
	if (stateVars[0]==16)
	{
		return stateCoding5_1;
	}
	if (stateVars[0]==32)
	{
		return stateCoding5_2;
	}
	if (stateVars[0]==64)
	{
		return stateCoding5_3;
	}
	return stateCoding_eot;
}
Coding *tmpParser::StateFunc6()
{
	if (true)
	{
		return stateCoding6_1;
	}
	return stateCoding_eot;
}
Coding *tmpParser::StateFunc7()
{
	if (true)
	{
		return stateCoding7_1;
	}
	return stateCoding_eot;
}
tmpParser::StateFuncDispatchType tmpParser::stateFuncs[] = {
	&tmpParser::StateFunc1,
	&tmpParser::StateFunc2,
	&tmpParser::StateFunc3,
	&tmpParser::StateFunc4,
	&tmpParser::StateFunc5,
	&tmpParser::StateFunc6,
	&tmpParser::StateFunc7,
};
bool tmpParser::Number1(int tokenPos)
{
	bool rv = false;
	if (!rv)
	{
		rv = ParseNumber(0, 1, 8, 1, tokenPos);
	}
	return rv;
}
bool tmpParser::Number2(int tokenPos)
{
	bool rv = false;
	if (!rv)
	{
		rv = ParseNumber(0, 1, 8, 1, tokenPos);
	}
	return rv;
}
bool tmpParser::Number3(int tokenPos)
{
	bool rv = false;
	if (!rv)
	{
		rv = ParseNumber(0, 0, 6, 1, tokenPos);
	}
	return rv;
}
bool tmpParser::Number4(int tokenPos)
{
	bool rv = false;
	if (!rv)
	{
		rv = ParseNumber(0, 0, 8, 1, tokenPos);
	}
	return rv;
}
bool tmpParser::Number5(int tokenPos)
{
	bool rv = false;
	if (!rv)
	{
		rv = ParseNumber(0, 0, 16, 0, tokenPos);
	}
	return rv;
}
bool tmpParser::Number6(int tokenPos)
{
	bool rv = false;
	if (!rv)
	{
		rv = ParseNumber(0, 0, 32, 0, tokenPos);
	}
	return rv;
}
bool tmpParser::Number7(int tokenPos)
{
	bool rv = false;
	if (!rv)
	{
		rv = ParseNumber(0, 0, 64, 0, tokenPos);
	}
	return rv;
}
bool tmpParser::Number8(int tokenPos)
{
	bool rv = false;
	if (!rv && stateVars[0]==16)
	{
		rv = ParseNumber(0, 0, 16, 0, tokenPos);
	}
	return rv;
}
bool tmpParser::Number9(int tokenPos)
{
	bool rv = false;
	if (!rv && stateVars[0]==32)
	{
		rv = ParseNumber(0, 0, 32, 0, tokenPos);
	}
	return rv;
}
bool tmpParser::Number10(int tokenPos)
{
	bool rv = false;
	if (!rv && stateVars[0]==32)
	{
		rv = ParseNumber(0, 0, 64, 0, tokenPos);
	}
	return rv;
}
bool tmpParser::Number11(int tokenPos)
{
	bool rv = false;
	if (!rv)
	{
		rv = ParseNumber(1, 1, 8, 0, tokenPos);
	}
	return rv;
}
bool tmpParser::Number12(int tokenPos)
{
	bool rv = false;
	if (!rv)
	{
		rv = ParseNumber(-1, 1, 8, 0, tokenPos);
	}
	return rv;
}
bool tmpParser::Number13(int tokenPos)
{
	bool rv = false;
	if (!rv && stateVars[0]==16)
	{
		rv = ParseNumber(2, 1, 16, 0, tokenPos);
	}
	return rv;
}
bool tmpParser::Number14(int tokenPos)
{
	bool rv = false;
	if (!rv && stateVars[0]==16)
	{
		rv = ParseNumber(-2, 1, 16, 0, tokenPos);
	}
	return rv;
}
bool tmpParser::Number15(int tokenPos)
{
	bool rv = false;
	if (!rv && stateVars[0]==16)
	{
		rv = ParseNumber(2, 1, 16, 0, tokenPos);
	}
	return rv;
}
bool tmpParser::Number16(int tokenPos)
{
	bool rv = false;
	if (!rv && stateVars[0]==16)
	{
		rv = ParseNumber(-2, 1, 16, 0, tokenPos);
	}
	return rv;
}
bool tmpParser::Number17(int tokenPos)
{
	bool rv = false;
	if (!rv && stateVars[0]!=16)
	{
		rv = ParseNumber(4, 1, 32, 0, tokenPos);
	}
	return rv;
}
bool tmpParser::Number18(int tokenPos)
{
	bool rv = false;
	if (!rv)
		rv = SetNumber(tokenPos, 1, 0);
	if (!rv)
		rv = SetNumber(tokenPos, 2, 1);
	if (!rv)
		rv = SetNumber(tokenPos, 4, 2);
	if (!rv)
		rv = SetNumber(tokenPos, 8, 3);
	return rv;
}
bool tmpParser::Number19(int tokenPos)
{
	bool rv = false;
	if (!rv)
		rv = SetNumber(tokenPos, 0, 0);
	return rv;
}
bool tmpParser::Number20(int tokenPos)
{
	bool rv = false;
	if (!rv)
		rv = SetNumber(tokenPos, 1, 1);
	return rv;
}
tmpParser::NumberDispatchType tmpParser::numberFuncs[] = {
	&tmpParser::Number1,
	&tmpParser::Number2,
	&tmpParser::Number3,
	&tmpParser::Number4,
	&tmpParser::Number5,
	&tmpParser::Number6,
	&tmpParser::Number7,
	&tmpParser::Number8,
	&tmpParser::Number9,
	&tmpParser::Number10,
	&tmpParser::Number11,
	&tmpParser::Number12,
	&tmpParser::Number13,
	&tmpParser::Number14,
	&tmpParser::Number15,
	&tmpParser::Number16,
	&tmpParser::Number17,
	&tmpParser::Number18,
	&tmpParser::Number19,
	&tmpParser::Number20,
};

unsigned char tmpParser::RegClassData1[] = {1, };

unsigned char tmpParser::registerData22[] = {0, 0, 0, 32, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData7[] = {8, 16, 32, 64, 64, 64, 64, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData10[] = {136, 16, 33, 66, 68, 68, 68, 68, 4, 0, 0, 0, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData13[] = {16, 32, 64, 128, 128, 128, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData16[] = {16, 33, 66, 132, 136, 136, 136, 136, 8, 0, 0, 0, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData31[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 252, 3, 0, 0, };
unsigned char tmpParser::registerData32[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 252, 3, 0, };
unsigned char tmpParser::registerData30[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 252, 3, 0, 0, 0, };
unsigned char tmpParser::registerData1[] = {255, 255, 255, 255, 15, 0, 240, 255, 15, 0, 0, 0, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData5[] = {2, 4, 8, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData24[] = {0, 0, 0, 0, 0, 0, 32, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData8[] = {8, 16, 32, 64, 0, 64, 64, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData11[] = {136, 16, 33, 66, 4, 64, 68, 68, 4, 0, 0, 0, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData14[] = {16, 32, 64, 128, 0, 128, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData17[] = {16, 33, 66, 132, 8, 128, 136, 136, 8, 0, 0, 0, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData4[] = {33, 66, 132, 8, 17, 17, 17, 17, 1, 0, 0, 0, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData21[] = {0, 0, 0, 120, 0, 0, 96, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData6[] = {4, 8, 16, 32, 32, 32, 32, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData20[] = {64, 128, 0, 1, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData9[] = {8, 16, 32, 64, 64, 64, 64, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData12[] = {136, 16, 33, 66, 68, 68, 68, 68, 4, 0, 0, 0, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData15[] = {16, 32, 64, 128, 128, 128, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData18[] = {16, 33, 66, 132, 136, 136, 136, 136, 8, 0, 0, 0, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData19[] = {16, 33, 66, 132, 136, 136, 136, 136, 8, 0, 0, 0, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData2[] = {3, 6, 12, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData3[] = {33, 66, 132, 8, 17, 17, 17, 17, 1, 0, 0, 0, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData26[] = {0, 0, 0, 0, 0, 0, 0, 0, 144, 36, 73, 2, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData25[] = {0, 0, 0, 0, 0, 0, 0, 0, 240, 255, 255, 15, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData27[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 72, 146, 4, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData29[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 240, 3, 0, 0, 0, 0, };
unsigned char tmpParser::registerData28[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 144, 36, 9, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData23[] = {0, 0, 0, 0, 240, 255, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
unsigned char tmpParser::registerData33[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 252, 3, };

unsigned char *tmpParser::registerDataIndirect[] = {
	registerData1,
	registerData2,
	registerData3,
	registerData4,
	registerData5,
	registerData6,
	registerData7,
	registerData8,
	registerData9,
	registerData10,
	registerData11,
	registerData12,
	registerData13,
	registerData14,
	registerData15,
	registerData16,
	registerData17,
	registerData18,
	registerData19,
	registerData20,
	registerData21,
	registerData22,
	registerData23,
	registerData24,
	registerData25,
	registerData26,
	registerData27,
	registerData28,
	registerData29,
	registerData30,
	registerData31,
	registerData32,
	registerData33,
};

int tmpParser::registerValues[][6] = {
	{ 0, 0, 0, -1, -1, -1, },
	{ 4, 0, 0, -1, -1, -1, },
	{ 0, 0, 0, -1, -1, -1, },
	{ 0, 0, 0, -1, -1, -1, },
	{ 0, 0, 8, -1, -1, -1, },
	{ 0, 4, 0, -1, -1, -1, },
	{ 0, 4, 0, -1, -1, -1, },
	{ 0, 4, 0, -1, -1, -1, },
	{ 0, 4, 8, -1, -1, -1, },
	{ 1, 0, 0, -1, -1, -1, },
	{ 5, 0, 0, -1, -1, -1, },
	{ 1, 0, 0, -1, -1, -1, },
	{ 1, 0, 0, -1, -1, -1, },
	{ 1, 0, 8, -1, -1, -1, },
	{ 1, 4, 0, -1, -1, -1, },
	{ 1, 4, 0, -1, -1, -1, },
	{ 1, 4, 0, -1, -1, -1, },
	{ 1, 4, 8, -1, -1, -1, },
	{ 2, 0, 0, -1, -1, -1, },
	{ 6, 0, 0, -1, -1, -1, },
	{ 2, 0, 0, -1, -1, -1, },
	{ 2, 0, 0, -1, -1, -1, },
	{ 2, 0, 8, -1, -1, -1, },
	{ 2, 4, 0, -1, -1, -1, },
	{ 2, 4, 0, -1, -1, -1, },
	{ 2, 4, 0, -1, -1, -1, },
	{ 2, 4, 8, -1, -1, -1, },
	{ 3, 0, 0, -1, -1, -1, },
	{ 7, 0, 0, -1, -1, -1, },
	{ 3, 0, 0, 0, -1, -1, },
	{ 3, 0, 0, -1, -1, -1, },
	{ 3, 0, 8, -1, -1, -1, },
	{ 3, 4, 0, -1, -1, -1, },
	{ 3, 4, 0, -1, -1, -1, },
	{ 3, 4, 0, -1, -1, -1, },
	{ 3, 4, 8, -1, -1, -1, },
	{ 4, 0, 0, -1, -1, -1, },
	{ 4, 0, 0, -1, -1, -1, },
	{ 4, 0, 0, -1, -1, -1, },
	{ 4, 0, 8, -1, -1, -1, },
	{ 4, 4, 0, -1, -1, -1, },
	{ 4, 4, 0, -1, -1, -1, },
	{ 4, 4, 0, -1, -1, -1, },
	{ 4, 4, 8, -1, -1, -1, },
	{ 5, 0, 0, -1, -1, -1, },
	{ 5, 0, 0, 1, -1, -1, },
	{ 5, 0, 0, -1, -1, -1, },
	{ 5, 0, 8, -1, -1, -1, },
	{ 5, 4, 0, -1, -1, -1, },
	{ 5, 4, 0, -1, -1, -1, },
	{ 5, 4, 0, -1, -1, -1, },
	{ 5, 4, 8, -1, -1, -1, },
	{ 6, 0, 0, -1, -1, -1, },
	{ 6, 0, 0, -1, 0, -1, },
	{ 6, 0, 0, -1, -1, -1, },
	{ 6, 0, 8, -1, -1, -1, },
	{ 6, 4, 0, -1, -1, -1, },
	{ 6, 4, 0, -1, -1, -1, },
	{ 6, 4, 0, -1, -1, -1, },
	{ 6, 4, 8, -1, -1, -1, },
	{ 7, 0, 0, -1, -1, -1, },
	{ 7, 0, 0, -1, 1, -1, },
	{ 7, 0, 0, -1, -1, -1, },
	{ 7, 0, 8, -1, -1, -1, },
	{ 7, 4, 0, -1, -1, -1, },
	{ 7, 4, 0, -1, -1, -1, },
	{ 7, 4, 0, -1, -1, -1, },
	{ 7, 4, 8, -1, -1, -1, },
	{ 0, 0, 0, -1, -1, -1, },
	{ 0, 0, 0, -1, -1, -1, },
	{ 0, 4, 0, -1, -1, -1, },
	{ 1, 0, 0, -1, -1, -1, },
	{ 1, 0, 0, -1, -1, -1, },
	{ 1, 4, 0, -1, -1, -1, },
	{ 2, 0, 0, -1, -1, -1, },
	{ 2, 0, 0, -1, -1, -1, },
	{ 2, 4, 0, -1, -1, -1, },
	{ 3, 0, 0, -1, -1, -1, },
	{ 3, 0, 0, -1, -1, -1, },
	{ 3, 4, 0, -1, -1, -1, },
	{ 4, 0, 0, -1, -1, -1, },
	{ 4, 0, 0, -1, -1, -1, },
	{ 4, 4, 0, -1, -1, -1, },
	{ 5, 0, 0, -1, -1, -1, },
	{ 5, 0, 0, -1, -1, -1, },
	{ 5, 4, 0, -1, -1, -1, },
	{ 6, 0, 0, -1, -1, -1, },
	{ 6, 0, 0, -1, -1, -1, },
	{ 6, 4, 0, -1, -1, -1, },
	{ 7, 0, 0, -1, -1, -1, },
	{ 7, 0, 0, -1, -1, -1, },
	{ 7, 4, 0, -1, -1, -1, },
	{ 0, -1, -1, -1, -1, 38, },
	{ 1, -1, -1, -1, -1, 46, },
	{ 2, -1, -1, -1, -1, 54, },
	{ 3, -1, -1, -1, -1, 62, },
	{ 4, -1, -1, -1, -1, 100, },
	{ 5, -1, -1, -1, -1, 101, },
	{ 0, -1, -1, -1, -1, -1, },
	{ 1, -1, -1, -1, -1, -1, },
	{ 2, -1, -1, -1, -1, -1, },
	{ 3, -1, -1, -1, -1, -1, },
	{ 4, -1, -1, -1, -1, -1, },
	{ 5, -1, -1, -1, -1, -1, },
	{ 6, -1, -1, -1, -1, -1, },
	{ 7, -1, -1, -1, -1, -1, },
	{ 0, -1, -1, -1, -1, -1, },
	{ 1, -1, -1, -1, -1, -1, },
	{ 2, -1, -1, -1, -1, -1, },
	{ 3, -1, -1, -1, -1, -1, },
	{ 4, -1, -1, -1, -1, -1, },
	{ 5, -1, -1, -1, -1, -1, },
	{ 6, -1, -1, -1, -1, -1, },
	{ 7, -1, -1, -1, -1, -1, },
	{ 0, -1, -1, -1, -1, -1, },
	{ 1, -1, -1, -1, -1, -1, },
	{ 2, -1, -1, -1, -1, -1, },
	{ 3, -1, -1, -1, -1, -1, },
	{ 4, -1, -1, -1, -1, -1, },
	{ 5, -1, -1, -1, -1, -1, },
	{ 6, -1, -1, -1, -1, -1, },
	{ 7, -1, -1, -1, -1, -1, },
	{ 0, -1, -1, -1, -1, -1, },
	{ 1, -1, -1, -1, -1, -1, },
	{ 2, -1, -1, -1, -1, -1, },
	{ 3, -1, -1, -1, -1, -1, },
	{ 4, -1, -1, -1, -1, -1, },
	{ 5, -1, -1, -1, -1, -1, },
	{ 6, -1, -1, -1, -1, -1, },
	{ 7, -1, -1, -1, -1, -1, },
};

void tmpParser::Init()
{
	memset(&stateVars, 0, sizeof(stateVars));
	stateVars[0] = 16;
	tokenTable[""] = 0;
	tokenTable["al"] = 1000;
	tokenTable["ah"] = 1001;
	tokenTable["ax"] = 1002;
	tokenTable["eax"] = 1003;
	tokenTable["rax"] = 1004;
	tokenTable["r8b"] = 1005;
	tokenTable["r8w"] = 1006;
	tokenTable["r8d"] = 1007;
	tokenTable["r8"] = 1008;
	tokenTable["cl"] = 1009;
	tokenTable["ch"] = 1010;
	tokenTable["cx"] = 1011;
	tokenTable["ecx"] = 1012;
	tokenTable["rcx"] = 1013;
	tokenTable["r9b"] = 1014;
	tokenTable["r9w"] = 1015;
	tokenTable["r9d"] = 1016;
	tokenTable["r9"] = 1017;
	tokenTable["dl"] = 1018;
	tokenTable["dh"] = 1019;
	tokenTable["dx"] = 1020;
	tokenTable["edx"] = 1021;
	tokenTable["rdx"] = 1022;
	tokenTable["r10b"] = 1023;
	tokenTable["r10w"] = 1024;
	tokenTable["r10d"] = 1025;
	tokenTable["r10"] = 1026;
	tokenTable["bl"] = 1027;
	tokenTable["bh"] = 1028;
	tokenTable["bx"] = 1029;
	tokenTable["ebx"] = 1030;
	tokenTable["rbx"] = 1031;
	tokenTable["r11b"] = 1032;
	tokenTable["r11w"] = 1033;
	tokenTable["r11d"] = 1034;
	tokenTable["r11"] = 1035;
	tokenTable["spl"] = 1036;
	tokenTable["sp"] = 1037;
	tokenTable["esp"] = 1038;
	tokenTable["rsp"] = 1039;
	tokenTable["r12b"] = 1040;
	tokenTable["r12w"] = 1041;
	tokenTable["r12d"] = 1042;
	tokenTable["r12"] = 1043;
	tokenTable["bpl"] = 1044;
	tokenTable["bp"] = 1045;
	tokenTable["ebp"] = 1046;
	tokenTable["rbp"] = 1047;
	tokenTable["r13b"] = 1048;
	tokenTable["r13w"] = 1049;
	tokenTable["r13d"] = 1050;
	tokenTable["r13"] = 1051;
	tokenTable["sil"] = 1052;
	tokenTable["si"] = 1053;
	tokenTable["esi"] = 1054;
	tokenTable["rsi"] = 1055;
	tokenTable["r14b"] = 1056;
	tokenTable["r14w"] = 1057;
	tokenTable["r14d"] = 1058;
	tokenTable["r14"] = 1059;
	tokenTable["dil"] = 1060;
	tokenTable["di"] = 1061;
	tokenTable["edi"] = 1062;
	tokenTable["rdi"] = 1063;
	tokenTable["r15b"] = 1064;
	tokenTable["r15w"] = 1065;
	tokenTable["r15d"] = 1066;
	tokenTable["r15"] = 1067;
	tokenTable["mm0"] = 1068;
	tokenTable["xmm0"] = 1069;
	tokenTable["xmm8"] = 1070;
	tokenTable["mm1"] = 1071;
	tokenTable["xmm1"] = 1072;
	tokenTable["xmm9"] = 1073;
	tokenTable["mm2"] = 1074;
	tokenTable["xmm2"] = 1075;
	tokenTable["xmm10"] = 1076;
	tokenTable["mm3"] = 1077;
	tokenTable["xmm3"] = 1078;
	tokenTable["xmm11"] = 1079;
	tokenTable["mm4"] = 1080;
	tokenTable["xmm4"] = 1081;
	tokenTable["xmm12"] = 1082;
	tokenTable["mm5"] = 1083;
	tokenTable["xmm5"] = 1084;
	tokenTable["xmm13"] = 1085;
	tokenTable["mm6"] = 1086;
	tokenTable["xmm6"] = 1087;
	tokenTable["xmm14"] = 1088;
	tokenTable["mm7"] = 1089;
	tokenTable["xmm7"] = 1090;
	tokenTable["xmm15"] = 1091;
	tokenTable["es"] = 1092;
	tokenTable["cs"] = 1093;
	tokenTable["ss"] = 1094;
	tokenTable["ds"] = 1095;
	tokenTable["fs"] = 1096;
	tokenTable["gs"] = 1097;
	tokenTable["st0"] = 1098;
	tokenTable["st1"] = 1099;
	tokenTable["st2"] = 1100;
	tokenTable["st3"] = 1101;
	tokenTable["st4"] = 1102;
	tokenTable["st5"] = 1103;
	tokenTable["st6"] = 1104;
	tokenTable["st7"] = 1105;
	tokenTable["cr0"] = 1106;
	tokenTable["cr1"] = 1107;
	tokenTable["cr2"] = 1108;
	tokenTable["cr3"] = 1109;
	tokenTable["cr4"] = 1110;
	tokenTable["cr5"] = 1111;
	tokenTable["cr6"] = 1112;
	tokenTable["cr7"] = 1113;
	tokenTable["dr0"] = 1114;
	tokenTable["dr1"] = 1115;
	tokenTable["dr2"] = 1116;
	tokenTable["dr3"] = 1117;
	tokenTable["dr4"] = 1118;
	tokenTable["dr5"] = 1119;
	tokenTable["dr6"] = 1120;
	tokenTable["dr7"] = 1121;
	tokenTable["tr0"] = 1122;
	tokenTable["tr1"] = 1123;
	tokenTable["tr2"] = 1124;
	tokenTable["tr3"] = 1125;
	tokenTable["tr4"] = 1126;
	tokenTable["tr5"] = 1127;
	tokenTable["tr6"] = 1128;
	tokenTable["tr7"] = 1129;
	opcodeTable["aaa"] = 0;
	opcodeTable["bbb"] = 1;
	prefixTable["a16"] = 0;
	prefixTable["a32"] = 1;
	prefixTable["lock"] = 2;
	prefixTable["o16"] = 3;
	prefixTable["o32"] = 4;
	prefixTable["rep"] = 5;
	prefixTable["repe"] = 6;
	prefixTable["repne"] = 7;
	prefixTable["repnz"] = 8;
	prefixTable["repz"] = 9;
}

void tmpParser::TokenFunc2(tmpOperand &operand, int tokenPos)
{
	operand.addressCoding = 0;
}
void tmpParser::TokenFunc3(tmpOperand &operand, int tokenPos)
{
	operand.addressCoding = 1;
}
tmpToken tmpParser::tokenBranches1[] = {
	{tmpToken::REGISTER, 3, 1, 0, tmpParser::RegClassData1, &tmpParser::TokenFunc2,  },
	{tmpToken::REGISTER, 30, 1, 0, tmpParser::RegClassData1, &tmpParser::TokenFunc3,  },
	{tmpToken::EOT }
};
Coding tmpParser::tokenCoding5_0[] = {
	{ (Coding::Type)(Coding::valSpecified), 3, -1, -1, 0, 0 }, 
	{ Coding::eot },
};
Coding tmpParser::tokenCoding5_1[] = {
	{ (Coding::Type)(Coding::valSpecified), 4, -1, -1, 0, 0 }, 
	{ Coding::eot },
};
Coding tmpParser::tokenCoding5_2[] = {
	{ (Coding::Type)(Coding::valSpecified), 1, -1, -1, 0, 0 }, 
	{ Coding::eot },
};
Coding tmpParser::tokenCoding5_3[] = {
	{ (Coding::Type)(Coding::valSpecified), 2, -1, -1, 0, 0 }, 
	{ Coding::eot },
};
void tmpParser::TokenFunc5(tmpOperand &operand, int tokenPos)
{
	operand.values[0] = tokenCoding5_0;
	operand.values[1] = tokenCoding5_1;
	operand.values[2] = tokenCoding5_2;
	operand.values[3] = tokenCoding5_3;
}
tmpToken tmpParser::tokenBranches4[] = {
	{tmpToken::ADDRESSCLASS, 0, 1, 0, NULL,&tmpParser::TokenFunc5,  },
	{tmpToken::EOT }
};
Coding tmpParser::tokenCoding7_1[] = {
	{ (Coding::Type)(Coding::valSpecified), 7, -1, -1, 0, 0 }, 
	{ Coding::eot },
};
Coding tmpParser::tokenCoding7_2[] = {
	{ (Coding::Type)(Coding::valSpecified), 0, -1, -1, 0, 0 }, 
	{ Coding::eot },
};
Coding tmpParser::tokenCoding7_3[] = {
	{ (Coding::Type)(Coding::valSpecified), 0, -1, -1, 0, 0 }, 
	{ Coding::eot },
};
void tmpParser::TokenFunc7(tmpOperand &operand, int tokenPos)
{
	operand.values[1] = tokenCoding7_1;
	operand.values[2] = tokenCoding7_2;
	operand.values[3] = tokenCoding7_3;
}
tmpToken tmpParser::tokenBranches6[] = {
	{tmpToken::ADDRESSCLASS, 0, 1, 0, NULL,&tmpParser::TokenFunc7,  },
	{tmpToken::EOT }
};
Coding tmpParser::OpcodeCodings0_5[] = {
	{ (Coding::Type)(Coding::valSpecified), 55, -1, -1, 0, 0 }, 
	{ Coding::eot },
};
bool tmpParser::Opcode0(tmpOperand &operand)
{
	operand.values[5] = OpcodeCodings0_5;
	bool rv = ParseOperands(tokenBranches4, operand);
	return rv;
}
Coding tmpParser::OpcodeCodings1_5[] = {
	{ (Coding::Type)(Coding::valSpecified), 85, -1, -1, 0, 0 }, 
	{ Coding::eot },
};
bool tmpParser::Opcode1(tmpOperand &operand)
{
	operand.values[5] = OpcodeCodings1_5;
	bool rv = ParseOperands(tokenBranches6, operand);
	return rv;
}
tmpParser::DispatchType tmpParser::DispatchTable[2] = {
	&tmpParser::Opcode0,
	&tmpParser::Opcode1,
};
Coding tmpParser::Coding1[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::optional | Coding::indirect), 0, -1, -1, 0, 0 }, 
	{ (Coding::Type)(Coding::indirect), 5, -1, -1, 0, 0 }, 
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2, -1, 0, 0 }, 
	{ (Coding::Type)(Coding::bitSpecified | Coding::indirect), 1, 3, -1, 0, 0 }, 
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3, -1, 0, 0 }, 
	{ Coding::eot },
};
Coding tmpParser::Coding2[] = {
	{ (Coding::Type)(Coding::optional | Coding::indirect), 0, -1, -1, 0, 0 }, 
	{ (Coding::Type)(Coding::indirect), 5, -1, -1, 0, 0 }, 
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2, -1, 0, 0 }, 
	{ (Coding::Type)(Coding::indirect), 2, -1, -1, 0, '+' }, 
	{ (Coding::Type)(Coding::indirect), 3, -1, -1, 0, '-' }, 
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 3, -1, 0, 0 }, 
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3, -1, 0, 0 }, 
	{ Coding::eot },
};
Coding * tmpParser::Codings[2] = {
	tmpParser::Coding1,
	tmpParser::Coding2,
};
Coding tmpParser::prefixCoding1[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::eot },
};
Coding tmpParser::prefixCoding2[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::eot },
};
Coding tmpParser::prefixCoding3[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 240, 8, -1, 0, 0 }, 
	{ Coding::eot },
};
Coding tmpParser::prefixCoding4[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::eot },
};
Coding tmpParser::prefixCoding5[] = {
	{ Coding::stateFunc, 4 },
	{ Coding::eot },
};
Coding tmpParser::prefixCoding6[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 243, 8, -1, 0, 0 }, 
	{ Coding::eot },
};
Coding tmpParser::prefixCoding7[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 243, 8, -1, 0, 0 }, 
	{ Coding::eot },
};
Coding tmpParser::prefixCoding8[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 242, 8, -1, 0, 0 }, 
	{ Coding::eot },
};
Coding tmpParser::prefixCoding9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 242, 8, -1, 0, 0 }, 
	{ Coding::eot },
};
Coding tmpParser::prefixCoding10[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 243, 8, -1, 0, 0 }, 
	{ Coding::eot },
};
Coding *tmpParser::prefixCodings[] = {
	tmpParser::prefixCoding1,
	tmpParser::prefixCoding2,
	tmpParser::prefixCoding3,
	tmpParser::prefixCoding4,
	tmpParser::prefixCoding5,
	tmpParser::prefixCoding6,
	tmpParser::prefixCoding7,
	tmpParser::prefixCoding8,
	tmpParser::prefixCoding9,
	tmpParser::prefixCoding10,
};
bool tmpParser::MatchesToken(int token, int tokenPos)
{
	return tokenPos < inputTokens.size() && inputTokens[tokenPos]->type == InputToken::TOKEN && inputTokens[tokenPos]->val->ival == token;
}
	bool tmpParser::MatchesRegister(int reg, int tokenPos)
{
	return tokenPos < inputTokens.size() && inputTokens[tokenPos]->type == InputToken::REGISTER && inputTokens[tokenPos]->val->ival == reg;
}
	bool tmpParser::MatchesRegisterClass(int cclass, int tokenPos)
{
	return tokenPos < inputTokens.size() && inputTokens[tokenPos]->type == InputToken::REGISTER && (registerDataIndirect[cclass][inputTokens[tokenPos]->val->ival >> 3] & (1 << (inputTokens[tokenPos]->val->ival & 7)));
}
bool tmpParser::ParseAddresses(tmpOperand &operand, int addrClass, int &tokenPos)
{
	int level = 0;
	bool rv = false;
	tmpToken *t = tokenBranches1;
	while (t->type != tmpToken::EOT)
	{
		bool matches = false;
		if (t->addrClass[addrClass >> 3] & (1 << (addrClass & 7)))
		{
			switch (t->type)
			{
				case tmpToken::EMPTY:
					matches = true;
					break;
				case tmpToken::TOKEN:
					matches = MatchesToken(t->id, tokenPos);
					break;
				case tmpToken::REGISTER:
					matches = MatchesRegister(t->id, tokenPos);
					break;
				case tmpToken::REGISTERCLASS:
					matches = MatchesRegisterClass(t->id, tokenPos);
					break;
				case tmpToken::NUMBER:
					if (tokenPos < inputTokens.size())
						matches = (this->*numberFuncs[t->id])(tokenPos);
					break;
				default:
					matches = false;
					break;
			}
		}
		if (!matches)
		{
			if (t->level > level)
			{
				t = t->next;
				continue;
			}
		}
		else
			level = t->level;
		if (matches)
		{
			if (t->tokenFunc)
				(this->*(t->tokenFunc))(operand, tokenPos);
			if (t->eos && (tokenPos == inputTokens.size()-1 || !t->next))
			{
				rv = true;
				break;
			}
			t = t->next;
			tokenPos++;
		}
		else
		{
			t++;
		}
	}
	return rv;
}

bool tmpParser::ParseOperands2(tmpToken *tokenList, tmpOperand &operand, int tokenPos, int level)
{
	bool rv = false;
	tmpToken *t = tokenList;
	while (t && t->type != tmpToken::EOT)
	{
		bool matches = false;
		int last = tokenPos;
		switch (t->type)
		{
			case tmpToken::EMPTY:
				matches = true;
				break;
			case tmpToken::TOKEN:
				matches = MatchesToken(t->id, tokenPos);
				break;
			case tmpToken::REGISTER:
				matches = MatchesRegister(t->id, tokenPos);
				break;
			case tmpToken::REGISTERCLASS:
				matches = MatchesRegisterClass(t->id,tokenPos);
				break;
			case tmpToken::ADDRESSCLASS:
				matches = ParseAddresses(operand, t->id, tokenPos);
				break;
			case tmpToken::NUMBER:
				if (tokenPos < inputTokens.size())
					matches = (this->*numberFuncs[t->id])(tokenPos);
				break;
			default:
				matches = false;
				break;
		}
		if (!matches)
		{
			if (t->level > level)
			{
				if (ParseOperands2(t->next, operand, tokenPos, t->level))
				{
					rv = true;
					break;
				}
			}
		}
		else
		{
			if (t->tokenFunc)
				(this->*(t->tokenFunc))(operand, last);
			if (t->eos && (!t->next || tokenPos == inputTokens.size()-1))
			{
				if (tokenPos >= (int)(inputTokens.size()-1))
					eol = true;
				rv = true;
				break;
			}
			if (tokenPos < inputTokens.size() && ParseOperands2(t->next, operand, tokenPos+1, t->level))
			{
				rv = true;
				break;
			}
		}
		tokenPos = last;
		t++;
	}
	return rv;
}

bool tmpParser::ParseOperands(tmpToken *tokenList, tmpOperand &operand)
{
	return ParseOperands2(tokenList, operand, 0, 0);
}

bool tmpParser::ProcessCoding(tmpOperand &operand, Coding *coding, int bits, int* arr, char* bitcounts, char *func, int &index)
{
	int acc = 0, binary = 0;
	while (coding->type != Coding::eot)
	{
		if (coding->type & Coding::bitSpecified)
			bits = coding->bits;
		if (coding->type & Coding::valSpecified)
		{
			func[index] = coding->binary;
			bitcounts[index] = bits;
			arr[index++] = coding->val;
		}
		else if (coding->type & Coding::reg)
		{
			int n = coding->val;
			if (coding->field != -1)
				n = registerValues[n][coding->field];
			func[index] = coding->binary;
			bitcounts[index] = bits;
			arr[index++] = n;
		}
		else if (coding->type & Coding::stateFunc)
		{
			Coding *c = (this->*stateFuncs[coding->val])();
			int index1 = index;
			if (!ProcessCoding(operand, c, bits, arr, bitcounts, func, index))
				return false;
			if (index != index1 && coding->binary)
				func[index - 1] = coding->binary;
		}
		else if (coding->type & Coding::stateVar)
		{
			func[index] = coding->binary;
			bitcounts[index] = bits;
			arr[index++] = stateVars[coding->val];
		}
		else if (coding->type & Coding::number)
		{
			int n = coding->val;
			auto it = operands.begin();
			for (int i = 0; i < n; i++)
			{
				++it;
			}
			(*it)->used = true;
			(*it)->pos = this->bits.GetBits();
			func[index] = coding->binary;
			bitcounts[index] = bits;
			arr[index++] = (*it)->node->ival;
		}
		else if (coding->type & Coding::native)
		{
			if (operand.addressCoding == -1)
				return false;
			int index1 = index;
			if (!ProcessCoding(operand, Codings[operand.addressCoding], bits, arr, bitcounts, func, index))
				return false;
			if (index != index1 && coding->binary)
				func[index - 1] = coding->binary;
		}
		else if (coding->type & Coding::indirect)
		{
			if (!operand.values[coding->val])
			{
				if (!(coding->type & Coding::optional))
				{
					return false;
				}
			}
			else
			{
				int index1 = index;
				if (!ProcessCoding(operand, operand.values[coding->val], bits, arr, bitcounts, func, index))
					return false;
				if (index != index1 && coding->binary)
					func[index - 1] = coding->binary;
			}
		}
		else if (coding->type & Coding::illegal)
		{
			return false;
		}
		else
		{
			return false;
		}
		coding++;
	}
	return true;
}
bool tmpParser::ProcessCoding(tmpOperand &operand, Coding *coding)
{
	int arr[1000];
	char bitcount[1000];
	char func[1000];
	int index = 0;
	if (coding->type == Coding::eot)
		return true;
	int defaultBits = 8;
	bool rv = ProcessCoding(operand, coding, defaultBits, arr, bitcount, func, index);
	if (rv)
	{
		for (int i = 0; i < index; i++)
		{
			if (func[i])
				arr[i + 1] = DoMath(func[i], arr[i], arr[i + 1]);
			else
				bits.Add(arr[i], bitcount[i]);
		}
	}
	return rv;
}
bool tmpParser::DispatchOpcode(int opcode)
{
	bool rv;
	if (opcode == -1)
	{
		rv = true;
		tmpOperand operand;
		for (auto& a : prefixes)
			rv &= ProcessCoding(operand, prefixCodings[a]);
	}
	else
	{
		tmpOperand operand;
		operand.opcode = opcode;
		rv = (this->*DispatchTable[opcode])(operand);
		if (rv)
		{
			for (auto& a : prefixes)
				rv &= ProcessCoding(operand, prefixCodings[a]);
			if (rv)
			{
				if (operand.operandCoding != -1)
					rv = ProcessCoding(operand, Codings[operand.operandCoding]);
				else if (operand.addressCoding != -1)
					rv = ProcessCoding(operand, Codings[operand.addressCoding]);
				else rv = false;
			}
		}
	}
	return rv;
}

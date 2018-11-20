#include "InstructionParser.h"
#include "x64Operand.h"
#include "x64Parser.h"

InstructionParser *InstructionParser::GetInstance()
{
	return static_cast<InstructionParser *>(new x64Parser());
}
Coding x64Parser::stateCoding_eot[] = { { Coding::eot } };
Coding x64Parser::stateCoding1_1[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 103, 8},
	{ Coding::eot },
};
Coding x64Parser::stateCoding1_2[] = {
	{ Coding::eot },
};
Coding x64Parser::stateCoding2_1[] = {
	{ Coding::eot },
};
Coding x64Parser::stateCoding2_2[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 103, 8},
	{ Coding::eot },
};
Coding x64Parser::stateCoding3_1[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 102, 8},
	{ Coding::eot },
};
Coding x64Parser::stateCoding3_2[] = {
	{ Coding::eot },
};
Coding x64Parser::stateCoding4_1[] = {
	{ Coding::eot },
};
Coding x64Parser::stateCoding4_2[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 102, 8},
	{ Coding::eot },
};
Coding *x64Parser::StateFunc1()
{
	if (stateVars[0]!=0)
	{
		return stateCoding1_1;
	}
	if (stateVars[0]==0)
	{
		return stateCoding1_2;
	}
	return stateCoding_eot;
}
Coding *x64Parser::StateFunc2()
{
	if (stateVars[0]!=0)
	{
		return stateCoding2_1;
	}
	if (stateVars[0]==0)
	{
		return stateCoding2_2;
	}
	return stateCoding_eot;
}
Coding *x64Parser::StateFunc3()
{
	if (stateVars[0]!=0)
	{
		return stateCoding3_1;
	}
	if (stateVars[0]==0)
	{
		return stateCoding3_2;
	}
	return stateCoding_eot;
}
Coding *x64Parser::StateFunc4()
{
	if (stateVars[0]!=0)
	{
		return stateCoding4_1;
	}
	if (stateVars[0]==0)
	{
		return stateCoding4_2;
	}
	return stateCoding_eot;
}
x64Parser::StateFuncDispatchType x64Parser::stateFuncs[] = {
	&x64Parser::StateFunc1,
	&x64Parser::StateFunc2,
	&x64Parser::StateFunc3,
	&x64Parser::StateFunc4,
};
bool x64Parser::Number1(int tokenPos)
{
	bool rv = false;
	if (!rv)
	{
		rv = ParseNumber(0, 1, 8, 1, tokenPos);
	}
	return rv;
}
bool x64Parser::Number2(int tokenPos)
{
	bool rv = false;
	if (!rv)
	{
		rv = ParseNumber(0, 1, 8, 1, tokenPos);
	}
	return rv;
}
bool x64Parser::Number3(int tokenPos)
{
	bool rv = false;
	if (!rv)
	{
		rv = ParseNumber(0, 0, 6, 1, tokenPos);
	}
	return rv;
}
bool x64Parser::Number4(int tokenPos)
{
	bool rv = false;
	if (!rv)
	{
		rv = ParseNumber(0, 0, 8, 1, tokenPos);
	}
	return rv;
}
bool x64Parser::Number5(int tokenPos)
{
	bool rv = false;
	if (!rv)
	{
		rv = ParseNumber(0, 0, 16, 0, tokenPos);
	}
	return rv;
}
bool x64Parser::Number6(int tokenPos)
{
	bool rv = false;
	if (!rv)
	{
		rv = ParseNumber(0, 0, 32, 0, tokenPos);
	}
	return rv;
}
bool x64Parser::Number7(int tokenPos)
{
	bool rv = false;
	if (!rv && stateVars[0]==0)
	{
		rv = ParseNumber(0, 0, 16, 0, tokenPos);
	}
	return rv;
}
bool x64Parser::Number8(int tokenPos)
{
	bool rv = false;
	if (!rv && stateVars[0]==1)
	{
		rv = ParseNumber(0, 0, 32, 0, tokenPos);
	}
	return rv;
}
bool x64Parser::Number9(int tokenPos)
{
	bool rv = false;
	if (!rv)
	{
		rv = ParseNumber(1, 1, 8, 0, tokenPos);
	}
	return rv;
}
bool x64Parser::Number10(int tokenPos)
{
	bool rv = false;
	if (!rv)
	{
		rv = ParseNumber(-1, 1, 8, 0, tokenPos);
	}
	return rv;
}
bool x64Parser::Number11(int tokenPos)
{
	bool rv = false;
	if (!rv && stateVars[0]==0)
	{
		rv = ParseNumber(2, 1, 16, 0, tokenPos);
	}
	return rv;
}
bool x64Parser::Number12(int tokenPos)
{
	bool rv = false;
	if (!rv && stateVars[0]==0)
	{
		rv = ParseNumber(-2, 1, 16, 0, tokenPos);
	}
	return rv;
}
bool x64Parser::Number13(int tokenPos)
{
	bool rv = false;
	if (!rv && stateVars[0]==0)
	{
		rv = ParseNumber(2, 1, 16, 0, tokenPos);
	}
	return rv;
}
bool x64Parser::Number14(int tokenPos)
{
	bool rv = false;
	if (!rv && stateVars[0]==0)
	{
		rv = ParseNumber(-2, 1, 16, 0, tokenPos);
	}
	return rv;
}
bool x64Parser::Number15(int tokenPos)
{
	bool rv = false;
	if (!rv && stateVars[0]!=0)
	{
		rv = ParseNumber(4, 1, 32, 0, tokenPos);
	}
	return rv;
}
bool x64Parser::Number16(int tokenPos)
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
bool x64Parser::Number17(int tokenPos)
{
	bool rv = false;
	if (!rv)
		rv = SetNumber(tokenPos, 0, 0);
	return rv;
}
bool x64Parser::Number18(int tokenPos)
{
	bool rv = false;
	if (!rv)
		rv = SetNumber(tokenPos, 1, 1);
	return rv;
}
x64Parser::NumberDispatchType x64Parser::numberFuncs[] = {
	&x64Parser::Number1,
	&x64Parser::Number2,
	&x64Parser::Number3,
	&x64Parser::Number4,
	&x64Parser::Number5,
	&x64Parser::Number6,
	&x64Parser::Number7,
	&x64Parser::Number8,
	&x64Parser::Number9,
	&x64Parser::Number10,
	&x64Parser::Number11,
	&x64Parser::Number12,
	&x64Parser::Number13,
	&x64Parser::Number14,
	&x64Parser::Number15,
	&x64Parser::Number16,
	&x64Parser::Number17,
	&x64Parser::Number18,
};

unsigned char x64Parser::RegClassData1[] = {255, 0, };
unsigned char x64Parser::RegClassData2[] = {127, 0, };
unsigned char x64Parser::RegClassData3[] = {191, 0, };
unsigned char x64Parser::RegClassData4[] = {9, 1, };
unsigned char x64Parser::RegClassData5[] = {17, 2, };
unsigned char x64Parser::RegClassData6[] = {33, 4, };
unsigned char x64Parser::RegClassData7[] = {4, 8, };

unsigned char x64Parser::registerData10[] = {0, 64, 4, 0, 0, 0, 0, 0, };
unsigned char x64Parser::registerData6[] = {136, 136, 170, 0, 0, 0, 0, 0, };
unsigned char x64Parser::registerData15[] = {0, 0, 0, 0, 192, 63, 0, 0, };
unsigned char x64Parser::registerData16[] = {0, 0, 0, 0, 0, 192, 63, 0, };
unsigned char x64Parser::registerData14[] = {0, 0, 0, 192, 63, 0, 0, 0, };
unsigned char x64Parser::registerData1[] = {255, 255, 240, 0, 0, 0, 0, 0, };
unsigned char x64Parser::registerData4[] = {34, 34, 0, 0, 0, 0, 0, 0, };
unsigned char x64Parser::registerData12[] = {0, 0, 80, 0, 0, 0, 0, 0, };
unsigned char x64Parser::registerData7[] = {136, 136, 168, 0, 0, 0, 0, 0, };
unsigned char x64Parser::registerData3[] = {17, 17, 0, 0, 0, 0, 0, 0, };
unsigned char x64Parser::registerData9[] = {0, 240, 240, 0, 0, 0, 0, 0, };
unsigned char x64Parser::registerData5[] = {68, 68, 85, 0, 0, 0, 0, 0, };
unsigned char x64Parser::registerData8[] = {136, 136, 170, 0, 0, 0, 0, 0, };
unsigned char x64Parser::registerData2[] = {51, 51, 0, 0, 0, 0, 0, 0, };
unsigned char x64Parser::registerData13[] = {0, 0, 0, 63, 0, 0, 0, 0, };
unsigned char x64Parser::registerData11[] = {0, 0, 15, 0, 0, 0, 0, 0, };
unsigned char x64Parser::registerData17[] = {0, 0, 0, 0, 0, 0, 192, 63, };

unsigned char *x64Parser::registerDataIndirect[] = {
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
};

int x64Parser::registerValues[][4] = {
	{ 0, -1, -1, -1, },
	{ 4, -1, -1, -1, },
	{ 0, -1, -1, -1, },
	{ 0, -1, -1, -1, },
	{ 1, -1, -1, -1, },
	{ 5, -1, -1, -1, },
	{ 1, -1, -1, -1, },
	{ 1, -1, -1, -1, },
	{ 2, -1, -1, -1, },
	{ 6, -1, -1, -1, },
	{ 2, -1, -1, -1, },
	{ 2, -1, -1, -1, },
	{ 3, -1, -1, -1, },
	{ 7, -1, -1, -1, },
	{ 3, 0, -1, -1, },
	{ 3, -1, -1, -1, },
	{ 4, -1, -1, -1, },
	{ 4, -1, -1, -1, },
	{ 5, 1, -1, -1, },
	{ 5, -1, -1, -1, },
	{ 6, -1, 0, -1, },
	{ 6, -1, -1, -1, },
	{ 7, -1, 1, -1, },
	{ 7, -1, -1, -1, },
	{ 0, -1, -1, 38, },
	{ 1, -1, -1, 46, },
	{ 2, -1, -1, 54, },
	{ 3, -1, -1, 62, },
	{ 4, -1, -1, 100, },
	{ 5, -1, -1, 101, },
	{ 0, -1, -1, -1, },
	{ 1, -1, -1, -1, },
	{ 2, -1, -1, -1, },
	{ 3, -1, -1, -1, },
	{ 4, -1, -1, -1, },
	{ 5, -1, -1, -1, },
	{ 6, -1, -1, -1, },
	{ 7, -1, -1, -1, },
	{ 0, -1, -1, -1, },
	{ 1, -1, -1, -1, },
	{ 2, -1, -1, -1, },
	{ 3, -1, -1, -1, },
	{ 4, -1, -1, -1, },
	{ 5, -1, -1, -1, },
	{ 6, -1, -1, -1, },
	{ 7, -1, -1, -1, },
	{ 0, -1, -1, -1, },
	{ 1, -1, -1, -1, },
	{ 2, -1, -1, -1, },
	{ 3, -1, -1, -1, },
	{ 4, -1, -1, -1, },
	{ 5, -1, -1, -1, },
	{ 6, -1, -1, -1, },
	{ 7, -1, -1, -1, },
	{ 0, -1, -1, -1, },
	{ 1, -1, -1, -1, },
	{ 2, -1, -1, -1, },
	{ 3, -1, -1, -1, },
	{ 4, -1, -1, -1, },
	{ 5, -1, -1, -1, },
	{ 6, -1, -1, -1, },
	{ 7, -1, -1, -1, },
};

void x64Parser::Init()
{
	memset(&stateVars, 0, sizeof(stateVars));
	stateVars[0] = 0;
	tokenTable[""] = 0;
	tokenTable["*"] = 5;
	tokenTable["+"] = 4;
	tokenTable[","] = 7;
	tokenTable[":"] = 3;
	tokenTable["["] = 1;
	tokenTable["]"] = 2;
	tokenTable["byte"] = 10;
	tokenTable["dword"] = 9;
	tokenTable["far"] = 15;
	tokenTable["near"] = 14;
	tokenTable["qword"] = 11;
	tokenTable["short"] = 13;
	tokenTable["tword"] = 12;
	tokenTable["word"] = 8;
	tokenTable["al"] = 1000;
	tokenTable["ah"] = 1001;
	tokenTable["ax"] = 1002;
	tokenTable["eax"] = 1003;
	tokenTable["cl"] = 1004;
	tokenTable["ch"] = 1005;
	tokenTable["cx"] = 1006;
	tokenTable["ecx"] = 1007;
	tokenTable["dl"] = 1008;
	tokenTable["dh"] = 1009;
	tokenTable["dx"] = 1010;
	tokenTable["edx"] = 1011;
	tokenTable["bl"] = 1012;
	tokenTable["bh"] = 1013;
	tokenTable["bx"] = 1014;
	tokenTable["ebx"] = 1015;
	tokenTable["sp"] = 1016;
	tokenTable["esp"] = 1017;
	tokenTable["bp"] = 1018;
	tokenTable["ebp"] = 1019;
	tokenTable["si"] = 1020;
	tokenTable["esi"] = 1021;
	tokenTable["di"] = 1022;
	tokenTable["edi"] = 1023;
	tokenTable["es"] = 1024;
	tokenTable["cs"] = 1025;
	tokenTable["ss"] = 1026;
	tokenTable["ds"] = 1027;
	tokenTable["fs"] = 1028;
	tokenTable["gs"] = 1029;
	tokenTable["st0"] = 1030;
	tokenTable["st1"] = 1031;
	tokenTable["st2"] = 1032;
	tokenTable["st3"] = 1033;
	tokenTable["st4"] = 1034;
	tokenTable["st5"] = 1035;
	tokenTable["st6"] = 1036;
	tokenTable["st7"] = 1037;
	tokenTable["cr0"] = 1038;
	tokenTable["cr1"] = 1039;
	tokenTable["cr2"] = 1040;
	tokenTable["cr3"] = 1041;
	tokenTable["cr4"] = 1042;
	tokenTable["cr5"] = 1043;
	tokenTable["cr6"] = 1044;
	tokenTable["cr7"] = 1045;
	tokenTable["dr0"] = 1046;
	tokenTable["dr1"] = 1047;
	tokenTable["dr2"] = 1048;
	tokenTable["dr3"] = 1049;
	tokenTable["dr4"] = 1050;
	tokenTable["dr5"] = 1051;
	tokenTable["dr6"] = 1052;
	tokenTable["dr7"] = 1053;
	tokenTable["tr0"] = 1054;
	tokenTable["tr1"] = 1055;
	tokenTable["tr2"] = 1056;
	tokenTable["tr3"] = 1057;
	tokenTable["tr4"] = 1058;
	tokenTable["tr5"] = 1059;
	tokenTable["tr6"] = 1060;
	tokenTable["tr7"] = 1061;
	opcodeTable["aaa"] = 19;
	opcodeTable["aad"] = 20;
	opcodeTable["aam"] = 21;
	opcodeTable["aas"] = 22;
	opcodeTable["adc"] = 23;
	opcodeTable["add"] = 24;
	opcodeTable["and"] = 25;
	opcodeTable["arpl"] = 26;
	opcodeTable["bound"] = 27;
	opcodeTable["bsf"] = 28;
	opcodeTable["bsr"] = 29;
	opcodeTable["bswap"] = 30;
	opcodeTable["bt"] = 31;
	opcodeTable["btc"] = 32;
	opcodeTable["btr"] = 33;
	opcodeTable["bts"] = 34;
	opcodeTable["call"] = 35;
	opcodeTable["cbw"] = 36;
	opcodeTable["cdq"] = 37;
	opcodeTable["clc"] = 38;
	opcodeTable["cld"] = 39;
	opcodeTable["cli"] = 40;
	opcodeTable["clts"] = 41;
	opcodeTable["cmc"] = 42;
	opcodeTable["cmova"] = 43;
	opcodeTable["cmovae"] = 44;
	opcodeTable["cmovb"] = 45;
	opcodeTable["cmovbe"] = 46;
	opcodeTable["cmovc"] = 47;
	opcodeTable["cmove"] = 48;
	opcodeTable["cmovg"] = 49;
	opcodeTable["cmovge"] = 50;
	opcodeTable["cmovl"] = 51;
	opcodeTable["cmovle"] = 52;
	opcodeTable["cmovna"] = 53;
	opcodeTable["cmovnae"] = 54;
	opcodeTable["cmovnb"] = 55;
	opcodeTable["cmovnbe"] = 56;
	opcodeTable["cmovnc"] = 57;
	opcodeTable["cmovne"] = 58;
	opcodeTable["cmovng"] = 59;
	opcodeTable["cmovnge"] = 60;
	opcodeTable["cmovnl"] = 61;
	opcodeTable["cmovnle"] = 62;
	opcodeTable["cmovno"] = 63;
	opcodeTable["cmovnp"] = 64;
	opcodeTable["cmovns"] = 65;
	opcodeTable["cmovnz"] = 66;
	opcodeTable["cmovo"] = 67;
	opcodeTable["cmovp"] = 68;
	opcodeTable["cmovpe"] = 69;
	opcodeTable["cmovpo"] = 70;
	opcodeTable["cmovs"] = 71;
	opcodeTable["cmovz"] = 72;
	opcodeTable["cmp"] = 73;
	opcodeTable["cmps"] = 74;
	opcodeTable["cmpsb"] = 75;
	opcodeTable["cmpsw"] = 76;
	opcodeTable["cmpsd"] = 77;
	opcodeTable["cmpxchg"] = 78;
	opcodeTable["cmpxchg8b"] = 79;
	opcodeTable["cpuid"] = 80;
	opcodeTable["cwd"] = 81;
	opcodeTable["cwde"] = 82;
	opcodeTable["daa"] = 83;
	opcodeTable["das"] = 84;
	opcodeTable["dec"] = 85;
	opcodeTable["div"] = 86;
	opcodeTable["enter"] = 87;
	opcodeTable["esc"] = 88;
	opcodeTable["f2xm1"] = 89;
	opcodeTable["fabs"] = 90;
	opcodeTable["fadd"] = 91;
	opcodeTable["faddp"] = 92;
	opcodeTable["fbld"] = 93;
	opcodeTable["fbstp"] = 94;
	opcodeTable["fchs"] = 95;
	opcodeTable["fclex"] = 96;
	opcodeTable["fnclex"] = 97;
	opcodeTable["fcom"] = 98;
	opcodeTable["fcomp"] = 99;
	opcodeTable["fcompp"] = 100;
	opcodeTable["fcos"] = 101;
	opcodeTable["fdecstp"] = 102;
	opcodeTable["fdisi"] = 103;
	opcodeTable["fdiv"] = 104;
	opcodeTable["fdivp"] = 105;
	opcodeTable["fdivr"] = 106;
	opcodeTable["fdivrp"] = 107;
	opcodeTable["feni"] = 108;
	opcodeTable["ffree"] = 109;
	opcodeTable["fiadd"] = 110;
	opcodeTable["ficom"] = 111;
	opcodeTable["ficomp"] = 112;
	opcodeTable["fidiv"] = 113;
	opcodeTable["fidivr"] = 114;
	opcodeTable["fild"] = 115;
	opcodeTable["fimul"] = 116;
	opcodeTable["fincstp"] = 117;
	opcodeTable["finit"] = 118;
	opcodeTable["fninit"] = 119;
	opcodeTable["fist"] = 120;
	opcodeTable["fistp"] = 121;
	opcodeTable["fisub"] = 122;
	opcodeTable["fisubr"] = 123;
	opcodeTable["fld"] = 124;
	opcodeTable["fld1"] = 125;
	opcodeTable["fldcw"] = 126;
	opcodeTable["fldenv"] = 127;
	opcodeTable["fldl2e"] = 128;
	opcodeTable["fldl2t"] = 129;
	opcodeTable["fldlg2"] = 130;
	opcodeTable["fldln2"] = 131;
	opcodeTable["fldpi"] = 132;
	opcodeTable["fldz"] = 133;
	opcodeTable["fmul"] = 134;
	opcodeTable["fmulp"] = 135;
	opcodeTable["fnop"] = 136;
	opcodeTable["fnsave"] = 137;
	opcodeTable["fpatan"] = 138;
	opcodeTable["fprem"] = 139;
	opcodeTable["fprem1"] = 140;
	opcodeTable["fptan"] = 141;
	opcodeTable["frndint"] = 142;
	opcodeTable["frstor"] = 143;
	opcodeTable["fsave"] = 144;
	opcodeTable["fscale"] = 145;
	opcodeTable["fsetpm"] = 146;
	opcodeTable["fsin"] = 147;
	opcodeTable["fsincos"] = 148;
	opcodeTable["fsqrt"] = 149;
	opcodeTable["fst"] = 150;
	opcodeTable["fstcw"] = 151;
	opcodeTable["fnstcw"] = 152;
	opcodeTable["fstenv"] = 153;
	opcodeTable["fnstenv"] = 154;
	opcodeTable["fstp"] = 155;
	opcodeTable["fstsw"] = 156;
	opcodeTable["fnstsw"] = 157;
	opcodeTable["fsub"] = 158;
	opcodeTable["fsubp"] = 159;
	opcodeTable["fsubr"] = 160;
	opcodeTable["fsubrp"] = 161;
	opcodeTable["ftst"] = 162;
	opcodeTable["fucom"] = 163;
	opcodeTable["fucomp"] = 164;
	opcodeTable["fucompp"] = 165;
	opcodeTable["fwait"] = 166;
	opcodeTable["fxam"] = 167;
	opcodeTable["fxch"] = 168;
	opcodeTable["fxtract"] = 169;
	opcodeTable["fyl2x"] = 170;
	opcodeTable["fyl2xp1"] = 171;
	opcodeTable["hlt"] = 172;
	opcodeTable["ibts"] = 173;
	opcodeTable["idiv"] = 174;
	opcodeTable["imul"] = 175;
	opcodeTable["in"] = 176;
	opcodeTable["ins"] = 177;
	opcodeTable["insb"] = 178;
	opcodeTable["insw"] = 179;
	opcodeTable["insd"] = 180;
	opcodeTable["inc"] = 181;
	opcodeTable["int"] = 182;
	opcodeTable["int1"] = 183;
	opcodeTable["int3"] = 184;
	opcodeTable["into"] = 185;
	opcodeTable["invd"] = 186;
	opcodeTable["invlpg"] = 187;
	opcodeTable["iret"] = 188;
	opcodeTable["iretw"] = 189;
	opcodeTable["iretd"] = 190;
	opcodeTable["ja"] = 191;
	opcodeTable["jae"] = 192;
	opcodeTable["jb"] = 193;
	opcodeTable["jbe"] = 194;
	opcodeTable["jc"] = 195;
	opcodeTable["jcxz"] = 196;
	opcodeTable["jecxz"] = 197;
	opcodeTable["je"] = 198;
	opcodeTable["jg"] = 199;
	opcodeTable["jge"] = 200;
	opcodeTable["jl"] = 201;
	opcodeTable["jle"] = 202;
	opcodeTable["jmp"] = 203;
	opcodeTable["jna"] = 204;
	opcodeTable["jnae"] = 205;
	opcodeTable["jnb"] = 206;
	opcodeTable["jnbe"] = 207;
	opcodeTable["jnc"] = 208;
	opcodeTable["jne"] = 209;
	opcodeTable["jng"] = 210;
	opcodeTable["jnge"] = 211;
	opcodeTable["jnl"] = 212;
	opcodeTable["jnle"] = 213;
	opcodeTable["jno"] = 214;
	opcodeTable["jnp"] = 215;
	opcodeTable["jns"] = 216;
	opcodeTable["jnz"] = 217;
	opcodeTable["jo"] = 218;
	opcodeTable["jp"] = 219;
	opcodeTable["jpe"] = 220;
	opcodeTable["jpo"] = 221;
	opcodeTable["js"] = 222;
	opcodeTable["jz"] = 223;
	opcodeTable["lahf"] = 224;
	opcodeTable["lar"] = 225;
	opcodeTable["lds"] = 226;
	opcodeTable["lea"] = 227;
	opcodeTable["leave"] = 228;
	opcodeTable["les"] = 229;
	opcodeTable["lfence"] = 230;
	opcodeTable["lfs"] = 231;
	opcodeTable["lgdt"] = 232;
	opcodeTable["lgs"] = 233;
	opcodeTable["lidt"] = 234;
	opcodeTable["lmsw"] = 235;
	opcodeTable["lods"] = 236;
	opcodeTable["lodsb"] = 237;
	opcodeTable["lodsw"] = 238;
	opcodeTable["lodsd"] = 239;
	opcodeTable["loop"] = 240;
	opcodeTable["loope"] = 241;
	opcodeTable["loopne"] = 242;
	opcodeTable["loopnz"] = 243;
	opcodeTable["loopz"] = 244;
	opcodeTable["lsl"] = 245;
	opcodeTable["lss"] = 246;
	opcodeTable["ltr"] = 247;
	opcodeTable["mfence"] = 248;
	opcodeTable["mov"] = 249;
	opcodeTable["movs"] = 250;
	opcodeTable["movsb"] = 251;
	opcodeTable["movsw"] = 252;
	opcodeTable["movsd"] = 253;
	opcodeTable["movsx"] = 254;
	opcodeTable["movzx"] = 255;
	opcodeTable["mul"] = 256;
	opcodeTable["neg"] = 257;
	opcodeTable["nop"] = 258;
	opcodeTable["not"] = 259;
	opcodeTable["or"] = 260;
	opcodeTable["out"] = 261;
	opcodeTable["outs"] = 262;
	opcodeTable["outsb"] = 263;
	opcodeTable["outsw"] = 264;
	opcodeTable["outsd"] = 265;
	opcodeTable["pop"] = 266;
	opcodeTable["popa"] = 267;
	opcodeTable["popaw"] = 268;
	opcodeTable["popad"] = 269;
	opcodeTable["popf"] = 270;
	opcodeTable["popfw"] = 271;
	opcodeTable["popfd"] = 272;
	opcodeTable["push"] = 273;
	opcodeTable["pusha"] = 274;
	opcodeTable["pushaw"] = 275;
	opcodeTable["pushad"] = 276;
	opcodeTable["pushf"] = 277;
	opcodeTable["pushfw"] = 278;
	opcodeTable["pushfd"] = 279;
	opcodeTable["rcl"] = 280;
	opcodeTable["rcr"] = 281;
	opcodeTable["rdmsr"] = 282;
	opcodeTable["rdpmc"] = 283;
	opcodeTable["rdtsc"] = 284;
	opcodeTable["ret"] = 285;
	opcodeTable["retf"] = 286;
	opcodeTable["rol"] = 287;
	opcodeTable["ror"] = 288;
	opcodeTable["rsm"] = 289;
	opcodeTable["sahf"] = 290;
	opcodeTable["sal"] = 291;
	opcodeTable["sar"] = 292;
	opcodeTable["sbb"] = 293;
	opcodeTable["scas"] = 294;
	opcodeTable["scasb"] = 295;
	opcodeTable["scasw"] = 296;
	opcodeTable["scasd"] = 297;
	opcodeTable["seta"] = 298;
	opcodeTable["setae"] = 299;
	opcodeTable["setb"] = 300;
	opcodeTable["setbe"] = 301;
	opcodeTable["setc"] = 302;
	opcodeTable["sete"] = 303;
	opcodeTable["setg"] = 304;
	opcodeTable["setge"] = 305;
	opcodeTable["setl"] = 306;
	opcodeTable["setle"] = 307;
	opcodeTable["setna"] = 308;
	opcodeTable["setnae"] = 309;
	opcodeTable["setnb"] = 310;
	opcodeTable["setnbe"] = 311;
	opcodeTable["setnc"] = 312;
	opcodeTable["setne"] = 313;
	opcodeTable["setng"] = 314;
	opcodeTable["setnge"] = 315;
	opcodeTable["setnl"] = 316;
	opcodeTable["setnle"] = 317;
	opcodeTable["setno"] = 318;
	opcodeTable["setnp"] = 319;
	opcodeTable["setns"] = 320;
	opcodeTable["setnz"] = 321;
	opcodeTable["seto"] = 322;
	opcodeTable["setp"] = 323;
	opcodeTable["setpe"] = 324;
	opcodeTable["setpo"] = 325;
	opcodeTable["sets"] = 326;
	opcodeTable["setz"] = 327;
	opcodeTable["sfence"] = 328;
	opcodeTable["shl"] = 329;
	opcodeTable["shld"] = 330;
	opcodeTable["shr"] = 331;
	opcodeTable["shrd"] = 332;
	opcodeTable["sidt"] = 333;
	opcodeTable["sldt"] = 334;
	opcodeTable["smsw"] = 335;
	opcodeTable["stc"] = 336;
	opcodeTable["std"] = 337;
	opcodeTable["sti"] = 338;
	opcodeTable["stos"] = 339;
	opcodeTable["stosb"] = 340;
	opcodeTable["stosw"] = 341;
	opcodeTable["stosd"] = 342;
	opcodeTable["str"] = 343;
	opcodeTable["sub"] = 344;
	opcodeTable["test"] = 345;
	opcodeTable["ud2"] = 346;
	opcodeTable["verr"] = 347;
	opcodeTable["verw"] = 348;
	opcodeTable["wait"] = 349;
	opcodeTable["wbinvd"] = 350;
	opcodeTable["wrmsr"] = 351;
	opcodeTable["xadd"] = 352;
	opcodeTable["xbts"] = 353;
	opcodeTable["xchg"] = 354;
	opcodeTable["xlat"] = 355;
	opcodeTable["xlatb"] = 356;
	opcodeTable["xor"] = 357;
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

void x64Parser::TokenFunc4(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 0;
}
x64Token x64Parser::tokenBranches3[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc4,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc9(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 1;
}
x64Token x64Parser::tokenBranches8[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc9,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc200(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 19;
}
x64Token x64Parser::tokenBranches199[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc200,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc346(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 34;
}
x64Token x64Parser::tokenBranches345[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc346,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc199(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc345(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches198[] = {
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc199, x64Parser::tokenBranches199 },
	{x64Token::NUMBER, 4, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc345, x64Parser::tokenBranches345 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc36(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 4;
}
x64Token x64Parser::tokenBranches35[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc36,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches198 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc137(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 13;
}
x64Token x64Parser::tokenBranches136[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc137,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc283(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 28;
}
x64Token x64Parser::tokenBranches282[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc283,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc429(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 43;
}
x64Token x64Parser::tokenBranches428[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc429,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc35(x64Operand &operand, int tokenPos)
{
	operand.values[2] = new Coding[2];
	CleanupValues.push_back(operand.values[2]);
	operand.values[2]->type = Coding::reg;
	operand.values[2]->val = inputTokens[tokenPos]->val->ival;
	operand.values[2]->bits = 0;
	operand.values[2]->field = 0;
	operand.values[2]->math = 0;
	operand.values[2]->mathval = 0;
	operand.values[2][1].type = Coding::eot;
}
void x64Parser::TokenFunc136(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc282(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc428(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches34[] = {
	{x64Token::REGISTERCLASS, 11, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc35, x64Parser::tokenBranches35 },
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc136, x64Parser::tokenBranches136 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc282, x64Parser::tokenBranches282 },
	{x64Token::NUMBER, 4, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc428, x64Parser::tokenBranches428 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc97(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 13;
}
x64Token x64Parser::tokenBranches33[] = {
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches34 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc97,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc227(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 22;
}
x64Token x64Parser::tokenBranches226[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc227,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc373(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 37;
}
x64Token x64Parser::tokenBranches372[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc373,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc226(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc372(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches225[] = {
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc226, x64Parser::tokenBranches226 },
	{x64Token::NUMBER, 4, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc372, x64Parser::tokenBranches372 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc57(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 7;
}
x64Token x64Parser::tokenBranches56[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc57,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches225 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc170(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 17;
}
x64Token x64Parser::tokenBranches169[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc170,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc316(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 32;
}
x64Token x64Parser::tokenBranches315[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc316,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc462(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 47;
}
x64Token x64Parser::tokenBranches461[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc462,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc56(x64Operand &operand, int tokenPos)
{
	operand.values[2] = new Coding[2];
	CleanupValues.push_back(operand.values[2]);
	operand.values[2]->type = Coding::reg;
	operand.values[2]->val = inputTokens[tokenPos]->val->ival;
	operand.values[2]->bits = 0;
	operand.values[2]->field = 0;
	operand.values[2]->math = 0;
	operand.values[2]->mathval = 0;
	operand.values[2][1].type = Coding::eot;
}
void x64Parser::TokenFunc169(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc315(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc461(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches55[] = {
	{x64Token::REGISTERCLASS, 11, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc56, x64Parser::tokenBranches56 },
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc169, x64Parser::tokenBranches169 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc315, x64Parser::tokenBranches315 },
	{x64Token::NUMBER, 4, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc461, x64Parser::tokenBranches461 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc120(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 17;
}
x64Token x64Parser::tokenBranches54[] = {
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches55 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc120,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc257(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 25;
}
x64Token x64Parser::tokenBranches256[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc257,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc403(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 40;
}
x64Token x64Parser::tokenBranches402[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc403,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc256(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc402(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches255[] = {
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc256, x64Parser::tokenBranches256 },
	{x64Token::NUMBER, 4, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc402, x64Parser::tokenBranches402 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc79(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 10;
}
x64Token x64Parser::tokenBranches78[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc79,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches255 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc584(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 53;
}
x64Token x64Parser::tokenBranches583[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc584,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc662(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 67;
}
x64Token x64Parser::tokenBranches661[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc662,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc740(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 77;
}
x64Token x64Parser::tokenBranches739[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc740,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1034(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 104;
}
x64Token x64Parser::tokenBranches1033[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1034,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1442(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 122;
}
x64Token x64Parser::tokenBranches1441[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1442,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1670(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 140;
}
x64Token x64Parser::tokenBranches1669[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1670,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1033(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1441(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1669(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches1032[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1033, x64Parser::tokenBranches1033 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1441, x64Parser::tokenBranches1441 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1669, x64Parser::tokenBranches1669 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1232(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 104;
}
x64Token x64Parser::tokenBranches1031[] = {
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches1032 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1232,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1031(x64Operand &operand, int tokenPos)
{
	operand.values[6] = new Coding[2];
	CleanupValues.push_back(operand.values[6]);
	operand.values[6]->type = Coding::number;
	operand.values[6]->val = operands.size();
	operand.values[6]->bits = 0;
	operand.values[6]->field = 0;
	operand.values[6]->math = 0;
	operand.values[6]->mathval = 0;
	operand.values[6][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches1030[] = {
	{x64Token::NUMBER, 15, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1031, x64Parser::tokenBranches1031 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1043(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 105;
}
x64Token x64Parser::tokenBranches1042[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1043,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1451(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 123;
}
x64Token x64Parser::tokenBranches1450[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1451,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1679(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 141;
}
x64Token x64Parser::tokenBranches1678[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1679,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1042(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1450(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1678(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches1041[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1042, x64Parser::tokenBranches1042 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1450, x64Parser::tokenBranches1450 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1678, x64Parser::tokenBranches1678 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1239(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 105;
}
x64Token x64Parser::tokenBranches1029[] = {
	{x64Token::TOKEN, 5, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches1030 },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches1041 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1239,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc583(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc661(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc739(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1029(x64Operand &operand, int tokenPos)
{
	operand.values[2] = new Coding[2];
	CleanupValues.push_back(operand.values[2]);
	operand.values[2]->type = Coding::reg;
	operand.values[2]->val = inputTokens[tokenPos]->val->ival;
	operand.values[2]->bits = 0;
	operand.values[2]->field = 0;
	operand.values[2]->math = 0;
	operand.values[2]->mathval = 0;
	operand.values[2][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches582[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc583, x64Parser::tokenBranches583 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc661, x64Parser::tokenBranches661 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc739, x64Parser::tokenBranches739 },
	{x64Token::REGISTERCLASS, 6, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1029, x64Parser::tokenBranches1029 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc498(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 53;
}
x64Token x64Parser::tokenBranches497[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc498,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches582 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc591(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 54;
}
x64Token x64Parser::tokenBranches590[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc591,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc669(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 68;
}
x64Token x64Parser::tokenBranches668[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc669,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc747(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 78;
}
x64Token x64Parser::tokenBranches746[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc747,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1014(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 102;
}
x64Token x64Parser::tokenBranches1013[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1014,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1422(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 120;
}
x64Token x64Parser::tokenBranches1421[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1422,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1650(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 138;
}
x64Token x64Parser::tokenBranches1649[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1650,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1013(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1421(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1649(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches1012[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1013, x64Parser::tokenBranches1013 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1421, x64Parser::tokenBranches1421 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1649, x64Parser::tokenBranches1649 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1216(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 102;
}
x64Token x64Parser::tokenBranches1011[] = {
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches1012 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1216,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1011(x64Operand &operand, int tokenPos)
{
	operand.values[6] = new Coding[2];
	CleanupValues.push_back(operand.values[6]);
	operand.values[6]->type = Coding::number;
	operand.values[6]->val = operands.size();
	operand.values[6]->bits = 0;
	operand.values[6]->field = 0;
	operand.values[6]->math = 0;
	operand.values[6]->mathval = 0;
	operand.values[6][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches1010[] = {
	{x64Token::NUMBER, 15, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1011, x64Parser::tokenBranches1011 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1023(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 103;
}
x64Token x64Parser::tokenBranches1022[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1023,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1431(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 121;
}
x64Token x64Parser::tokenBranches1430[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1431,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1659(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 139;
}
x64Token x64Parser::tokenBranches1658[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1659,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1022(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1430(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1658(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches1021[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1022, x64Parser::tokenBranches1022 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1430, x64Parser::tokenBranches1430 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1658, x64Parser::tokenBranches1658 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1223(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 103;
}
x64Token x64Parser::tokenBranches1009[] = {
	{x64Token::TOKEN, 5, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches1010 },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches1021 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1223,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc590(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc668(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc746(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1009(x64Operand &operand, int tokenPos)
{
	operand.values[2] = new Coding[2];
	CleanupValues.push_back(operand.values[2]);
	operand.values[2]->type = Coding::reg;
	operand.values[2]->val = inputTokens[tokenPos]->val->ival;
	operand.values[2]->bits = 0;
	operand.values[2]->field = 0;
	operand.values[2]->math = 0;
	operand.values[2]->mathval = 0;
	operand.values[2][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches589[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc590, x64Parser::tokenBranches590 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc668, x64Parser::tokenBranches668 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc746, x64Parser::tokenBranches746 },
	{x64Token::REGISTERCLASS, 6, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1009, x64Parser::tokenBranches1009 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc847(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 83;
}
x64Token x64Parser::tokenBranches846[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc847,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc846(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches845[] = {
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc846, x64Parser::tokenBranches846 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc838(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 82;
}
x64Token x64Parser::tokenBranches837[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc838,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches845 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc837(x64Operand &operand, int tokenPos)
{
	operand.values[6] = new Coding[2];
	CleanupValues.push_back(operand.values[6]);
	operand.values[6]->type = Coding::number;
	operand.values[6]->val = operands.size();
	operand.values[6]->bits = 0;
	operand.values[6]->field = 0;
	operand.values[6]->math = 0;
	operand.values[6]->mathval = 0;
	operand.values[6][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches836[] = {
	{x64Token::NUMBER, 15, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc837, x64Parser::tokenBranches837 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc503(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 54;
}
x64Token x64Parser::tokenBranches502[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc503,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches589 },
	{x64Token::TOKEN, 5, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches836 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc598(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 51;
}
x64Token x64Parser::tokenBranches597[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc598,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc676(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 64;
}
x64Token x64Parser::tokenBranches675[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc676,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc754(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 74;
}
x64Token x64Parser::tokenBranches753[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc754,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1054(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 98;
}
x64Token x64Parser::tokenBranches1053[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1054,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1462(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 116;
}
x64Token x64Parser::tokenBranches1461[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1462,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1690(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 134;
}
x64Token x64Parser::tokenBranches1689[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1690,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1053(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1461(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1689(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches1052[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1053, x64Parser::tokenBranches1053 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1461, x64Parser::tokenBranches1461 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1689, x64Parser::tokenBranches1689 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1248(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 98;
}
x64Token x64Parser::tokenBranches1051[] = {
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches1052 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1248,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1051(x64Operand &operand, int tokenPos)
{
	operand.values[6] = new Coding[2];
	CleanupValues.push_back(operand.values[6]);
	operand.values[6]->type = Coding::number;
	operand.values[6]->val = operands.size();
	operand.values[6]->bits = 0;
	operand.values[6]->field = 0;
	operand.values[6]->math = 0;
	operand.values[6]->mathval = 0;
	operand.values[6][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches1050[] = {
	{x64Token::NUMBER, 15, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1051, x64Parser::tokenBranches1051 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1063(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 99;
}
x64Token x64Parser::tokenBranches1062[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1063,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1471(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 117;
}
x64Token x64Parser::tokenBranches1470[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1471,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1699(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 135;
}
x64Token x64Parser::tokenBranches1698[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1699,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1062(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1470(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1698(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches1061[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1062, x64Parser::tokenBranches1062 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1470, x64Parser::tokenBranches1470 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1698, x64Parser::tokenBranches1698 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1255(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 99;
}
x64Token x64Parser::tokenBranches1049[] = {
	{x64Token::TOKEN, 5, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches1050 },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches1061 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1255,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc597(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc675(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc753(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1049(x64Operand &operand, int tokenPos)
{
	operand.values[2] = new Coding[2];
	CleanupValues.push_back(operand.values[2]);
	operand.values[2]->type = Coding::reg;
	operand.values[2]->val = inputTokens[tokenPos]->val->ival;
	operand.values[2]->bits = 0;
	operand.values[2]->field = 0;
	operand.values[2]->math = 0;
	operand.values[2]->mathval = 0;
	operand.values[2][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches596[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc597, x64Parser::tokenBranches597 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc675, x64Parser::tokenBranches675 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc753, x64Parser::tokenBranches753 },
	{x64Token::REGISTERCLASS, 6, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1049, x64Parser::tokenBranches1049 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc863(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 85;
}
x64Token x64Parser::tokenBranches862[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc863,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc862(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches861[] = {
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc862, x64Parser::tokenBranches862 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc854(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 84;
}
x64Token x64Parser::tokenBranches853[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc854,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches861 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc853(x64Operand &operand, int tokenPos)
{
	operand.values[6] = new Coding[2];
	CleanupValues.push_back(operand.values[6]);
	operand.values[6]->type = Coding::number;
	operand.values[6]->val = operands.size();
	operand.values[6]->bits = 0;
	operand.values[6]->field = 0;
	operand.values[6]->math = 0;
	operand.values[6]->mathval = 0;
	operand.values[6][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches852[] = {
	{x64Token::NUMBER, 15, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc853, x64Parser::tokenBranches853 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc508(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 51;
}
x64Token x64Parser::tokenBranches507[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc508,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches596 },
	{x64Token::TOKEN, 5, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches852 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc531(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 59;
}
x64Token x64Parser::tokenBranches530[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc531,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc8(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc78(x64Operand &operand, int tokenPos)
{
	operand.values[2] = new Coding[2];
	CleanupValues.push_back(operand.values[2]);
	operand.values[2]->type = Coding::reg;
	operand.values[2]->val = inputTokens[tokenPos]->val->ival;
	operand.values[2]->bits = 0;
	operand.values[2]->field = 0;
	operand.values[2]->math = 0;
	operand.values[2]->mathval = 0;
	operand.values[2][1].type = Coding::eot;
}
void x64Parser::TokenFunc507(x64Operand &operand, int tokenPos)
{
	operand.values[5] = new Coding[2];
	CleanupValues.push_back(operand.values[5]);
	operand.values[5]->type = Coding::reg;
	operand.values[5]->val = inputTokens[tokenPos]->val->ival;
	operand.values[5]->bits = 0;
	operand.values[5]->field = 0;
	operand.values[5]->math = 0;
	operand.values[5]->mathval = 0;
	operand.values[5][1].type = Coding::eot;
}
void x64Parser::TokenFunc530(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches7[] = {
	{x64Token::REGISTER, 14, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches33 },
	{x64Token::REGISTER, 18, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches54 },
	{x64Token::REGISTER, 17, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches497 },
	{x64Token::REGISTER, 19, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches502 },
	{x64Token::NUMBER, 6, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc8, x64Parser::tokenBranches8 },
	{x64Token::REGISTERCLASS, 11, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc78, x64Parser::tokenBranches78 },
	{x64Token::REGISTERCLASS, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc507, x64Parser::tokenBranches507 },
	{x64Token::NUMBER, 7, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc530, x64Parser::tokenBranches530 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches6[] = {
	{x64Token::TOKEN, 3, 0, 0, x64Parser::RegClassData1,  NULL, x64Parser::tokenBranches7 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc14(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 2;
}
x64Token x64Parser::tokenBranches13[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc14,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc209(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 21;
}
x64Token x64Parser::tokenBranches208[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc209,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc355(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 36;
}
x64Token x64Parser::tokenBranches354[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc355,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc208(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc354(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches207[] = {
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc208, x64Parser::tokenBranches208 },
	{x64Token::NUMBER, 4, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc354, x64Parser::tokenBranches354 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc43(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 6;
}
x64Token x64Parser::tokenBranches42[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc43,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches207 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc144(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 14;
}
x64Token x64Parser::tokenBranches143[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc144,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc290(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 29;
}
x64Token x64Parser::tokenBranches289[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc290,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc436(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 44;
}
x64Token x64Parser::tokenBranches435[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc436,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc42(x64Operand &operand, int tokenPos)
{
	operand.values[2] = new Coding[2];
	CleanupValues.push_back(operand.values[2]);
	operand.values[2]->type = Coding::reg;
	operand.values[2]->val = inputTokens[tokenPos]->val->ival;
	operand.values[2]->bits = 0;
	operand.values[2]->field = 0;
	operand.values[2]->math = 0;
	operand.values[2]->mathval = 0;
	operand.values[2][1].type = Coding::eot;
}
void x64Parser::TokenFunc143(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc289(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc435(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches41[] = {
	{x64Token::REGISTERCLASS, 11, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc42, x64Parser::tokenBranches42 },
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc143, x64Parser::tokenBranches143 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc289, x64Parser::tokenBranches289 },
	{x64Token::NUMBER, 4, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc435, x64Parser::tokenBranches435 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc102(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 14;
}
x64Token x64Parser::tokenBranches40[] = {
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches41 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc102,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc218(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 20;
}
x64Token x64Parser::tokenBranches217[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc218,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc364(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 35;
}
x64Token x64Parser::tokenBranches363[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc364,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc217(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc363(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches216[] = {
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc217, x64Parser::tokenBranches217 },
	{x64Token::NUMBER, 4, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc363, x64Parser::tokenBranches363 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc50(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 5;
}
x64Token x64Parser::tokenBranches49[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc50,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches216 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc163(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 16;
}
x64Token x64Parser::tokenBranches162[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc163,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc309(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 31;
}
x64Token x64Parser::tokenBranches308[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc309,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc455(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 46;
}
x64Token x64Parser::tokenBranches454[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc455,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc49(x64Operand &operand, int tokenPos)
{
	operand.values[2] = new Coding[2];
	CleanupValues.push_back(operand.values[2]);
	operand.values[2]->type = Coding::reg;
	operand.values[2]->val = inputTokens[tokenPos]->val->ival;
	operand.values[2]->bits = 0;
	operand.values[2]->field = 0;
	operand.values[2]->math = 0;
	operand.values[2]->mathval = 0;
	operand.values[2][1].type = Coding::eot;
}
void x64Parser::TokenFunc162(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc308(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc454(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches48[] = {
	{x64Token::REGISTERCLASS, 11, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc49, x64Parser::tokenBranches49 },
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc162, x64Parser::tokenBranches162 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc308, x64Parser::tokenBranches308 },
	{x64Token::NUMBER, 4, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc454, x64Parser::tokenBranches454 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc115(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 16;
}
x64Token x64Parser::tokenBranches47[] = {
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches48 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc115,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc264(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 26;
}
x64Token x64Parser::tokenBranches263[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc264,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc410(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 41;
}
x64Token x64Parser::tokenBranches409[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc410,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc263(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc409(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches262[] = {
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc263, x64Parser::tokenBranches263 },
	{x64Token::NUMBER, 4, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc409, x64Parser::tokenBranches409 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc84(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 11;
}
x64Token x64Parser::tokenBranches83[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc84,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches262 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc563(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 49;
}
x64Token x64Parser::tokenBranches562[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc563,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc641(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 62;
}
x64Token x64Parser::tokenBranches640[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc641,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc719(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 73;
}
x64Token x64Parser::tokenBranches718[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc719,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc974(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 96;
}
x64Token x64Parser::tokenBranches973[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc974,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1382(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 114;
}
x64Token x64Parser::tokenBranches1381[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1382,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1610(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 132;
}
x64Token x64Parser::tokenBranches1609[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1610,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc973(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1381(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1609(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches972[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc973, x64Parser::tokenBranches973 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1381, x64Parser::tokenBranches1381 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1609, x64Parser::tokenBranches1609 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1184(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 96;
}
x64Token x64Parser::tokenBranches971[] = {
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches972 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1184,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc971(x64Operand &operand, int tokenPos)
{
	operand.values[6] = new Coding[2];
	CleanupValues.push_back(operand.values[6]);
	operand.values[6]->type = Coding::number;
	operand.values[6]->val = operands.size();
	operand.values[6]->bits = 0;
	operand.values[6]->field = 0;
	operand.values[6]->math = 0;
	operand.values[6]->mathval = 0;
	operand.values[6][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches970[] = {
	{x64Token::NUMBER, 15, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc971, x64Parser::tokenBranches971 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc983(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 97;
}
x64Token x64Parser::tokenBranches982[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc983,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1391(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 115;
}
x64Token x64Parser::tokenBranches1390[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1391,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1619(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 133;
}
x64Token x64Parser::tokenBranches1618[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1619,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc982(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1390(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1618(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches981[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc982, x64Parser::tokenBranches982 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1390, x64Parser::tokenBranches1390 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1618, x64Parser::tokenBranches1618 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1191(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 97;
}
x64Token x64Parser::tokenBranches969[] = {
	{x64Token::TOKEN, 5, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches970 },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches981 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1191,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc562(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc640(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc718(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc969(x64Operand &operand, int tokenPos)
{
	operand.values[2] = new Coding[2];
	CleanupValues.push_back(operand.values[2]);
	operand.values[2]->type = Coding::reg;
	operand.values[2]->val = inputTokens[tokenPos]->val->ival;
	operand.values[2]->bits = 0;
	operand.values[2]->field = 0;
	operand.values[2]->math = 0;
	operand.values[2]->mathval = 0;
	operand.values[2][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches561[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc562, x64Parser::tokenBranches562 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc640, x64Parser::tokenBranches640 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc718, x64Parser::tokenBranches718 },
	{x64Token::REGISTERCLASS, 6, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc969, x64Parser::tokenBranches969 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc483(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 49;
}
x64Token x64Parser::tokenBranches482[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc483,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches561 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc570(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 50;
}
x64Token x64Parser::tokenBranches569[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc570,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc648(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 65;
}
x64Token x64Parser::tokenBranches647[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc648,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc726(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 75;
}
x64Token x64Parser::tokenBranches725[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc726,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc954(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 94;
}
x64Token x64Parser::tokenBranches953[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc954,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1362(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 112;
}
x64Token x64Parser::tokenBranches1361[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1362,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1590(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 130;
}
x64Token x64Parser::tokenBranches1589[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1590,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc953(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1361(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1589(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches952[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc953, x64Parser::tokenBranches953 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1361, x64Parser::tokenBranches1361 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1589, x64Parser::tokenBranches1589 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1168(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 94;
}
x64Token x64Parser::tokenBranches951[] = {
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches952 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1168,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc951(x64Operand &operand, int tokenPos)
{
	operand.values[6] = new Coding[2];
	CleanupValues.push_back(operand.values[6]);
	operand.values[6]->type = Coding::number;
	operand.values[6]->val = operands.size();
	operand.values[6]->bits = 0;
	operand.values[6]->field = 0;
	operand.values[6]->math = 0;
	operand.values[6]->mathval = 0;
	operand.values[6][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches950[] = {
	{x64Token::NUMBER, 15, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc951, x64Parser::tokenBranches951 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc963(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 95;
}
x64Token x64Parser::tokenBranches962[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc963,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1371(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 113;
}
x64Token x64Parser::tokenBranches1370[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1371,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1599(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 131;
}
x64Token x64Parser::tokenBranches1598[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1599,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc962(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1370(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1598(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches961[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc962, x64Parser::tokenBranches962 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1370, x64Parser::tokenBranches1370 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1598, x64Parser::tokenBranches1598 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1175(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 95;
}
x64Token x64Parser::tokenBranches949[] = {
	{x64Token::TOKEN, 5, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches950 },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches961 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1175,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc569(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc647(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc725(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc949(x64Operand &operand, int tokenPos)
{
	operand.values[2] = new Coding[2];
	CleanupValues.push_back(operand.values[2]);
	operand.values[2]->type = Coding::reg;
	operand.values[2]->val = inputTokens[tokenPos]->val->ival;
	operand.values[2]->bits = 0;
	operand.values[2]->field = 0;
	operand.values[2]->math = 0;
	operand.values[2]->mathval = 0;
	operand.values[2][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches568[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc569, x64Parser::tokenBranches569 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc647, x64Parser::tokenBranches647 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc725, x64Parser::tokenBranches725 },
	{x64Token::REGISTERCLASS, 6, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc949, x64Parser::tokenBranches949 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc815(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 87;
}
x64Token x64Parser::tokenBranches814[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc815,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc814(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches813[] = {
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc814, x64Parser::tokenBranches814 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc806(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 86;
}
x64Token x64Parser::tokenBranches805[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc806,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches813 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc805(x64Operand &operand, int tokenPos)
{
	operand.values[6] = new Coding[2];
	CleanupValues.push_back(operand.values[6]);
	operand.values[6]->type = Coding::number;
	operand.values[6]->val = operands.size();
	operand.values[6]->bits = 0;
	operand.values[6]->field = 0;
	operand.values[6]->math = 0;
	operand.values[6]->mathval = 0;
	operand.values[6][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches804[] = {
	{x64Token::NUMBER, 15, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc805, x64Parser::tokenBranches805 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc488(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 50;
}
x64Token x64Parser::tokenBranches487[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc488,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches568 },
	{x64Token::TOKEN, 5, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches804 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc577(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 52;
}
x64Token x64Parser::tokenBranches576[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc577,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc655(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 66;
}
x64Token x64Parser::tokenBranches654[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc655,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc733(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 76;
}
x64Token x64Parser::tokenBranches732[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc733,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc994(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 100;
}
x64Token x64Parser::tokenBranches993[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc994,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1402(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 118;
}
x64Token x64Parser::tokenBranches1401[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1402,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1630(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 136;
}
x64Token x64Parser::tokenBranches1629[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1630,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc993(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1401(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1629(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches992[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc993, x64Parser::tokenBranches993 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1401, x64Parser::tokenBranches1401 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1629, x64Parser::tokenBranches1629 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1200(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 100;
}
x64Token x64Parser::tokenBranches991[] = {
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches992 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1200,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc991(x64Operand &operand, int tokenPos)
{
	operand.values[6] = new Coding[2];
	CleanupValues.push_back(operand.values[6]);
	operand.values[6]->type = Coding::number;
	operand.values[6]->val = operands.size();
	operand.values[6]->bits = 0;
	operand.values[6]->field = 0;
	operand.values[6]->math = 0;
	operand.values[6]->mathval = 0;
	operand.values[6][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches990[] = {
	{x64Token::NUMBER, 15, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc991, x64Parser::tokenBranches991 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1003(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 101;
}
x64Token x64Parser::tokenBranches1002[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1003,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1411(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 119;
}
x64Token x64Parser::tokenBranches1410[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1411,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1639(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 137;
}
x64Token x64Parser::tokenBranches1638[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1639,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1002(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1410(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1638(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches1001[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1002, x64Parser::tokenBranches1002 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1410, x64Parser::tokenBranches1410 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1638, x64Parser::tokenBranches1638 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1207(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 101;
}
x64Token x64Parser::tokenBranches989[] = {
	{x64Token::TOKEN, 5, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches990 },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches1001 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1207,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc576(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc654(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc732(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc989(x64Operand &operand, int tokenPos)
{
	operand.values[2] = new Coding[2];
	CleanupValues.push_back(operand.values[2]);
	operand.values[2]->type = Coding::reg;
	operand.values[2]->val = inputTokens[tokenPos]->val->ival;
	operand.values[2]->bits = 0;
	operand.values[2]->field = 0;
	operand.values[2]->math = 0;
	operand.values[2]->mathval = 0;
	operand.values[2][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches575[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc576, x64Parser::tokenBranches576 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc654, x64Parser::tokenBranches654 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc732, x64Parser::tokenBranches732 },
	{x64Token::REGISTERCLASS, 6, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc989, x64Parser::tokenBranches989 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc831(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 89;
}
x64Token x64Parser::tokenBranches830[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc831,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc830(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches829[] = {
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc830, x64Parser::tokenBranches830 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc822(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 88;
}
x64Token x64Parser::tokenBranches821[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc822,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches829 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc821(x64Operand &operand, int tokenPos)
{
	operand.values[6] = new Coding[2];
	CleanupValues.push_back(operand.values[6]);
	operand.values[6]->type = Coding::number;
	operand.values[6]->val = operands.size();
	operand.values[6]->bits = 0;
	operand.values[6]->field = 0;
	operand.values[6]->math = 0;
	operand.values[6]->mathval = 0;
	operand.values[6][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches820[] = {
	{x64Token::NUMBER, 15, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc821, x64Parser::tokenBranches821 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc493(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 52;
}
x64Token x64Parser::tokenBranches492[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc493,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches575 },
	{x64Token::TOKEN, 5, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches820 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc536(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 60;
}
x64Token x64Parser::tokenBranches535[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc536,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc13(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc83(x64Operand &operand, int tokenPos)
{
	operand.values[2] = new Coding[2];
	CleanupValues.push_back(operand.values[2]);
	operand.values[2]->type = Coding::reg;
	operand.values[2]->val = inputTokens[tokenPos]->val->ival;
	operand.values[2]->bits = 0;
	operand.values[2]->field = 0;
	operand.values[2]->math = 0;
	operand.values[2]->mathval = 0;
	operand.values[2][1].type = Coding::eot;
}
void x64Parser::TokenFunc492(x64Operand &operand, int tokenPos)
{
	operand.values[5] = new Coding[2];
	CleanupValues.push_back(operand.values[5]);
	operand.values[5]->type = Coding::reg;
	operand.values[5]->val = inputTokens[tokenPos]->val->ival;
	operand.values[5]->bits = 0;
	operand.values[5]->field = 0;
	operand.values[5]->math = 0;
	operand.values[5]->mathval = 0;
	operand.values[5][1].type = Coding::eot;
}
void x64Parser::TokenFunc535(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches12[] = {
	{x64Token::REGISTER, 14, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches40 },
	{x64Token::REGISTER, 18, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches47 },
	{x64Token::REGISTER, 17, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches482 },
	{x64Token::REGISTER, 19, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches487 },
	{x64Token::NUMBER, 6, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc13, x64Parser::tokenBranches13 },
	{x64Token::REGISTERCLASS, 11, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc83, x64Parser::tokenBranches83 },
	{x64Token::REGISTERCLASS, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc492, x64Parser::tokenBranches492 },
	{x64Token::NUMBER, 7, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc535, x64Parser::tokenBranches535 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches11[] = {
	{x64Token::TOKEN, 3, 0, 0, x64Parser::RegClassData1,  NULL, x64Parser::tokenBranches12 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc19(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 3;
}
x64Token x64Parser::tokenBranches18[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc19,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc236(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 23;
}
x64Token x64Parser::tokenBranches235[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc236,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc382(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 38;
}
x64Token x64Parser::tokenBranches381[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc382,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc235(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc381(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches234[] = {
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc235, x64Parser::tokenBranches235 },
	{x64Token::NUMBER, 4, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc381, x64Parser::tokenBranches381 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc64(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 8;
}
x64Token x64Parser::tokenBranches63[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc64,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches234 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc151(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 15;
}
x64Token x64Parser::tokenBranches150[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc151,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc297(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 30;
}
x64Token x64Parser::tokenBranches296[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc297,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc443(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 45;
}
x64Token x64Parser::tokenBranches442[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc443,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc63(x64Operand &operand, int tokenPos)
{
	operand.values[2] = new Coding[2];
	CleanupValues.push_back(operand.values[2]);
	operand.values[2]->type = Coding::reg;
	operand.values[2]->val = inputTokens[tokenPos]->val->ival;
	operand.values[2]->bits = 0;
	operand.values[2]->field = 0;
	operand.values[2]->math = 0;
	operand.values[2]->mathval = 0;
	operand.values[2][1].type = Coding::eot;
}
void x64Parser::TokenFunc150(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc296(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc442(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches62[] = {
	{x64Token::REGISTERCLASS, 11, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc63, x64Parser::tokenBranches63 },
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc150, x64Parser::tokenBranches150 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc296, x64Parser::tokenBranches296 },
	{x64Token::NUMBER, 4, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc442, x64Parser::tokenBranches442 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc107(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 15;
}
x64Token x64Parser::tokenBranches61[] = {
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches62 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc107,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc245(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 24;
}
x64Token x64Parser::tokenBranches244[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc245,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc391(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 39;
}
x64Token x64Parser::tokenBranches390[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc391,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc244(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc390(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches243[] = {
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc244, x64Parser::tokenBranches244 },
	{x64Token::NUMBER, 4, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc390, x64Parser::tokenBranches390 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc71(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 9;
}
x64Token x64Parser::tokenBranches70[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc71,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches243 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc177(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 18;
}
x64Token x64Parser::tokenBranches176[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc177,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc323(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 33;
}
x64Token x64Parser::tokenBranches322[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc323,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc469(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 48;
}
x64Token x64Parser::tokenBranches468[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc469,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc70(x64Operand &operand, int tokenPos)
{
	operand.values[2] = new Coding[2];
	CleanupValues.push_back(operand.values[2]);
	operand.values[2]->type = Coding::reg;
	operand.values[2]->val = inputTokens[tokenPos]->val->ival;
	operand.values[2]->bits = 0;
	operand.values[2]->field = 0;
	operand.values[2]->math = 0;
	operand.values[2]->mathval = 0;
	operand.values[2][1].type = Coding::eot;
}
void x64Parser::TokenFunc176(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc322(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc468(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches69[] = {
	{x64Token::REGISTERCLASS, 11, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc70, x64Parser::tokenBranches70 },
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc176, x64Parser::tokenBranches176 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc322, x64Parser::tokenBranches322 },
	{x64Token::NUMBER, 4, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc468, x64Parser::tokenBranches468 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc125(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 18;
}
x64Token x64Parser::tokenBranches68[] = {
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches69 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc125,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc271(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 27;
}
x64Token x64Parser::tokenBranches270[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc271,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc417(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 42;
}
x64Token x64Parser::tokenBranches416[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc417,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc270(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc416(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches269[] = {
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc270, x64Parser::tokenBranches270 },
	{x64Token::NUMBER, 4, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc416, x64Parser::tokenBranches416 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc89(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 12;
}
x64Token x64Parser::tokenBranches88[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc89,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches269 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc605(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 55;
}
x64Token x64Parser::tokenBranches604[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc605,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc683(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 69;
}
x64Token x64Parser::tokenBranches682[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc683,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc768(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 80;
}
x64Token x64Parser::tokenBranches767[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc768,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1094(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 108;
}
x64Token x64Parser::tokenBranches1093[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1094,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1502(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 126;
}
x64Token x64Parser::tokenBranches1501[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1502,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1730(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 144;
}
x64Token x64Parser::tokenBranches1729[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1730,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1093(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1501(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1729(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches1092[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1093, x64Parser::tokenBranches1093 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1501, x64Parser::tokenBranches1501 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1729, x64Parser::tokenBranches1729 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1280(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 108;
}
x64Token x64Parser::tokenBranches1091[] = {
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches1092 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1280,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1091(x64Operand &operand, int tokenPos)
{
	operand.values[6] = new Coding[2];
	CleanupValues.push_back(operand.values[6]);
	operand.values[6]->type = Coding::number;
	operand.values[6]->val = operands.size();
	operand.values[6]->bits = 0;
	operand.values[6]->field = 0;
	operand.values[6]->math = 0;
	operand.values[6]->mathval = 0;
	operand.values[6][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches1090[] = {
	{x64Token::NUMBER, 15, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1091, x64Parser::tokenBranches1091 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1103(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 109;
}
x64Token x64Parser::tokenBranches1102[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1103,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1511(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 127;
}
x64Token x64Parser::tokenBranches1510[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1511,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1739(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 145;
}
x64Token x64Parser::tokenBranches1738[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1739,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1102(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1510(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1738(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches1101[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1102, x64Parser::tokenBranches1102 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1510, x64Parser::tokenBranches1510 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1738, x64Parser::tokenBranches1738 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1287(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 109;
}
x64Token x64Parser::tokenBranches1089[] = {
	{x64Token::TOKEN, 5, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches1090 },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches1101 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1287,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc604(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc682(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc767(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1089(x64Operand &operand, int tokenPos)
{
	operand.values[2] = new Coding[2];
	CleanupValues.push_back(operand.values[2]);
	operand.values[2]->type = Coding::reg;
	operand.values[2]->val = inputTokens[tokenPos]->val->ival;
	operand.values[2]->bits = 0;
	operand.values[2]->field = 0;
	operand.values[2]->math = 0;
	operand.values[2]->mathval = 0;
	operand.values[2][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches603[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc604, x64Parser::tokenBranches604 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc682, x64Parser::tokenBranches682 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc767, x64Parser::tokenBranches767 },
	{x64Token::REGISTERCLASS, 6, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1089, x64Parser::tokenBranches1089 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc513(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 55;
}
x64Token x64Parser::tokenBranches512[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc513,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches603 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc612(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 56;
}
x64Token x64Parser::tokenBranches611[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc612,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc690(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 70;
}
x64Token x64Parser::tokenBranches689[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc690,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc761(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 79;
}
x64Token x64Parser::tokenBranches760[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc761,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1074(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 106;
}
x64Token x64Parser::tokenBranches1073[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1074,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1482(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 124;
}
x64Token x64Parser::tokenBranches1481[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1482,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1710(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 142;
}
x64Token x64Parser::tokenBranches1709[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1710,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1073(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1481(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1709(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches1072[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1073, x64Parser::tokenBranches1073 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1481, x64Parser::tokenBranches1481 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1709, x64Parser::tokenBranches1709 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1264(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 106;
}
x64Token x64Parser::tokenBranches1071[] = {
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches1072 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1264,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1071(x64Operand &operand, int tokenPos)
{
	operand.values[6] = new Coding[2];
	CleanupValues.push_back(operand.values[6]);
	operand.values[6]->type = Coding::number;
	operand.values[6]->val = operands.size();
	operand.values[6]->bits = 0;
	operand.values[6]->field = 0;
	operand.values[6]->math = 0;
	operand.values[6]->mathval = 0;
	operand.values[6][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches1070[] = {
	{x64Token::NUMBER, 15, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1071, x64Parser::tokenBranches1071 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1083(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 107;
}
x64Token x64Parser::tokenBranches1082[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1083,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1491(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 125;
}
x64Token x64Parser::tokenBranches1490[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1491,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1719(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 143;
}
x64Token x64Parser::tokenBranches1718[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1719,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1082(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1490(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1718(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches1081[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1082, x64Parser::tokenBranches1082 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1490, x64Parser::tokenBranches1490 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1718, x64Parser::tokenBranches1718 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1271(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 107;
}
x64Token x64Parser::tokenBranches1069[] = {
	{x64Token::TOKEN, 5, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches1070 },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches1081 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1271,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc611(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc689(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc760(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1069(x64Operand &operand, int tokenPos)
{
	operand.values[2] = new Coding[2];
	CleanupValues.push_back(operand.values[2]);
	operand.values[2]->type = Coding::reg;
	operand.values[2]->val = inputTokens[tokenPos]->val->ival;
	operand.values[2]->bits = 0;
	operand.values[2]->field = 0;
	operand.values[2]->math = 0;
	operand.values[2]->mathval = 0;
	operand.values[2][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches610[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc611, x64Parser::tokenBranches611 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc689, x64Parser::tokenBranches689 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc760, x64Parser::tokenBranches760 },
	{x64Token::REGISTERCLASS, 6, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1069, x64Parser::tokenBranches1069 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc879(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 91;
}
x64Token x64Parser::tokenBranches878[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc879,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc878(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches877[] = {
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc878, x64Parser::tokenBranches878 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc870(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 90;
}
x64Token x64Parser::tokenBranches869[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc870,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches877 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc869(x64Operand &operand, int tokenPos)
{
	operand.values[6] = new Coding[2];
	CleanupValues.push_back(operand.values[6]);
	operand.values[6]->type = Coding::number;
	operand.values[6]->val = operands.size();
	operand.values[6]->bits = 0;
	operand.values[6]->field = 0;
	operand.values[6]->math = 0;
	operand.values[6]->mathval = 0;
	operand.values[6][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches868[] = {
	{x64Token::NUMBER, 15, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc869, x64Parser::tokenBranches869 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc518(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 56;
}
x64Token x64Parser::tokenBranches517[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc518,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches610 },
	{x64Token::TOKEN, 5, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches868 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc619(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 57;
}
x64Token x64Parser::tokenBranches618[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc619,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc697(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 71;
}
x64Token x64Parser::tokenBranches696[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc697,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc775(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 81;
}
x64Token x64Parser::tokenBranches774[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc775,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1114(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 110;
}
x64Token x64Parser::tokenBranches1113[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1114,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1522(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 128;
}
x64Token x64Parser::tokenBranches1521[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1522,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1750(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 146;
}
x64Token x64Parser::tokenBranches1749[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1750,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1113(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1521(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1749(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches1112[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1113, x64Parser::tokenBranches1113 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1521, x64Parser::tokenBranches1521 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1749, x64Parser::tokenBranches1749 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1296(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 110;
}
x64Token x64Parser::tokenBranches1111[] = {
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches1112 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1296,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1111(x64Operand &operand, int tokenPos)
{
	operand.values[6] = new Coding[2];
	CleanupValues.push_back(operand.values[6]);
	operand.values[6]->type = Coding::number;
	operand.values[6]->val = operands.size();
	operand.values[6]->bits = 0;
	operand.values[6]->field = 0;
	operand.values[6]->math = 0;
	operand.values[6]->mathval = 0;
	operand.values[6][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches1110[] = {
	{x64Token::NUMBER, 15, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1111, x64Parser::tokenBranches1111 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1123(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 111;
}
x64Token x64Parser::tokenBranches1122[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1123,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1531(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 129;
}
x64Token x64Parser::tokenBranches1530[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1531,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1759(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 147;
}
x64Token x64Parser::tokenBranches1758[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1759,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1122(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1530(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1758(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches1121[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1122, x64Parser::tokenBranches1122 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1530, x64Parser::tokenBranches1530 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1758, x64Parser::tokenBranches1758 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1303(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 111;
}
x64Token x64Parser::tokenBranches1109[] = {
	{x64Token::TOKEN, 5, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches1110 },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches1121 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1303,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc618(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc696(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc774(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1109(x64Operand &operand, int tokenPos)
{
	operand.values[2] = new Coding[2];
	CleanupValues.push_back(operand.values[2]);
	operand.values[2]->type = Coding::reg;
	operand.values[2]->val = inputTokens[tokenPos]->val->ival;
	operand.values[2]->bits = 0;
	operand.values[2]->field = 0;
	operand.values[2]->math = 0;
	operand.values[2]->mathval = 0;
	operand.values[2][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches617[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc618, x64Parser::tokenBranches618 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc696, x64Parser::tokenBranches696 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc774, x64Parser::tokenBranches774 },
	{x64Token::REGISTERCLASS, 6, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1109, x64Parser::tokenBranches1109 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc895(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 93;
}
x64Token x64Parser::tokenBranches894[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc895,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc894(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches893[] = {
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc894, x64Parser::tokenBranches894 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc886(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 92;
}
x64Token x64Parser::tokenBranches885[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc886,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches893 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc885(x64Operand &operand, int tokenPos)
{
	operand.values[6] = new Coding[2];
	CleanupValues.push_back(operand.values[6]);
	operand.values[6]->type = Coding::number;
	operand.values[6]->val = operands.size();
	operand.values[6]->bits = 0;
	operand.values[6]->field = 0;
	operand.values[6]->math = 0;
	operand.values[6]->mathval = 0;
	operand.values[6][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches884[] = {
	{x64Token::NUMBER, 15, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc885, x64Parser::tokenBranches885 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc523(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 57;
}
x64Token x64Parser::tokenBranches522[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc523,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches617 },
	{x64Token::TOKEN, 5, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches884 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc541(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 61;
}
x64Token x64Parser::tokenBranches540[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc541,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc18(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc88(x64Operand &operand, int tokenPos)
{
	operand.values[2] = new Coding[2];
	CleanupValues.push_back(operand.values[2]);
	operand.values[2]->type = Coding::reg;
	operand.values[2]->val = inputTokens[tokenPos]->val->ival;
	operand.values[2]->bits = 0;
	operand.values[2]->field = 0;
	operand.values[2]->math = 0;
	operand.values[2]->mathval = 0;
	operand.values[2][1].type = Coding::eot;
}
void x64Parser::TokenFunc522(x64Operand &operand, int tokenPos)
{
	operand.values[5] = new Coding[2];
	CleanupValues.push_back(operand.values[5]);
	operand.values[5]->type = Coding::reg;
	operand.values[5]->val = inputTokens[tokenPos]->val->ival;
	operand.values[5]->bits = 0;
	operand.values[5]->field = 0;
	operand.values[5]->math = 0;
	operand.values[5]->mathval = 0;
	operand.values[5][1].type = Coding::eot;
}
void x64Parser::TokenFunc540(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches17[] = {
	{x64Token::REGISTER, 14, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches61 },
	{x64Token::REGISTER, 18, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches68 },
	{x64Token::REGISTER, 17, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches512 },
	{x64Token::REGISTER, 19, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches517 },
	{x64Token::NUMBER, 6, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc18, x64Parser::tokenBranches18 },
	{x64Token::REGISTERCLASS, 11, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc88, x64Parser::tokenBranches88 },
	{x64Token::REGISTERCLASS, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc522, x64Parser::tokenBranches522 },
	{x64Token::NUMBER, 7, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc540, x64Parser::tokenBranches540 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches16[] = {
	{x64Token::TOKEN, 3, 0, 0, x64Parser::RegClassData1,  NULL, x64Parser::tokenBranches17 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc184(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 19;
}
x64Token x64Parser::tokenBranches183[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc184,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc330(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 34;
}
x64Token x64Parser::tokenBranches329[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc330,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc183(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc329(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches182[] = {
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc183, x64Parser::tokenBranches183 },
	{x64Token::NUMBER, 4, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc329, x64Parser::tokenBranches329 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc24(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 4;
}
x64Token x64Parser::tokenBranches23[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc24,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches182 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc130(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 13;
}
x64Token x64Parser::tokenBranches129[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc130,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc276(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 28;
}
x64Token x64Parser::tokenBranches275[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc276,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc422(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 43;
}
x64Token x64Parser::tokenBranches421[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc422,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc23(x64Operand &operand, int tokenPos)
{
	operand.values[2] = new Coding[2];
	CleanupValues.push_back(operand.values[2]);
	operand.values[2]->type = Coding::reg;
	operand.values[2]->val = inputTokens[tokenPos]->val->ival;
	operand.values[2]->bits = 0;
	operand.values[2]->field = 0;
	operand.values[2]->math = 0;
	operand.values[2]->mathval = 0;
	operand.values[2][1].type = Coding::eot;
}
void x64Parser::TokenFunc129(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc275(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc421(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches22[] = {
	{x64Token::REGISTERCLASS, 11, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc23, x64Parser::tokenBranches23 },
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc129, x64Parser::tokenBranches129 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc275, x64Parser::tokenBranches275 },
	{x64Token::NUMBER, 4, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc421, x64Parser::tokenBranches421 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc92(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 13;
}
x64Token x64Parser::tokenBranches21[] = {
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches22 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc92,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc191(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 20;
}
x64Token x64Parser::tokenBranches190[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc191,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc337(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 35;
}
x64Token x64Parser::tokenBranches336[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc337,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc190(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc336(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches189[] = {
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc190, x64Parser::tokenBranches190 },
	{x64Token::NUMBER, 4, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc336, x64Parser::tokenBranches336 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc29(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 5;
}
x64Token x64Parser::tokenBranches28[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc29,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches189 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc156(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 16;
}
x64Token x64Parser::tokenBranches155[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc156,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc302(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 31;
}
x64Token x64Parser::tokenBranches301[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc302,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc448(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 46;
}
x64Token x64Parser::tokenBranches447[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc448,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc28(x64Operand &operand, int tokenPos)
{
	operand.values[2] = new Coding[2];
	CleanupValues.push_back(operand.values[2]);
	operand.values[2]->type = Coding::reg;
	operand.values[2]->val = inputTokens[tokenPos]->val->ival;
	operand.values[2]->bits = 0;
	operand.values[2]->field = 0;
	operand.values[2]->math = 0;
	operand.values[2]->mathval = 0;
	operand.values[2][1].type = Coding::eot;
}
void x64Parser::TokenFunc155(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc301(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc447(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches27[] = {
	{x64Token::REGISTERCLASS, 11, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc28, x64Parser::tokenBranches28 },
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc155, x64Parser::tokenBranches155 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc301, x64Parser::tokenBranches301 },
	{x64Token::NUMBER, 4, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc447, x64Parser::tokenBranches447 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc110(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 16;
}
x64Token x64Parser::tokenBranches26[] = {
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches27 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc110,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc250(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 25;
}
x64Token x64Parser::tokenBranches249[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc250,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc396(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 40;
}
x64Token x64Parser::tokenBranches395[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc396,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc249(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc395(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches248[] = {
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc249, x64Parser::tokenBranches249 },
	{x64Token::NUMBER, 4, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc395, x64Parser::tokenBranches395 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc74(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 10;
}
x64Token x64Parser::tokenBranches73[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc74,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches248 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc546(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 49;
}
x64Token x64Parser::tokenBranches545[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc546,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc624(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 62;
}
x64Token x64Parser::tokenBranches623[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc624,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc707(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 73;
}
x64Token x64Parser::tokenBranches706[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc707,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc920(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 96;
}
x64Token x64Parser::tokenBranches919[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc920,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1328(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 114;
}
x64Token x64Parser::tokenBranches1327[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1328,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1556(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 132;
}
x64Token x64Parser::tokenBranches1555[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1556,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc919(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1327(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1555(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches918[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc919, x64Parser::tokenBranches919 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1327, x64Parser::tokenBranches1327 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1555, x64Parser::tokenBranches1555 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1142(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 96;
}
x64Token x64Parser::tokenBranches917[] = {
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches918 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1142,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc917(x64Operand &operand, int tokenPos)
{
	operand.values[6] = new Coding[2];
	CleanupValues.push_back(operand.values[6]);
	operand.values[6]->type = Coding::number;
	operand.values[6]->val = operands.size();
	operand.values[6]->bits = 0;
	operand.values[6]->field = 0;
	operand.values[6]->math = 0;
	operand.values[6]->mathval = 0;
	operand.values[6][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches916[] = {
	{x64Token::NUMBER, 15, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc917, x64Parser::tokenBranches917 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc927(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 97;
}
x64Token x64Parser::tokenBranches926[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc927,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1335(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 115;
}
x64Token x64Parser::tokenBranches1334[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1335,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1563(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 133;
}
x64Token x64Parser::tokenBranches1562[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1563,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc926(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1334(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1562(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches925[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc926, x64Parser::tokenBranches926 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1334, x64Parser::tokenBranches1334 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1562, x64Parser::tokenBranches1562 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1147(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 97;
}
x64Token x64Parser::tokenBranches915[] = {
	{x64Token::TOKEN, 5, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches916 },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches925 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1147,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc545(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc623(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc706(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc915(x64Operand &operand, int tokenPos)
{
	operand.values[2] = new Coding[2];
	CleanupValues.push_back(operand.values[2]);
	operand.values[2]->type = Coding::reg;
	operand.values[2]->val = inputTokens[tokenPos]->val->ival;
	operand.values[2]->bits = 0;
	operand.values[2]->field = 0;
	operand.values[2]->math = 0;
	operand.values[2]->mathval = 0;
	operand.values[2][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches544[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc545, x64Parser::tokenBranches545 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc623, x64Parser::tokenBranches623 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc706, x64Parser::tokenBranches706 },
	{x64Token::REGISTERCLASS, 6, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc915, x64Parser::tokenBranches915 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc472(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 49;
}
x64Token x64Parser::tokenBranches471[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc472,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches544 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc551(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 50;
}
x64Token x64Parser::tokenBranches550[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc551,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc629(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 63;
}
x64Token x64Parser::tokenBranches628[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc629,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc702(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 72;
}
x64Token x64Parser::tokenBranches701[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc702,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc904(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 94;
}
x64Token x64Parser::tokenBranches903[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc904,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1312(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 112;
}
x64Token x64Parser::tokenBranches1311[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1312,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1540(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 130;
}
x64Token x64Parser::tokenBranches1539[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1540,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc903(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1311(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1539(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches902[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc903, x64Parser::tokenBranches903 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1311, x64Parser::tokenBranches1311 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1539, x64Parser::tokenBranches1539 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1130(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 94;
}
x64Token x64Parser::tokenBranches901[] = {
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches902 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1130,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc901(x64Operand &operand, int tokenPos)
{
	operand.values[6] = new Coding[2];
	CleanupValues.push_back(operand.values[6]);
	operand.values[6]->type = Coding::number;
	operand.values[6]->val = operands.size();
	operand.values[6]->bits = 0;
	operand.values[6]->field = 0;
	operand.values[6]->math = 0;
	operand.values[6]->mathval = 0;
	operand.values[6][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches900[] = {
	{x64Token::NUMBER, 15, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc901, x64Parser::tokenBranches901 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc911(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 95;
}
x64Token x64Parser::tokenBranches910[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc911,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1319(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 113;
}
x64Token x64Parser::tokenBranches1318[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1319,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1547(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 131;
}
x64Token x64Parser::tokenBranches1546[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1547,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc910(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1318(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1546(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches909[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc910, x64Parser::tokenBranches910 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1318, x64Parser::tokenBranches1318 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1546, x64Parser::tokenBranches1546 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1135(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 95;
}
x64Token x64Parser::tokenBranches899[] = {
	{x64Token::TOKEN, 5, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches900 },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches909 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1135,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc550(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc628(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc701(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc899(x64Operand &operand, int tokenPos)
{
	operand.values[2] = new Coding[2];
	CleanupValues.push_back(operand.values[2]);
	operand.values[2]->type = Coding::reg;
	operand.values[2]->val = inputTokens[tokenPos]->val->ival;
	operand.values[2]->bits = 0;
	operand.values[2]->field = 0;
	operand.values[2]->math = 0;
	operand.values[2]->mathval = 0;
	operand.values[2][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches549[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc550, x64Parser::tokenBranches550 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc628, x64Parser::tokenBranches628 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc701, x64Parser::tokenBranches701 },
	{x64Token::REGISTERCLASS, 6, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc899, x64Parser::tokenBranches899 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc787(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 83;
}
x64Token x64Parser::tokenBranches786[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc787,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc786(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches785[] = {
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc786, x64Parser::tokenBranches786 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc780(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 82;
}
x64Token x64Parser::tokenBranches779[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc780,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches785 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc779(x64Operand &operand, int tokenPos)
{
	operand.values[6] = new Coding[2];
	CleanupValues.push_back(operand.values[6]);
	operand.values[6]->type = Coding::number;
	operand.values[6]->val = operands.size();
	operand.values[6]->bits = 0;
	operand.values[6]->field = 0;
	operand.values[6]->math = 0;
	operand.values[6]->mathval = 0;
	operand.values[6][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches778[] = {
	{x64Token::NUMBER, 15, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc779, x64Parser::tokenBranches779 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc475(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 50;
}
x64Token x64Parser::tokenBranches474[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc475,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches549 },
	{x64Token::TOKEN, 5, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches778 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc556(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 51;
}
x64Token x64Parser::tokenBranches555[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc556,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc634(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 64;
}
x64Token x64Parser::tokenBranches633[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc634,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc712(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 74;
}
x64Token x64Parser::tokenBranches711[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc712,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc936(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 98;
}
x64Token x64Parser::tokenBranches935[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc936,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1344(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 116;
}
x64Token x64Parser::tokenBranches1343[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1344,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1572(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 134;
}
x64Token x64Parser::tokenBranches1571[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1572,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc935(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1343(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1571(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches934[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc935, x64Parser::tokenBranches935 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1343, x64Parser::tokenBranches1343 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1571, x64Parser::tokenBranches1571 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1154(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 98;
}
x64Token x64Parser::tokenBranches933[] = {
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches934 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1154,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc933(x64Operand &operand, int tokenPos)
{
	operand.values[6] = new Coding[2];
	CleanupValues.push_back(operand.values[6]);
	operand.values[6]->type = Coding::number;
	operand.values[6]->val = operands.size();
	operand.values[6]->bits = 0;
	operand.values[6]->field = 0;
	operand.values[6]->math = 0;
	operand.values[6]->mathval = 0;
	operand.values[6][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches932[] = {
	{x64Token::NUMBER, 15, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc933, x64Parser::tokenBranches933 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc943(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 99;
}
x64Token x64Parser::tokenBranches942[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc943,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1351(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 117;
}
x64Token x64Parser::tokenBranches1350[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1351,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1579(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 135;
}
x64Token x64Parser::tokenBranches1578[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1579,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc942(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1350(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1578(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches941[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc942, x64Parser::tokenBranches942 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1350, x64Parser::tokenBranches1350 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1578, x64Parser::tokenBranches1578 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1159(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 99;
}
x64Token x64Parser::tokenBranches931[] = {
	{x64Token::TOKEN, 5, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches932 },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches941 },
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc1159,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc555(x64Operand &operand, int tokenPos)
{
	operand.values[3] = new Coding[2];
	CleanupValues.push_back(operand.values[3]);
	operand.values[3]->type = Coding::number;
	operand.values[3]->val = operands.size();
	operand.values[3]->bits = 0;
	operand.values[3]->field = 0;
	operand.values[3]->math = 0;
	operand.values[3]->mathval = 0;
	operand.values[3][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc633(x64Operand &operand, int tokenPos)
{
	operand.values[4] = new Coding[2];
	CleanupValues.push_back(operand.values[4]);
	operand.values[4]->type = Coding::number;
	operand.values[4]->val = operands.size();
	operand.values[4]->bits = 0;
	operand.values[4]->field = 0;
	operand.values[4]->math = 0;
	operand.values[4]->mathval = 0;
	operand.values[4][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc711(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc931(x64Operand &operand, int tokenPos)
{
	operand.values[2] = new Coding[2];
	CleanupValues.push_back(operand.values[2]);
	operand.values[2]->type = Coding::reg;
	operand.values[2]->val = inputTokens[tokenPos]->val->ival;
	operand.values[2]->bits = 0;
	operand.values[2]->field = 0;
	operand.values[2]->math = 0;
	operand.values[2]->mathval = 0;
	operand.values[2][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches554[] = {
	{x64Token::NUMBER, 16, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc555, x64Parser::tokenBranches555 },
	{x64Token::NUMBER, 0, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc633, x64Parser::tokenBranches633 },
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc711, x64Parser::tokenBranches711 },
	{x64Token::REGISTERCLASS, 6, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc931, x64Parser::tokenBranches931 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc799(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 85;
}
x64Token x64Parser::tokenBranches798[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc799,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc798(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches797[] = {
	{x64Token::NUMBER, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc798, x64Parser::tokenBranches798 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc792(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 84;
}
x64Token x64Parser::tokenBranches791[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc792,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches797 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc791(x64Operand &operand, int tokenPos)
{
	operand.values[6] = new Coding[2];
	CleanupValues.push_back(operand.values[6]);
	operand.values[6]->type = Coding::number;
	operand.values[6]->val = operands.size();
	operand.values[6]->bits = 0;
	operand.values[6]->field = 0;
	operand.values[6]->math = 0;
	operand.values[6]->mathval = 0;
	operand.values[6][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches790[] = {
	{x64Token::NUMBER, 15, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc791, x64Parser::tokenBranches791 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc478(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 51;
}
x64Token x64Parser::tokenBranches477[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc478,  },
	{x64Token::TOKEN, 4, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches554 },
	{x64Token::TOKEN, 5, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches790 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc526(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 58;
}
x64Token x64Parser::tokenBranches525[] = {
	{x64Token::TOKEN, 2, 1, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc526,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc16(x64Operand &operand, int tokenPos)
{
	operand.values[1] = new Coding[2];
	CleanupValues.push_back(operand.values[1]);
	operand.values[1]->type = Coding::reg;
	operand.values[1]->val = inputTokens[tokenPos]->val->ival;
	operand.values[1]->bits = 0;
	operand.values[1]->field = 0;
	operand.values[1]->math = 0;
	operand.values[1]->mathval = 0;
	operand.values[1][1].type = Coding::eot;
}
void x64Parser::TokenFunc73(x64Operand &operand, int tokenPos)
{
	operand.values[2] = new Coding[2];
	CleanupValues.push_back(operand.values[2]);
	operand.values[2]->type = Coding::reg;
	operand.values[2]->val = inputTokens[tokenPos]->val->ival;
	operand.values[2]->bits = 0;
	operand.values[2]->field = 0;
	operand.values[2]->math = 0;
	operand.values[2]->mathval = 0;
	operand.values[2][1].type = Coding::eot;
}
void x64Parser::TokenFunc477(x64Operand &operand, int tokenPos)
{
	operand.values[5] = new Coding[2];
	CleanupValues.push_back(operand.values[5]);
	operand.values[5]->type = Coding::reg;
	operand.values[5]->val = inputTokens[tokenPos]->val->ival;
	operand.values[5]->bits = 0;
	operand.values[5]->field = 0;
	operand.values[5]->math = 0;
	operand.values[5]->mathval = 0;
	operand.values[5][1].type = Coding::eot;
}
void x64Parser::TokenFunc525(x64Operand &operand, int tokenPos)
{
	operand.values[0] = new Coding[2];
	CleanupValues.push_back(operand.values[0]);
	operand.values[0]->type = Coding::number;
	operand.values[0]->val = operands.size();
	operand.values[0]->bits = 0;
	operand.values[0]->field = 0;
	operand.values[0]->math = 0;
	operand.values[0]->mathval = 0;
	operand.values[0][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2[] = {
	{x64Token::REGISTER, 27, 0, 0, x64Parser::RegClassData1,  NULL, x64Parser::tokenBranches6 },
	{x64Token::REGISTER, 26, 0, 0, x64Parser::RegClassData1,  NULL, x64Parser::tokenBranches11 },
	{x64Token::REGISTER, 14, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches21 },
	{x64Token::REGISTER, 18, 0, 0, x64Parser::RegClassData2,  NULL, x64Parser::tokenBranches26 },
	{x64Token::REGISTER, 17, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches471 },
	{x64Token::REGISTER, 19, 0, 0, x64Parser::RegClassData3,  NULL, x64Parser::tokenBranches474 },
	{x64Token::NUMBER, 6, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc3, x64Parser::tokenBranches3 },
	{x64Token::REGISTERCLASS, 12, 0, 0, x64Parser::RegClassData1, &x64Parser::TokenFunc16, x64Parser::tokenBranches16 },
	{x64Token::REGISTERCLASS, 11, 0, 0, x64Parser::RegClassData2, &x64Parser::TokenFunc73, x64Parser::tokenBranches73 },
	{x64Token::REGISTERCLASS, 5, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc477, x64Parser::tokenBranches477 },
	{x64Token::NUMBER, 7, 0, 0, x64Parser::RegClassData3, &x64Parser::TokenFunc525, x64Parser::tokenBranches525 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1760(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 148;
	operand.values[7] = new Coding[2];
	CleanupValues.push_back(operand.values[7]);
	operand.values[7]->type = Coding::reg;
	operand.values[7]->val = inputTokens[tokenPos]->val->ival;
	operand.values[7]->bits = 0;
	operand.values[7]->field = 0;
	operand.values[7]->math = 0;
	operand.values[7]->mathval = 0;
	operand.values[7][1].type = Coding::eot;
}
void x64Parser::TokenFunc1761(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 148;
	operand.values[7] = new Coding[2];
	CleanupValues.push_back(operand.values[7]);
	operand.values[7]->type = Coding::reg;
	operand.values[7]->val = inputTokens[tokenPos]->val->ival;
	operand.values[7]->bits = 0;
	operand.values[7]->field = 0;
	operand.values[7]->math = 0;
	operand.values[7]->mathval = 0;
	operand.values[7][1].type = Coding::eot;
}
void x64Parser::TokenFunc1762(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 148;
	operand.values[7] = new Coding[2];
	CleanupValues.push_back(operand.values[7]);
	operand.values[7]->type = Coding::reg;
	operand.values[7]->val = inputTokens[tokenPos]->val->ival;
	operand.values[7]->bits = 0;
	operand.values[7]->field = 0;
	operand.values[7]->math = 0;
	operand.values[7]->mathval = 0;
	operand.values[7][1].type = Coding::eot;
}
void x64Parser::TokenFunc1763(x64Operand &operand, int tokenPos)
{
	operand.addressCoding = 148;
	operand.values[7] = new Coding[2];
	CleanupValues.push_back(operand.values[7]);
	operand.values[7]->type = Coding::reg;
	operand.values[7]->val = inputTokens[tokenPos]->val->ival;
	operand.values[7]->bits = 0;
	operand.values[7]->field = 0;
	operand.values[7]->math = 0;
	operand.values[7]->mathval = 0;
	operand.values[7][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches1[] = {
	{x64Token::TOKEN, 1, 0, 0, x64Parser::RegClassData1,  NULL, x64Parser::tokenBranches2 },
	{x64Token::REGISTERCLASS, 1, 1, 0, x64Parser::RegClassData4, &x64Parser::TokenFunc1760,  },
	{x64Token::REGISTERCLASS, 4, 1, 0, x64Parser::RegClassData5, &x64Parser::TokenFunc1761,  },
	{x64Token::REGISTERCLASS, 7, 1, 0, x64Parser::RegClassData6, &x64Parser::TokenFunc1762,  },
	{x64Token::REGISTERCLASS, 13, 1, 0, x64Parser::RegClassData7, &x64Parser::TokenFunc1763,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1765(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 149;
}
x64Token x64Parser::tokenBranches1764[] = {
	{x64Token::EMPTY, 0, 1, 0, NULL,&x64Parser::TokenFunc1765,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1767(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 150;
}
x64Token x64Parser::tokenBranches1766[] = {
	{x64Token::EMPTY, 0, 1, 0, NULL,&x64Parser::TokenFunc1767,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1772_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding1772_9[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::indirect, 23, -1, 0 },
	{ Coding::eot },
};
void x64Parser::TokenFunc1772(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1772_8;
	operand.values[9] = tokenCoding1772_9;
}
x64Token x64Parser::tokenBranches1771[] = {
	{x64Token::ADDRESSCLASS, 4, 1, 0, NULL,&x64Parser::TokenFunc1772,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1770[] = {
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches1771 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1769[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1770 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1776_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding1776_9[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::indirect, 23, -1, 0 },
	{ Coding::eot },
};
void x64Parser::TokenFunc1776(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1776_8;
	operand.values[9] = tokenCoding1776_9;
}
x64Token x64Parser::tokenBranches1775[] = {
	{x64Token::ADDRESSCLASS, 5, 1, 0, NULL,&x64Parser::TokenFunc1776,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1774[] = {
	{x64Token::TOKEN, 9, 0, 1, NULL, NULL, x64Parser::tokenBranches1775 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1773[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1774 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1769(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
void x64Parser::TokenFunc1773(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches1768[] = {
	{x64Token::REGISTERCLASS, 4, 0, 0, NULL,&x64Parser::TokenFunc1769, x64Parser::tokenBranches1769 },
	{x64Token::REGISTERCLASS, 7, 0, 0, NULL,&x64Parser::TokenFunc1773, x64Parser::tokenBranches1773 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1781_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding1781_9[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::indirect, 23, -1, 0 },
	{ Coding::eot },
};
void x64Parser::TokenFunc1781(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1781_8;
	operand.values[9] = tokenCoding1781_9;
}
x64Token x64Parser::tokenBranches1780[] = {
	{x64Token::ADDRESSCLASS, 1, 1, 0, NULL,&x64Parser::TokenFunc1781,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1779[] = {
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches1780 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1778[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1779 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1785_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding1785_9[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::indirect, 23, -1, 0 },
	{ Coding::eot },
};
void x64Parser::TokenFunc1785(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1785_8;
	operand.values[9] = tokenCoding1785_9;
}
x64Token x64Parser::tokenBranches1784[] = {
	{x64Token::ADDRESSCLASS, 1, 1, 0, NULL,&x64Parser::TokenFunc1785,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1783[] = {
	{x64Token::TOKEN, 9, 0, 1, NULL, NULL, x64Parser::tokenBranches1784 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1782[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1783 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1778(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
void x64Parser::TokenFunc1782(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches1777[] = {
	{x64Token::REGISTERCLASS, 4, 0, 0, NULL,&x64Parser::TokenFunc1778, x64Parser::tokenBranches1778 },
	{x64Token::REGISTERCLASS, 7, 0, 0, NULL,&x64Parser::TokenFunc1782, x64Parser::tokenBranches1782 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1790(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 151;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches1789[] = {
	{x64Token::NUMBER, 3, 1, 0, NULL,&x64Parser::TokenFunc1790,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1788[] = {
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches1789 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1787[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1788 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1794(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 152;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches1793[] = {
	{x64Token::NUMBER, 4, 1, 0, NULL,&x64Parser::TokenFunc1794,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1792[] = {
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches1793 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1791[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1792 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1798(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 153;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches1797[] = {
	{x64Token::NUMBER, 5, 1, 0, NULL,&x64Parser::TokenFunc1798,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1796[] = {
	{x64Token::TOKEN, 9, 0, 1, NULL, NULL, x64Parser::tokenBranches1797 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1795[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1796 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1802_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding1802_9[] = {
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 24, 8, 0, 43, 2 },
	{ Coding::eot },
};
void x64Parser::TokenFunc1802(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1802_8;
	operand.values[9] = tokenCoding1802_9;
}
x64Token x64Parser::tokenBranches1801[] = {
	{x64Token::ADDRESSCLASS, 3, 1, 0, NULL,&x64Parser::TokenFunc1802,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1800[] = {
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches1801 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1799[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1800 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1806_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding1806_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 24, 8, 0, 43, 3 },
	{ Coding::eot },
};
void x64Parser::TokenFunc1806(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1806_8;
	operand.values[9] = tokenCoding1806_9;
}
x64Token x64Parser::tokenBranches1805[] = {
	{x64Token::ADDRESSCLASS, 4, 1, 0, NULL,&x64Parser::TokenFunc1806,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1804[] = {
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches1805 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1803[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1804 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1810_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding1810_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 24, 8, 0, 43, 3 },
	{ Coding::eot },
};
void x64Parser::TokenFunc1810(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1810_8;
	operand.values[9] = tokenCoding1810_9;
}
x64Token x64Parser::tokenBranches1809[] = {
	{x64Token::ADDRESSCLASS, 5, 1, 0, NULL,&x64Parser::TokenFunc1810,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1808[] = {
	{x64Token::TOKEN, 9, 0, 1, NULL, NULL, x64Parser::tokenBranches1809 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1807[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1808 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1814_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding1814_9[] = {
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 24, 8, 0 },
	{ Coding::eot },
};
void x64Parser::TokenFunc1814(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
	operand.values[8] = tokenCoding1814_8;
	operand.values[9] = tokenCoding1814_9;
}
x64Token x64Parser::tokenBranches1813[] = {
	{x64Token::REGISTERCLASS, 1, 1, 0, NULL,&x64Parser::TokenFunc1814,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1812[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1813 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1811[] = {
	{x64Token::ADDRESSCLASS, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches1812 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1818_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding1818_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 24, 8, 0, 43, 1 },
	{ Coding::eot },
};
void x64Parser::TokenFunc1818(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
	operand.values[8] = tokenCoding1818_8;
	operand.values[9] = tokenCoding1818_9;
}
x64Token x64Parser::tokenBranches1817[] = {
	{x64Token::REGISTERCLASS, 4, 1, 0, NULL,&x64Parser::TokenFunc1818,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1816[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1817 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1815[] = {
	{x64Token::ADDRESSCLASS, 4, 0, 0, NULL, NULL, x64Parser::tokenBranches1816 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1822_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding1822_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 24, 8, 0, 43, 1 },
	{ Coding::eot },
};
void x64Parser::TokenFunc1822(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
	operand.values[8] = tokenCoding1822_8;
	operand.values[9] = tokenCoding1822_9;
}
x64Token x64Parser::tokenBranches1821[] = {
	{x64Token::REGISTERCLASS, 7, 1, 0, NULL,&x64Parser::TokenFunc1822,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1820[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1821 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1819[] = {
	{x64Token::ADDRESSCLASS, 5, 0, 0, NULL, NULL, x64Parser::tokenBranches1820 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1827_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 128, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1827(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 154;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[9] = tokenCoding1827_9;
}
x64Token x64Parser::tokenBranches1826[] = {
	{x64Token::NUMBER, 3, 1, 0, NULL,&x64Parser::TokenFunc1827,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1825[] = {
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches1826 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1824[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1825 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1823[] = {
	{x64Token::ADDRESSCLASS, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches1824 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1832_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 131, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1832(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 155;
	operand.values[13] = new Coding[2];
	CleanupValues.push_back(operand.values[13]);
	operand.values[13]->type = Coding::number;
	operand.values[13]->val = operands.size();
	operand.values[13]->bits = 0;
	operand.values[13]->field = 0;
	operand.values[13]->math = 0;
	operand.values[13]->mathval = 0;
	operand.values[13][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[9] = tokenCoding1832_9;
}
x64Token x64Parser::tokenBranches1831[] = {
	{x64Token::NUMBER, 1, 1, 0, NULL,&x64Parser::TokenFunc1832,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1837_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 129, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1837(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 156;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[9] = tokenCoding1837_9;
}
x64Token x64Parser::tokenBranches1836[] = {
	{x64Token::NUMBER, 4, 1, 0, NULL,&x64Parser::TokenFunc1837,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1830[] = {
	{x64Token::TOKEN, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches1831 },
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches1836 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1829[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1830 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1828[] = {
	{x64Token::ADDRESSCLASS, 4, 0, 0, NULL, NULL, x64Parser::tokenBranches1829 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1842_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 131, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1842(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 157;
	operand.values[13] = new Coding[2];
	CleanupValues.push_back(operand.values[13]);
	operand.values[13]->type = Coding::number;
	operand.values[13]->val = operands.size();
	operand.values[13]->bits = 0;
	operand.values[13]->field = 0;
	operand.values[13]->math = 0;
	operand.values[13]->mathval = 0;
	operand.values[13][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[9] = tokenCoding1842_9;
}
x64Token x64Parser::tokenBranches1841[] = {
	{x64Token::NUMBER, 1, 1, 0, NULL,&x64Parser::TokenFunc1842,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1847_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 129, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1847(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 158;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[9] = tokenCoding1847_9;
}
x64Token x64Parser::tokenBranches1846[] = {
	{x64Token::NUMBER, 5, 1, 0, NULL,&x64Parser::TokenFunc1847,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1840[] = {
	{x64Token::TOKEN, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches1841 },
	{x64Token::TOKEN, 9, 0, 1, NULL, NULL, x64Parser::tokenBranches1846 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1839[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1840 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1838[] = {
	{x64Token::ADDRESSCLASS, 5, 0, 0, NULL, NULL, x64Parser::tokenBranches1839 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1851_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 128, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1851(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 154;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[9] = tokenCoding1851_9;
}
x64Token x64Parser::tokenBranches1850[] = {
	{x64Token::NUMBER, 3, 1, 0, NULL,&x64Parser::TokenFunc1851,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1849[] = {
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches1850 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1848[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1849 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1855_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 131, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1855(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 155;
	operand.values[13] = new Coding[2];
	CleanupValues.push_back(operand.values[13]);
	operand.values[13]->type = Coding::number;
	operand.values[13]->val = operands.size();
	operand.values[13]->bits = 0;
	operand.values[13]->field = 0;
	operand.values[13]->math = 0;
	operand.values[13]->mathval = 0;
	operand.values[13][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[9] = tokenCoding1855_9;
}
x64Token x64Parser::tokenBranches1854[] = {
	{x64Token::NUMBER, 1, 1, 0, NULL,&x64Parser::TokenFunc1855,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1859_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 129, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1859(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 156;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[9] = tokenCoding1859_9;
}
x64Token x64Parser::tokenBranches1858[] = {
	{x64Token::NUMBER, 4, 1, 0, NULL,&x64Parser::TokenFunc1859,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1853[] = {
	{x64Token::TOKEN, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches1854 },
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches1858 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1852[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1853 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1863_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 131, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1863(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 157;
	operand.values[13] = new Coding[2];
	CleanupValues.push_back(operand.values[13]);
	operand.values[13]->type = Coding::number;
	operand.values[13]->val = operands.size();
	operand.values[13]->bits = 0;
	operand.values[13]->field = 0;
	operand.values[13]->math = 0;
	operand.values[13]->mathval = 0;
	operand.values[13][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[9] = tokenCoding1863_9;
}
x64Token x64Parser::tokenBranches1862[] = {
	{x64Token::NUMBER, 1, 1, 0, NULL,&x64Parser::TokenFunc1863,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1867_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 129, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1867(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 158;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[9] = tokenCoding1867_9;
}
x64Token x64Parser::tokenBranches1866[] = {
	{x64Token::NUMBER, 5, 1, 0, NULL,&x64Parser::TokenFunc1867,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1861[] = {
	{x64Token::TOKEN, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches1862 },
	{x64Token::TOKEN, 9, 0, 1, NULL, NULL, x64Parser::tokenBranches1866 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1860[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1861 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1799(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
void x64Parser::TokenFunc1803(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
void x64Parser::TokenFunc1807(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches1786[] = {
	{x64Token::REGISTER, 0, 0, 0, NULL, NULL, x64Parser::tokenBranches1787 },
	{x64Token::REGISTER, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches1791 },
	{x64Token::REGISTER, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches1795 },
	{x64Token::REGISTERCLASS, 1, 0, 0, NULL,&x64Parser::TokenFunc1799, x64Parser::tokenBranches1799 },
	{x64Token::REGISTERCLASS, 4, 0, 0, NULL,&x64Parser::TokenFunc1803, x64Parser::tokenBranches1803 },
	{x64Token::REGISTERCLASS, 7, 0, 0, NULL,&x64Parser::TokenFunc1807, x64Parser::tokenBranches1807 },
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches1811 },
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches1815 },
	{x64Token::TOKEN, 9, 0, 1, NULL, NULL, x64Parser::tokenBranches1819 },
	{x64Token::TOKEN, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches1823 },
	{x64Token::TOKEN, 8, 0, 0, NULL, NULL, x64Parser::tokenBranches1828 },
	{x64Token::TOKEN, 9, 0, 0, NULL, NULL, x64Parser::tokenBranches1838 },
	{x64Token::ADDRESSCLASS, 8, 0, 0, NULL, NULL, x64Parser::tokenBranches1848 },
	{x64Token::ADDRESSCLASS, 9, 0, 0, NULL, NULL, x64Parser::tokenBranches1852 },
	{x64Token::ADDRESSCLASS, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches1860 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1873_9[] = {
	{ Coding::indirect, 23, -1, 0 },
	{ Coding::eot },
};
void x64Parser::TokenFunc1873(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 159;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[9] = tokenCoding1873_9;
}
x64Token x64Parser::tokenBranches1872[] = {
	{x64Token::NUMBER, 3, 1, 0, NULL,&x64Parser::TokenFunc1873,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1871[] = {
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches1872 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1870[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1871 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1869[] = {
	{x64Token::ADDRESSCLASS, 4, 0, 0, NULL, NULL, x64Parser::tokenBranches1870 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1878_9[] = {
	{ Coding::indirect, 23, -1, 0 },
	{ Coding::eot },
};
void x64Parser::TokenFunc1878(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 160;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[9] = tokenCoding1878_9;
}
x64Token x64Parser::tokenBranches1877[] = {
	{x64Token::NUMBER, 3, 1, 0, NULL,&x64Parser::TokenFunc1878,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1876[] = {
	{x64Token::TOKEN, 9, 0, 1, NULL, NULL, x64Parser::tokenBranches1877 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1875[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1876 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1874[] = {
	{x64Token::ADDRESSCLASS, 5, 0, 0, NULL, NULL, x64Parser::tokenBranches1875 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1868[] = {
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches1869 },
	{x64Token::TOKEN, 9, 0, 1, NULL, NULL, x64Parser::tokenBranches1874 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1884_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 254, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1884(x64Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1884_9;
}
x64Token x64Parser::tokenBranches1883[] = {
	{x64Token::ADDRESSCLASS, 3, 1, 0, NULL,&x64Parser::TokenFunc1884,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1886_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1886(x64Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1886_9;
}
x64Token x64Parser::tokenBranches1885[] = {
	{x64Token::ADDRESSCLASS, 4, 1, 0, NULL,&x64Parser::TokenFunc1886,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1888_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1888(x64Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1888_9;
}
x64Token x64Parser::tokenBranches1887[] = {
	{x64Token::ADDRESSCLASS, 5, 1, 0, NULL,&x64Parser::TokenFunc1888,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1880_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 254, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1880(x64Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1880_9;
}
void x64Parser::TokenFunc1881(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 161;
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
void x64Parser::TokenFunc1882(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 162;
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches1879[] = {
	{x64Token::ADDRESSCLASS, 8, 1, 0, NULL,&x64Parser::TokenFunc1880,  },
	{x64Token::REGISTERCLASS, 4, 1, 0, NULL,&x64Parser::TokenFunc1881,  },
	{x64Token::REGISTERCLASS, 7, 1, 0, NULL,&x64Parser::TokenFunc1882,  },
	{x64Token::TOKEN, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches1883 },
	{x64Token::TOKEN, 8, 0, 0, NULL, NULL, x64Parser::tokenBranches1885 },
	{x64Token::TOKEN, 9, 0, 0, NULL, NULL, x64Parser::tokenBranches1887 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1894_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 246, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1894(x64Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1894_9;
}
x64Token x64Parser::tokenBranches1893[] = {
	{x64Token::ADDRESSCLASS, 3, 1, 0, NULL,&x64Parser::TokenFunc1894,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1896_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 247, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1896(x64Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1896_9;
}
x64Token x64Parser::tokenBranches1895[] = {
	{x64Token::ADDRESSCLASS, 4, 1, 0, NULL,&x64Parser::TokenFunc1896,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1898_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 247, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1898(x64Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1898_9;
}
x64Token x64Parser::tokenBranches1897[] = {
	{x64Token::ADDRESSCLASS, 5, 1, 0, NULL,&x64Parser::TokenFunc1898,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1890_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 246, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1890(x64Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1890_9;
}
Coding x64Parser::tokenCoding1891_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 247, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1891(x64Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1891_9;
}
Coding x64Parser::tokenCoding1892_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 247, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1892(x64Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1892_9;
}
x64Token x64Parser::tokenBranches1889[] = {
	{x64Token::ADDRESSCLASS, 8, 1, 0, NULL,&x64Parser::TokenFunc1890,  },
	{x64Token::ADDRESSCLASS, 9, 1, 0, NULL,&x64Parser::TokenFunc1891,  },
	{x64Token::ADDRESSCLASS, 10, 1, 0, NULL,&x64Parser::TokenFunc1892,  },
	{x64Token::TOKEN, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches1893 },
	{x64Token::TOKEN, 8, 0, 0, NULL, NULL, x64Parser::tokenBranches1895 },
	{x64Token::TOKEN, 9, 0, 0, NULL, NULL, x64Parser::tokenBranches1897 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1902(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 163;
	operand.values[7] = new Coding[2];
	CleanupValues.push_back(operand.values[7]);
	operand.values[7]->type = Coding::reg;
	operand.values[7]->val = inputTokens[tokenPos]->val->ival;
	operand.values[7]->bits = 0;
	operand.values[7]->field = 0;
	operand.values[7]->math = 0;
	operand.values[7]->mathval = 0;
	operand.values[7][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches1901[] = {
	{x64Token::REGISTERCLASS, 13, 1, 0, NULL,&x64Parser::TokenFunc1902,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1900[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1901 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1905(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 164;
}
x64Token x64Parser::tokenBranches1904[] = {
	{x64Token::REGISTER, 30, 1, 0, NULL,&x64Parser::TokenFunc1905,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1903[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1904 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1908_9[] = {
	{ Coding::indirect, 23, -1, 0 },
	{ Coding::eot },
};
void x64Parser::TokenFunc1908(x64Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1908_9;
}
x64Token x64Parser::tokenBranches1907[] = {
	{x64Token::ADDRESSCLASS, 2, 1, 0, NULL,&x64Parser::TokenFunc1908,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1910_9[] = {
	{ Coding::indirect, 23, -1, 0, 43, 4 },
	{ Coding::eot },
};
void x64Parser::TokenFunc1910(x64Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1910_9;
}
x64Token x64Parser::tokenBranches1909[] = {
	{x64Token::ADDRESSCLASS, 2, 1, 0, NULL,&x64Parser::TokenFunc1910,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1903(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 165;
	operand.values[7] = new Coding[2];
	CleanupValues.push_back(operand.values[7]);
	operand.values[7]->type = Coding::reg;
	operand.values[7]->val = inputTokens[tokenPos]->val->ival;
	operand.values[7]->bits = 0;
	operand.values[7]->field = 0;
	operand.values[7]->math = 0;
	operand.values[7]->mathval = 0;
	operand.values[7][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches1899[] = {
	{x64Token::REGISTER, 30, 0, 0, NULL, NULL, x64Parser::tokenBranches1900 },
	{x64Token::REGISTERCLASS, 13, 1, 0, NULL,&x64Parser::TokenFunc1903, x64Parser::tokenBranches1903 },
	{x64Token::TOKEN, 9, 0, 0, NULL, NULL, x64Parser::tokenBranches1907 },
	{x64Token::TOKEN, 11, 0, 0, NULL, NULL, x64Parser::tokenBranches1909 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1914(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 148;
}
x64Token x64Parser::tokenBranches1913[] = {
	{x64Token::REGISTER, 30, 1, 0, NULL,&x64Parser::TokenFunc1914,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1912[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1913 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1912(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 148;
	operand.values[7] = new Coding[2];
	CleanupValues.push_back(operand.values[7]);
	operand.values[7]->type = Coding::reg;
	operand.values[7]->val = inputTokens[tokenPos]->val->ival;
	operand.values[7]->bits = 0;
	operand.values[7]->field = 0;
	operand.values[7]->math = 0;
	operand.values[7]->mathval = 0;
	operand.values[7][1].type = Coding::eot;
}
void x64Parser::TokenFunc1916(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 166;
}
x64Token x64Parser::tokenBranches1911[] = {
	{x64Token::REGISTERCLASS, 13, 1, 0, NULL,&x64Parser::TokenFunc1912, x64Parser::tokenBranches1912 },
	{x64Token::EMPTY, 0, 1, 0, NULL,&x64Parser::TokenFunc1916,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1919_9[] = {
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0, 43, 4 },
	{ Coding::eot },
};
void x64Parser::TokenFunc1919(x64Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1919_9;
}
x64Token x64Parser::tokenBranches1918[] = {
	{x64Token::ADDRESSCLASS, 1, 1, 0, NULL,&x64Parser::TokenFunc1919,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1921_9[] = {
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0 },
	{ Coding::eot },
};
void x64Parser::TokenFunc1921(x64Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1921_9;
}
x64Token x64Parser::tokenBranches1920[] = {
	{x64Token::ADDRESSCLASS, 1, 1, 0, NULL,&x64Parser::TokenFunc1921,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1917[] = {
	{x64Token::TOKEN, 8, 0, 0, NULL, NULL, x64Parser::tokenBranches1918 },
	{x64Token::TOKEN, 9, 0, 0, NULL, NULL, x64Parser::tokenBranches1920 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1923[] = {
	{x64Token::ADDRESSCLASS, 0, 1, 0, NULL, NULL,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1922[] = {
	{x64Token::TOKEN, 12, 0, 1, NULL, NULL, x64Parser::tokenBranches1923 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1927(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 167;
	operand.values[14] = new Coding[2];
	CleanupValues.push_back(operand.values[14]);
	operand.values[14]->type = Coding::number;
	operand.values[14]->val = operands.size();
	operand.values[14]->bits = 0;
	operand.values[14]->field = 0;
	operand.values[14]->math = 0;
	operand.values[14]->mathval = 0;
	operand.values[14][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches1926[] = {
	{x64Token::NUMBER, 8, 1, 0, NULL,&x64Parser::TokenFunc1927,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1929(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 168;
	operand.values[14] = new Coding[2];
	CleanupValues.push_back(operand.values[14]);
	operand.values[14]->type = Coding::number;
	operand.values[14]->val = operands.size();
	operand.values[14]->bits = 0;
	operand.values[14]->field = 0;
	operand.values[14]->math = 0;
	operand.values[14]->mathval = 0;
	operand.values[14][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc1931(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 169;
	operand.values[14] = new Coding[2];
	CleanupValues.push_back(operand.values[14]);
	operand.values[14]->type = Coding::number;
	operand.values[14]->val = operands.size();
	operand.values[14]->bits = 0;
	operand.values[14]->field = 0;
	operand.values[14]->math = 0;
	operand.values[14]->mathval = 0;
	operand.values[14][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches1928[] = {
	{x64Token::NUMBER, 11, 1, 0, NULL,&x64Parser::TokenFunc1929,  },
	{x64Token::NUMBER, 14, 1, 0, NULL,&x64Parser::TokenFunc1931,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1932(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 168;
	operand.values[14] = new Coding[2];
	CleanupValues.push_back(operand.values[14]);
	operand.values[14]->type = Coding::number;
	operand.values[14]->val = operands.size();
	operand.values[14]->bits = 0;
	operand.values[14]->field = 0;
	operand.values[14]->math = 0;
	operand.values[14]->mathval = 0;
	operand.values[14][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches1925[] = {
	{x64Token::TOKEN, 13, 0, 0, NULL, NULL, x64Parser::tokenBranches1926 },
	{x64Token::TOKEN, 14, 0, 0, NULL, NULL, x64Parser::tokenBranches1928 },
	{x64Token::NUMBER, 10, 1, 0, NULL,&x64Parser::TokenFunc1932,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1937_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding1937_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 4},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 25, 4, 0 },
	{ Coding::eot },
};
void x64Parser::TokenFunc1937(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1937_8;
	operand.values[9] = tokenCoding1937_9;
}
x64Token x64Parser::tokenBranches1936[] = {
	{x64Token::ADDRESSCLASS, 4, 1, 0, NULL,&x64Parser::TokenFunc1937,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1935[] = {
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches1936 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1934[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1935 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1941_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding1941_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 4},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 25, 4, 0 },
	{ Coding::eot },
};
void x64Parser::TokenFunc1941(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1941_8;
	operand.values[9] = tokenCoding1941_9;
}
x64Token x64Parser::tokenBranches1940[] = {
	{x64Token::ADDRESSCLASS, 5, 1, 0, NULL,&x64Parser::TokenFunc1941,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1939[] = {
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches1940 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1938[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1939 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1934(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
void x64Parser::TokenFunc1938(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches1933[] = {
	{x64Token::REGISTERCLASS, 4, 0, 0, NULL,&x64Parser::TokenFunc1934, x64Parser::tokenBranches1934 },
	{x64Token::REGISTERCLASS, 7, 0, 0, NULL,&x64Parser::TokenFunc1938, x64Parser::tokenBranches1938 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1944_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding1944_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 9, 4},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 25, 4, 0 },
	{ Coding::eot },
};
void x64Parser::TokenFunc1944(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1944_8;
	operand.values[9] = tokenCoding1944_9;
}
x64Token x64Parser::tokenBranches1943[] = {
	{x64Token::ADDRESSCLASS, 3, 1, 0, NULL,&x64Parser::TokenFunc1944,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1942[] = {
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches1943 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1958_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding1958_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0 },
	{ Coding::eot },
};
void x64Parser::TokenFunc1958(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1958_8;
	operand.values[9] = tokenCoding1958_9;
}
x64Token x64Parser::tokenBranches1957[] = {
	{x64Token::ADDRESSCLASS, 3, 1, 0, NULL,&x64Parser::TokenFunc1958,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1948_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding1948_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0 },
	{ Coding::eot },
};
void x64Parser::TokenFunc1948(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1948_8;
	operand.values[9] = tokenCoding1948_9;
}
x64Token x64Parser::tokenBranches1947[] = {
	{x64Token::ADDRESSCLASS, 8, 1, 0, NULL,&x64Parser::TokenFunc1948,  },
	{x64Token::TOKEN, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches1957 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1946[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1947 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1962_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding1962_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0 },
	{ Coding::eot },
};
void x64Parser::TokenFunc1962(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1962_8;
	operand.values[9] = tokenCoding1962_9;
}
x64Token x64Parser::tokenBranches1961[] = {
	{x64Token::ADDRESSCLASS, 4, 1, 0, NULL,&x64Parser::TokenFunc1962,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1966_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding1966_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0, 43, 1 },
	{ Coding::eot },
};
void x64Parser::TokenFunc1966(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1966_8;
	operand.values[9] = tokenCoding1966_9;
}
x64Token x64Parser::tokenBranches1965[] = {
	{x64Token::ADDRESSCLASS, 5, 1, 0, NULL,&x64Parser::TokenFunc1966,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1951_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding1951_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0 },
	{ Coding::eot },
};
void x64Parser::TokenFunc1951(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1951_8;
	operand.values[9] = tokenCoding1951_9;
}
Coding x64Parser::tokenCoding1954_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding1954_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0, 43, 1 },
	{ Coding::eot },
};
void x64Parser::TokenFunc1954(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1954_8;
	operand.values[9] = tokenCoding1954_9;
}
x64Token x64Parser::tokenBranches1950[] = {
	{x64Token::ADDRESSCLASS, 8, 1, 0, NULL,&x64Parser::TokenFunc1951,  },
	{x64Token::ADDRESSCLASS, 9, 1, 0, NULL,&x64Parser::TokenFunc1954,  },
	{x64Token::TOKEN, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches1961 },
	{x64Token::TOKEN, 8, 0, 0, NULL, NULL, x64Parser::tokenBranches1965 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1949[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1950 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc1946(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
void x64Parser::TokenFunc1949(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches1945[] = {
	{x64Token::REGISTERCLASS, 4, 0, 0, NULL,&x64Parser::TokenFunc1946, x64Parser::tokenBranches1946 },
	{x64Token::REGISTERCLASS, 7, 0, 0, NULL,&x64Parser::TokenFunc1949, x64Parser::tokenBranches1949 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1970_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 208, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1970(x64Operand &operand, int tokenPos)
{
	operand.values[15] = new Coding[2];
	CleanupValues.push_back(operand.values[15]);
	operand.values[15]->type = Coding::number;
	operand.values[15]->val = operands.size();
	operand.values[15]->bits = 0;
	operand.values[15]->field = 0;
	operand.values[15]->math = 0;
	operand.values[15]->mathval = 0;
	operand.values[15][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[9] = tokenCoding1970_9;
}
Coding x64Parser::tokenCoding1973_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 192, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1973(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 154;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[9] = tokenCoding1973_9;
}
Coding x64Parser::tokenCoding1976_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 210, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1976(x64Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1976_9;
}
x64Token x64Parser::tokenBranches1969[] = {
	{x64Token::NUMBER, 17, 1, 0, NULL,&x64Parser::TokenFunc1970,  },
	{x64Token::NUMBER, 3, 1, 0, NULL,&x64Parser::TokenFunc1973,  },
	{x64Token::REGISTER, 4, 1, 0, NULL,&x64Parser::TokenFunc1976,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1968[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1969 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1981_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 208, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1981(x64Operand &operand, int tokenPos)
{
	operand.values[15] = new Coding[2];
	CleanupValues.push_back(operand.values[15]);
	operand.values[15]->type = Coding::number;
	operand.values[15]->val = operands.size();
	operand.values[15]->bits = 0;
	operand.values[15]->field = 0;
	operand.values[15]->math = 0;
	operand.values[15]->mathval = 0;
	operand.values[15][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[9] = tokenCoding1981_9;
}
Coding x64Parser::tokenCoding1986_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 192, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1986(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 154;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[9] = tokenCoding1986_9;
}
x64Token x64Parser::tokenBranches1980[] = {
	{x64Token::NUMBER, 17, 1, 0, NULL,&x64Parser::TokenFunc1981,  },
	{x64Token::NUMBER, 3, 1, 0, NULL,&x64Parser::TokenFunc1986,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1990_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 210, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1990(x64Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1990_9;
}
x64Token x64Parser::tokenBranches1979[] = {
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches1980 },
	{x64Token::REGISTER, 4, 1, 0, NULL,&x64Parser::TokenFunc1990,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1978[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1979 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1977[] = {
	{x64Token::ADDRESSCLASS, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches1978 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding1993_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 209, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1993(x64Operand &operand, int tokenPos)
{
	operand.values[15] = new Coding[2];
	CleanupValues.push_back(operand.values[15]);
	operand.values[15]->type = Coding::number;
	operand.values[15]->val = operands.size();
	operand.values[15]->bits = 0;
	operand.values[15]->field = 0;
	operand.values[15]->math = 0;
	operand.values[15]->mathval = 0;
	operand.values[15][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[9] = tokenCoding1993_9;
}
Coding x64Parser::tokenCoding1996_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 193, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1996(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 154;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[9] = tokenCoding1996_9;
}
Coding x64Parser::tokenCoding1999_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 211, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc1999(x64Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1999_9;
}
x64Token x64Parser::tokenBranches1992[] = {
	{x64Token::NUMBER, 17, 1, 0, NULL,&x64Parser::TokenFunc1993,  },
	{x64Token::NUMBER, 3, 1, 0, NULL,&x64Parser::TokenFunc1996,  },
	{x64Token::REGISTER, 4, 1, 0, NULL,&x64Parser::TokenFunc1999,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1991[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches1992 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2004_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 209, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2004(x64Operand &operand, int tokenPos)
{
	operand.values[15] = new Coding[2];
	CleanupValues.push_back(operand.values[15]);
	operand.values[15]->type = Coding::number;
	operand.values[15]->val = operands.size();
	operand.values[15]->bits = 0;
	operand.values[15]->field = 0;
	operand.values[15]->math = 0;
	operand.values[15]->mathval = 0;
	operand.values[15][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[9] = tokenCoding2004_9;
}
Coding x64Parser::tokenCoding2009_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 193, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2009(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 154;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[9] = tokenCoding2009_9;
}
x64Token x64Parser::tokenBranches2003[] = {
	{x64Token::NUMBER, 17, 1, 0, NULL,&x64Parser::TokenFunc2004,  },
	{x64Token::NUMBER, 3, 1, 0, NULL,&x64Parser::TokenFunc2009,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2013_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 211, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2013(x64Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding2013_9;
}
x64Token x64Parser::tokenBranches2002[] = {
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches2003 },
	{x64Token::REGISTER, 4, 1, 0, NULL,&x64Parser::TokenFunc2013,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2001[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2002 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2000[] = {
	{x64Token::ADDRESSCLASS, 4, 0, 0, NULL, NULL, x64Parser::tokenBranches2001 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2020_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 193, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2020(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 154;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[9] = tokenCoding2020_9;
}
x64Token x64Parser::tokenBranches2019[] = {
	{x64Token::NUMBER, 3, 1, 0, NULL,&x64Parser::TokenFunc2020,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2016_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 209, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2016(x64Operand &operand, int tokenPos)
{
	operand.values[15] = new Coding[2];
	CleanupValues.push_back(operand.values[15]);
	operand.values[15]->type = Coding::number;
	operand.values[15]->val = operands.size();
	operand.values[15]->bits = 0;
	operand.values[15]->field = 0;
	operand.values[15]->math = 0;
	operand.values[15]->mathval = 0;
	operand.values[15][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[9] = tokenCoding2016_9;
}
Coding x64Parser::tokenCoding2023_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 211, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2023(x64Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding2023_9;
}
x64Token x64Parser::tokenBranches2015[] = {
	{x64Token::NUMBER, 17, 1, 0, NULL,&x64Parser::TokenFunc2016,  },
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches2019 },
	{x64Token::REGISTER, 4, 1, 0, NULL,&x64Parser::TokenFunc2023,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2014[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2015 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2028_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 209, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2028(x64Operand &operand, int tokenPos)
{
	operand.values[15] = new Coding[2];
	CleanupValues.push_back(operand.values[15]);
	operand.values[15]->type = Coding::number;
	operand.values[15]->val = operands.size();
	operand.values[15]->bits = 0;
	operand.values[15]->field = 0;
	operand.values[15]->math = 0;
	operand.values[15]->mathval = 0;
	operand.values[15][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[9] = tokenCoding2028_9;
}
Coding x64Parser::tokenCoding2033_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 193, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2033(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 154;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[9] = tokenCoding2033_9;
}
x64Token x64Parser::tokenBranches2027[] = {
	{x64Token::NUMBER, 17, 1, 0, NULL,&x64Parser::TokenFunc2028,  },
	{x64Token::NUMBER, 3, 1, 0, NULL,&x64Parser::TokenFunc2033,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2037_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 211, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2037(x64Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding2037_9;
}
x64Token x64Parser::tokenBranches2026[] = {
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches2027 },
	{x64Token::REGISTER, 4, 1, 0, NULL,&x64Parser::TokenFunc2037,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2025[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2026 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2024[] = {
	{x64Token::ADDRESSCLASS, 5, 0, 0, NULL, NULL, x64Parser::tokenBranches2025 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches1967[] = {
	{x64Token::ADDRESSCLASS, 8, 0, 0, NULL, NULL, x64Parser::tokenBranches1968 },
	{x64Token::TOKEN, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches1977 },
	{x64Token::ADDRESSCLASS, 9, 0, 0, NULL, NULL, x64Parser::tokenBranches1991 },
	{x64Token::TOKEN, 8, 0, 0, NULL, NULL, x64Parser::tokenBranches2000 },
	{x64Token::ADDRESSCLASS, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches2014 },
	{x64Token::TOKEN, 9, 0, 0, NULL, NULL, x64Parser::tokenBranches2024 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2044_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2044_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0 },
	{ Coding::eot },
};
void x64Parser::TokenFunc2044(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 154;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[8] = tokenCoding2044_8;
	operand.values[9] = tokenCoding2044_9;
}
Coding x64Parser::tokenCoding2050_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2050_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0, 43, 1 },
	{ Coding::eot },
};
void x64Parser::TokenFunc2050(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2050_8;
	operand.values[9] = tokenCoding2050_9;
}
x64Token x64Parser::tokenBranches2043[] = {
	{x64Token::NUMBER, 3, 1, 0, NULL,&x64Parser::TokenFunc2044,  },
	{x64Token::REGISTER, 4, 1, 0, NULL,&x64Parser::TokenFunc2050,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2042[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2043 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2042(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches2041[] = {
	{x64Token::REGISTERCLASS, 4, 0, 0, NULL,&x64Parser::TokenFunc2042, x64Parser::tokenBranches2042 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2040[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2041 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2039[] = {
	{x64Token::ADDRESSCLASS, 4, 0, 0, NULL, NULL, x64Parser::tokenBranches2040 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2056_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2056_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0 },
	{ Coding::eot },
};
void x64Parser::TokenFunc2056(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 154;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[8] = tokenCoding2056_8;
	operand.values[9] = tokenCoding2056_9;
}
Coding x64Parser::tokenCoding2062_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2062_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0, 43, 1 },
	{ Coding::eot },
};
void x64Parser::TokenFunc2062(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2062_8;
	operand.values[9] = tokenCoding2062_9;
}
x64Token x64Parser::tokenBranches2055[] = {
	{x64Token::NUMBER, 3, 1, 0, NULL,&x64Parser::TokenFunc2056,  },
	{x64Token::REGISTER, 4, 1, 0, NULL,&x64Parser::TokenFunc2062,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2054[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2055 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2054(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches2053[] = {
	{x64Token::REGISTERCLASS, 7, 0, 0, NULL,&x64Parser::TokenFunc2054, x64Parser::tokenBranches2054 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2052[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2053 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2051[] = {
	{x64Token::ADDRESSCLASS, 5, 0, 0, NULL, NULL, x64Parser::tokenBranches2052 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2038[] = {
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches2039 },
	{x64Token::TOKEN, 9, 0, 1, NULL, NULL, x64Parser::tokenBranches2051 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2063[] = {
	{x64Token::ADDRESSCLASS, 1, 1, 0, NULL, NULL,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2075_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2075_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 99, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2075(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
	operand.values[8] = tokenCoding2075_8;
	operand.values[9] = tokenCoding2075_9;
}
x64Token x64Parser::tokenBranches2074[] = {
	{x64Token::REGISTERCLASS, 4, 1, 0, NULL,&x64Parser::TokenFunc2075,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2073[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2074 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2072[] = {
	{x64Token::ADDRESSCLASS, 4, 0, 0, NULL, NULL, x64Parser::tokenBranches2073 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2080_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
void x64Parser::TokenFunc2080(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 170;
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
	operand.values[8] = tokenCoding2080_8;
}
x64Token x64Parser::tokenBranches2079[] = {
	{x64Token::REGISTERCLASS, 7, 1, 0, NULL,&x64Parser::TokenFunc2080,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2089(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 171;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2088[] = {
	{x64Token::NUMBER, 6, 1, 0, NULL,&x64Parser::TokenFunc2089,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2093(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 172;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2092[] = {
	{x64Token::NUMBER, 7, 1, 0, NULL,&x64Parser::TokenFunc2093,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2087[] = {
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches2088 },
	{x64Token::TOKEN, 9, 0, 1, NULL, NULL, x64Parser::tokenBranches2092 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2086[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2087 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2099_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2099_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2099(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2099_8;
	operand.values[9] = tokenCoding2099_9;
}
x64Token x64Parser::tokenBranches2098[] = {
	{x64Token::ADDRESSCLASS, 6, 1, 0, NULL,&x64Parser::TokenFunc2099,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2101_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2101_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2101(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2101_8;
	operand.values[9] = tokenCoding2101_9;
}
x64Token x64Parser::tokenBranches2100[] = {
	{x64Token::ADDRESSCLASS, 7, 1, 0, NULL,&x64Parser::TokenFunc2101,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2103(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 175;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc2105(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 176;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
Coding x64Parser::tokenCoding2107_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2107_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2107(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2107_8;
	operand.values[9] = tokenCoding2107_9;
}
Coding x64Parser::tokenCoding2109_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2109_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2109(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2109_8;
	operand.values[9] = tokenCoding2109_9;
}
x64Token x64Parser::tokenBranches2102[] = {
	{x64Token::NUMBER, 6, 1, 0, NULL,&x64Parser::TokenFunc2103,  },
	{x64Token::NUMBER, 7, 1, 0, NULL,&x64Parser::TokenFunc2105,  },
	{x64Token::ADDRESSCLASS, 6, 1, 0, NULL,&x64Parser::TokenFunc2107,  },
	{x64Token::ADDRESSCLASS, 7, 1, 0, NULL,&x64Parser::TokenFunc2109,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2086(x64Operand &operand, int tokenPos)
{
	operand.values[1] = new Coding[2];
	CleanupValues.push_back(operand.values[1]);
	operand.values[1]->type = Coding::number;
	operand.values[1]->val = operands.size();
	operand.values[1]->bits = 0;
	operand.values[1]->field = 0;
	operand.values[1]->math = 0;
	operand.values[1]->mathval = 0;
	operand.values[1][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc2094(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 173;
	operand.values[14] = new Coding[2];
	CleanupValues.push_back(operand.values[14]);
	operand.values[14]->type = Coding::number;
	operand.values[14]->val = operands.size();
	operand.values[14]->bits = 0;
	operand.values[14]->field = 0;
	operand.values[14]->math = 0;
	operand.values[14]->mathval = 0;
	operand.values[14][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc2095(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 174;
	operand.values[14] = new Coding[2];
	CleanupValues.push_back(operand.values[14]);
	operand.values[14]->type = Coding::number;
	operand.values[14]->val = operands.size();
	operand.values[14]->bits = 0;
	operand.values[14]->field = 0;
	operand.values[14]->math = 0;
	operand.values[14]->mathval = 0;
	operand.values[14][1].type = Coding::eot;
	operands.push_back(numeric);
}
Coding x64Parser::tokenCoding2096_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2096_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2096(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2096_8;
	operand.values[9] = tokenCoding2096_9;
}
Coding x64Parser::tokenCoding2097_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2097_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2097(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2097_8;
	operand.values[9] = tokenCoding2097_9;
}
x64Token x64Parser::tokenBranches2085[] = {
	{x64Token::NUMBER, 4, 0, 0, NULL,&x64Parser::TokenFunc2086, x64Parser::tokenBranches2086 },
	{x64Token::NUMBER, 12, 1, 0, NULL,&x64Parser::TokenFunc2094,  },
	{x64Token::NUMBER, 14, 1, 0, NULL,&x64Parser::TokenFunc2095,  },
	{x64Token::ADDRESSCLASS, 9, 1, 0, NULL,&x64Parser::TokenFunc2096,  },
	{x64Token::ADDRESSCLASS, 10, 1, 0, NULL,&x64Parser::TokenFunc2097,  },
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches2098 },
	{x64Token::TOKEN, 9, 0, 1, NULL, NULL, x64Parser::tokenBranches2100 },
	{x64Token::TOKEN, 15, 0, 0, NULL, NULL, x64Parser::tokenBranches2102 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2161(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 177;
}
x64Token x64Parser::tokenBranches2160[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc2161,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2159[] = {
	{x64Token::REGISTER, 22, 0, 0, NULL, NULL, x64Parser::tokenBranches2160 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2158[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2159 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2157[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2158 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2156[] = {
	{x64Token::REGISTER, 24, 0, 0, NULL, NULL, x64Parser::tokenBranches2157 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2155[] = {
	{x64Token::TOKEN, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches2156 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2154[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2155 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2153[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2154 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2187(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 179;
}
x64Token x64Parser::tokenBranches2186[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc2187,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2185[] = {
	{x64Token::REGISTER, 23, 0, 0, NULL, NULL, x64Parser::tokenBranches2186 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2184[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2185 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2183[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2184 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2182[] = {
	{x64Token::REGISTER, 24, 0, 0, NULL, NULL, x64Parser::tokenBranches2183 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2181[] = {
	{x64Token::TOKEN, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches2182 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2180[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2181 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2179[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2180 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2152[] = {
	{x64Token::REGISTER, 20, 0, 0, NULL, NULL, x64Parser::tokenBranches2153 },
	{x64Token::REGISTER, 21, 0, 0, NULL, NULL, x64Parser::tokenBranches2179 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2151[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2152 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2174(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 178;
}
x64Token x64Parser::tokenBranches2173[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc2174,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2172[] = {
	{x64Token::REGISTER, 22, 0, 0, NULL, NULL, x64Parser::tokenBranches2173 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2171[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2172 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2170[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2171 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2169[] = {
	{x64Token::REGISTER, 24, 0, 0, NULL, NULL, x64Parser::tokenBranches2170 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2168[] = {
	{x64Token::TOKEN, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches2169 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2167[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2168 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2166[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2167 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2200(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 180;
}
x64Token x64Parser::tokenBranches2199[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc2200,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2198[] = {
	{x64Token::REGISTER, 23, 0, 0, NULL, NULL, x64Parser::tokenBranches2199 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2197[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2198 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2196[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2197 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2195[] = {
	{x64Token::REGISTER, 24, 0, 0, NULL, NULL, x64Parser::tokenBranches2196 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2194[] = {
	{x64Token::TOKEN, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches2195 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2193[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2194 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2192[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2193 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2165[] = {
	{x64Token::REGISTER, 20, 0, 0, NULL, NULL, x64Parser::tokenBranches2166 },
	{x64Token::REGISTER, 21, 0, 0, NULL, NULL, x64Parser::tokenBranches2192 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2164[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2165 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2164(x64Operand &operand, int tokenPos)
{
	operand.values[1] = new Coding[2];
	CleanupValues.push_back(operand.values[1]);
	operand.values[1]->type = Coding::reg;
	operand.values[1]->val = inputTokens[tokenPos]->val->ival;
	operand.values[1]->bits = 0;
	operand.values[1]->field = 0;
	operand.values[1]->math = 0;
	operand.values[1]->mathval = 0;
	operand.values[1][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches2150[] = {
	{x64Token::REGISTER, 27, 0, 0, NULL, NULL, x64Parser::tokenBranches2151 },
	{x64Token::REGISTERCLASS, 12, 0, 0, NULL,&x64Parser::TokenFunc2164, x64Parser::tokenBranches2164 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2149[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2150 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2213(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 181;
}
x64Token x64Parser::tokenBranches2212[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc2213,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2211[] = {
	{x64Token::REGISTER, 22, 0, 0, NULL, NULL, x64Parser::tokenBranches2212 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2210[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2211 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2209[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2210 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2208[] = {
	{x64Token::REGISTER, 24, 0, 0, NULL, NULL, x64Parser::tokenBranches2209 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2207[] = {
	{x64Token::TOKEN, 8, 0, 0, NULL, NULL, x64Parser::tokenBranches2208 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2206[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2207 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2205[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2206 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2239(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 183;
}
x64Token x64Parser::tokenBranches2238[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc2239,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2237[] = {
	{x64Token::REGISTER, 23, 0, 0, NULL, NULL, x64Parser::tokenBranches2238 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2236[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2237 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2235[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2236 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2234[] = {
	{x64Token::REGISTER, 24, 0, 0, NULL, NULL, x64Parser::tokenBranches2235 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2233[] = {
	{x64Token::TOKEN, 8, 0, 0, NULL, NULL, x64Parser::tokenBranches2234 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2232[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2233 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2231[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2232 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2204[] = {
	{x64Token::REGISTER, 20, 0, 0, NULL, NULL, x64Parser::tokenBranches2205 },
	{x64Token::REGISTER, 21, 0, 0, NULL, NULL, x64Parser::tokenBranches2231 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2203[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2204 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2226(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 182;
}
x64Token x64Parser::tokenBranches2225[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc2226,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2224[] = {
	{x64Token::REGISTER, 22, 0, 0, NULL, NULL, x64Parser::tokenBranches2225 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2223[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2224 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2222[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2223 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2221[] = {
	{x64Token::REGISTER, 24, 0, 0, NULL, NULL, x64Parser::tokenBranches2222 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2220[] = {
	{x64Token::TOKEN, 8, 0, 0, NULL, NULL, x64Parser::tokenBranches2221 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2219[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2220 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2218[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2219 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2252(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 184;
}
x64Token x64Parser::tokenBranches2251[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc2252,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2250[] = {
	{x64Token::REGISTER, 23, 0, 0, NULL, NULL, x64Parser::tokenBranches2251 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2249[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2250 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2248[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2249 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2247[] = {
	{x64Token::REGISTER, 24, 0, 0, NULL, NULL, x64Parser::tokenBranches2248 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2246[] = {
	{x64Token::TOKEN, 8, 0, 0, NULL, NULL, x64Parser::tokenBranches2247 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2245[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2246 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2244[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2245 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2217[] = {
	{x64Token::REGISTER, 20, 0, 0, NULL, NULL, x64Parser::tokenBranches2218 },
	{x64Token::REGISTER, 21, 0, 0, NULL, NULL, x64Parser::tokenBranches2244 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2216[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2217 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2216(x64Operand &operand, int tokenPos)
{
	operand.values[1] = new Coding[2];
	CleanupValues.push_back(operand.values[1]);
	operand.values[1]->type = Coding::reg;
	operand.values[1]->val = inputTokens[tokenPos]->val->ival;
	operand.values[1]->bits = 0;
	operand.values[1]->field = 0;
	operand.values[1]->math = 0;
	operand.values[1]->mathval = 0;
	operand.values[1][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches2202[] = {
	{x64Token::REGISTER, 27, 0, 0, NULL, NULL, x64Parser::tokenBranches2203 },
	{x64Token::REGISTERCLASS, 12, 0, 0, NULL,&x64Parser::TokenFunc2216, x64Parser::tokenBranches2216 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2201[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2202 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2265(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 185;
}
x64Token x64Parser::tokenBranches2264[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc2265,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2263[] = {
	{x64Token::REGISTER, 22, 0, 0, NULL, NULL, x64Parser::tokenBranches2264 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2262[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2263 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2261[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2262 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2260[] = {
	{x64Token::REGISTER, 24, 0, 0, NULL, NULL, x64Parser::tokenBranches2261 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2259[] = {
	{x64Token::TOKEN, 9, 0, 0, NULL, NULL, x64Parser::tokenBranches2260 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2258[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2259 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2257[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2258 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2291(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 187;
}
x64Token x64Parser::tokenBranches2290[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc2291,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2289[] = {
	{x64Token::REGISTER, 23, 0, 0, NULL, NULL, x64Parser::tokenBranches2290 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2288[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2289 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2287[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2288 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2286[] = {
	{x64Token::REGISTER, 24, 0, 0, NULL, NULL, x64Parser::tokenBranches2287 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2285[] = {
	{x64Token::TOKEN, 9, 0, 0, NULL, NULL, x64Parser::tokenBranches2286 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2284[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2285 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2283[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2284 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2256[] = {
	{x64Token::REGISTER, 20, 0, 0, NULL, NULL, x64Parser::tokenBranches2257 },
	{x64Token::REGISTER, 21, 0, 0, NULL, NULL, x64Parser::tokenBranches2283 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2255[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2256 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2278(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 186;
}
x64Token x64Parser::tokenBranches2277[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc2278,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2276[] = {
	{x64Token::REGISTER, 22, 0, 0, NULL, NULL, x64Parser::tokenBranches2277 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2275[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2276 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2274[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2275 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2273[] = {
	{x64Token::REGISTER, 24, 0, 0, NULL, NULL, x64Parser::tokenBranches2274 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2272[] = {
	{x64Token::TOKEN, 9, 0, 0, NULL, NULL, x64Parser::tokenBranches2273 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2271[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2272 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2270[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2271 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2304(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 188;
}
x64Token x64Parser::tokenBranches2303[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc2304,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2302[] = {
	{x64Token::REGISTER, 23, 0, 0, NULL, NULL, x64Parser::tokenBranches2303 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2301[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2302 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2300[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2301 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2299[] = {
	{x64Token::REGISTER, 24, 0, 0, NULL, NULL, x64Parser::tokenBranches2300 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2298[] = {
	{x64Token::TOKEN, 9, 0, 0, NULL, NULL, x64Parser::tokenBranches2299 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2297[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2298 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2296[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2297 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2269[] = {
	{x64Token::REGISTER, 20, 0, 0, NULL, NULL, x64Parser::tokenBranches2270 },
	{x64Token::REGISTER, 21, 0, 0, NULL, NULL, x64Parser::tokenBranches2296 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2268[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2269 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2268(x64Operand &operand, int tokenPos)
{
	operand.values[1] = new Coding[2];
	CleanupValues.push_back(operand.values[1]);
	operand.values[1]->type = Coding::reg;
	operand.values[1]->val = inputTokens[tokenPos]->val->ival;
	operand.values[1]->bits = 0;
	operand.values[1]->field = 0;
	operand.values[1]->math = 0;
	operand.values[1]->mathval = 0;
	operand.values[1][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches2254[] = {
	{x64Token::REGISTER, 27, 0, 0, NULL, NULL, x64Parser::tokenBranches2255 },
	{x64Token::REGISTERCLASS, 12, 0, 0, NULL,&x64Parser::TokenFunc2268, x64Parser::tokenBranches2268 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2253[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2254 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2148[] = {
	{x64Token::TOKEN, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches2149 },
	{x64Token::TOKEN, 8, 0, 0, NULL, NULL, x64Parser::tokenBranches2201 },
	{x64Token::TOKEN, 9, 0, 0, NULL, NULL, x64Parser::tokenBranches2253 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2312_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2312_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 176, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2312(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
	operand.values[8] = tokenCoding2312_8;
	operand.values[9] = tokenCoding2312_9;
}
x64Token x64Parser::tokenBranches2311[] = {
	{x64Token::REGISTERCLASS, 1, 1, 0, NULL,&x64Parser::TokenFunc2312,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2310[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2311 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2309[] = {
	{x64Token::ADDRESSCLASS, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2310 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2308[] = {
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches2309 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2326(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 189;
	operand.values[16] = new Coding[2];
	CleanupValues.push_back(operand.values[16]);
	operand.values[16]->type = Coding::number;
	operand.values[16]->val = operands.size();
	operand.values[16]->bits = 0;
	operand.values[16]->field = 0;
	operand.values[16]->math = 0;
	operand.values[16]->mathval = 0;
	operand.values[16][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2325[] = {
	{x64Token::NUMBER, 3, 1, 0, NULL,&x64Parser::TokenFunc2326,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2324[] = {
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches2325 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2323[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2324 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2323(x64Operand &operand, int tokenPos)
{
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2322[] = {
	{x64Token::NUMBER, 4, 0, 0, NULL,&x64Parser::TokenFunc2323, x64Parser::tokenBranches2323 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2321[] = {
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches2322 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2330_8[] = {
	{ Coding::indirect, 12, -1, 0, 38, 7 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2330_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 27, 5},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 3, 0, 62, 3 },
	{ Coding::eot },
};
void x64Parser::TokenFunc2330(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2330_8;
	operand.values[9] = tokenCoding2330_9;
}
x64Token x64Parser::tokenBranches2329[] = {
	{x64Token::ADDRESSCLASS, 0, 1, 0, NULL,&x64Parser::TokenFunc2330,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2328[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2329 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2328(x64Operand &operand, int tokenPos)
{
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2327[] = {
	{x64Token::NUMBER, 2, 0, 0, NULL,&x64Parser::TokenFunc2328, x64Parser::tokenBranches2328 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2343(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 163;
	operand.values[7] = new Coding[2];
	CleanupValues.push_back(operand.values[7]);
	operand.values[7]->type = Coding::reg;
	operand.values[7]->val = inputTokens[tokenPos]->val->ival;
	operand.values[7]->bits = 0;
	operand.values[7]->field = 0;
	operand.values[7]->math = 0;
	operand.values[7]->mathval = 0;
	operand.values[7][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches2342[] = {
	{x64Token::REGISTERCLASS, 13, 1, 0, NULL,&x64Parser::TokenFunc2343,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2341[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2342 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2346_9[] = {
	{ Coding::indirect, 23, -1, 0 },
	{ Coding::eot },
};
void x64Parser::TokenFunc2346(x64Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding2346_9;
}
x64Token x64Parser::tokenBranches2345[] = {
	{x64Token::ADDRESSCLASS, 2, 1, 0, NULL,&x64Parser::TokenFunc2346,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2348_9[] = {
	{ Coding::indirect, 23, -1, 0, 43, 4 },
	{ Coding::eot },
};
void x64Parser::TokenFunc2348(x64Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding2348_9;
}
x64Token x64Parser::tokenBranches2347[] = {
	{x64Token::ADDRESSCLASS, 2, 1, 0, NULL,&x64Parser::TokenFunc2348,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2344(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 163;
	operand.values[7] = new Coding[2];
	CleanupValues.push_back(operand.values[7]);
	operand.values[7]->type = Coding::reg;
	operand.values[7]->val = inputTokens[tokenPos]->val->ival;
	operand.values[7]->bits = 0;
	operand.values[7]->field = 0;
	operand.values[7]->math = 0;
	operand.values[7]->mathval = 0;
	operand.values[7][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches2340[] = {
	{x64Token::REGISTER, 30, 0, 0, NULL, NULL, x64Parser::tokenBranches2341 },
	{x64Token::REGISTERCLASS, 13, 1, 0, NULL,&x64Parser::TokenFunc2344,  },
	{x64Token::TOKEN, 9, 0, 0, NULL, NULL, x64Parser::tokenBranches2345 },
	{x64Token::TOKEN, 11, 0, 0, NULL, NULL, x64Parser::tokenBranches2347 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2352_9[] = {
	{ Coding::indirect, 23, -1, 0 },
	{ Coding::eot },
};
void x64Parser::TokenFunc2352(x64Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding2352_9;
}
x64Token x64Parser::tokenBranches2351[] = {
	{x64Token::ADDRESSCLASS, 2, 1, 0, NULL,&x64Parser::TokenFunc2352,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2350(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 163;
	operand.values[7] = new Coding[2];
	CleanupValues.push_back(operand.values[7]);
	operand.values[7]->type = Coding::reg;
	operand.values[7]->val = inputTokens[tokenPos]->val->ival;
	operand.values[7]->bits = 0;
	operand.values[7]->field = 0;
	operand.values[7]->math = 0;
	operand.values[7]->mathval = 0;
	operand.values[7][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches2349[] = {
	{x64Token::REGISTERCLASS, 13, 1, 0, NULL,&x64Parser::TokenFunc2350,  },
	{x64Token::TOKEN, 9, 0, 0, NULL, NULL, x64Parser::tokenBranches2351 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2363_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2363_9[] = {
	{ Coding::valSpecified, 221},
	{ Coding::eot },
};
void x64Parser::TokenFunc2363(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2363_8;
	operand.values[9] = tokenCoding2363_9;
}
x64Token x64Parser::tokenBranches2362[] = {
	{x64Token::ADDRESSCLASS, 11, 1, 0, NULL,&x64Parser::TokenFunc2363,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2371_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2371_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 223, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2371(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2371_8;
	operand.values[9] = tokenCoding2371_9;
}
x64Token x64Parser::tokenBranches2370[] = {
	{x64Token::ADDRESSCLASS, 1, 1, 0, NULL,&x64Parser::TokenFunc2371,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2369[] = {
	{x64Token::TOKEN, 11, 0, 0, NULL, NULL, x64Parser::tokenBranches2370 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2379_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2379_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 223, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2379(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2379_8;
	operand.values[9] = tokenCoding2379_9;
}
x64Token x64Parser::tokenBranches2378[] = {
	{x64Token::ADDRESSCLASS, 1, 1, 0, NULL,&x64Parser::TokenFunc2379,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2377[] = {
	{x64Token::TOKEN, 11, 0, 0, NULL, NULL, x64Parser::tokenBranches2378 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2385_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2385_9[] = {
	{ Coding::valSpecified, 217},
	{ Coding::eot },
};
void x64Parser::TokenFunc2385(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2385_8;
	operand.values[9] = tokenCoding2385_9;
}
x64Token x64Parser::tokenBranches2384[] = {
	{x64Token::ADDRESSCLASS, 1, 1, 0, NULL,&x64Parser::TokenFunc2385,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2387_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2387_9[] = {
	{ Coding::valSpecified, 221},
	{ Coding::eot },
};
void x64Parser::TokenFunc2387(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2387_8;
	operand.values[9] = tokenCoding2387_9;
}
x64Token x64Parser::tokenBranches2386[] = {
	{x64Token::ADDRESSCLASS, 1, 1, 0, NULL,&x64Parser::TokenFunc2387,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2389_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2389_9[] = {
	{ Coding::valSpecified, 219},
	{ Coding::eot },
};
void x64Parser::TokenFunc2389(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2389_8;
	operand.values[9] = tokenCoding2389_9;
}
x64Token x64Parser::tokenBranches2388[] = {
	{x64Token::ADDRESSCLASS, 1, 1, 0, NULL,&x64Parser::TokenFunc2389,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2383_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2383_9[] = {
	{ Coding::valSpecified, 217},
	{ Coding::eot },
};
void x64Parser::TokenFunc2383(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2383_8;
	operand.values[9] = tokenCoding2383_9;
}
x64Token x64Parser::tokenBranches2382[] = {
	{x64Token::ADDRESSCLASS, 11, 1, 0, NULL,&x64Parser::TokenFunc2383,  },
	{x64Token::TOKEN, 9, 0, 0, NULL, NULL, x64Parser::tokenBranches2384 },
	{x64Token::TOKEN, 11, 0, 0, NULL, NULL, x64Parser::tokenBranches2386 },
	{x64Token::TOKEN, 12, 0, 0, NULL, NULL, x64Parser::tokenBranches2388 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2393_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2393_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2393(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2393_8;
	operand.values[9] = tokenCoding2393_9;
}
x64Token x64Parser::tokenBranches2392[] = {
	{x64Token::ADDRESSCLASS, 1, 1, 0, NULL,&x64Parser::TokenFunc2393,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2391[] = {
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches2392 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2395_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2395_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2395(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2395_8;
	operand.values[9] = tokenCoding2395_9;
}
x64Token x64Parser::tokenBranches2394[] = {
	{x64Token::ADDRESSCLASS, 1, 1, 0, NULL,&x64Parser::TokenFunc2395,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2406_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2406_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 221, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2406(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2406_8;
	operand.values[9] = tokenCoding2406_9;
}
x64Token x64Parser::tokenBranches2405[] = {
	{x64Token::ADDRESSCLASS, 1, 1, 0, NULL,&x64Parser::TokenFunc2406,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2413_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2413_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 221, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2413(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2413_8;
	operand.values[9] = tokenCoding2413_9;
}
x64Token x64Parser::tokenBranches2412[] = {
	{x64Token::ADDRESSCLASS, 1, 1, 0, NULL,&x64Parser::TokenFunc2413,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2415_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2415_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 155, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 221, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2415(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2415_8;
	operand.values[9] = tokenCoding2415_9;
}
x64Token x64Parser::tokenBranches2414[] = {
	{x64Token::ADDRESSCLASS, 1, 1, 0, NULL,&x64Parser::TokenFunc2415,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2424_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2424_9[] = {
	{ Coding::valSpecified, 217},
	{ Coding::eot },
};
void x64Parser::TokenFunc2424(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2424_8;
	operand.values[9] = tokenCoding2424_9;
}
x64Token x64Parser::tokenBranches2423[] = {
	{x64Token::ADDRESSCLASS, 2, 1, 0, NULL,&x64Parser::TokenFunc2424,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2426_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2426_9[] = {
	{ Coding::valSpecified, 221},
	{ Coding::eot },
};
void x64Parser::TokenFunc2426(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2426_8;
	operand.values[9] = tokenCoding2426_9;
}
x64Token x64Parser::tokenBranches2425[] = {
	{x64Token::ADDRESSCLASS, 2, 1, 0, NULL,&x64Parser::TokenFunc2426,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2422_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2422_9[] = {
	{ Coding::valSpecified, 221},
	{ Coding::eot },
};
void x64Parser::TokenFunc2422(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2422_8;
	operand.values[9] = tokenCoding2422_9;
}
x64Token x64Parser::tokenBranches2421[] = {
	{x64Token::ADDRESSCLASS, 11, 1, 0, NULL,&x64Parser::TokenFunc2422,  },
	{x64Token::TOKEN, 9, 0, 0, NULL, NULL, x64Parser::tokenBranches2423 },
	{x64Token::TOKEN, 11, 0, 0, NULL, NULL, x64Parser::tokenBranches2425 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2429_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2429_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 155, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2429(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2429_8;
	operand.values[9] = tokenCoding2429_9;
}
x64Token x64Parser::tokenBranches2428[] = {
	{x64Token::ADDRESSCLASS, 1, 1, 0, NULL,&x64Parser::TokenFunc2429,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2427[] = {
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches2428 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2432_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2432_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2432(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2432_8;
	operand.values[9] = tokenCoding2432_9;
}
x64Token x64Parser::tokenBranches2431[] = {
	{x64Token::ADDRESSCLASS, 1, 1, 0, NULL,&x64Parser::TokenFunc2432,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2430[] = {
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches2431 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2434_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2434_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 155, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2434(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2434_8;
	operand.values[9] = tokenCoding2434_9;
}
x64Token x64Parser::tokenBranches2433[] = {
	{x64Token::ADDRESSCLASS, 1, 1, 0, NULL,&x64Parser::TokenFunc2434,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2436_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2436_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2436(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2436_8;
	operand.values[9] = tokenCoding2436_9;
}
x64Token x64Parser::tokenBranches2435[] = {
	{x64Token::ADDRESSCLASS, 1, 1, 0, NULL,&x64Parser::TokenFunc2436,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2440_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2440_9[] = {
	{ Coding::valSpecified, 217},
	{ Coding::eot },
};
void x64Parser::TokenFunc2440(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2440_8;
	operand.values[9] = tokenCoding2440_9;
}
x64Token x64Parser::tokenBranches2439[] = {
	{x64Token::ADDRESSCLASS, 2, 1, 0, NULL,&x64Parser::TokenFunc2440,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2442_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2442_9[] = {
	{ Coding::valSpecified, 221},
	{ Coding::eot },
};
void x64Parser::TokenFunc2442(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2442_8;
	operand.values[9] = tokenCoding2442_9;
}
x64Token x64Parser::tokenBranches2441[] = {
	{x64Token::ADDRESSCLASS, 2, 1, 0, NULL,&x64Parser::TokenFunc2442,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2444_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2444_9[] = {
	{ Coding::valSpecified, 219},
	{ Coding::eot },
};
void x64Parser::TokenFunc2444(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2444_8;
	operand.values[9] = tokenCoding2444_9;
}
x64Token x64Parser::tokenBranches2443[] = {
	{x64Token::ADDRESSCLASS, 2, 1, 0, NULL,&x64Parser::TokenFunc2444,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2438_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2438_9[] = {
	{ Coding::valSpecified, 221},
	{ Coding::eot },
};
void x64Parser::TokenFunc2438(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2438_8;
	operand.values[9] = tokenCoding2438_9;
}
x64Token x64Parser::tokenBranches2437[] = {
	{x64Token::ADDRESSCLASS, 11, 1, 0, NULL,&x64Parser::TokenFunc2438,  },
	{x64Token::TOKEN, 9, 0, 0, NULL, NULL, x64Parser::tokenBranches2439 },
	{x64Token::TOKEN, 11, 0, 0, NULL, NULL, x64Parser::tokenBranches2441 },
	{x64Token::TOKEN, 12, 0, 0, NULL, NULL, x64Parser::tokenBranches2443 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2447_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2447_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 155, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 221, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2447(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2447_8;
	operand.values[9] = tokenCoding2447_9;
}
x64Token x64Parser::tokenBranches2446[] = {
	{x64Token::ADDRESSCLASS, 1, 1, 0, NULL,&x64Parser::TokenFunc2447,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2448(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 190;
}
x64Token x64Parser::tokenBranches2445[] = {
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches2446 },
	{x64Token::REGISTER, 2, 1, 0, NULL,&x64Parser::TokenFunc2448,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2451_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2451_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 221, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2451(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2451_8;
	operand.values[9] = tokenCoding2451_9;
}
x64Token x64Parser::tokenBranches2450[] = {
	{x64Token::ADDRESSCLASS, 1, 1, 0, NULL,&x64Parser::TokenFunc2451,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2452(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 191;
}
x64Token x64Parser::tokenBranches2449[] = {
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches2450 },
	{x64Token::REGISTER, 2, 1, 0, NULL,&x64Parser::TokenFunc2452,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2477_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2477_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 107, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2477(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 192;
	operand.values[17] = new Coding[2];
	CleanupValues.push_back(operand.values[17]);
	operand.values[17]->type = Coding::number;
	operand.values[17]->val = operands.size();
	operand.values[17]->bits = 0;
	operand.values[17]->field = 0;
	operand.values[17]->math = 0;
	operand.values[17]->mathval = 0;
	operand.values[17][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[8] = tokenCoding2477_8;
	operand.values[9] = tokenCoding2477_9;
}
x64Token x64Parser::tokenBranches2476[] = {
	{x64Token::NUMBER, 1, 1, 0, NULL,&x64Parser::TokenFunc2477,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2483_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2483_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 105, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2483(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 193;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[8] = tokenCoding2483_8;
	operand.values[9] = tokenCoding2483_9;
}
x64Token x64Parser::tokenBranches2475[] = {
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches2476 },
	{x64Token::NUMBER, 4, 1, 0, NULL,&x64Parser::TokenFunc2483,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2474[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2475 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2474_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2474_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 175, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2474(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2474_8;
	operand.values[9] = tokenCoding2474_9;
}
void x64Parser::TokenFunc2512(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 196;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2473[] = {
	{x64Token::ADDRESSCLASS, 4, 1, 0, NULL,&x64Parser::TokenFunc2474, x64Parser::tokenBranches2474 },
	{x64Token::NUMBER, 4, 1, 0, NULL,&x64Parser::TokenFunc2512,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2508(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 195;
	operand.values[13] = new Coding[2];
	CleanupValues.push_back(operand.values[13]);
	operand.values[13]->type = Coding::number;
	operand.values[13]->val = operands.size();
	operand.values[13]->bits = 0;
	operand.values[13]->field = 0;
	operand.values[13]->math = 0;
	operand.values[13]->mathval = 0;
	operand.values[13][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2507[] = {
	{x64Token::NUMBER, 1, 1, 0, NULL,&x64Parser::TokenFunc2508,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2472[] = {
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches2473 },
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches2507 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2471[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2472 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2494_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2494_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 107, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2494(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 192;
	operand.values[17] = new Coding[2];
	CleanupValues.push_back(operand.values[17]);
	operand.values[17]->type = Coding::number;
	operand.values[17]->val = operands.size();
	operand.values[17]->bits = 0;
	operand.values[17]->field = 0;
	operand.values[17]->math = 0;
	operand.values[17]->mathval = 0;
	operand.values[17][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[8] = tokenCoding2494_8;
	operand.values[9] = tokenCoding2494_9;
}
x64Token x64Parser::tokenBranches2493[] = {
	{x64Token::NUMBER, 1, 1, 0, NULL,&x64Parser::TokenFunc2494,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2500_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2500_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 105, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2500(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 194;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[8] = tokenCoding2500_8;
	operand.values[9] = tokenCoding2500_9;
}
x64Token x64Parser::tokenBranches2492[] = {
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches2493 },
	{x64Token::NUMBER, 5, 1, 0, NULL,&x64Parser::TokenFunc2500,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2491[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2492 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2491_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2491_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 175, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2491(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2491_8;
	operand.values[9] = tokenCoding2491_9;
}
void x64Parser::TokenFunc2520(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 198;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2490[] = {
	{x64Token::ADDRESSCLASS, 5, 1, 0, NULL,&x64Parser::TokenFunc2491, x64Parser::tokenBranches2491 },
	{x64Token::NUMBER, 5, 1, 0, NULL,&x64Parser::TokenFunc2520,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2516(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 197;
	operand.values[13] = new Coding[2];
	CleanupValues.push_back(operand.values[13]);
	operand.values[13]->type = Coding::number;
	operand.values[13]->val = operands.size();
	operand.values[13]->bits = 0;
	operand.values[13]->field = 0;
	operand.values[13]->math = 0;
	operand.values[13]->mathval = 0;
	operand.values[13][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2515[] = {
	{x64Token::NUMBER, 1, 1, 0, NULL,&x64Parser::TokenFunc2516,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2489[] = {
	{x64Token::TOKEN, 9, 0, 1, NULL, NULL, x64Parser::tokenBranches2490 },
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches2515 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2488[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2489 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2471(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
void x64Parser::TokenFunc2488(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches2470[] = {
	{x64Token::REGISTERCLASS, 4, 0, 0, NULL,&x64Parser::TokenFunc2471, x64Parser::tokenBranches2471 },
	{x64Token::REGISTERCLASS, 7, 0, 0, NULL,&x64Parser::TokenFunc2488, x64Parser::tokenBranches2488 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2525(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 199;
	operand.values[18] = new Coding[2];
	CleanupValues.push_back(operand.values[18]);
	operand.values[18]->type = Coding::number;
	operand.values[18]->val = operands.size();
	operand.values[18]->bits = 0;
	operand.values[18]->field = 0;
	operand.values[18]->math = 0;
	operand.values[18]->mathval = 0;
	operand.values[18][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2524[] = {
	{x64Token::NUMBER, 3, 1, 0, NULL,&x64Parser::TokenFunc2525,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2536(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 202;
}
x64Token x64Parser::tokenBranches2523[] = {
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches2524 },
	{x64Token::REGISTER, 10, 1, 0, NULL,&x64Parser::TokenFunc2536,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2522[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2523 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2529(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 200;
	operand.values[18] = new Coding[2];
	CleanupValues.push_back(operand.values[18]);
	operand.values[18]->type = Coding::number;
	operand.values[18]->val = operands.size();
	operand.values[18]->bits = 0;
	operand.values[18]->field = 0;
	operand.values[18]->math = 0;
	operand.values[18]->mathval = 0;
	operand.values[18][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2528[] = {
	{x64Token::NUMBER, 3, 1, 0, NULL,&x64Parser::TokenFunc2529,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2539(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 203;
}
x64Token x64Parser::tokenBranches2527[] = {
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches2528 },
	{x64Token::REGISTER, 10, 1, 0, NULL,&x64Parser::TokenFunc2539,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2526[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2527 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2533(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 201;
	operand.values[18] = new Coding[2];
	CleanupValues.push_back(operand.values[18]);
	operand.values[18]->type = Coding::number;
	operand.values[18]->val = operands.size();
	operand.values[18]->bits = 0;
	operand.values[18]->field = 0;
	operand.values[18]->math = 0;
	operand.values[18]->mathval = 0;
	operand.values[18][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2532[] = {
	{x64Token::NUMBER, 3, 1, 0, NULL,&x64Parser::TokenFunc2533,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2542(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 204;
}
x64Token x64Parser::tokenBranches2531[] = {
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches2532 },
	{x64Token::REGISTER, 10, 1, 0, NULL,&x64Parser::TokenFunc2542,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2530[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2531 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2521[] = {
	{x64Token::REGISTER, 0, 0, 0, NULL, NULL, x64Parser::tokenBranches2522 },
	{x64Token::REGISTER, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2526 },
	{x64Token::REGISTER, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2530 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2551(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 205;
}
x64Token x64Parser::tokenBranches2550[] = {
	{x64Token::REGISTER, 10, 1, 0, NULL,&x64Parser::TokenFunc2551,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2549[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2550 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2548[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2549 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2567(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 207;
}
x64Token x64Parser::tokenBranches2566[] = {
	{x64Token::REGISTER, 10, 1, 0, NULL,&x64Parser::TokenFunc2567,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2565[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2566 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2564[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2565 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2547[] = {
	{x64Token::REGISTER, 22, 0, 0, NULL, NULL, x64Parser::tokenBranches2548 },
	{x64Token::REGISTER, 23, 0, 0, NULL, NULL, x64Parser::tokenBranches2564 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2546[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2547 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2545[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2546 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2559(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 206;
}
x64Token x64Parser::tokenBranches2558[] = {
	{x64Token::REGISTER, 10, 1, 0, NULL,&x64Parser::TokenFunc2559,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2557[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2558 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2556[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2557 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2575(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 208;
}
x64Token x64Parser::tokenBranches2574[] = {
	{x64Token::REGISTER, 10, 1, 0, NULL,&x64Parser::TokenFunc2575,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2573[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2574 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2572[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2573 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2555[] = {
	{x64Token::REGISTER, 22, 0, 0, NULL, NULL, x64Parser::tokenBranches2556 },
	{x64Token::REGISTER, 23, 0, 0, NULL, NULL, x64Parser::tokenBranches2572 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2554[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2555 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2554(x64Operand &operand, int tokenPos)
{
	operand.values[1] = new Coding[2];
	CleanupValues.push_back(operand.values[1]);
	operand.values[1]->type = Coding::reg;
	operand.values[1]->val = inputTokens[tokenPos]->val->ival;
	operand.values[1]->bits = 0;
	operand.values[1]->field = 0;
	operand.values[1]->math = 0;
	operand.values[1]->mathval = 0;
	operand.values[1][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches2553[] = {
	{x64Token::REGISTERCLASS, 12, 0, 0, NULL,&x64Parser::TokenFunc2554, x64Parser::tokenBranches2554 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2544[] = {
	{x64Token::REGISTER, 24, 0, 0, NULL, NULL, x64Parser::tokenBranches2545 },
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2553 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2583(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 209;
}
x64Token x64Parser::tokenBranches2582[] = {
	{x64Token::REGISTER, 10, 1, 0, NULL,&x64Parser::TokenFunc2583,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2581[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2582 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2580[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2581 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2599(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 211;
}
x64Token x64Parser::tokenBranches2598[] = {
	{x64Token::REGISTER, 10, 1, 0, NULL,&x64Parser::TokenFunc2599,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2597[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2598 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2596[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2597 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2579[] = {
	{x64Token::REGISTER, 22, 0, 0, NULL, NULL, x64Parser::tokenBranches2580 },
	{x64Token::REGISTER, 23, 0, 0, NULL, NULL, x64Parser::tokenBranches2596 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2578[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2579 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2577[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2578 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2591(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 210;
}
x64Token x64Parser::tokenBranches2590[] = {
	{x64Token::REGISTER, 10, 1, 0, NULL,&x64Parser::TokenFunc2591,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2589[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2590 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2588[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2589 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2607(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 212;
}
x64Token x64Parser::tokenBranches2606[] = {
	{x64Token::REGISTER, 10, 1, 0, NULL,&x64Parser::TokenFunc2607,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2605[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2606 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2604[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2605 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2587[] = {
	{x64Token::REGISTER, 22, 0, 0, NULL, NULL, x64Parser::tokenBranches2588 },
	{x64Token::REGISTER, 23, 0, 0, NULL, NULL, x64Parser::tokenBranches2604 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2586[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2587 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2586(x64Operand &operand, int tokenPos)
{
	operand.values[1] = new Coding[2];
	CleanupValues.push_back(operand.values[1]);
	operand.values[1]->type = Coding::reg;
	operand.values[1]->val = inputTokens[tokenPos]->val->ival;
	operand.values[1]->bits = 0;
	operand.values[1]->field = 0;
	operand.values[1]->math = 0;
	operand.values[1]->mathval = 0;
	operand.values[1][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches2585[] = {
	{x64Token::REGISTERCLASS, 12, 0, 0, NULL,&x64Parser::TokenFunc2586, x64Parser::tokenBranches2586 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2576[] = {
	{x64Token::REGISTER, 24, 0, 0, NULL, NULL, x64Parser::tokenBranches2577 },
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2585 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2615(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 213;
}
x64Token x64Parser::tokenBranches2614[] = {
	{x64Token::REGISTER, 10, 1, 0, NULL,&x64Parser::TokenFunc2615,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2613[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2614 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2612[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2613 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2631(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 215;
}
x64Token x64Parser::tokenBranches2630[] = {
	{x64Token::REGISTER, 10, 1, 0, NULL,&x64Parser::TokenFunc2631,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2629[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2630 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2628[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2629 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2611[] = {
	{x64Token::REGISTER, 22, 0, 0, NULL, NULL, x64Parser::tokenBranches2612 },
	{x64Token::REGISTER, 23, 0, 0, NULL, NULL, x64Parser::tokenBranches2628 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2610[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2611 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2609[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2610 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2623(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 214;
}
x64Token x64Parser::tokenBranches2622[] = {
	{x64Token::REGISTER, 10, 1, 0, NULL,&x64Parser::TokenFunc2623,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2621[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2622 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2620[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2621 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2639(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 216;
}
x64Token x64Parser::tokenBranches2638[] = {
	{x64Token::REGISTER, 10, 1, 0, NULL,&x64Parser::TokenFunc2639,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2637[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2638 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2636[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2637 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2619[] = {
	{x64Token::REGISTER, 22, 0, 0, NULL, NULL, x64Parser::tokenBranches2620 },
	{x64Token::REGISTER, 23, 0, 0, NULL, NULL, x64Parser::tokenBranches2636 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2618[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2619 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2618(x64Operand &operand, int tokenPos)
{
	operand.values[1] = new Coding[2];
	CleanupValues.push_back(operand.values[1]);
	operand.values[1]->type = Coding::reg;
	operand.values[1]->val = inputTokens[tokenPos]->val->ival;
	operand.values[1]->bits = 0;
	operand.values[1]->field = 0;
	operand.values[1]->math = 0;
	operand.values[1]->mathval = 0;
	operand.values[1][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches2617[] = {
	{x64Token::REGISTERCLASS, 12, 0, 0, NULL,&x64Parser::TokenFunc2618, x64Parser::tokenBranches2618 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2608[] = {
	{x64Token::REGISTER, 24, 0, 0, NULL, NULL, x64Parser::tokenBranches2609 },
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2617 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2543[] = {
	{x64Token::TOKEN, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches2544 },
	{x64Token::TOKEN, 8, 0, 0, NULL, NULL, x64Parser::tokenBranches2576 },
	{x64Token::TOKEN, 9, 0, 0, NULL, NULL, x64Parser::tokenBranches2608 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2646(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 217;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2645[] = {
	{x64Token::NUMBER, 3, 1, 0, NULL,&x64Parser::TokenFunc2646,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2644[] = {
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches2645 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2652_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2652_9[] = {
	{ Coding::valSpecified, 15},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2652(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2652_8;
	operand.values[9] = tokenCoding2652_9;
}
x64Token x64Parser::tokenBranches2651[] = {
	{x64Token::ADDRESSCLASS, 1, 1, 0, NULL,&x64Parser::TokenFunc2652,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2662(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 218;
	operand.values[14] = new Coding[2];
	CleanupValues.push_back(operand.values[14]);
	operand.values[14]->type = Coding::number;
	operand.values[14]->val = operands.size();
	operand.values[14]->bits = 0;
	operand.values[14]->field = 0;
	operand.values[14]->math = 0;
	operand.values[14]->mathval = 0;
	operand.values[14][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2661[] = {
	{x64Token::NUMBER, 9, 1, 0, NULL,&x64Parser::TokenFunc2662,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2664(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 219;
	operand.values[14] = new Coding[2];
	CleanupValues.push_back(operand.values[14]);
	operand.values[14]->type = Coding::number;
	operand.values[14]->val = operands.size();
	operand.values[14]->bits = 0;
	operand.values[14]->field = 0;
	operand.values[14]->math = 0;
	operand.values[14]->mathval = 0;
	operand.values[14][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2663[] = {
	{x64Token::NUMBER, 9, 1, 0, NULL,&x64Parser::TokenFunc2664,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2675(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 220;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2674[] = {
	{x64Token::NUMBER, 6, 1, 0, NULL,&x64Parser::TokenFunc2675,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2680(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 221;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2679[] = {
	{x64Token::NUMBER, 7, 1, 0, NULL,&x64Parser::TokenFunc2680,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2673[] = {
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches2674 },
	{x64Token::TOKEN, 9, 0, 1, NULL, NULL, x64Parser::tokenBranches2679 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2672[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2673 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2672(x64Operand &operand, int tokenPos)
{
	operand.values[1] = new Coding[2];
	CleanupValues.push_back(operand.values[1]);
	operand.values[1]->type = Coding::number;
	operand.values[1]->val = operands.size();
	operand.values[1]->bits = 0;
	operand.values[1]->field = 0;
	operand.values[1]->math = 0;
	operand.values[1]->mathval = 0;
	operand.values[1][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc2682(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 222;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc2684(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 223;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
Coding x64Parser::tokenCoding2686_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2686_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2686(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2686_8;
	operand.values[9] = tokenCoding2686_9;
}
x64Token x64Parser::tokenBranches2671[] = {
	{x64Token::NUMBER, 4, 0, 0, NULL,&x64Parser::TokenFunc2672, x64Parser::tokenBranches2672 },
	{x64Token::NUMBER, 6, 1, 0, NULL,&x64Parser::TokenFunc2682,  },
	{x64Token::NUMBER, 7, 1, 0, NULL,&x64Parser::TokenFunc2684,  },
	{x64Token::ADDRESSCLASS, 6, 1, 0, NULL,&x64Parser::TokenFunc2686,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2670[] = {
	{x64Token::TOKEN, 15, 0, 0, NULL, NULL, x64Parser::tokenBranches2671 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2719_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2719_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2719(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2719_8;
	operand.values[9] = tokenCoding2719_9;
}
Coding x64Parser::tokenCoding2720_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2720_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2720(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2720_8;
	operand.values[9] = tokenCoding2720_9;
}
x64Token x64Parser::tokenBranches2718[] = {
	{x64Token::ADDRESSCLASS, 9, 1, 0, NULL,&x64Parser::TokenFunc2719,  },
	{x64Token::ADDRESSCLASS, 10, 1, 0, NULL,&x64Parser::TokenFunc2720,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2727(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 224;
}
x64Token x64Parser::tokenBranches2726[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc2727,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2725[] = {
	{x64Token::REGISTER, 20, 0, 0, NULL, NULL, x64Parser::tokenBranches2726 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2724[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2725 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2723[] = {
	{x64Token::REGISTER, 27, 0, 0, NULL, NULL, x64Parser::tokenBranches2724 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2722[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2723 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2721[] = {
	{x64Token::TOKEN, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches2722 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2732(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 225;
	operand.values[14] = new Coding[2];
	CleanupValues.push_back(operand.values[14]);
	operand.values[14]->type = Coding::number;
	operand.values[14]->val = operands.size();
	operand.values[14]->bits = 0;
	operand.values[14]->field = 0;
	operand.values[14]->math = 0;
	operand.values[14]->mathval = 0;
	operand.values[14][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2731[] = {
	{x64Token::NUMBER, 9, 1, 0, NULL,&x64Parser::TokenFunc2732,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2734(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 226;
	operand.values[14] = new Coding[2];
	CleanupValues.push_back(operand.values[14]);
	operand.values[14]->type = Coding::number;
	operand.values[14]->val = operands.size();
	operand.values[14]->bits = 0;
	operand.values[14]->field = 0;
	operand.values[14]->math = 0;
	operand.values[14]->mathval = 0;
	operand.values[14][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2733[] = {
	{x64Token::NUMBER, 9, 1, 0, NULL,&x64Parser::TokenFunc2734,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2736(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 227;
	operand.values[14] = new Coding[2];
	CleanupValues.push_back(operand.values[14]);
	operand.values[14]->type = Coding::number;
	operand.values[14]->val = operands.size();
	operand.values[14]->bits = 0;
	operand.values[14]->field = 0;
	operand.values[14]->math = 0;
	operand.values[14]->mathval = 0;
	operand.values[14][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2735[] = {
	{x64Token::NUMBER, 9, 1, 0, NULL,&x64Parser::TokenFunc2736,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2738(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 227;
	operand.values[14] = new Coding[2];
	CleanupValues.push_back(operand.values[14]);
	operand.values[14]->type = Coding::number;
	operand.values[14]->val = operands.size();
	operand.values[14]->bits = 0;
	operand.values[14]->field = 0;
	operand.values[14]->math = 0;
	operand.values[14]->mathval = 0;
	operand.values[14][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2737[] = {
	{x64Token::NUMBER, 9, 1, 0, NULL,&x64Parser::TokenFunc2738,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2740(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 226;
	operand.values[14] = new Coding[2];
	CleanupValues.push_back(operand.values[14]);
	operand.values[14]->type = Coding::number;
	operand.values[14]->val = operands.size();
	operand.values[14]->bits = 0;
	operand.values[14]->field = 0;
	operand.values[14]->math = 0;
	operand.values[14]->mathval = 0;
	operand.values[14][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2739[] = {
	{x64Token::NUMBER, 9, 1, 0, NULL,&x64Parser::TokenFunc2740,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2744_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2744_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2744(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2744_8;
	operand.values[9] = tokenCoding2744_9;
}
x64Token x64Parser::tokenBranches2743[] = {
	{x64Token::ADDRESSCLASS, 10, 1, 0, NULL,&x64Parser::TokenFunc2744,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2749_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 19, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2749_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 34, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2749(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2749_8;
	operand.values[9] = tokenCoding2749_9;
}
x64Token x64Parser::tokenBranches2748[] = {
	{x64Token::ADDRESSCLASS, 10, 1, 0, NULL,&x64Parser::TokenFunc2749,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2747[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2748 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2752_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 20, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2752_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 35, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2752(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2752_8;
	operand.values[9] = tokenCoding2752_9;
}
x64Token x64Parser::tokenBranches2751[] = {
	{x64Token::ADDRESSCLASS, 10, 1, 0, NULL,&x64Parser::TokenFunc2752,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2750[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2751 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2755_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 21, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2755_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 38, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2755(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2755_8;
	operand.values[9] = tokenCoding2755_9;
}
x64Token x64Parser::tokenBranches2754[] = {
	{x64Token::ADDRESSCLASS, 10, 1, 0, NULL,&x64Parser::TokenFunc2755,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2753[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2754 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2761(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 228;
}
x64Token x64Parser::tokenBranches2760[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc2761,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2769(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 229;
}
x64Token x64Parser::tokenBranches2768[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc2769,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2783(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 231;
}
x64Token x64Parser::tokenBranches2782[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc2783,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2768(x64Operand &operand, int tokenPos)
{
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc2782(x64Operand &operand, int tokenPos)
{
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2767[] = {
	{x64Token::NUMBER, 6, 0, 0, NULL,&x64Parser::TokenFunc2768, x64Parser::tokenBranches2768 },
	{x64Token::NUMBER, 7, 0, 0, NULL,&x64Parser::TokenFunc2782, x64Parser::tokenBranches2782 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2766[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2767 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2775(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 230;
}
x64Token x64Parser::tokenBranches2774[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc2775,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2760(x64Operand &operand, int tokenPos)
{
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc2766(x64Operand &operand, int tokenPos)
{
	operand.values[1] = new Coding[2];
	CleanupValues.push_back(operand.values[1]);
	operand.values[1]->type = Coding::reg;
	operand.values[1]->val = inputTokens[tokenPos]->val->ival;
	operand.values[1]->bits = 0;
	operand.values[1]->field = 0;
	operand.values[1]->math = 0;
	operand.values[1]->mathval = 0;
	operand.values[1][1].type = Coding::eot;
}
void x64Parser::TokenFunc2774(x64Operand &operand, int tokenPos)
{
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2759[] = {
	{x64Token::NUMBER, 6, 0, 0, NULL,&x64Parser::TokenFunc2760, x64Parser::tokenBranches2760 },
	{x64Token::REGISTERCLASS, 12, 0, 0, NULL,&x64Parser::TokenFunc2766, x64Parser::tokenBranches2766 },
	{x64Token::NUMBER, 7, 0, 0, NULL,&x64Parser::TokenFunc2774, x64Parser::tokenBranches2774 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2758[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2759 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2757[] = {
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches2758 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2756[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2757 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2789(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 232;
}
x64Token x64Parser::tokenBranches2788[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc2789,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2797(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 233;
}
x64Token x64Parser::tokenBranches2796[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc2797,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2811(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 235;
}
x64Token x64Parser::tokenBranches2810[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc2811,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2796(x64Operand &operand, int tokenPos)
{
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc2810(x64Operand &operand, int tokenPos)
{
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2795[] = {
	{x64Token::NUMBER, 6, 0, 0, NULL,&x64Parser::TokenFunc2796, x64Parser::tokenBranches2796 },
	{x64Token::NUMBER, 7, 0, 0, NULL,&x64Parser::TokenFunc2810, x64Parser::tokenBranches2810 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2794[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2795 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2803(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 234;
}
x64Token x64Parser::tokenBranches2802[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc2803,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2788(x64Operand &operand, int tokenPos)
{
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc2794(x64Operand &operand, int tokenPos)
{
	operand.values[1] = new Coding[2];
	CleanupValues.push_back(operand.values[1]);
	operand.values[1]->type = Coding::reg;
	operand.values[1]->val = inputTokens[tokenPos]->val->ival;
	operand.values[1]->bits = 0;
	operand.values[1]->field = 0;
	operand.values[1]->math = 0;
	operand.values[1]->mathval = 0;
	operand.values[1][1].type = Coding::eot;
}
void x64Parser::TokenFunc2802(x64Operand &operand, int tokenPos)
{
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2787[] = {
	{x64Token::NUMBER, 6, 0, 0, NULL,&x64Parser::TokenFunc2788, x64Parser::tokenBranches2788 },
	{x64Token::REGISTERCLASS, 12, 0, 0, NULL,&x64Parser::TokenFunc2794, x64Parser::tokenBranches2794 },
	{x64Token::NUMBER, 7, 0, 0, NULL,&x64Parser::TokenFunc2802, x64Parser::tokenBranches2802 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2786[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2787 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2785[] = {
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches2786 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2784[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2785 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2817(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 236;
}
x64Token x64Parser::tokenBranches2816[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc2817,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2825(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 237;
}
x64Token x64Parser::tokenBranches2824[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc2825,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2839(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 239;
}
x64Token x64Parser::tokenBranches2838[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc2839,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2824(x64Operand &operand, int tokenPos)
{
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc2838(x64Operand &operand, int tokenPos)
{
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2823[] = {
	{x64Token::NUMBER, 6, 0, 0, NULL,&x64Parser::TokenFunc2824, x64Parser::tokenBranches2824 },
	{x64Token::NUMBER, 7, 0, 0, NULL,&x64Parser::TokenFunc2838, x64Parser::tokenBranches2838 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2822[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2823 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2831(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 238;
}
x64Token x64Parser::tokenBranches2830[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc2831,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2816(x64Operand &operand, int tokenPos)
{
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc2822(x64Operand &operand, int tokenPos)
{
	operand.values[1] = new Coding[2];
	CleanupValues.push_back(operand.values[1]);
	operand.values[1]->type = Coding::reg;
	operand.values[1]->val = inputTokens[tokenPos]->val->ival;
	operand.values[1]->bits = 0;
	operand.values[1]->field = 0;
	operand.values[1]->math = 0;
	operand.values[1]->mathval = 0;
	operand.values[1][1].type = Coding::eot;
}
void x64Parser::TokenFunc2830(x64Operand &operand, int tokenPos)
{
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2815[] = {
	{x64Token::NUMBER, 6, 0, 0, NULL,&x64Parser::TokenFunc2816, x64Parser::tokenBranches2816 },
	{x64Token::REGISTERCLASS, 12, 0, 0, NULL,&x64Parser::TokenFunc2822, x64Parser::tokenBranches2822 },
	{x64Token::NUMBER, 7, 0, 0, NULL,&x64Parser::TokenFunc2830, x64Parser::tokenBranches2830 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2814[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2815 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2813[] = {
	{x64Token::TOKEN, 9, 0, 1, NULL, NULL, x64Parser::tokenBranches2814 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2812[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2813 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2843(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 240;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
Coding x64Parser::tokenCoding2855_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2855_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 138, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2855(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2855_8;
	operand.values[9] = tokenCoding2855_9;
}
x64Token x64Parser::tokenBranches2842[] = {
	{x64Token::NUMBER, 3, 1, 0, NULL,&x64Parser::TokenFunc2843,  },
	{x64Token::ADDRESSCLASS, 3, 1, 0, NULL,&x64Parser::TokenFunc2855,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2841[] = {
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches2842 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2840[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2841 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2847(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 241;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
Coding x64Parser::tokenCoding2859_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2859_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 139, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2859(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2859_8;
	operand.values[9] = tokenCoding2859_9;
}
x64Token x64Parser::tokenBranches2846[] = {
	{x64Token::NUMBER, 4, 1, 0, NULL,&x64Parser::TokenFunc2847,  },
	{x64Token::ADDRESSCLASS, 4, 1, 0, NULL,&x64Parser::TokenFunc2859,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2845[] = {
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches2846 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2844[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2845 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2851(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 242;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
Coding x64Parser::tokenCoding2863_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2863_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 139, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2863(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2863_8;
	operand.values[9] = tokenCoding2863_9;
}
x64Token x64Parser::tokenBranches2850[] = {
	{x64Token::NUMBER, 5, 1, 0, NULL,&x64Parser::TokenFunc2851,  },
	{x64Token::ADDRESSCLASS, 5, 1, 0, NULL,&x64Parser::TokenFunc2863,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2849[] = {
	{x64Token::TOKEN, 9, 0, 1, NULL, NULL, x64Parser::tokenBranches2850 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2848[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2849 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2868_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2868_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 198, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2868(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 154;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[8] = tokenCoding2868_8;
	operand.values[9] = tokenCoding2868_9;
}
x64Token x64Parser::tokenBranches2867[] = {
	{x64Token::NUMBER, 3, 1, 0, NULL,&x64Parser::TokenFunc2868,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2866[] = {
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches2867 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2865[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2866 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2864[] = {
	{x64Token::ADDRESSCLASS, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2865 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2873_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2873_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 199, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2873(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 193;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[8] = tokenCoding2873_8;
	operand.values[9] = tokenCoding2873_9;
}
x64Token x64Parser::tokenBranches2872[] = {
	{x64Token::NUMBER, 4, 1, 0, NULL,&x64Parser::TokenFunc2873,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2871[] = {
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches2872 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2870[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2871 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2869[] = {
	{x64Token::ADDRESSCLASS, 4, 0, 0, NULL, NULL, x64Parser::tokenBranches2870 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2878_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2878_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 199, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2878(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 194;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[8] = tokenCoding2878_8;
	operand.values[9] = tokenCoding2878_9;
}
x64Token x64Parser::tokenBranches2877[] = {
	{x64Token::NUMBER, 5, 1, 0, NULL,&x64Parser::TokenFunc2878,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2876[] = {
	{x64Token::TOKEN, 9, 0, 1, NULL, NULL, x64Parser::tokenBranches2877 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2875[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2876 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2874[] = {
	{x64Token::ADDRESSCLASS, 5, 0, 0, NULL, NULL, x64Parser::tokenBranches2875 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding3011_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3011_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 199, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3011(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 194;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[8] = tokenCoding3011_8;
	operand.values[9] = tokenCoding3011_9;
}
x64Token x64Parser::tokenBranches3010[] = {
	{x64Token::NUMBER, 5, 1, 0, NULL,&x64Parser::TokenFunc3011,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2881_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 19, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2881_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 32, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2881(x64Operand &operand, int tokenPos)
{
	operand.values[19] = new Coding[2];
	CleanupValues.push_back(operand.values[19]);
	operand.values[19]->type = Coding::reg;
	operand.values[19]->val = inputTokens[tokenPos]->val->ival;
	operand.values[19]->bits = 0;
	operand.values[19]->field = 0;
	operand.values[19]->math = 0;
	operand.values[19]->mathval = 0;
	operand.values[19][1].type = Coding::eot;
	operand.values[8] = tokenCoding2881_8;
	operand.values[9] = tokenCoding2881_9;
}
Coding x64Parser::tokenCoding2884_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 20, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2884_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 33, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2884(x64Operand &operand, int tokenPos)
{
	operand.values[20] = new Coding[2];
	CleanupValues.push_back(operand.values[20]);
	operand.values[20]->type = Coding::reg;
	operand.values[20]->val = inputTokens[tokenPos]->val->ival;
	operand.values[20]->bits = 0;
	operand.values[20]->field = 0;
	operand.values[20]->math = 0;
	operand.values[20]->mathval = 0;
	operand.values[20][1].type = Coding::eot;
	operand.values[8] = tokenCoding2884_8;
	operand.values[9] = tokenCoding2884_9;
}
Coding x64Parser::tokenCoding2887_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 21, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2887_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 36, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2887(x64Operand &operand, int tokenPos)
{
	operand.values[21] = new Coding[2];
	CleanupValues.push_back(operand.values[21]);
	operand.values[21]->type = Coding::reg;
	operand.values[21]->val = inputTokens[tokenPos]->val->ival;
	operand.values[21]->bits = 0;
	operand.values[21]->field = 0;
	operand.values[21]->math = 0;
	operand.values[21]->mathval = 0;
	operand.values[21][1].type = Coding::eot;
	operand.values[8] = tokenCoding2887_8;
	operand.values[9] = tokenCoding2887_9;
}
x64Token x64Parser::tokenBranches2880[] = {
	{x64Token::REGISTERCLASS, 14, 1, 0, NULL,&x64Parser::TokenFunc2881,  },
	{x64Token::REGISTERCLASS, 15, 1, 0, NULL,&x64Parser::TokenFunc2884,  },
	{x64Token::REGISTERCLASS, 16, 1, 0, NULL,&x64Parser::TokenFunc2887,  },
	{x64Token::TOKEN, 9, 0, 1, NULL, NULL, x64Parser::tokenBranches3010 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2879[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2880 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2893(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 243;
}
x64Token x64Parser::tokenBranches2892[] = {
	{x64Token::REGISTER, 0, 1, 0, NULL,&x64Parser::TokenFunc2893,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2891[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2892 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2890[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2891 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2901(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 244;
}
x64Token x64Parser::tokenBranches2900[] = {
	{x64Token::REGISTER, 0, 1, 0, NULL,&x64Parser::TokenFunc2901,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2899[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2900 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2898[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2899 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2915(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 246;
}
x64Token x64Parser::tokenBranches2914[] = {
	{x64Token::REGISTER, 0, 1, 0, NULL,&x64Parser::TokenFunc2915,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2913[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2914 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2912[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2913 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2898(x64Operand &operand, int tokenPos)
{
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc2912(x64Operand &operand, int tokenPos)
{
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2897[] = {
	{x64Token::NUMBER, 6, 0, 0, NULL,&x64Parser::TokenFunc2898, x64Parser::tokenBranches2898 },
	{x64Token::NUMBER, 7, 0, 0, NULL,&x64Parser::TokenFunc2912, x64Parser::tokenBranches2912 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2896[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2897 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2907(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 245;
}
x64Token x64Parser::tokenBranches2906[] = {
	{x64Token::REGISTER, 0, 1, 0, NULL,&x64Parser::TokenFunc2907,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2905[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2906 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2904[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2905 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2890(x64Operand &operand, int tokenPos)
{
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc2896(x64Operand &operand, int tokenPos)
{
	operand.values[1] = new Coding[2];
	CleanupValues.push_back(operand.values[1]);
	operand.values[1]->type = Coding::reg;
	operand.values[1]->val = inputTokens[tokenPos]->val->ival;
	operand.values[1]->bits = 0;
	operand.values[1]->field = 0;
	operand.values[1]->math = 0;
	operand.values[1]->mathval = 0;
	operand.values[1][1].type = Coding::eot;
}
void x64Parser::TokenFunc2904(x64Operand &operand, int tokenPos)
{
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2889[] = {
	{x64Token::NUMBER, 6, 0, 0, NULL,&x64Parser::TokenFunc2890, x64Parser::tokenBranches2890 },
	{x64Token::REGISTERCLASS, 12, 0, 0, NULL,&x64Parser::TokenFunc2896, x64Parser::tokenBranches2896 },
	{x64Token::NUMBER, 7, 0, 0, NULL,&x64Parser::TokenFunc2904, x64Parser::tokenBranches2904 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2975_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2975_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 136, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2975(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
	operand.values[8] = tokenCoding2975_8;
	operand.values[9] = tokenCoding2975_9;
}
x64Token x64Parser::tokenBranches2974[] = {
	{x64Token::REGISTERCLASS, 1, 1, 0, NULL,&x64Parser::TokenFunc2975,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2973[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2974 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2888[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2889 },
	{x64Token::ADDRESSCLASS, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2973 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2921(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 247;
}
x64Token x64Parser::tokenBranches2920[] = {
	{x64Token::REGISTER, 2, 1, 0, NULL,&x64Parser::TokenFunc2921,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2919[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2920 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2918[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2919 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2929(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 248;
}
x64Token x64Parser::tokenBranches2928[] = {
	{x64Token::REGISTER, 2, 1, 0, NULL,&x64Parser::TokenFunc2929,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2927[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2928 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2926[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2927 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2943(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 250;
}
x64Token x64Parser::tokenBranches2942[] = {
	{x64Token::REGISTER, 2, 1, 0, NULL,&x64Parser::TokenFunc2943,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2941[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2942 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2940[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2941 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2926(x64Operand &operand, int tokenPos)
{
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc2940(x64Operand &operand, int tokenPos)
{
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2925[] = {
	{x64Token::NUMBER, 6, 0, 0, NULL,&x64Parser::TokenFunc2926, x64Parser::tokenBranches2926 },
	{x64Token::NUMBER, 7, 0, 0, NULL,&x64Parser::TokenFunc2940, x64Parser::tokenBranches2940 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2924[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2925 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2935(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 249;
}
x64Token x64Parser::tokenBranches2934[] = {
	{x64Token::REGISTER, 2, 1, 0, NULL,&x64Parser::TokenFunc2935,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2933[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2934 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2932[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2933 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2918(x64Operand &operand, int tokenPos)
{
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc2924(x64Operand &operand, int tokenPos)
{
	operand.values[1] = new Coding[2];
	CleanupValues.push_back(operand.values[1]);
	operand.values[1]->type = Coding::reg;
	operand.values[1]->val = inputTokens[tokenPos]->val->ival;
	operand.values[1]->bits = 0;
	operand.values[1]->field = 0;
	operand.values[1]->math = 0;
	operand.values[1]->mathval = 0;
	operand.values[1][1].type = Coding::eot;
}
void x64Parser::TokenFunc2932(x64Operand &operand, int tokenPos)
{
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2917[] = {
	{x64Token::NUMBER, 6, 0, 0, NULL,&x64Parser::TokenFunc2918, x64Parser::tokenBranches2918 },
	{x64Token::REGISTERCLASS, 12, 0, 0, NULL,&x64Parser::TokenFunc2924, x64Parser::tokenBranches2924 },
	{x64Token::NUMBER, 7, 0, 0, NULL,&x64Parser::TokenFunc2932, x64Parser::tokenBranches2932 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2979_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2979_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 137, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2979(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
	operand.values[8] = tokenCoding2979_8;
	operand.values[9] = tokenCoding2979_9;
}
Coding x64Parser::tokenCoding2995_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 22, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2995_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 140, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2995(x64Operand &operand, int tokenPos)
{
	operand.values[22] = new Coding[2];
	CleanupValues.push_back(operand.values[22]);
	operand.values[22]->type = Coding::reg;
	operand.values[22]->val = inputTokens[tokenPos]->val->ival;
	operand.values[22]->bits = 0;
	operand.values[22]->field = 0;
	operand.values[22]->math = 0;
	operand.values[22]->mathval = 0;
	operand.values[22][1].type = Coding::eot;
	operand.values[8] = tokenCoding2995_8;
	operand.values[9] = tokenCoding2995_9;
}
x64Token x64Parser::tokenBranches2978[] = {
	{x64Token::REGISTERCLASS, 4, 1, 0, NULL,&x64Parser::TokenFunc2979,  },
	{x64Token::REGISTERCLASS, 12, 1, 0, NULL,&x64Parser::TokenFunc2995,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2977[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2978 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2916[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2917 },
	{x64Token::ADDRESSCLASS, 4, 0, 0, NULL, NULL, x64Parser::tokenBranches2977 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2949(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 251;
}
x64Token x64Parser::tokenBranches2948[] = {
	{x64Token::REGISTER, 3, 1, 0, NULL,&x64Parser::TokenFunc2949,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2947[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2948 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2946[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2947 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2957(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 252;
}
x64Token x64Parser::tokenBranches2956[] = {
	{x64Token::REGISTER, 3, 1, 0, NULL,&x64Parser::TokenFunc2957,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2955[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2956 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2954[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2955 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2971(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 254;
}
x64Token x64Parser::tokenBranches2970[] = {
	{x64Token::REGISTER, 3, 1, 0, NULL,&x64Parser::TokenFunc2971,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2969[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2970 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2968[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2969 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2954(x64Operand &operand, int tokenPos)
{
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc2968(x64Operand &operand, int tokenPos)
{
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2953[] = {
	{x64Token::NUMBER, 6, 0, 0, NULL,&x64Parser::TokenFunc2954, x64Parser::tokenBranches2954 },
	{x64Token::NUMBER, 7, 0, 0, NULL,&x64Parser::TokenFunc2968, x64Parser::tokenBranches2968 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2952[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2953 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2963(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 253;
}
x64Token x64Parser::tokenBranches2962[] = {
	{x64Token::REGISTER, 3, 1, 0, NULL,&x64Parser::TokenFunc2963,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2961[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2962 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2960[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2961 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2946(x64Operand &operand, int tokenPos)
{
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc2952(x64Operand &operand, int tokenPos)
{
	operand.values[1] = new Coding[2];
	CleanupValues.push_back(operand.values[1]);
	operand.values[1]->type = Coding::reg;
	operand.values[1]->val = inputTokens[tokenPos]->val->ival;
	operand.values[1]->bits = 0;
	operand.values[1]->field = 0;
	operand.values[1]->math = 0;
	operand.values[1]->mathval = 0;
	operand.values[1][1].type = Coding::eot;
}
void x64Parser::TokenFunc2960(x64Operand &operand, int tokenPos)
{
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches2945[] = {
	{x64Token::NUMBER, 6, 0, 0, NULL,&x64Parser::TokenFunc2946, x64Parser::tokenBranches2946 },
	{x64Token::REGISTERCLASS, 12, 0, 0, NULL,&x64Parser::TokenFunc2952, x64Parser::tokenBranches2952 },
	{x64Token::NUMBER, 7, 0, 0, NULL,&x64Parser::TokenFunc2960, x64Parser::tokenBranches2960 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2983_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2983_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 137, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2983(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
	operand.values[8] = tokenCoding2983_8;
	operand.values[9] = tokenCoding2983_9;
}
x64Token x64Parser::tokenBranches2982[] = {
	{x64Token::REGISTERCLASS, 7, 1, 0, NULL,&x64Parser::TokenFunc2983,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2981[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2982 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2999_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 22, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2999_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 140, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2999(x64Operand &operand, int tokenPos)
{
	operand.values[22] = new Coding[2];
	CleanupValues.push_back(operand.values[22]);
	operand.values[22]->type = Coding::reg;
	operand.values[22]->val = inputTokens[tokenPos]->val->ival;
	operand.values[22]->bits = 0;
	operand.values[22]->field = 0;
	operand.values[22]->math = 0;
	operand.values[22]->mathval = 0;
	operand.values[22][1].type = Coding::eot;
	operand.values[8] = tokenCoding2999_8;
	operand.values[9] = tokenCoding2999_9;
}
x64Token x64Parser::tokenBranches2998[] = {
	{x64Token::REGISTERCLASS, 12, 1, 0, NULL,&x64Parser::TokenFunc2999,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2997[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2998 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2944[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches2945 },
	{x64Token::ADDRESSCLASS, 5, 0, 0, NULL, NULL, x64Parser::tokenBranches2981 },
	{x64Token::ADDRESSCLASS, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches2997 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2987_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 22, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2987_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 142, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2987(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2987_8;
	operand.values[9] = tokenCoding2987_9;
}
x64Token x64Parser::tokenBranches2986[] = {
	{x64Token::ADDRESSCLASS, 4, 1, 0, NULL,&x64Parser::TokenFunc2987,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding2991_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 22, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding2991_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 142, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc2991(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2991_8;
	operand.values[9] = tokenCoding2991_9;
}
x64Token x64Parser::tokenBranches2990[] = {
	{x64Token::ADDRESSCLASS, 10, 1, 0, NULL,&x64Parser::TokenFunc2991,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2985[] = {
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches2986 },
	{x64Token::TOKEN, 9, 0, 1, NULL, NULL, x64Parser::tokenBranches2990 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches2984[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches2985 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding3003_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3003_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 198, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3003(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 154;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[8] = tokenCoding3003_8;
	operand.values[9] = tokenCoding3003_9;
}
x64Token x64Parser::tokenBranches3002[] = {
	{x64Token::NUMBER, 3, 1, 0, NULL,&x64Parser::TokenFunc3003,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3001[] = {
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches3002 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3000[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches3001 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding3007_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3007_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 199, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3007(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 193;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[8] = tokenCoding3007_8;
	operand.values[9] = tokenCoding3007_9;
}
x64Token x64Parser::tokenBranches3006[] = {
	{x64Token::NUMBER, 4, 1, 0, NULL,&x64Parser::TokenFunc3007,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3005[] = {
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches3006 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3004[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches3005 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc2747(x64Operand &operand, int tokenPos)
{
	operand.values[19] = new Coding[2];
	CleanupValues.push_back(operand.values[19]);
	operand.values[19]->type = Coding::reg;
	operand.values[19]->val = inputTokens[tokenPos]->val->ival;
	operand.values[19]->bits = 0;
	operand.values[19]->field = 0;
	operand.values[19]->math = 0;
	operand.values[19]->mathval = 0;
	operand.values[19][1].type = Coding::eot;
}
void x64Parser::TokenFunc2750(x64Operand &operand, int tokenPos)
{
	operand.values[20] = new Coding[2];
	CleanupValues.push_back(operand.values[20]);
	operand.values[20]->type = Coding::reg;
	operand.values[20]->val = inputTokens[tokenPos]->val->ival;
	operand.values[20]->bits = 0;
	operand.values[20]->field = 0;
	operand.values[20]->math = 0;
	operand.values[20]->mathval = 0;
	operand.values[20][1].type = Coding::eot;
}
void x64Parser::TokenFunc2753(x64Operand &operand, int tokenPos)
{
	operand.values[21] = new Coding[2];
	CleanupValues.push_back(operand.values[21]);
	operand.values[21]->type = Coding::reg;
	operand.values[21]->val = inputTokens[tokenPos]->val->ival;
	operand.values[21]->bits = 0;
	operand.values[21]->field = 0;
	operand.values[21]->math = 0;
	operand.values[21]->mathval = 0;
	operand.values[21][1].type = Coding::eot;
}
void x64Parser::TokenFunc2840(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
void x64Parser::TokenFunc2844(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
void x64Parser::TokenFunc2848(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
void x64Parser::TokenFunc2984(x64Operand &operand, int tokenPos)
{
	operand.values[22] = new Coding[2];
	CleanupValues.push_back(operand.values[22]);
	operand.values[22]->type = Coding::reg;
	operand.values[22]->val = inputTokens[tokenPos]->val->ival;
	operand.values[22]->bits = 0;
	operand.values[22]->field = 0;
	operand.values[22]->math = 0;
	operand.values[22]->mathval = 0;
	operand.values[22][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches2746[] = {
	{x64Token::REGISTERCLASS, 14, 0, 0, NULL,&x64Parser::TokenFunc2747, x64Parser::tokenBranches2747 },
	{x64Token::REGISTERCLASS, 15, 0, 0, NULL,&x64Parser::TokenFunc2750, x64Parser::tokenBranches2750 },
	{x64Token::REGISTERCLASS, 16, 0, 0, NULL,&x64Parser::TokenFunc2753, x64Parser::tokenBranches2753 },
	{x64Token::REGISTER, 0, 0, 0, NULL, NULL, x64Parser::tokenBranches2756 },
	{x64Token::REGISTER, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches2784 },
	{x64Token::REGISTER, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches2812 },
	{x64Token::REGISTERCLASS, 1, 0, 0, NULL,&x64Parser::TokenFunc2840, x64Parser::tokenBranches2840 },
	{x64Token::REGISTERCLASS, 4, 0, 0, NULL,&x64Parser::TokenFunc2844, x64Parser::tokenBranches2844 },
	{x64Token::REGISTERCLASS, 7, 0, 0, NULL,&x64Parser::TokenFunc2848, x64Parser::tokenBranches2848 },
	{x64Token::TOKEN, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches2864 },
	{x64Token::TOKEN, 8, 0, 0, NULL, NULL, x64Parser::tokenBranches2869 },
	{x64Token::TOKEN, 9, 0, 0, NULL, NULL, x64Parser::tokenBranches2874 },
	{x64Token::ADDRESSCLASS, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches2879 },
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches2888 },
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches2916 },
	{x64Token::TOKEN, 9, 0, 1, NULL, NULL, x64Parser::tokenBranches2944 },
	{x64Token::REGISTERCLASS, 12, 0, 0, NULL,&x64Parser::TokenFunc2984, x64Parser::tokenBranches2984 },
	{x64Token::ADDRESSCLASS, 8, 0, 0, NULL, NULL, x64Parser::tokenBranches3000 },
	{x64Token::ADDRESSCLASS, 9, 0, 0, NULL, NULL, x64Parser::tokenBranches3004 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3025(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 255;
}
x64Token x64Parser::tokenBranches3024[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3025,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3023[] = {
	{x64Token::REGISTER, 20, 0, 0, NULL, NULL, x64Parser::tokenBranches3024 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3022[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches3023 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3038(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 256;
}
x64Token x64Parser::tokenBranches3037[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3038,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3036[] = {
	{x64Token::REGISTER, 20, 0, 0, NULL, NULL, x64Parser::tokenBranches3037 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3035[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches3036 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3035(x64Operand &operand, int tokenPos)
{
	operand.values[1] = new Coding[2];
	CleanupValues.push_back(operand.values[1]);
	operand.values[1]->type = Coding::reg;
	operand.values[1]->val = inputTokens[tokenPos]->val->ival;
	operand.values[1]->bits = 0;
	operand.values[1]->field = 0;
	operand.values[1]->math = 0;
	operand.values[1]->mathval = 0;
	operand.values[1][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches3021[] = {
	{x64Token::REGISTER, 27, 0, 0, NULL, NULL, x64Parser::tokenBranches3022 },
	{x64Token::REGISTERCLASS, 12, 0, 0, NULL,&x64Parser::TokenFunc3035, x64Parser::tokenBranches3035 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3020[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches3021 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3019[] = {
	{x64Token::TOKEN, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches3020 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3018[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches3019 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3017[] = {
	{x64Token::TOKEN, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches3018 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3016[] = {
	{x64Token::REGISTER, 22, 0, 0, NULL, NULL, x64Parser::tokenBranches3017 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3015[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches3016 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3014[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches3015 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3013[] = {
	{x64Token::REGISTER, 24, 0, 0, NULL, NULL, x64Parser::tokenBranches3014 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3012[] = {
	{x64Token::TOKEN, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches3013 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3053(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 257;
}
void x64Parser::TokenFunc3057(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 258;
}
void x64Parser::TokenFunc3061(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 259;
}
x64Token x64Parser::tokenBranches3052[] = {
	{x64Token::REGISTER, 0, 1, 0, NULL,&x64Parser::TokenFunc3053,  },
	{x64Token::REGISTER, 2, 1, 0, NULL,&x64Parser::TokenFunc3057,  },
	{x64Token::REGISTER, 3, 1, 0, NULL,&x64Parser::TokenFunc3061,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3051[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches3052 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3051(x64Operand &operand, int tokenPos)
{
	operand.values[18] = new Coding[2];
	CleanupValues.push_back(operand.values[18]);
	operand.values[18]->type = Coding::number;
	operand.values[18]->val = operands.size();
	operand.values[18]->bits = 0;
	operand.values[18]->field = 0;
	operand.values[18]->math = 0;
	operand.values[18]->mathval = 0;
	operand.values[18][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches3050[] = {
	{x64Token::NUMBER, 3, 0, 0, NULL,&x64Parser::TokenFunc3051, x64Parser::tokenBranches3051 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3064(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 260;
}
void x64Parser::TokenFunc3067(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 261;
}
void x64Parser::TokenFunc3070(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 262;
}
x64Token x64Parser::tokenBranches3063[] = {
	{x64Token::REGISTER, 0, 1, 0, NULL,&x64Parser::TokenFunc3064,  },
	{x64Token::REGISTER, 2, 1, 0, NULL,&x64Parser::TokenFunc3067,  },
	{x64Token::REGISTER, 3, 1, 0, NULL,&x64Parser::TokenFunc3070,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3062[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches3063 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3049[] = {
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches3050 },
	{x64Token::REGISTER, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches3062 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3079(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 263;
}
x64Token x64Parser::tokenBranches3078[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3079,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3095(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 265;
}
x64Token x64Parser::tokenBranches3094[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3095,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3077[] = {
	{x64Token::REGISTER, 20, 0, 0, NULL, NULL, x64Parser::tokenBranches3078 },
	{x64Token::REGISTER, 21, 0, 0, NULL, NULL, x64Parser::tokenBranches3094 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3076[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches3077 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3087(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 264;
}
x64Token x64Parser::tokenBranches3086[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3087,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3103(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 266;
}
x64Token x64Parser::tokenBranches3102[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3103,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3085[] = {
	{x64Token::REGISTER, 20, 0, 0, NULL, NULL, x64Parser::tokenBranches3086 },
	{x64Token::REGISTER, 21, 0, 0, NULL, NULL, x64Parser::tokenBranches3102 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3084[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches3085 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3084(x64Operand &operand, int tokenPos)
{
	operand.values[1] = new Coding[2];
	CleanupValues.push_back(operand.values[1]);
	operand.values[1]->type = Coding::reg;
	operand.values[1]->val = inputTokens[tokenPos]->val->ival;
	operand.values[1]->bits = 0;
	operand.values[1]->field = 0;
	operand.values[1]->math = 0;
	operand.values[1]->mathval = 0;
	operand.values[1][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches3075[] = {
	{x64Token::REGISTER, 27, 0, 0, NULL, NULL, x64Parser::tokenBranches3076 },
	{x64Token::REGISTERCLASS, 12, 0, 0, NULL,&x64Parser::TokenFunc3084, x64Parser::tokenBranches3084 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3074[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches3075 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3111(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 267;
}
x64Token x64Parser::tokenBranches3110[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3111,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3127(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 269;
}
x64Token x64Parser::tokenBranches3126[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3127,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3109[] = {
	{x64Token::REGISTER, 20, 0, 0, NULL, NULL, x64Parser::tokenBranches3110 },
	{x64Token::REGISTER, 21, 0, 0, NULL, NULL, x64Parser::tokenBranches3126 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3108[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches3109 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3119(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 268;
}
x64Token x64Parser::tokenBranches3118[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3119,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3135(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 270;
}
x64Token x64Parser::tokenBranches3134[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3135,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3117[] = {
	{x64Token::REGISTER, 20, 0, 0, NULL, NULL, x64Parser::tokenBranches3118 },
	{x64Token::REGISTER, 21, 0, 0, NULL, NULL, x64Parser::tokenBranches3134 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3116[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches3117 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3116(x64Operand &operand, int tokenPos)
{
	operand.values[1] = new Coding[2];
	CleanupValues.push_back(operand.values[1]);
	operand.values[1]->type = Coding::reg;
	operand.values[1]->val = inputTokens[tokenPos]->val->ival;
	operand.values[1]->bits = 0;
	operand.values[1]->field = 0;
	operand.values[1]->math = 0;
	operand.values[1]->mathval = 0;
	operand.values[1][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches3107[] = {
	{x64Token::REGISTER, 27, 0, 0, NULL, NULL, x64Parser::tokenBranches3108 },
	{x64Token::REGISTERCLASS, 12, 0, 0, NULL,&x64Parser::TokenFunc3116, x64Parser::tokenBranches3116 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3106[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches3107 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3143(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 271;
}
x64Token x64Parser::tokenBranches3142[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3143,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3159(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 273;
}
x64Token x64Parser::tokenBranches3158[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3159,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3141[] = {
	{x64Token::REGISTER, 20, 0, 0, NULL, NULL, x64Parser::tokenBranches3142 },
	{x64Token::REGISTER, 21, 0, 0, NULL, NULL, x64Parser::tokenBranches3158 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3140[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches3141 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3151(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 272;
}
x64Token x64Parser::tokenBranches3150[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3151,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3167(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 274;
}
x64Token x64Parser::tokenBranches3166[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3167,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3149[] = {
	{x64Token::REGISTER, 20, 0, 0, NULL, NULL, x64Parser::tokenBranches3150 },
	{x64Token::REGISTER, 21, 0, 0, NULL, NULL, x64Parser::tokenBranches3166 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3148[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches3149 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3148(x64Operand &operand, int tokenPos)
{
	operand.values[1] = new Coding[2];
	CleanupValues.push_back(operand.values[1]);
	operand.values[1]->type = Coding::reg;
	operand.values[1]->val = inputTokens[tokenPos]->val->ival;
	operand.values[1]->bits = 0;
	operand.values[1]->field = 0;
	operand.values[1]->math = 0;
	operand.values[1]->mathval = 0;
	operand.values[1][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches3139[] = {
	{x64Token::REGISTER, 27, 0, 0, NULL, NULL, x64Parser::tokenBranches3140 },
	{x64Token::REGISTERCLASS, 12, 0, 0, NULL,&x64Parser::TokenFunc3148, x64Parser::tokenBranches3148 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3138[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches3139 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3073[] = {
	{x64Token::TOKEN, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches3074 },
	{x64Token::TOKEN, 8, 0, 0, NULL, NULL, x64Parser::tokenBranches3106 },
	{x64Token::TOKEN, 9, 0, 0, NULL, NULL, x64Parser::tokenBranches3138 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3072[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches3073 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3071[] = {
	{x64Token::REGISTER, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches3072 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding3175_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3175_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 143, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3175(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3175_8;
	operand.values[9] = tokenCoding3175_9;
}
x64Token x64Parser::tokenBranches3174[] = {
	{x64Token::ADDRESSCLASS, 4, 1, 0, NULL,&x64Parser::TokenFunc3175,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3172(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 275;
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
void x64Parser::TokenFunc3173(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 276;
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches3171[] = {
	{x64Token::REGISTERCLASS, 4, 1, 0, NULL,&x64Parser::TokenFunc3172,  },
	{x64Token::REGISTERCLASS, 7, 1, 0, NULL,&x64Parser::TokenFunc3173,  },
	{x64Token::TOKEN, 8, 0, 0, NULL, NULL, x64Parser::tokenBranches3174 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3186(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 279;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches3185[] = {
	{x64Token::NUMBER, 1, 1, 0, NULL,&x64Parser::TokenFunc3186,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3190(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 280;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
Coding x64Parser::tokenCoding3194_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3194_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3194(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 282;
	operand.values[8] = tokenCoding3194_8;
	operand.values[9] = tokenCoding3194_9;
}
x64Token x64Parser::tokenBranches3189[] = {
	{x64Token::NUMBER, 4, 1, 0, NULL,&x64Parser::TokenFunc3190,  },
	{x64Token::ADDRESSCLASS, 4, 1, 0, NULL,&x64Parser::TokenFunc3194,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3192(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 281;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
Coding x64Parser::tokenCoding3196_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3196_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3196(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 283;
	operand.values[8] = tokenCoding3196_8;
	operand.values[9] = tokenCoding3196_9;
}
x64Token x64Parser::tokenBranches3191[] = {
	{x64Token::NUMBER, 5, 1, 0, NULL,&x64Parser::TokenFunc3192,  },
	{x64Token::ADDRESSCLASS, 5, 1, 0, NULL,&x64Parser::TokenFunc3196,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3183(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 277;
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
void x64Parser::TokenFunc3184(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 278;
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
void x64Parser::TokenFunc3187(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 280;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc3188(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 281;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc3197(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 284;
}
void x64Parser::TokenFunc3198(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 285;
}
void x64Parser::TokenFunc3199(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 286;
	operand.values[1] = new Coding[2];
	CleanupValues.push_back(operand.values[1]);
	operand.values[1]->type = Coding::reg;
	operand.values[1]->val = inputTokens[tokenPos]->val->ival;
	operand.values[1]->bits = 0;
	operand.values[1]->field = 0;
	operand.values[1]->math = 0;
	operand.values[1]->mathval = 0;
	operand.values[1][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches3182[] = {
	{x64Token::REGISTERCLASS, 4, 1, 0, NULL,&x64Parser::TokenFunc3183,  },
	{x64Token::REGISTERCLASS, 7, 1, 0, NULL,&x64Parser::TokenFunc3184,  },
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches3185 },
	{x64Token::NUMBER, 6, 1, 0, NULL,&x64Parser::TokenFunc3187,  },
	{x64Token::NUMBER, 7, 1, 0, NULL,&x64Parser::TokenFunc3188,  },
	{x64Token::TOKEN, 8, 0, 0, NULL, NULL, x64Parser::tokenBranches3189 },
	{x64Token::TOKEN, 9, 0, 0, NULL, NULL, x64Parser::tokenBranches3191 },
	{x64Token::REGISTER, 28, 1, 0, NULL,&x64Parser::TokenFunc3197,  },
	{x64Token::REGISTER, 29, 1, 0, NULL,&x64Parser::TokenFunc3198,  },
	{x64Token::REGISTERCLASS, 12, 1, 0, NULL,&x64Parser::TokenFunc3199,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3212(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 287;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc3213(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 288;
}
x64Token x64Parser::tokenBranches3211[] = {
	{x64Token::NUMBER, 4, 1, 0, NULL,&x64Parser::TokenFunc3212,  },
	{x64Token::EMPTY, 0, 1, 0, NULL,&x64Parser::TokenFunc3213,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3215(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 289;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
void x64Parser::TokenFunc3216(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 290;
}
x64Token x64Parser::tokenBranches3214[] = {
	{x64Token::NUMBER, 4, 1, 0, NULL,&x64Parser::TokenFunc3215,  },
	{x64Token::EMPTY, 0, 1, 0, NULL,&x64Parser::TokenFunc3216,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3230(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 291;
}
x64Token x64Parser::tokenBranches3229[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3230,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3242(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 293;
}
x64Token x64Parser::tokenBranches3241[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3242,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3228[] = {
	{x64Token::REGISTER, 22, 0, 0, NULL, NULL, x64Parser::tokenBranches3229 },
	{x64Token::REGISTER, 23, 0, 0, NULL, NULL, x64Parser::tokenBranches3241 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3227[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches3228 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3226[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches3227 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3236(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 292;
}
x64Token x64Parser::tokenBranches3235[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3236,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3248(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 294;
}
x64Token x64Parser::tokenBranches3247[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3248,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3234[] = {
	{x64Token::REGISTER, 22, 0, 0, NULL, NULL, x64Parser::tokenBranches3235 },
	{x64Token::REGISTER, 23, 0, 0, NULL, NULL, x64Parser::tokenBranches3247 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3233[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches3234 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3233(x64Operand &operand, int tokenPos)
{
	operand.values[1] = new Coding[2];
	CleanupValues.push_back(operand.values[1]);
	operand.values[1]->type = Coding::reg;
	operand.values[1]->val = inputTokens[tokenPos]->val->ival;
	operand.values[1]->bits = 0;
	operand.values[1]->field = 0;
	operand.values[1]->math = 0;
	operand.values[1]->mathval = 0;
	operand.values[1][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches3232[] = {
	{x64Token::REGISTERCLASS, 12, 0, 0, NULL,&x64Parser::TokenFunc3233, x64Parser::tokenBranches3233 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3225[] = {
	{x64Token::REGISTER, 24, 0, 0, NULL, NULL, x64Parser::tokenBranches3226 },
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches3232 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3254(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 295;
}
x64Token x64Parser::tokenBranches3253[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3254,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3266(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 297;
}
x64Token x64Parser::tokenBranches3265[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3266,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3252[] = {
	{x64Token::REGISTER, 22, 0, 0, NULL, NULL, x64Parser::tokenBranches3253 },
	{x64Token::REGISTER, 23, 0, 0, NULL, NULL, x64Parser::tokenBranches3265 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3251[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches3252 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3250[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches3251 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3260(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 296;
}
x64Token x64Parser::tokenBranches3259[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3260,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3272(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 298;
}
x64Token x64Parser::tokenBranches3271[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3272,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3258[] = {
	{x64Token::REGISTER, 22, 0, 0, NULL, NULL, x64Parser::tokenBranches3259 },
	{x64Token::REGISTER, 23, 0, 0, NULL, NULL, x64Parser::tokenBranches3271 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3257[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches3258 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3257(x64Operand &operand, int tokenPos)
{
	operand.values[1] = new Coding[2];
	CleanupValues.push_back(operand.values[1]);
	operand.values[1]->type = Coding::reg;
	operand.values[1]->val = inputTokens[tokenPos]->val->ival;
	operand.values[1]->bits = 0;
	operand.values[1]->field = 0;
	operand.values[1]->math = 0;
	operand.values[1]->mathval = 0;
	operand.values[1][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches3256[] = {
	{x64Token::REGISTERCLASS, 12, 0, 0, NULL,&x64Parser::TokenFunc3257, x64Parser::tokenBranches3257 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3249[] = {
	{x64Token::REGISTER, 24, 0, 0, NULL, NULL, x64Parser::tokenBranches3250 },
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches3256 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3278(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 299;
}
x64Token x64Parser::tokenBranches3277[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3278,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3290(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 301;
}
x64Token x64Parser::tokenBranches3289[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3290,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3276[] = {
	{x64Token::REGISTER, 22, 0, 0, NULL, NULL, x64Parser::tokenBranches3277 },
	{x64Token::REGISTER, 23, 0, 0, NULL, NULL, x64Parser::tokenBranches3289 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3275[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches3276 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3274[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches3275 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3284(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 300;
}
x64Token x64Parser::tokenBranches3283[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3284,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3296(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 302;
}
x64Token x64Parser::tokenBranches3295[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3296,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3282[] = {
	{x64Token::REGISTER, 22, 0, 0, NULL, NULL, x64Parser::tokenBranches3283 },
	{x64Token::REGISTER, 23, 0, 0, NULL, NULL, x64Parser::tokenBranches3295 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3281[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches3282 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3281(x64Operand &operand, int tokenPos)
{
	operand.values[1] = new Coding[2];
	CleanupValues.push_back(operand.values[1]);
	operand.values[1]->type = Coding::reg;
	operand.values[1]->val = inputTokens[tokenPos]->val->ival;
	operand.values[1]->bits = 0;
	operand.values[1]->field = 0;
	operand.values[1]->math = 0;
	operand.values[1]->mathval = 0;
	operand.values[1][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches3280[] = {
	{x64Token::REGISTERCLASS, 12, 0, 0, NULL,&x64Parser::TokenFunc3281, x64Parser::tokenBranches3281 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3273[] = {
	{x64Token::REGISTER, 24, 0, 0, NULL, NULL, x64Parser::tokenBranches3274 },
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches3280 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3224[] = {
	{x64Token::TOKEN, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches3225 },
	{x64Token::TOKEN, 8, 0, 0, NULL, NULL, x64Parser::tokenBranches3249 },
	{x64Token::TOKEN, 9, 0, 0, NULL, NULL, x64Parser::tokenBranches3273 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding3338_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3338_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3338(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3338_8;
	operand.values[9] = tokenCoding3338_9;
}
Coding x64Parser::tokenCoding3339_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3339_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3339(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3339_8;
	operand.values[9] = tokenCoding3339_9;
}
x64Token x64Parser::tokenBranches3337[] = {
	{x64Token::ADDRESSCLASS, 9, 1, 0, NULL,&x64Parser::TokenFunc3338,  },
	{x64Token::ADDRESSCLASS, 10, 1, 0, NULL,&x64Parser::TokenFunc3339,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3349(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 303;
}
x64Token x64Parser::tokenBranches3348[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3349,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3361(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 305;
}
x64Token x64Parser::tokenBranches3360[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3361,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3347[] = {
	{x64Token::REGISTER, 22, 0, 0, NULL, NULL, x64Parser::tokenBranches3348 },
	{x64Token::REGISTER, 23, 0, 0, NULL, NULL, x64Parser::tokenBranches3360 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3346[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches3347 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3345[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches3346 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3355(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 304;
}
x64Token x64Parser::tokenBranches3354[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3355,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3353[] = {
	{x64Token::REGISTER, 22, 0, 0, NULL, NULL, x64Parser::tokenBranches3354 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3352[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches3353 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3352(x64Operand &operand, int tokenPos)
{
	operand.values[1] = new Coding[2];
	CleanupValues.push_back(operand.values[1]);
	operand.values[1]->type = Coding::reg;
	operand.values[1]->val = inputTokens[tokenPos]->val->ival;
	operand.values[1]->bits = 0;
	operand.values[1]->field = 0;
	operand.values[1]->math = 0;
	operand.values[1]->mathval = 0;
	operand.values[1][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches3351[] = {
	{x64Token::REGISTERCLASS, 12, 0, 0, NULL,&x64Parser::TokenFunc3352, x64Parser::tokenBranches3352 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3344[] = {
	{x64Token::REGISTER, 24, 0, 0, NULL, NULL, x64Parser::tokenBranches3345 },
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches3351 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3343[] = {
	{x64Token::TOKEN, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches3344 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding3366_8[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3366_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3366(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3366_8;
	operand.values[9] = tokenCoding3366_9;
}
Coding x64Parser::tokenCoding3367_8[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3367_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3367(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3367_8;
	operand.values[9] = tokenCoding3367_9;
}
x64Token x64Parser::tokenBranches3365[] = {
	{x64Token::ADDRESSCLASS, 9, 1, 0, NULL,&x64Parser::TokenFunc3366,  },
	{x64Token::ADDRESSCLASS, 10, 1, 0, NULL,&x64Parser::TokenFunc3367,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3373(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 306;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches3372[] = {
	{x64Token::NUMBER, 3, 1, 0, NULL,&x64Parser::TokenFunc3373,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3371[] = {
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches3372 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3370[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches3371 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3377(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 307;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches3376[] = {
	{x64Token::NUMBER, 4, 1, 0, NULL,&x64Parser::TokenFunc3377,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3375[] = {
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches3376 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3374[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches3375 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3381(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 308;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
}
x64Token x64Parser::tokenBranches3380[] = {
	{x64Token::NUMBER, 5, 1, 0, NULL,&x64Parser::TokenFunc3381,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3379[] = {
	{x64Token::TOKEN, 9, 0, 1, NULL, NULL, x64Parser::tokenBranches3380 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3378[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches3379 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding3385_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3385_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 132, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3385(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
	operand.values[8] = tokenCoding3385_8;
	operand.values[9] = tokenCoding3385_9;
}
x64Token x64Parser::tokenBranches3384[] = {
	{x64Token::REGISTERCLASS, 1, 1, 0, NULL,&x64Parser::TokenFunc3385,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3383[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches3384 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3382[] = {
	{x64Token::ADDRESSCLASS, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches3383 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding3389_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3389_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 133, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3389(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
	operand.values[8] = tokenCoding3389_8;
	operand.values[9] = tokenCoding3389_9;
}
x64Token x64Parser::tokenBranches3388[] = {
	{x64Token::REGISTERCLASS, 4, 1, 0, NULL,&x64Parser::TokenFunc3389,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3387[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches3388 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3386[] = {
	{x64Token::ADDRESSCLASS, 4, 0, 0, NULL, NULL, x64Parser::tokenBranches3387 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding3393_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3393_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 133, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3393(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
	operand.values[8] = tokenCoding3393_8;
	operand.values[9] = tokenCoding3393_9;
}
x64Token x64Parser::tokenBranches3392[] = {
	{x64Token::REGISTERCLASS, 7, 1, 0, NULL,&x64Parser::TokenFunc3393,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3391[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches3392 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3390[] = {
	{x64Token::ADDRESSCLASS, 5, 0, 0, NULL, NULL, x64Parser::tokenBranches3391 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding3397_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3397_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 132, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3397(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3397_8;
	operand.values[9] = tokenCoding3397_9;
}
x64Token x64Parser::tokenBranches3396[] = {
	{x64Token::ADDRESSCLASS, 3, 1, 0, NULL,&x64Parser::TokenFunc3397,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3395[] = {
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches3396 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3394[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches3395 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding3401_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3401_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 133, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3401(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3401_8;
	operand.values[9] = tokenCoding3401_9;
}
x64Token x64Parser::tokenBranches3400[] = {
	{x64Token::ADDRESSCLASS, 4, 1, 0, NULL,&x64Parser::TokenFunc3401,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3399[] = {
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches3400 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3398[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches3399 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding3405_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3405_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 133, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3405(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3405_8;
	operand.values[9] = tokenCoding3405_9;
}
x64Token x64Parser::tokenBranches3404[] = {
	{x64Token::ADDRESSCLASS, 5, 1, 0, NULL,&x64Parser::TokenFunc3405,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3403[] = {
	{x64Token::TOKEN, 9, 0, 1, NULL, NULL, x64Parser::tokenBranches3404 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3402[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches3403 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding3408_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3408_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 246, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3408(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 154;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[8] = tokenCoding3408_8;
	operand.values[9] = tokenCoding3408_9;
}
x64Token x64Parser::tokenBranches3407[] = {
	{x64Token::NUMBER, 3, 1, 0, NULL,&x64Parser::TokenFunc3408,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3406[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches3407 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding3411_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3411_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 247, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3411(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 193;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[8] = tokenCoding3411_8;
	operand.values[9] = tokenCoding3411_9;
}
x64Token x64Parser::tokenBranches3410[] = {
	{x64Token::NUMBER, 4, 1, 0, NULL,&x64Parser::TokenFunc3411,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3409[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches3410 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding3414_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3414_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 247, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3414(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 194;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[8] = tokenCoding3414_8;
	operand.values[9] = tokenCoding3414_9;
}
x64Token x64Parser::tokenBranches3413[] = {
	{x64Token::NUMBER, 5, 1, 0, NULL,&x64Parser::TokenFunc3414,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3412[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches3413 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding3418_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3418_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 246, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3418(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 154;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[8] = tokenCoding3418_8;
	operand.values[9] = tokenCoding3418_9;
}
x64Token x64Parser::tokenBranches3417[] = {
	{x64Token::NUMBER, 3, 1, 0, NULL,&x64Parser::TokenFunc3418,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3416[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches3417 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3415[] = {
	{x64Token::ADDRESSCLASS, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches3416 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding3422_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3422_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 247, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3422(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 193;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[8] = tokenCoding3422_8;
	operand.values[9] = tokenCoding3422_9;
}
x64Token x64Parser::tokenBranches3421[] = {
	{x64Token::NUMBER, 4, 1, 0, NULL,&x64Parser::TokenFunc3422,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3420[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches3421 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3419[] = {
	{x64Token::ADDRESSCLASS, 4, 0, 0, NULL, NULL, x64Parser::tokenBranches3420 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding3426_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3426_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 247, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3426(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 194;
	operand.values[12] = new Coding[2];
	CleanupValues.push_back(operand.values[12]);
	operand.values[12]->type = Coding::number;
	operand.values[12]->val = operands.size();
	operand.values[12]->bits = 0;
	operand.values[12]->field = 0;
	operand.values[12]->math = 0;
	operand.values[12]->mathval = 0;
	operand.values[12][1].type = Coding::eot;
	operands.push_back(numeric);
	operand.values[8] = tokenCoding3426_8;
	operand.values[9] = tokenCoding3426_9;
}
x64Token x64Parser::tokenBranches3425[] = {
	{x64Token::NUMBER, 5, 1, 0, NULL,&x64Parser::TokenFunc3426,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3424[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches3425 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3423[] = {
	{x64Token::ADDRESSCLASS, 5, 0, 0, NULL, NULL, x64Parser::tokenBranches3424 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3394(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
void x64Parser::TokenFunc3398(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
void x64Parser::TokenFunc3402(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches3369[] = {
	{x64Token::REGISTER, 0, 0, 0, NULL, NULL, x64Parser::tokenBranches3370 },
	{x64Token::REGISTER, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches3374 },
	{x64Token::REGISTER, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches3378 },
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches3382 },
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches3386 },
	{x64Token::TOKEN, 9, 0, 1, NULL, NULL, x64Parser::tokenBranches3390 },
	{x64Token::REGISTERCLASS, 1, 0, 0, NULL,&x64Parser::TokenFunc3394, x64Parser::tokenBranches3394 },
	{x64Token::REGISTERCLASS, 4, 0, 0, NULL,&x64Parser::TokenFunc3398, x64Parser::tokenBranches3398 },
	{x64Token::REGISTERCLASS, 7, 0, 0, NULL,&x64Parser::TokenFunc3402, x64Parser::tokenBranches3402 },
	{x64Token::ADDRESSCLASS, 8, 0, 0, NULL, NULL, x64Parser::tokenBranches3406 },
	{x64Token::ADDRESSCLASS, 9, 0, 0, NULL, NULL, x64Parser::tokenBranches3409 },
	{x64Token::ADDRESSCLASS, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches3412 },
	{x64Token::TOKEN, 10, 0, 0, NULL, NULL, x64Parser::tokenBranches3415 },
	{x64Token::TOKEN, 8, 0, 0, NULL, NULL, x64Parser::tokenBranches3419 },
	{x64Token::TOKEN, 9, 0, 0, NULL, NULL, x64Parser::tokenBranches3423 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding3429_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3429_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3429(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3429_8;
	operand.values[9] = tokenCoding3429_9;
}
Coding x64Parser::tokenCoding3430_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3430_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3430(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3430_8;
	operand.values[9] = tokenCoding3430_9;
}
x64Token x64Parser::tokenBranches3428[] = {
	{x64Token::ADDRESSCLASS, 9, 1, 0, NULL,&x64Parser::TokenFunc3429,  },
	{x64Token::ADDRESSCLASS, 10, 1, 0, NULL,&x64Parser::TokenFunc3430,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding3432_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3432_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3432(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3432_8;
	operand.values[9] = tokenCoding3432_9;
}
Coding x64Parser::tokenCoding3433_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3433_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3433(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3433_8;
	operand.values[9] = tokenCoding3433_9;
}
x64Token x64Parser::tokenBranches3431[] = {
	{x64Token::ADDRESSCLASS, 9, 1, 0, NULL,&x64Parser::TokenFunc3432,  },
	{x64Token::ADDRESSCLASS, 10, 1, 0, NULL,&x64Parser::TokenFunc3433,  },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding3441_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3441_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 192, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3441(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
	operand.values[8] = tokenCoding3441_8;
	operand.values[9] = tokenCoding3441_9;
}
x64Token x64Parser::tokenBranches3440[] = {
	{x64Token::REGISTERCLASS, 1, 1, 0, NULL,&x64Parser::TokenFunc3441,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3439[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches3440 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3438[] = {
	{x64Token::ADDRESSCLASS, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches3439 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3437[] = {
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches3438 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding3446_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3446_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 166, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3446(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3446_8;
	operand.values[9] = tokenCoding3446_9;
}
x64Token x64Parser::tokenBranches3445[] = {
	{x64Token::ADDRESSCLASS, 4, 1, 0, NULL,&x64Parser::TokenFunc3446,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3444[] = {
	{x64Token::TOKEN, 8, 0, 1, NULL, NULL, x64Parser::tokenBranches3445 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3443[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches3444 },
	{x64Token::EOT }
};
Coding x64Parser::tokenCoding3450_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::tokenCoding3450_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 166, 8},
	{ Coding::eot },
};
void x64Parser::TokenFunc3450(x64Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3450_8;
	operand.values[9] = tokenCoding3450_9;
}
x64Token x64Parser::tokenBranches3449[] = {
	{x64Token::ADDRESSCLASS, 5, 1, 0, NULL,&x64Parser::TokenFunc3450,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3448[] = {
	{x64Token::TOKEN, 9, 0, 1, NULL, NULL, x64Parser::tokenBranches3449 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3447[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches3448 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3443(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
void x64Parser::TokenFunc3447(x64Operand &operand, int tokenPos)
{
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches3442[] = {
	{x64Token::REGISTERCLASS, 4, 0, 0, NULL,&x64Parser::TokenFunc3443, x64Parser::tokenBranches3443 },
	{x64Token::REGISTERCLASS, 7, 0, 0, NULL,&x64Parser::TokenFunc3447, x64Parser::tokenBranches3447 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3454(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 309;
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches3453[] = {
	{x64Token::REGISTERCLASS, 4, 1, 0, NULL,&x64Parser::TokenFunc3454,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3452[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches3453 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3457(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 310;
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches3456[] = {
	{x64Token::REGISTERCLASS, 7, 1, 0, NULL,&x64Parser::TokenFunc3457,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3455[] = {
	{x64Token::TOKEN, 7, 0, 0, NULL, NULL, x64Parser::tokenBranches3456 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3451[] = {
	{x64Token::REGISTER, 2, 0, 0, NULL, NULL, x64Parser::tokenBranches3452 },
	{x64Token::REGISTER, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches3455 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3464(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 311;
}
x64Token x64Parser::tokenBranches3463[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3464,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3476(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 313;
}
x64Token x64Parser::tokenBranches3475[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3476,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3462[] = {
	{x64Token::REGISTER, 14, 0, 0, NULL, NULL, x64Parser::tokenBranches3463 },
	{x64Token::REGISTER, 15, 0, 0, NULL, NULL, x64Parser::tokenBranches3475 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3461[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches3462 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3470(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 312;
}
x64Token x64Parser::tokenBranches3469[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3470,  },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3482(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 314;
}
x64Token x64Parser::tokenBranches3481[] = {
	{x64Token::TOKEN, 2, 1, 0, NULL,&x64Parser::TokenFunc3482,  },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3468[] = {
	{x64Token::REGISTER, 14, 0, 0, NULL, NULL, x64Parser::tokenBranches3469 },
	{x64Token::REGISTER, 15, 0, 0, NULL, NULL, x64Parser::tokenBranches3481 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3467[] = {
	{x64Token::TOKEN, 3, 0, 0, NULL, NULL, x64Parser::tokenBranches3468 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3467(x64Operand &operand, int tokenPos)
{
	operand.values[1] = new Coding[2];
	CleanupValues.push_back(operand.values[1]);
	operand.values[1]->type = Coding::reg;
	operand.values[1]->val = inputTokens[tokenPos]->val->ival;
	operand.values[1]->bits = 0;
	operand.values[1]->field = 0;
	operand.values[1]->math = 0;
	operand.values[1]->mathval = 0;
	operand.values[1][1].type = Coding::eot;
}
x64Token x64Parser::tokenBranches3460[] = {
	{x64Token::REGISTER, 27, 0, 0, NULL, NULL, x64Parser::tokenBranches3461 },
	{x64Token::REGISTERCLASS, 12, 0, 0, NULL,&x64Parser::TokenFunc3467, x64Parser::tokenBranches3467 },
	{x64Token::EOT }
};
x64Token x64Parser::tokenBranches3459[] = {
	{x64Token::TOKEN, 1, 0, 0, NULL, NULL, x64Parser::tokenBranches3460 },
	{x64Token::EOT }
};
void x64Parser::TokenFunc3483(x64Operand &operand, int tokenPos)
{
	operand.operandCoding = 315;
}
x64Token x64Parser::tokenBranches3458[] = {
	{x64Token::TOKEN, 10, 0, 1, NULL, NULL, x64Parser::tokenBranches3459 },
	{x64Token::EMPTY, 0, 1, 0, NULL,&x64Parser::TokenFunc3483,  },
	{x64Token::EOT }
};
bool x64Parser::Opcode0(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1764, operand);
	return rv;
}
bool x64Parser::Opcode1(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1766, operand);
	return rv;
}
bool x64Parser::Opcode2(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1768, operand);
	return rv;
}
bool x64Parser::Opcode3(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1777, operand);
	return rv;
}
bool x64Parser::Opcode4(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1786, operand);
	return rv;
}
bool x64Parser::Opcode5(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1868, operand);
	return rv;
}
bool x64Parser::Opcode6(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1879, operand);
	return rv;
}
bool x64Parser::Opcode7(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1889, operand);
	return rv;
}
bool x64Parser::Opcode8(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1899, operand);
	return rv;
}
bool x64Parser::Opcode9(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1911, operand);
	return rv;
}
bool x64Parser::Opcode10(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1917, operand);
	return rv;
}
bool x64Parser::Opcode11(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1922, operand);
	return rv;
}
bool x64Parser::Opcode12(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1925, operand);
	return rv;
}
bool x64Parser::Opcode13(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1933, operand);
	return rv;
}
bool x64Parser::Opcode14(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1942, operand);
	return rv;
}
bool x64Parser::Opcode15(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1945, operand);
	return rv;
}
bool x64Parser::Opcode16(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1967, operand);
	return rv;
}
bool x64Parser::Opcode17(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2038, operand);
	return rv;
}
bool x64Parser::Opcode18(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2063, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings19_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 55, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode19(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings19_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings20_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 213, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 10, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode20(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings20_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings21_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 212, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 10, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode21(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings21_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings22_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 63, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode22(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings22_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings23_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings23_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 20, 8},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings23_24[] = {
	{ Coding::valSpecified, 16},
	{ Coding::eot },
};
bool x64Parser::Opcode23(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings23_8;
	operand.values[23] = OpcodeCodings23_23;
	operand.values[24] = OpcodeCodings23_24;
	bool rv;
	{
		rv = Opcode4(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings24_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings24_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 8},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings24_24[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
bool x64Parser::Opcode24(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings24_8;
	operand.values[23] = OpcodeCodings24_23;
	operand.values[24] = OpcodeCodings24_24;
	bool rv;
	{
		rv = Opcode4(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings25_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings25_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 36, 8},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings25_24[] = {
	{ Coding::valSpecified, 32},
	{ Coding::eot },
};
bool x64Parser::Opcode25(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings25_8;
	operand.values[23] = OpcodeCodings25_23;
	operand.values[24] = OpcodeCodings25_24;
	bool rv;
	{
		rv = Opcode4(operand);
	}
	return rv;
}
bool x64Parser::Opcode26(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2072, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings27_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 98, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode27(x64Operand &operand)
{
	operand.values[23] = OpcodeCodings27_23;
	bool rv;
	{
		rv = Opcode2(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings28_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 188, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode28(x64Operand &operand)
{
	operand.values[23] = OpcodeCodings28_23;
	bool rv;
	{
		rv = Opcode2(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings29_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 189, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode29(x64Operand &operand)
{
	operand.values[23] = OpcodeCodings29_23;
	bool rv;
	{
		rv = Opcode2(operand);
	}
	return rv;
}
bool x64Parser::Opcode30(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2079, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings31_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings31_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 186, 8},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings31_24[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 163, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode31(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings31_8;
	operand.values[23] = OpcodeCodings31_23;
	operand.values[24] = OpcodeCodings31_24;
	bool rv;
	{
		rv = Opcode5(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings32_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings32_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 186, 8},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings32_24[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 187, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode32(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings32_8;
	operand.values[23] = OpcodeCodings32_23;
	operand.values[24] = OpcodeCodings32_24;
	bool rv;
	{
		rv = Opcode5(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings33_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings33_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 186, 8},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings33_24[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 179, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode33(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings33_8;
	operand.values[23] = OpcodeCodings33_23;
	operand.values[24] = OpcodeCodings33_24;
	bool rv;
	{
		rv = Opcode5(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings34_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings34_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 186, 8},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings34_24[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 171, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode34(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings34_8;
	operand.values[23] = OpcodeCodings34_23;
	operand.values[24] = OpcodeCodings34_24;
	bool rv;
	{
		rv = Opcode5(operand);
	}
	return rv;
}
bool x64Parser::Opcode35(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2085, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings36_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 152, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode36(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings36_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings37_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 153, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode37(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings37_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings38_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 248, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode38(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings38_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings39_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 252, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode39(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings39_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings40_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 250, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode40(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings40_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings41_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode41(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings41_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings42_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 245, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode42(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings42_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings43_25[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
bool x64Parser::Opcode43(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings43_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings44_25[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
bool x64Parser::Opcode44(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings44_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings45_25[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
bool x64Parser::Opcode45(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings45_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings46_25[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
bool x64Parser::Opcode46(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings46_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings47_25[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
bool x64Parser::Opcode47(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings47_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings48_25[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
bool x64Parser::Opcode48(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings48_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings49_25[] = {
	{ Coding::valSpecified, 15},
	{ Coding::eot },
};
bool x64Parser::Opcode49(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings49_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings50_25[] = {
	{ Coding::valSpecified, 13},
	{ Coding::eot },
};
bool x64Parser::Opcode50(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings50_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings51_25[] = {
	{ Coding::valSpecified, 12},
	{ Coding::eot },
};
bool x64Parser::Opcode51(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings51_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings52_25[] = {
	{ Coding::valSpecified, 14},
	{ Coding::eot },
};
bool x64Parser::Opcode52(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings52_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings53_25[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
bool x64Parser::Opcode53(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings53_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings54_25[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
bool x64Parser::Opcode54(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings54_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings55_25[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
bool x64Parser::Opcode55(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings55_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings56_25[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
bool x64Parser::Opcode56(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings56_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings57_25[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
bool x64Parser::Opcode57(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings57_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings58_25[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
bool x64Parser::Opcode58(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings58_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings59_25[] = {
	{ Coding::valSpecified, 14},
	{ Coding::eot },
};
bool x64Parser::Opcode59(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings59_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings60_25[] = {
	{ Coding::valSpecified, 12},
	{ Coding::eot },
};
bool x64Parser::Opcode60(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings60_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings61_25[] = {
	{ Coding::valSpecified, 13},
	{ Coding::eot },
};
bool x64Parser::Opcode61(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings61_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings62_25[] = {
	{ Coding::valSpecified, 15},
	{ Coding::eot },
};
bool x64Parser::Opcode62(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings62_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings63_25[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
bool x64Parser::Opcode63(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings63_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings64_25[] = {
	{ Coding::valSpecified, 11},
	{ Coding::eot },
};
bool x64Parser::Opcode64(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings64_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings65_25[] = {
	{ Coding::valSpecified, 9},
	{ Coding::eot },
};
bool x64Parser::Opcode65(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings65_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings66_25[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
bool x64Parser::Opcode66(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings66_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings67_25[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
bool x64Parser::Opcode67(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings67_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings68_25[] = {
	{ Coding::valSpecified, 10},
	{ Coding::eot },
};
bool x64Parser::Opcode68(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings68_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings69_25[] = {
	{ Coding::valSpecified, 10},
	{ Coding::eot },
};
bool x64Parser::Opcode69(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings69_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings70_25[] = {
	{ Coding::valSpecified, 11},
	{ Coding::eot },
};
bool x64Parser::Opcode70(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings70_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings71_25[] = {
	{ Coding::valSpecified, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode71(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings71_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings72_25[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
bool x64Parser::Opcode72(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings72_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings73_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings73_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 60, 8},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings73_24[] = {
	{ Coding::valSpecified, 56},
	{ Coding::eot },
};
bool x64Parser::Opcode73(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings73_8;
	operand.values[23] = OpcodeCodings73_23;
	operand.values[24] = OpcodeCodings73_24;
	bool rv;
	{
		rv = Opcode4(operand);
	}
	return rv;
}
bool x64Parser::Opcode74(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2148, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings75_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 166, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode75(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings75_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings76_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 167, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode76(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings76_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings77_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 167, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode77(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings77_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
bool x64Parser::Opcode78(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2308, operand);
	return rv;
}
bool x64Parser::Opcode79(x64Operand &operand)
{
	bool rv = true;
	return rv;
}
Coding x64Parser::OpcodeCodings80_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 162, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode80(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings80_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings81_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 153, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode81(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings81_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings82_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 152, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode82(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings82_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings83_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 39, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode83(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings83_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings84_9[] = {
	{ Coding::valSpecified, 47},
	{ Coding::eot },
};
bool x64Parser::Opcode84(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings84_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings85_8[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings85_9[] = {
	{ Coding::valSpecified, 9},
	{ Coding::eot },
};
bool x64Parser::Opcode85(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings85_8;
	operand.values[9] = OpcodeCodings85_9;
	bool rv;
	{
		rv = Opcode6(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings86_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
bool x64Parser::Opcode86(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings86_8;
	bool rv;
	{
		rv = Opcode7(operand);
	}
	return rv;
}
bool x64Parser::Opcode87(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2321, operand);
	return rv;
}
bool x64Parser::Opcode88(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2327, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings89_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 240, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode89(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings89_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings90_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 225, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode90(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings90_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings91_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings91_26[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings91_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 216, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode91(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings91_8;
	operand.values[26] = OpcodeCodings91_26;
	operand.values[23] = OpcodeCodings91_23;
	bool rv;
	{
		rv = Opcode8(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings92_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings92_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 222, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode92(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings92_8;
	operand.values[9] = OpcodeCodings92_9;
	bool rv;
	{
		rv = Opcode9(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings93_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings93_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 223, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode93(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings93_8;
	operand.values[9] = OpcodeCodings93_9;
	bool rv;
	{
		rv = Opcode11(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings94_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings94_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 223, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode94(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings94_8;
	operand.values[9] = OpcodeCodings94_9;
	bool rv;
	{
		rv = Opcode11(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings95_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 224, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode95(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings95_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings96_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 155, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 219, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 226, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode96(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings96_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings97_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 219, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 226, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode97(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings97_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings98_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings98_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 216, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode98(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings98_8;
	operand.values[23] = OpcodeCodings98_23;
	bool rv = ParseOperands(tokenBranches2340, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings99_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings99_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 216, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode99(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings99_8;
	operand.values[23] = OpcodeCodings99_23;
	bool rv = ParseOperands(tokenBranches2349, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings100_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 222, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode100(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings100_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings101_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode101(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings101_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings102_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 246, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode102(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings102_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings103_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 219, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 225, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode103(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings103_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings104_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings104_26[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings104_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 216, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode104(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings104_8;
	operand.values[26] = OpcodeCodings104_26;
	operand.values[23] = OpcodeCodings104_23;
	bool rv;
	{
		rv = Opcode8(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings105_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings105_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 222, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode105(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings105_8;
	operand.values[9] = OpcodeCodings105_9;
	bool rv;
	{
		rv = Opcode9(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings106_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings106_26[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings106_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 216, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode106(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings106_8;
	operand.values[26] = OpcodeCodings106_26;
	operand.values[23] = OpcodeCodings106_23;
	bool rv;
	{
		rv = Opcode8(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings107_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings107_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 222, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode107(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings107_8;
	operand.values[9] = OpcodeCodings107_9;
	bool rv;
	{
		rv = Opcode9(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings108_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 219, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 224, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode108(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings108_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
bool x64Parser::Opcode109(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2362, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings110_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings110_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 218, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode110(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings110_8;
	operand.values[23] = OpcodeCodings110_23;
	bool rv;
	{
		rv = Opcode10(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings111_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings111_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 218, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode111(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings111_8;
	operand.values[23] = OpcodeCodings111_23;
	bool rv;
	{
		rv = Opcode10(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings112_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings112_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 218, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode112(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings112_8;
	operand.values[23] = OpcodeCodings112_23;
	bool rv;
	{
		rv = Opcode10(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings113_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings113_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 218, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode113(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings113_8;
	operand.values[23] = OpcodeCodings113_23;
	bool rv;
	{
		rv = Opcode10(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings114_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings114_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 218, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode114(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings114_8;
	operand.values[23] = OpcodeCodings114_23;
	bool rv;
	{
		rv = Opcode10(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings115_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings115_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 219, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode115(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings115_8;
	operand.values[23] = OpcodeCodings115_23;
	bool rv = ParseOperands(tokenBranches2369, operand);
	if (!rv)
	{
		rv = Opcode10(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings116_8[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings116_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 218, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode116(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings116_8;
	operand.values[23] = OpcodeCodings116_23;
	bool rv;
	{
		rv = Opcode10(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings117_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 247, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode117(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings117_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings118_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 155, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 219, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 227, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode118(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings118_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings119_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 219, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 227, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode119(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings119_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings120_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings120_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 219, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode120(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings120_8;
	operand.values[23] = OpcodeCodings120_23;
	bool rv;
	{
		rv = Opcode10(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings121_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings121_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 219, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode121(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings121_8;
	operand.values[23] = OpcodeCodings121_23;
	bool rv = ParseOperands(tokenBranches2377, operand);
	if (!rv)
	{
		rv = Opcode10(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings122_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings122_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 218, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode122(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings122_8;
	operand.values[23] = OpcodeCodings122_23;
	bool rv;
	{
		rv = Opcode10(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings123_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings123_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 218, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode123(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings123_8;
	operand.values[23] = OpcodeCodings123_23;
	bool rv;
	{
		rv = Opcode10(operand);
	}
	return rv;
}
bool x64Parser::Opcode124(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2382, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings125_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 232, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode125(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings125_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
bool x64Parser::Opcode126(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2391, operand);
	return rv;
}
bool x64Parser::Opcode127(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2394, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings128_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 234, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode128(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings128_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings129_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 233, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode129(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings129_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings130_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 236, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode130(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings130_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings131_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 237, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode131(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings131_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings132_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 235, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode132(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings132_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings133_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 238, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode133(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings133_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings134_8[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings134_26[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings134_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 216, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode134(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings134_8;
	operand.values[26] = OpcodeCodings134_26;
	operand.values[23] = OpcodeCodings134_23;
	bool rv;
	{
		rv = Opcode8(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings135_8[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings135_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 222, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode135(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings135_8;
	operand.values[9] = OpcodeCodings135_9;
	bool rv;
	{
		rv = Opcode9(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings136_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 208, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode136(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings136_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
bool x64Parser::Opcode137(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2405, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings138_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 243, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode138(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings138_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings139_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 248, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode139(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings139_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings140_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 245, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode140(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings140_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings141_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 242, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode141(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings141_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings142_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 252, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode142(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings142_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
bool x64Parser::Opcode143(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2412, operand);
	return rv;
}
bool x64Parser::Opcode144(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2414, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings145_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 253, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode145(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings145_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings146_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 219, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 228, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode146(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings146_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings147_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 254, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode147(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings147_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings148_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 251, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode148(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings148_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings149_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 250, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode149(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings149_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
bool x64Parser::Opcode150(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2421, operand);
	return rv;
}
bool x64Parser::Opcode151(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2427, operand);
	return rv;
}
bool x64Parser::Opcode152(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2430, operand);
	return rv;
}
bool x64Parser::Opcode153(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2433, operand);
	return rv;
}
bool x64Parser::Opcode154(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2435, operand);
	return rv;
}
bool x64Parser::Opcode155(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2437, operand);
	return rv;
}
bool x64Parser::Opcode156(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2445, operand);
	return rv;
}
bool x64Parser::Opcode157(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2449, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings158_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings158_26[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings158_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 216, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode158(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings158_8;
	operand.values[26] = OpcodeCodings158_26;
	operand.values[23] = OpcodeCodings158_23;
	bool rv;
	{
		rv = Opcode8(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings159_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings159_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 222, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode159(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings159_8;
	operand.values[9] = OpcodeCodings159_9;
	bool rv;
	{
		rv = Opcode9(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings160_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings160_26[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings160_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 216, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode160(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings160_8;
	operand.values[26] = OpcodeCodings160_26;
	operand.values[23] = OpcodeCodings160_23;
	bool rv;
	{
		rv = Opcode8(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings161_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings161_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 222, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode161(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings161_8;
	operand.values[9] = OpcodeCodings161_9;
	bool rv;
	{
		rv = Opcode9(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings162_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 228, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode162(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings162_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings163_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings163_9[] = {
	{ Coding::valSpecified, 221},
	{ Coding::eot },
};
bool x64Parser::Opcode163(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings163_8;
	operand.values[9] = OpcodeCodings163_9;
	bool rv;
	{
		rv = Opcode9(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings164_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings164_9[] = {
	{ Coding::valSpecified, 221},
	{ Coding::eot },
};
bool x64Parser::Opcode164(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings164_8;
	operand.values[9] = OpcodeCodings164_9;
	bool rv;
	{
		rv = Opcode9(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings165_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 218, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 233, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode165(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings165_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings166_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 155, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode166(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings166_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings167_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 229, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode167(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings167_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings168_8[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings168_9[] = {
	{ Coding::valSpecified, 217},
	{ Coding::eot },
};
bool x64Parser::Opcode168(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings168_8;
	operand.values[9] = OpcodeCodings168_9;
	bool rv;
	{
		rv = Opcode9(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings169_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 244, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode169(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings169_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings170_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 241, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode170(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings170_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings171_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 249, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode171(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings171_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings172_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 244, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode172(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings172_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings173_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 167, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode173(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings173_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings174_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
bool x64Parser::Opcode174(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings174_8;
	bool rv;
	{
		rv = Opcode7(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings175_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
bool x64Parser::Opcode175(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings175_8;
	bool rv = ParseOperands(tokenBranches2470, operand);
	if (!rv)
	{
		rv = Opcode7(operand);
	}
	return rv;
}
bool x64Parser::Opcode176(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2521, operand);
	return rv;
}
bool x64Parser::Opcode177(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2543, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings178_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 108, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode178(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings178_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings179_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 109, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode179(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings179_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings180_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 109, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode180(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings180_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings181_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings181_9[] = {
	{ Coding::valSpecified, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode181(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings181_8;
	operand.values[9] = OpcodeCodings181_9;
	bool rv;
	{
		rv = Opcode6(operand);
	}
	return rv;
}
bool x64Parser::Opcode182(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2644, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings183_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 241, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode183(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings183_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings184_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 204, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode184(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings184_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings185_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 206, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode185(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings185_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings186_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 8, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode186(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings186_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
bool x64Parser::Opcode187(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2651, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings188_9[] = {
	{ Coding::valSpecified, 207},
	{ Coding::eot },
};
bool x64Parser::Opcode188(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings188_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings189_9[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::valSpecified, 207},
	{ Coding::eot },
};
bool x64Parser::Opcode189(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings189_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings190_9[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::valSpecified, 207},
	{ Coding::eot },
};
bool x64Parser::Opcode190(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings190_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings191_25[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
bool x64Parser::Opcode191(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings191_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings192_25[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
bool x64Parser::Opcode192(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings192_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings193_25[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
bool x64Parser::Opcode193(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings193_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings194_25[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
bool x64Parser::Opcode194(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings194_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings195_25[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
bool x64Parser::Opcode195(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings195_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
bool x64Parser::Opcode196(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2661, operand);
	return rv;
}
bool x64Parser::Opcode197(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2663, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings198_25[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
bool x64Parser::Opcode198(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings198_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings199_25[] = {
	{ Coding::valSpecified, 15},
	{ Coding::eot },
};
bool x64Parser::Opcode199(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings199_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings200_25[] = {
	{ Coding::valSpecified, 13},
	{ Coding::eot },
};
bool x64Parser::Opcode200(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings200_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings201_25[] = {
	{ Coding::valSpecified, 12},
	{ Coding::eot },
};
bool x64Parser::Opcode201(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings201_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings202_25[] = {
	{ Coding::valSpecified, 14},
	{ Coding::eot },
};
bool x64Parser::Opcode202(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings202_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
bool x64Parser::Opcode203(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2670, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings204_25[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
bool x64Parser::Opcode204(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings204_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings205_25[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
bool x64Parser::Opcode205(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings205_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings206_25[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
bool x64Parser::Opcode206(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings206_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings207_25[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
bool x64Parser::Opcode207(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings207_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings208_25[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
bool x64Parser::Opcode208(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings208_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings209_25[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
bool x64Parser::Opcode209(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings209_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings210_25[] = {
	{ Coding::valSpecified, 14},
	{ Coding::eot },
};
bool x64Parser::Opcode210(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings210_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings211_25[] = {
	{ Coding::valSpecified, 12},
	{ Coding::eot },
};
bool x64Parser::Opcode211(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings211_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings212_25[] = {
	{ Coding::valSpecified, 13},
	{ Coding::eot },
};
bool x64Parser::Opcode212(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings212_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings213_25[] = {
	{ Coding::valSpecified, 15},
	{ Coding::eot },
};
bool x64Parser::Opcode213(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings213_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings214_25[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
bool x64Parser::Opcode214(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings214_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings215_25[] = {
	{ Coding::valSpecified, 11},
	{ Coding::eot },
};
bool x64Parser::Opcode215(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings215_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings216_25[] = {
	{ Coding::valSpecified, 9},
	{ Coding::eot },
};
bool x64Parser::Opcode216(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings216_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings217_25[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
bool x64Parser::Opcode217(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings217_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings218_25[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
bool x64Parser::Opcode218(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings218_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings219_25[] = {
	{ Coding::valSpecified, 10},
	{ Coding::eot },
};
bool x64Parser::Opcode219(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings219_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings220_25[] = {
	{ Coding::valSpecified, 10},
	{ Coding::eot },
};
bool x64Parser::Opcode220(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings220_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings221_25[] = {
	{ Coding::valSpecified, 11},
	{ Coding::eot },
};
bool x64Parser::Opcode221(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings221_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings222_25[] = {
	{ Coding::valSpecified, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode222(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings222_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings223_25[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
bool x64Parser::Opcode223(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings223_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings224_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 159, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode224(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings224_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings225_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode225(x64Operand &operand)
{
	operand.values[23] = OpcodeCodings225_23;
	bool rv;
	{
		rv = Opcode2(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings226_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 197, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode226(x64Operand &operand)
{
	operand.values[23] = OpcodeCodings226_23;
	bool rv;
	{
		rv = Opcode3(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings227_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 141, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode227(x64Operand &operand)
{
	operand.values[23] = OpcodeCodings227_23;
	bool rv;
	{
		rv = Opcode3(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings228_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 201, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode228(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings228_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings229_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 196, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode229(x64Operand &operand)
{
	operand.values[23] = OpcodeCodings229_23;
	bool rv;
	{
		rv = Opcode3(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings230_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 174, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 232, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode230(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings230_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings231_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 180, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode231(x64Operand &operand)
{
	operand.values[23] = OpcodeCodings231_23;
	bool rv;
	{
		rv = Opcode3(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings232_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings232_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode232(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings232_8;
	operand.values[9] = OpcodeCodings232_9;
	bool rv;
	{
		rv = Opcode18(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings233_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 181, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode233(x64Operand &operand)
{
	operand.values[23] = OpcodeCodings233_23;
	bool rv;
	{
		rv = Opcode3(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings234_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings234_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode234(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings234_8;
	operand.values[9] = OpcodeCodings234_9;
	bool rv;
	{
		rv = Opcode18(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings235_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings235_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode235(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings235_8;
	operand.values[9] = OpcodeCodings235_9;
	bool rv = ParseOperands(tokenBranches2718, operand);
	if (!rv)
	{
		rv = Opcode18(operand);
	}
	return rv;
}
bool x64Parser::Opcode236(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2721, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings237_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 172, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode237(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings237_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings238_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 173, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode238(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings238_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings239_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 173, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode239(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings239_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
bool x64Parser::Opcode240(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2731, operand);
	return rv;
}
bool x64Parser::Opcode241(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2733, operand);
	return rv;
}
bool x64Parser::Opcode242(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2735, operand);
	return rv;
}
bool x64Parser::Opcode243(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2737, operand);
	return rv;
}
bool x64Parser::Opcode244(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2739, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings245_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 3, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode245(x64Operand &operand)
{
	operand.values[23] = OpcodeCodings245_23;
	bool rv;
	{
		rv = Opcode2(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings246_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 178, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode246(x64Operand &operand)
{
	operand.values[23] = OpcodeCodings246_23;
	bool rv;
	{
		rv = Opcode3(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings247_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings247_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode247(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings247_8;
	operand.values[9] = OpcodeCodings247_9;
	bool rv = ParseOperands(tokenBranches2743, operand);
	if (!rv)
	{
		rv = Opcode18(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings248_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 174, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 240, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode248(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings248_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
bool x64Parser::Opcode249(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2746, operand);
	return rv;
}
bool x64Parser::Opcode250(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3012, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings251_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 164, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode251(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings251_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings252_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 165, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode252(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings252_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings253_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 165, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode253(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings253_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings254_23[] = {
	{ Coding::valSpecified, 190},
	{ Coding::eot },
};
bool x64Parser::Opcode254(x64Operand &operand)
{
	operand.values[23] = OpcodeCodings254_23;
	bool rv;
	{
		rv = Opcode15(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings255_23[] = {
	{ Coding::valSpecified, 182},
	{ Coding::eot },
};
bool x64Parser::Opcode255(x64Operand &operand)
{
	operand.values[23] = OpcodeCodings255_23;
	bool rv;
	{
		rv = Opcode15(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings256_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
bool x64Parser::Opcode256(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings256_8;
	bool rv;
	{
		rv = Opcode7(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings257_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
bool x64Parser::Opcode257(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings257_8;
	bool rv;
	{
		rv = Opcode7(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings258_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 144, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode258(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings258_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings259_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
bool x64Parser::Opcode259(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings259_8;
	bool rv;
	{
		rv = Opcode7(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings260_8[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings260_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 12, 8},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings260_24[] = {
	{ Coding::valSpecified, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode260(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings260_8;
	operand.values[23] = OpcodeCodings260_23;
	operand.values[24] = OpcodeCodings260_24;
	bool rv;
	{
		rv = Opcode4(operand);
	}
	return rv;
}
bool x64Parser::Opcode261(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3049, operand);
	return rv;
}
bool x64Parser::Opcode262(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3071, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings263_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 110, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode263(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings263_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings264_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 111, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode264(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings264_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings265_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 111, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode265(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings265_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
bool x64Parser::Opcode266(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3171, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings267_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 97, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode267(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings267_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings268_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 97, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode268(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings268_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings269_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 97, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode269(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings269_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings270_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 157, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode270(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings270_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings271_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 157, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode271(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings271_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings272_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 157, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode272(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings272_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
bool x64Parser::Opcode273(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3182, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings274_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 96, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode274(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings274_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings275_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 96, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode275(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings275_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings276_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 96, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode276(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings276_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings277_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 156, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode277(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings277_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings278_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 156, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode278(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings278_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings279_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 156, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode279(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings279_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings280_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
bool x64Parser::Opcode280(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings280_8;
	bool rv;
	{
		rv = Opcode16(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings281_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
bool x64Parser::Opcode281(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings281_8;
	bool rv;
	{
		rv = Opcode16(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings282_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 50, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode282(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings282_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings283_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 51, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode283(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings283_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings284_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 49, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode284(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings284_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
bool x64Parser::Opcode285(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3211, operand);
	return rv;
}
bool x64Parser::Opcode286(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3214, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings287_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
bool x64Parser::Opcode287(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings287_8;
	bool rv;
	{
		rv = Opcode16(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings288_8[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
bool x64Parser::Opcode288(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings288_8;
	bool rv;
	{
		rv = Opcode16(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings289_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 170, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode289(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings289_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings290_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 158, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode290(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings290_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings291_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
bool x64Parser::Opcode291(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings291_8;
	bool rv;
	{
		rv = Opcode16(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings292_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
bool x64Parser::Opcode292(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings292_8;
	bool rv;
	{
		rv = Opcode16(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings293_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings293_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 28, 8},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings293_24[] = {
	{ Coding::valSpecified, 24},
	{ Coding::eot },
};
bool x64Parser::Opcode293(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings293_8;
	operand.values[23] = OpcodeCodings293_23;
	operand.values[24] = OpcodeCodings293_24;
	bool rv;
	{
		rv = Opcode4(operand);
	}
	return rv;
}
bool x64Parser::Opcode294(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3224, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings295_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 174, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode295(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings295_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings296_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 175, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode296(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings296_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings297_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 175, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode297(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings297_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings298_25[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
bool x64Parser::Opcode298(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings298_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings299_25[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
bool x64Parser::Opcode299(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings299_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings300_25[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
bool x64Parser::Opcode300(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings300_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings301_25[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
bool x64Parser::Opcode301(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings301_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings302_25[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
bool x64Parser::Opcode302(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings302_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings303_25[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
bool x64Parser::Opcode303(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings303_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings304_25[] = {
	{ Coding::valSpecified, 15},
	{ Coding::eot },
};
bool x64Parser::Opcode304(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings304_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings305_25[] = {
	{ Coding::valSpecified, 13},
	{ Coding::eot },
};
bool x64Parser::Opcode305(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings305_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings306_25[] = {
	{ Coding::valSpecified, 12},
	{ Coding::eot },
};
bool x64Parser::Opcode306(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings306_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings307_25[] = {
	{ Coding::valSpecified, 14},
	{ Coding::eot },
};
bool x64Parser::Opcode307(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings307_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings308_25[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
bool x64Parser::Opcode308(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings308_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings309_25[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
bool x64Parser::Opcode309(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings309_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings310_25[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
bool x64Parser::Opcode310(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings310_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings311_25[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
bool x64Parser::Opcode311(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings311_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings312_25[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
bool x64Parser::Opcode312(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings312_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings313_25[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
bool x64Parser::Opcode313(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings313_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings314_25[] = {
	{ Coding::valSpecified, 14},
	{ Coding::eot },
};
bool x64Parser::Opcode314(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings314_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings315_25[] = {
	{ Coding::valSpecified, 12},
	{ Coding::eot },
};
bool x64Parser::Opcode315(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings315_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings316_25[] = {
	{ Coding::valSpecified, 13},
	{ Coding::eot },
};
bool x64Parser::Opcode316(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings316_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings317_25[] = {
	{ Coding::valSpecified, 15},
	{ Coding::eot },
};
bool x64Parser::Opcode317(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings317_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings318_25[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
bool x64Parser::Opcode318(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings318_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings319_25[] = {
	{ Coding::valSpecified, 11},
	{ Coding::eot },
};
bool x64Parser::Opcode319(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings319_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings320_25[] = {
	{ Coding::valSpecified, 9},
	{ Coding::eot },
};
bool x64Parser::Opcode320(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings320_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings321_25[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
bool x64Parser::Opcode321(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings321_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings322_25[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
bool x64Parser::Opcode322(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings322_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings323_25[] = {
	{ Coding::valSpecified, 10},
	{ Coding::eot },
};
bool x64Parser::Opcode323(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings323_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings324_25[] = {
	{ Coding::valSpecified, 10},
	{ Coding::eot },
};
bool x64Parser::Opcode324(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings324_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings325_25[] = {
	{ Coding::valSpecified, 11},
	{ Coding::eot },
};
bool x64Parser::Opcode325(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings325_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings326_25[] = {
	{ Coding::valSpecified, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode326(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings326_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings327_25[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
bool x64Parser::Opcode327(x64Operand &operand)
{
	operand.values[25] = OpcodeCodings327_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings328_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 174, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 248, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode328(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings328_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings329_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
bool x64Parser::Opcode329(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings329_8;
	bool rv;
	{
		rv = Opcode16(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings330_23[] = {
	{ Coding::valSpecified, 164},
	{ Coding::eot },
};
bool x64Parser::Opcode330(x64Operand &operand)
{
	operand.values[23] = OpcodeCodings330_23;
	bool rv;
	{
		rv = Opcode17(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings331_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
bool x64Parser::Opcode331(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings331_8;
	bool rv;
	{
		rv = Opcode16(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings332_23[] = {
	{ Coding::valSpecified, 172},
	{ Coding::eot },
};
bool x64Parser::Opcode332(x64Operand &operand)
{
	operand.values[23] = OpcodeCodings332_23;
	bool rv;
	{
		rv = Opcode17(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings333_8[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings333_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode333(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings333_8;
	operand.values[9] = OpcodeCodings333_9;
	bool rv;
	{
		rv = Opcode18(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings334_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings334_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode334(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings334_8;
	operand.values[9] = OpcodeCodings334_9;
	bool rv;
	{
		rv = Opcode18(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings335_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings335_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode335(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings335_8;
	operand.values[9] = OpcodeCodings335_9;
	bool rv = ParseOperands(tokenBranches3337, operand);
	if (!rv)
	{
		rv = Opcode18(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings336_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 249, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode336(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings336_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings337_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 253, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode337(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings337_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings338_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 251, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode338(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings338_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
bool x64Parser::Opcode339(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3343, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings340_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 170, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode340(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings340_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings341_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 171, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode341(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings341_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings342_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 171, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode342(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings342_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings343_8[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings343_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode343(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings343_8;
	operand.values[9] = OpcodeCodings343_9;
	bool rv = ParseOperands(tokenBranches3365, operand);
	if (!rv)
	{
		rv = Opcode18(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings344_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings344_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 44, 8},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings344_24[] = {
	{ Coding::valSpecified, 40},
	{ Coding::eot },
};
bool x64Parser::Opcode344(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings344_8;
	operand.values[23] = OpcodeCodings344_23;
	operand.values[24] = OpcodeCodings344_24;
	bool rv;
	{
		rv = Opcode4(operand);
	}
	return rv;
}
bool x64Parser::Opcode345(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3369, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings346_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 11, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode346(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings346_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings347_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings347_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode347(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings347_8;
	operand.values[9] = OpcodeCodings347_9;
	bool rv = ParseOperands(tokenBranches3428, operand);
	if (!rv)
	{
		rv = Opcode18(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings348_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings348_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode348(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings348_8;
	operand.values[9] = OpcodeCodings348_9;
	bool rv = ParseOperands(tokenBranches3431, operand);
	if (!rv)
	{
		rv = Opcode18(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings349_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 155, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode349(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings349_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings350_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 9, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode350(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings350_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings351_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 48, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode351(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings351_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
bool x64Parser::Opcode352(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3437, operand);
	return rv;
}
bool x64Parser::Opcode353(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3442, operand);
	return rv;
}
bool x64Parser::Opcode354(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3451, operand);
	return rv;
}
bool x64Parser::Opcode355(x64Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3458, operand);
	return rv;
}
Coding x64Parser::OpcodeCodings356_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 215, 8},
	{ Coding::eot },
};
bool x64Parser::Opcode356(x64Operand &operand)
{
	operand.values[9] = OpcodeCodings356_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x64Parser::OpcodeCodings357_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings357_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 52, 8},
	{ Coding::eot },
};
Coding x64Parser::OpcodeCodings357_24[] = {
	{ Coding::valSpecified, 48},
	{ Coding::eot },
};
bool x64Parser::Opcode357(x64Operand &operand)
{
	operand.values[8] = OpcodeCodings357_8;
	operand.values[23] = OpcodeCodings357_23;
	operand.values[24] = OpcodeCodings357_24;
	bool rv;
	{
		rv = Opcode4(operand);
	}
	return rv;
}
x64Parser::DispatchType x64Parser::DispatchTable[358] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&x64Parser::Opcode19,
	&x64Parser::Opcode20,
	&x64Parser::Opcode21,
	&x64Parser::Opcode22,
	&x64Parser::Opcode23,
	&x64Parser::Opcode24,
	&x64Parser::Opcode25,
	&x64Parser::Opcode26,
	&x64Parser::Opcode27,
	&x64Parser::Opcode28,
	&x64Parser::Opcode29,
	&x64Parser::Opcode30,
	&x64Parser::Opcode31,
	&x64Parser::Opcode32,
	&x64Parser::Opcode33,
	&x64Parser::Opcode34,
	&x64Parser::Opcode35,
	&x64Parser::Opcode36,
	&x64Parser::Opcode37,
	&x64Parser::Opcode38,
	&x64Parser::Opcode39,
	&x64Parser::Opcode40,
	&x64Parser::Opcode41,
	&x64Parser::Opcode42,
	&x64Parser::Opcode43,
	&x64Parser::Opcode44,
	&x64Parser::Opcode45,
	&x64Parser::Opcode46,
	&x64Parser::Opcode47,
	&x64Parser::Opcode48,
	&x64Parser::Opcode49,
	&x64Parser::Opcode50,
	&x64Parser::Opcode51,
	&x64Parser::Opcode52,
	&x64Parser::Opcode53,
	&x64Parser::Opcode54,
	&x64Parser::Opcode55,
	&x64Parser::Opcode56,
	&x64Parser::Opcode57,
	&x64Parser::Opcode58,
	&x64Parser::Opcode59,
	&x64Parser::Opcode60,
	&x64Parser::Opcode61,
	&x64Parser::Opcode62,
	&x64Parser::Opcode63,
	&x64Parser::Opcode64,
	&x64Parser::Opcode65,
	&x64Parser::Opcode66,
	&x64Parser::Opcode67,
	&x64Parser::Opcode68,
	&x64Parser::Opcode69,
	&x64Parser::Opcode70,
	&x64Parser::Opcode71,
	&x64Parser::Opcode72,
	&x64Parser::Opcode73,
	&x64Parser::Opcode74,
	&x64Parser::Opcode75,
	&x64Parser::Opcode76,
	&x64Parser::Opcode77,
	&x64Parser::Opcode78,
	&x64Parser::Opcode79,
	&x64Parser::Opcode80,
	&x64Parser::Opcode81,
	&x64Parser::Opcode82,
	&x64Parser::Opcode83,
	&x64Parser::Opcode84,
	&x64Parser::Opcode85,
	&x64Parser::Opcode86,
	&x64Parser::Opcode87,
	&x64Parser::Opcode88,
	&x64Parser::Opcode89,
	&x64Parser::Opcode90,
	&x64Parser::Opcode91,
	&x64Parser::Opcode92,
	&x64Parser::Opcode93,
	&x64Parser::Opcode94,
	&x64Parser::Opcode95,
	&x64Parser::Opcode96,
	&x64Parser::Opcode97,
	&x64Parser::Opcode98,
	&x64Parser::Opcode99,
	&x64Parser::Opcode100,
	&x64Parser::Opcode101,
	&x64Parser::Opcode102,
	&x64Parser::Opcode103,
	&x64Parser::Opcode104,
	&x64Parser::Opcode105,
	&x64Parser::Opcode106,
	&x64Parser::Opcode107,
	&x64Parser::Opcode108,
	&x64Parser::Opcode109,
	&x64Parser::Opcode110,
	&x64Parser::Opcode111,
	&x64Parser::Opcode112,
	&x64Parser::Opcode113,
	&x64Parser::Opcode114,
	&x64Parser::Opcode115,
	&x64Parser::Opcode116,
	&x64Parser::Opcode117,
	&x64Parser::Opcode118,
	&x64Parser::Opcode119,
	&x64Parser::Opcode120,
	&x64Parser::Opcode121,
	&x64Parser::Opcode122,
	&x64Parser::Opcode123,
	&x64Parser::Opcode124,
	&x64Parser::Opcode125,
	&x64Parser::Opcode126,
	&x64Parser::Opcode127,
	&x64Parser::Opcode128,
	&x64Parser::Opcode129,
	&x64Parser::Opcode130,
	&x64Parser::Opcode131,
	&x64Parser::Opcode132,
	&x64Parser::Opcode133,
	&x64Parser::Opcode134,
	&x64Parser::Opcode135,
	&x64Parser::Opcode136,
	&x64Parser::Opcode137,
	&x64Parser::Opcode138,
	&x64Parser::Opcode139,
	&x64Parser::Opcode140,
	&x64Parser::Opcode141,
	&x64Parser::Opcode142,
	&x64Parser::Opcode143,
	&x64Parser::Opcode144,
	&x64Parser::Opcode145,
	&x64Parser::Opcode146,
	&x64Parser::Opcode147,
	&x64Parser::Opcode148,
	&x64Parser::Opcode149,
	&x64Parser::Opcode150,
	&x64Parser::Opcode151,
	&x64Parser::Opcode152,
	&x64Parser::Opcode153,
	&x64Parser::Opcode154,
	&x64Parser::Opcode155,
	&x64Parser::Opcode156,
	&x64Parser::Opcode157,
	&x64Parser::Opcode158,
	&x64Parser::Opcode159,
	&x64Parser::Opcode160,
	&x64Parser::Opcode161,
	&x64Parser::Opcode162,
	&x64Parser::Opcode163,
	&x64Parser::Opcode164,
	&x64Parser::Opcode165,
	&x64Parser::Opcode166,
	&x64Parser::Opcode167,
	&x64Parser::Opcode168,
	&x64Parser::Opcode169,
	&x64Parser::Opcode170,
	&x64Parser::Opcode171,
	&x64Parser::Opcode172,
	&x64Parser::Opcode173,
	&x64Parser::Opcode174,
	&x64Parser::Opcode175,
	&x64Parser::Opcode176,
	&x64Parser::Opcode177,
	&x64Parser::Opcode178,
	&x64Parser::Opcode179,
	&x64Parser::Opcode180,
	&x64Parser::Opcode181,
	&x64Parser::Opcode182,
	&x64Parser::Opcode183,
	&x64Parser::Opcode184,
	&x64Parser::Opcode185,
	&x64Parser::Opcode186,
	&x64Parser::Opcode187,
	&x64Parser::Opcode188,
	&x64Parser::Opcode189,
	&x64Parser::Opcode190,
	&x64Parser::Opcode191,
	&x64Parser::Opcode192,
	&x64Parser::Opcode193,
	&x64Parser::Opcode194,
	&x64Parser::Opcode195,
	&x64Parser::Opcode196,
	&x64Parser::Opcode197,
	&x64Parser::Opcode198,
	&x64Parser::Opcode199,
	&x64Parser::Opcode200,
	&x64Parser::Opcode201,
	&x64Parser::Opcode202,
	&x64Parser::Opcode203,
	&x64Parser::Opcode204,
	&x64Parser::Opcode205,
	&x64Parser::Opcode206,
	&x64Parser::Opcode207,
	&x64Parser::Opcode208,
	&x64Parser::Opcode209,
	&x64Parser::Opcode210,
	&x64Parser::Opcode211,
	&x64Parser::Opcode212,
	&x64Parser::Opcode213,
	&x64Parser::Opcode214,
	&x64Parser::Opcode215,
	&x64Parser::Opcode216,
	&x64Parser::Opcode217,
	&x64Parser::Opcode218,
	&x64Parser::Opcode219,
	&x64Parser::Opcode220,
	&x64Parser::Opcode221,
	&x64Parser::Opcode222,
	&x64Parser::Opcode223,
	&x64Parser::Opcode224,
	&x64Parser::Opcode225,
	&x64Parser::Opcode226,
	&x64Parser::Opcode227,
	&x64Parser::Opcode228,
	&x64Parser::Opcode229,
	&x64Parser::Opcode230,
	&x64Parser::Opcode231,
	&x64Parser::Opcode232,
	&x64Parser::Opcode233,
	&x64Parser::Opcode234,
	&x64Parser::Opcode235,
	&x64Parser::Opcode236,
	&x64Parser::Opcode237,
	&x64Parser::Opcode238,
	&x64Parser::Opcode239,
	&x64Parser::Opcode240,
	&x64Parser::Opcode241,
	&x64Parser::Opcode242,
	&x64Parser::Opcode243,
	&x64Parser::Opcode244,
	&x64Parser::Opcode245,
	&x64Parser::Opcode246,
	&x64Parser::Opcode247,
	&x64Parser::Opcode248,
	&x64Parser::Opcode249,
	&x64Parser::Opcode250,
	&x64Parser::Opcode251,
	&x64Parser::Opcode252,
	&x64Parser::Opcode253,
	&x64Parser::Opcode254,
	&x64Parser::Opcode255,
	&x64Parser::Opcode256,
	&x64Parser::Opcode257,
	&x64Parser::Opcode258,
	&x64Parser::Opcode259,
	&x64Parser::Opcode260,
	&x64Parser::Opcode261,
	&x64Parser::Opcode262,
	&x64Parser::Opcode263,
	&x64Parser::Opcode264,
	&x64Parser::Opcode265,
	&x64Parser::Opcode266,
	&x64Parser::Opcode267,
	&x64Parser::Opcode268,
	&x64Parser::Opcode269,
	&x64Parser::Opcode270,
	&x64Parser::Opcode271,
	&x64Parser::Opcode272,
	&x64Parser::Opcode273,
	&x64Parser::Opcode274,
	&x64Parser::Opcode275,
	&x64Parser::Opcode276,
	&x64Parser::Opcode277,
	&x64Parser::Opcode278,
	&x64Parser::Opcode279,
	&x64Parser::Opcode280,
	&x64Parser::Opcode281,
	&x64Parser::Opcode282,
	&x64Parser::Opcode283,
	&x64Parser::Opcode284,
	&x64Parser::Opcode285,
	&x64Parser::Opcode286,
	&x64Parser::Opcode287,
	&x64Parser::Opcode288,
	&x64Parser::Opcode289,
	&x64Parser::Opcode290,
	&x64Parser::Opcode291,
	&x64Parser::Opcode292,
	&x64Parser::Opcode293,
	&x64Parser::Opcode294,
	&x64Parser::Opcode295,
	&x64Parser::Opcode296,
	&x64Parser::Opcode297,
	&x64Parser::Opcode298,
	&x64Parser::Opcode299,
	&x64Parser::Opcode300,
	&x64Parser::Opcode301,
	&x64Parser::Opcode302,
	&x64Parser::Opcode303,
	&x64Parser::Opcode304,
	&x64Parser::Opcode305,
	&x64Parser::Opcode306,
	&x64Parser::Opcode307,
	&x64Parser::Opcode308,
	&x64Parser::Opcode309,
	&x64Parser::Opcode310,
	&x64Parser::Opcode311,
	&x64Parser::Opcode312,
	&x64Parser::Opcode313,
	&x64Parser::Opcode314,
	&x64Parser::Opcode315,
	&x64Parser::Opcode316,
	&x64Parser::Opcode317,
	&x64Parser::Opcode318,
	&x64Parser::Opcode319,
	&x64Parser::Opcode320,
	&x64Parser::Opcode321,
	&x64Parser::Opcode322,
	&x64Parser::Opcode323,
	&x64Parser::Opcode324,
	&x64Parser::Opcode325,
	&x64Parser::Opcode326,
	&x64Parser::Opcode327,
	&x64Parser::Opcode328,
	&x64Parser::Opcode329,
	&x64Parser::Opcode330,
	&x64Parser::Opcode331,
	&x64Parser::Opcode332,
	&x64Parser::Opcode333,
	&x64Parser::Opcode334,
	&x64Parser::Opcode335,
	&x64Parser::Opcode336,
	&x64Parser::Opcode337,
	&x64Parser::Opcode338,
	&x64Parser::Opcode339,
	&x64Parser::Opcode340,
	&x64Parser::Opcode341,
	&x64Parser::Opcode342,
	&x64Parser::Opcode343,
	&x64Parser::Opcode344,
	&x64Parser::Opcode345,
	&x64Parser::Opcode346,
	&x64Parser::Opcode347,
	&x64Parser::Opcode348,
	&x64Parser::Opcode349,
	&x64Parser::Opcode350,
	&x64Parser::Opcode351,
	&x64Parser::Opcode352,
	&x64Parser::Opcode353,
	&x64Parser::Opcode354,
	&x64Parser::Opcode355,
	&x64Parser::Opcode356,
	&x64Parser::Opcode357,
};
Coding x64Parser::Coding1[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding2[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding3[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding4[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding5[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ Coding::eot },
};
Coding x64Parser::Coding6[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ Coding::eot },
};
Coding x64Parser::Coding7[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ Coding::eot },
};
Coding x64Parser::Coding8[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ Coding::eot },
};
Coding x64Parser::Coding9[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ Coding::eot },
};
Coding x64Parser::Coding10[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ Coding::eot },
};
Coding x64Parser::Coding11[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 2, 43, 4 },
	{ Coding::eot },
};
Coding x64Parser::Coding12[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 2, 43, 4 },
	{ Coding::eot },
};
Coding x64Parser::Coding13[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 2, 43, 4 },
	{ Coding::eot },
};
Coding x64Parser::Coding14[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 7, 3},
	{ Coding::eot },
};
Coding x64Parser::Coding15[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 7, 3},
	{ Coding::eot },
};
Coding x64Parser::Coding16[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 7, 3},
	{ Coding::eot },
};
Coding x64Parser::Coding17[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding18[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding19[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding20[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding21[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding22[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding23[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding24[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 11, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding25[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding26[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 2, 43, 4 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding27[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 2, 43, 4 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding28[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 2, 43, 4 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding29[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 7, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding30[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::valSpecified, 54},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 7, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding31[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 7, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding32[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding33[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::valSpecified, 62},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding34[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding35[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding36[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding37[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding38[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding39[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding40[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding41[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 2, 43, 4 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding42[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 2, 43, 4 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding43[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 2, 43, 4 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding44[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 7, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding45[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 7, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding46[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 7, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding47[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding48[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding49[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding50[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ Coding::eot },
};
Coding x64Parser::Coding51[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding52[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding53[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding54[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ Coding::eot },
};
Coding x64Parser::Coding55[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding56[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ Coding::eot },
};
Coding x64Parser::Coding57[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding58[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding59[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding60[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding61[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding62[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding63[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding64[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding65[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding66[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding67[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 3, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding68[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding69[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding70[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding71[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding72[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding73[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding74[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding75[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding76[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding77[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding78[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding79[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding80[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding81[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding82[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding83[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 32},
	{ Coding::eot },
};
Coding x64Parser::Coding84[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding85[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 32},
	{ Coding::eot },
};
Coding x64Parser::Coding86[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding87[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 32},
	{ Coding::eot },
};
Coding x64Parser::Coding88[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding89[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 32},
	{ Coding::eot },
};
Coding x64Parser::Coding90[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding91[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 32},
	{ Coding::eot },
};
Coding x64Parser::Coding92[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding93[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 32},
	{ Coding::eot },
};
Coding x64Parser::Coding94[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding95[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding96[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding97[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ Coding::eot },
};
Coding x64Parser::Coding98[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ Coding::eot },
};
Coding x64Parser::Coding99[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding100[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding101[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding102[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding103[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding104[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding105[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ Coding::eot },
};
Coding x64Parser::Coding106[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ Coding::eot },
};
Coding x64Parser::Coding107[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding108[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding109[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ Coding::eot },
};
Coding x64Parser::Coding110[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ Coding::eot },
};
Coding x64Parser::Coding111[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding112[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding113[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding114[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding115[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding116[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding117[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding118[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding119[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding120[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding121[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding122[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding123[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding124[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding125[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding126[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding127[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding128[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding129[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding130[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding131[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding132[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding133[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding134[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding135[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding136[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding137[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding138[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding139[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding140[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding141[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding142[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding143[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding144[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding145[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding146[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding147[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding148[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding149[] = {
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 3, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 7, 3, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding150[] = {
	{ Coding::indirect, 9, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding151[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding152[] = {
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding153[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0, 43, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding154[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0, 43, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding155[] = {
	{ Coding::native },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding156[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::native },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 13, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding157[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::native },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding158[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::native },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 13, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding159[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::native },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding160[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::native },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding161[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::native },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding162[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 9, 5, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding163[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 9, 5, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding164[] = {
	{ Coding::indirect, 23, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 3, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 7, 3, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding165[] = {
	{ Coding::indirect, 23, -1, 0, 43, 4 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 3, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 26, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 7, 3, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding166[] = {
	{ Coding::indirect, 23, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 3, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 26, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 7, 3, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding167[] = {
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 3, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 3},
	{ Coding::eot },
};
Coding x64Parser::Coding168[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 7, 4},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 25, 4, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 14, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding169[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::valSpecified, 15},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 8, 4},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 25, 4, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 14, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding170[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::valSpecified, 15},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 8, 4},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 25, 4, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 14, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding171[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 25, 5},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding172[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 154, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 1, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding173[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 154, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 1, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding174[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 232, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 14, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding175[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 232, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 14, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding176[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 154, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 16},
	{ Coding::eot },
};
Coding x64Parser::Coding177[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 154, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 16},
	{ Coding::eot },
};
Coding x64Parser::Coding178[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 166, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding179[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 166, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding180[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 166, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding181[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 166, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding182[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 167, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding183[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 167, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding184[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 167, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding185[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 167, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding186[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 167, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding187[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 167, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding188[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 167, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding189[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 167, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding190[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 200, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 16, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding191[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 155, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 223, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 224, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding192[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 223, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 224, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding193[] = {
	{ Coding::native },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 17, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding194[] = {
	{ Coding::native },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding195[] = {
	{ Coding::native },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding196[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 107, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 3, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 13, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding197[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 105, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 3, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding198[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 107, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 3, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 13, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding199[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 105, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 3, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding200[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 228, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 18, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding201[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 229, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 18, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding202[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 229, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 18, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding203[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 236, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding204[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 237, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding205[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 237, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding206[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 108, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding207[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 108, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding208[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 108, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding209[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 108, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding210[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 109, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding211[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 109, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding212[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 109, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding213[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 109, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding214[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 109, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding215[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 109, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding216[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 109, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding217[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 109, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding218[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 205, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding219[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 227, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 14, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding220[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 227, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 14, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding221[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 234, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 1, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding222[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 234, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 1, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding223[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 234, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 16},
	{ Coding::eot },
};
Coding x64Parser::Coding224[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 234, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 16},
	{ Coding::eot },
};
Coding x64Parser::Coding225[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 172, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding226[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 226, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 14, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding227[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 225, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 14, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding228[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 224, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 14, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding229[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 160, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding230[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 160, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding231[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 160, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding232[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 160, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding233[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 161, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding234[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 161, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding235[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 161, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding236[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 161, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding237[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 161, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding238[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 161, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding239[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 161, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding240[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 161, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding241[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 22, 5},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding242[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 23, 5},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding243[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 23, 5},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding244[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 162, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding245[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 162, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding246[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 162, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding247[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 162, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding248[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 163, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding249[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 163, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding250[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 163, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding251[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 163, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding252[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 163, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding253[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 163, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding254[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 163, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding255[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 163, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding256[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 164, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding257[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 164, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding258[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 230, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 18, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding259[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 231, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 18, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding260[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 231, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 18, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding261[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 238, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding262[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 239, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding263[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 239, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding264[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 110, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding265[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 110, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding266[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 110, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding267[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 110, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding268[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 111, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding269[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 111, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding270[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 111, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding271[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 111, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding272[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 111, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding273[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 111, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding274[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 111, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding275[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 111, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding276[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 11, 5},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding277[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 11, 5},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding278[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 10, 5},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding279[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 10, 5},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding280[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 106, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding281[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 104, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding282[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 104, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding283[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::native },
	{ Coding::eot },
};
Coding x64Parser::Coding284[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::native },
	{ Coding::eot },
};
Coding x64Parser::Coding285[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 160, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding286[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 168, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding287[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ Coding::eot },
};
Coding x64Parser::Coding288[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 194, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding289[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 195, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding290[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 202, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding291[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 203, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding292[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 174, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding293[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 174, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding294[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 174, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding295[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 174, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding296[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 175, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding297[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 175, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding298[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 175, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding299[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 175, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding300[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 175, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding301[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 175, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding302[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 175, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding303[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 175, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding304[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 170, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding305[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 170, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding306[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 170, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding307[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 168, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 8, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding308[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 169, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding309[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 169, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding310[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 18, 5},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding311[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 18, 5},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ Coding::eot },
};
Coding x64Parser::Coding312[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 215, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding313[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 1, -1, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 215, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding314[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 215, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding315[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 1, -1, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 215, 8},
	{ Coding::eot },
};
Coding x64Parser::Coding316[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 215, 8},
	{ Coding::eot },
};
Coding * x64Parser::Codings[316] = {
	x64Parser::Coding1,
	x64Parser::Coding2,
	x64Parser::Coding3,
	x64Parser::Coding4,
	x64Parser::Coding5,
	x64Parser::Coding6,
	x64Parser::Coding7,
	x64Parser::Coding8,
	x64Parser::Coding9,
	x64Parser::Coding10,
	x64Parser::Coding11,
	x64Parser::Coding12,
	x64Parser::Coding13,
	x64Parser::Coding14,
	x64Parser::Coding15,
	x64Parser::Coding16,
	x64Parser::Coding17,
	x64Parser::Coding18,
	x64Parser::Coding19,
	x64Parser::Coding20,
	x64Parser::Coding21,
	x64Parser::Coding22,
	x64Parser::Coding23,
	x64Parser::Coding24,
	x64Parser::Coding25,
	x64Parser::Coding26,
	x64Parser::Coding27,
	x64Parser::Coding28,
	x64Parser::Coding29,
	x64Parser::Coding30,
	x64Parser::Coding31,
	x64Parser::Coding32,
	x64Parser::Coding33,
	x64Parser::Coding34,
	x64Parser::Coding35,
	x64Parser::Coding36,
	x64Parser::Coding37,
	x64Parser::Coding38,
	x64Parser::Coding39,
	x64Parser::Coding40,
	x64Parser::Coding41,
	x64Parser::Coding42,
	x64Parser::Coding43,
	x64Parser::Coding44,
	x64Parser::Coding45,
	x64Parser::Coding46,
	x64Parser::Coding47,
	x64Parser::Coding48,
	x64Parser::Coding49,
	x64Parser::Coding50,
	x64Parser::Coding51,
	x64Parser::Coding52,
	x64Parser::Coding53,
	x64Parser::Coding54,
	x64Parser::Coding55,
	x64Parser::Coding56,
	x64Parser::Coding57,
	x64Parser::Coding58,
	x64Parser::Coding59,
	x64Parser::Coding60,
	x64Parser::Coding61,
	x64Parser::Coding62,
	x64Parser::Coding63,
	x64Parser::Coding64,
	x64Parser::Coding65,
	x64Parser::Coding66,
	x64Parser::Coding67,
	x64Parser::Coding68,
	x64Parser::Coding69,
	x64Parser::Coding70,
	x64Parser::Coding71,
	x64Parser::Coding72,
	x64Parser::Coding73,
	x64Parser::Coding74,
	x64Parser::Coding75,
	x64Parser::Coding76,
	x64Parser::Coding77,
	x64Parser::Coding78,
	x64Parser::Coding79,
	x64Parser::Coding80,
	x64Parser::Coding81,
	x64Parser::Coding82,
	x64Parser::Coding83,
	x64Parser::Coding84,
	x64Parser::Coding85,
	x64Parser::Coding86,
	x64Parser::Coding87,
	x64Parser::Coding88,
	x64Parser::Coding89,
	x64Parser::Coding90,
	x64Parser::Coding91,
	x64Parser::Coding92,
	x64Parser::Coding93,
	x64Parser::Coding94,
	x64Parser::Coding95,
	x64Parser::Coding96,
	x64Parser::Coding97,
	x64Parser::Coding98,
	x64Parser::Coding99,
	x64Parser::Coding100,
	x64Parser::Coding101,
	x64Parser::Coding102,
	x64Parser::Coding103,
	x64Parser::Coding104,
	x64Parser::Coding105,
	x64Parser::Coding106,
	x64Parser::Coding107,
	x64Parser::Coding108,
	x64Parser::Coding109,
	x64Parser::Coding110,
	x64Parser::Coding111,
	x64Parser::Coding112,
	x64Parser::Coding113,
	x64Parser::Coding114,
	x64Parser::Coding115,
	x64Parser::Coding116,
	x64Parser::Coding117,
	x64Parser::Coding118,
	x64Parser::Coding119,
	x64Parser::Coding120,
	x64Parser::Coding121,
	x64Parser::Coding122,
	x64Parser::Coding123,
	x64Parser::Coding124,
	x64Parser::Coding125,
	x64Parser::Coding126,
	x64Parser::Coding127,
	x64Parser::Coding128,
	x64Parser::Coding129,
	x64Parser::Coding130,
	x64Parser::Coding131,
	x64Parser::Coding132,
	x64Parser::Coding133,
	x64Parser::Coding134,
	x64Parser::Coding135,
	x64Parser::Coding136,
	x64Parser::Coding137,
	x64Parser::Coding138,
	x64Parser::Coding139,
	x64Parser::Coding140,
	x64Parser::Coding141,
	x64Parser::Coding142,
	x64Parser::Coding143,
	x64Parser::Coding144,
	x64Parser::Coding145,
	x64Parser::Coding146,
	x64Parser::Coding147,
	x64Parser::Coding148,
	x64Parser::Coding149,
	x64Parser::Coding150,
	x64Parser::Coding151,
	x64Parser::Coding152,
	x64Parser::Coding153,
	x64Parser::Coding154,
	x64Parser::Coding155,
	x64Parser::Coding156,
	x64Parser::Coding157,
	x64Parser::Coding158,
	x64Parser::Coding159,
	x64Parser::Coding160,
	x64Parser::Coding161,
	x64Parser::Coding162,
	x64Parser::Coding163,
	x64Parser::Coding164,
	x64Parser::Coding165,
	x64Parser::Coding166,
	x64Parser::Coding167,
	x64Parser::Coding168,
	x64Parser::Coding169,
	x64Parser::Coding170,
	x64Parser::Coding171,
	x64Parser::Coding172,
	x64Parser::Coding173,
	x64Parser::Coding174,
	x64Parser::Coding175,
	x64Parser::Coding176,
	x64Parser::Coding177,
	x64Parser::Coding178,
	x64Parser::Coding179,
	x64Parser::Coding180,
	x64Parser::Coding181,
	x64Parser::Coding182,
	x64Parser::Coding183,
	x64Parser::Coding184,
	x64Parser::Coding185,
	x64Parser::Coding186,
	x64Parser::Coding187,
	x64Parser::Coding188,
	x64Parser::Coding189,
	x64Parser::Coding190,
	x64Parser::Coding191,
	x64Parser::Coding192,
	x64Parser::Coding193,
	x64Parser::Coding194,
	x64Parser::Coding195,
	x64Parser::Coding196,
	x64Parser::Coding197,
	x64Parser::Coding198,
	x64Parser::Coding199,
	x64Parser::Coding200,
	x64Parser::Coding201,
	x64Parser::Coding202,
	x64Parser::Coding203,
	x64Parser::Coding204,
	x64Parser::Coding205,
	x64Parser::Coding206,
	x64Parser::Coding207,
	x64Parser::Coding208,
	x64Parser::Coding209,
	x64Parser::Coding210,
	x64Parser::Coding211,
	x64Parser::Coding212,
	x64Parser::Coding213,
	x64Parser::Coding214,
	x64Parser::Coding215,
	x64Parser::Coding216,
	x64Parser::Coding217,
	x64Parser::Coding218,
	x64Parser::Coding219,
	x64Parser::Coding220,
	x64Parser::Coding221,
	x64Parser::Coding222,
	x64Parser::Coding223,
	x64Parser::Coding224,
	x64Parser::Coding225,
	x64Parser::Coding226,
	x64Parser::Coding227,
	x64Parser::Coding228,
	x64Parser::Coding229,
	x64Parser::Coding230,
	x64Parser::Coding231,
	x64Parser::Coding232,
	x64Parser::Coding233,
	x64Parser::Coding234,
	x64Parser::Coding235,
	x64Parser::Coding236,
	x64Parser::Coding237,
	x64Parser::Coding238,
	x64Parser::Coding239,
	x64Parser::Coding240,
	x64Parser::Coding241,
	x64Parser::Coding242,
	x64Parser::Coding243,
	x64Parser::Coding244,
	x64Parser::Coding245,
	x64Parser::Coding246,
	x64Parser::Coding247,
	x64Parser::Coding248,
	x64Parser::Coding249,
	x64Parser::Coding250,
	x64Parser::Coding251,
	x64Parser::Coding252,
	x64Parser::Coding253,
	x64Parser::Coding254,
	x64Parser::Coding255,
	x64Parser::Coding256,
	x64Parser::Coding257,
	x64Parser::Coding258,
	x64Parser::Coding259,
	x64Parser::Coding260,
	x64Parser::Coding261,
	x64Parser::Coding262,
	x64Parser::Coding263,
	x64Parser::Coding264,
	x64Parser::Coding265,
	x64Parser::Coding266,
	x64Parser::Coding267,
	x64Parser::Coding268,
	x64Parser::Coding269,
	x64Parser::Coding270,
	x64Parser::Coding271,
	x64Parser::Coding272,
	x64Parser::Coding273,
	x64Parser::Coding274,
	x64Parser::Coding275,
	x64Parser::Coding276,
	x64Parser::Coding277,
	x64Parser::Coding278,
	x64Parser::Coding279,
	x64Parser::Coding280,
	x64Parser::Coding281,
	x64Parser::Coding282,
	x64Parser::Coding283,
	x64Parser::Coding284,
	x64Parser::Coding285,
	x64Parser::Coding286,
	x64Parser::Coding287,
	x64Parser::Coding288,
	x64Parser::Coding289,
	x64Parser::Coding290,
	x64Parser::Coding291,
	x64Parser::Coding292,
	x64Parser::Coding293,
	x64Parser::Coding294,
	x64Parser::Coding295,
	x64Parser::Coding296,
	x64Parser::Coding297,
	x64Parser::Coding298,
	x64Parser::Coding299,
	x64Parser::Coding300,
	x64Parser::Coding301,
	x64Parser::Coding302,
	x64Parser::Coding303,
	x64Parser::Coding304,
	x64Parser::Coding305,
	x64Parser::Coding306,
	x64Parser::Coding307,
	x64Parser::Coding308,
	x64Parser::Coding309,
	x64Parser::Coding310,
	x64Parser::Coding311,
	x64Parser::Coding312,
	x64Parser::Coding313,
	x64Parser::Coding314,
	x64Parser::Coding315,
	x64Parser::Coding316,
};
Coding x64Parser::prefixCoding1[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::eot },
};
Coding x64Parser::prefixCoding2[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::eot },
};
Coding x64Parser::prefixCoding3[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 240, 8},
	{ Coding::eot },
};
Coding x64Parser::prefixCoding4[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::eot },
};
Coding x64Parser::prefixCoding5[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::eot },
};
Coding x64Parser::prefixCoding6[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 243, 8},
	{ Coding::eot },
};
Coding x64Parser::prefixCoding7[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 243, 8},
	{ Coding::eot },
};
Coding x64Parser::prefixCoding8[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 242, 8},
	{ Coding::eot },
};
Coding x64Parser::prefixCoding9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 242, 8},
	{ Coding::eot },
};
Coding x64Parser::prefixCoding10[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 243, 8},
	{ Coding::eot },
};
Coding *x64Parser::prefixCodings[] = {
	x64Parser::prefixCoding1,
	x64Parser::prefixCoding2,
	x64Parser::prefixCoding3,
	x64Parser::prefixCoding4,
	x64Parser::prefixCoding5,
	x64Parser::prefixCoding6,
	x64Parser::prefixCoding7,
	x64Parser::prefixCoding8,
	x64Parser::prefixCoding9,
	x64Parser::prefixCoding10,
};
bool x64Parser::MatchesToken(int token, int tokenPos)
{
	return tokenPos < inputTokens.size() && inputTokens[tokenPos]->type == InputToken::TOKEN && inputTokens[tokenPos]->val->ival == token;
}
	bool x64Parser::MatchesRegister(int reg, int tokenPos)
{
	return tokenPos < inputTokens.size() && inputTokens[tokenPos]->type == InputToken::REGISTER && inputTokens[tokenPos]->val->ival == reg;
}
	bool x64Parser::MatchesRegisterClass(int cclass, int tokenPos)
{
	return tokenPos < inputTokens.size() && inputTokens[tokenPos]->type == InputToken::REGISTER && (registerDataIndirect[cclass][inputTokens[tokenPos]->val->ival >> 3] & (1 << (inputTokens[tokenPos]->val->ival & 7)));
}
bool x64Parser::ParseAddresses(x64Operand &operand, int addrClass, int &tokenPos)
{
	int level = 0;
	bool rv = false;
	x64Token *t = tokenBranches1;
	while (t->type != x64Token::EOT)
	{
		bool matches = false;
		if (t->addrClass[addrClass >> 3] & (1 << (addrClass & 7)))
		{
			switch (t->type)
			{
				case x64Token::EMPTY:
					matches = true;
					break;
				case x64Token::TOKEN:
					matches = MatchesToken(t->id, tokenPos);
					break;
				case x64Token::REGISTER:
					matches = MatchesRegister(t->id, tokenPos);
					break;
				case x64Token::REGISTERCLASS:
					matches = MatchesRegisterClass(t->id, tokenPos);
					break;
				case x64Token::NUMBER:
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

bool x64Parser::ParseOperands2(x64Token *tokenList, x64Operand &operand, int tokenPos, int level)
{
	bool rv = false;
	x64Token *t = tokenList;
	while (t && t->type != x64Token::EOT)
	{
		bool matches = false;
		int last = tokenPos;
		switch (t->type)
		{
			case x64Token::EMPTY:
				matches = true;
				break;
			case x64Token::TOKEN:
				matches = MatchesToken(t->id, tokenPos);
				break;
			case x64Token::REGISTER:
				matches = MatchesRegister(t->id, tokenPos);
				break;
			case x64Token::REGISTERCLASS:
				matches = MatchesRegisterClass(t->id,tokenPos);
				break;
			case x64Token::ADDRESSCLASS:
				matches = ParseAddresses(operand, t->id, tokenPos);
				break;
			case x64Token::NUMBER:
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

bool x64Parser::ParseOperands(x64Token *tokenList, x64Operand &operand)
{
	return ParseOperands2(tokenList, operand, 0, 0);
}

bool x64Parser::ProcessCoding(CodingHelper &base, x64Operand &operand, Coding *coding)
{
	CodingHelper current;

	while (coding->type != Coding::eot)
	{
		current = base;
		if (coding->type & Coding::bitSpecified)
			current.bits = coding->bits;
		if (coding->type & Coding::fieldSpecified)
			current.field = coding->field;
		if (coding->math)
		{
			current.math = coding->math;
			current.mathval = coding->mathval;
		}
		if (coding->type & Coding::valSpecified)
		{
			int n = current.DoMath(coding->val);
			bits.Add(n, current.bits);
		}
		else if (coding->type & Coding::reg)
		{
			int n = coding->val;
			if (current.field != -1)
				n = registerValues[n][current.field];
			n = current.DoMath(n);
			bits.Add(n, current.bits);
		}
		else if (coding->type & Coding::stateFunc)
		{
			Coding *c = (this->*stateFuncs[coding->val])();
			if (!ProcessCoding(current, operand,c))
				return false;
		}
		else if (coding->type & Coding::stateVar)
		{
			int n = current.DoMath(stateVars[coding->val]);
			bits.Add(n, current.bits);
		}
		else if (coding->type & Coding::number)
		{
			int n = coding->val;
			auto it = operands.begin();
			for (int i=0; i < n; i++)
			{
				++it;
			}
			(*it)->used = true;
			(*it)->pos = this->bits.GetBits();
			bits.Add((*it)->node->ival, current.bits);
		}
		else if (coding->type & Coding::native)
		{
			if (operand.addressCoding == -1)
				return false;
			if (!ProcessCoding(current, operand, Codings[operand.addressCoding]))
				return false;
		}
		else if (coding->type & Coding::indirect)
		{
			if (!operand.values[coding->val])
				return false;
			if (!ProcessCoding(current, operand, operand.values[coding->val]))
				return false;
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

bool x64Parser::DispatchOpcode(int opcode)
{
	bool rv;
	if (opcode == -1)
	{
		rv = true;
		x64Operand operand;
		CodingHelper base;
		for (auto& a : prefixes)
			rv &= ProcessCoding(base, operand, prefixCodings[a]);
	}
	else
	{
		x64Operand operand;
		operand.opcode = opcode;
		rv = (this->*DispatchTable[opcode])(operand);
		if (rv)
		{
			CodingHelper base;
			for (auto& a : prefixes)
				rv &= ProcessCoding(base, operand, prefixCodings[a]);
			if (rv)
			{
				if (operand.operandCoding != -1)
					rv = ProcessCoding(base, operand, Codings[operand.operandCoding]);
				else if (operand.addressCoding != -1)
					rv = ProcessCoding(base, operand, Codings[operand.addressCoding]);
				else rv = false;
			}
		}
	}
	return rv;
}

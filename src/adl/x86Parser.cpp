#include "InstructionParser.h"
#include "x86Operand.h"
#include "x86Parser.h"

InstructionParser *InstructionParser::GetInstance()
{
	return static_cast<InstructionParser *>(new x86Parser());
}
Coding x86Parser::stateCoding_eot[] = { { Coding::eot } };
Coding x86Parser::stateCoding1_1[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 103, 8},
	{ Coding::eot },
};
Coding x86Parser::stateCoding1_2[] = {
	{ Coding::eot },
};
Coding x86Parser::stateCoding2_1[] = {
	{ Coding::eot },
};
Coding x86Parser::stateCoding2_2[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 103, 8},
	{ Coding::eot },
};
Coding x86Parser::stateCoding3_1[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 102, 8},
	{ Coding::eot },
};
Coding x86Parser::stateCoding3_2[] = {
	{ Coding::eot },
};
Coding x86Parser::stateCoding4_1[] = {
	{ Coding::eot },
};
Coding x86Parser::stateCoding4_2[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 102, 8},
	{ Coding::eot },
};
Coding *x86Parser::StateFunc1()
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
Coding *x86Parser::StateFunc2()
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
Coding *x86Parser::StateFunc3()
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
Coding *x86Parser::StateFunc4()
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
x86Parser::StateFuncDispatchType x86Parser::stateFuncs[] = {
	&StateFunc1,
	&StateFunc2,
	&StateFunc3,
	&StateFunc4,
};
bool x86Parser::Number1(int tokenPos)
{
	bool rv = false;
	if (!rv)
	{
		rv = ParseNumber(0, 1, 8, 1, tokenPos);
	}
	return rv;
}
bool x86Parser::Number2(int tokenPos)
{
	bool rv = false;
	if (!rv)
	{
		rv = ParseNumber(0, 0, 6, 1, tokenPos);
	}
	return rv;
}
bool x86Parser::Number3(int tokenPos)
{
	bool rv = false;
	if (!rv)
	{
		rv = ParseNumber(0, 0, 8, 1, tokenPos);
	}
	return rv;
}
bool x86Parser::Number4(int tokenPos)
{
	bool rv = false;
	if (!rv)
	{
		rv = ParseNumber(0, 0, 16, 0, tokenPos);
	}
	return rv;
}
bool x86Parser::Number5(int tokenPos)
{
	bool rv = false;
	if (!rv)
	{
		rv = ParseNumber(0, 0, 32, 0, tokenPos);
	}
	return rv;
}
bool x86Parser::Number6(int tokenPos)
{
	bool rv = false;
	if (!rv && stateVars[0]==0)
	{
		rv = ParseNumber(0, 0, 16, 0, tokenPos);
	}
	return rv;
}
bool x86Parser::Number7(int tokenPos)
{
	bool rv = false;
	if (!rv && stateVars[0]==1)
	{
		rv = ParseNumber(0, 0, 32, 0, tokenPos);
	}
	return rv;
}
bool x86Parser::Number8(int tokenPos)
{
	bool rv = false;
	if (!rv)
	{
		rv = ParseNumber(1, 1, 8, 0, tokenPos);
	}
	return rv;
}
bool x86Parser::Number9(int tokenPos)
{
	bool rv = false;
	if (!rv)
	{
		rv = ParseNumber(-1, 1, 8, 0, tokenPos);
	}
	return rv;
}
bool x86Parser::Number10(int tokenPos)
{
	bool rv = false;
	if (!rv && stateVars[0]==0)
	{
		rv = ParseNumber(2, 1, 16, 0, tokenPos);
	}
	return rv;
}
bool x86Parser::Number11(int tokenPos)
{
	bool rv = false;
	if (!rv && stateVars[0]==0)
	{
		rv = ParseNumber(-2, 1, 16, 0, tokenPos);
	}
	return rv;
}
bool x86Parser::Number12(int tokenPos)
{
	bool rv = false;
	if (!rv && stateVars[0]==0)
	{
		rv = ParseNumber(2, 1, 16, 0, tokenPos);
	}
	return rv;
}
bool x86Parser::Number13(int tokenPos)
{
	bool rv = false;
	if (!rv && stateVars[0]==0)
	{
		rv = ParseNumber(-2, 1, 16, 0, tokenPos);
	}
	return rv;
}
bool x86Parser::Number14(int tokenPos)
{
	bool rv = false;
	if (!rv && stateVars[0]!=0)
	{
		rv = ParseNumber(4, 1, 32, 0, tokenPos);
	}
	return rv;
}
bool x86Parser::Number15(int tokenPos)
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
bool x86Parser::Number16(int tokenPos)
{
	bool rv = false;
	if (!rv)
		rv = SetNumber(tokenPos, 0, 0);
	return rv;
}
bool x86Parser::Number17(int tokenPos)
{
	bool rv = false;
	if (!rv)
		rv = SetNumber(tokenPos, 1, 1);
	return rv;
}
x86Parser::NumberDispatchType x86Parser::numberFuncs[] = {
	&x86Parser::Number1,
	&x86Parser::Number2,
	&x86Parser::Number3,
	&x86Parser::Number4,
	&x86Parser::Number5,
	&x86Parser::Number6,
	&x86Parser::Number7,
	&x86Parser::Number8,
	&x86Parser::Number9,
	&x86Parser::Number10,
	&x86Parser::Number11,
	&x86Parser::Number12,
	&x86Parser::Number13,
	&x86Parser::Number14,
	&x86Parser::Number15,
	&x86Parser::Number16,
	&x86Parser::Number17,
};

unsigned char x86Parser::RegClassData1[] = {255, 0, };
unsigned char x86Parser::RegClassData2[] = {127, 0, };
unsigned char x86Parser::RegClassData3[] = {191, 0, };
unsigned char x86Parser::RegClassData4[] = {9, 1, };
unsigned char x86Parser::RegClassData5[] = {17, 2, };
unsigned char x86Parser::RegClassData6[] = {33, 4, };
unsigned char x86Parser::RegClassData7[] = {4, 8, };

unsigned char x86Parser::registerData10[] = {0, 64, 4, 0, 0, 0, 0, 0, };
unsigned char x86Parser::registerData6[] = {136, 136, 170, 0, 0, 0, 0, 0, };
unsigned char x86Parser::registerData15[] = {0, 0, 0, 0, 192, 63, 0, 0, };
unsigned char x86Parser::registerData16[] = {0, 0, 0, 0, 0, 192, 63, 0, };
unsigned char x86Parser::registerData14[] = {0, 0, 0, 192, 63, 0, 0, 0, };
unsigned char x86Parser::registerData1[] = {255, 255, 240, 0, 0, 0, 0, 0, };
unsigned char x86Parser::registerData4[] = {34, 34, 0, 0, 0, 0, 0, 0, };
unsigned char x86Parser::registerData12[] = {0, 0, 80, 0, 0, 0, 0, 0, };
unsigned char x86Parser::registerData7[] = {136, 136, 168, 0, 0, 0, 0, 0, };
unsigned char x86Parser::registerData3[] = {17, 17, 0, 0, 0, 0, 0, 0, };
unsigned char x86Parser::registerData9[] = {0, 240, 240, 0, 0, 0, 0, 0, };
unsigned char x86Parser::registerData5[] = {68, 68, 85, 0, 0, 0, 0, 0, };
unsigned char x86Parser::registerData8[] = {136, 136, 170, 0, 0, 0, 0, 0, };
unsigned char x86Parser::registerData2[] = {51, 51, 0, 0, 0, 0, 0, 0, };
unsigned char x86Parser::registerData13[] = {0, 0, 0, 63, 0, 0, 0, 0, };
unsigned char x86Parser::registerData11[] = {0, 0, 15, 0, 0, 0, 0, 0, };
unsigned char x86Parser::registerData17[] = {0, 0, 0, 0, 0, 0, 192, 63, };

unsigned char *x86Parser::registerDataIndirect[] = {
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

int x86Parser::registerValues[][4] = {
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

void x86Parser::Init()
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
	opcodeTable["lfs"] = 230;
	opcodeTable["lgdt"] = 231;
	opcodeTable["lgs"] = 232;
	opcodeTable["lidt"] = 233;
	opcodeTable["lmsw"] = 234;
	opcodeTable["lods"] = 235;
	opcodeTable["lodsb"] = 236;
	opcodeTable["lodsw"] = 237;
	opcodeTable["lodsd"] = 238;
	opcodeTable["loop"] = 239;
	opcodeTable["loope"] = 240;
	opcodeTable["loopne"] = 241;
	opcodeTable["loopnz"] = 242;
	opcodeTable["loopz"] = 243;
	opcodeTable["lsl"] = 244;
	opcodeTable["lss"] = 245;
	opcodeTable["ltr"] = 246;
	opcodeTable["mov"] = 247;
	opcodeTable["movs"] = 248;
	opcodeTable["movsb"] = 249;
	opcodeTable["movsw"] = 250;
	opcodeTable["movsd"] = 251;
	opcodeTable["movsx"] = 252;
	opcodeTable["movzx"] = 253;
	opcodeTable["mul"] = 254;
	opcodeTable["neg"] = 255;
	opcodeTable["nop"] = 256;
	opcodeTable["not"] = 257;
	opcodeTable["or"] = 258;
	opcodeTable["out"] = 259;
	opcodeTable["outs"] = 260;
	opcodeTable["outsb"] = 261;
	opcodeTable["outsw"] = 262;
	opcodeTable["outsd"] = 263;
	opcodeTable["pop"] = 264;
	opcodeTable["popa"] = 265;
	opcodeTable["popaw"] = 266;
	opcodeTable["popad"] = 267;
	opcodeTable["popf"] = 268;
	opcodeTable["popfw"] = 269;
	opcodeTable["popfd"] = 270;
	opcodeTable["push"] = 271;
	opcodeTable["pusha"] = 272;
	opcodeTable["pushaw"] = 273;
	opcodeTable["pushad"] = 274;
	opcodeTable["pushf"] = 275;
	opcodeTable["pushfw"] = 276;
	opcodeTable["pushfd"] = 277;
	opcodeTable["rcl"] = 278;
	opcodeTable["rcr"] = 279;
	opcodeTable["rdmsr"] = 280;
	opcodeTable["rdpmc"] = 281;
	opcodeTable["rdtsc"] = 282;
	opcodeTable["ret"] = 283;
	opcodeTable["retf"] = 284;
	opcodeTable["rol"] = 285;
	opcodeTable["ror"] = 286;
	opcodeTable["rsm"] = 287;
	opcodeTable["sahf"] = 288;
	opcodeTable["sal"] = 289;
	opcodeTable["sar"] = 290;
	opcodeTable["sbb"] = 291;
	opcodeTable["scas"] = 292;
	opcodeTable["scasb"] = 293;
	opcodeTable["scasw"] = 294;
	opcodeTable["scasd"] = 295;
	opcodeTable["seta"] = 296;
	opcodeTable["setae"] = 297;
	opcodeTable["setb"] = 298;
	opcodeTable["setbe"] = 299;
	opcodeTable["setc"] = 300;
	opcodeTable["sete"] = 301;
	opcodeTable["setg"] = 302;
	opcodeTable["setge"] = 303;
	opcodeTable["setl"] = 304;
	opcodeTable["setle"] = 305;
	opcodeTable["setna"] = 306;
	opcodeTable["setnae"] = 307;
	opcodeTable["setnb"] = 308;
	opcodeTable["setnbe"] = 309;
	opcodeTable["setnc"] = 310;
	opcodeTable["setne"] = 311;
	opcodeTable["setng"] = 312;
	opcodeTable["setnge"] = 313;
	opcodeTable["setnl"] = 314;
	opcodeTable["setnle"] = 315;
	opcodeTable["setno"] = 316;
	opcodeTable["setnp"] = 317;
	opcodeTable["setns"] = 318;
	opcodeTable["setnz"] = 319;
	opcodeTable["seto"] = 320;
	opcodeTable["setp"] = 321;
	opcodeTable["setpe"] = 322;
	opcodeTable["setpo"] = 323;
	opcodeTable["sets"] = 324;
	opcodeTable["setz"] = 325;
	opcodeTable["shl"] = 326;
	opcodeTable["shld"] = 327;
	opcodeTable["shr"] = 328;
	opcodeTable["shrd"] = 329;
	opcodeTable["sidt"] = 330;
	opcodeTable["sldt"] = 331;
	opcodeTable["smsw"] = 332;
	opcodeTable["stc"] = 333;
	opcodeTable["std"] = 334;
	opcodeTable["sti"] = 335;
	opcodeTable["stos"] = 336;
	opcodeTable["stosb"] = 337;
	opcodeTable["stosw"] = 338;
	opcodeTable["stosd"] = 339;
	opcodeTable["str"] = 340;
	opcodeTable["sub"] = 341;
	opcodeTable["test"] = 342;
	opcodeTable["ud2"] = 343;
	opcodeTable["verr"] = 344;
	opcodeTable["verw"] = 345;
	opcodeTable["wait"] = 346;
	opcodeTable["wbinvd"] = 347;
	opcodeTable["wrmsr"] = 348;
	opcodeTable["xadd"] = 349;
	opcodeTable["xbts"] = 350;
	opcodeTable["xchg"] = 351;
	opcodeTable["xlat"] = 352;
	opcodeTable["xlatb"] = 353;
	opcodeTable["xor"] = 354;
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

void x86Parser::TokenFunc4(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 0;
}
x86Token x86Parser::tokenBranches3[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc4,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc9(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 1;
}
x86Token x86Parser::tokenBranches8[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc9,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc200(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 19;
}
x86Token x86Parser::tokenBranches199[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc200,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc346(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 34;
}
x86Token x86Parser::tokenBranches345[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc346,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc199(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc345(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches198[] = {
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc199, x86Parser::tokenBranches199 },
	{x86Token::NUMBER, 3, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc345, x86Parser::tokenBranches345 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc36(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 4;
}
x86Token x86Parser::tokenBranches35[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc36,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches198 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc137(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 13;
}
x86Token x86Parser::tokenBranches136[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc137,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc283(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 28;
}
x86Token x86Parser::tokenBranches282[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc283,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc429(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 43;
}
x86Token x86Parser::tokenBranches428[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc429,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc35(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc136(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc282(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc428(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches34[] = {
	{x86Token::REGISTERCLASS, 11, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc35, x86Parser::tokenBranches35 },
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc136, x86Parser::tokenBranches136 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc282, x86Parser::tokenBranches282 },
	{x86Token::NUMBER, 3, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc428, x86Parser::tokenBranches428 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc97(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 13;
}
x86Token x86Parser::tokenBranches33[] = {
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches34 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc97,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc227(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 22;
}
x86Token x86Parser::tokenBranches226[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc227,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc373(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 37;
}
x86Token x86Parser::tokenBranches372[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc373,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc226(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc372(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches225[] = {
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc226, x86Parser::tokenBranches226 },
	{x86Token::NUMBER, 3, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc372, x86Parser::tokenBranches372 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc57(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 7;
}
x86Token x86Parser::tokenBranches56[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc57,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches225 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc170(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 17;
}
x86Token x86Parser::tokenBranches169[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc170,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc316(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 32;
}
x86Token x86Parser::tokenBranches315[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc316,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc462(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 47;
}
x86Token x86Parser::tokenBranches461[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc462,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc56(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc169(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc315(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc461(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches55[] = {
	{x86Token::REGISTERCLASS, 11, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc56, x86Parser::tokenBranches56 },
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc169, x86Parser::tokenBranches169 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc315, x86Parser::tokenBranches315 },
	{x86Token::NUMBER, 3, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc461, x86Parser::tokenBranches461 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc120(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 17;
}
x86Token x86Parser::tokenBranches54[] = {
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches55 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc120,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc257(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 25;
}
x86Token x86Parser::tokenBranches256[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc257,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc403(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 40;
}
x86Token x86Parser::tokenBranches402[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc403,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc256(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc402(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches255[] = {
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc256, x86Parser::tokenBranches256 },
	{x86Token::NUMBER, 3, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc402, x86Parser::tokenBranches402 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc79(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 10;
}
x86Token x86Parser::tokenBranches78[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc79,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches255 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc584(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 53;
}
x86Token x86Parser::tokenBranches583[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc584,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc662(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 68;
}
x86Token x86Parser::tokenBranches661[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc662,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc740(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 79;
}
x86Token x86Parser::tokenBranches739[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc740,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1034(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 106;
}
x86Token x86Parser::tokenBranches1033[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1034,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1442(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 124;
}
x86Token x86Parser::tokenBranches1441[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1442,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1670(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 142;
}
x86Token x86Parser::tokenBranches1669[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1670,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1033(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1441(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1669(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1032[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1033, x86Parser::tokenBranches1033 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1441, x86Parser::tokenBranches1441 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1669, x86Parser::tokenBranches1669 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1232(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 106;
}
x86Token x86Parser::tokenBranches1031[] = {
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches1032 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1232,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1031(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1030[] = {
	{x86Token::NUMBER, 14, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1031, x86Parser::tokenBranches1031 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1043(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 107;
}
x86Token x86Parser::tokenBranches1042[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1043,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1451(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 125;
}
x86Token x86Parser::tokenBranches1450[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1451,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1679(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 143;
}
x86Token x86Parser::tokenBranches1678[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1679,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1042(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1450(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1678(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1041[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1042, x86Parser::tokenBranches1042 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1450, x86Parser::tokenBranches1450 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1678, x86Parser::tokenBranches1678 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1239(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 107;
}
x86Token x86Parser::tokenBranches1029[] = {
	{x86Token::TOKEN, 5, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches1030 },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches1041 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1239,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc583(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc661(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc739(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1029(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches582[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc583, x86Parser::tokenBranches583 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc661, x86Parser::tokenBranches661 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc739, x86Parser::tokenBranches739 },
	{x86Token::REGISTERCLASS, 6, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1029, x86Parser::tokenBranches1029 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc498(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 53;
}
x86Token x86Parser::tokenBranches497[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc498,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches582 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc591(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 54;
}
x86Token x86Parser::tokenBranches590[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc591,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc669(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 69;
}
x86Token x86Parser::tokenBranches668[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc669,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc747(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 80;
}
x86Token x86Parser::tokenBranches746[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc747,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1014(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 104;
}
x86Token x86Parser::tokenBranches1013[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1014,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1422(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 122;
}
x86Token x86Parser::tokenBranches1421[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1422,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1650(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 140;
}
x86Token x86Parser::tokenBranches1649[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1650,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1013(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1421(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1649(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1012[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1013, x86Parser::tokenBranches1013 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1421, x86Parser::tokenBranches1421 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1649, x86Parser::tokenBranches1649 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1216(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 104;
}
x86Token x86Parser::tokenBranches1011[] = {
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches1012 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1216,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1011(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1010[] = {
	{x86Token::NUMBER, 14, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1011, x86Parser::tokenBranches1011 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1023(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 105;
}
x86Token x86Parser::tokenBranches1022[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1023,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1431(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 123;
}
x86Token x86Parser::tokenBranches1430[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1431,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1659(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 141;
}
x86Token x86Parser::tokenBranches1658[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1659,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1022(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1430(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1658(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1021[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1022, x86Parser::tokenBranches1022 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1430, x86Parser::tokenBranches1430 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1658, x86Parser::tokenBranches1658 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1223(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 105;
}
x86Token x86Parser::tokenBranches1009[] = {
	{x86Token::TOKEN, 5, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches1010 },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches1021 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1223,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc590(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc668(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc746(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1009(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches589[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc590, x86Parser::tokenBranches590 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc668, x86Parser::tokenBranches668 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc746, x86Parser::tokenBranches746 },
	{x86Token::REGISTERCLASS, 6, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1009, x86Parser::tokenBranches1009 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc847(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 85;
}
x86Token x86Parser::tokenBranches846[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc847,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc846(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches845[] = {
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc846, x86Parser::tokenBranches846 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc838(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 84;
}
x86Token x86Parser::tokenBranches837[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc838,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches845 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc837(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches836[] = {
	{x86Token::NUMBER, 14, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc837, x86Parser::tokenBranches837 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc503(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 54;
}
x86Token x86Parser::tokenBranches502[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc503,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches589 },
	{x86Token::TOKEN, 5, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches836 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc598(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 55;
}
x86Token x86Parser::tokenBranches597[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc598,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc676(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 70;
}
x86Token x86Parser::tokenBranches675[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc676,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc754(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 76;
}
x86Token x86Parser::tokenBranches753[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc754,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1054(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 100;
}
x86Token x86Parser::tokenBranches1053[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1054,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1462(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 118;
}
x86Token x86Parser::tokenBranches1461[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1462,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1690(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 136;
}
x86Token x86Parser::tokenBranches1689[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1690,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1053(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1461(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1689(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1052[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1053, x86Parser::tokenBranches1053 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1461, x86Parser::tokenBranches1461 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1689, x86Parser::tokenBranches1689 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1248(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 100;
}
x86Token x86Parser::tokenBranches1051[] = {
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches1052 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1248,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1051(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1050[] = {
	{x86Token::NUMBER, 14, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1051, x86Parser::tokenBranches1051 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1063(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 101;
}
x86Token x86Parser::tokenBranches1062[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1063,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1471(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 119;
}
x86Token x86Parser::tokenBranches1470[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1471,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1699(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 137;
}
x86Token x86Parser::tokenBranches1698[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1699,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1062(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1470(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1698(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1061[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1062, x86Parser::tokenBranches1062 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1470, x86Parser::tokenBranches1470 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1698, x86Parser::tokenBranches1698 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1255(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 101;
}
x86Token x86Parser::tokenBranches1049[] = {
	{x86Token::TOKEN, 5, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches1050 },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches1061 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1255,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc597(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc675(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc753(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1049(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches596[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc597, x86Parser::tokenBranches597 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc675, x86Parser::tokenBranches675 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc753, x86Parser::tokenBranches753 },
	{x86Token::REGISTERCLASS, 6, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1049, x86Parser::tokenBranches1049 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc863(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 87;
}
x86Token x86Parser::tokenBranches862[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc863,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc862(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches861[] = {
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc862, x86Parser::tokenBranches862 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc854(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 86;
}
x86Token x86Parser::tokenBranches853[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc854,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches861 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc853(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches852[] = {
	{x86Token::NUMBER, 14, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc853, x86Parser::tokenBranches853 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc508(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 55;
}
x86Token x86Parser::tokenBranches507[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc508,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches596 },
	{x86Token::TOKEN, 5, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches852 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc531(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 60;
}
x86Token x86Parser::tokenBranches530[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc531,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc8(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc78(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc507(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc530(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches7[] = {
	{x86Token::REGISTER, 14, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches33 },
	{x86Token::REGISTER, 18, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches54 },
	{x86Token::REGISTER, 17, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches497 },
	{x86Token::REGISTER, 19, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches502 },
	{x86Token::NUMBER, 5, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc8, x86Parser::tokenBranches8 },
	{x86Token::REGISTERCLASS, 11, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc78, x86Parser::tokenBranches78 },
	{x86Token::REGISTERCLASS, 5, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc507, x86Parser::tokenBranches507 },
	{x86Token::NUMBER, 6, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc530, x86Parser::tokenBranches530 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches6[] = {
	{x86Token::TOKEN, 3, 0, 0, x86Parser::RegClassData1,  NULL, x86Parser::tokenBranches7 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc14(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 2;
}
x86Token x86Parser::tokenBranches13[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc14,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc209(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 21;
}
x86Token x86Parser::tokenBranches208[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc209,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc355(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 36;
}
x86Token x86Parser::tokenBranches354[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc355,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc208(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc354(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches207[] = {
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc208, x86Parser::tokenBranches208 },
	{x86Token::NUMBER, 3, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc354, x86Parser::tokenBranches354 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc43(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 6;
}
x86Token x86Parser::tokenBranches42[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc43,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches207 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc144(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 14;
}
x86Token x86Parser::tokenBranches143[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc144,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc290(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 29;
}
x86Token x86Parser::tokenBranches289[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc290,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc436(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 44;
}
x86Token x86Parser::tokenBranches435[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc436,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc42(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc143(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc289(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc435(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches41[] = {
	{x86Token::REGISTERCLASS, 11, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc42, x86Parser::tokenBranches42 },
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc143, x86Parser::tokenBranches143 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc289, x86Parser::tokenBranches289 },
	{x86Token::NUMBER, 3, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc435, x86Parser::tokenBranches435 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc102(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 14;
}
x86Token x86Parser::tokenBranches40[] = {
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches41 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc102,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc218(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 20;
}
x86Token x86Parser::tokenBranches217[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc218,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc364(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 35;
}
x86Token x86Parser::tokenBranches363[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc364,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc217(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc363(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches216[] = {
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc217, x86Parser::tokenBranches217 },
	{x86Token::NUMBER, 3, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc363, x86Parser::tokenBranches363 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc50(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 5;
}
x86Token x86Parser::tokenBranches49[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc50,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches216 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc163(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 16;
}
x86Token x86Parser::tokenBranches162[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc163,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc309(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 31;
}
x86Token x86Parser::tokenBranches308[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc309,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc455(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 46;
}
x86Token x86Parser::tokenBranches454[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc455,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc49(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc162(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc308(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc454(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches48[] = {
	{x86Token::REGISTERCLASS, 11, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc49, x86Parser::tokenBranches49 },
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc162, x86Parser::tokenBranches162 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc308, x86Parser::tokenBranches308 },
	{x86Token::NUMBER, 3, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc454, x86Parser::tokenBranches454 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc115(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 16;
}
x86Token x86Parser::tokenBranches47[] = {
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches48 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc115,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc264(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 26;
}
x86Token x86Parser::tokenBranches263[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc264,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc410(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 41;
}
x86Token x86Parser::tokenBranches409[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc410,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc263(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc409(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches262[] = {
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc263, x86Parser::tokenBranches263 },
	{x86Token::NUMBER, 3, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc409, x86Parser::tokenBranches409 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc84(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 11;
}
x86Token x86Parser::tokenBranches83[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc84,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches262 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc563(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 49;
}
x86Token x86Parser::tokenBranches562[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc563,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc641(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 63;
}
x86Token x86Parser::tokenBranches640[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc641,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc719(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 75;
}
x86Token x86Parser::tokenBranches718[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc719,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc974(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 98;
}
x86Token x86Parser::tokenBranches973[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc974,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1382(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 116;
}
x86Token x86Parser::tokenBranches1381[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1382,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1610(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 134;
}
x86Token x86Parser::tokenBranches1609[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1610,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc973(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1381(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1609(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches972[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc973, x86Parser::tokenBranches973 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1381, x86Parser::tokenBranches1381 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1609, x86Parser::tokenBranches1609 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1184(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 98;
}
x86Token x86Parser::tokenBranches971[] = {
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches972 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1184,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc971(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches970[] = {
	{x86Token::NUMBER, 14, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc971, x86Parser::tokenBranches971 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc983(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 99;
}
x86Token x86Parser::tokenBranches982[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc983,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1391(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 117;
}
x86Token x86Parser::tokenBranches1390[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1391,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1619(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 135;
}
x86Token x86Parser::tokenBranches1618[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1619,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc982(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1390(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1618(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches981[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc982, x86Parser::tokenBranches982 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1390, x86Parser::tokenBranches1390 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1618, x86Parser::tokenBranches1618 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1191(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 99;
}
x86Token x86Parser::tokenBranches969[] = {
	{x86Token::TOKEN, 5, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches970 },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches981 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1191,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc562(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc640(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc718(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc969(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches561[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc562, x86Parser::tokenBranches562 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc640, x86Parser::tokenBranches640 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc718, x86Parser::tokenBranches718 },
	{x86Token::REGISTERCLASS, 6, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc969, x86Parser::tokenBranches969 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc483(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 49;
}
x86Token x86Parser::tokenBranches482[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc483,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches561 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc570(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 50;
}
x86Token x86Parser::tokenBranches569[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc570,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc648(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 66;
}
x86Token x86Parser::tokenBranches647[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc648,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc726(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 77;
}
x86Token x86Parser::tokenBranches725[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc726,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc954(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 96;
}
x86Token x86Parser::tokenBranches953[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc954,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1362(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 114;
}
x86Token x86Parser::tokenBranches1361[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1362,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1590(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 132;
}
x86Token x86Parser::tokenBranches1589[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1590,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc953(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1361(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1589(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches952[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc953, x86Parser::tokenBranches953 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1361, x86Parser::tokenBranches1361 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1589, x86Parser::tokenBranches1589 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1168(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 96;
}
x86Token x86Parser::tokenBranches951[] = {
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches952 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1168,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc951(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches950[] = {
	{x86Token::NUMBER, 14, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc951, x86Parser::tokenBranches951 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc963(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 97;
}
x86Token x86Parser::tokenBranches962[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc963,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1371(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 115;
}
x86Token x86Parser::tokenBranches1370[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1371,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1599(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 133;
}
x86Token x86Parser::tokenBranches1598[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1599,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc962(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1370(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1598(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches961[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc962, x86Parser::tokenBranches962 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1370, x86Parser::tokenBranches1370 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1598, x86Parser::tokenBranches1598 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1175(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 97;
}
x86Token x86Parser::tokenBranches949[] = {
	{x86Token::TOKEN, 5, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches950 },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches961 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1175,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc569(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc647(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc725(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc949(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches568[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc569, x86Parser::tokenBranches569 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc647, x86Parser::tokenBranches647 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc725, x86Parser::tokenBranches725 },
	{x86Token::REGISTERCLASS, 6, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc949, x86Parser::tokenBranches949 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc815(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 89;
}
x86Token x86Parser::tokenBranches814[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc815,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc814(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches813[] = {
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc814, x86Parser::tokenBranches814 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc806(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 88;
}
x86Token x86Parser::tokenBranches805[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc806,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches813 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc805(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches804[] = {
	{x86Token::NUMBER, 14, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc805, x86Parser::tokenBranches805 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc488(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 50;
}
x86Token x86Parser::tokenBranches487[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc488,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches568 },
	{x86Token::TOKEN, 5, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches804 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc577(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 52;
}
x86Token x86Parser::tokenBranches576[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc577,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc655(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 67;
}
x86Token x86Parser::tokenBranches654[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc655,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc733(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 78;
}
x86Token x86Parser::tokenBranches732[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc733,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc994(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 102;
}
x86Token x86Parser::tokenBranches993[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc994,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1402(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 120;
}
x86Token x86Parser::tokenBranches1401[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1402,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1630(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 138;
}
x86Token x86Parser::tokenBranches1629[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1630,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc993(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1401(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1629(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches992[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc993, x86Parser::tokenBranches993 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1401, x86Parser::tokenBranches1401 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1629, x86Parser::tokenBranches1629 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1200(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 102;
}
x86Token x86Parser::tokenBranches991[] = {
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches992 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1200,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc991(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches990[] = {
	{x86Token::NUMBER, 14, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc991, x86Parser::tokenBranches991 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1003(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 103;
}
x86Token x86Parser::tokenBranches1002[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1003,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1411(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 121;
}
x86Token x86Parser::tokenBranches1410[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1411,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1639(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 139;
}
x86Token x86Parser::tokenBranches1638[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1639,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1002(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1410(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1638(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1001[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1002, x86Parser::tokenBranches1002 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1410, x86Parser::tokenBranches1410 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1638, x86Parser::tokenBranches1638 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1207(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 103;
}
x86Token x86Parser::tokenBranches989[] = {
	{x86Token::TOKEN, 5, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches990 },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches1001 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1207,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc576(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc654(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc732(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc989(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches575[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc576, x86Parser::tokenBranches576 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc654, x86Parser::tokenBranches654 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc732, x86Parser::tokenBranches732 },
	{x86Token::REGISTERCLASS, 6, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc989, x86Parser::tokenBranches989 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc831(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 91;
}
x86Token x86Parser::tokenBranches830[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc831,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc830(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches829[] = {
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc830, x86Parser::tokenBranches830 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc822(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 90;
}
x86Token x86Parser::tokenBranches821[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc822,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches829 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc821(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches820[] = {
	{x86Token::NUMBER, 14, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc821, x86Parser::tokenBranches821 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc493(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 52;
}
x86Token x86Parser::tokenBranches492[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc493,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches575 },
	{x86Token::TOKEN, 5, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches820 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc536(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 61;
}
x86Token x86Parser::tokenBranches535[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc536,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc13(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc83(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc492(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc535(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches12[] = {
	{x86Token::REGISTER, 14, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches40 },
	{x86Token::REGISTER, 18, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches47 },
	{x86Token::REGISTER, 17, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches482 },
	{x86Token::REGISTER, 19, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches487 },
	{x86Token::NUMBER, 5, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc13, x86Parser::tokenBranches13 },
	{x86Token::REGISTERCLASS, 11, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc83, x86Parser::tokenBranches83 },
	{x86Token::REGISTERCLASS, 5, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc492, x86Parser::tokenBranches492 },
	{x86Token::NUMBER, 6, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc535, x86Parser::tokenBranches535 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches11[] = {
	{x86Token::TOKEN, 3, 0, 0, x86Parser::RegClassData1,  NULL, x86Parser::tokenBranches12 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc19(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 3;
}
x86Token x86Parser::tokenBranches18[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc19,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc236(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 23;
}
x86Token x86Parser::tokenBranches235[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc236,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc382(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 38;
}
x86Token x86Parser::tokenBranches381[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc382,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc235(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc381(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches234[] = {
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc235, x86Parser::tokenBranches235 },
	{x86Token::NUMBER, 3, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc381, x86Parser::tokenBranches381 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc64(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 8;
}
x86Token x86Parser::tokenBranches63[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc64,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches234 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc151(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 15;
}
x86Token x86Parser::tokenBranches150[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc151,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc297(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 30;
}
x86Token x86Parser::tokenBranches296[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc297,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc443(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 45;
}
x86Token x86Parser::tokenBranches442[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc443,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc63(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc150(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc296(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc442(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches62[] = {
	{x86Token::REGISTERCLASS, 11, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc63, x86Parser::tokenBranches63 },
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc150, x86Parser::tokenBranches150 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc296, x86Parser::tokenBranches296 },
	{x86Token::NUMBER, 3, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc442, x86Parser::tokenBranches442 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc107(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 15;
}
x86Token x86Parser::tokenBranches61[] = {
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches62 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc107,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc245(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 24;
}
x86Token x86Parser::tokenBranches244[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc245,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc391(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 39;
}
x86Token x86Parser::tokenBranches390[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc391,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc244(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc390(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches243[] = {
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc244, x86Parser::tokenBranches244 },
	{x86Token::NUMBER, 3, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc390, x86Parser::tokenBranches390 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc71(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 9;
}
x86Token x86Parser::tokenBranches70[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc71,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches243 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc177(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 18;
}
x86Token x86Parser::tokenBranches176[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc177,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc323(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 33;
}
x86Token x86Parser::tokenBranches322[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc323,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc469(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 48;
}
x86Token x86Parser::tokenBranches468[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc469,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc70(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc176(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc322(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc468(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches69[] = {
	{x86Token::REGISTERCLASS, 11, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc70, x86Parser::tokenBranches70 },
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc176, x86Parser::tokenBranches176 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc322, x86Parser::tokenBranches322 },
	{x86Token::NUMBER, 3, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc468, x86Parser::tokenBranches468 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc125(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 18;
}
x86Token x86Parser::tokenBranches68[] = {
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches69 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc125,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc271(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 27;
}
x86Token x86Parser::tokenBranches270[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc271,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc417(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 42;
}
x86Token x86Parser::tokenBranches416[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc417,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc270(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc416(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches269[] = {
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc270, x86Parser::tokenBranches270 },
	{x86Token::NUMBER, 3, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc416, x86Parser::tokenBranches416 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc89(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 12;
}
x86Token x86Parser::tokenBranches88[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc89,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches269 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc605(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 56;
}
x86Token x86Parser::tokenBranches604[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc605,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc683(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 71;
}
x86Token x86Parser::tokenBranches682[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc683,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc768(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 82;
}
x86Token x86Parser::tokenBranches767[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc768,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1094(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 110;
}
x86Token x86Parser::tokenBranches1093[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1094,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1502(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 128;
}
x86Token x86Parser::tokenBranches1501[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1502,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1730(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 146;
}
x86Token x86Parser::tokenBranches1729[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1730,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1093(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1501(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1729(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1092[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1093, x86Parser::tokenBranches1093 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1501, x86Parser::tokenBranches1501 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1729, x86Parser::tokenBranches1729 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1280(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 110;
}
x86Token x86Parser::tokenBranches1091[] = {
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches1092 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1280,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1091(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1090[] = {
	{x86Token::NUMBER, 14, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1091, x86Parser::tokenBranches1091 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1103(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 111;
}
x86Token x86Parser::tokenBranches1102[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1103,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1511(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 129;
}
x86Token x86Parser::tokenBranches1510[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1511,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1739(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 147;
}
x86Token x86Parser::tokenBranches1738[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1739,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1102(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1510(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1738(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1101[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1102, x86Parser::tokenBranches1102 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1510, x86Parser::tokenBranches1510 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1738, x86Parser::tokenBranches1738 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1287(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 111;
}
x86Token x86Parser::tokenBranches1089[] = {
	{x86Token::TOKEN, 5, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches1090 },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches1101 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1287,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc604(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc682(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc767(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1089(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches603[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc604, x86Parser::tokenBranches604 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc682, x86Parser::tokenBranches682 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc767, x86Parser::tokenBranches767 },
	{x86Token::REGISTERCLASS, 6, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1089, x86Parser::tokenBranches1089 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc513(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 56;
}
x86Token x86Parser::tokenBranches512[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc513,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches603 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc612(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 57;
}
x86Token x86Parser::tokenBranches611[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc612,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc690(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 72;
}
x86Token x86Parser::tokenBranches689[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc690,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc761(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 81;
}
x86Token x86Parser::tokenBranches760[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc761,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1074(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 108;
}
x86Token x86Parser::tokenBranches1073[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1074,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1482(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 126;
}
x86Token x86Parser::tokenBranches1481[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1482,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1710(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 144;
}
x86Token x86Parser::tokenBranches1709[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1710,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1073(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1481(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1709(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1072[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1073, x86Parser::tokenBranches1073 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1481, x86Parser::tokenBranches1481 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1709, x86Parser::tokenBranches1709 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1264(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 108;
}
x86Token x86Parser::tokenBranches1071[] = {
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches1072 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1264,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1071(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1070[] = {
	{x86Token::NUMBER, 14, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1071, x86Parser::tokenBranches1071 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1083(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 109;
}
x86Token x86Parser::tokenBranches1082[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1083,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1491(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 127;
}
x86Token x86Parser::tokenBranches1490[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1491,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1719(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 145;
}
x86Token x86Parser::tokenBranches1718[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1719,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1082(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1490(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1718(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1081[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1082, x86Parser::tokenBranches1082 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1490, x86Parser::tokenBranches1490 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1718, x86Parser::tokenBranches1718 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1271(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 109;
}
x86Token x86Parser::tokenBranches1069[] = {
	{x86Token::TOKEN, 5, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches1070 },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches1081 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1271,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc611(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc689(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc760(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1069(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches610[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc611, x86Parser::tokenBranches611 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc689, x86Parser::tokenBranches689 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc760, x86Parser::tokenBranches760 },
	{x86Token::REGISTERCLASS, 6, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1069, x86Parser::tokenBranches1069 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc879(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 93;
}
x86Token x86Parser::tokenBranches878[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc879,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc878(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches877[] = {
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc878, x86Parser::tokenBranches878 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc870(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 92;
}
x86Token x86Parser::tokenBranches869[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc870,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches877 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc869(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches868[] = {
	{x86Token::NUMBER, 14, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc869, x86Parser::tokenBranches869 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc518(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 57;
}
x86Token x86Parser::tokenBranches517[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc518,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches610 },
	{x86Token::TOKEN, 5, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches868 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc619(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 58;
}
x86Token x86Parser::tokenBranches618[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc619,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc697(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 73;
}
x86Token x86Parser::tokenBranches696[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc697,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc775(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 83;
}
x86Token x86Parser::tokenBranches774[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc775,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1114(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 112;
}
x86Token x86Parser::tokenBranches1113[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1114,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1522(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 130;
}
x86Token x86Parser::tokenBranches1521[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1522,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1750(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 148;
}
x86Token x86Parser::tokenBranches1749[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1750,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1113(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1521(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1749(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1112[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1113, x86Parser::tokenBranches1113 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1521, x86Parser::tokenBranches1521 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1749, x86Parser::tokenBranches1749 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1296(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 112;
}
x86Token x86Parser::tokenBranches1111[] = {
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches1112 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1296,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1111(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1110[] = {
	{x86Token::NUMBER, 14, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1111, x86Parser::tokenBranches1111 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1123(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 113;
}
x86Token x86Parser::tokenBranches1122[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1123,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1531(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 131;
}
x86Token x86Parser::tokenBranches1530[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1531,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1759(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 149;
}
x86Token x86Parser::tokenBranches1758[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1759,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1122(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1530(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1758(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1121[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1122, x86Parser::tokenBranches1122 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1530, x86Parser::tokenBranches1530 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1758, x86Parser::tokenBranches1758 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1303(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 113;
}
x86Token x86Parser::tokenBranches1109[] = {
	{x86Token::TOKEN, 5, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches1110 },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches1121 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1303,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc618(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc696(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc774(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1109(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches617[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc618, x86Parser::tokenBranches618 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc696, x86Parser::tokenBranches696 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc774, x86Parser::tokenBranches774 },
	{x86Token::REGISTERCLASS, 6, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1109, x86Parser::tokenBranches1109 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc895(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 95;
}
x86Token x86Parser::tokenBranches894[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc895,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc894(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches893[] = {
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc894, x86Parser::tokenBranches894 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc886(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 94;
}
x86Token x86Parser::tokenBranches885[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc886,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches893 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc885(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches884[] = {
	{x86Token::NUMBER, 14, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc885, x86Parser::tokenBranches885 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc523(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 58;
}
x86Token x86Parser::tokenBranches522[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc523,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches617 },
	{x86Token::TOKEN, 5, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches884 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc541(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 62;
}
x86Token x86Parser::tokenBranches540[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc541,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc18(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc88(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc522(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc540(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches17[] = {
	{x86Token::REGISTER, 14, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches61 },
	{x86Token::REGISTER, 18, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches68 },
	{x86Token::REGISTER, 17, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches512 },
	{x86Token::REGISTER, 19, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches517 },
	{x86Token::NUMBER, 5, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc18, x86Parser::tokenBranches18 },
	{x86Token::REGISTERCLASS, 11, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc88, x86Parser::tokenBranches88 },
	{x86Token::REGISTERCLASS, 5, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc522, x86Parser::tokenBranches522 },
	{x86Token::NUMBER, 6, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc540, x86Parser::tokenBranches540 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches16[] = {
	{x86Token::TOKEN, 3, 0, 0, x86Parser::RegClassData1,  NULL, x86Parser::tokenBranches17 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc184(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 19;
}
x86Token x86Parser::tokenBranches183[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc184,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc330(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 34;
}
x86Token x86Parser::tokenBranches329[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc330,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc183(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc329(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches182[] = {
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc183, x86Parser::tokenBranches183 },
	{x86Token::NUMBER, 3, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc329, x86Parser::tokenBranches329 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc24(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 4;
}
x86Token x86Parser::tokenBranches23[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc24,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches182 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc130(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 13;
}
x86Token x86Parser::tokenBranches129[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc130,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc276(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 28;
}
x86Token x86Parser::tokenBranches275[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc276,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc422(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 43;
}
x86Token x86Parser::tokenBranches421[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc422,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc23(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc129(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc275(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc421(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches22[] = {
	{x86Token::REGISTERCLASS, 11, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc23, x86Parser::tokenBranches23 },
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc129, x86Parser::tokenBranches129 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc275, x86Parser::tokenBranches275 },
	{x86Token::NUMBER, 3, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc421, x86Parser::tokenBranches421 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc92(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 13;
}
x86Token x86Parser::tokenBranches21[] = {
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches22 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc92,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc191(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 20;
}
x86Token x86Parser::tokenBranches190[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc191,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc337(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 35;
}
x86Token x86Parser::tokenBranches336[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc337,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc190(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc336(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches189[] = {
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc190, x86Parser::tokenBranches190 },
	{x86Token::NUMBER, 3, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc336, x86Parser::tokenBranches336 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc29(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 5;
}
x86Token x86Parser::tokenBranches28[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc29,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches189 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc156(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 16;
}
x86Token x86Parser::tokenBranches155[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc156,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc302(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 31;
}
x86Token x86Parser::tokenBranches301[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc302,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc448(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 46;
}
x86Token x86Parser::tokenBranches447[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc448,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc28(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc155(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc301(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc447(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches27[] = {
	{x86Token::REGISTERCLASS, 11, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc28, x86Parser::tokenBranches28 },
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc155, x86Parser::tokenBranches155 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc301, x86Parser::tokenBranches301 },
	{x86Token::NUMBER, 3, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc447, x86Parser::tokenBranches447 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc110(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 16;
}
x86Token x86Parser::tokenBranches26[] = {
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches27 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc110,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc250(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 25;
}
x86Token x86Parser::tokenBranches249[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc250,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc396(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 40;
}
x86Token x86Parser::tokenBranches395[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc396,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc249(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc395(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches248[] = {
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc249, x86Parser::tokenBranches249 },
	{x86Token::NUMBER, 3, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc395, x86Parser::tokenBranches395 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc74(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 10;
}
x86Token x86Parser::tokenBranches73[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc74,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches248 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc546(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 49;
}
x86Token x86Parser::tokenBranches545[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc546,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc624(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 63;
}
x86Token x86Parser::tokenBranches623[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc624,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc707(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 75;
}
x86Token x86Parser::tokenBranches706[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc707,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc920(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 98;
}
x86Token x86Parser::tokenBranches919[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc920,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1328(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 116;
}
x86Token x86Parser::tokenBranches1327[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1328,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1556(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 134;
}
x86Token x86Parser::tokenBranches1555[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1556,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc919(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1327(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1555(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches918[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc919, x86Parser::tokenBranches919 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1327, x86Parser::tokenBranches1327 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1555, x86Parser::tokenBranches1555 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1142(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 98;
}
x86Token x86Parser::tokenBranches917[] = {
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches918 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1142,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc917(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches916[] = {
	{x86Token::NUMBER, 14, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc917, x86Parser::tokenBranches917 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc927(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 99;
}
x86Token x86Parser::tokenBranches926[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc927,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1335(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 117;
}
x86Token x86Parser::tokenBranches1334[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1335,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1563(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 135;
}
x86Token x86Parser::tokenBranches1562[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1563,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc926(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1334(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1562(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches925[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc926, x86Parser::tokenBranches926 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1334, x86Parser::tokenBranches1334 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1562, x86Parser::tokenBranches1562 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1147(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 99;
}
x86Token x86Parser::tokenBranches915[] = {
	{x86Token::TOKEN, 5, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches916 },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches925 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1147,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc545(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc623(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc706(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc915(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches544[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc545, x86Parser::tokenBranches545 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc623, x86Parser::tokenBranches623 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc706, x86Parser::tokenBranches706 },
	{x86Token::REGISTERCLASS, 6, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc915, x86Parser::tokenBranches915 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc472(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 49;
}
x86Token x86Parser::tokenBranches471[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc472,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches544 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc551(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 50;
}
x86Token x86Parser::tokenBranches550[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc551,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc629(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 64;
}
x86Token x86Parser::tokenBranches628[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc629,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc702(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 74;
}
x86Token x86Parser::tokenBranches701[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc702,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc904(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 96;
}
x86Token x86Parser::tokenBranches903[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc904,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1312(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 114;
}
x86Token x86Parser::tokenBranches1311[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1312,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1540(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 132;
}
x86Token x86Parser::tokenBranches1539[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1540,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc903(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1311(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1539(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches902[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc903, x86Parser::tokenBranches903 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1311, x86Parser::tokenBranches1311 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1539, x86Parser::tokenBranches1539 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1130(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 96;
}
x86Token x86Parser::tokenBranches901[] = {
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches902 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1130,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc901(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches900[] = {
	{x86Token::NUMBER, 14, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc901, x86Parser::tokenBranches901 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc911(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 97;
}
x86Token x86Parser::tokenBranches910[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc911,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1319(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 115;
}
x86Token x86Parser::tokenBranches1318[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1319,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1547(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 133;
}
x86Token x86Parser::tokenBranches1546[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1547,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc910(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1318(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1546(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches909[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc910, x86Parser::tokenBranches910 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1318, x86Parser::tokenBranches1318 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1546, x86Parser::tokenBranches1546 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1135(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 97;
}
x86Token x86Parser::tokenBranches899[] = {
	{x86Token::TOKEN, 5, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches900 },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches909 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1135,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc550(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc628(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc701(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc899(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches549[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc550, x86Parser::tokenBranches550 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc628, x86Parser::tokenBranches628 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc701, x86Parser::tokenBranches701 },
	{x86Token::REGISTERCLASS, 6, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc899, x86Parser::tokenBranches899 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc787(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 85;
}
x86Token x86Parser::tokenBranches786[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc787,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc786(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches785[] = {
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc786, x86Parser::tokenBranches786 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc780(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 84;
}
x86Token x86Parser::tokenBranches779[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc780,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches785 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc779(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches778[] = {
	{x86Token::NUMBER, 14, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc779, x86Parser::tokenBranches779 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc475(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 50;
}
x86Token x86Parser::tokenBranches474[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc475,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches549 },
	{x86Token::TOKEN, 5, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches778 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc556(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 51;
}
x86Token x86Parser::tokenBranches555[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc556,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc634(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 65;
}
x86Token x86Parser::tokenBranches633[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc634,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc712(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 76;
}
x86Token x86Parser::tokenBranches711[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc712,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc936(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 100;
}
x86Token x86Parser::tokenBranches935[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc936,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1344(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 118;
}
x86Token x86Parser::tokenBranches1343[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1344,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1572(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 136;
}
x86Token x86Parser::tokenBranches1571[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1572,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc935(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1343(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1571(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches934[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc935, x86Parser::tokenBranches935 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1343, x86Parser::tokenBranches1343 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1571, x86Parser::tokenBranches1571 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1154(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 100;
}
x86Token x86Parser::tokenBranches933[] = {
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches934 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1154,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc933(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches932[] = {
	{x86Token::NUMBER, 14, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc933, x86Parser::tokenBranches933 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc943(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 101;
}
x86Token x86Parser::tokenBranches942[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc943,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1351(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 119;
}
x86Token x86Parser::tokenBranches1350[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1351,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1579(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 137;
}
x86Token x86Parser::tokenBranches1578[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1579,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc942(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1350(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1578(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches941[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc942, x86Parser::tokenBranches942 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1350, x86Parser::tokenBranches1350 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1578, x86Parser::tokenBranches1578 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1159(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 101;
}
x86Token x86Parser::tokenBranches931[] = {
	{x86Token::TOKEN, 5, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches932 },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches941 },
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc1159,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc555(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc633(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc711(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc931(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches554[] = {
	{x86Token::NUMBER, 15, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc555, x86Parser::tokenBranches555 },
	{x86Token::NUMBER, 0, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc633, x86Parser::tokenBranches633 },
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc711, x86Parser::tokenBranches711 },
	{x86Token::REGISTERCLASS, 6, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc931, x86Parser::tokenBranches931 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc799(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 87;
}
x86Token x86Parser::tokenBranches798[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc799,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc798(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches797[] = {
	{x86Token::NUMBER, 4, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc798, x86Parser::tokenBranches798 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc792(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 86;
}
x86Token x86Parser::tokenBranches791[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc792,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches797 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc791(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches790[] = {
	{x86Token::NUMBER, 14, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc791, x86Parser::tokenBranches791 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc478(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 51;
}
x86Token x86Parser::tokenBranches477[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc478,  },
	{x86Token::TOKEN, 4, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches554 },
	{x86Token::TOKEN, 5, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches790 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc526(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 59;
}
x86Token x86Parser::tokenBranches525[] = {
	{x86Token::TOKEN, 2, 1, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc526,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc16(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc73(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc477(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc525(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2[] = {
	{x86Token::REGISTER, 27, 0, 0, x86Parser::RegClassData1,  NULL, x86Parser::tokenBranches6 },
	{x86Token::REGISTER, 26, 0, 0, x86Parser::RegClassData1,  NULL, x86Parser::tokenBranches11 },
	{x86Token::REGISTER, 14, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches21 },
	{x86Token::REGISTER, 18, 0, 0, x86Parser::RegClassData2,  NULL, x86Parser::tokenBranches26 },
	{x86Token::REGISTER, 17, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches471 },
	{x86Token::REGISTER, 19, 0, 0, x86Parser::RegClassData3,  NULL, x86Parser::tokenBranches474 },
	{x86Token::NUMBER, 5, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc3, x86Parser::tokenBranches3 },
	{x86Token::REGISTERCLASS, 12, 0, 0, x86Parser::RegClassData1, &x86Parser::TokenFunc16, x86Parser::tokenBranches16 },
	{x86Token::REGISTERCLASS, 11, 0, 0, x86Parser::RegClassData2, &x86Parser::TokenFunc73, x86Parser::tokenBranches73 },
	{x86Token::REGISTERCLASS, 5, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc477, x86Parser::tokenBranches477 },
	{x86Token::NUMBER, 6, 0, 0, x86Parser::RegClassData3, &x86Parser::TokenFunc525, x86Parser::tokenBranches525 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1760(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 150;
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
void x86Parser::TokenFunc1761(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 150;
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
void x86Parser::TokenFunc1762(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 150;
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
void x86Parser::TokenFunc1763(x86Operand &operand, int tokenPos)
{
	operand.addressCoding = 150;
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
x86Token x86Parser::tokenBranches1[] = {
	{x86Token::TOKEN, 1, 0, 0, x86Parser::RegClassData1,  NULL, x86Parser::tokenBranches2 },
	{x86Token::REGISTERCLASS, 1, 1, 0, x86Parser::RegClassData4, &x86Parser::TokenFunc1760,  },
	{x86Token::REGISTERCLASS, 4, 1, 0, x86Parser::RegClassData5, &x86Parser::TokenFunc1761,  },
	{x86Token::REGISTERCLASS, 7, 1, 0, x86Parser::RegClassData6, &x86Parser::TokenFunc1762,  },
	{x86Token::REGISTERCLASS, 13, 1, 0, x86Parser::RegClassData7, &x86Parser::TokenFunc1763,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1765(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 151;
}
x86Token x86Parser::tokenBranches1764[] = {
	{x86Token::EMPTY, 0, 1, 0, NULL,&x86Parser::TokenFunc1765,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1767(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 152;
}
x86Token x86Parser::tokenBranches1766[] = {
	{x86Token::EMPTY, 0, 1, 0, NULL,&x86Parser::TokenFunc1767,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1772_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding1772_9[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::indirect, 23, -1, 0 },
	{ Coding::eot },
};
void x86Parser::TokenFunc1772(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1772_8;
	operand.values[9] = tokenCoding1772_9;
}
x86Token x86Parser::tokenBranches1771[] = {
	{x86Token::ADDRESSCLASS, 4, 1, 0, NULL,&x86Parser::TokenFunc1772,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1770[] = {
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches1771 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1769[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1770 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1776_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding1776_9[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::indirect, 23, -1, 0 },
	{ Coding::eot },
};
void x86Parser::TokenFunc1776(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1776_8;
	operand.values[9] = tokenCoding1776_9;
}
x86Token x86Parser::tokenBranches1775[] = {
	{x86Token::ADDRESSCLASS, 5, 1, 0, NULL,&x86Parser::TokenFunc1776,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1774[] = {
	{x86Token::TOKEN, 9, 0, 1, NULL, NULL, x86Parser::tokenBranches1775 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1773[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1774 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1769(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1773(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1768[] = {
	{x86Token::REGISTERCLASS, 4, 0, 0, NULL,&x86Parser::TokenFunc1769, x86Parser::tokenBranches1769 },
	{x86Token::REGISTERCLASS, 7, 0, 0, NULL,&x86Parser::TokenFunc1773, x86Parser::tokenBranches1773 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1781_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding1781_9[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::indirect, 23, -1, 0 },
	{ Coding::eot },
};
void x86Parser::TokenFunc1781(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1781_8;
	operand.values[9] = tokenCoding1781_9;
}
x86Token x86Parser::tokenBranches1780[] = {
	{x86Token::ADDRESSCLASS, 1, 1, 0, NULL,&x86Parser::TokenFunc1781,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1779[] = {
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches1780 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1778[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1779 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1785_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding1785_9[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::indirect, 23, -1, 0 },
	{ Coding::eot },
};
void x86Parser::TokenFunc1785(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1785_8;
	operand.values[9] = tokenCoding1785_9;
}
x86Token x86Parser::tokenBranches1784[] = {
	{x86Token::ADDRESSCLASS, 1, 1, 0, NULL,&x86Parser::TokenFunc1785,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1783[] = {
	{x86Token::TOKEN, 9, 0, 1, NULL, NULL, x86Parser::tokenBranches1784 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1782[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1783 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1778(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1782(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1777[] = {
	{x86Token::REGISTERCLASS, 4, 0, 0, NULL,&x86Parser::TokenFunc1778, x86Parser::tokenBranches1778 },
	{x86Token::REGISTERCLASS, 7, 0, 0, NULL,&x86Parser::TokenFunc1782, x86Parser::tokenBranches1782 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1790(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1789[] = {
	{x86Token::NUMBER, 2, 1, 0, NULL,&x86Parser::TokenFunc1790,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1788[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches1789 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1787[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1788 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1794(x86Operand &operand, int tokenPos)
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
}
x86Token x86Parser::tokenBranches1793[] = {
	{x86Token::NUMBER, 3, 1, 0, NULL,&x86Parser::TokenFunc1794,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1792[] = {
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches1793 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1791[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1792 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1798(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 155;
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
x86Token x86Parser::tokenBranches1797[] = {
	{x86Token::NUMBER, 4, 1, 0, NULL,&x86Parser::TokenFunc1798,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1796[] = {
	{x86Token::TOKEN, 9, 0, 1, NULL, NULL, x86Parser::tokenBranches1797 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1795[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1796 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1802_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding1802_9[] = {
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 24, 8, 0, 43, 2 },
	{ Coding::eot },
};
void x86Parser::TokenFunc1802(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1802_8;
	operand.values[9] = tokenCoding1802_9;
}
x86Token x86Parser::tokenBranches1801[] = {
	{x86Token::ADDRESSCLASS, 3, 1, 0, NULL,&x86Parser::TokenFunc1802,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1800[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches1801 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1799[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1800 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1806_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding1806_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 24, 8, 0, 43, 3 },
	{ Coding::eot },
};
void x86Parser::TokenFunc1806(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1806_8;
	operand.values[9] = tokenCoding1806_9;
}
x86Token x86Parser::tokenBranches1805[] = {
	{x86Token::ADDRESSCLASS, 4, 1, 0, NULL,&x86Parser::TokenFunc1806,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1804[] = {
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches1805 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1803[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1804 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1810_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding1810_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 24, 8, 0, 43, 3 },
	{ Coding::eot },
};
void x86Parser::TokenFunc1810(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1810_8;
	operand.values[9] = tokenCoding1810_9;
}
x86Token x86Parser::tokenBranches1809[] = {
	{x86Token::ADDRESSCLASS, 5, 1, 0, NULL,&x86Parser::TokenFunc1810,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1808[] = {
	{x86Token::TOKEN, 9, 0, 1, NULL, NULL, x86Parser::tokenBranches1809 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1807[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1808 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1814_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding1814_9[] = {
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 24, 8, 0 },
	{ Coding::eot },
};
void x86Parser::TokenFunc1814(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1813[] = {
	{x86Token::REGISTERCLASS, 1, 1, 0, NULL,&x86Parser::TokenFunc1814,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1812[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1813 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1811[] = {
	{x86Token::ADDRESSCLASS, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches1812 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1818_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding1818_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 24, 8, 0, 43, 1 },
	{ Coding::eot },
};
void x86Parser::TokenFunc1818(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1817[] = {
	{x86Token::REGISTERCLASS, 4, 1, 0, NULL,&x86Parser::TokenFunc1818,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1816[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1817 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1815[] = {
	{x86Token::ADDRESSCLASS, 4, 0, 0, NULL, NULL, x86Parser::tokenBranches1816 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1822_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding1822_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 24, 8, 0, 43, 1 },
	{ Coding::eot },
};
void x86Parser::TokenFunc1822(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1821[] = {
	{x86Token::REGISTERCLASS, 7, 1, 0, NULL,&x86Parser::TokenFunc1822,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1820[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1821 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1819[] = {
	{x86Token::ADDRESSCLASS, 5, 0, 0, NULL, NULL, x86Parser::tokenBranches1820 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1827_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 128, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc1827(x86Operand &operand, int tokenPos)
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
	operand.values[9] = tokenCoding1827_9;
}
x86Token x86Parser::tokenBranches1826[] = {
	{x86Token::NUMBER, 2, 1, 0, NULL,&x86Parser::TokenFunc1827,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1825[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches1826 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1824[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1825 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1823[] = {
	{x86Token::ADDRESSCLASS, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches1824 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1832_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 131, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc1832(x86Operand &operand, int tokenPos)
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
	operand.values[9] = tokenCoding1832_9;
}
x86Token x86Parser::tokenBranches1831[] = {
	{x86Token::NUMBER, 0, 1, 0, NULL,&x86Parser::TokenFunc1832,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1837_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 129, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc1837(x86Operand &operand, int tokenPos)
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
	operand.values[9] = tokenCoding1837_9;
}
x86Token x86Parser::tokenBranches1836[] = {
	{x86Token::NUMBER, 3, 1, 0, NULL,&x86Parser::TokenFunc1837,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1830[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches1831 },
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches1836 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1829[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1830 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1828[] = {
	{x86Token::ADDRESSCLASS, 4, 0, 0, NULL, NULL, x86Parser::tokenBranches1829 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1842_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 131, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc1842(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 159;
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
x86Token x86Parser::tokenBranches1841[] = {
	{x86Token::NUMBER, 0, 1, 0, NULL,&x86Parser::TokenFunc1842,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1847_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 129, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc1847(x86Operand &operand, int tokenPos)
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
	operand.values[9] = tokenCoding1847_9;
}
x86Token x86Parser::tokenBranches1846[] = {
	{x86Token::NUMBER, 4, 1, 0, NULL,&x86Parser::TokenFunc1847,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1840[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches1841 },
	{x86Token::TOKEN, 9, 0, 1, NULL, NULL, x86Parser::tokenBranches1846 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1839[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1840 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1838[] = {
	{x86Token::ADDRESSCLASS, 5, 0, 0, NULL, NULL, x86Parser::tokenBranches1839 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1851_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 128, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc1851(x86Operand &operand, int tokenPos)
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
	operand.values[9] = tokenCoding1851_9;
}
x86Token x86Parser::tokenBranches1850[] = {
	{x86Token::NUMBER, 2, 1, 0, NULL,&x86Parser::TokenFunc1851,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1849[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches1850 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1848[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1849 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1855_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 131, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc1855(x86Operand &operand, int tokenPos)
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
	operand.values[9] = tokenCoding1855_9;
}
x86Token x86Parser::tokenBranches1854[] = {
	{x86Token::NUMBER, 0, 1, 0, NULL,&x86Parser::TokenFunc1855,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1859_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 129, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc1859(x86Operand &operand, int tokenPos)
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
	operand.values[9] = tokenCoding1859_9;
}
x86Token x86Parser::tokenBranches1858[] = {
	{x86Token::NUMBER, 3, 1, 0, NULL,&x86Parser::TokenFunc1859,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1853[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches1854 },
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches1858 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1852[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1853 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1863_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 131, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc1863(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 159;
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
x86Token x86Parser::tokenBranches1862[] = {
	{x86Token::NUMBER, 0, 1, 0, NULL,&x86Parser::TokenFunc1863,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1867_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 129, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc1867(x86Operand &operand, int tokenPos)
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
	operand.values[9] = tokenCoding1867_9;
}
x86Token x86Parser::tokenBranches1866[] = {
	{x86Token::NUMBER, 4, 1, 0, NULL,&x86Parser::TokenFunc1867,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1861[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches1862 },
	{x86Token::TOKEN, 9, 0, 1, NULL, NULL, x86Parser::tokenBranches1866 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1860[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1861 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1799(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1803(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1807(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1786[] = {
	{x86Token::REGISTER, 0, 0, 0, NULL, NULL, x86Parser::tokenBranches1787 },
	{x86Token::REGISTER, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches1791 },
	{x86Token::REGISTER, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches1795 },
	{x86Token::REGISTERCLASS, 1, 0, 0, NULL,&x86Parser::TokenFunc1799, x86Parser::tokenBranches1799 },
	{x86Token::REGISTERCLASS, 4, 0, 0, NULL,&x86Parser::TokenFunc1803, x86Parser::tokenBranches1803 },
	{x86Token::REGISTERCLASS, 7, 0, 0, NULL,&x86Parser::TokenFunc1807, x86Parser::tokenBranches1807 },
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches1811 },
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches1815 },
	{x86Token::TOKEN, 9, 0, 1, NULL, NULL, x86Parser::tokenBranches1819 },
	{x86Token::TOKEN, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches1823 },
	{x86Token::TOKEN, 8, 0, 0, NULL, NULL, x86Parser::tokenBranches1828 },
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches1838 },
	{x86Token::ADDRESSCLASS, 8, 0, 0, NULL, NULL, x86Parser::tokenBranches1848 },
	{x86Token::ADDRESSCLASS, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches1852 },
	{x86Token::ADDRESSCLASS, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches1860 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1873_9[] = {
	{ Coding::indirect, 23, -1, 0 },
	{ Coding::eot },
};
void x86Parser::TokenFunc1873(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 161;
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
x86Token x86Parser::tokenBranches1872[] = {
	{x86Token::NUMBER, 2, 1, 0, NULL,&x86Parser::TokenFunc1873,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1882_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding1882_9[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::indirect, 24, -1, 0 },
	{ Coding::eot },
};
void x86Parser::TokenFunc1882(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding1882_8;
	operand.values[9] = tokenCoding1882_9;
}
x86Token x86Parser::tokenBranches1871[] = {
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches1872 },
	{x86Token::REGISTERCLASS, 4, 1, 0, NULL,&x86Parser::TokenFunc1882,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1870[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1871 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1869[] = {
	{x86Token::ADDRESSCLASS, 4, 0, 0, NULL, NULL, x86Parser::tokenBranches1870 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1878_9[] = {
	{ Coding::indirect, 23, -1, 0 },
	{ Coding::eot },
};
void x86Parser::TokenFunc1878(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 162;
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
x86Token x86Parser::tokenBranches1877[] = {
	{x86Token::NUMBER, 2, 1, 0, NULL,&x86Parser::TokenFunc1878,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1886_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding1886_9[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::indirect, 24, -1, 0 },
	{ Coding::eot },
};
void x86Parser::TokenFunc1886(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding1886_8;
	operand.values[9] = tokenCoding1886_9;
}
x86Token x86Parser::tokenBranches1876[] = {
	{x86Token::TOKEN, 9, 0, 1, NULL, NULL, x86Parser::tokenBranches1877 },
	{x86Token::REGISTERCLASS, 7, 1, 0, NULL,&x86Parser::TokenFunc1886,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1875[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1876 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1874[] = {
	{x86Token::ADDRESSCLASS, 5, 0, 0, NULL, NULL, x86Parser::tokenBranches1875 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1868[] = {
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches1869 },
	{x86Token::TOKEN, 9, 0, 1, NULL, NULL, x86Parser::tokenBranches1874 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1892_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 254, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc1892(x86Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1892_9;
}
x86Token x86Parser::tokenBranches1891[] = {
	{x86Token::ADDRESSCLASS, 3, 1, 0, NULL,&x86Parser::TokenFunc1892,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1894_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc1894(x86Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1894_9;
}
x86Token x86Parser::tokenBranches1893[] = {
	{x86Token::ADDRESSCLASS, 4, 1, 0, NULL,&x86Parser::TokenFunc1894,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1896_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc1896(x86Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1896_9;
}
x86Token x86Parser::tokenBranches1895[] = {
	{x86Token::ADDRESSCLASS, 5, 1, 0, NULL,&x86Parser::TokenFunc1896,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1888_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 254, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc1888(x86Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1888_9;
}
void x86Parser::TokenFunc1889(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 163;
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
void x86Parser::TokenFunc1890(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 164;
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
x86Token x86Parser::tokenBranches1887[] = {
	{x86Token::ADDRESSCLASS, 8, 1, 0, NULL,&x86Parser::TokenFunc1888,  },
	{x86Token::REGISTERCLASS, 4, 1, 0, NULL,&x86Parser::TokenFunc1889,  },
	{x86Token::REGISTERCLASS, 7, 1, 0, NULL,&x86Parser::TokenFunc1890,  },
	{x86Token::TOKEN, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches1891 },
	{x86Token::TOKEN, 8, 0, 0, NULL, NULL, x86Parser::tokenBranches1893 },
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches1895 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1902_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 246, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc1902(x86Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1902_9;
}
x86Token x86Parser::tokenBranches1901[] = {
	{x86Token::ADDRESSCLASS, 3, 1, 0, NULL,&x86Parser::TokenFunc1902,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1904_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 247, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc1904(x86Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1904_9;
}
x86Token x86Parser::tokenBranches1903[] = {
	{x86Token::ADDRESSCLASS, 4, 1, 0, NULL,&x86Parser::TokenFunc1904,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1906_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 247, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc1906(x86Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1906_9;
}
x86Token x86Parser::tokenBranches1905[] = {
	{x86Token::ADDRESSCLASS, 5, 1, 0, NULL,&x86Parser::TokenFunc1906,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1898_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 246, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc1898(x86Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1898_9;
}
Coding x86Parser::tokenCoding1899_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 247, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc1899(x86Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1899_9;
}
Coding x86Parser::tokenCoding1900_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 247, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc1900(x86Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1900_9;
}
x86Token x86Parser::tokenBranches1897[] = {
	{x86Token::ADDRESSCLASS, 8, 1, 0, NULL,&x86Parser::TokenFunc1898,  },
	{x86Token::ADDRESSCLASS, 9, 1, 0, NULL,&x86Parser::TokenFunc1899,  },
	{x86Token::ADDRESSCLASS, 10, 1, 0, NULL,&x86Parser::TokenFunc1900,  },
	{x86Token::TOKEN, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches1901 },
	{x86Token::TOKEN, 8, 0, 0, NULL, NULL, x86Parser::tokenBranches1903 },
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches1905 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1910(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1909[] = {
	{x86Token::REGISTERCLASS, 13, 1, 0, NULL,&x86Parser::TokenFunc1910,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1908[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1909 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1913(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 166;
}
x86Token x86Parser::tokenBranches1912[] = {
	{x86Token::REGISTER, 30, 1, 0, NULL,&x86Parser::TokenFunc1913,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1911[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1912 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1916_9[] = {
	{ Coding::indirect, 23, -1, 0 },
	{ Coding::eot },
};
void x86Parser::TokenFunc1916(x86Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1916_9;
}
x86Token x86Parser::tokenBranches1915[] = {
	{x86Token::ADDRESSCLASS, 2, 1, 0, NULL,&x86Parser::TokenFunc1916,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1918_9[] = {
	{ Coding::indirect, 23, -1, 0, 43, 4 },
	{ Coding::eot },
};
void x86Parser::TokenFunc1918(x86Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1918_9;
}
x86Token x86Parser::tokenBranches1917[] = {
	{x86Token::ADDRESSCLASS, 2, 1, 0, NULL,&x86Parser::TokenFunc1918,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1911(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 167;
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
x86Token x86Parser::tokenBranches1907[] = {
	{x86Token::REGISTER, 30, 0, 0, NULL, NULL, x86Parser::tokenBranches1908 },
	{x86Token::REGISTERCLASS, 13, 1, 0, NULL,&x86Parser::TokenFunc1911, x86Parser::tokenBranches1911 },
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches1915 },
	{x86Token::TOKEN, 11, 0, 0, NULL, NULL, x86Parser::tokenBranches1917 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1922(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 150;
}
x86Token x86Parser::tokenBranches1921[] = {
	{x86Token::REGISTER, 30, 1, 0, NULL,&x86Parser::TokenFunc1922,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1920[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1921 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1920(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 150;
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
void x86Parser::TokenFunc1924(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 168;
}
x86Token x86Parser::tokenBranches1919[] = {
	{x86Token::REGISTERCLASS, 13, 1, 0, NULL,&x86Parser::TokenFunc1920, x86Parser::tokenBranches1920 },
	{x86Token::EMPTY, 0, 1, 0, NULL,&x86Parser::TokenFunc1924,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1927_9[] = {
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0, 43, 4 },
	{ Coding::eot },
};
void x86Parser::TokenFunc1927(x86Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1927_9;
}
x86Token x86Parser::tokenBranches1926[] = {
	{x86Token::ADDRESSCLASS, 1, 1, 0, NULL,&x86Parser::TokenFunc1927,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1929_9[] = {
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0 },
	{ Coding::eot },
};
void x86Parser::TokenFunc1929(x86Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1929_9;
}
x86Token x86Parser::tokenBranches1928[] = {
	{x86Token::ADDRESSCLASS, 1, 1, 0, NULL,&x86Parser::TokenFunc1929,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1925[] = {
	{x86Token::TOKEN, 8, 0, 0, NULL, NULL, x86Parser::tokenBranches1926 },
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches1928 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1931[] = {
	{x86Token::ADDRESSCLASS, 0, 1, 0, NULL, NULL,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1930[] = {
	{x86Token::TOKEN, 12, 0, 1, NULL, NULL, x86Parser::tokenBranches1931 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1935(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1934[] = {
	{x86Token::NUMBER, 7, 1, 0, NULL,&x86Parser::TokenFunc1935,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1937(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 170;
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
void x86Parser::TokenFunc1939(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 171;
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
x86Token x86Parser::tokenBranches1936[] = {
	{x86Token::NUMBER, 10, 1, 0, NULL,&x86Parser::TokenFunc1937,  },
	{x86Token::NUMBER, 13, 1, 0, NULL,&x86Parser::TokenFunc1939,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1940(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 170;
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
void x86Parser::TokenFunc1941(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 171;
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
x86Token x86Parser::tokenBranches1933[] = {
	{x86Token::TOKEN, 13, 0, 0, NULL, NULL, x86Parser::tokenBranches1934 },
	{x86Token::TOKEN, 14, 0, 0, NULL, NULL, x86Parser::tokenBranches1936 },
	{x86Token::NUMBER, 9, 1, 0, NULL,&x86Parser::TokenFunc1940,  },
	{x86Token::NUMBER, 13, 1, 0, NULL,&x86Parser::TokenFunc1941,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1946_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding1946_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 4},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 25, 4, 0 },
	{ Coding::eot },
};
void x86Parser::TokenFunc1946(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1946_8;
	operand.values[9] = tokenCoding1946_9;
}
x86Token x86Parser::tokenBranches1945[] = {
	{x86Token::ADDRESSCLASS, 4, 1, 0, NULL,&x86Parser::TokenFunc1946,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1944[] = {
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches1945 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1943[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1944 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1950_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding1950_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 4},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 25, 4, 0 },
	{ Coding::eot },
};
void x86Parser::TokenFunc1950(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1950_8;
	operand.values[9] = tokenCoding1950_9;
}
x86Token x86Parser::tokenBranches1949[] = {
	{x86Token::ADDRESSCLASS, 5, 1, 0, NULL,&x86Parser::TokenFunc1950,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1948[] = {
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches1949 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1947[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1948 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1943(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1947(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1942[] = {
	{x86Token::REGISTERCLASS, 4, 0, 0, NULL,&x86Parser::TokenFunc1943, x86Parser::tokenBranches1943 },
	{x86Token::REGISTERCLASS, 7, 0, 0, NULL,&x86Parser::TokenFunc1947, x86Parser::tokenBranches1947 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1953_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding1953_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 9, 4},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 25, 4, 0 },
	{ Coding::eot },
};
void x86Parser::TokenFunc1953(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1953_8;
	operand.values[9] = tokenCoding1953_9;
}
x86Token x86Parser::tokenBranches1952[] = {
	{x86Token::ADDRESSCLASS, 3, 1, 0, NULL,&x86Parser::TokenFunc1953,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1951[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches1952 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1967_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding1967_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0 },
	{ Coding::eot },
};
void x86Parser::TokenFunc1967(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1967_8;
	operand.values[9] = tokenCoding1967_9;
}
x86Token x86Parser::tokenBranches1966[] = {
	{x86Token::ADDRESSCLASS, 3, 1, 0, NULL,&x86Parser::TokenFunc1967,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1957_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding1957_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0 },
	{ Coding::eot },
};
void x86Parser::TokenFunc1957(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1957_8;
	operand.values[9] = tokenCoding1957_9;
}
x86Token x86Parser::tokenBranches1956[] = {
	{x86Token::ADDRESSCLASS, 8, 1, 0, NULL,&x86Parser::TokenFunc1957,  },
	{x86Token::TOKEN, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches1966 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1955[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1956 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1971_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding1971_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0 },
	{ Coding::eot },
};
void x86Parser::TokenFunc1971(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1971_8;
	operand.values[9] = tokenCoding1971_9;
}
x86Token x86Parser::tokenBranches1970[] = {
	{x86Token::ADDRESSCLASS, 4, 1, 0, NULL,&x86Parser::TokenFunc1971,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1975_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding1975_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0, 43, 1 },
	{ Coding::eot },
};
void x86Parser::TokenFunc1975(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1975_8;
	operand.values[9] = tokenCoding1975_9;
}
x86Token x86Parser::tokenBranches1974[] = {
	{x86Token::ADDRESSCLASS, 5, 1, 0, NULL,&x86Parser::TokenFunc1975,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1960_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding1960_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0 },
	{ Coding::eot },
};
void x86Parser::TokenFunc1960(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1960_8;
	operand.values[9] = tokenCoding1960_9;
}
Coding x86Parser::tokenCoding1963_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding1963_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0, 43, 1 },
	{ Coding::eot },
};
void x86Parser::TokenFunc1963(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding1963_8;
	operand.values[9] = tokenCoding1963_9;
}
x86Token x86Parser::tokenBranches1959[] = {
	{x86Token::ADDRESSCLASS, 8, 1, 0, NULL,&x86Parser::TokenFunc1960,  },
	{x86Token::ADDRESSCLASS, 9, 1, 0, NULL,&x86Parser::TokenFunc1963,  },
	{x86Token::TOKEN, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches1970 },
	{x86Token::TOKEN, 8, 0, 0, NULL, NULL, x86Parser::tokenBranches1974 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1958[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1959 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc1955(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc1958(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches1954[] = {
	{x86Token::REGISTERCLASS, 4, 0, 0, NULL,&x86Parser::TokenFunc1955, x86Parser::tokenBranches1955 },
	{x86Token::REGISTERCLASS, 7, 0, 0, NULL,&x86Parser::TokenFunc1958, x86Parser::tokenBranches1958 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1979_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 208, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc1979(x86Operand &operand, int tokenPos)
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
	operand.values[9] = tokenCoding1979_9;
}
Coding x86Parser::tokenCoding1982_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 192, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc1982(x86Operand &operand, int tokenPos)
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
	operand.values[9] = tokenCoding1982_9;
}
Coding x86Parser::tokenCoding1985_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 210, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc1985(x86Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1985_9;
}
x86Token x86Parser::tokenBranches1978[] = {
	{x86Token::NUMBER, 16, 1, 0, NULL,&x86Parser::TokenFunc1979,  },
	{x86Token::NUMBER, 2, 1, 0, NULL,&x86Parser::TokenFunc1982,  },
	{x86Token::REGISTER, 4, 1, 0, NULL,&x86Parser::TokenFunc1985,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1977[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1978 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1990_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 208, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc1990(x86Operand &operand, int tokenPos)
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
	operand.values[9] = tokenCoding1990_9;
}
Coding x86Parser::tokenCoding1995_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 192, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc1995(x86Operand &operand, int tokenPos)
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
	operand.values[9] = tokenCoding1995_9;
}
x86Token x86Parser::tokenBranches1989[] = {
	{x86Token::NUMBER, 16, 1, 0, NULL,&x86Parser::TokenFunc1990,  },
	{x86Token::NUMBER, 2, 1, 0, NULL,&x86Parser::TokenFunc1995,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding1999_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 210, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc1999(x86Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding1999_9;
}
x86Token x86Parser::tokenBranches1988[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches1989 },
	{x86Token::REGISTER, 4, 1, 0, NULL,&x86Parser::TokenFunc1999,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1987[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches1988 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1986[] = {
	{x86Token::ADDRESSCLASS, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches1987 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2002_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 209, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2002(x86Operand &operand, int tokenPos)
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
	operand.values[9] = tokenCoding2002_9;
}
Coding x86Parser::tokenCoding2005_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 193, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2005(x86Operand &operand, int tokenPos)
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
	operand.values[9] = tokenCoding2005_9;
}
Coding x86Parser::tokenCoding2008_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 211, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2008(x86Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding2008_9;
}
x86Token x86Parser::tokenBranches2001[] = {
	{x86Token::NUMBER, 16, 1, 0, NULL,&x86Parser::TokenFunc2002,  },
	{x86Token::NUMBER, 2, 1, 0, NULL,&x86Parser::TokenFunc2005,  },
	{x86Token::REGISTER, 4, 1, 0, NULL,&x86Parser::TokenFunc2008,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2000[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2001 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2013_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 209, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2013(x86Operand &operand, int tokenPos)
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
	operand.values[9] = tokenCoding2013_9;
}
Coding x86Parser::tokenCoding2018_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 193, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2018(x86Operand &operand, int tokenPos)
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
	operand.values[9] = tokenCoding2018_9;
}
x86Token x86Parser::tokenBranches2012[] = {
	{x86Token::NUMBER, 16, 1, 0, NULL,&x86Parser::TokenFunc2013,  },
	{x86Token::NUMBER, 2, 1, 0, NULL,&x86Parser::TokenFunc2018,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2022_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 211, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2022(x86Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding2022_9;
}
x86Token x86Parser::tokenBranches2011[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches2012 },
	{x86Token::REGISTER, 4, 1, 0, NULL,&x86Parser::TokenFunc2022,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2010[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2011 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2009[] = {
	{x86Token::ADDRESSCLASS, 4, 0, 0, NULL, NULL, x86Parser::tokenBranches2010 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2029_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 193, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2029(x86Operand &operand, int tokenPos)
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
	operand.values[9] = tokenCoding2029_9;
}
x86Token x86Parser::tokenBranches2028[] = {
	{x86Token::NUMBER, 2, 1, 0, NULL,&x86Parser::TokenFunc2029,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2025_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 209, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2025(x86Operand &operand, int tokenPos)
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
	operand.values[9] = tokenCoding2025_9;
}
Coding x86Parser::tokenCoding2032_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 211, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2032(x86Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding2032_9;
}
x86Token x86Parser::tokenBranches2024[] = {
	{x86Token::NUMBER, 16, 1, 0, NULL,&x86Parser::TokenFunc2025,  },
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches2028 },
	{x86Token::REGISTER, 4, 1, 0, NULL,&x86Parser::TokenFunc2032,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2023[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2024 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2037_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 209, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2037(x86Operand &operand, int tokenPos)
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
	operand.values[9] = tokenCoding2037_9;
}
Coding x86Parser::tokenCoding2042_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 193, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2042(x86Operand &operand, int tokenPos)
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
	operand.values[9] = tokenCoding2042_9;
}
x86Token x86Parser::tokenBranches2036[] = {
	{x86Token::NUMBER, 16, 1, 0, NULL,&x86Parser::TokenFunc2037,  },
	{x86Token::NUMBER, 2, 1, 0, NULL,&x86Parser::TokenFunc2042,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2046_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 211, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2046(x86Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding2046_9;
}
x86Token x86Parser::tokenBranches2035[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches2036 },
	{x86Token::REGISTER, 4, 1, 0, NULL,&x86Parser::TokenFunc2046,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2034[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2035 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2033[] = {
	{x86Token::ADDRESSCLASS, 5, 0, 0, NULL, NULL, x86Parser::tokenBranches2034 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches1976[] = {
	{x86Token::ADDRESSCLASS, 8, 0, 0, NULL, NULL, x86Parser::tokenBranches1977 },
	{x86Token::TOKEN, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches1986 },
	{x86Token::ADDRESSCLASS, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches2000 },
	{x86Token::TOKEN, 8, 0, 0, NULL, NULL, x86Parser::tokenBranches2009 },
	{x86Token::ADDRESSCLASS, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches2023 },
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches2033 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2053_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2053_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0 },
	{ Coding::eot },
};
void x86Parser::TokenFunc2053(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding2053_8;
	operand.values[9] = tokenCoding2053_9;
}
Coding x86Parser::tokenCoding2059_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2059_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0, 43, 1 },
	{ Coding::eot },
};
void x86Parser::TokenFunc2059(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2059_8;
	operand.values[9] = tokenCoding2059_9;
}
x86Token x86Parser::tokenBranches2052[] = {
	{x86Token::NUMBER, 2, 1, 0, NULL,&x86Parser::TokenFunc2053,  },
	{x86Token::REGISTER, 4, 1, 0, NULL,&x86Parser::TokenFunc2059,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2051[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2052 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2051(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2050[] = {
	{x86Token::REGISTERCLASS, 4, 0, 0, NULL,&x86Parser::TokenFunc2051, x86Parser::tokenBranches2051 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2049[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2050 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2048[] = {
	{x86Token::ADDRESSCLASS, 4, 0, 0, NULL, NULL, x86Parser::tokenBranches2049 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2065_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2065_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0 },
	{ Coding::eot },
};
void x86Parser::TokenFunc2065(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding2065_8;
	operand.values[9] = tokenCoding2065_9;
}
Coding x86Parser::tokenCoding2071_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2071_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0, 43, 1 },
	{ Coding::eot },
};
void x86Parser::TokenFunc2071(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2071_8;
	operand.values[9] = tokenCoding2071_9;
}
x86Token x86Parser::tokenBranches2064[] = {
	{x86Token::NUMBER, 2, 1, 0, NULL,&x86Parser::TokenFunc2065,  },
	{x86Token::REGISTER, 4, 1, 0, NULL,&x86Parser::TokenFunc2071,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2063[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2064 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2063(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2062[] = {
	{x86Token::REGISTERCLASS, 7, 0, 0, NULL,&x86Parser::TokenFunc2063, x86Parser::tokenBranches2063 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2061[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2062 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2060[] = {
	{x86Token::ADDRESSCLASS, 5, 0, 0, NULL, NULL, x86Parser::tokenBranches2061 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2047[] = {
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches2048 },
	{x86Token::TOKEN, 9, 0, 1, NULL, NULL, x86Parser::tokenBranches2060 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2072[] = {
	{x86Token::ADDRESSCLASS, 1, 1, 0, NULL, NULL,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2084_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2084_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 99, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2084(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding2084_8;
	operand.values[9] = tokenCoding2084_9;
}
x86Token x86Parser::tokenBranches2083[] = {
	{x86Token::REGISTERCLASS, 4, 1, 0, NULL,&x86Parser::TokenFunc2084,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2082[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2083 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2081[] = {
	{x86Token::ADDRESSCLASS, 4, 0, 0, NULL, NULL, x86Parser::tokenBranches2082 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2089_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
void x86Parser::TokenFunc2089(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 172;
	operand.values[10] = new Coding[2];
	CleanupValues.push_back(operand.values[10]);
	operand.values[10]->type = Coding::reg;
	operand.values[10]->val = inputTokens[tokenPos]->val->ival;
	operand.values[10]->bits = 0;
	operand.values[10]->field = 0;
	operand.values[10]->math = 0;
	operand.values[10]->mathval = 0;
	operand.values[10][1].type = Coding::eot;
	operand.values[8] = tokenCoding2089_8;
}
x86Token x86Parser::tokenBranches2088[] = {
	{x86Token::REGISTERCLASS, 7, 1, 0, NULL,&x86Parser::TokenFunc2089,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2098(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 173;
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
x86Token x86Parser::tokenBranches2097[] = {
	{x86Token::NUMBER, 5, 1, 0, NULL,&x86Parser::TokenFunc2098,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2102(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 174;
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
x86Token x86Parser::tokenBranches2101[] = {
	{x86Token::NUMBER, 6, 1, 0, NULL,&x86Parser::TokenFunc2102,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2096[] = {
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches2097 },
	{x86Token::TOKEN, 9, 0, 1, NULL, NULL, x86Parser::tokenBranches2101 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2095[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2096 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2108_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2108_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2108(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2108_8;
	operand.values[9] = tokenCoding2108_9;
}
x86Token x86Parser::tokenBranches2107[] = {
	{x86Token::ADDRESSCLASS, 6, 1, 0, NULL,&x86Parser::TokenFunc2108,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2110_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2110_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2110(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2110_8;
	operand.values[9] = tokenCoding2110_9;
}
x86Token x86Parser::tokenBranches2109[] = {
	{x86Token::ADDRESSCLASS, 7, 1, 0, NULL,&x86Parser::TokenFunc2110,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2112(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 177;
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
void x86Parser::TokenFunc2114(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 178;
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
Coding x86Parser::tokenCoding2116_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2116_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2116(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2116_8;
	operand.values[9] = tokenCoding2116_9;
}
Coding x86Parser::tokenCoding2118_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2118_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2118(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2118_8;
	operand.values[9] = tokenCoding2118_9;
}
x86Token x86Parser::tokenBranches2111[] = {
	{x86Token::NUMBER, 5, 1, 0, NULL,&x86Parser::TokenFunc2112,  },
	{x86Token::NUMBER, 6, 1, 0, NULL,&x86Parser::TokenFunc2114,  },
	{x86Token::ADDRESSCLASS, 6, 1, 0, NULL,&x86Parser::TokenFunc2116,  },
	{x86Token::ADDRESSCLASS, 7, 1, 0, NULL,&x86Parser::TokenFunc2118,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2095(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc2103(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 175;
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
void x86Parser::TokenFunc2104(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 176;
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
Coding x86Parser::tokenCoding2105_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2105_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2105(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2105_8;
	operand.values[9] = tokenCoding2105_9;
}
Coding x86Parser::tokenCoding2106_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2106_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2106(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2106_8;
	operand.values[9] = tokenCoding2106_9;
}
x86Token x86Parser::tokenBranches2094[] = {
	{x86Token::NUMBER, 3, 0, 0, NULL,&x86Parser::TokenFunc2095, x86Parser::tokenBranches2095 },
	{x86Token::NUMBER, 11, 1, 0, NULL,&x86Parser::TokenFunc2103,  },
	{x86Token::NUMBER, 13, 1, 0, NULL,&x86Parser::TokenFunc2104,  },
	{x86Token::ADDRESSCLASS, 9, 1, 0, NULL,&x86Parser::TokenFunc2105,  },
	{x86Token::ADDRESSCLASS, 10, 1, 0, NULL,&x86Parser::TokenFunc2106,  },
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches2107 },
	{x86Token::TOKEN, 9, 0, 1, NULL, NULL, x86Parser::tokenBranches2109 },
	{x86Token::TOKEN, 15, 0, 0, NULL, NULL, x86Parser::tokenBranches2111 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2170(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 179;
}
x86Token x86Parser::tokenBranches2169[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2170,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2168[] = {
	{x86Token::REGISTER, 22, 0, 0, NULL, NULL, x86Parser::tokenBranches2169 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2167[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2168 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2166[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2167 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2165[] = {
	{x86Token::REGISTER, 24, 0, 0, NULL, NULL, x86Parser::tokenBranches2166 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2164[] = {
	{x86Token::TOKEN, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches2165 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2163[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2164 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2162[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2163 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2196(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 181;
}
x86Token x86Parser::tokenBranches2195[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2196,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2194[] = {
	{x86Token::REGISTER, 23, 0, 0, NULL, NULL, x86Parser::tokenBranches2195 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2193[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2194 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2192[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2193 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2191[] = {
	{x86Token::REGISTER, 24, 0, 0, NULL, NULL, x86Parser::tokenBranches2192 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2190[] = {
	{x86Token::TOKEN, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches2191 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2189[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2190 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2188[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2189 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2161[] = {
	{x86Token::REGISTER, 20, 0, 0, NULL, NULL, x86Parser::tokenBranches2162 },
	{x86Token::REGISTER, 21, 0, 0, NULL, NULL, x86Parser::tokenBranches2188 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2160[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2161 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2183(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 180;
}
x86Token x86Parser::tokenBranches2182[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2183,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2181[] = {
	{x86Token::REGISTER, 22, 0, 0, NULL, NULL, x86Parser::tokenBranches2182 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2180[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2181 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2179[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2180 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2178[] = {
	{x86Token::REGISTER, 24, 0, 0, NULL, NULL, x86Parser::tokenBranches2179 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2177[] = {
	{x86Token::TOKEN, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches2178 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2176[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2177 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2175[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2176 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2209(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 182;
}
x86Token x86Parser::tokenBranches2208[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2209,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2207[] = {
	{x86Token::REGISTER, 23, 0, 0, NULL, NULL, x86Parser::tokenBranches2208 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2206[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2207 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2205[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2206 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2204[] = {
	{x86Token::REGISTER, 24, 0, 0, NULL, NULL, x86Parser::tokenBranches2205 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2203[] = {
	{x86Token::TOKEN, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches2204 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2202[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2203 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2201[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2202 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2174[] = {
	{x86Token::REGISTER, 20, 0, 0, NULL, NULL, x86Parser::tokenBranches2175 },
	{x86Token::REGISTER, 21, 0, 0, NULL, NULL, x86Parser::tokenBranches2201 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2173[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2174 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2173(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2159[] = {
	{x86Token::REGISTER, 27, 0, 0, NULL, NULL, x86Parser::tokenBranches2160 },
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc2173, x86Parser::tokenBranches2173 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2158[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2159 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2222(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 183;
}
x86Token x86Parser::tokenBranches2221[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2222,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2220[] = {
	{x86Token::REGISTER, 22, 0, 0, NULL, NULL, x86Parser::tokenBranches2221 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2219[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2220 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2218[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2219 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2217[] = {
	{x86Token::REGISTER, 24, 0, 0, NULL, NULL, x86Parser::tokenBranches2218 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2216[] = {
	{x86Token::TOKEN, 8, 0, 0, NULL, NULL, x86Parser::tokenBranches2217 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2215[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2216 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2214[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2215 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2248(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 185;
}
x86Token x86Parser::tokenBranches2247[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2248,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2246[] = {
	{x86Token::REGISTER, 23, 0, 0, NULL, NULL, x86Parser::tokenBranches2247 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2245[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2246 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2244[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2245 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2243[] = {
	{x86Token::REGISTER, 24, 0, 0, NULL, NULL, x86Parser::tokenBranches2244 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2242[] = {
	{x86Token::TOKEN, 8, 0, 0, NULL, NULL, x86Parser::tokenBranches2243 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2241[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2242 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2240[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2241 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2213[] = {
	{x86Token::REGISTER, 20, 0, 0, NULL, NULL, x86Parser::tokenBranches2214 },
	{x86Token::REGISTER, 21, 0, 0, NULL, NULL, x86Parser::tokenBranches2240 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2212[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2213 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2235(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 184;
}
x86Token x86Parser::tokenBranches2234[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2235,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2233[] = {
	{x86Token::REGISTER, 22, 0, 0, NULL, NULL, x86Parser::tokenBranches2234 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2232[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2233 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2231[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2232 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2230[] = {
	{x86Token::REGISTER, 24, 0, 0, NULL, NULL, x86Parser::tokenBranches2231 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2229[] = {
	{x86Token::TOKEN, 8, 0, 0, NULL, NULL, x86Parser::tokenBranches2230 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2228[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2229 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2227[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2228 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2261(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 186;
}
x86Token x86Parser::tokenBranches2260[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2261,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2259[] = {
	{x86Token::REGISTER, 23, 0, 0, NULL, NULL, x86Parser::tokenBranches2260 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2258[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2259 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2257[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2258 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2256[] = {
	{x86Token::REGISTER, 24, 0, 0, NULL, NULL, x86Parser::tokenBranches2257 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2255[] = {
	{x86Token::TOKEN, 8, 0, 0, NULL, NULL, x86Parser::tokenBranches2256 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2254[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2255 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2253[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2254 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2226[] = {
	{x86Token::REGISTER, 20, 0, 0, NULL, NULL, x86Parser::tokenBranches2227 },
	{x86Token::REGISTER, 21, 0, 0, NULL, NULL, x86Parser::tokenBranches2253 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2225[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2226 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2225(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2211[] = {
	{x86Token::REGISTER, 27, 0, 0, NULL, NULL, x86Parser::tokenBranches2212 },
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc2225, x86Parser::tokenBranches2225 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2210[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2211 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2274(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 187;
}
x86Token x86Parser::tokenBranches2273[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2274,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2272[] = {
	{x86Token::REGISTER, 22, 0, 0, NULL, NULL, x86Parser::tokenBranches2273 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2271[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2272 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2270[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2271 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2269[] = {
	{x86Token::REGISTER, 24, 0, 0, NULL, NULL, x86Parser::tokenBranches2270 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2268[] = {
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches2269 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2267[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2268 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2266[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2267 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2300(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 189;
}
x86Token x86Parser::tokenBranches2299[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2300,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2298[] = {
	{x86Token::REGISTER, 23, 0, 0, NULL, NULL, x86Parser::tokenBranches2299 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2297[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2298 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2296[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2297 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2295[] = {
	{x86Token::REGISTER, 24, 0, 0, NULL, NULL, x86Parser::tokenBranches2296 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2294[] = {
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches2295 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2293[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2294 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2292[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2293 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2265[] = {
	{x86Token::REGISTER, 20, 0, 0, NULL, NULL, x86Parser::tokenBranches2266 },
	{x86Token::REGISTER, 21, 0, 0, NULL, NULL, x86Parser::tokenBranches2292 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2264[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2265 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2287(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 188;
}
x86Token x86Parser::tokenBranches2286[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2287,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2285[] = {
	{x86Token::REGISTER, 22, 0, 0, NULL, NULL, x86Parser::tokenBranches2286 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2284[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2285 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2283[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2284 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2282[] = {
	{x86Token::REGISTER, 24, 0, 0, NULL, NULL, x86Parser::tokenBranches2283 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2281[] = {
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches2282 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2280[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2281 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2279[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2280 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2313(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 190;
}
x86Token x86Parser::tokenBranches2312[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2313,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2311[] = {
	{x86Token::REGISTER, 23, 0, 0, NULL, NULL, x86Parser::tokenBranches2312 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2310[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2311 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2309[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2310 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2308[] = {
	{x86Token::REGISTER, 24, 0, 0, NULL, NULL, x86Parser::tokenBranches2309 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2307[] = {
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches2308 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2306[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2307 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2305[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2306 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2278[] = {
	{x86Token::REGISTER, 20, 0, 0, NULL, NULL, x86Parser::tokenBranches2279 },
	{x86Token::REGISTER, 21, 0, 0, NULL, NULL, x86Parser::tokenBranches2305 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2277[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2278 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2277(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2263[] = {
	{x86Token::REGISTER, 27, 0, 0, NULL, NULL, x86Parser::tokenBranches2264 },
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc2277, x86Parser::tokenBranches2277 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2262[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2263 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2157[] = {
	{x86Token::TOKEN, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches2158 },
	{x86Token::TOKEN, 8, 0, 0, NULL, NULL, x86Parser::tokenBranches2210 },
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches2262 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2321_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2321_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 176, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2321(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding2321_8;
	operand.values[9] = tokenCoding2321_9;
}
x86Token x86Parser::tokenBranches2320[] = {
	{x86Token::REGISTERCLASS, 1, 1, 0, NULL,&x86Parser::TokenFunc2321,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2319[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2320 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2318[] = {
	{x86Token::ADDRESSCLASS, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2319 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2317[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches2318 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2335(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 191;
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
x86Token x86Parser::tokenBranches2334[] = {
	{x86Token::NUMBER, 2, 1, 0, NULL,&x86Parser::TokenFunc2335,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2333[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches2334 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2332[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2333 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2332(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2331[] = {
	{x86Token::NUMBER, 3, 0, 0, NULL,&x86Parser::TokenFunc2332, x86Parser::tokenBranches2332 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2330[] = {
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches2331 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2339_8[] = {
	{ Coding::indirect, 12, -1, 0, 38, 7 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2339_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 27, 5},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 3, 0, 62, 3 },
	{ Coding::eot },
};
void x86Parser::TokenFunc2339(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2339_8;
	operand.values[9] = tokenCoding2339_9;
}
x86Token x86Parser::tokenBranches2338[] = {
	{x86Token::ADDRESSCLASS, 0, 1, 0, NULL,&x86Parser::TokenFunc2339,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2337[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2338 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2337(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2336[] = {
	{x86Token::NUMBER, 1, 0, 0, NULL,&x86Parser::TokenFunc2337, x86Parser::tokenBranches2337 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2352(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 167;
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
x86Token x86Parser::tokenBranches2351[] = {
	{x86Token::REGISTERCLASS, 13, 1, 0, NULL,&x86Parser::TokenFunc2352,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2350[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2351 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2355_9[] = {
	{ Coding::indirect, 23, -1, 0 },
	{ Coding::eot },
};
void x86Parser::TokenFunc2355(x86Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding2355_9;
}
x86Token x86Parser::tokenBranches2354[] = {
	{x86Token::ADDRESSCLASS, 2, 1, 0, NULL,&x86Parser::TokenFunc2355,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2357_9[] = {
	{ Coding::indirect, 23, -1, 0, 43, 4 },
	{ Coding::eot },
};
void x86Parser::TokenFunc2357(x86Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding2357_9;
}
x86Token x86Parser::tokenBranches2356[] = {
	{x86Token::ADDRESSCLASS, 2, 1, 0, NULL,&x86Parser::TokenFunc2357,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2353(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 167;
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
x86Token x86Parser::tokenBranches2349[] = {
	{x86Token::REGISTER, 30, 0, 0, NULL, NULL, x86Parser::tokenBranches2350 },
	{x86Token::REGISTERCLASS, 13, 1, 0, NULL,&x86Parser::TokenFunc2353,  },
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches2354 },
	{x86Token::TOKEN, 11, 0, 0, NULL, NULL, x86Parser::tokenBranches2356 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2361_9[] = {
	{ Coding::indirect, 23, -1, 0 },
	{ Coding::eot },
};
void x86Parser::TokenFunc2361(x86Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding2361_9;
}
x86Token x86Parser::tokenBranches2360[] = {
	{x86Token::ADDRESSCLASS, 2, 1, 0, NULL,&x86Parser::TokenFunc2361,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2363_9[] = {
	{ Coding::indirect, 23, -1, 0, 43, 4 },
	{ Coding::eot },
};
void x86Parser::TokenFunc2363(x86Operand &operand, int tokenPos)
{
	operand.values[9] = tokenCoding2363_9;
}
x86Token x86Parser::tokenBranches2362[] = {
	{x86Token::ADDRESSCLASS, 2, 1, 0, NULL,&x86Parser::TokenFunc2363,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2359(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 167;
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
x86Token x86Parser::tokenBranches2358[] = {
	{x86Token::REGISTERCLASS, 13, 1, 0, NULL,&x86Parser::TokenFunc2359,  },
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches2360 },
	{x86Token::TOKEN, 11, 0, 0, NULL, NULL, x86Parser::tokenBranches2362 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2374_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2374_9[] = {
	{ Coding::valSpecified, 221},
	{ Coding::eot },
};
void x86Parser::TokenFunc2374(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2374_8;
	operand.values[9] = tokenCoding2374_9;
}
x86Token x86Parser::tokenBranches2373[] = {
	{x86Token::ADDRESSCLASS, 11, 1, 0, NULL,&x86Parser::TokenFunc2374,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2382_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2382_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 223, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2382(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2382_8;
	operand.values[9] = tokenCoding2382_9;
}
x86Token x86Parser::tokenBranches2381[] = {
	{x86Token::ADDRESSCLASS, 1, 1, 0, NULL,&x86Parser::TokenFunc2382,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2380[] = {
	{x86Token::TOKEN, 11, 0, 0, NULL, NULL, x86Parser::tokenBranches2381 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2390_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2390_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 223, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2390(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2390_8;
	operand.values[9] = tokenCoding2390_9;
}
x86Token x86Parser::tokenBranches2389[] = {
	{x86Token::ADDRESSCLASS, 1, 1, 0, NULL,&x86Parser::TokenFunc2390,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2388[] = {
	{x86Token::TOKEN, 11, 0, 0, NULL, NULL, x86Parser::tokenBranches2389 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2396_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2396_9[] = {
	{ Coding::valSpecified, 217},
	{ Coding::eot },
};
void x86Parser::TokenFunc2396(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2396_8;
	operand.values[9] = tokenCoding2396_9;
}
x86Token x86Parser::tokenBranches2395[] = {
	{x86Token::ADDRESSCLASS, 1, 1, 0, NULL,&x86Parser::TokenFunc2396,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2398_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2398_9[] = {
	{ Coding::valSpecified, 221},
	{ Coding::eot },
};
void x86Parser::TokenFunc2398(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2398_8;
	operand.values[9] = tokenCoding2398_9;
}
x86Token x86Parser::tokenBranches2397[] = {
	{x86Token::ADDRESSCLASS, 1, 1, 0, NULL,&x86Parser::TokenFunc2398,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2400_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2400_9[] = {
	{ Coding::valSpecified, 219},
	{ Coding::eot },
};
void x86Parser::TokenFunc2400(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2400_8;
	operand.values[9] = tokenCoding2400_9;
}
x86Token x86Parser::tokenBranches2399[] = {
	{x86Token::ADDRESSCLASS, 1, 1, 0, NULL,&x86Parser::TokenFunc2400,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2394_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2394_9[] = {
	{ Coding::valSpecified, 217},
	{ Coding::eot },
};
void x86Parser::TokenFunc2394(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2394_8;
	operand.values[9] = tokenCoding2394_9;
}
x86Token x86Parser::tokenBranches2393[] = {
	{x86Token::ADDRESSCLASS, 11, 1, 0, NULL,&x86Parser::TokenFunc2394,  },
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches2395 },
	{x86Token::TOKEN, 11, 0, 0, NULL, NULL, x86Parser::tokenBranches2397 },
	{x86Token::TOKEN, 12, 0, 0, NULL, NULL, x86Parser::tokenBranches2399 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2404_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2404_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2404(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2404_8;
	operand.values[9] = tokenCoding2404_9;
}
x86Token x86Parser::tokenBranches2403[] = {
	{x86Token::ADDRESSCLASS, 1, 1, 0, NULL,&x86Parser::TokenFunc2404,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2402[] = {
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches2403 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2406_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2406_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2406(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2406_8;
	operand.values[9] = tokenCoding2406_9;
}
x86Token x86Parser::tokenBranches2405[] = {
	{x86Token::ADDRESSCLASS, 1, 1, 0, NULL,&x86Parser::TokenFunc2406,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2417_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2417_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 221, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2417(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2417_8;
	operand.values[9] = tokenCoding2417_9;
}
x86Token x86Parser::tokenBranches2416[] = {
	{x86Token::ADDRESSCLASS, 1, 1, 0, NULL,&x86Parser::TokenFunc2417,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2424_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2424_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 221, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2424(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2424_8;
	operand.values[9] = tokenCoding2424_9;
}
x86Token x86Parser::tokenBranches2423[] = {
	{x86Token::ADDRESSCLASS, 1, 1, 0, NULL,&x86Parser::TokenFunc2424,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2426_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2426_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 155, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 221, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2426(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2426_8;
	operand.values[9] = tokenCoding2426_9;
}
x86Token x86Parser::tokenBranches2425[] = {
	{x86Token::ADDRESSCLASS, 1, 1, 0, NULL,&x86Parser::TokenFunc2426,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2435_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2435_9[] = {
	{ Coding::valSpecified, 217},
	{ Coding::eot },
};
void x86Parser::TokenFunc2435(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2435_8;
	operand.values[9] = tokenCoding2435_9;
}
x86Token x86Parser::tokenBranches2434[] = {
	{x86Token::ADDRESSCLASS, 2, 1, 0, NULL,&x86Parser::TokenFunc2435,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2437_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2437_9[] = {
	{ Coding::valSpecified, 221},
	{ Coding::eot },
};
void x86Parser::TokenFunc2437(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2437_8;
	operand.values[9] = tokenCoding2437_9;
}
x86Token x86Parser::tokenBranches2436[] = {
	{x86Token::ADDRESSCLASS, 2, 1, 0, NULL,&x86Parser::TokenFunc2437,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2433_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2433_9[] = {
	{ Coding::valSpecified, 221},
	{ Coding::eot },
};
void x86Parser::TokenFunc2433(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2433_8;
	operand.values[9] = tokenCoding2433_9;
}
x86Token x86Parser::tokenBranches2432[] = {
	{x86Token::ADDRESSCLASS, 11, 1, 0, NULL,&x86Parser::TokenFunc2433,  },
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches2434 },
	{x86Token::TOKEN, 11, 0, 0, NULL, NULL, x86Parser::tokenBranches2436 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2440_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2440_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 155, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2440(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2440_8;
	operand.values[9] = tokenCoding2440_9;
}
x86Token x86Parser::tokenBranches2439[] = {
	{x86Token::ADDRESSCLASS, 1, 1, 0, NULL,&x86Parser::TokenFunc2440,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2438[] = {
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches2439 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2443_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2443_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2443(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2443_8;
	operand.values[9] = tokenCoding2443_9;
}
x86Token x86Parser::tokenBranches2442[] = {
	{x86Token::ADDRESSCLASS, 1, 1, 0, NULL,&x86Parser::TokenFunc2443,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2441[] = {
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches2442 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2445_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2445_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 155, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2445(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2445_8;
	operand.values[9] = tokenCoding2445_9;
}
x86Token x86Parser::tokenBranches2444[] = {
	{x86Token::ADDRESSCLASS, 1, 1, 0, NULL,&x86Parser::TokenFunc2445,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2447_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2447_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2447(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2447_8;
	operand.values[9] = tokenCoding2447_9;
}
x86Token x86Parser::tokenBranches2446[] = {
	{x86Token::ADDRESSCLASS, 1, 1, 0, NULL,&x86Parser::TokenFunc2447,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2451_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2451_9[] = {
	{ Coding::valSpecified, 217},
	{ Coding::eot },
};
void x86Parser::TokenFunc2451(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2451_8;
	operand.values[9] = tokenCoding2451_9;
}
x86Token x86Parser::tokenBranches2450[] = {
	{x86Token::ADDRESSCLASS, 2, 1, 0, NULL,&x86Parser::TokenFunc2451,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2453_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2453_9[] = {
	{ Coding::valSpecified, 221},
	{ Coding::eot },
};
void x86Parser::TokenFunc2453(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2453_8;
	operand.values[9] = tokenCoding2453_9;
}
x86Token x86Parser::tokenBranches2452[] = {
	{x86Token::ADDRESSCLASS, 2, 1, 0, NULL,&x86Parser::TokenFunc2453,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2455_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2455_9[] = {
	{ Coding::valSpecified, 219},
	{ Coding::eot },
};
void x86Parser::TokenFunc2455(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2455_8;
	operand.values[9] = tokenCoding2455_9;
}
x86Token x86Parser::tokenBranches2454[] = {
	{x86Token::ADDRESSCLASS, 2, 1, 0, NULL,&x86Parser::TokenFunc2455,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2449_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2449_9[] = {
	{ Coding::valSpecified, 221},
	{ Coding::eot },
};
void x86Parser::TokenFunc2449(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2449_8;
	operand.values[9] = tokenCoding2449_9;
}
x86Token x86Parser::tokenBranches2448[] = {
	{x86Token::ADDRESSCLASS, 11, 1, 0, NULL,&x86Parser::TokenFunc2449,  },
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches2450 },
	{x86Token::TOKEN, 11, 0, 0, NULL, NULL, x86Parser::tokenBranches2452 },
	{x86Token::TOKEN, 12, 0, 0, NULL, NULL, x86Parser::tokenBranches2454 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2458_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2458_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 155, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 221, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2458(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2458_8;
	operand.values[9] = tokenCoding2458_9;
}
x86Token x86Parser::tokenBranches2457[] = {
	{x86Token::ADDRESSCLASS, 1, 1, 0, NULL,&x86Parser::TokenFunc2458,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2459(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 192;
}
x86Token x86Parser::tokenBranches2456[] = {
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches2457 },
	{x86Token::REGISTER, 2, 1, 0, NULL,&x86Parser::TokenFunc2459,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2462_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2462_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 221, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2462(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2462_8;
	operand.values[9] = tokenCoding2462_9;
}
x86Token x86Parser::tokenBranches2461[] = {
	{x86Token::ADDRESSCLASS, 1, 1, 0, NULL,&x86Parser::TokenFunc2462,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2463(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 193;
}
x86Token x86Parser::tokenBranches2460[] = {
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches2461 },
	{x86Token::REGISTER, 2, 1, 0, NULL,&x86Parser::TokenFunc2463,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2488_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2488_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 107, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2488(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 194;
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
	operand.values[8] = tokenCoding2488_8;
	operand.values[9] = tokenCoding2488_9;
}
x86Token x86Parser::tokenBranches2487[] = {
	{x86Token::NUMBER, 0, 1, 0, NULL,&x86Parser::TokenFunc2488,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2494_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2494_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 105, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2494(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 195;
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
	operand.values[8] = tokenCoding2494_8;
	operand.values[9] = tokenCoding2494_9;
}
x86Token x86Parser::tokenBranches2486[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches2487 },
	{x86Token::NUMBER, 3, 1, 0, NULL,&x86Parser::TokenFunc2494,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2485[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2486 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2485_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2485_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 175, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2485(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2485_8;
	operand.values[9] = tokenCoding2485_9;
}
void x86Parser::TokenFunc2523(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2484[] = {
	{x86Token::ADDRESSCLASS, 4, 1, 0, NULL,&x86Parser::TokenFunc2485, x86Parser::tokenBranches2485 },
	{x86Token::NUMBER, 3, 1, 0, NULL,&x86Parser::TokenFunc2523,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2519(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2518[] = {
	{x86Token::NUMBER, 0, 1, 0, NULL,&x86Parser::TokenFunc2519,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2483[] = {
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches2484 },
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches2518 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2482[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2483 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2505_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2505_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 107, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2505(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 194;
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
	operand.values[8] = tokenCoding2505_8;
	operand.values[9] = tokenCoding2505_9;
}
x86Token x86Parser::tokenBranches2504[] = {
	{x86Token::NUMBER, 0, 1, 0, NULL,&x86Parser::TokenFunc2505,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2511_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2511_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 105, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2511(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding2511_8;
	operand.values[9] = tokenCoding2511_9;
}
x86Token x86Parser::tokenBranches2503[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches2504 },
	{x86Token::NUMBER, 4, 1, 0, NULL,&x86Parser::TokenFunc2511,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2502[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2503 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2502_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2502_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 175, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2502(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2502_8;
	operand.values[9] = tokenCoding2502_9;
}
void x86Parser::TokenFunc2531(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 200;
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
x86Token x86Parser::tokenBranches2501[] = {
	{x86Token::ADDRESSCLASS, 5, 1, 0, NULL,&x86Parser::TokenFunc2502, x86Parser::tokenBranches2502 },
	{x86Token::NUMBER, 4, 1, 0, NULL,&x86Parser::TokenFunc2531,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2527(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 199;
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
x86Token x86Parser::tokenBranches2526[] = {
	{x86Token::NUMBER, 0, 1, 0, NULL,&x86Parser::TokenFunc2527,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2500[] = {
	{x86Token::TOKEN, 9, 0, 1, NULL, NULL, x86Parser::tokenBranches2501 },
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches2526 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2499[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2500 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2482(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc2499(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2481[] = {
	{x86Token::REGISTERCLASS, 4, 0, 0, NULL,&x86Parser::TokenFunc2482, x86Parser::tokenBranches2482 },
	{x86Token::REGISTERCLASS, 7, 0, 0, NULL,&x86Parser::TokenFunc2499, x86Parser::tokenBranches2499 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2536(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2535[] = {
	{x86Token::NUMBER, 2, 1, 0, NULL,&x86Parser::TokenFunc2536,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2547(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 204;
}
x86Token x86Parser::tokenBranches2534[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches2535 },
	{x86Token::REGISTER, 10, 1, 0, NULL,&x86Parser::TokenFunc2547,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2533[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2534 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2540(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 202;
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
x86Token x86Parser::tokenBranches2539[] = {
	{x86Token::NUMBER, 2, 1, 0, NULL,&x86Parser::TokenFunc2540,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2550(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 205;
}
x86Token x86Parser::tokenBranches2538[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches2539 },
	{x86Token::REGISTER, 10, 1, 0, NULL,&x86Parser::TokenFunc2550,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2537[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2538 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2544(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 203;
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
x86Token x86Parser::tokenBranches2543[] = {
	{x86Token::NUMBER, 2, 1, 0, NULL,&x86Parser::TokenFunc2544,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2553(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 206;
}
x86Token x86Parser::tokenBranches2542[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches2543 },
	{x86Token::REGISTER, 10, 1, 0, NULL,&x86Parser::TokenFunc2553,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2541[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2542 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2532[] = {
	{x86Token::REGISTER, 0, 0, 0, NULL, NULL, x86Parser::tokenBranches2533 },
	{x86Token::REGISTER, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2537 },
	{x86Token::REGISTER, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2541 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2562(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 207;
}
x86Token x86Parser::tokenBranches2561[] = {
	{x86Token::REGISTER, 10, 1, 0, NULL,&x86Parser::TokenFunc2562,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2560[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2561 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2559[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2560 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2578(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 209;
}
x86Token x86Parser::tokenBranches2577[] = {
	{x86Token::REGISTER, 10, 1, 0, NULL,&x86Parser::TokenFunc2578,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2576[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2577 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2575[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2576 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2558[] = {
	{x86Token::REGISTER, 22, 0, 0, NULL, NULL, x86Parser::tokenBranches2559 },
	{x86Token::REGISTER, 23, 0, 0, NULL, NULL, x86Parser::tokenBranches2575 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2557[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2558 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2556[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2557 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2570(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 208;
}
x86Token x86Parser::tokenBranches2569[] = {
	{x86Token::REGISTER, 10, 1, 0, NULL,&x86Parser::TokenFunc2570,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2568[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2569 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2567[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2568 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2586(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 210;
}
x86Token x86Parser::tokenBranches2585[] = {
	{x86Token::REGISTER, 10, 1, 0, NULL,&x86Parser::TokenFunc2586,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2584[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2585 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2583[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2584 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2566[] = {
	{x86Token::REGISTER, 22, 0, 0, NULL, NULL, x86Parser::tokenBranches2567 },
	{x86Token::REGISTER, 23, 0, 0, NULL, NULL, x86Parser::tokenBranches2583 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2565[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2566 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2565(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2564[] = {
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc2565, x86Parser::tokenBranches2565 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2555[] = {
	{x86Token::REGISTER, 24, 0, 0, NULL, NULL, x86Parser::tokenBranches2556 },
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2564 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2594(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 211;
}
x86Token x86Parser::tokenBranches2593[] = {
	{x86Token::REGISTER, 10, 1, 0, NULL,&x86Parser::TokenFunc2594,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2592[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2593 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2591[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2592 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2610(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 213;
}
x86Token x86Parser::tokenBranches2609[] = {
	{x86Token::REGISTER, 10, 1, 0, NULL,&x86Parser::TokenFunc2610,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2608[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2609 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2607[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2608 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2590[] = {
	{x86Token::REGISTER, 22, 0, 0, NULL, NULL, x86Parser::tokenBranches2591 },
	{x86Token::REGISTER, 23, 0, 0, NULL, NULL, x86Parser::tokenBranches2607 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2589[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2590 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2588[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2589 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2602(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 212;
}
x86Token x86Parser::tokenBranches2601[] = {
	{x86Token::REGISTER, 10, 1, 0, NULL,&x86Parser::TokenFunc2602,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2600[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2601 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2599[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2600 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2618(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 214;
}
x86Token x86Parser::tokenBranches2617[] = {
	{x86Token::REGISTER, 10, 1, 0, NULL,&x86Parser::TokenFunc2618,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2616[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2617 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2615[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2616 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2598[] = {
	{x86Token::REGISTER, 22, 0, 0, NULL, NULL, x86Parser::tokenBranches2599 },
	{x86Token::REGISTER, 23, 0, 0, NULL, NULL, x86Parser::tokenBranches2615 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2597[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2598 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2597(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2596[] = {
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc2597, x86Parser::tokenBranches2597 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2587[] = {
	{x86Token::REGISTER, 24, 0, 0, NULL, NULL, x86Parser::tokenBranches2588 },
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2596 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2626(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 215;
}
x86Token x86Parser::tokenBranches2625[] = {
	{x86Token::REGISTER, 10, 1, 0, NULL,&x86Parser::TokenFunc2626,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2624[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2625 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2623[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2624 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2642(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 217;
}
x86Token x86Parser::tokenBranches2641[] = {
	{x86Token::REGISTER, 10, 1, 0, NULL,&x86Parser::TokenFunc2642,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2640[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2641 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2639[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2640 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2622[] = {
	{x86Token::REGISTER, 22, 0, 0, NULL, NULL, x86Parser::tokenBranches2623 },
	{x86Token::REGISTER, 23, 0, 0, NULL, NULL, x86Parser::tokenBranches2639 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2621[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2622 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2620[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2621 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2634(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 216;
}
x86Token x86Parser::tokenBranches2633[] = {
	{x86Token::REGISTER, 10, 1, 0, NULL,&x86Parser::TokenFunc2634,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2632[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2633 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2631[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2632 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2650(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 218;
}
x86Token x86Parser::tokenBranches2649[] = {
	{x86Token::REGISTER, 10, 1, 0, NULL,&x86Parser::TokenFunc2650,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2648[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2649 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2647[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2648 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2630[] = {
	{x86Token::REGISTER, 22, 0, 0, NULL, NULL, x86Parser::tokenBranches2631 },
	{x86Token::REGISTER, 23, 0, 0, NULL, NULL, x86Parser::tokenBranches2647 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2629[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2630 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2629(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2628[] = {
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc2629, x86Parser::tokenBranches2629 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2619[] = {
	{x86Token::REGISTER, 24, 0, 0, NULL, NULL, x86Parser::tokenBranches2620 },
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2628 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2554[] = {
	{x86Token::TOKEN, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches2555 },
	{x86Token::TOKEN, 8, 0, 0, NULL, NULL, x86Parser::tokenBranches2587 },
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches2619 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2657(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 219;
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
x86Token x86Parser::tokenBranches2656[] = {
	{x86Token::NUMBER, 2, 1, 0, NULL,&x86Parser::TokenFunc2657,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2655[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches2656 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2663_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2663_9[] = {
	{ Coding::valSpecified, 15},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2663(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2663_8;
	operand.values[9] = tokenCoding2663_9;
}
x86Token x86Parser::tokenBranches2662[] = {
	{x86Token::ADDRESSCLASS, 1, 1, 0, NULL,&x86Parser::TokenFunc2663,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2673(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 220;
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
x86Token x86Parser::tokenBranches2672[] = {
	{x86Token::NUMBER, 8, 1, 0, NULL,&x86Parser::TokenFunc2673,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2675(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 221;
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
x86Token x86Parser::tokenBranches2674[] = {
	{x86Token::NUMBER, 8, 1, 0, NULL,&x86Parser::TokenFunc2675,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2686(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2685[] = {
	{x86Token::NUMBER, 5, 1, 0, NULL,&x86Parser::TokenFunc2686,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2691(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2690[] = {
	{x86Token::NUMBER, 6, 1, 0, NULL,&x86Parser::TokenFunc2691,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2684[] = {
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches2685 },
	{x86Token::TOKEN, 9, 0, 1, NULL, NULL, x86Parser::tokenBranches2690 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2683[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2684 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2683(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc2693(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 224;
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
void x86Parser::TokenFunc2695(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 225;
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
Coding x86Parser::tokenCoding2697_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2697_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2697(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2697_8;
	operand.values[9] = tokenCoding2697_9;
}
Coding x86Parser::tokenCoding2699_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2699_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2699(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2699_8;
	operand.values[9] = tokenCoding2699_9;
}
x86Token x86Parser::tokenBranches2682[] = {
	{x86Token::NUMBER, 3, 0, 0, NULL,&x86Parser::TokenFunc2683, x86Parser::tokenBranches2683 },
	{x86Token::NUMBER, 5, 1, 0, NULL,&x86Parser::TokenFunc2693,  },
	{x86Token::NUMBER, 6, 1, 0, NULL,&x86Parser::TokenFunc2695,  },
	{x86Token::ADDRESSCLASS, 6, 1, 0, NULL,&x86Parser::TokenFunc2697,  },
	{x86Token::ADDRESSCLASS, 7, 1, 0, NULL,&x86Parser::TokenFunc2699,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2701(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2700[] = {
	{x86Token::NUMBER, 7, 1, 0, NULL,&x86Parser::TokenFunc2701,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2703(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc2705(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 228;
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
x86Token x86Parser::tokenBranches2702[] = {
	{x86Token::NUMBER, 12, 1, 0, NULL,&x86Parser::TokenFunc2703,  },
	{x86Token::NUMBER, 13, 1, 0, NULL,&x86Parser::TokenFunc2705,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2711_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2711_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2711(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2711_8;
	operand.values[9] = tokenCoding2711_9;
}
x86Token x86Parser::tokenBranches2710[] = {
	{x86Token::ADDRESSCLASS, 6, 1, 0, NULL,&x86Parser::TokenFunc2711,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2713_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2713_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2713(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2713_8;
	operand.values[9] = tokenCoding2713_9;
}
x86Token x86Parser::tokenBranches2712[] = {
	{x86Token::ADDRESSCLASS, 7, 1, 0, NULL,&x86Parser::TokenFunc2713,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2706(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc2707(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 228;
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
Coding x86Parser::tokenCoding2708_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2708_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2708(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2708_8;
	operand.values[9] = tokenCoding2708_9;
}
Coding x86Parser::tokenCoding2709_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2709_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2709(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2709_8;
	operand.values[9] = tokenCoding2709_9;
}
x86Token x86Parser::tokenBranches2681[] = {
	{x86Token::TOKEN, 15, 0, 0, NULL, NULL, x86Parser::tokenBranches2682 },
	{x86Token::TOKEN, 13, 0, 0, NULL, NULL, x86Parser::tokenBranches2700 },
	{x86Token::TOKEN, 14, 0, 0, NULL, NULL, x86Parser::tokenBranches2702 },
	{x86Token::NUMBER, 11, 1, 0, NULL,&x86Parser::TokenFunc2706,  },
	{x86Token::NUMBER, 13, 1, 0, NULL,&x86Parser::TokenFunc2707,  },
	{x86Token::ADDRESSCLASS, 9, 1, 0, NULL,&x86Parser::TokenFunc2708,  },
	{x86Token::ADDRESSCLASS, 10, 1, 0, NULL,&x86Parser::TokenFunc2709,  },
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches2710 },
	{x86Token::TOKEN, 9, 0, 1, NULL, NULL, x86Parser::tokenBranches2712 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2745_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2745_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2745(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2745_8;
	operand.values[9] = tokenCoding2745_9;
}
Coding x86Parser::tokenCoding2746_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2746_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2746(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2746_8;
	operand.values[9] = tokenCoding2746_9;
}
x86Token x86Parser::tokenBranches2744[] = {
	{x86Token::ADDRESSCLASS, 9, 1, 0, NULL,&x86Parser::TokenFunc2745,  },
	{x86Token::ADDRESSCLASS, 10, 1, 0, NULL,&x86Parser::TokenFunc2746,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2753(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 229;
}
x86Token x86Parser::tokenBranches2752[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2753,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2765(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 231;
}
x86Token x86Parser::tokenBranches2764[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2765,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2751[] = {
	{x86Token::REGISTER, 20, 0, 0, NULL, NULL, x86Parser::tokenBranches2752 },
	{x86Token::REGISTER, 21, 0, 0, NULL, NULL, x86Parser::tokenBranches2764 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2750[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2751 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2759(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 230;
}
x86Token x86Parser::tokenBranches2758[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2759,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2771(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 232;
}
x86Token x86Parser::tokenBranches2770[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2771,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2757[] = {
	{x86Token::REGISTER, 20, 0, 0, NULL, NULL, x86Parser::tokenBranches2758 },
	{x86Token::REGISTER, 21, 0, 0, NULL, NULL, x86Parser::tokenBranches2770 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2756[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2757 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2756(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2749[] = {
	{x86Token::REGISTER, 27, 0, 0, NULL, NULL, x86Parser::tokenBranches2750 },
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc2756, x86Parser::tokenBranches2756 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2748[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2749 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2777(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 233;
}
x86Token x86Parser::tokenBranches2776[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2777,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2789(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 235;
}
x86Token x86Parser::tokenBranches2788[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2789,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2775[] = {
	{x86Token::REGISTER, 20, 0, 0, NULL, NULL, x86Parser::tokenBranches2776 },
	{x86Token::REGISTER, 21, 0, 0, NULL, NULL, x86Parser::tokenBranches2788 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2774[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2775 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2783(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 234;
}
x86Token x86Parser::tokenBranches2782[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2783,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2795(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 236;
}
x86Token x86Parser::tokenBranches2794[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2795,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2781[] = {
	{x86Token::REGISTER, 20, 0, 0, NULL, NULL, x86Parser::tokenBranches2782 },
	{x86Token::REGISTER, 21, 0, 0, NULL, NULL, x86Parser::tokenBranches2794 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2780[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2781 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2780(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2773[] = {
	{x86Token::REGISTER, 27, 0, 0, NULL, NULL, x86Parser::tokenBranches2774 },
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc2780, x86Parser::tokenBranches2780 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2772[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2773 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2801(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 237;
}
x86Token x86Parser::tokenBranches2800[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2801,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2813(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 239;
}
x86Token x86Parser::tokenBranches2812[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2813,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2799[] = {
	{x86Token::REGISTER, 20, 0, 0, NULL, NULL, x86Parser::tokenBranches2800 },
	{x86Token::REGISTER, 21, 0, 0, NULL, NULL, x86Parser::tokenBranches2812 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2798[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2799 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2807(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 238;
}
x86Token x86Parser::tokenBranches2806[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2807,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2819(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 240;
}
x86Token x86Parser::tokenBranches2818[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2819,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2805[] = {
	{x86Token::REGISTER, 20, 0, 0, NULL, NULL, x86Parser::tokenBranches2806 },
	{x86Token::REGISTER, 21, 0, 0, NULL, NULL, x86Parser::tokenBranches2818 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2804[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2805 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2804(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2797[] = {
	{x86Token::REGISTER, 27, 0, 0, NULL, NULL, x86Parser::tokenBranches2798 },
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc2804, x86Parser::tokenBranches2804 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2796[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2797 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2747[] = {
	{x86Token::TOKEN, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches2748 },
	{x86Token::TOKEN, 8, 0, 0, NULL, NULL, x86Parser::tokenBranches2772 },
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches2796 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2824(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 241;
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
x86Token x86Parser::tokenBranches2823[] = {
	{x86Token::NUMBER, 8, 1, 0, NULL,&x86Parser::TokenFunc2824,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2826(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 242;
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
x86Token x86Parser::tokenBranches2825[] = {
	{x86Token::NUMBER, 8, 1, 0, NULL,&x86Parser::TokenFunc2826,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2828(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 243;
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
x86Token x86Parser::tokenBranches2827[] = {
	{x86Token::NUMBER, 8, 1, 0, NULL,&x86Parser::TokenFunc2828,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2830(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 243;
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
x86Token x86Parser::tokenBranches2829[] = {
	{x86Token::NUMBER, 8, 1, 0, NULL,&x86Parser::TokenFunc2830,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2832(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 242;
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
x86Token x86Parser::tokenBranches2831[] = {
	{x86Token::NUMBER, 8, 1, 0, NULL,&x86Parser::TokenFunc2832,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2836_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2836_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2836(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2836_8;
	operand.values[9] = tokenCoding2836_9;
}
x86Token x86Parser::tokenBranches2835[] = {
	{x86Token::ADDRESSCLASS, 10, 1, 0, NULL,&x86Parser::TokenFunc2836,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2840_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 19, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2840_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 34, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2840(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2840_8;
	operand.values[9] = tokenCoding2840_9;
}
x86Token x86Parser::tokenBranches2839[] = {
	{x86Token::ADDRESSCLASS, 10, 1, 0, NULL,&x86Parser::TokenFunc2840,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2838[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2839 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2843_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 20, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2843_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 35, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2843(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2843_8;
	operand.values[9] = tokenCoding2843_9;
}
x86Token x86Parser::tokenBranches2842[] = {
	{x86Token::ADDRESSCLASS, 10, 1, 0, NULL,&x86Parser::TokenFunc2843,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2841[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2842 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2846_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 21, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2846_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 38, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2846(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2846_8;
	operand.values[9] = tokenCoding2846_9;
}
x86Token x86Parser::tokenBranches2845[] = {
	{x86Token::ADDRESSCLASS, 10, 1, 0, NULL,&x86Parser::TokenFunc2846,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2844[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2845 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2852(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 244;
}
x86Token x86Parser::tokenBranches2851[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2852,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2860(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 245;
}
x86Token x86Parser::tokenBranches2859[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2860,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2874(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 247;
}
x86Token x86Parser::tokenBranches2873[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2874,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2859(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc2873(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2858[] = {
	{x86Token::NUMBER, 5, 0, 0, NULL,&x86Parser::TokenFunc2859, x86Parser::tokenBranches2859 },
	{x86Token::NUMBER, 6, 0, 0, NULL,&x86Parser::TokenFunc2873, x86Parser::tokenBranches2873 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2857[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2858 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2866(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 246;
}
x86Token x86Parser::tokenBranches2865[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2866,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2851(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc2857(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc2865(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2850[] = {
	{x86Token::NUMBER, 5, 0, 0, NULL,&x86Parser::TokenFunc2851, x86Parser::tokenBranches2851 },
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc2857, x86Parser::tokenBranches2857 },
	{x86Token::NUMBER, 6, 0, 0, NULL,&x86Parser::TokenFunc2865, x86Parser::tokenBranches2865 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2849[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2850 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2848[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches2849 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2847[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2848 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2880(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 248;
}
x86Token x86Parser::tokenBranches2879[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2880,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2888(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 249;
}
x86Token x86Parser::tokenBranches2887[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2888,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2902(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 251;
}
x86Token x86Parser::tokenBranches2901[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2902,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2887(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc2901(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2886[] = {
	{x86Token::NUMBER, 5, 0, 0, NULL,&x86Parser::TokenFunc2887, x86Parser::tokenBranches2887 },
	{x86Token::NUMBER, 6, 0, 0, NULL,&x86Parser::TokenFunc2901, x86Parser::tokenBranches2901 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2885[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2886 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2894(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 250;
}
x86Token x86Parser::tokenBranches2893[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2894,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2879(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc2885(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc2893(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2878[] = {
	{x86Token::NUMBER, 5, 0, 0, NULL,&x86Parser::TokenFunc2879, x86Parser::tokenBranches2879 },
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc2885, x86Parser::tokenBranches2885 },
	{x86Token::NUMBER, 6, 0, 0, NULL,&x86Parser::TokenFunc2893, x86Parser::tokenBranches2893 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2877[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2878 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2876[] = {
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches2877 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2875[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2876 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2908(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 252;
}
x86Token x86Parser::tokenBranches2907[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2908,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2916(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 253;
}
x86Token x86Parser::tokenBranches2915[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2916,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2930(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 255;
}
x86Token x86Parser::tokenBranches2929[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2930,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2915(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc2929(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2914[] = {
	{x86Token::NUMBER, 5, 0, 0, NULL,&x86Parser::TokenFunc2915, x86Parser::tokenBranches2915 },
	{x86Token::NUMBER, 6, 0, 0, NULL,&x86Parser::TokenFunc2929, x86Parser::tokenBranches2929 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2913[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2914 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2922(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 254;
}
x86Token x86Parser::tokenBranches2921[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc2922,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2907(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc2913(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc2921(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2906[] = {
	{x86Token::NUMBER, 5, 0, 0, NULL,&x86Parser::TokenFunc2907, x86Parser::tokenBranches2907 },
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc2913, x86Parser::tokenBranches2913 },
	{x86Token::NUMBER, 6, 0, 0, NULL,&x86Parser::TokenFunc2921, x86Parser::tokenBranches2921 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2905[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2906 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2904[] = {
	{x86Token::TOKEN, 9, 0, 1, NULL, NULL, x86Parser::tokenBranches2905 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2903[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2904 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2934(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 256;
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
Coding x86Parser::tokenCoding2946_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2946_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 138, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2946(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2946_8;
	operand.values[9] = tokenCoding2946_9;
}
x86Token x86Parser::tokenBranches2933[] = {
	{x86Token::NUMBER, 2, 1, 0, NULL,&x86Parser::TokenFunc2934,  },
	{x86Token::ADDRESSCLASS, 3, 1, 0, NULL,&x86Parser::TokenFunc2946,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2932[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches2933 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2931[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2932 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2938(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 257;
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
Coding x86Parser::tokenCoding2950_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2950_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 139, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2950(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2950_8;
	operand.values[9] = tokenCoding2950_9;
}
x86Token x86Parser::tokenBranches2937[] = {
	{x86Token::NUMBER, 3, 1, 0, NULL,&x86Parser::TokenFunc2938,  },
	{x86Token::ADDRESSCLASS, 4, 1, 0, NULL,&x86Parser::TokenFunc2950,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2936[] = {
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches2937 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2935[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2936 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2942(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 258;
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
Coding x86Parser::tokenCoding2954_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2954_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 139, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2954(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding2954_8;
	operand.values[9] = tokenCoding2954_9;
}
x86Token x86Parser::tokenBranches2941[] = {
	{x86Token::NUMBER, 4, 1, 0, NULL,&x86Parser::TokenFunc2942,  },
	{x86Token::ADDRESSCLASS, 5, 1, 0, NULL,&x86Parser::TokenFunc2954,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2940[] = {
	{x86Token::TOKEN, 9, 0, 1, NULL, NULL, x86Parser::tokenBranches2941 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2939[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2940 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3102_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3102_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 199, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3102(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding3102_8;
	operand.values[9] = tokenCoding3102_9;
}
x86Token x86Parser::tokenBranches3101[] = {
	{x86Token::NUMBER, 4, 1, 0, NULL,&x86Parser::TokenFunc3102,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2957_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 19, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2957_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 32, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2957(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding2957_8;
	operand.values[9] = tokenCoding2957_9;
}
Coding x86Parser::tokenCoding2960_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 20, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2960_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 33, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2960(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding2960_8;
	operand.values[9] = tokenCoding2960_9;
}
Coding x86Parser::tokenCoding2963_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 21, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2963_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 36, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2963(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding2963_8;
	operand.values[9] = tokenCoding2963_9;
}
x86Token x86Parser::tokenBranches2956[] = {
	{x86Token::REGISTERCLASS, 14, 1, 0, NULL,&x86Parser::TokenFunc2957,  },
	{x86Token::REGISTERCLASS, 15, 1, 0, NULL,&x86Parser::TokenFunc2960,  },
	{x86Token::REGISTERCLASS, 16, 1, 0, NULL,&x86Parser::TokenFunc2963,  },
	{x86Token::TOKEN, 9, 0, 1, NULL, NULL, x86Parser::tokenBranches3101 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2955[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2956 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2968_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2968_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 198, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2968(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding2968_8;
	operand.values[9] = tokenCoding2968_9;
}
x86Token x86Parser::tokenBranches2967[] = {
	{x86Token::NUMBER, 2, 1, 0, NULL,&x86Parser::TokenFunc2968,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2966[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches2967 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2965[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2966 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2964[] = {
	{x86Token::ADDRESSCLASS, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2965 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2973_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2973_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 199, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2973(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 195;
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
	operand.values[8] = tokenCoding2973_8;
	operand.values[9] = tokenCoding2973_9;
}
x86Token x86Parser::tokenBranches2972[] = {
	{x86Token::NUMBER, 3, 1, 0, NULL,&x86Parser::TokenFunc2973,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2971[] = {
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches2972 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2970[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2971 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2969[] = {
	{x86Token::ADDRESSCLASS, 4, 0, 0, NULL, NULL, x86Parser::tokenBranches2970 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding2978_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding2978_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 199, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc2978(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding2978_8;
	operand.values[9] = tokenCoding2978_9;
}
x86Token x86Parser::tokenBranches2977[] = {
	{x86Token::NUMBER, 4, 1, 0, NULL,&x86Parser::TokenFunc2978,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2976[] = {
	{x86Token::TOKEN, 9, 0, 1, NULL, NULL, x86Parser::tokenBranches2977 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2975[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2976 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2974[] = {
	{x86Token::ADDRESSCLASS, 5, 0, 0, NULL, NULL, x86Parser::tokenBranches2975 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2984(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 259;
}
x86Token x86Parser::tokenBranches2983[] = {
	{x86Token::REGISTER, 0, 1, 0, NULL,&x86Parser::TokenFunc2984,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2982[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2983 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2981[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2982 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2992(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 260;
}
x86Token x86Parser::tokenBranches2991[] = {
	{x86Token::REGISTER, 0, 1, 0, NULL,&x86Parser::TokenFunc2992,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2990[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2991 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2989[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2990 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3006(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 262;
}
x86Token x86Parser::tokenBranches3005[] = {
	{x86Token::REGISTER, 0, 1, 0, NULL,&x86Parser::TokenFunc3006,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3004[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3005 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3003[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches3004 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2989(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc3003(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2988[] = {
	{x86Token::NUMBER, 5, 0, 0, NULL,&x86Parser::TokenFunc2989, x86Parser::tokenBranches2989 },
	{x86Token::NUMBER, 6, 0, 0, NULL,&x86Parser::TokenFunc3003, x86Parser::tokenBranches3003 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2987[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2988 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2998(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 261;
}
x86Token x86Parser::tokenBranches2997[] = {
	{x86Token::REGISTER, 0, 1, 0, NULL,&x86Parser::TokenFunc2998,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2996[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches2997 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2995[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2996 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2981(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc2987(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc2995(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2980[] = {
	{x86Token::NUMBER, 5, 0, 0, NULL,&x86Parser::TokenFunc2981, x86Parser::tokenBranches2981 },
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc2987, x86Parser::tokenBranches2987 },
	{x86Token::NUMBER, 6, 0, 0, NULL,&x86Parser::TokenFunc2995, x86Parser::tokenBranches2995 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3066_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3066_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 136, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3066(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding3066_8;
	operand.values[9] = tokenCoding3066_9;
}
x86Token x86Parser::tokenBranches3065[] = {
	{x86Token::REGISTERCLASS, 1, 1, 0, NULL,&x86Parser::TokenFunc3066,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3064[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3065 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches2979[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches2980 },
	{x86Token::ADDRESSCLASS, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3064 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3012(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 263;
}
x86Token x86Parser::tokenBranches3011[] = {
	{x86Token::REGISTER, 2, 1, 0, NULL,&x86Parser::TokenFunc3012,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3010[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3011 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3009[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches3010 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3020(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 264;
}
x86Token x86Parser::tokenBranches3019[] = {
	{x86Token::REGISTER, 2, 1, 0, NULL,&x86Parser::TokenFunc3020,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3018[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3019 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3017[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches3018 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3034(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 266;
}
x86Token x86Parser::tokenBranches3033[] = {
	{x86Token::REGISTER, 2, 1, 0, NULL,&x86Parser::TokenFunc3034,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3032[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3033 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3031[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches3032 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3017(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc3031(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches3016[] = {
	{x86Token::NUMBER, 5, 0, 0, NULL,&x86Parser::TokenFunc3017, x86Parser::tokenBranches3017 },
	{x86Token::NUMBER, 6, 0, 0, NULL,&x86Parser::TokenFunc3031, x86Parser::tokenBranches3031 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3015[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3016 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3026(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 265;
}
x86Token x86Parser::tokenBranches3025[] = {
	{x86Token::REGISTER, 2, 1, 0, NULL,&x86Parser::TokenFunc3026,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3024[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3025 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3023[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches3024 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3009(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc3015(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc3023(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches3008[] = {
	{x86Token::NUMBER, 5, 0, 0, NULL,&x86Parser::TokenFunc3009, x86Parser::tokenBranches3009 },
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc3015, x86Parser::tokenBranches3015 },
	{x86Token::NUMBER, 6, 0, 0, NULL,&x86Parser::TokenFunc3023, x86Parser::tokenBranches3023 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3070_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3070_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 137, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3070(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding3070_8;
	operand.values[9] = tokenCoding3070_9;
}
Coding x86Parser::tokenCoding3086_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 22, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3086_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 140, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3086(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding3086_8;
	operand.values[9] = tokenCoding3086_9;
}
x86Token x86Parser::tokenBranches3069[] = {
	{x86Token::REGISTERCLASS, 4, 1, 0, NULL,&x86Parser::TokenFunc3070,  },
	{x86Token::REGISTERCLASS, 12, 1, 0, NULL,&x86Parser::TokenFunc3086,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3068[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3069 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3007[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches3008 },
	{x86Token::ADDRESSCLASS, 4, 0, 0, NULL, NULL, x86Parser::tokenBranches3068 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3040(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 267;
}
x86Token x86Parser::tokenBranches3039[] = {
	{x86Token::REGISTER, 3, 1, 0, NULL,&x86Parser::TokenFunc3040,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3038[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3039 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3037[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches3038 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3048(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 268;
}
x86Token x86Parser::tokenBranches3047[] = {
	{x86Token::REGISTER, 3, 1, 0, NULL,&x86Parser::TokenFunc3048,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3046[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3047 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3045[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches3046 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3062(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 270;
}
x86Token x86Parser::tokenBranches3061[] = {
	{x86Token::REGISTER, 3, 1, 0, NULL,&x86Parser::TokenFunc3062,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3060[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3061 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3059[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches3060 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3045(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc3059(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches3044[] = {
	{x86Token::NUMBER, 5, 0, 0, NULL,&x86Parser::TokenFunc3045, x86Parser::tokenBranches3045 },
	{x86Token::NUMBER, 6, 0, 0, NULL,&x86Parser::TokenFunc3059, x86Parser::tokenBranches3059 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3043[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3044 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3054(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 269;
}
x86Token x86Parser::tokenBranches3053[] = {
	{x86Token::REGISTER, 3, 1, 0, NULL,&x86Parser::TokenFunc3054,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3052[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3053 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3051[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches3052 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3037(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc3043(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc3051(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches3036[] = {
	{x86Token::NUMBER, 5, 0, 0, NULL,&x86Parser::TokenFunc3037, x86Parser::tokenBranches3037 },
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc3043, x86Parser::tokenBranches3043 },
	{x86Token::NUMBER, 6, 0, 0, NULL,&x86Parser::TokenFunc3051, x86Parser::tokenBranches3051 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3074_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3074_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 137, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3074(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding3074_8;
	operand.values[9] = tokenCoding3074_9;
}
x86Token x86Parser::tokenBranches3073[] = {
	{x86Token::REGISTERCLASS, 7, 1, 0, NULL,&x86Parser::TokenFunc3074,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3072[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3073 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3090_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 22, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3090_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 140, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3090(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding3090_8;
	operand.values[9] = tokenCoding3090_9;
}
x86Token x86Parser::tokenBranches3089[] = {
	{x86Token::REGISTERCLASS, 12, 1, 0, NULL,&x86Parser::TokenFunc3090,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3088[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3089 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3035[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches3036 },
	{x86Token::ADDRESSCLASS, 5, 0, 0, NULL, NULL, x86Parser::tokenBranches3072 },
	{x86Token::ADDRESSCLASS, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches3088 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3078_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 22, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3078_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 142, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3078(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3078_8;
	operand.values[9] = tokenCoding3078_9;
}
x86Token x86Parser::tokenBranches3077[] = {
	{x86Token::ADDRESSCLASS, 4, 1, 0, NULL,&x86Parser::TokenFunc3078,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3082_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 22, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3082_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 142, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3082(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3082_8;
	operand.values[9] = tokenCoding3082_9;
}
x86Token x86Parser::tokenBranches3081[] = {
	{x86Token::ADDRESSCLASS, 10, 1, 0, NULL,&x86Parser::TokenFunc3082,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3076[] = {
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches3077 },
	{x86Token::TOKEN, 9, 0, 1, NULL, NULL, x86Parser::tokenBranches3081 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3075[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3076 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3094_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3094_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 198, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3094(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding3094_8;
	operand.values[9] = tokenCoding3094_9;
}
x86Token x86Parser::tokenBranches3093[] = {
	{x86Token::NUMBER, 2, 1, 0, NULL,&x86Parser::TokenFunc3094,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3092[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches3093 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3091[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3092 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3098_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3098_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 199, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3098(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 195;
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
	operand.values[8] = tokenCoding3098_8;
	operand.values[9] = tokenCoding3098_9;
}
x86Token x86Parser::tokenBranches3097[] = {
	{x86Token::NUMBER, 3, 1, 0, NULL,&x86Parser::TokenFunc3098,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3096[] = {
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches3097 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3095[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3096 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc2838(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc2841(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc2844(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc2931(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc2935(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc2939(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc3075(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches2837[] = {
	{x86Token::REGISTERCLASS, 14, 0, 0, NULL,&x86Parser::TokenFunc2838, x86Parser::tokenBranches2838 },
	{x86Token::REGISTERCLASS, 15, 0, 0, NULL,&x86Parser::TokenFunc2841, x86Parser::tokenBranches2841 },
	{x86Token::REGISTERCLASS, 16, 0, 0, NULL,&x86Parser::TokenFunc2844, x86Parser::tokenBranches2844 },
	{x86Token::REGISTER, 0, 0, 0, NULL, NULL, x86Parser::tokenBranches2847 },
	{x86Token::REGISTER, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches2875 },
	{x86Token::REGISTER, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches2903 },
	{x86Token::REGISTERCLASS, 1, 0, 0, NULL,&x86Parser::TokenFunc2931, x86Parser::tokenBranches2931 },
	{x86Token::REGISTERCLASS, 4, 0, 0, NULL,&x86Parser::TokenFunc2935, x86Parser::tokenBranches2935 },
	{x86Token::REGISTERCLASS, 7, 0, 0, NULL,&x86Parser::TokenFunc2939, x86Parser::tokenBranches2939 },
	{x86Token::ADDRESSCLASS, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches2955 },
	{x86Token::TOKEN, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches2964 },
	{x86Token::TOKEN, 8, 0, 0, NULL, NULL, x86Parser::tokenBranches2969 },
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches2974 },
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches2979 },
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches3007 },
	{x86Token::TOKEN, 9, 0, 1, NULL, NULL, x86Parser::tokenBranches3035 },
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc3075, x86Parser::tokenBranches3075 },
	{x86Token::ADDRESSCLASS, 8, 0, 0, NULL, NULL, x86Parser::tokenBranches3091 },
	{x86Token::ADDRESSCLASS, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches3095 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3116(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 271;
}
x86Token x86Parser::tokenBranches3115[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3116,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3114[] = {
	{x86Token::REGISTER, 20, 0, 0, NULL, NULL, x86Parser::tokenBranches3115 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3113[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3114 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3129(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 272;
}
x86Token x86Parser::tokenBranches3128[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3129,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3127[] = {
	{x86Token::REGISTER, 20, 0, 0, NULL, NULL, x86Parser::tokenBranches3128 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3126[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3127 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3126(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches3112[] = {
	{x86Token::REGISTER, 27, 0, 0, NULL, NULL, x86Parser::tokenBranches3113 },
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc3126, x86Parser::tokenBranches3126 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3111[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches3112 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3110[] = {
	{x86Token::TOKEN, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches3111 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3109[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3110 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3108[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches3109 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3142(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 273;
}
x86Token x86Parser::tokenBranches3141[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3142,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3140[] = {
	{x86Token::REGISTER, 21, 0, 0, NULL, NULL, x86Parser::tokenBranches3141 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3139[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3140 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3155(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 274;
}
x86Token x86Parser::tokenBranches3154[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3155,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3153[] = {
	{x86Token::REGISTER, 21, 0, 0, NULL, NULL, x86Parser::tokenBranches3154 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3152[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3153 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3152(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches3138[] = {
	{x86Token::REGISTER, 27, 0, 0, NULL, NULL, x86Parser::tokenBranches3139 },
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc3152, x86Parser::tokenBranches3152 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3137[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches3138 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3136[] = {
	{x86Token::TOKEN, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches3137 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3135[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3136 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3134[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches3135 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3107[] = {
	{x86Token::REGISTER, 22, 0, 0, NULL, NULL, x86Parser::tokenBranches3108 },
	{x86Token::REGISTER, 23, 0, 0, NULL, NULL, x86Parser::tokenBranches3134 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3106[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches3107 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3105[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3106 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3104[] = {
	{x86Token::REGISTER, 24, 0, 0, NULL, NULL, x86Parser::tokenBranches3105 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3168(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 275;
}
x86Token x86Parser::tokenBranches3167[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3168,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3166[] = {
	{x86Token::REGISTER, 20, 0, 0, NULL, NULL, x86Parser::tokenBranches3167 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3165[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3166 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3181(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 276;
}
x86Token x86Parser::tokenBranches3180[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3181,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3179[] = {
	{x86Token::REGISTER, 20, 0, 0, NULL, NULL, x86Parser::tokenBranches3180 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3178[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3179 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3178(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches3164[] = {
	{x86Token::REGISTER, 27, 0, 0, NULL, NULL, x86Parser::tokenBranches3165 },
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc3178, x86Parser::tokenBranches3178 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3163[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches3164 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3162[] = {
	{x86Token::TOKEN, 8, 0, 0, NULL, NULL, x86Parser::tokenBranches3163 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3161[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3162 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3160[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches3161 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3194(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 277;
}
x86Token x86Parser::tokenBranches3193[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3194,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3192[] = {
	{x86Token::REGISTER, 21, 0, 0, NULL, NULL, x86Parser::tokenBranches3193 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3191[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3192 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3207(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 278;
}
x86Token x86Parser::tokenBranches3206[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3207,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3205[] = {
	{x86Token::REGISTER, 21, 0, 0, NULL, NULL, x86Parser::tokenBranches3206 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3204[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3205 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3204(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches3190[] = {
	{x86Token::REGISTER, 27, 0, 0, NULL, NULL, x86Parser::tokenBranches3191 },
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc3204, x86Parser::tokenBranches3204 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3189[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches3190 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3188[] = {
	{x86Token::TOKEN, 8, 0, 0, NULL, NULL, x86Parser::tokenBranches3189 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3187[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3188 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3186[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches3187 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3159[] = {
	{x86Token::REGISTER, 22, 0, 0, NULL, NULL, x86Parser::tokenBranches3160 },
	{x86Token::REGISTER, 23, 0, 0, NULL, NULL, x86Parser::tokenBranches3186 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3158[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches3159 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3157[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3158 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3156[] = {
	{x86Token::REGISTER, 24, 0, 0, NULL, NULL, x86Parser::tokenBranches3157 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3220(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 279;
}
x86Token x86Parser::tokenBranches3219[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3220,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3218[] = {
	{x86Token::REGISTER, 20, 0, 0, NULL, NULL, x86Parser::tokenBranches3219 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3217[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3218 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3233(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 280;
}
x86Token x86Parser::tokenBranches3232[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3233,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3231[] = {
	{x86Token::REGISTER, 20, 0, 0, NULL, NULL, x86Parser::tokenBranches3232 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3230[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3231 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3230(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches3216[] = {
	{x86Token::REGISTER, 27, 0, 0, NULL, NULL, x86Parser::tokenBranches3217 },
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc3230, x86Parser::tokenBranches3230 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3215[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches3216 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3214[] = {
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches3215 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3213[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3214 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3212[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches3213 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3246(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 281;
}
x86Token x86Parser::tokenBranches3245[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3246,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3244[] = {
	{x86Token::REGISTER, 21, 0, 0, NULL, NULL, x86Parser::tokenBranches3245 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3243[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3244 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3259(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 282;
}
x86Token x86Parser::tokenBranches3258[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3259,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3257[] = {
	{x86Token::REGISTER, 21, 0, 0, NULL, NULL, x86Parser::tokenBranches3258 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3256[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3257 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3256(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches3242[] = {
	{x86Token::REGISTER, 27, 0, 0, NULL, NULL, x86Parser::tokenBranches3243 },
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc3256, x86Parser::tokenBranches3256 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3241[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches3242 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3240[] = {
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches3241 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3239[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3240 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3238[] = {
	{x86Token::TOKEN, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches3239 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3211[] = {
	{x86Token::REGISTER, 22, 0, 0, NULL, NULL, x86Parser::tokenBranches3212 },
	{x86Token::REGISTER, 23, 0, 0, NULL, NULL, x86Parser::tokenBranches3238 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3210[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches3211 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3209[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3210 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3208[] = {
	{x86Token::REGISTER, 24, 0, 0, NULL, NULL, x86Parser::tokenBranches3209 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3103[] = {
	{x86Token::TOKEN, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches3104 },
	{x86Token::TOKEN, 8, 0, 0, NULL, NULL, x86Parser::tokenBranches3156 },
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches3208 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3274(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 283;
}
void x86Parser::TokenFunc3278(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 284;
}
void x86Parser::TokenFunc3282(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 285;
}
x86Token x86Parser::tokenBranches3273[] = {
	{x86Token::REGISTER, 0, 1, 0, NULL,&x86Parser::TokenFunc3274,  },
	{x86Token::REGISTER, 2, 1, 0, NULL,&x86Parser::TokenFunc3278,  },
	{x86Token::REGISTER, 3, 1, 0, NULL,&x86Parser::TokenFunc3282,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3272[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3273 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3272(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches3271[] = {
	{x86Token::NUMBER, 2, 0, 0, NULL,&x86Parser::TokenFunc3272, x86Parser::tokenBranches3272 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3285(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 286;
}
void x86Parser::TokenFunc3288(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 287;
}
void x86Parser::TokenFunc3291(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 288;
}
x86Token x86Parser::tokenBranches3284[] = {
	{x86Token::REGISTER, 0, 1, 0, NULL,&x86Parser::TokenFunc3285,  },
	{x86Token::REGISTER, 2, 1, 0, NULL,&x86Parser::TokenFunc3288,  },
	{x86Token::REGISTER, 3, 1, 0, NULL,&x86Parser::TokenFunc3291,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3283[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3284 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3270[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches3271 },
	{x86Token::REGISTER, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches3283 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3300(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 289;
}
x86Token x86Parser::tokenBranches3299[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3300,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3316(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 291;
}
x86Token x86Parser::tokenBranches3315[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3316,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3298[] = {
	{x86Token::REGISTER, 20, 0, 0, NULL, NULL, x86Parser::tokenBranches3299 },
	{x86Token::REGISTER, 21, 0, 0, NULL, NULL, x86Parser::tokenBranches3315 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3297[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3298 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3308(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 290;
}
x86Token x86Parser::tokenBranches3307[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3308,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3324(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 292;
}
x86Token x86Parser::tokenBranches3323[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3324,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3306[] = {
	{x86Token::REGISTER, 20, 0, 0, NULL, NULL, x86Parser::tokenBranches3307 },
	{x86Token::REGISTER, 21, 0, 0, NULL, NULL, x86Parser::tokenBranches3323 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3305[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3306 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3305(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches3296[] = {
	{x86Token::REGISTER, 27, 0, 0, NULL, NULL, x86Parser::tokenBranches3297 },
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc3305, x86Parser::tokenBranches3305 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3295[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches3296 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3332(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 293;
}
x86Token x86Parser::tokenBranches3331[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3332,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3348(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 295;
}
x86Token x86Parser::tokenBranches3347[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3348,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3330[] = {
	{x86Token::REGISTER, 20, 0, 0, NULL, NULL, x86Parser::tokenBranches3331 },
	{x86Token::REGISTER, 21, 0, 0, NULL, NULL, x86Parser::tokenBranches3347 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3329[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3330 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3340(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 294;
}
x86Token x86Parser::tokenBranches3339[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3340,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3356(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 296;
}
x86Token x86Parser::tokenBranches3355[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3356,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3338[] = {
	{x86Token::REGISTER, 20, 0, 0, NULL, NULL, x86Parser::tokenBranches3339 },
	{x86Token::REGISTER, 21, 0, 0, NULL, NULL, x86Parser::tokenBranches3355 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3337[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3338 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3337(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches3328[] = {
	{x86Token::REGISTER, 27, 0, 0, NULL, NULL, x86Parser::tokenBranches3329 },
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc3337, x86Parser::tokenBranches3337 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3327[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches3328 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3364(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 297;
}
x86Token x86Parser::tokenBranches3363[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3364,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3380(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 299;
}
x86Token x86Parser::tokenBranches3379[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3380,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3362[] = {
	{x86Token::REGISTER, 20, 0, 0, NULL, NULL, x86Parser::tokenBranches3363 },
	{x86Token::REGISTER, 21, 0, 0, NULL, NULL, x86Parser::tokenBranches3379 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3361[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3362 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3372(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 298;
}
x86Token x86Parser::tokenBranches3371[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3372,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3388(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 300;
}
x86Token x86Parser::tokenBranches3387[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3388,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3370[] = {
	{x86Token::REGISTER, 20, 0, 0, NULL, NULL, x86Parser::tokenBranches3371 },
	{x86Token::REGISTER, 21, 0, 0, NULL, NULL, x86Parser::tokenBranches3387 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3369[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3370 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3369(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches3360[] = {
	{x86Token::REGISTER, 27, 0, 0, NULL, NULL, x86Parser::tokenBranches3361 },
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc3369, x86Parser::tokenBranches3369 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3359[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches3360 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3294[] = {
	{x86Token::TOKEN, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches3295 },
	{x86Token::TOKEN, 8, 0, 0, NULL, NULL, x86Parser::tokenBranches3327 },
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches3359 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3293[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3294 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3292[] = {
	{x86Token::REGISTER, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches3293 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3396_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3396_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 143, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3396(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3396_8;
	operand.values[9] = tokenCoding3396_9;
}
x86Token x86Parser::tokenBranches3395[] = {
	{x86Token::ADDRESSCLASS, 4, 1, 0, NULL,&x86Parser::TokenFunc3396,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3398_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3398_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 143, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3398(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3398_8;
	operand.values[9] = tokenCoding3398_9;
}
x86Token x86Parser::tokenBranches3397[] = {
	{x86Token::ADDRESSCLASS, 5, 1, 0, NULL,&x86Parser::TokenFunc3398,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3393(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 301;
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
void x86Parser::TokenFunc3394(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 302;
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
void x86Parser::TokenFunc3399(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 303;
}
void x86Parser::TokenFunc3400(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 304;
}
void x86Parser::TokenFunc3401(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 305;
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
x86Token x86Parser::tokenBranches3392[] = {
	{x86Token::REGISTERCLASS, 4, 1, 0, NULL,&x86Parser::TokenFunc3393,  },
	{x86Token::REGISTERCLASS, 7, 1, 0, NULL,&x86Parser::TokenFunc3394,  },
	{x86Token::TOKEN, 8, 0, 0, NULL, NULL, x86Parser::tokenBranches3395 },
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches3397 },
	{x86Token::REGISTER, 28, 1, 0, NULL,&x86Parser::TokenFunc3399,  },
	{x86Token::REGISTER, 29, 1, 0, NULL,&x86Parser::TokenFunc3400,  },
	{x86Token::REGISTERCLASS, 12, 1, 0, NULL,&x86Parser::TokenFunc3401,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3412(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches3411[] = {
	{x86Token::NUMBER, 0, 1, 0, NULL,&x86Parser::TokenFunc3412,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3416(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 309;
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
Coding x86Parser::tokenCoding3420_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3420_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3420(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 311;
	operand.values[8] = tokenCoding3420_8;
	operand.values[9] = tokenCoding3420_9;
}
x86Token x86Parser::tokenBranches3415[] = {
	{x86Token::NUMBER, 3, 1, 0, NULL,&x86Parser::TokenFunc3416,  },
	{x86Token::ADDRESSCLASS, 4, 1, 0, NULL,&x86Parser::TokenFunc3420,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3418(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 310;
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
Coding x86Parser::tokenCoding3422_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3422_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3422(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 312;
	operand.values[8] = tokenCoding3422_8;
	operand.values[9] = tokenCoding3422_9;
}
x86Token x86Parser::tokenBranches3417[] = {
	{x86Token::NUMBER, 4, 1, 0, NULL,&x86Parser::TokenFunc3418,  },
	{x86Token::ADDRESSCLASS, 5, 1, 0, NULL,&x86Parser::TokenFunc3422,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3409(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 306;
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
void x86Parser::TokenFunc3410(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 307;
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
void x86Parser::TokenFunc3413(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 309;
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
void x86Parser::TokenFunc3414(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 310;
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
void x86Parser::TokenFunc3423(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 313;
}
void x86Parser::TokenFunc3424(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 314;
}
void x86Parser::TokenFunc3425(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 315;
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
x86Token x86Parser::tokenBranches3408[] = {
	{x86Token::REGISTERCLASS, 4, 1, 0, NULL,&x86Parser::TokenFunc3409,  },
	{x86Token::REGISTERCLASS, 7, 1, 0, NULL,&x86Parser::TokenFunc3410,  },
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches3411 },
	{x86Token::NUMBER, 5, 1, 0, NULL,&x86Parser::TokenFunc3413,  },
	{x86Token::NUMBER, 6, 1, 0, NULL,&x86Parser::TokenFunc3414,  },
	{x86Token::TOKEN, 8, 0, 0, NULL, NULL, x86Parser::tokenBranches3415 },
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches3417 },
	{x86Token::REGISTER, 28, 1, 0, NULL,&x86Parser::TokenFunc3423,  },
	{x86Token::REGISTER, 29, 1, 0, NULL,&x86Parser::TokenFunc3424,  },
	{x86Token::REGISTERCLASS, 12, 1, 0, NULL,&x86Parser::TokenFunc3425,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3438(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 316;
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
void x86Parser::TokenFunc3439(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 317;
}
x86Token x86Parser::tokenBranches3437[] = {
	{x86Token::NUMBER, 3, 1, 0, NULL,&x86Parser::TokenFunc3438,  },
	{x86Token::EMPTY, 0, 1, 0, NULL,&x86Parser::TokenFunc3439,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3441(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 318;
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
void x86Parser::TokenFunc3442(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 319;
}
x86Token x86Parser::tokenBranches3440[] = {
	{x86Token::NUMBER, 3, 1, 0, NULL,&x86Parser::TokenFunc3441,  },
	{x86Token::EMPTY, 0, 1, 0, NULL,&x86Parser::TokenFunc3442,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3456(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 320;
}
x86Token x86Parser::tokenBranches3455[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3456,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3468(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 322;
}
x86Token x86Parser::tokenBranches3467[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3468,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3454[] = {
	{x86Token::REGISTER, 22, 0, 0, NULL, NULL, x86Parser::tokenBranches3455 },
	{x86Token::REGISTER, 23, 0, 0, NULL, NULL, x86Parser::tokenBranches3467 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3453[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches3454 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3452[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3453 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3462(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 321;
}
x86Token x86Parser::tokenBranches3461[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3462,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3474(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 323;
}
x86Token x86Parser::tokenBranches3473[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3474,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3460[] = {
	{x86Token::REGISTER, 22, 0, 0, NULL, NULL, x86Parser::tokenBranches3461 },
	{x86Token::REGISTER, 23, 0, 0, NULL, NULL, x86Parser::tokenBranches3473 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3459[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3460 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3459(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches3458[] = {
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc3459, x86Parser::tokenBranches3459 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3451[] = {
	{x86Token::REGISTER, 24, 0, 0, NULL, NULL, x86Parser::tokenBranches3452 },
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches3458 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3480(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 324;
}
x86Token x86Parser::tokenBranches3479[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3480,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3492(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 326;
}
x86Token x86Parser::tokenBranches3491[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3492,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3478[] = {
	{x86Token::REGISTER, 22, 0, 0, NULL, NULL, x86Parser::tokenBranches3479 },
	{x86Token::REGISTER, 23, 0, 0, NULL, NULL, x86Parser::tokenBranches3491 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3477[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches3478 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3476[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3477 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3486(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 325;
}
x86Token x86Parser::tokenBranches3485[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3486,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3498(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 327;
}
x86Token x86Parser::tokenBranches3497[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3498,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3484[] = {
	{x86Token::REGISTER, 22, 0, 0, NULL, NULL, x86Parser::tokenBranches3485 },
	{x86Token::REGISTER, 23, 0, 0, NULL, NULL, x86Parser::tokenBranches3497 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3483[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3484 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3483(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches3482[] = {
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc3483, x86Parser::tokenBranches3483 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3475[] = {
	{x86Token::REGISTER, 24, 0, 0, NULL, NULL, x86Parser::tokenBranches3476 },
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches3482 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3504(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 328;
}
x86Token x86Parser::tokenBranches3503[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3504,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3516(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 330;
}
x86Token x86Parser::tokenBranches3515[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3516,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3502[] = {
	{x86Token::REGISTER, 22, 0, 0, NULL, NULL, x86Parser::tokenBranches3503 },
	{x86Token::REGISTER, 23, 0, 0, NULL, NULL, x86Parser::tokenBranches3515 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3501[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches3502 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3500[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3501 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3510(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 329;
}
x86Token x86Parser::tokenBranches3509[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3510,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3522(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 331;
}
x86Token x86Parser::tokenBranches3521[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3522,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3508[] = {
	{x86Token::REGISTER, 22, 0, 0, NULL, NULL, x86Parser::tokenBranches3509 },
	{x86Token::REGISTER, 23, 0, 0, NULL, NULL, x86Parser::tokenBranches3521 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3507[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3508 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3507(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches3506[] = {
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc3507, x86Parser::tokenBranches3507 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3499[] = {
	{x86Token::REGISTER, 24, 0, 0, NULL, NULL, x86Parser::tokenBranches3500 },
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches3506 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3450[] = {
	{x86Token::TOKEN, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches3451 },
	{x86Token::TOKEN, 8, 0, 0, NULL, NULL, x86Parser::tokenBranches3475 },
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches3499 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3563_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3563_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3563(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3563_8;
	operand.values[9] = tokenCoding3563_9;
}
Coding x86Parser::tokenCoding3564_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3564_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3564(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3564_8;
	operand.values[9] = tokenCoding3564_9;
}
x86Token x86Parser::tokenBranches3562[] = {
	{x86Token::ADDRESSCLASS, 9, 1, 0, NULL,&x86Parser::TokenFunc3563,  },
	{x86Token::ADDRESSCLASS, 10, 1, 0, NULL,&x86Parser::TokenFunc3564,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3574(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 332;
}
x86Token x86Parser::tokenBranches3573[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3574,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3586(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 334;
}
x86Token x86Parser::tokenBranches3585[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3586,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3572[] = {
	{x86Token::REGISTER, 22, 0, 0, NULL, NULL, x86Parser::tokenBranches3573 },
	{x86Token::REGISTER, 23, 0, 0, NULL, NULL, x86Parser::tokenBranches3585 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3571[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches3572 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3570[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3571 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3580(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 333;
}
x86Token x86Parser::tokenBranches3579[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3580,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3592(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 335;
}
x86Token x86Parser::tokenBranches3591[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3592,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3578[] = {
	{x86Token::REGISTER, 22, 0, 0, NULL, NULL, x86Parser::tokenBranches3579 },
	{x86Token::REGISTER, 23, 0, 0, NULL, NULL, x86Parser::tokenBranches3591 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3577[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3578 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3577(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches3576[] = {
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc3577, x86Parser::tokenBranches3577 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3569[] = {
	{x86Token::REGISTER, 24, 0, 0, NULL, NULL, x86Parser::tokenBranches3570 },
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches3576 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3598(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 336;
}
x86Token x86Parser::tokenBranches3597[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3598,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3610(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 338;
}
x86Token x86Parser::tokenBranches3609[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3610,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3596[] = {
	{x86Token::REGISTER, 22, 0, 0, NULL, NULL, x86Parser::tokenBranches3597 },
	{x86Token::REGISTER, 23, 0, 0, NULL, NULL, x86Parser::tokenBranches3609 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3595[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches3596 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3594[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3595 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3604(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 337;
}
x86Token x86Parser::tokenBranches3603[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3604,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3616(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 339;
}
x86Token x86Parser::tokenBranches3615[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3616,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3602[] = {
	{x86Token::REGISTER, 22, 0, 0, NULL, NULL, x86Parser::tokenBranches3603 },
	{x86Token::REGISTER, 23, 0, 0, NULL, NULL, x86Parser::tokenBranches3615 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3601[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3602 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3601(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches3600[] = {
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc3601, x86Parser::tokenBranches3601 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3593[] = {
	{x86Token::REGISTER, 24, 0, 0, NULL, NULL, x86Parser::tokenBranches3594 },
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches3600 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3622(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 340;
}
x86Token x86Parser::tokenBranches3621[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3622,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3634(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 342;
}
x86Token x86Parser::tokenBranches3633[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3634,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3620[] = {
	{x86Token::REGISTER, 22, 0, 0, NULL, NULL, x86Parser::tokenBranches3621 },
	{x86Token::REGISTER, 23, 0, 0, NULL, NULL, x86Parser::tokenBranches3633 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3619[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches3620 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3618[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3619 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3628(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 341;
}
x86Token x86Parser::tokenBranches3627[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3628,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3640(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 343;
}
x86Token x86Parser::tokenBranches3639[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3640,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3626[] = {
	{x86Token::REGISTER, 22, 0, 0, NULL, NULL, x86Parser::tokenBranches3627 },
	{x86Token::REGISTER, 23, 0, 0, NULL, NULL, x86Parser::tokenBranches3639 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3625[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3626 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3625(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches3624[] = {
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc3625, x86Parser::tokenBranches3625 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3617[] = {
	{x86Token::REGISTER, 24, 0, 0, NULL, NULL, x86Parser::tokenBranches3618 },
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches3624 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3568[] = {
	{x86Token::TOKEN, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches3569 },
	{x86Token::TOKEN, 8, 0, 0, NULL, NULL, x86Parser::tokenBranches3593 },
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches3617 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3645_8[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3645_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3645(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3645_8;
	operand.values[9] = tokenCoding3645_9;
}
Coding x86Parser::tokenCoding3646_8[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3646_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3646(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3646_8;
	operand.values[9] = tokenCoding3646_9;
}
x86Token x86Parser::tokenBranches3644[] = {
	{x86Token::ADDRESSCLASS, 9, 1, 0, NULL,&x86Parser::TokenFunc3645,  },
	{x86Token::ADDRESSCLASS, 10, 1, 0, NULL,&x86Parser::TokenFunc3646,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3652(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 344;
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
x86Token x86Parser::tokenBranches3651[] = {
	{x86Token::NUMBER, 2, 1, 0, NULL,&x86Parser::TokenFunc3652,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3650[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches3651 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3649[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3650 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3656(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 345;
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
x86Token x86Parser::tokenBranches3655[] = {
	{x86Token::NUMBER, 3, 1, 0, NULL,&x86Parser::TokenFunc3656,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3654[] = {
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches3655 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3653[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3654 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3660(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 346;
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
x86Token x86Parser::tokenBranches3659[] = {
	{x86Token::NUMBER, 4, 1, 0, NULL,&x86Parser::TokenFunc3660,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3658[] = {
	{x86Token::TOKEN, 9, 0, 1, NULL, NULL, x86Parser::tokenBranches3659 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3657[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3658 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3664_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3664_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 132, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3664(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding3664_8;
	operand.values[9] = tokenCoding3664_9;
}
x86Token x86Parser::tokenBranches3663[] = {
	{x86Token::REGISTERCLASS, 1, 1, 0, NULL,&x86Parser::TokenFunc3664,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3662[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3663 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3661[] = {
	{x86Token::ADDRESSCLASS, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3662 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3668_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3668_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 133, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3668(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding3668_8;
	operand.values[9] = tokenCoding3668_9;
}
x86Token x86Parser::tokenBranches3667[] = {
	{x86Token::REGISTERCLASS, 4, 1, 0, NULL,&x86Parser::TokenFunc3668,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3666[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3667 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3665[] = {
	{x86Token::ADDRESSCLASS, 4, 0, 0, NULL, NULL, x86Parser::tokenBranches3666 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3672_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3672_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 133, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3672(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding3672_8;
	operand.values[9] = tokenCoding3672_9;
}
x86Token x86Parser::tokenBranches3671[] = {
	{x86Token::REGISTERCLASS, 7, 1, 0, NULL,&x86Parser::TokenFunc3672,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3670[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3671 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3669[] = {
	{x86Token::ADDRESSCLASS, 5, 0, 0, NULL, NULL, x86Parser::tokenBranches3670 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3676_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3676_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 132, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3676(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3676_8;
	operand.values[9] = tokenCoding3676_9;
}
x86Token x86Parser::tokenBranches3675[] = {
	{x86Token::ADDRESSCLASS, 3, 1, 0, NULL,&x86Parser::TokenFunc3676,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3674[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches3675 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3673[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3674 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3680_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3680_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 133, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3680(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3680_8;
	operand.values[9] = tokenCoding3680_9;
}
x86Token x86Parser::tokenBranches3679[] = {
	{x86Token::ADDRESSCLASS, 4, 1, 0, NULL,&x86Parser::TokenFunc3680,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3678[] = {
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches3679 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3677[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3678 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3684_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3684_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 133, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3684(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3684_8;
	operand.values[9] = tokenCoding3684_9;
}
x86Token x86Parser::tokenBranches3683[] = {
	{x86Token::ADDRESSCLASS, 5, 1, 0, NULL,&x86Parser::TokenFunc3684,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3682[] = {
	{x86Token::TOKEN, 9, 0, 1, NULL, NULL, x86Parser::tokenBranches3683 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3681[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3682 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3687_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3687_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 246, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3687(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding3687_8;
	operand.values[9] = tokenCoding3687_9;
}
x86Token x86Parser::tokenBranches3686[] = {
	{x86Token::NUMBER, 2, 1, 0, NULL,&x86Parser::TokenFunc3687,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3685[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3686 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3690_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3690_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 247, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3690(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 195;
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
	operand.values[8] = tokenCoding3690_8;
	operand.values[9] = tokenCoding3690_9;
}
x86Token x86Parser::tokenBranches3689[] = {
	{x86Token::NUMBER, 3, 1, 0, NULL,&x86Parser::TokenFunc3690,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3688[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3689 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3693_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3693_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 247, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3693(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding3693_8;
	operand.values[9] = tokenCoding3693_9;
}
x86Token x86Parser::tokenBranches3692[] = {
	{x86Token::NUMBER, 4, 1, 0, NULL,&x86Parser::TokenFunc3693,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3691[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3692 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3697_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3697_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 246, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3697(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding3697_8;
	operand.values[9] = tokenCoding3697_9;
}
x86Token x86Parser::tokenBranches3696[] = {
	{x86Token::NUMBER, 2, 1, 0, NULL,&x86Parser::TokenFunc3697,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3695[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3696 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3694[] = {
	{x86Token::ADDRESSCLASS, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3695 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3701_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3701_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 247, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3701(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 195;
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
	operand.values[8] = tokenCoding3701_8;
	operand.values[9] = tokenCoding3701_9;
}
x86Token x86Parser::tokenBranches3700[] = {
	{x86Token::NUMBER, 3, 1, 0, NULL,&x86Parser::TokenFunc3701,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3699[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3700 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3698[] = {
	{x86Token::ADDRESSCLASS, 4, 0, 0, NULL, NULL, x86Parser::tokenBranches3699 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3705_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3705_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 247, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3705(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding3705_8;
	operand.values[9] = tokenCoding3705_9;
}
x86Token x86Parser::tokenBranches3704[] = {
	{x86Token::NUMBER, 4, 1, 0, NULL,&x86Parser::TokenFunc3705,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3703[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3704 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3702[] = {
	{x86Token::ADDRESSCLASS, 5, 0, 0, NULL, NULL, x86Parser::tokenBranches3703 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3673(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc3677(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc3681(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches3648[] = {
	{x86Token::REGISTER, 0, 0, 0, NULL, NULL, x86Parser::tokenBranches3649 },
	{x86Token::REGISTER, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches3653 },
	{x86Token::REGISTER, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3657 },
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches3661 },
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches3665 },
	{x86Token::TOKEN, 9, 0, 1, NULL, NULL, x86Parser::tokenBranches3669 },
	{x86Token::REGISTERCLASS, 1, 0, 0, NULL,&x86Parser::TokenFunc3673, x86Parser::tokenBranches3673 },
	{x86Token::REGISTERCLASS, 4, 0, 0, NULL,&x86Parser::TokenFunc3677, x86Parser::tokenBranches3677 },
	{x86Token::REGISTERCLASS, 7, 0, 0, NULL,&x86Parser::TokenFunc3681, x86Parser::tokenBranches3681 },
	{x86Token::ADDRESSCLASS, 8, 0, 0, NULL, NULL, x86Parser::tokenBranches3685 },
	{x86Token::ADDRESSCLASS, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches3688 },
	{x86Token::ADDRESSCLASS, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches3691 },
	{x86Token::TOKEN, 10, 0, 0, NULL, NULL, x86Parser::tokenBranches3694 },
	{x86Token::TOKEN, 8, 0, 0, NULL, NULL, x86Parser::tokenBranches3698 },
	{x86Token::TOKEN, 9, 0, 0, NULL, NULL, x86Parser::tokenBranches3702 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3708_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3708_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3708(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3708_8;
	operand.values[9] = tokenCoding3708_9;
}
Coding x86Parser::tokenCoding3709_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3709_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3709(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3709_8;
	operand.values[9] = tokenCoding3709_9;
}
x86Token x86Parser::tokenBranches3707[] = {
	{x86Token::ADDRESSCLASS, 9, 1, 0, NULL,&x86Parser::TokenFunc3708,  },
	{x86Token::ADDRESSCLASS, 10, 1, 0, NULL,&x86Parser::TokenFunc3709,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3711_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3711_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3711(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3711_8;
	operand.values[9] = tokenCoding3711_9;
}
Coding x86Parser::tokenCoding3712_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3712_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3712(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3712_8;
	operand.values[9] = tokenCoding3712_9;
}
x86Token x86Parser::tokenBranches3710[] = {
	{x86Token::ADDRESSCLASS, 9, 1, 0, NULL,&x86Parser::TokenFunc3711,  },
	{x86Token::ADDRESSCLASS, 10, 1, 0, NULL,&x86Parser::TokenFunc3712,  },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3720_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3720_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 192, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3720(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding3720_8;
	operand.values[9] = tokenCoding3720_9;
}
x86Token x86Parser::tokenBranches3719[] = {
	{x86Token::REGISTERCLASS, 1, 1, 0, NULL,&x86Parser::TokenFunc3720,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3718[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3719 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3717[] = {
	{x86Token::ADDRESSCLASS, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3718 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3716[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches3717 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3725_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3725_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 166, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3725(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3725_8;
	operand.values[9] = tokenCoding3725_9;
}
x86Token x86Parser::tokenBranches3724[] = {
	{x86Token::ADDRESSCLASS, 4, 1, 0, NULL,&x86Parser::TokenFunc3725,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3723[] = {
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches3724 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3722[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3723 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3729_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3729_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 166, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3729(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3729_8;
	operand.values[9] = tokenCoding3729_9;
}
x86Token x86Parser::tokenBranches3728[] = {
	{x86Token::ADDRESSCLASS, 5, 1, 0, NULL,&x86Parser::TokenFunc3729,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3727[] = {
	{x86Token::TOKEN, 9, 0, 1, NULL, NULL, x86Parser::tokenBranches3728 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3726[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3727 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3722(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc3726(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches3721[] = {
	{x86Token::REGISTERCLASS, 4, 0, 0, NULL,&x86Parser::TokenFunc3722, x86Parser::tokenBranches3722 },
	{x86Token::REGISTERCLASS, 7, 0, 0, NULL,&x86Parser::TokenFunc3726, x86Parser::tokenBranches3726 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3733(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 347;
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
x86Token x86Parser::tokenBranches3732[] = {
	{x86Token::REGISTERCLASS, 4, 1, 0, NULL,&x86Parser::TokenFunc3733,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3731[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3732 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3736(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 348;
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
x86Token x86Parser::tokenBranches3735[] = {
	{x86Token::REGISTERCLASS, 7, 1, 0, NULL,&x86Parser::TokenFunc3736,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3734[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3735 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3750_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3750_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 135, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3750(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3750_8;
	operand.values[9] = tokenCoding3750_9;
}
x86Token x86Parser::tokenBranches3749[] = {
	{x86Token::ADDRESSCLASS, 4, 1, 0, NULL,&x86Parser::TokenFunc3750,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3739(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 347;
}
x86Token x86Parser::tokenBranches3738[] = {
	{x86Token::REGISTER, 2, 1, 0, NULL,&x86Parser::TokenFunc3739,  },
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches3749 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3737[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3738 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3754_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3754_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 135, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3754(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3754_8;
	operand.values[9] = tokenCoding3754_9;
}
x86Token x86Parser::tokenBranches3753[] = {
	{x86Token::ADDRESSCLASS, 5, 1, 0, NULL,&x86Parser::TokenFunc3754,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3742(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 348;
}
x86Token x86Parser::tokenBranches3741[] = {
	{x86Token::REGISTER, 3, 1, 0, NULL,&x86Parser::TokenFunc3742,  },
	{x86Token::TOKEN, 9, 0, 1, NULL, NULL, x86Parser::tokenBranches3753 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3740[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3741 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3746_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3746_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 134, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3746(x86Operand &operand, int tokenPos)
{
	operand.values[8] = tokenCoding3746_8;
	operand.values[9] = tokenCoding3746_9;
}
x86Token x86Parser::tokenBranches3745[] = {
	{x86Token::ADDRESSCLASS, 3, 1, 0, NULL,&x86Parser::TokenFunc3746,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3744[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches3745 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3743[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3744 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3758_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3758_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 134, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3758(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding3758_8;
	operand.values[9] = tokenCoding3758_9;
}
x86Token x86Parser::tokenBranches3757[] = {
	{x86Token::REGISTERCLASS, 1, 1, 0, NULL,&x86Parser::TokenFunc3758,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3756[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3757 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3755[] = {
	{x86Token::ADDRESSCLASS, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3756 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3762_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3762_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 135, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3762(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding3762_8;
	operand.values[9] = tokenCoding3762_9;
}
x86Token x86Parser::tokenBranches3761[] = {
	{x86Token::REGISTERCLASS, 4, 1, 0, NULL,&x86Parser::TokenFunc3762,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3760[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3761 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3759[] = {
	{x86Token::ADDRESSCLASS, 4, 0, 0, NULL, NULL, x86Parser::tokenBranches3760 },
	{x86Token::EOT }
};
Coding x86Parser::tokenCoding3766_8[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 10, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::tokenCoding3766_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 135, 8},
	{ Coding::eot },
};
void x86Parser::TokenFunc3766(x86Operand &operand, int tokenPos)
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
	operand.values[8] = tokenCoding3766_8;
	operand.values[9] = tokenCoding3766_9;
}
x86Token x86Parser::tokenBranches3765[] = {
	{x86Token::REGISTERCLASS, 7, 1, 0, NULL,&x86Parser::TokenFunc3766,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3764[] = {
	{x86Token::TOKEN, 7, 0, 0, NULL, NULL, x86Parser::tokenBranches3765 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3763[] = {
	{x86Token::ADDRESSCLASS, 5, 0, 0, NULL, NULL, x86Parser::tokenBranches3764 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3737(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc3740(x86Operand &operand, int tokenPos)
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
void x86Parser::TokenFunc3743(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches3730[] = {
	{x86Token::REGISTER, 2, 0, 0, NULL, NULL, x86Parser::tokenBranches3731 },
	{x86Token::REGISTER, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3734 },
	{x86Token::REGISTERCLASS, 4, 0, 0, NULL,&x86Parser::TokenFunc3737, x86Parser::tokenBranches3737 },
	{x86Token::REGISTERCLASS, 7, 0, 0, NULL,&x86Parser::TokenFunc3740, x86Parser::tokenBranches3740 },
	{x86Token::REGISTERCLASS, 1, 0, 0, NULL,&x86Parser::TokenFunc3743, x86Parser::tokenBranches3743 },
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches3755 },
	{x86Token::TOKEN, 8, 0, 1, NULL, NULL, x86Parser::tokenBranches3759 },
	{x86Token::TOKEN, 9, 0, 1, NULL, NULL, x86Parser::tokenBranches3763 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3773(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 349;
}
x86Token x86Parser::tokenBranches3772[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3773,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3785(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 351;
}
x86Token x86Parser::tokenBranches3784[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3785,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3771[] = {
	{x86Token::REGISTER, 14, 0, 0, NULL, NULL, x86Parser::tokenBranches3772 },
	{x86Token::REGISTER, 15, 0, 0, NULL, NULL, x86Parser::tokenBranches3784 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3770[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3771 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3779(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 350;
}
x86Token x86Parser::tokenBranches3778[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3779,  },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3791(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 352;
}
x86Token x86Parser::tokenBranches3790[] = {
	{x86Token::TOKEN, 2, 1, 0, NULL,&x86Parser::TokenFunc3791,  },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3777[] = {
	{x86Token::REGISTER, 14, 0, 0, NULL, NULL, x86Parser::tokenBranches3778 },
	{x86Token::REGISTER, 15, 0, 0, NULL, NULL, x86Parser::tokenBranches3790 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3776[] = {
	{x86Token::TOKEN, 3, 0, 0, NULL, NULL, x86Parser::tokenBranches3777 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3776(x86Operand &operand, int tokenPos)
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
x86Token x86Parser::tokenBranches3769[] = {
	{x86Token::REGISTER, 27, 0, 0, NULL, NULL, x86Parser::tokenBranches3770 },
	{x86Token::REGISTERCLASS, 12, 0, 0, NULL,&x86Parser::TokenFunc3776, x86Parser::tokenBranches3776 },
	{x86Token::EOT }
};
x86Token x86Parser::tokenBranches3768[] = {
	{x86Token::TOKEN, 1, 0, 0, NULL, NULL, x86Parser::tokenBranches3769 },
	{x86Token::EOT }
};
void x86Parser::TokenFunc3792(x86Operand &operand, int tokenPos)
{
	operand.operandCoding = 353;
}
x86Token x86Parser::tokenBranches3767[] = {
	{x86Token::TOKEN, 10, 0, 1, NULL, NULL, x86Parser::tokenBranches3768 },
	{x86Token::EMPTY, 0, 1, 0, NULL,&x86Parser::TokenFunc3792,  },
	{x86Token::EOT }
};
bool x86Parser::Opcode0(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1764, operand);
	return rv;
}
bool x86Parser::Opcode1(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1766, operand);
	return rv;
}
bool x86Parser::Opcode2(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1768, operand);
	return rv;
}
bool x86Parser::Opcode3(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1777, operand);
	return rv;
}
bool x86Parser::Opcode4(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1786, operand);
	return rv;
}
bool x86Parser::Opcode5(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1868, operand);
	return rv;
}
bool x86Parser::Opcode6(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1887, operand);
	return rv;
}
bool x86Parser::Opcode7(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1897, operand);
	return rv;
}
bool x86Parser::Opcode8(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1907, operand);
	return rv;
}
bool x86Parser::Opcode9(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1919, operand);
	return rv;
}
bool x86Parser::Opcode10(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1925, operand);
	return rv;
}
bool x86Parser::Opcode11(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1930, operand);
	return rv;
}
bool x86Parser::Opcode12(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1933, operand);
	return rv;
}
bool x86Parser::Opcode13(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1942, operand);
	return rv;
}
bool x86Parser::Opcode14(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1951, operand);
	return rv;
}
bool x86Parser::Opcode15(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1954, operand);
	return rv;
}
bool x86Parser::Opcode16(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches1976, operand);
	return rv;
}
bool x86Parser::Opcode17(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2047, operand);
	return rv;
}
bool x86Parser::Opcode18(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2072, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings19_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 55, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode19(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings19_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings20_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 213, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 10, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode20(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings20_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings21_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 212, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 10, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode21(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings21_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings22_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 63, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode22(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings22_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings23_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings23_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 20, 8},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings23_24[] = {
	{ Coding::valSpecified, 16},
	{ Coding::eot },
};
bool x86Parser::Opcode23(x86Operand &operand)
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
Coding x86Parser::OpcodeCodings24_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings24_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 8},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings24_24[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
bool x86Parser::Opcode24(x86Operand &operand)
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
Coding x86Parser::OpcodeCodings25_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings25_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 36, 8},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings25_24[] = {
	{ Coding::valSpecified, 32},
	{ Coding::eot },
};
bool x86Parser::Opcode25(x86Operand &operand)
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
bool x86Parser::Opcode26(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2081, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings27_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 98, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode27(x86Operand &operand)
{
	operand.values[23] = OpcodeCodings27_23;
	bool rv;
	{
		rv = Opcode2(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings28_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 188, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode28(x86Operand &operand)
{
	operand.values[23] = OpcodeCodings28_23;
	bool rv;
	{
		rv = Opcode2(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings29_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 189, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode29(x86Operand &operand)
{
	operand.values[23] = OpcodeCodings29_23;
	bool rv;
	{
		rv = Opcode2(operand);
	}
	return rv;
}
bool x86Parser::Opcode30(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2088, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings31_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings31_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 186, 8},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings31_24[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 163, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode31(x86Operand &operand)
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
Coding x86Parser::OpcodeCodings32_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings32_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 186, 8},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings32_24[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 187, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode32(x86Operand &operand)
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
Coding x86Parser::OpcodeCodings33_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings33_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 186, 8},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings33_24[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 179, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode33(x86Operand &operand)
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
Coding x86Parser::OpcodeCodings34_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings34_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 186, 8},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings34_24[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 171, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode34(x86Operand &operand)
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
bool x86Parser::Opcode35(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2094, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings36_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 152, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode36(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings36_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings37_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 153, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode37(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings37_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings38_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 248, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode38(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings38_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings39_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 252, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode39(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings39_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings40_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 250, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode40(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings40_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings41_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode41(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings41_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings42_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 245, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode42(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings42_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings43_25[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
bool x86Parser::Opcode43(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings43_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings44_25[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
bool x86Parser::Opcode44(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings44_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings45_25[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
bool x86Parser::Opcode45(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings45_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings46_25[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
bool x86Parser::Opcode46(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings46_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings47_25[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
bool x86Parser::Opcode47(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings47_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings48_25[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
bool x86Parser::Opcode48(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings48_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings49_25[] = {
	{ Coding::valSpecified, 15},
	{ Coding::eot },
};
bool x86Parser::Opcode49(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings49_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings50_25[] = {
	{ Coding::valSpecified, 13},
	{ Coding::eot },
};
bool x86Parser::Opcode50(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings50_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings51_25[] = {
	{ Coding::valSpecified, 12},
	{ Coding::eot },
};
bool x86Parser::Opcode51(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings51_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings52_25[] = {
	{ Coding::valSpecified, 14},
	{ Coding::eot },
};
bool x86Parser::Opcode52(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings52_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings53_25[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
bool x86Parser::Opcode53(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings53_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings54_25[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
bool x86Parser::Opcode54(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings54_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings55_25[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
bool x86Parser::Opcode55(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings55_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings56_25[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
bool x86Parser::Opcode56(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings56_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings57_25[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
bool x86Parser::Opcode57(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings57_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings58_25[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
bool x86Parser::Opcode58(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings58_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings59_25[] = {
	{ Coding::valSpecified, 14},
	{ Coding::eot },
};
bool x86Parser::Opcode59(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings59_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings60_25[] = {
	{ Coding::valSpecified, 12},
	{ Coding::eot },
};
bool x86Parser::Opcode60(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings60_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings61_25[] = {
	{ Coding::valSpecified, 13},
	{ Coding::eot },
};
bool x86Parser::Opcode61(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings61_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings62_25[] = {
	{ Coding::valSpecified, 15},
	{ Coding::eot },
};
bool x86Parser::Opcode62(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings62_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings63_25[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
bool x86Parser::Opcode63(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings63_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings64_25[] = {
	{ Coding::valSpecified, 11},
	{ Coding::eot },
};
bool x86Parser::Opcode64(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings64_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings65_25[] = {
	{ Coding::valSpecified, 9},
	{ Coding::eot },
};
bool x86Parser::Opcode65(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings65_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings66_25[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
bool x86Parser::Opcode66(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings66_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings67_25[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
bool x86Parser::Opcode67(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings67_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings68_25[] = {
	{ Coding::valSpecified, 10},
	{ Coding::eot },
};
bool x86Parser::Opcode68(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings68_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings69_25[] = {
	{ Coding::valSpecified, 10},
	{ Coding::eot },
};
bool x86Parser::Opcode69(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings69_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings70_25[] = {
	{ Coding::valSpecified, 11},
	{ Coding::eot },
};
bool x86Parser::Opcode70(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings70_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings71_25[] = {
	{ Coding::valSpecified, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode71(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings71_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings72_25[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
bool x86Parser::Opcode72(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings72_25;
	bool rv;
	{
		rv = Opcode13(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings73_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings73_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 60, 8},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings73_24[] = {
	{ Coding::valSpecified, 56},
	{ Coding::eot },
};
bool x86Parser::Opcode73(x86Operand &operand)
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
bool x86Parser::Opcode74(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2157, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings75_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 166, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode75(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings75_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings76_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 167, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode76(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings76_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings77_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 167, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode77(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings77_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
bool x86Parser::Opcode78(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2317, operand);
	return rv;
}
bool x86Parser::Opcode79(x86Operand &operand)
{
	bool rv = true;
	return rv;
}
Coding x86Parser::OpcodeCodings80_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 162, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode80(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings80_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings81_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 153, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode81(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings81_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings82_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 152, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode82(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings82_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings83_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 39, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode83(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings83_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings84_9[] = {
	{ Coding::valSpecified, 47},
	{ Coding::eot },
};
bool x86Parser::Opcode84(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings84_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings85_8[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings85_9[] = {
	{ Coding::valSpecified, 9},
	{ Coding::eot },
};
bool x86Parser::Opcode85(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings85_8;
	operand.values[9] = OpcodeCodings85_9;
	bool rv;
	{
		rv = Opcode6(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings86_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
bool x86Parser::Opcode86(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings86_8;
	bool rv;
	{
		rv = Opcode7(operand);
	}
	return rv;
}
bool x86Parser::Opcode87(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2330, operand);
	return rv;
}
bool x86Parser::Opcode88(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2336, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings89_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 240, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode89(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings89_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings90_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 225, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode90(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings90_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings91_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings91_26[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings91_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 216, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode91(x86Operand &operand)
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
Coding x86Parser::OpcodeCodings92_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings92_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 222, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode92(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings92_8;
	operand.values[9] = OpcodeCodings92_9;
	bool rv;
	{
		rv = Opcode9(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings93_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings93_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 223, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode93(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings93_8;
	operand.values[9] = OpcodeCodings93_9;
	bool rv;
	{
		rv = Opcode11(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings94_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings94_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 223, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode94(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings94_8;
	operand.values[9] = OpcodeCodings94_9;
	bool rv;
	{
		rv = Opcode11(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings95_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 224, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode95(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings95_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings96_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 155, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 219, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 226, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode96(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings96_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings97_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 219, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 226, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode97(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings97_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings98_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings98_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 216, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode98(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings98_8;
	operand.values[23] = OpcodeCodings98_23;
	bool rv = ParseOperands(tokenBranches2349, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings99_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings99_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 216, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode99(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings99_8;
	operand.values[23] = OpcodeCodings99_23;
	bool rv = ParseOperands(tokenBranches2358, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings100_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 222, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode100(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings100_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings101_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 255, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode101(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings101_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings102_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 246, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode102(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings102_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings103_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 219, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 225, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode103(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings103_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings104_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings104_26[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings104_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 216, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode104(x86Operand &operand)
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
Coding x86Parser::OpcodeCodings105_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings105_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 222, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode105(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings105_8;
	operand.values[9] = OpcodeCodings105_9;
	bool rv;
	{
		rv = Opcode9(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings106_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings106_26[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings106_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 216, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode106(x86Operand &operand)
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
Coding x86Parser::OpcodeCodings107_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings107_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 222, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode107(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings107_8;
	operand.values[9] = OpcodeCodings107_9;
	bool rv;
	{
		rv = Opcode9(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings108_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 219, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 224, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode108(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings108_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
bool x86Parser::Opcode109(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2373, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings110_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings110_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 218, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode110(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings110_8;
	operand.values[23] = OpcodeCodings110_23;
	bool rv;
	{
		rv = Opcode10(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings111_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings111_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 218, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode111(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings111_8;
	operand.values[23] = OpcodeCodings111_23;
	bool rv;
	{
		rv = Opcode10(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings112_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings112_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 218, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode112(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings112_8;
	operand.values[23] = OpcodeCodings112_23;
	bool rv;
	{
		rv = Opcode10(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings113_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings113_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 218, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode113(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings113_8;
	operand.values[23] = OpcodeCodings113_23;
	bool rv;
	{
		rv = Opcode10(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings114_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings114_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 218, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode114(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings114_8;
	operand.values[23] = OpcodeCodings114_23;
	bool rv;
	{
		rv = Opcode10(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings115_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings115_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 219, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode115(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings115_8;
	operand.values[23] = OpcodeCodings115_23;
	bool rv = ParseOperands(tokenBranches2380, operand);
	if (!rv)
	{
		rv = Opcode10(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings116_8[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings116_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 218, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode116(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings116_8;
	operand.values[23] = OpcodeCodings116_23;
	bool rv;
	{
		rv = Opcode10(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings117_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 247, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode117(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings117_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings118_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 155, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 219, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 227, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode118(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings118_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings119_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 219, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 227, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode119(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings119_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings120_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings120_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 219, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode120(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings120_8;
	operand.values[23] = OpcodeCodings120_23;
	bool rv;
	{
		rv = Opcode10(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings121_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings121_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 219, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode121(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings121_8;
	operand.values[23] = OpcodeCodings121_23;
	bool rv = ParseOperands(tokenBranches2388, operand);
	if (!rv)
	{
		rv = Opcode10(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings122_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings122_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 218, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode122(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings122_8;
	operand.values[23] = OpcodeCodings122_23;
	bool rv;
	{
		rv = Opcode10(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings123_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings123_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 218, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode123(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings123_8;
	operand.values[23] = OpcodeCodings123_23;
	bool rv;
	{
		rv = Opcode10(operand);
	}
	return rv;
}
bool x86Parser::Opcode124(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2393, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings125_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 232, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode125(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings125_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
bool x86Parser::Opcode126(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2402, operand);
	return rv;
}
bool x86Parser::Opcode127(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2405, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings128_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 234, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode128(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings128_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings129_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 233, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode129(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings129_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings130_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 236, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode130(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings130_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings131_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 237, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode131(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings131_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings132_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 235, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode132(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings132_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings133_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 238, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode133(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings133_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings134_8[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings134_26[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings134_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 216, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode134(x86Operand &operand)
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
Coding x86Parser::OpcodeCodings135_8[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings135_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 222, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode135(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings135_8;
	operand.values[9] = OpcodeCodings135_9;
	bool rv;
	{
		rv = Opcode9(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings136_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 208, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode136(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings136_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
bool x86Parser::Opcode137(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2416, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings138_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 243, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode138(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings138_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings139_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 248, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode139(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings139_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings140_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 245, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode140(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings140_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings141_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 242, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode141(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings141_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings142_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 252, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode142(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings142_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
bool x86Parser::Opcode143(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2423, operand);
	return rv;
}
bool x86Parser::Opcode144(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2425, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings145_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 253, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode145(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings145_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings146_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 219, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 228, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode146(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings146_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings147_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 254, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode147(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings147_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings148_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 251, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode148(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings148_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings149_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 250, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode149(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings149_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
bool x86Parser::Opcode150(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2432, operand);
	return rv;
}
bool x86Parser::Opcode151(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2438, operand);
	return rv;
}
bool x86Parser::Opcode152(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2441, operand);
	return rv;
}
bool x86Parser::Opcode153(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2444, operand);
	return rv;
}
bool x86Parser::Opcode154(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2446, operand);
	return rv;
}
bool x86Parser::Opcode155(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2448, operand);
	return rv;
}
bool x86Parser::Opcode156(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2456, operand);
	return rv;
}
bool x86Parser::Opcode157(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2460, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings158_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings158_26[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings158_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 216, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode158(x86Operand &operand)
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
Coding x86Parser::OpcodeCodings159_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings159_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 222, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode159(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings159_8;
	operand.values[9] = OpcodeCodings159_9;
	bool rv;
	{
		rv = Opcode9(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings160_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings160_26[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings160_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 216, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode160(x86Operand &operand)
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
Coding x86Parser::OpcodeCodings161_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings161_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 222, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode161(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings161_8;
	operand.values[9] = OpcodeCodings161_9;
	bool rv;
	{
		rv = Opcode9(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings162_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 228, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode162(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings162_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings163_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings163_9[] = {
	{ Coding::valSpecified, 221},
	{ Coding::eot },
};
bool x86Parser::Opcode163(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings163_8;
	operand.values[9] = OpcodeCodings163_9;
	bool rv;
	{
		rv = Opcode9(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings164_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings164_9[] = {
	{ Coding::valSpecified, 221},
	{ Coding::eot },
};
bool x86Parser::Opcode164(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings164_8;
	operand.values[9] = OpcodeCodings164_9;
	bool rv;
	{
		rv = Opcode9(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings165_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 218, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 233, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode165(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings165_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings166_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 155, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode166(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings166_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings167_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 229, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode167(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings167_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings168_8[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings168_9[] = {
	{ Coding::valSpecified, 217},
	{ Coding::eot },
};
bool x86Parser::Opcode168(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings168_8;
	operand.values[9] = OpcodeCodings168_9;
	bool rv;
	{
		rv = Opcode9(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings169_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 244, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode169(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings169_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings170_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 241, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode170(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings170_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings171_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 217, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 249, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode171(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings171_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings172_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 244, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode172(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings172_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings173_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 167, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode173(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings173_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings174_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
bool x86Parser::Opcode174(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings174_8;
	bool rv;
	{
		rv = Opcode7(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings175_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
bool x86Parser::Opcode175(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings175_8;
	bool rv = ParseOperands(tokenBranches2481, operand);
	if (!rv)
	{
		rv = Opcode7(operand);
	}
	return rv;
}
bool x86Parser::Opcode176(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2532, operand);
	return rv;
}
bool x86Parser::Opcode177(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2554, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings178_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 108, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode178(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings178_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings179_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 109, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode179(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings179_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings180_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 109, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode180(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings180_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings181_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings181_9[] = {
	{ Coding::valSpecified, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode181(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings181_8;
	operand.values[9] = OpcodeCodings181_9;
	bool rv;
	{
		rv = Opcode6(operand);
	}
	return rv;
}
bool x86Parser::Opcode182(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2655, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings183_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 241, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode183(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings183_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings184_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 204, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode184(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings184_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings185_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 206, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode185(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings185_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings186_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 8, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode186(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings186_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
bool x86Parser::Opcode187(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2662, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings188_9[] = {
	{ Coding::valSpecified, 207},
	{ Coding::eot },
};
bool x86Parser::Opcode188(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings188_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings189_9[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::valSpecified, 207},
	{ Coding::eot },
};
bool x86Parser::Opcode189(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings189_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings190_9[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::valSpecified, 207},
	{ Coding::eot },
};
bool x86Parser::Opcode190(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings190_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings191_25[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
bool x86Parser::Opcode191(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings191_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings192_25[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
bool x86Parser::Opcode192(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings192_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings193_25[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
bool x86Parser::Opcode193(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings193_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings194_25[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
bool x86Parser::Opcode194(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings194_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings195_25[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
bool x86Parser::Opcode195(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings195_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
bool x86Parser::Opcode196(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2672, operand);
	return rv;
}
bool x86Parser::Opcode197(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2674, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings198_25[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
bool x86Parser::Opcode198(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings198_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings199_25[] = {
	{ Coding::valSpecified, 15},
	{ Coding::eot },
};
bool x86Parser::Opcode199(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings199_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings200_25[] = {
	{ Coding::valSpecified, 13},
	{ Coding::eot },
};
bool x86Parser::Opcode200(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings200_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings201_25[] = {
	{ Coding::valSpecified, 12},
	{ Coding::eot },
};
bool x86Parser::Opcode201(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings201_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings202_25[] = {
	{ Coding::valSpecified, 14},
	{ Coding::eot },
};
bool x86Parser::Opcode202(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings202_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
bool x86Parser::Opcode203(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2681, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings204_25[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
bool x86Parser::Opcode204(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings204_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings205_25[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
bool x86Parser::Opcode205(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings205_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings206_25[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
bool x86Parser::Opcode206(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings206_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings207_25[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
bool x86Parser::Opcode207(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings207_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings208_25[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
bool x86Parser::Opcode208(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings208_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings209_25[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
bool x86Parser::Opcode209(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings209_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings210_25[] = {
	{ Coding::valSpecified, 14},
	{ Coding::eot },
};
bool x86Parser::Opcode210(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings210_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings211_25[] = {
	{ Coding::valSpecified, 12},
	{ Coding::eot },
};
bool x86Parser::Opcode211(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings211_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings212_25[] = {
	{ Coding::valSpecified, 13},
	{ Coding::eot },
};
bool x86Parser::Opcode212(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings212_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings213_25[] = {
	{ Coding::valSpecified, 15},
	{ Coding::eot },
};
bool x86Parser::Opcode213(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings213_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings214_25[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
bool x86Parser::Opcode214(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings214_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings215_25[] = {
	{ Coding::valSpecified, 11},
	{ Coding::eot },
};
bool x86Parser::Opcode215(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings215_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings216_25[] = {
	{ Coding::valSpecified, 9},
	{ Coding::eot },
};
bool x86Parser::Opcode216(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings216_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings217_25[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
bool x86Parser::Opcode217(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings217_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings218_25[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
bool x86Parser::Opcode218(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings218_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings219_25[] = {
	{ Coding::valSpecified, 10},
	{ Coding::eot },
};
bool x86Parser::Opcode219(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings219_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings220_25[] = {
	{ Coding::valSpecified, 10},
	{ Coding::eot },
};
bool x86Parser::Opcode220(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings220_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings221_25[] = {
	{ Coding::valSpecified, 11},
	{ Coding::eot },
};
bool x86Parser::Opcode221(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings221_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings222_25[] = {
	{ Coding::valSpecified, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode222(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings222_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings223_25[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
bool x86Parser::Opcode223(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings223_25;
	bool rv;
	{
		rv = Opcode12(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings224_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 159, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode224(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings224_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings225_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode225(x86Operand &operand)
{
	operand.values[23] = OpcodeCodings225_23;
	bool rv;
	{
		rv = Opcode2(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings226_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 197, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode226(x86Operand &operand)
{
	operand.values[23] = OpcodeCodings226_23;
	bool rv;
	{
		rv = Opcode3(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings227_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 141, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode227(x86Operand &operand)
{
	operand.values[23] = OpcodeCodings227_23;
	bool rv;
	{
		rv = Opcode3(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings228_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 201, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode228(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings228_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings229_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 196, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode229(x86Operand &operand)
{
	operand.values[23] = OpcodeCodings229_23;
	bool rv;
	{
		rv = Opcode3(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings230_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 180, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode230(x86Operand &operand)
{
	operand.values[23] = OpcodeCodings230_23;
	bool rv;
	{
		rv = Opcode3(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings231_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings231_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode231(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings231_8;
	operand.values[9] = OpcodeCodings231_9;
	bool rv;
	{
		rv = Opcode18(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings232_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 181, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode232(x86Operand &operand)
{
	operand.values[23] = OpcodeCodings232_23;
	bool rv;
	{
		rv = Opcode3(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings233_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings233_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode233(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings233_8;
	operand.values[9] = OpcodeCodings233_9;
	bool rv;
	{
		rv = Opcode18(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings234_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings234_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode234(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings234_8;
	operand.values[9] = OpcodeCodings234_9;
	bool rv = ParseOperands(tokenBranches2744, operand);
	if (!rv)
	{
		rv = Opcode18(operand);
	}
	return rv;
}
bool x86Parser::Opcode235(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2747, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings236_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 172, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode236(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings236_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings237_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 173, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode237(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings237_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings238_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 173, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode238(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings238_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
bool x86Parser::Opcode239(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2823, operand);
	return rv;
}
bool x86Parser::Opcode240(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2825, operand);
	return rv;
}
bool x86Parser::Opcode241(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2827, operand);
	return rv;
}
bool x86Parser::Opcode242(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2829, operand);
	return rv;
}
bool x86Parser::Opcode243(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2831, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings244_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 3, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode244(x86Operand &operand)
{
	operand.values[23] = OpcodeCodings244_23;
	bool rv;
	{
		rv = Opcode2(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings245_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 178, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode245(x86Operand &operand)
{
	operand.values[23] = OpcodeCodings245_23;
	bool rv;
	{
		rv = Opcode3(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings246_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings246_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode246(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings246_8;
	operand.values[9] = OpcodeCodings246_9;
	bool rv = ParseOperands(tokenBranches2835, operand);
	if (!rv)
	{
		rv = Opcode18(operand);
	}
	return rv;
}
bool x86Parser::Opcode247(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches2837, operand);
	return rv;
}
bool x86Parser::Opcode248(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3103, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings249_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 164, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode249(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings249_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings250_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 165, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode250(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings250_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings251_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 165, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode251(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings251_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings252_23[] = {
	{ Coding::valSpecified, 190},
	{ Coding::eot },
};
bool x86Parser::Opcode252(x86Operand &operand)
{
	operand.values[23] = OpcodeCodings252_23;
	bool rv;
	{
		rv = Opcode15(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings253_23[] = {
	{ Coding::valSpecified, 182},
	{ Coding::eot },
};
bool x86Parser::Opcode253(x86Operand &operand)
{
	operand.values[23] = OpcodeCodings253_23;
	bool rv;
	{
		rv = Opcode15(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings254_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
bool x86Parser::Opcode254(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings254_8;
	bool rv;
	{
		rv = Opcode7(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings255_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
bool x86Parser::Opcode255(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings255_8;
	bool rv;
	{
		rv = Opcode7(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings256_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 144, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode256(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings256_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings257_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
bool x86Parser::Opcode257(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings257_8;
	bool rv;
	{
		rv = Opcode7(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings258_8[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings258_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 12, 8},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings258_24[] = {
	{ Coding::valSpecified, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode258(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings258_8;
	operand.values[23] = OpcodeCodings258_23;
	operand.values[24] = OpcodeCodings258_24;
	bool rv;
	{
		rv = Opcode4(operand);
	}
	return rv;
}
bool x86Parser::Opcode259(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3270, operand);
	return rv;
}
bool x86Parser::Opcode260(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3292, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings261_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 110, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode261(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings261_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings262_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 111, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode262(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings262_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings263_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 111, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode263(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings263_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
bool x86Parser::Opcode264(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3392, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings265_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 97, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode265(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings265_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings266_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 97, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode266(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings266_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings267_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 97, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode267(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings267_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings268_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 157, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode268(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings268_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings269_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 157, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode269(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings269_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings270_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 157, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode270(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings270_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
bool x86Parser::Opcode271(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3408, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings272_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 96, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode272(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings272_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings273_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 96, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode273(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings273_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings274_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 96, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode274(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings274_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings275_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 156, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode275(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings275_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings276_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 156, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode276(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings276_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings277_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 156, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode277(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings277_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings278_8[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
bool x86Parser::Opcode278(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings278_8;
	bool rv;
	{
		rv = Opcode16(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings279_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
bool x86Parser::Opcode279(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings279_8;
	bool rv;
	{
		rv = Opcode16(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings280_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 50, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode280(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings280_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings281_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 51, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode281(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings281_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings282_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 49, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode282(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings282_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
bool x86Parser::Opcode283(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3437, operand);
	return rv;
}
bool x86Parser::Opcode284(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3440, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings285_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
bool x86Parser::Opcode285(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings285_8;
	bool rv;
	{
		rv = Opcode16(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings286_8[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
bool x86Parser::Opcode286(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings286_8;
	bool rv;
	{
		rv = Opcode16(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings287_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 170, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode287(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings287_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings288_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 158, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode288(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings288_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings289_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
bool x86Parser::Opcode289(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings289_8;
	bool rv;
	{
		rv = Opcode16(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings290_8[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
bool x86Parser::Opcode290(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings290_8;
	bool rv;
	{
		rv = Opcode16(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings291_8[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings291_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 28, 8},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings291_24[] = {
	{ Coding::valSpecified, 24},
	{ Coding::eot },
};
bool x86Parser::Opcode291(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings291_8;
	operand.values[23] = OpcodeCodings291_23;
	operand.values[24] = OpcodeCodings291_24;
	bool rv;
	{
		rv = Opcode4(operand);
	}
	return rv;
}
bool x86Parser::Opcode292(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3450, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings293_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 174, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode293(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings293_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings294_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 175, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode294(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings294_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings295_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 175, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode295(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings295_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings296_25[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
bool x86Parser::Opcode296(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings296_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings297_25[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
bool x86Parser::Opcode297(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings297_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings298_25[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
bool x86Parser::Opcode298(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings298_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings299_25[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
bool x86Parser::Opcode299(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings299_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings300_25[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
bool x86Parser::Opcode300(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings300_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings301_25[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
bool x86Parser::Opcode301(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings301_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings302_25[] = {
	{ Coding::valSpecified, 15},
	{ Coding::eot },
};
bool x86Parser::Opcode302(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings302_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings303_25[] = {
	{ Coding::valSpecified, 13},
	{ Coding::eot },
};
bool x86Parser::Opcode303(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings303_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings304_25[] = {
	{ Coding::valSpecified, 12},
	{ Coding::eot },
};
bool x86Parser::Opcode304(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings304_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings305_25[] = {
	{ Coding::valSpecified, 14},
	{ Coding::eot },
};
bool x86Parser::Opcode305(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings305_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings306_25[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
bool x86Parser::Opcode306(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings306_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings307_25[] = {
	{ Coding::valSpecified, 2},
	{ Coding::eot },
};
bool x86Parser::Opcode307(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings307_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings308_25[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
bool x86Parser::Opcode308(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings308_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings309_25[] = {
	{ Coding::valSpecified, 7},
	{ Coding::eot },
};
bool x86Parser::Opcode309(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings309_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings310_25[] = {
	{ Coding::valSpecified, 3},
	{ Coding::eot },
};
bool x86Parser::Opcode310(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings310_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings311_25[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
bool x86Parser::Opcode311(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings311_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings312_25[] = {
	{ Coding::valSpecified, 14},
	{ Coding::eot },
};
bool x86Parser::Opcode312(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings312_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings313_25[] = {
	{ Coding::valSpecified, 12},
	{ Coding::eot },
};
bool x86Parser::Opcode313(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings313_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings314_25[] = {
	{ Coding::valSpecified, 13},
	{ Coding::eot },
};
bool x86Parser::Opcode314(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings314_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings315_25[] = {
	{ Coding::valSpecified, 15},
	{ Coding::eot },
};
bool x86Parser::Opcode315(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings315_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings316_25[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
bool x86Parser::Opcode316(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings316_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings317_25[] = {
	{ Coding::valSpecified, 11},
	{ Coding::eot },
};
bool x86Parser::Opcode317(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings317_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings318_25[] = {
	{ Coding::valSpecified, 9},
	{ Coding::eot },
};
bool x86Parser::Opcode318(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings318_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings319_25[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
bool x86Parser::Opcode319(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings319_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings320_25[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
bool x86Parser::Opcode320(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings320_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings321_25[] = {
	{ Coding::valSpecified, 10},
	{ Coding::eot },
};
bool x86Parser::Opcode321(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings321_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings322_25[] = {
	{ Coding::valSpecified, 10},
	{ Coding::eot },
};
bool x86Parser::Opcode322(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings322_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings323_25[] = {
	{ Coding::valSpecified, 11},
	{ Coding::eot },
};
bool x86Parser::Opcode323(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings323_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings324_25[] = {
	{ Coding::valSpecified, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode324(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings324_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings325_25[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
bool x86Parser::Opcode325(x86Operand &operand)
{
	operand.values[25] = OpcodeCodings325_25;
	bool rv;
	{
		rv = Opcode14(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings326_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
bool x86Parser::Opcode326(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings326_8;
	bool rv;
	{
		rv = Opcode16(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings327_23[] = {
	{ Coding::valSpecified, 164},
	{ Coding::eot },
};
bool x86Parser::Opcode327(x86Operand &operand)
{
	operand.values[23] = OpcodeCodings327_23;
	bool rv;
	{
		rv = Opcode17(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings328_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
bool x86Parser::Opcode328(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings328_8;
	bool rv;
	{
		rv = Opcode16(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings329_23[] = {
	{ Coding::valSpecified, 172},
	{ Coding::eot },
};
bool x86Parser::Opcode329(x86Operand &operand)
{
	operand.values[23] = OpcodeCodings329_23;
	bool rv;
	{
		rv = Opcode17(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings330_8[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings330_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode330(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings330_8;
	operand.values[9] = OpcodeCodings330_9;
	bool rv;
	{
		rv = Opcode18(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings331_8[] = {
	{ Coding::valSpecified, 0},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings331_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode331(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings331_8;
	operand.values[9] = OpcodeCodings331_9;
	bool rv;
	{
		rv = Opcode18(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings332_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings332_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode332(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings332_8;
	operand.values[9] = OpcodeCodings332_9;
	bool rv = ParseOperands(tokenBranches3562, operand);
	if (!rv)
	{
		rv = Opcode18(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings333_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 249, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode333(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings333_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings334_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 253, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode334(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings334_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings335_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 251, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode335(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings335_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
bool x86Parser::Opcode336(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3568, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings337_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 170, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode337(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings337_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings338_9[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 171, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode338(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings338_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings339_9[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 171, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode339(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings339_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings340_8[] = {
	{ Coding::valSpecified, 1},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings340_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode340(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings340_8;
	operand.values[9] = OpcodeCodings340_9;
	bool rv = ParseOperands(tokenBranches3644, operand);
	if (!rv)
	{
		rv = Opcode18(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings341_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings341_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 44, 8},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings341_24[] = {
	{ Coding::valSpecified, 40},
	{ Coding::eot },
};
bool x86Parser::Opcode341(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings341_8;
	operand.values[23] = OpcodeCodings341_23;
	operand.values[24] = OpcodeCodings341_24;
	bool rv;
	{
		rv = Opcode4(operand);
	}
	return rv;
}
bool x86Parser::Opcode342(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3648, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings343_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 11, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode343(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings343_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings344_8[] = {
	{ Coding::valSpecified, 4},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings344_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode344(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings344_8;
	operand.values[9] = OpcodeCodings344_9;
	bool rv = ParseOperands(tokenBranches3707, operand);
	if (!rv)
	{
		rv = Opcode18(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings345_8[] = {
	{ Coding::valSpecified, 5},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings345_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode345(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings345_8;
	operand.values[9] = OpcodeCodings345_9;
	bool rv = ParseOperands(tokenBranches3710, operand);
	if (!rv)
	{
		rv = Opcode18(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings346_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 155, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode346(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings346_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings347_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 9, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode347(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings347_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings348_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 48, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode348(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings348_9;
	bool rv;
	{
		rv = Opcode1(operand);
	}
	return rv;
}
bool x86Parser::Opcode349(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3716, operand);
	return rv;
}
bool x86Parser::Opcode350(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3721, operand);
	return rv;
}
bool x86Parser::Opcode351(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3730, operand);
	return rv;
}
bool x86Parser::Opcode352(x86Operand &operand)
{
	bool rv = ParseOperands(tokenBranches3767, operand);
	return rv;
}
Coding x86Parser::OpcodeCodings353_9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 215, 8},
	{ Coding::eot },
};
bool x86Parser::Opcode353(x86Operand &operand)
{
	operand.values[9] = OpcodeCodings353_9;
	bool rv;
	{
		rv = Opcode0(operand);
	}
	return rv;
}
Coding x86Parser::OpcodeCodings354_8[] = {
	{ Coding::valSpecified, 6},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings354_23[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 52, 8},
	{ Coding::eot },
};
Coding x86Parser::OpcodeCodings354_24[] = {
	{ Coding::valSpecified, 48},
	{ Coding::eot },
};
bool x86Parser::Opcode354(x86Operand &operand)
{
	operand.values[8] = OpcodeCodings354_8;
	operand.values[23] = OpcodeCodings354_23;
	operand.values[24] = OpcodeCodings354_24;
	bool rv;
	{
		rv = Opcode4(operand);
	}
	return rv;
}
x86Parser::DispatchType x86Parser::DispatchTable[355] = {
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
	&x86Parser::Opcode19,
	&x86Parser::Opcode20,
	&x86Parser::Opcode21,
	&x86Parser::Opcode22,
	&x86Parser::Opcode23,
	&x86Parser::Opcode24,
	&x86Parser::Opcode25,
	&x86Parser::Opcode26,
	&x86Parser::Opcode27,
	&x86Parser::Opcode28,
	&x86Parser::Opcode29,
	&x86Parser::Opcode30,
	&x86Parser::Opcode31,
	&x86Parser::Opcode32,
	&x86Parser::Opcode33,
	&x86Parser::Opcode34,
	&x86Parser::Opcode35,
	&x86Parser::Opcode36,
	&x86Parser::Opcode37,
	&x86Parser::Opcode38,
	&x86Parser::Opcode39,
	&x86Parser::Opcode40,
	&x86Parser::Opcode41,
	&x86Parser::Opcode42,
	&x86Parser::Opcode43,
	&x86Parser::Opcode44,
	&x86Parser::Opcode45,
	&x86Parser::Opcode46,
	&x86Parser::Opcode47,
	&x86Parser::Opcode48,
	&x86Parser::Opcode49,
	&x86Parser::Opcode50,
	&x86Parser::Opcode51,
	&x86Parser::Opcode52,
	&x86Parser::Opcode53,
	&x86Parser::Opcode54,
	&x86Parser::Opcode55,
	&x86Parser::Opcode56,
	&x86Parser::Opcode57,
	&x86Parser::Opcode58,
	&x86Parser::Opcode59,
	&x86Parser::Opcode60,
	&x86Parser::Opcode61,
	&x86Parser::Opcode62,
	&x86Parser::Opcode63,
	&x86Parser::Opcode64,
	&x86Parser::Opcode65,
	&x86Parser::Opcode66,
	&x86Parser::Opcode67,
	&x86Parser::Opcode68,
	&x86Parser::Opcode69,
	&x86Parser::Opcode70,
	&x86Parser::Opcode71,
	&x86Parser::Opcode72,
	&x86Parser::Opcode73,
	&x86Parser::Opcode74,
	&x86Parser::Opcode75,
	&x86Parser::Opcode76,
	&x86Parser::Opcode77,
	&x86Parser::Opcode78,
	&x86Parser::Opcode79,
	&x86Parser::Opcode80,
	&x86Parser::Opcode81,
	&x86Parser::Opcode82,
	&x86Parser::Opcode83,
	&x86Parser::Opcode84,
	&x86Parser::Opcode85,
	&x86Parser::Opcode86,
	&x86Parser::Opcode87,
	&x86Parser::Opcode88,
	&x86Parser::Opcode89,
	&x86Parser::Opcode90,
	&x86Parser::Opcode91,
	&x86Parser::Opcode92,
	&x86Parser::Opcode93,
	&x86Parser::Opcode94,
	&x86Parser::Opcode95,
	&x86Parser::Opcode96,
	&x86Parser::Opcode97,
	&x86Parser::Opcode98,
	&x86Parser::Opcode99,
	&x86Parser::Opcode100,
	&x86Parser::Opcode101,
	&x86Parser::Opcode102,
	&x86Parser::Opcode103,
	&x86Parser::Opcode104,
	&x86Parser::Opcode105,
	&x86Parser::Opcode106,
	&x86Parser::Opcode107,
	&x86Parser::Opcode108,
	&x86Parser::Opcode109,
	&x86Parser::Opcode110,
	&x86Parser::Opcode111,
	&x86Parser::Opcode112,
	&x86Parser::Opcode113,
	&x86Parser::Opcode114,
	&x86Parser::Opcode115,
	&x86Parser::Opcode116,
	&x86Parser::Opcode117,
	&x86Parser::Opcode118,
	&x86Parser::Opcode119,
	&x86Parser::Opcode120,
	&x86Parser::Opcode121,
	&x86Parser::Opcode122,
	&x86Parser::Opcode123,
	&x86Parser::Opcode124,
	&x86Parser::Opcode125,
	&x86Parser::Opcode126,
	&x86Parser::Opcode127,
	&x86Parser::Opcode128,
	&x86Parser::Opcode129,
	&x86Parser::Opcode130,
	&x86Parser::Opcode131,
	&x86Parser::Opcode132,
	&x86Parser::Opcode133,
	&x86Parser::Opcode134,
	&x86Parser::Opcode135,
	&x86Parser::Opcode136,
	&x86Parser::Opcode137,
	&x86Parser::Opcode138,
	&x86Parser::Opcode139,
	&x86Parser::Opcode140,
	&x86Parser::Opcode141,
	&x86Parser::Opcode142,
	&x86Parser::Opcode143,
	&x86Parser::Opcode144,
	&x86Parser::Opcode145,
	&x86Parser::Opcode146,
	&x86Parser::Opcode147,
	&x86Parser::Opcode148,
	&x86Parser::Opcode149,
	&x86Parser::Opcode150,
	&x86Parser::Opcode151,
	&x86Parser::Opcode152,
	&x86Parser::Opcode153,
	&x86Parser::Opcode154,
	&x86Parser::Opcode155,
	&x86Parser::Opcode156,
	&x86Parser::Opcode157,
	&x86Parser::Opcode158,
	&x86Parser::Opcode159,
	&x86Parser::Opcode160,
	&x86Parser::Opcode161,
	&x86Parser::Opcode162,
	&x86Parser::Opcode163,
	&x86Parser::Opcode164,
	&x86Parser::Opcode165,
	&x86Parser::Opcode166,
	&x86Parser::Opcode167,
	&x86Parser::Opcode168,
	&x86Parser::Opcode169,
	&x86Parser::Opcode170,
	&x86Parser::Opcode171,
	&x86Parser::Opcode172,
	&x86Parser::Opcode173,
	&x86Parser::Opcode174,
	&x86Parser::Opcode175,
	&x86Parser::Opcode176,
	&x86Parser::Opcode177,
	&x86Parser::Opcode178,
	&x86Parser::Opcode179,
	&x86Parser::Opcode180,
	&x86Parser::Opcode181,
	&x86Parser::Opcode182,
	&x86Parser::Opcode183,
	&x86Parser::Opcode184,
	&x86Parser::Opcode185,
	&x86Parser::Opcode186,
	&x86Parser::Opcode187,
	&x86Parser::Opcode188,
	&x86Parser::Opcode189,
	&x86Parser::Opcode190,
	&x86Parser::Opcode191,
	&x86Parser::Opcode192,
	&x86Parser::Opcode193,
	&x86Parser::Opcode194,
	&x86Parser::Opcode195,
	&x86Parser::Opcode196,
	&x86Parser::Opcode197,
	&x86Parser::Opcode198,
	&x86Parser::Opcode199,
	&x86Parser::Opcode200,
	&x86Parser::Opcode201,
	&x86Parser::Opcode202,
	&x86Parser::Opcode203,
	&x86Parser::Opcode204,
	&x86Parser::Opcode205,
	&x86Parser::Opcode206,
	&x86Parser::Opcode207,
	&x86Parser::Opcode208,
	&x86Parser::Opcode209,
	&x86Parser::Opcode210,
	&x86Parser::Opcode211,
	&x86Parser::Opcode212,
	&x86Parser::Opcode213,
	&x86Parser::Opcode214,
	&x86Parser::Opcode215,
	&x86Parser::Opcode216,
	&x86Parser::Opcode217,
	&x86Parser::Opcode218,
	&x86Parser::Opcode219,
	&x86Parser::Opcode220,
	&x86Parser::Opcode221,
	&x86Parser::Opcode222,
	&x86Parser::Opcode223,
	&x86Parser::Opcode224,
	&x86Parser::Opcode225,
	&x86Parser::Opcode226,
	&x86Parser::Opcode227,
	&x86Parser::Opcode228,
	&x86Parser::Opcode229,
	&x86Parser::Opcode230,
	&x86Parser::Opcode231,
	&x86Parser::Opcode232,
	&x86Parser::Opcode233,
	&x86Parser::Opcode234,
	&x86Parser::Opcode235,
	&x86Parser::Opcode236,
	&x86Parser::Opcode237,
	&x86Parser::Opcode238,
	&x86Parser::Opcode239,
	&x86Parser::Opcode240,
	&x86Parser::Opcode241,
	&x86Parser::Opcode242,
	&x86Parser::Opcode243,
	&x86Parser::Opcode244,
	&x86Parser::Opcode245,
	&x86Parser::Opcode246,
	&x86Parser::Opcode247,
	&x86Parser::Opcode248,
	&x86Parser::Opcode249,
	&x86Parser::Opcode250,
	&x86Parser::Opcode251,
	&x86Parser::Opcode252,
	&x86Parser::Opcode253,
	&x86Parser::Opcode254,
	&x86Parser::Opcode255,
	&x86Parser::Opcode256,
	&x86Parser::Opcode257,
	&x86Parser::Opcode258,
	&x86Parser::Opcode259,
	&x86Parser::Opcode260,
	&x86Parser::Opcode261,
	&x86Parser::Opcode262,
	&x86Parser::Opcode263,
	&x86Parser::Opcode264,
	&x86Parser::Opcode265,
	&x86Parser::Opcode266,
	&x86Parser::Opcode267,
	&x86Parser::Opcode268,
	&x86Parser::Opcode269,
	&x86Parser::Opcode270,
	&x86Parser::Opcode271,
	&x86Parser::Opcode272,
	&x86Parser::Opcode273,
	&x86Parser::Opcode274,
	&x86Parser::Opcode275,
	&x86Parser::Opcode276,
	&x86Parser::Opcode277,
	&x86Parser::Opcode278,
	&x86Parser::Opcode279,
	&x86Parser::Opcode280,
	&x86Parser::Opcode281,
	&x86Parser::Opcode282,
	&x86Parser::Opcode283,
	&x86Parser::Opcode284,
	&x86Parser::Opcode285,
	&x86Parser::Opcode286,
	&x86Parser::Opcode287,
	&x86Parser::Opcode288,
	&x86Parser::Opcode289,
	&x86Parser::Opcode290,
	&x86Parser::Opcode291,
	&x86Parser::Opcode292,
	&x86Parser::Opcode293,
	&x86Parser::Opcode294,
	&x86Parser::Opcode295,
	&x86Parser::Opcode296,
	&x86Parser::Opcode297,
	&x86Parser::Opcode298,
	&x86Parser::Opcode299,
	&x86Parser::Opcode300,
	&x86Parser::Opcode301,
	&x86Parser::Opcode302,
	&x86Parser::Opcode303,
	&x86Parser::Opcode304,
	&x86Parser::Opcode305,
	&x86Parser::Opcode306,
	&x86Parser::Opcode307,
	&x86Parser::Opcode308,
	&x86Parser::Opcode309,
	&x86Parser::Opcode310,
	&x86Parser::Opcode311,
	&x86Parser::Opcode312,
	&x86Parser::Opcode313,
	&x86Parser::Opcode314,
	&x86Parser::Opcode315,
	&x86Parser::Opcode316,
	&x86Parser::Opcode317,
	&x86Parser::Opcode318,
	&x86Parser::Opcode319,
	&x86Parser::Opcode320,
	&x86Parser::Opcode321,
	&x86Parser::Opcode322,
	&x86Parser::Opcode323,
	&x86Parser::Opcode324,
	&x86Parser::Opcode325,
	&x86Parser::Opcode326,
	&x86Parser::Opcode327,
	&x86Parser::Opcode328,
	&x86Parser::Opcode329,
	&x86Parser::Opcode330,
	&x86Parser::Opcode331,
	&x86Parser::Opcode332,
	&x86Parser::Opcode333,
	&x86Parser::Opcode334,
	&x86Parser::Opcode335,
	&x86Parser::Opcode336,
	&x86Parser::Opcode337,
	&x86Parser::Opcode338,
	&x86Parser::Opcode339,
	&x86Parser::Opcode340,
	&x86Parser::Opcode341,
	&x86Parser::Opcode342,
	&x86Parser::Opcode343,
	&x86Parser::Opcode344,
	&x86Parser::Opcode345,
	&x86Parser::Opcode346,
	&x86Parser::Opcode347,
	&x86Parser::Opcode348,
	&x86Parser::Opcode349,
	&x86Parser::Opcode350,
	&x86Parser::Opcode351,
	&x86Parser::Opcode352,
	&x86Parser::Opcode353,
	&x86Parser::Opcode354,
};
Coding x86Parser::Coding1[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding2[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding3[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding4[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding5[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ Coding::eot },
};
Coding x86Parser::Coding6[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ Coding::eot },
};
Coding x86Parser::Coding7[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ Coding::eot },
};
Coding x86Parser::Coding8[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ Coding::eot },
};
Coding x86Parser::Coding9[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ Coding::eot },
};
Coding x86Parser::Coding10[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ Coding::eot },
};
Coding x86Parser::Coding11[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 2, 43, 4 },
	{ Coding::eot },
};
Coding x86Parser::Coding12[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 2, 43, 4 },
	{ Coding::eot },
};
Coding x86Parser::Coding13[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 2, 43, 4 },
	{ Coding::eot },
};
Coding x86Parser::Coding14[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 7, 3},
	{ Coding::eot },
};
Coding x86Parser::Coding15[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 7, 3},
	{ Coding::eot },
};
Coding x86Parser::Coding16[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 7, 3},
	{ Coding::eot },
};
Coding x86Parser::Coding17[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding18[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding19[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding20[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding21[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding22[] = {
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
Coding x86Parser::Coding23[] = {
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
Coding x86Parser::Coding24[] = {
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
Coding x86Parser::Coding25[] = {
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
Coding x86Parser::Coding26[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 2, 43, 4 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding27[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 2, 43, 4 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding28[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 2, 43, 4 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding29[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 7, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding30[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::valSpecified, 54},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 7, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding31[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 7, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding32[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding33[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::valSpecified, 62},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding34[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding35[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding36[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 1, 2 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding37[] = {
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
Coding x86Parser::Coding38[] = {
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
Coding x86Parser::Coding39[] = {
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
Coding x86Parser::Coding40[] = {
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
Coding x86Parser::Coding41[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 2, 43, 4 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding42[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 2, 43, 4 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding43[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 2, 43, 4 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding44[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 7, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding45[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 7, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding46[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 7, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding47[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding48[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding49[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding50[] = {
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
Coding x86Parser::Coding51[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding52[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding53[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding54[] = {
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
Coding x86Parser::Coding55[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding56[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 3, 3},
	{ Coding::eot },
};
Coding x86Parser::Coding57[] = {
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
Coding x86Parser::Coding58[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding59[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding60[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding61[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding62[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding63[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding64[] = {
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
Coding x86Parser::Coding65[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding66[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding67[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding68[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 3, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding69[] = {
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
Coding x86Parser::Coding70[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding71[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 3, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding72[] = {
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
Coding x86Parser::Coding73[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding74[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 4, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding75[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding76[] = {
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
Coding x86Parser::Coding77[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding78[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding79[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 54, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding80[] = {
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
Coding x86Parser::Coding81[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding82[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding83[] = {
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
Coding x86Parser::Coding84[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 2, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 5, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 0, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding85[] = {
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
Coding x86Parser::Coding86[] = {
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
Coding x86Parser::Coding87[] = {
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
Coding x86Parser::Coding88[] = {
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
Coding x86Parser::Coding89[] = {
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
Coding x86Parser::Coding90[] = {
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
Coding x86Parser::Coding91[] = {
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
Coding x86Parser::Coding92[] = {
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
Coding x86Parser::Coding93[] = {
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
Coding x86Parser::Coding94[] = {
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
Coding x86Parser::Coding95[] = {
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
Coding x86Parser::Coding96[] = {
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
Coding x86Parser::Coding97[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ Coding::eot },
};
Coding x86Parser::Coding98[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ Coding::eot },
};
Coding x86Parser::Coding99[] = {
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
Coding x86Parser::Coding100[] = {
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
Coding x86Parser::Coding101[] = {
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
Coding x86Parser::Coding102[] = {
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
Coding x86Parser::Coding103[] = {
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
Coding x86Parser::Coding104[] = {
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
Coding x86Parser::Coding105[] = {
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
Coding x86Parser::Coding106[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 62, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ Coding::eot },
};
Coding x86Parser::Coding107[] = {
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
Coding x86Parser::Coding108[] = {
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
Coding x86Parser::Coding109[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 6, 2, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ Coding::eot },
};
Coding x86Parser::Coding110[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 4, 3},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 2, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 5, 3},
	{ Coding::eot },
};
Coding x86Parser::Coding111[] = {
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
Coding x86Parser::Coding112[] = {
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
Coding x86Parser::Coding113[] = {
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
Coding x86Parser::Coding114[] = {
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
Coding x86Parser::Coding115[] = {
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
Coding x86Parser::Coding116[] = {
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
Coding x86Parser::Coding117[] = {
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
Coding x86Parser::Coding118[] = {
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
Coding x86Parser::Coding119[] = {
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
Coding x86Parser::Coding120[] = {
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
Coding x86Parser::Coding121[] = {
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
Coding x86Parser::Coding122[] = {
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
Coding x86Parser::Coding123[] = {
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
Coding x86Parser::Coding124[] = {
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
Coding x86Parser::Coding125[] = {
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
Coding x86Parser::Coding126[] = {
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
Coding x86Parser::Coding127[] = {
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
Coding x86Parser::Coding128[] = {
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
Coding x86Parser::Coding129[] = {
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
Coding x86Parser::Coding130[] = {
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
Coding x86Parser::Coding131[] = {
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
Coding x86Parser::Coding132[] = {
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
Coding x86Parser::Coding133[] = {
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
Coding x86Parser::Coding134[] = {
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
Coding x86Parser::Coding135[] = {
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
Coding x86Parser::Coding136[] = {
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
Coding x86Parser::Coding137[] = {
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
Coding x86Parser::Coding138[] = {
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
Coding x86Parser::Coding139[] = {
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
Coding x86Parser::Coding140[] = {
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
Coding x86Parser::Coding141[] = {
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
Coding x86Parser::Coding142[] = {
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
Coding x86Parser::Coding143[] = {
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
Coding x86Parser::Coding144[] = {
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
Coding x86Parser::Coding145[] = {
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
Coding x86Parser::Coding146[] = {
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
Coding x86Parser::Coding147[] = {
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
Coding x86Parser::Coding148[] = {
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
Coding x86Parser::Coding149[] = {
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
Coding x86Parser::Coding150[] = {
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
Coding x86Parser::Coding151[] = {
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 3, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 7, 3, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding152[] = {
	{ Coding::indirect, 9, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding153[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ Coding::indirect, 9, -1, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding154[] = {
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding155[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0, 43, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding156[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 23, 8, 0, 43, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding157[] = {
	{ Coding::native },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding158[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::native },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 13, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding159[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::native },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding160[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::native },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 13, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding161[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::native },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding162[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::native },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding163[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::native },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding164[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 9, 5, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding165[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 9, 5, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding166[] = {
	{ Coding::indirect, 23, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 3, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 26, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 7, 3, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding167[] = {
	{ Coding::indirect, 23, -1, 0, 43, 4 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 3, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 7, 3, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding168[] = {
	{ Coding::indirect, 23, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 3, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 7, 3, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding169[] = {
	{ Coding::indirect, 9, -1, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 3, 2},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 8, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 1, 3},
	{ Coding::eot },
};
Coding x86Parser::Coding170[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 7, 4},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 25, 4, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 14, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding171[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::valSpecified, 15},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 8, 4},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 25, 4, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 14, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding172[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::valSpecified, 15},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 8, 4},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 25, 4, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 14, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding173[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 25, 5},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding174[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 154, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 1, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding175[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 154, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 1, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding176[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 232, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 14, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding177[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 232, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 14, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding178[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 154, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 16},
	{ Coding::eot },
};
Coding x86Parser::Coding179[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 154, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 16},
	{ Coding::eot },
};
Coding x86Parser::Coding180[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 166, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding181[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 166, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding182[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 166, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding183[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 166, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding184[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 167, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding185[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 167, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding186[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 167, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding187[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 167, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding188[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 167, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding189[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 167, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding190[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 167, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding191[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 167, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding192[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 200, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 16, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding193[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 155, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 223, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 224, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding194[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 223, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 224, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding195[] = {
	{ Coding::native },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 17, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding196[] = {
	{ Coding::native },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding197[] = {
	{ Coding::native },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding198[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 107, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 3, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 13, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding199[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 105, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 3, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding200[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 107, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 3, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 13, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding201[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 105, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 3, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding202[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 228, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 18, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding203[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 229, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 18, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding204[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 229, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 18, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding205[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 236, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding206[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 237, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding207[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 237, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding208[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 108, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding209[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 108, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding210[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 108, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding211[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 108, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding212[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 109, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding213[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 109, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding214[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 109, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding215[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 109, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding216[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 109, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding217[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 109, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding218[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 109, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding219[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 109, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding220[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 205, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding221[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 227, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 14, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding222[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 227, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 14, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding223[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 234, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 1, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding224[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 234, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 1, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding225[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 234, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 16},
	{ Coding::eot },
};
Coding x86Parser::Coding226[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 234, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 16},
	{ Coding::eot },
};
Coding x86Parser::Coding227[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 235, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 14, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding228[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 233, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 14, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding229[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 233, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 14, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding230[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 172, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding231[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 172, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding232[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 172, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding233[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 172, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding234[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 173, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding235[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 173, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding236[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 173, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding237[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 173, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding238[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 173, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding239[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 173, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding240[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 173, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding241[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 173, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding242[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 226, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 14, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding243[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 225, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 14, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding244[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 224, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 14, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding245[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 160, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding246[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 160, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding247[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 160, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding248[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 160, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding249[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 161, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding250[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 161, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding251[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 161, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding252[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 161, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding253[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 161, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding254[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 161, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding255[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 161, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding256[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 161, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding257[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 22, 5},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding258[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 23, 5},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding259[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 23, 5},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding260[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 162, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding261[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 162, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding262[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 162, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding263[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 162, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding264[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 163, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding265[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 163, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding266[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 163, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding267[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 163, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding268[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 163, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding269[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 163, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding270[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 163, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding271[] = {
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 163, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding272[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 164, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding273[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 164, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding274[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 164, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding275[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 164, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding276[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 165, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding277[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 165, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding278[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 165, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding279[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 165, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding280[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 165, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding281[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 165, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding282[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 165, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding283[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 165, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding284[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 230, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 18, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding285[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 231, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 18, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding286[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 231, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 18, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding287[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 238, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding288[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 239, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding289[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 239, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding290[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 110, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding291[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 110, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding292[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 110, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding293[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 110, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding294[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 111, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding295[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 111, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding296[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 111, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding297[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 111, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding298[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 111, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding299[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 111, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding300[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 111, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding301[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 111, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding302[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 11, 5},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding303[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 11, 5},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding304[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 161, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding305[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 169, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding306[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 7, 3},
	{ Coding::eot },
};
Coding x86Parser::Coding307[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 10, 5},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding308[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 10, 5},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding309[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 106, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding310[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 104, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding311[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 104, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding312[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::native },
	{ Coding::eot },
};
Coding x86Parser::Coding313[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::native },
	{ Coding::eot },
};
Coding x86Parser::Coding314[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 160, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding315[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 15, 8},
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 168, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding316[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 0, 2},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 3, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 6, 3},
	{ Coding::eot },
};
Coding x86Parser::Coding317[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 194, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding318[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 195, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding319[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 202, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding320[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 203, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding321[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 174, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding322[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 174, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding323[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 174, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding324[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 174, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding325[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 175, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding326[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 175, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding327[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 175, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding328[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 175, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding329[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 175, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding330[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 175, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding331[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 175, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding332[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 175, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding333[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 170, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding334[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 170, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding335[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 170, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding336[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 170, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding337[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 171, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding338[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 171, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding339[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 171, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding340[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 171, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding341[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 171, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding342[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 171, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding343[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 171, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding344[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 1, 8, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 171, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding345[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 168, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 8, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding346[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 169, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 16, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding347[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 169, 8},
	{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), 12, 32, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding348[] = {
	{ Coding::stateFunc, 2 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 18, 5},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding349[] = {
	{ Coding::stateFunc, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 18, 5},
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), 10, 3, 0 },
	{ Coding::eot },
};
Coding x86Parser::Coding350[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 215, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding351[] = {
	{ Coding::stateFunc, 0 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 1, -1, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 215, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding352[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 215, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding353[] = {
	{ Coding::stateFunc, 1 },
	{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), 1, -1, 3 },
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 215, 8},
	{ Coding::eot },
};
Coding x86Parser::Coding354[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 215, 8},
	{ Coding::eot },
};
Coding * x86Parser::Codings[354] = {
	x86Parser::Coding1,
	x86Parser::Coding2,
	x86Parser::Coding3,
	x86Parser::Coding4,
	x86Parser::Coding5,
	x86Parser::Coding6,
	x86Parser::Coding7,
	x86Parser::Coding8,
	x86Parser::Coding9,
	x86Parser::Coding10,
	x86Parser::Coding11,
	x86Parser::Coding12,
	x86Parser::Coding13,
	x86Parser::Coding14,
	x86Parser::Coding15,
	x86Parser::Coding16,
	x86Parser::Coding17,
	x86Parser::Coding18,
	x86Parser::Coding19,
	x86Parser::Coding20,
	x86Parser::Coding21,
	x86Parser::Coding22,
	x86Parser::Coding23,
	x86Parser::Coding24,
	x86Parser::Coding25,
	x86Parser::Coding26,
	x86Parser::Coding27,
	x86Parser::Coding28,
	x86Parser::Coding29,
	x86Parser::Coding30,
	x86Parser::Coding31,
	x86Parser::Coding32,
	x86Parser::Coding33,
	x86Parser::Coding34,
	x86Parser::Coding35,
	x86Parser::Coding36,
	x86Parser::Coding37,
	x86Parser::Coding38,
	x86Parser::Coding39,
	x86Parser::Coding40,
	x86Parser::Coding41,
	x86Parser::Coding42,
	x86Parser::Coding43,
	x86Parser::Coding44,
	x86Parser::Coding45,
	x86Parser::Coding46,
	x86Parser::Coding47,
	x86Parser::Coding48,
	x86Parser::Coding49,
	x86Parser::Coding50,
	x86Parser::Coding51,
	x86Parser::Coding52,
	x86Parser::Coding53,
	x86Parser::Coding54,
	x86Parser::Coding55,
	x86Parser::Coding56,
	x86Parser::Coding57,
	x86Parser::Coding58,
	x86Parser::Coding59,
	x86Parser::Coding60,
	x86Parser::Coding61,
	x86Parser::Coding62,
	x86Parser::Coding63,
	x86Parser::Coding64,
	x86Parser::Coding65,
	x86Parser::Coding66,
	x86Parser::Coding67,
	x86Parser::Coding68,
	x86Parser::Coding69,
	x86Parser::Coding70,
	x86Parser::Coding71,
	x86Parser::Coding72,
	x86Parser::Coding73,
	x86Parser::Coding74,
	x86Parser::Coding75,
	x86Parser::Coding76,
	x86Parser::Coding77,
	x86Parser::Coding78,
	x86Parser::Coding79,
	x86Parser::Coding80,
	x86Parser::Coding81,
	x86Parser::Coding82,
	x86Parser::Coding83,
	x86Parser::Coding84,
	x86Parser::Coding85,
	x86Parser::Coding86,
	x86Parser::Coding87,
	x86Parser::Coding88,
	x86Parser::Coding89,
	x86Parser::Coding90,
	x86Parser::Coding91,
	x86Parser::Coding92,
	x86Parser::Coding93,
	x86Parser::Coding94,
	x86Parser::Coding95,
	x86Parser::Coding96,
	x86Parser::Coding97,
	x86Parser::Coding98,
	x86Parser::Coding99,
	x86Parser::Coding100,
	x86Parser::Coding101,
	x86Parser::Coding102,
	x86Parser::Coding103,
	x86Parser::Coding104,
	x86Parser::Coding105,
	x86Parser::Coding106,
	x86Parser::Coding107,
	x86Parser::Coding108,
	x86Parser::Coding109,
	x86Parser::Coding110,
	x86Parser::Coding111,
	x86Parser::Coding112,
	x86Parser::Coding113,
	x86Parser::Coding114,
	x86Parser::Coding115,
	x86Parser::Coding116,
	x86Parser::Coding117,
	x86Parser::Coding118,
	x86Parser::Coding119,
	x86Parser::Coding120,
	x86Parser::Coding121,
	x86Parser::Coding122,
	x86Parser::Coding123,
	x86Parser::Coding124,
	x86Parser::Coding125,
	x86Parser::Coding126,
	x86Parser::Coding127,
	x86Parser::Coding128,
	x86Parser::Coding129,
	x86Parser::Coding130,
	x86Parser::Coding131,
	x86Parser::Coding132,
	x86Parser::Coding133,
	x86Parser::Coding134,
	x86Parser::Coding135,
	x86Parser::Coding136,
	x86Parser::Coding137,
	x86Parser::Coding138,
	x86Parser::Coding139,
	x86Parser::Coding140,
	x86Parser::Coding141,
	x86Parser::Coding142,
	x86Parser::Coding143,
	x86Parser::Coding144,
	x86Parser::Coding145,
	x86Parser::Coding146,
	x86Parser::Coding147,
	x86Parser::Coding148,
	x86Parser::Coding149,
	x86Parser::Coding150,
	x86Parser::Coding151,
	x86Parser::Coding152,
	x86Parser::Coding153,
	x86Parser::Coding154,
	x86Parser::Coding155,
	x86Parser::Coding156,
	x86Parser::Coding157,
	x86Parser::Coding158,
	x86Parser::Coding159,
	x86Parser::Coding160,
	x86Parser::Coding161,
	x86Parser::Coding162,
	x86Parser::Coding163,
	x86Parser::Coding164,
	x86Parser::Coding165,
	x86Parser::Coding166,
	x86Parser::Coding167,
	x86Parser::Coding168,
	x86Parser::Coding169,
	x86Parser::Coding170,
	x86Parser::Coding171,
	x86Parser::Coding172,
	x86Parser::Coding173,
	x86Parser::Coding174,
	x86Parser::Coding175,
	x86Parser::Coding176,
	x86Parser::Coding177,
	x86Parser::Coding178,
	x86Parser::Coding179,
	x86Parser::Coding180,
	x86Parser::Coding181,
	x86Parser::Coding182,
	x86Parser::Coding183,
	x86Parser::Coding184,
	x86Parser::Coding185,
	x86Parser::Coding186,
	x86Parser::Coding187,
	x86Parser::Coding188,
	x86Parser::Coding189,
	x86Parser::Coding190,
	x86Parser::Coding191,
	x86Parser::Coding192,
	x86Parser::Coding193,
	x86Parser::Coding194,
	x86Parser::Coding195,
	x86Parser::Coding196,
	x86Parser::Coding197,
	x86Parser::Coding198,
	x86Parser::Coding199,
	x86Parser::Coding200,
	x86Parser::Coding201,
	x86Parser::Coding202,
	x86Parser::Coding203,
	x86Parser::Coding204,
	x86Parser::Coding205,
	x86Parser::Coding206,
	x86Parser::Coding207,
	x86Parser::Coding208,
	x86Parser::Coding209,
	x86Parser::Coding210,
	x86Parser::Coding211,
	x86Parser::Coding212,
	x86Parser::Coding213,
	x86Parser::Coding214,
	x86Parser::Coding215,
	x86Parser::Coding216,
	x86Parser::Coding217,
	x86Parser::Coding218,
	x86Parser::Coding219,
	x86Parser::Coding220,
	x86Parser::Coding221,
	x86Parser::Coding222,
	x86Parser::Coding223,
	x86Parser::Coding224,
	x86Parser::Coding225,
	x86Parser::Coding226,
	x86Parser::Coding227,
	x86Parser::Coding228,
	x86Parser::Coding229,
	x86Parser::Coding230,
	x86Parser::Coding231,
	x86Parser::Coding232,
	x86Parser::Coding233,
	x86Parser::Coding234,
	x86Parser::Coding235,
	x86Parser::Coding236,
	x86Parser::Coding237,
	x86Parser::Coding238,
	x86Parser::Coding239,
	x86Parser::Coding240,
	x86Parser::Coding241,
	x86Parser::Coding242,
	x86Parser::Coding243,
	x86Parser::Coding244,
	x86Parser::Coding245,
	x86Parser::Coding246,
	x86Parser::Coding247,
	x86Parser::Coding248,
	x86Parser::Coding249,
	x86Parser::Coding250,
	x86Parser::Coding251,
	x86Parser::Coding252,
	x86Parser::Coding253,
	x86Parser::Coding254,
	x86Parser::Coding255,
	x86Parser::Coding256,
	x86Parser::Coding257,
	x86Parser::Coding258,
	x86Parser::Coding259,
	x86Parser::Coding260,
	x86Parser::Coding261,
	x86Parser::Coding262,
	x86Parser::Coding263,
	x86Parser::Coding264,
	x86Parser::Coding265,
	x86Parser::Coding266,
	x86Parser::Coding267,
	x86Parser::Coding268,
	x86Parser::Coding269,
	x86Parser::Coding270,
	x86Parser::Coding271,
	x86Parser::Coding272,
	x86Parser::Coding273,
	x86Parser::Coding274,
	x86Parser::Coding275,
	x86Parser::Coding276,
	x86Parser::Coding277,
	x86Parser::Coding278,
	x86Parser::Coding279,
	x86Parser::Coding280,
	x86Parser::Coding281,
	x86Parser::Coding282,
	x86Parser::Coding283,
	x86Parser::Coding284,
	x86Parser::Coding285,
	x86Parser::Coding286,
	x86Parser::Coding287,
	x86Parser::Coding288,
	x86Parser::Coding289,
	x86Parser::Coding290,
	x86Parser::Coding291,
	x86Parser::Coding292,
	x86Parser::Coding293,
	x86Parser::Coding294,
	x86Parser::Coding295,
	x86Parser::Coding296,
	x86Parser::Coding297,
	x86Parser::Coding298,
	x86Parser::Coding299,
	x86Parser::Coding300,
	x86Parser::Coding301,
	x86Parser::Coding302,
	x86Parser::Coding303,
	x86Parser::Coding304,
	x86Parser::Coding305,
	x86Parser::Coding306,
	x86Parser::Coding307,
	x86Parser::Coding308,
	x86Parser::Coding309,
	x86Parser::Coding310,
	x86Parser::Coding311,
	x86Parser::Coding312,
	x86Parser::Coding313,
	x86Parser::Coding314,
	x86Parser::Coding315,
	x86Parser::Coding316,
	x86Parser::Coding317,
	x86Parser::Coding318,
	x86Parser::Coding319,
	x86Parser::Coding320,
	x86Parser::Coding321,
	x86Parser::Coding322,
	x86Parser::Coding323,
	x86Parser::Coding324,
	x86Parser::Coding325,
	x86Parser::Coding326,
	x86Parser::Coding327,
	x86Parser::Coding328,
	x86Parser::Coding329,
	x86Parser::Coding330,
	x86Parser::Coding331,
	x86Parser::Coding332,
	x86Parser::Coding333,
	x86Parser::Coding334,
	x86Parser::Coding335,
	x86Parser::Coding336,
	x86Parser::Coding337,
	x86Parser::Coding338,
	x86Parser::Coding339,
	x86Parser::Coding340,
	x86Parser::Coding341,
	x86Parser::Coding342,
	x86Parser::Coding343,
	x86Parser::Coding344,
	x86Parser::Coding345,
	x86Parser::Coding346,
	x86Parser::Coding347,
	x86Parser::Coding348,
	x86Parser::Coding349,
	x86Parser::Coding350,
	x86Parser::Coding351,
	x86Parser::Coding352,
	x86Parser::Coding353,
	x86Parser::Coding354,
};
Coding x86Parser::prefixCoding1[] = {
	{ Coding::stateFunc, 0 },
	{ Coding::eot },
};
Coding x86Parser::prefixCoding2[] = {
	{ Coding::stateFunc, 1 },
	{ Coding::eot },
};
Coding x86Parser::prefixCoding3[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 240, 8},
	{ Coding::eot },
};
Coding x86Parser::prefixCoding4[] = {
	{ Coding::stateFunc, 2 },
	{ Coding::eot },
};
Coding x86Parser::prefixCoding5[] = {
	{ Coding::stateFunc, 3 },
	{ Coding::eot },
};
Coding x86Parser::prefixCoding6[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 243, 8},
	{ Coding::eot },
};
Coding x86Parser::prefixCoding7[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 243, 8},
	{ Coding::eot },
};
Coding x86Parser::prefixCoding8[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 242, 8},
	{ Coding::eot },
};
Coding x86Parser::prefixCoding9[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 242, 8},
	{ Coding::eot },
};
Coding x86Parser::prefixCoding10[] = {
	{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), 243, 8},
	{ Coding::eot },
};
Coding *x86Parser::prefixCodings[] = {
	x86Parser::prefixCoding1,
	x86Parser::prefixCoding2,
	x86Parser::prefixCoding3,
	x86Parser::prefixCoding4,
	x86Parser::prefixCoding5,
	x86Parser::prefixCoding6,
	x86Parser::prefixCoding7,
	x86Parser::prefixCoding8,
	x86Parser::prefixCoding9,
	x86Parser::prefixCoding10,
};
bool x86Parser::MatchesToken(int token, int tokenPos)
{
	return tokenPos < inputTokens.size() && inputTokens[tokenPos]->type == InputToken::TOKEN && inputTokens[tokenPos]->val->ival == token;
}
	bool x86Parser::MatchesRegister(int reg, int tokenPos)
{
	return tokenPos < inputTokens.size() && inputTokens[tokenPos]->type == InputToken::REGISTER && inputTokens[tokenPos]->val->ival == reg;
}
	bool x86Parser::MatchesRegisterClass(int cclass, int tokenPos)
{
	return tokenPos < inputTokens.size() && inputTokens[tokenPos]->type == InputToken::REGISTER && (registerDataIndirect[cclass][inputTokens[tokenPos]->val->ival >> 3] & (1 << (inputTokens[tokenPos]->val->ival & 7)));
}
bool x86Parser::ParseAddresses(x86Operand &operand, int addrClass, int &tokenPos)
{
	int level = 0;
	bool rv = false;
	x86Token *t = tokenBranches1;
	while (t->type != x86Token::EOT)
	{
		bool matches = false;
		if (t->addrClass[addrClass >> 3] & (1 << (addrClass & 7)))
		{
			switch (t->type)
			{
				case x86Token::EMPTY:
					matches = true;
					break;
				case x86Token::TOKEN:
					matches = MatchesToken(t->id, tokenPos);
					break;
				case x86Token::REGISTER:
					matches = MatchesRegister(t->id, tokenPos);
					break;
				case x86Token::REGISTERCLASS:
					matches = MatchesRegisterClass(t->id, tokenPos);
					break;
				case x86Token::NUMBER:
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

bool x86Parser::ParseOperands2(x86Token *tokenList, x86Operand &operand, int tokenPos, int level)
{
	bool rv = false;
	x86Token *t = tokenList;
	while (t && t->type != x86Token::EOT)
	{
		bool matches = false;
		int last = tokenPos;
		switch (t->type)
		{
			case x86Token::EMPTY:
				matches = true;
				break;
			case x86Token::TOKEN:
				matches = MatchesToken(t->id, tokenPos);
				break;
			case x86Token::REGISTER:
				matches = MatchesRegister(t->id, tokenPos);
				break;
			case x86Token::REGISTERCLASS:
				matches = MatchesRegisterClass(t->id,tokenPos);
				break;
			case x86Token::ADDRESSCLASS:
				matches = ParseAddresses(operand, t->id, tokenPos);
				break;
			case x86Token::NUMBER:
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

bool x86Parser::ParseOperands(x86Token *tokenList, x86Operand &operand)
{
	return ParseOperands2(tokenList, operand, 0, 0);
}

bool x86Parser::ProcessCoding(CodingHelper &base, x86Operand &operand, Coding *coding)
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
			std::deque<Numeric *>::iterator it = operands.begin();
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

bool x86Parser::DispatchOpcode(int opcode)
{
	bool rv;
	if (opcode == -1)
	{
		rv = true;
		x86Operand operand;
		CodingHelper base;
		for (std::deque<int>::iterator it = prefixes.begin(); rv && it !=prefixes.end(); ++it)
			rv &= ProcessCoding(base, operand, prefixCodings[*it]);
	}
	else
	{
		x86Operand operand;
		operand.opcode = opcode;
		rv = (this->*DispatchTable[opcode])(operand);
		if (rv)
		{
			CodingHelper base;
			for (std::deque<int>::iterator it = prefixes.begin(); rv && it !=prefixes.end(); ++it)
				rv &= ProcessCoding(base, operand, prefixCodings[*it]);
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

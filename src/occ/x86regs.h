/* Software License Agreement
 *
 *     Copyright(C) 1994-2026 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 *
 */

#define REG_MAX 64

#define R_EAX 0
#define R_ECX 1
#define R_EDX 2
#define R_EAXEDX 8
#define R_EDXECX 10
#define R_AL 16
#define R_CL 17
#define R_AX 24
#define R_CX 25
static int regmap[REG_MAX][2] = {
    {0, -1}, {1, -1}, {2, -1}, {3, -1}, {4, -1}, {5, -1}, {6, -1}, {7, -1},

    {0, 2},  {0, 1},  {1, 2},  {6, 3},  {7, 3},  {6, 7},  {3, 5},  {7, 5},

    {0, -1}, {1, -1}, {2, -1}, {3, -1}, {4, -1}, {5, -1}, {6, -1}, {7, -1},

    {0, -1}, {1, -1}, {2, -1}, {3, -1}, {4, -1}, {5, -1}, {6, -1}, {7, -1},

    {0, -1}, {1, -1}, {2, -1}, {3, -1}, {4, -1}, {5, -1}, {6, -1}, {7, -1},

    {0, 1},  {2, 3},  {4, 5},  {6, 7},  {0, 4},  {1, 5},  {2, 6},  {3, 7},

    {0, -1}, {1, -1}, {2, -1}, {3, -1}, {4, -1}, {5, -1}, {6, -1}, {7, -1},

    {0, 1},  {2, 3},  {4, 5},  {6, 7},  {0, 4},  {1, 5},  {2, 6},  {3, 7},

};
static Optimizer::ARCH_REGDESC regNames[] = {
    {"EAX", 0, {0, -1}, {8, 9, 16, 20, 24}},
    {"ECX", 0, {1, -1}, {9, 10, 17, 21, 25}},
    {"EDX", 0, {2, -1}, {8, 10, 18, 22, 26}},
    {"EBX", 8, {3, -1}, {11, 12, 14, 19, 23, 27}},
    {""},
    {"EBP", 32, {5, -1}, {14, 15, 29}},
    {"ESI", 64, {6, -1}, {11, 13, 30}},
    {"EDI", 128, {7, -1}, {12, 13, 15, 31}},
    {"EDX:EAX", 0, {2, 0}, {0, 2, 9, 10, 16, 18, 20, 22, 24, 26}},
    {"ECX:EAX", 0, {1, 0}, {0, 1, 8, 10, 16, 17, 20, 21, 24, 25}},
    {"EDX:ECX", 0, {2, 1}, {1, 2, 8, 9, 17, 18, 21, 22, 25, 26}},
    {"EBX:ESI", 72, {3, 6}, {3, 6, 12, 13, 14, 19, 23, 27, 30}},
    {"EBX:EDI", 136, {3, 7}, {3, 7, 11, 13, 14, 15, 19, 23, 27, 31}},
    {"EDI:ESI", 192, {7, 6}, {6, 7, 11, 12, 15, 30, 31}},
    {"EBX:EBP", 40, {3, 5}, {4, 5, 11, 12, 15, 19, 23, 27, 29}},
    {"EDI:EBP", 160, {7, 5}, {5, 7, 13, 14, 29, 31}},
    {"AL", 0, {0, -1}, {0, 8, 9, 20, 24}},
    {"CL", 0, {1, -1}, {1, 9, 10, 21, 25}},
    {"DL", 0, {2, -1}, {2, 8, 10, 22, 26}},
    {"BL", 8, {3, -1}, {3, 11, 12, 14, 23, 27}},
    {"AH", 0, {0, -1}, {0, 8, 9, 16, 24}},
    {"CH", 0, {1, -1}, {1, 9, 10, 17, 25}},
    {"DH", 0, {2, -1}, {2, 8, 10, 18, 26}},
    {"BH", 8, {3, -1}, {3, 11, 12, 14, 19, 27}},
    {"AX", 0, {0, -1}, {0, 8, 9, 16, 20}},
    {"CX", 0, {1, -1}, {1, 9, 10, 17, 21}},
    {"DX", 0, {2, -1}, {2, 8, 10, 18, 22}},
    {"BX", 8, {3, -1}, {3, 11, 12, 14, 19, 23}},
    {"", 0},
    {"BP", 32, {5, -1}, {5, 14, 15}},
    {"SI", 64, {6, -1}, {6, 11, 13}},
    {"DI", 128, {7, -1}, {7, 12, 13, 15}},
    {"xmm0s", 0, {0, -1}, {40, 44, 48, 56, 60}},
    {"xmm1s", 0, {1, -1}, {40, 45, 49, 56, 61}},
    {"xmm2s", 0, {2, -1}, {41, 46, 50, 57, 62}},
    {"xmm3s", 0, {3, -1}, {41, 47, 51, 57, 63}},
    {"xmm4s", 0, {4, -1}, {42, 44, 52, 58, 60}},
    {"xmm5s", 0, {5, -1}, {42, 45, 53, 58, 61}},
    {"xmm6s", 0, {6, -1}, {43, 46, 54, 59, 62}},
    {"xmm7s", 0, {7, -1}, {43, 47, 55, 59, 63}},
    {"xmm1:xmm0s", 0, {1, 0}, {32, 33, 44, 45, 48, 49, 56, 60, 61}},
    {"xmm3:xmm2s", 0, {3, 2}, {34, 35, 46, 47, 50, 51, 57, 62, 63}},
    {"xmm5:xmm4s", 0, {5, 4}, {36, 37, 44, 45, 52, 53, 58, 60, 61}},
    {"xmm7:xmm6s", 0, {7, 6}, {38, 39, 46, 47, 54, 55, 59, 62, 63}},
    {"xmm4:xmm0s", 0, {4, 0}, {32, 36, 40, 42, 48, 52, 56, 58, 60}},
    {"xmm5:xmm1s", 0, {5, 1}, {33, 37, 40, 42, 49, 53, 56, 58, 61}},
    {"xmm6:xmm2s", 0, {6, 2}, {34, 38, 41, 43, 50, 54, 57, 59, 62}},
    {"xmm7:xmm3s", 0, {7, 3}, {35, 39, 41, 43, 51, 55, 57, 59, 63}},
    {"xmm0d", 0, {0, -1}, {32, 40, 44, 56, 60}},
    {"xmm1d", 0, {1, -1}, {33, 40, 45, 56, 61}},
    {"xmm2d", 0, {2, -1}, {34, 41, 46, 57, 62}},
    {"xmm3d", 0, {3, -1}, {35, 41, 47, 57, 63}},
    {"xmm4d", 0, {4, -1}, {36, 42, 44, 58, 60}},
    {"xmm5d", 0, {5, -1}, {37, 42, 45, 58, 61}},
    {"xmm6d", 0, {6, -1}, {38, 43, 46, 59, 62}},
    {"xmm7d", 0, {7, -1}, {39, 43, 47, 59, 63}},
    {"xmm1:xmm0d", 0, {1, 0}, {32, 33, 40, 44, 45, 48, 49, 60, 61}},
    {"xmm3:xmm2d", 0, {3, 2}, {34, 35, 41, 46, 47, 50, 51, 62, 63}},
    {"xmm5:xmm4d", 0, {5, 4}, {36, 37, 42, 44, 45, 52, 53, 60, 61}},
    {"xmm7:xmm6d", 0, {7, 6}, {38, 39, 43, 46, 47, 54, 55, 62, 63}},
    {"xmm4:xmm0d", 0, {4, 0}, {32, 36, 40, 42, 44, 48, 52, 56, 58}},
    {"xmm5:xmm1d", 0, {5, 1}, {33, 37, 40, 42, 45, 49, 53, 56, 58}},
    {"xmm6:xmm2d", 0, {6, 2}, {34, 38, 41, 43, 46, 50, 54, 57, 59}},
    {"xmm7:xmm3d", 0, {7, 3}, {35, 39, 41, 43, 47, 51, 55, 57, 59}},
};

static Optimizer::ARCH_REGCLASS unpushedDoubleDoubleRegs = {nullptr, {56, 57, 58, 59, 60, 61, 62, 63}};
static Optimizer::ARCH_REGCLASS unpushedDoubleFloatRegs = {&unpushedDoubleDoubleRegs, {40, 41, 42, 43, 44, 45, 46, 47}};
static Optimizer::ARCH_REGCLASS unpushedFDoubleRegs = {&unpushedDoubleFloatRegs, {48, 49, 50, 51, 52, 53, 54, 55}};
static Optimizer::ARCH_REGCLASS unpushedFloatRegs = {&unpushedFDoubleRegs, {32, 33, 34, 35, 36, 37, 38, 39}};

static Optimizer::ARCH_REGCLASS pushedDoubleDoubleRegs = {nullptr, {}};
static Optimizer::ARCH_REGCLASS pushedDoubleFloatRegs = {&pushedDoubleDoubleRegs, {}};
static Optimizer::ARCH_REGCLASS pushedFDoubleRegs = {&pushedDoubleFloatRegs, {}};
static Optimizer::ARCH_REGCLASS pushedFloatRegs = {&pushedFDoubleRegs, {}};

static Optimizer::ARCH_REGCLASS pushedDoubleRegs = {&pushedFloatRegs, {11, 12, 13, 14, 15}, -2};
static Optimizer::ARCH_REGCLASS unpushedDoubleRegs = {&unpushedFloatRegs, {8, 9, 10}};
static Optimizer::ARCH_REGCLASS pushedDwordRegs = {&pushedDoubleRegs, {3, 6, 7, 5}, -1};
static Optimizer::ARCH_REGCLASS unpushedDwordRegs = {&unpushedDoubleRegs, {0, 1, 2}};
static Optimizer::ARCH_REGCLASS pushedWordRegs = {&pushedDwordRegs, {27, 30, 31, 29}, -1};
static Optimizer::ARCH_REGCLASS unpushedWordRegs = {&unpushedDwordRegs, {24, 25, 26}};
static Optimizer::ARCH_REGCLASS pushedByteRegs = {&pushedWordRegs, {19, 23}};
static Optimizer::ARCH_REGCLASS unpushedByteRegs = {&unpushedWordRegs, {16, 17, 18, 20, 21, 22}};
static Optimizer::ARCH_REGCLASS allByteRegs = {nullptr, {16, 17, 18, 19, 20, 21, 22, 23}};
static Optimizer::ARCH_REGCLASS allWordRegs = {&allByteRegs, {24, 25, 26, 27, 30, 31, 29}, -1};
static Optimizer::ARCH_REGCLASS allDwordRegs = {&allWordRegs, {0, 1, 2, 3, 6, 7, 5}, -1};
static Optimizer::ARCH_REGCLASS allDoubleRegs = {&allDwordRegs, {8, 9, 10, 11, 12, 13, 14, 15}, -2};
static Optimizer::ARCH_REGCLASS allFloatRegs = {&allDoubleRegs, {32, 33, 34, 35, 36, 37, 38, 39}};
static Optimizer::ARCH_REGCLASS allFDoubleRegs = {&allFloatRegs, {48, 49, 50, 51, 52, 53, 54, 55}};
static Optimizer::ARCH_REGCLASS allDoubleFloatRegs = {&allFDoubleRegs, {40, 41, 42, 43, 44, 45, 46, 47}};
static Optimizer::ARCH_REGCLASS allDoubleDoubleRegs = {&allDoubleFloatRegs, {56, 57, 58, 59, 60, 61, 62, 63}};
static Optimizer::ARCH_REGCLASS* regClassDefaults[27][2] = {
    {nullptr, nullptr},
    {nullptr, nullptr},
    {&allByteRegs, &pushedByteRegs},
    {&allByteRegs, &pushedByteRegs},
    {&allWordRegs, &pushedWordRegs},
    {&allWordRegs, &pushedWordRegs},
    {&allWordRegs, &pushedWordRegs},
    {&allDwordRegs, &pushedDwordRegs},
    {&allDwordRegs, &pushedDwordRegs},
    {&allDwordRegs, &pushedDwordRegs},
    {&allDwordRegs, &pushedDwordRegs},
    {&allDoubleRegs, &pushedDoubleRegs},
    {&allDwordRegs, &pushedDwordRegs},
    {nullptr, nullptr},
    {nullptr, nullptr},
    {nullptr, nullptr},
    {nullptr, nullptr},
    {nullptr, nullptr},
    {&allFloatRegs, &pushedFloatRegs},
    {&allFDoubleRegs, &pushedFDoubleRegs},
    {&allFDoubleRegs, &pushedFDoubleRegs},
    {&allFloatRegs, &pushedFloatRegs},
    {&allFDoubleRegs, &pushedFDoubleRegs},
    {&allFDoubleRegs, &pushedFDoubleRegs},
    {&allDoubleFloatRegs, &pushedDoubleFloatRegs},
    {&allDoubleDoubleRegs, &pushedDoubleDoubleRegs},
    {&allDoubleDoubleRegs, &pushedDoubleDoubleRegs},
};
static Optimizer::ARCH_REGVERTEX pushedVertex = {&pushedByteRegs, nullptr, nullptr};
static Optimizer::ARCH_REGVERTEX unpushedVertex = {&unpushedByteRegs, nullptr, nullptr};
static Optimizer::ARCH_REGVERTEX regRoot = {&allDoubleDoubleRegs, &pushedVertex, &unpushedVertex};

static Optimizer::ARCH_SIZING regCosts = {
    1, /*char a_bool; */
    1, /*char a_char; */
    4, /*char a_short; */
    4, /*char a_wchar_t; */
    4, /*char a_enum; */
    4, /*char a_int; */
    4, /*char a_long; */
    8, /*char a_longlong; */
    4, /*char a_addr; */
    8, /*char a_farptr; */
    4, /*char a_farseg; */
    8, /*char a_memberptr; */
    0,
    /*char a_struct; */ /* alignment only */
    0,                  /*char a_float; */
    0,                  /*char a_double; */
    0,                  /*char a_longdouble; */
    0,                  /*char a_fcomplexpad; */
    0,                  /*char a_rcomplexpad; */
    0,                  /*char a_lrcomplexpad; */
};

static Optimizer::UBYTE allocOrder[] = {1, 1};

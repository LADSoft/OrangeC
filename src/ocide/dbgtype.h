/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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
 */

enum eDbgType
{
    eNone = 0,
    ePointer,
    eFunction,
    eBitField,
    eStruct,
    eUnion,
    eArray,
    eVla,
    eEnum,
    eField,
    eTypeDef,
    eLRef,
    eRRef,
    eVoid = 32,
    ePVoid,
    eBool,
    eBit,
    eChar = 40,
    eShort,
    eInt,
    eLong,
    eLongLong,
    eWcharT,
    eChar16T,
    eChar32T,
    eUChar = 48,
    eUShort,
    eUInt,
    eULong,
    eULongLong,
    ePChar = 56,
    ePShort,
    ePInt,
    ePLong,
    ePLongLong,
    ePWcharT,
    ePChar16T,
    ePChar32T,
    ePUChar = 64,
    ePUShort,
    ePUInt,
    ePULong,
    ePULongLong,
    eFloat = 72,
    eDouble,
    eLongDouble,
    eImaginary = 80,
    eImaginaryDouble,
    eImaginaryLongDouble,
    eComplex = 88,
    eComplexDouble,
    eComplexLongDouble,
    ePFloat = 96,
    ePDouble,
    ePLongDouble,
    ePImaginary = 104,
    ePImaginaryDouble,
    ePImaginaryLongDouble,
    ePComplex = 112,
    ePComplexDouble,
    ePComplexLongDouble,
    eReservedTop = 1023
};
enum eDBReg
{
    DB_REG_NONE = 0,
    DB_REG_AL = 1,
    DB_REG_CL = 2,
    DB_REG_DL = 3,
    DB_REG_BL = 4,
    DB_REG_AH = 5,
    DB_REG_CH = 6,
    DB_REG_DH = 7,
    DB_REG_BH = 8,
    DB_REG_AX = 9,
    DB_REG_CX = 10,
    DB_REG_DX = 11,
    DB_REG_BX = 12,
    DB_REG_SP = 13,
    DB_REG_BP = 14,
    DB_REG_SI = 15,
    DB_REG_DI = 16,
    DB_REG_EAX = 17,
    DB_REG_ECX = 18,
    DB_REG_EDX = 19,
    DB_REG_EBX = 20,
    DB_REG_ESP = 21,
    DB_REG_EBP = 22,
    DB_REG_ESI = 23,
    DB_REG_EDI = 24,
    DB_REG_ES = 25,
    DB_REG_CS = 26,
    DB_REG_SS = 27,
    DB_REG_DS = 28,
    DB_REG_FS = 29,
    DB_REG_GS = 30,
    DB_REG_IP = 31,
    DB_REG_FLAGS = 32,
    DB_REG_EIP = 33,
    DB_REG_EFLAGS = 34
};
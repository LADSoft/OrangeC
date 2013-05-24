/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2012, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/
    enum eDbgType { eNone = 0, ePointer, eFunction, eBitField,
                 eStruct, eUnion, eArray, eVla, eEnum,
                 eField, eTypeDef,
                 eVoid = 32, ePVoid, eBool, eBit,
                 eChar = 40, eShort, eInt, eLong,
                 eLongLong, eWcharT, eChar16T, eChar32T,
                 eUChar = 48, eUShort, eUInt, eULong,
                 eULongLong,
                 ePChar = 56, ePShort, ePInt, ePLong,
                 ePLongLong, ePWcharT, ePChar16T, ePChar32T,
                 ePUChar = 64, ePUShort, ePUInt, ePULong,
                 ePULongLong,
                 eFloat = 72, eDouble, eLongDouble,
                 eImaginary = 80, eImaginaryDouble, eImaginaryLongDouble,
                 eComplex = 88, eComplexDouble, eComplexLongDouble,
                 ePFloat = 96, ePDouble, ePLongDouble,
                 ePImaginary = 104, ePImaginaryDouble, ePImaginaryLongDouble,
                 ePComplex = 112, ePComplexDouble, ePComplexLongDouble,
                 eReservedTop = 1023
    } ;
    enum eDBReg {
            DB_REG_NONE = 0, 
            DB_REG_AL = 1, DB_REG_CL = 2, DB_REG_DL = 3, DB_REG_BL = 4,
            DB_REG_AH = 5, DB_REG_CH = 6, DB_REG_DH = 7, DB_REG_BH = 8,
            DB_REG_AX = 9, DB_REG_CX = 10, DB_REG_DX = 11, DB_REG_BX = 12,
            DB_REG_SP = 13, DB_REG_BP = 14, DB_REG_SI = 15, DB_REG_DI = 16,
            DB_REG_EAX = 17, DB_REG_ECX = 18, DB_REG_EDX = 19, DB_REG_EBX = 20,
            DB_REG_ESP = 21, DB_REG_EBP = 22, DB_REG_ESI = 23, DB_REG_EDI = 24,
            DB_REG_ES = 25, DB_REG_CS = 26, DB_REG_SS = 27, DB_REG_DS = 28,
            DB_REG_FS = 29, DB_REG_GS = 30, DB_REG_IP = 31, DB_REG_FLAGS = 32,
            DB_REG_EIP = 33, DB_REG_EFLAGS = 34
    } ;
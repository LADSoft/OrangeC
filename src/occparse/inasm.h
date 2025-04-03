/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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

#pragma once

#define ERR_LABEL_EXPECTED 0
#define ERR_ILLEGAL_ADDRESS_MODE 1
#define ERR_ADDRESS_MODE_EXPECTED 2
#define ERR_INVALID_OPCODE 3
#define ERR_INVALID_SIZE 4
#define ERR_INVALID_INDEX_MODE 5
#define ERR_INVALID_SCALE_SPECIFIER 6
#define ERR_USE_LEA 7
#define ERR_TOO_MANY_SEGMENTS 8
#define ERR_SYNTAX 9
#define ERR_UNKNOWN_OP 10
#define ERR_BAD_OPERAND_COMBO 11
#define ERR_INVALID_USE_OF_INSTRUCTION 12
namespace Parser
{
typedef struct
{
    const char* name;
    int instruction;
    void* data;
} ASM_HASH_ENTRY;

void inlineAsmInit(void);
int inasm_enterauto(EXPRESSION* node, int* reg1, int* reg2);
LexList* inlineAsm(LexList* inlex, std::list<FunctionBlock*>& parent);
void adjustcodelab(void* select, int offset);
void* inlineAsmStmt(void* param);
}  // namespace Parser
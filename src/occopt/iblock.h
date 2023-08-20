#pragma once
/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

#include "FNV_hash.h"

namespace Optimizer
{
extern TEMP_INFO** tempInfo;
extern BLOCK** blockArray;

extern QUAD *intermed_head, *intermed_tail;
extern int blockCount;
extern std::unordered_map<QUAD*, QUAD*, OrangeC::Utils::fnv1a32_binary<DAGCOMPARE>, OrangeC::Utils::bin_eql<DAGCOMPARE>> ins_hash;
extern std::unordered_map<IMODE**, QUAD*, OrangeC::Utils::fnv1a32_binary<sizeof(IMODE*)>, OrangeC::Utils::bin_eql<sizeof(IMODE*)>>
    name_hash;
extern bool wasgoto;

extern BLOCK* currentBlock;

extern int blockMax;

void gen_nodag(enum i_ops op, IMODE* res, IMODE* left, IMODE* right);
int equalimode(IMODE* ap1, IMODE* ap2);
void add_intermed(QUAD* newQuad);
IMODE* liveout2(QUAD* q);
QUAD* liveout(QUAD* node);
int ToQuadConst(IMODE** im);
bool usesAddress(IMODE* im);
void flush_dag(bool leaveAddresses = false);
void dag_rundown(void);
BLOCKLIST* newBlock(void);
void addblock(int val);
void gen_label(int labno);
QUAD* gen_icode_with_conflict(enum i_ops op, IMODE* res, IMODE* left, IMODE* right, bool conflicting, bool atomic = false);
QUAD* gen_icode(enum i_ops op, IMODE* res, IMODE* left, IMODE* right);
QUAD* gen_icode_with_atomic(enum i_ops op, IMODE* res, IMODE* left, IMODE* right);
void gen_iiconst(IMODE* res, long long val);
void gen_ifconst(IMODE* res, FPF val);
void gen_igoto(enum i_ops op, long label);
void gen_data(int val);
void gen_icgoto(enum i_ops op, long label, IMODE* left, IMODE* right);
QUAD* gen_igosub(enum i_ops op, IMODE* left);
void gen_icode2(enum i_ops op, IMODE* res, IMODE* left, IMODE* right, int label);
void gen_line(LINEDATA* data);
void gen_nodag(enum i_ops op, IMODE* res, IMODE* left, IMODE* right);
void RemoveFromUses(QUAD* ins, int tnum);
void InsertUses(QUAD* ins, int tnum);
void RemoveInstruction(QUAD* ins);
void InsertInstruction(QUAD* before, QUAD* ins);
}  // namespace Optimizer
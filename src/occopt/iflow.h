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

namespace Optimizer
{
extern int firstLabel;
extern QUAD *criticalThunks, **criticalThunkPtr;
extern int walkPreorder, walkPostorder;

typedef struct _edge
{
    struct _edge* next;
    int first;
    int second;
    enum e_fgtype edgetype;
} EDGE;

#define EDGE_HASH_SIZE 256

void flow_init(void);
void dump_flowgraph(void);
void WalkFlowgraph(BLOCK* b, int (*func)(enum e_fgtype type, BLOCK* parent, BLOCK* b), int fwd);
void reflowConditional(BLOCK* src, BLOCK* dst);
bool dominatedby(BLOCK* src, BLOCK* ancestor);
enum e_fgtype getEdgeType(int first, int second);
void UnlinkCritical(BLOCK* s);
void RemoveCriticalThunks(void);
void unlinkBlock(BLOCK* succ, BLOCK* pred);
void doms_only(bool always);
void flows_and_doms(void);
}  // namespace Optimizer
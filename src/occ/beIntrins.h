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

namespace occx86
{
void promoteToBoolean(AMODE* addr);
bool handleBSR(void);
bool handleBSF(void);
bool handleINB(void);
bool handleINW(void);
bool handleIND(void);
bool handleOUTB(void);
bool handleOUTW(void);
bool handleOUTD(void);
bool handleROTL8(void);
bool handleROTL16(void);
bool handleROTR8(void);
bool handleROTR16(void);
bool handleROTL(void);
bool handleROTR(void);
bool handleCTZ(void);
bool handleCLZ(void);
bool handleBSWAP16(void);
bool handleBSWAP32(void);
bool handleBSWAP64(void);
bool handlePAUSE(void);
bool handleVASTART(void);
bool handleVAARG(void);
bool BackendIntrinsic(Optimizer::QUAD* q);
}  // namespace occx86
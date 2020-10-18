/* Software License Agreement
 *
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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

#include "ObjFactory.h"
#include "ObjIeee.h"
#ifdef XXXXX
#    include "LinkPartition.h"
#    include "LinkAttribs.h"
#    include "LinkExpression.h"
#    include "LinkLibrary.h"
#    include "LinkManager.h"
#    include "LinkMap.h"
#    include "LinkOverlay.h"
#    include "LinkPartition.h"
#    include "LinkRegion.h"
#    include "LinkRegionFileSpec.h"
#    include "LinkRemapper.h"
#    include "LinkTokenizer.h"
#    include "CmdSwitch.h"
#endif
#include <cstdio>
#include <iomanip>

int main()
{
    try
    {
        ObjIeeeIndexManager im1;
        ObjFactory fact1(&im1);
        FILE* b = fopen("bzip2.o", "r");
        FILE* c = fopen("q.o", "w");
        ObjIeee i1("hi");
        ObjFile* fi1 = i1.Read(b, ObjIeee::eAll, &fact1);
        i1.Write(c, fi1, &fact1);
    }
    catch (ObjIeeeBinary::SyntaxError e)
    {
       std::cout << e.what() << std::endl;
    }

}

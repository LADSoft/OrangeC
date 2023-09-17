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

/*
 * C configuration when we are using ICODE code generator output
 */
#include <cstdio>
#include <cstring>
#include <cctype>
#include "compiler.h"
#include "PreProcessor.h"
#include "Utils.h"
#include "CmdSwitch.h"
#include "config.h"
#include "help.h"
#include "beinterf.h"

namespace Parser
{
TYPE stdobject = {BasicType::object_, 0};
TYPE stdvoid = {BasicType::void_, 0, 0, &stdvoid};
TYPE stdany = {BasicType::any_, 1, 0, &stdany};
TYPE stdauto = {BasicType::auto_, 0, 0, &stdauto};
TYPE stdfunc = {BasicType::func_, 0, &stdvoid, &stdfunc};
TYPE stdpointer = {BasicType::pointer_, 4, &stdvoid, &stdpointer};
TYPE std__string = {BasicType::string_, 4, 0, &std__string};
TYPE std__object = {BasicType::object_, 4, 0, &std__object};
TYPE stdnullpointer = {BasicType::pointer_, 4, &stdvoid, &stdnullpointer};
TYPE stdfloatcomplex = {BasicType::float__complex_, 8, 0, &stdfloatcomplex};
TYPE stddoublecomplex = {BasicType::double__complex_, 16, 0, &stddoublecomplex};
TYPE stdlongdoublecomplex = {BasicType::long_double_complex_, 16, 0, &stdlongdoublecomplex};
TYPE stdfloat = {BasicType::float_, 4, 0, &stdfloat};
TYPE stdfloatimaginary = {BasicType::float__imaginary_, 4, 0, &stdfloatimaginary};
TYPE stddouble = {BasicType::double_, 8, 0, &stddouble};
TYPE stddoubleimaginary = {BasicType::double__imaginary_, 8, 0, &stddoubleimaginary};
TYPE stdlongdoubleimaginary = {BasicType::long_double_imaginary_, 8, 0, &stdlongdoubleimaginary};
TYPE stdlonglong = {BasicType::long_long_, 8, 0, &stdlonglong};
TYPE stdunsigned = {BasicType::unsigned_, 4, 0, &stdunsigned};
TYPE stdunative = {BasicType::unative_, 4, 0, &stdunative};
TYPE stdunsignedlong = {BasicType::unsigned_long_, 4, 0, &stdunsignedlong};
TYPE stdunsignedlonglong = {BasicType::unsigned_long_long_, 8, 0, &stdunsignedlonglong};
TYPE stdconst = {BasicType::int_, 4, 0, &stdconst};
TYPE stdchar = {BasicType::char_, 1, 0, &stdchar};
TYPE stdchar8_t = {BasicType::char_, 1, 0, &stdchar};
TYPE stdchar16t = {BasicType::char16_t_, 2, 0, &stdchar16t};
TYPE stdchar16tptr = {BasicType::pointer_, 0, &stdchar16t, &stdchar16tptr};
TYPE stdchar32t = {BasicType::char32_t_, 4, 0, &stdchar32t};
TYPE stdchar32tptr = {BasicType::pointer_, 0, &stdchar32t, &stdchar32tptr};
TYPE stdsignedchar = {BasicType::char_, 1, 0, &stdsignedchar};
TYPE stdunsignedchar = {BasicType::unsigned_char_, 1, 0, &stdunsignedchar};
TYPE stdshort = {BasicType::short_, 2, 0, &stdshort};
TYPE stdunsignedshort = {BasicType::unsigned_short_, 2, 0, &stdunsignedshort};
TYPE std__func__nc = {BasicType::pointer_, 4, &stdchar, &std__func__nc};
static TYPE std__func__c = {BasicType::const_, 4, &stdchar, &stdchar};
TYPE std__func__ = {BasicType::pointer_, 4, &std__func__c, &std__func__};
TYPE stdstring = {BasicType::pointer_, 4, &stdchar, &stdstring};
TYPE stdint = {BasicType::int_, 4, 0, &stdint};
TYPE stdinative = {BasicType::inative_, 4, 0, &stdinative};
TYPE stdlong = {BasicType::long_, 4, 0, &stdlong};
TYPE stdlongdouble = {BasicType::long_double_, 8, 0, &stdlongdouble};
TYPE stdbool = {BasicType::bool_, 1, 0, &stdbool};
TYPE stdwidechar = {BasicType::wchar_t_, 0, 0, &stdwidechar};
TYPE stdwcharptr = {BasicType::pointer_, 0, &stdwidechar, &stdwcharptr};
TYPE stdcharptr = {BasicType::pointer_, 0, &stdchar, &stdcharptr};
TYPE stdchar8_tptr = {BasicType::pointer_, 0, &stdchar8_t, &stdchar8_tptr};

int init_backend()
{
    int rv;
    Optimizer::SelectBackendData();
    if (!Optimizer::chosenAssembler->objext)
        Optimizer::cparams.prm_asmfile = true;
    rv = true;  // be_init(&cparams, Optimizer::chosenAssembler, chosenDebugger);
    if (rv)
    {
        stdpointer.size = getSize(BasicType::pointer_);
        stdnullpointer.size = getSize(BasicType::pointer_);
        stdnullpointer.nullptrType = true;
        stdfloatimaginary.size = stdfloat.size = getSize(BasicType::float_);
        stddouble.size = stddoubleimaginary.size = getSize(BasicType::double_);
        stdlongdouble.size = stdlongdoubleimaginary.size = getSize(BasicType::long_double_);
        stdfloatcomplex.size = getSize(BasicType::float__complex_);
        stddoublecomplex.size = getSize(BasicType::double__complex_);
        stdlongdoublecomplex.size = getSize(BasicType::long_double_complex_);
        stdunsignedlonglong.size = stdlonglong.size = getSize(BasicType::long_long_);
        stdunsignedlong.size = stdlong.size = getSize(BasicType::long_);
        stdconst.size = stdunsigned.size = stdint.size = getSize(BasicType::unsigned_);
        stdstring.size = getSize(BasicType::pointer_);
        std__string.size = getSize(BasicType::pointer_);
        std__object.size = getSize(BasicType::pointer_);
        stdchar.size = getSize(BasicType::char_);
        stdsignedchar.size = getSize(BasicType::unsigned_char_);
        stdunsignedchar.size = getSize(BasicType::unsigned_char_);
        stdshort.size = getSize(BasicType::short_);
        stdunsignedshort.size = getSize(BasicType::unsigned_short_);
        stdcharptr.size = getSize(BasicType::pointer_);
        std__func__.size = getSize(BasicType::pointer_);
        std__func__nc.size = getSize(BasicType::pointer_);
        std__func__nc.array = true;
        stdbool.size = getSize(BasicType::bool_);
        stdwidechar.size = getSize(BasicType::wchar_t_);
        stdwcharptr.size = getSize(BasicType::pointer_);
        stdchar16tptr.size = getSize(BasicType::pointer_);
        stdchar32tptr.size = getSize(BasicType::pointer_);
        stdfunc.size = getSize(BasicType::pointer_);

        stdpointer.rootType = &stdpointer;
        stdnullpointer.rootType = &stdnullpointer;
        stdfloatimaginary.rootType = &stdfloatimaginary;
        stdfloat.rootType = &stdfloat;
        stddouble.rootType = &stddouble;
        stddoubleimaginary.rootType = &stddoubleimaginary;
        ;
        stdlongdouble.rootType = &stdlongdouble;
        stdlongdoubleimaginary.rootType = &stdlongdoubleimaginary;
        ;
        stdfloatcomplex.rootType = &stdfloatcomplex;
        stddoublecomplex.rootType = &stddoublecomplex;
        stdlongdoublecomplex.rootType = &stdlongdoublecomplex;
        stdunsignedlonglong.rootType = &stdunsignedlonglong;
        stdlonglong.rootType = &stdlonglong;
        stdunsignedlong.rootType = &stdunsignedlong;
        stdlong.rootType = &stdlong;
        stdconst.rootType = &stdconst;
        stdunsigned.rootType = &stdunsigned;
        stdint.rootType = &stdint;
        stdunative.rootType = &stdunative;
        stdinative.rootType = &stdinative;
        stdstring.rootType = &stdstring;
        stdchar.rootType = &stdchar;
        stdsignedchar.rootType = &stdsignedchar;
        stdunsignedchar.rootType = &stdunsignedchar;
        stdshort.rootType = &stdshort;
        stdunsignedshort.rootType = &stdunsignedshort;
        stdcharptr.rootType = &stdcharptr;
        UpdateRootTypes(&std__func__);
        UpdateRootTypes(&std__func__c);
        UpdateRootTypes(&std__func__nc);
        stdbool.rootType = &stdbool;
        stdwidechar.rootType = &stdwidechar;
        stdwcharptr.rootType = &stdwcharptr;
        stdchar8_tptr.rootType = &stdchar8_tptr;
        stdchar16tptr.rootType = &stdchar16tptr;
        stdchar32tptr.rootType = &stdchar32tptr;
        if (Optimizer::cparams.c_dialect >= Dialect::c2x)
            stdchar8_t.type = BasicType::char8_t_;
    }
    return rv;
}
}  // namespace Parser
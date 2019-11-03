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

 /*
  * C configuration when we are using ICODE code generator output
  */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "compiler.h"
#include "PreProcessor.h"
#include "Utils.h"
#include "CmdSwitch.h"

extern COMPILER_PARAMS cparams;
extern ARCH_ASM* chosenAssembler;


TYPE stdobject = { bt___object, 0 };
TYPE stdvoid = { bt_void, 0 };
TYPE stdany = { bt_any, 1 };
TYPE stdauto = { bt_auto, 0 };
TYPE stdfunc = { bt_func, 0, &stdvoid };
TYPE stdpointer = { bt_pointer, 4, &stdvoid };
TYPE std__string = { bt___string, 4 };
TYPE std__object = { bt___object, 4 };
TYPE stdnullpointer = { bt_pointer, 4, &stdvoid };
TYPE stdfloatcomplex = { bt_float_complex, 8 };
TYPE stddoublecomplex = { bt_double_complex, 16 };
TYPE stdlongdoublecomplex = { bt_long_double_complex, 16 };
TYPE stdfloat = {
    bt_float,
    4,
};
TYPE stdfloatimaginary = {
    bt_float_imaginary,
    4,
};
TYPE stddouble = { bt_double, 8 };
TYPE stddoubleimaginary = { bt_double_imaginary, 8 };
TYPE stdlongdoubleimaginary = { bt_long_double_imaginary, 8 };
TYPE stdlonglong = { bt_long_long, 8 };
TYPE stdunsigned = { bt_unsigned, 4 };
TYPE stdunative = { bt_unative, 4 };
TYPE stdunsignedlong = {
    bt_unsigned_long,
    4,
};
TYPE stdunsignedlonglong = { bt_unsigned_long_long, 8 };
TYPE stdconst = {
    bt_int,
    4,
};
TYPE stdchar = {
    bt_char,
    1,
};
TYPE stdchar16t = {
    bt_char16_t,
    2,
};
TYPE stdchar16tptr = { bt_pointer, 0, &stdchar16t };
TYPE stdchar32t = {
    bt_char32_t,
    4,
};
TYPE stdchar32tptr = { bt_pointer, 0, &stdchar32t };
TYPE stdsignedchar = {
    bt_char,
    1,
};
TYPE stdunsignedchar = {
    bt_unsigned_char,
    1,
};
TYPE stdshort = {
    bt_short,
    2,
};
TYPE stdunsignedshort = {
    bt_unsigned_short,
    2,
};
TYPE std__func__nc = { bt_pointer, 4, &stdchar };
static TYPE std__func__c = { bt_const, 4, &stdchar };
TYPE std__func__ = { bt_pointer, 4, &std__func__c };
TYPE stdstring = { bt_pointer, 4, &stdchar };
TYPE stdint = { bt_int, 4 };
TYPE stdinative = { bt_inative, 4 };
TYPE stdlong = { bt_long, 4 };
TYPE stdlongdouble = { bt_long_double, 8 };
TYPE stdbool = { bt_bool, 1 };
TYPE stdwidechar = { bt_wchar_t, 0 };
TYPE stdwcharptr = { bt_pointer, 0, &stdwidechar };
TYPE stdcharptr = { bt_pointer, 0, &stdchar };


int init_backend()
{
    int rv;
    SelectBackendData();
    if (!chosenAssembler->objext)
        cparams.prm_asmfile = true;
    rv = true;// be_init(&cparams, chosenAssembler, chosenDebugger);
    if (rv)
    {
        stdpointer.size = getSize(bt_pointer);
        stdnullpointer.size = getSize(bt_pointer);
        stdnullpointer.nullptrType = true;
        stdfloatimaginary.size = stdfloat.size = getSize(bt_float);
        stddouble.size = stddoubleimaginary.size = getSize(bt_double);
        stdlongdouble.size = stdlongdoubleimaginary.size = getSize(bt_long_double);
        stdfloatcomplex.size = getSize(bt_float_complex);
        stddoublecomplex.size = getSize(bt_double_complex);
        stdlongdoublecomplex.size = getSize(bt_long_double_complex);
        stdunsignedlonglong.size = stdlonglong.size = getSize(bt_long_long);
        stdunsignedlong.size = stdlong.size = getSize(bt_long);
        stdconst.size = stdunsigned.size = stdint.size = getSize(bt_unsigned);
        stdstring.size = getSize(bt_pointer);
        std__string.size = getSize(bt_pointer);
        std__object.size = getSize(bt_pointer);
        stdchar.size = getSize(bt_char);
        stdsignedchar.size = getSize(bt_unsigned_char);
        stdunsignedchar.size = getSize(bt_unsigned_char);
        stdshort.size = getSize(bt_short);
        stdunsignedshort.size = getSize(bt_unsigned_short);
        stdcharptr.size = getSize(bt_pointer);
        std__func__.size = getSize(bt_pointer);
        std__func__nc.size = getSize(bt_pointer);
        std__func__nc.array = true;
        stdbool.size = getSize(bt_bool);
        stdwidechar.size = getSize(bt_wchar_t);
        stdwcharptr.size = getSize(bt_pointer);
        stdchar16tptr.size = getSize(bt_pointer);
        stdchar32tptr.size = getSize(bt_pointer);

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
        stdchar16tptr.rootType = &stdchar16tptr;
        stdchar32tptr.rootType = &stdchar32tptr;
    }
    return rv;
}

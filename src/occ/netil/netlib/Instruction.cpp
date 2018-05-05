/* Software License Agreement
 * 
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the 
 *     Orange C "Target Code" exception.
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

#include "DotNetPELib.h"
#include <float.h>
namespace DotNetPELib
{
    Instruction::InstructionName Instruction::instructions_[] =
    {
        { "<unknown>", 0, -1, 0, o_none, 0 },
        { ".label", 0, -1, 0, o_none, 0 },
        { ".comment", 0, -1, 0, o_none, 0},
        { ".SEH", 0, -1, 0, o_none, 0},
        { "add", 0x58, -1, 1, o_single, -1 },
        { "add.ovf", 0xd6, -1, 1, o_single, -1},
        { "add.ovf.un", 0xd7, -1, 1, o_single, -1},
        { "and", 0x5f, -1, 1, o_single, -1 },
        { "arglist", 0xfe, 0, 2, o_single, 1 },
        { "beq", 0x3b, -1, 5, o_rel4, -2},
        { "beq.s", 0x2e, -1, 2, o_rel1, -2 },
        { "bge", 0x3c, -1, 5, o_rel4, -2 },
        { "bge.s", 0x2f, -1, 2, o_rel1, -2 },
        { "bge.un", 0x41, -1, 5, o_rel4, -2 },
        { "bge.un.s", 0x34, -1, 2, o_rel1, -2 },
        { "bgt", 0x3d, -1, 5, o_rel4, -2   },
        { "bgt.s", 0x30, -1, 2, o_rel1, -2},
        { "bgt.un", 0x42, -1, 5, o_rel4, -2 },
        { "bgt.un.s", 0x35, -1, 2, o_rel1, -2 },
        { "ble", 0x3e, -1, 5, o_rel4, -2  },
        { "ble.s", 0x31, -1, 2, o_rel1, -2 },
        { "ble.un", 0x43, -1, 5, o_rel4, -2  },
        { "ble.un.s", 0x36, -1, 2, o_rel1, -2 },
        { "blt", 0x3f, -1, 5, o_rel4, -2  },
        { "blt.s", 0x32, -1, 2, o_rel1, -2 },
        { "blt.un", 0x44, -1, 5, o_rel4, -2  },
        { "blt.un.s", 0x37, -1, 2, o_rel1, -2 },
        { "bne.un", 0x40, -1, 5, o_rel4, -2  },
        { "bne.un.s", 0x33, -1, 2, o_rel1, -2 },
        { "box",0x8c, -1, 5, o_index4, 0 },
        { "br", 0x38, -1, 5, o_rel4, 0 },
        { "br.s", 0x2b, -1, 2, o_rel1, 0 },
        { "break", 0x01, -1, 1, o_single, 0 },
        { "brfalse", 0x39, -1, 5, o_rel4, -1 },
        { "brfalse.s", 0x2c, -1, 2, o_rel1, -1 },
        { "brinst", 0x3a, -1, 5, o_rel4, -1 },
        { "brinst.s", 0x2d, -1, 2, o_rel1, -1 },
        { "brnull", 0x39, -1, 5, o_rel4, -1 },
        { "brnull.s", 0x2c, -1, 2, o_rel1, -1 },
        { "brtrue", 0x3a, -1, 5, o_rel4, -1 },
        { "brtrue.s", 0x2d, -1, 2, o_rel1, -1 },
        { "brzero", 0x39, -1, 5, o_rel4, -1 },
        { "brzero.s", 0x2c, -1, 2, o_rel1, -1 },
        { "call", 0x28, -1, 5, o_index4, 0 }, // - args + 1 if rv nonvoid 
        { "calli", 0x29, -1, 5, o_index4, -1 },  // - args + 1 if rv nonvoid 
        { "callvirt", 0x6f, -1,5, o_index4, 0 }, // - args + 1 if rv nonvoid 
        { "castclass", 0x74, -1, 5, o_index4, 0 },
        { "ceq", 0xfe, 1, 2, o_single, -1 },
        { "cgt", 0xfe, 2, 2, o_single, -1 },
        { "cgt.un", 0xfe, 3, 2, o_single, -1 },
        { "ckfinite", 0xc3, -1, 1, o_single, 0 },
        { "clt", 0xfe, 4, 2, o_single, -1 },
        { "clt.un", 0xfe, 5, 2, o_single, -1 },
        { "constrained.", 0xfe, 0x16, 6, o_index4, 0 },
        { "conv.i", 0xd3, -1, 1, o_single, 0 },
        { "conv.i1", 0x67, -1, 1, o_single, 0 },
        { "conv.i2", 0x68, -1, 1, o_single, 0 },
        { "conv.i4", 0x69, -1, 1, o_single, 0 },
        { "conv.i8", 0x6a, -1, 1, o_single, 0 },
        { "conv.ovf.i", 0xd4, -1, 1, o_single, 0 },
        { "conv.ovf.i.un", 0x8a, -1, 1, o_single, 0 },
        { "conv.ovf.i1", 0xb3, -1, 1, o_single, 0 },
        { "conv.ovf.i1.un", 0x82, -1, 1, o_single, 0 },
        { "conv.ovf.i2", 0xb5, -1, 1, o_single, 0 },
        { "conv.ovf.i2.un", 0x83, -1, 1, o_single, 0 },
        { "conv.ovf.i4", 0xb7, -1, 1, o_single, 0 },
        { "conv.ovf.i4.un", 0x84, -1, 1, o_single, 0 },
        { "conv.ovf.i8", 0xb9, -1, 1, o_single, 0 },
        { "conv.ovf.i8.un", 0x85, -1, 1, o_single, 0 },
        { "conv.ovf.u", 0xd5, -1, 1, o_single, 0 },
        { "conv.ovf.u.un", 0x8b, -1, 1, o_single, 0 },
        { "conv.ovf.u1", 0xb4, -1, 1, o_single, 0 },
        { "conv.ovf.u1.un", 0x86, -1, 1, o_single },
        { "conv.ovf.u2", 0xb6, -1, 1, o_single, 0 },
        { "conv.ovf.u2.un", 0x87, -1, 1, o_single, 0 },
        { "conv.ovf.u4", 0xb8, -1, 1, o_single, 0 },
        { "conv.ovf.u4.un", 0x88, -1, 1, o_single, 0 },
        { "conv.ovf.u8", 0xba, -1, 1, o_single, 0 },
        { "conv.ovf.u8.un", 0x89, -1, 1, o_single, 0 },
        { "conv.r.un", 0x76, -1, 1, o_single, 0 },
        { "conv.r4", 0x6b, -1, 1, o_single, 0 },
        { "conv.r8", 0x6c, -1, 1, o_single, 0 },
        { "conv.u", 0xe0, -1, 1, o_single, 0 },
        { "conv.u1", 0xd2, -1, 1, o_single, 0 },
        { "conv.u2", 0xd1, -1, 1, o_single, 0 },
        { "conv.u4", 0x6d, -1, 1, o_single, 0 },
        { "conv.u8", 0x6e, -1, 1, o_single, 0 },
        { "cpblk", 0xfe, 0x17, 2, o_single, -3 },
        { "cpobj", 0x70, -1, 5, o_index4, -1 },
        { "div", 0x5b, -1, 1, o_single, -1 },
        { "div.un", 0x5c, -1, 1, o_single, -1 },
        { "dup", 0x25, -1, 1, o_single, 1 },
        { "endfault", 0xdc, -1, 1, o_single, 0 },
        { "endfilter", 0xfe, 0x17, 2, o_single, -1 },
        { "endfinally", 0xdc, -1, 1, o_single, 0 },
        { "initblk", 0xfe, 0x18, 2, o_single, -3 },
        { "initobj", 0xfe, 0x15, 6, o_index4, -1 },
        { "isinst", 0x75, -1, 5, o_index4, 0  },
        { "jmp", 0x27, -1, 5, o_index4, 0 },
        { "ldarg", 0xfe, 0x09, 4, o_index2, 1 },
        { "ldarg.0", 0x02, -1, 1, o_single, 1 },
        { "ldarg.1", 0x03, -1, 1, o_single, 1 },
        { "ldarg.2", 0x04, -1, 1, o_single, 1 },
        { "ldarg.3", 0x05, -1, 1, o_single, 1 },
        { "ldarg.s", 0xe, -1, 2, o_index1, 1  },
        { "ldarga", 0xfe, 0x0a, 4, o_index2, 1 },
        { "ldarga.s", 0x0f, -1, 2, o_index1, 1 },
        { "ldc.i4", 0x20, -1, 5, o_immed4, 1 },
        { "ldc.i4.0", 0x16, -1, 1, o_single, 1 },
        { "ldc.i4.1", 0x17, -1, 1, o_single, 1 },
        { "ldc.i4.2", 0x18, -1, 1, o_single, 1 },
        { "ldc.i4.3", 0x19, -1, 1, o_single, 1 },
        { "ldc.i4.4", 0x1a, -1, 1, o_single, 1 },
        { "ldc.i4.5", 0x1b, -1, 1, o_single, 1 },
        { "ldc.i4.6", 0x1c, -1, 1, o_single, 1 },
        { "ldc.i4.7", 0x1d, -1, 1, o_single, 1 },
        { "ldc.i4.8", 0x1e, -1, 1, o_single, 1 },
        { "ldc.i4.m1", 0x15, -1, 1, o_single, 1 },
        { "ldc.i4.M1", 0x15, -1, 1, o_single, 1 },
        { "ldc.i4.s", 0x1f, -1, 2, o_immed1, 1 },
        { "ldc.i8", 0x21, -1, 9, o_immed8, 1 },
        { "ldc.r4", 0x22, -1, 5, o_float4, 1 },
        { "ldc.r8", 0x23, -1, 9, o_float8, 1 },
        { "ldelem", 0xa3, -1, 5, o_index4, -1 },
        { "ldelem.i", 0x97, -1, 1, o_single, -1  },
        { "ldelem.i1", 0x90, -1, 1, o_single, -1 },
        { "ldelem.i2", 0x92, -1, 1, o_single, -1 },
        { "ldelem.i4", 0x94, -1, 1, o_single, -1 },
        { "ldelem.i8", 0x96, -1, 1, o_single, -1 },
        { "ldelem.r4", 0x98, -1, 1, o_single, -1 },
        { "ldelem.r8", 0x99, -1, 1, o_single, -1 },
        { "ldelem.ref", 0x9a, -1, 1, o_single, -1 },
        { "ldelem.u1", 0x91, -1, 1, o_single, -1 },
        { "ldelem.u2", 0x93, -1, 1, o_single, -1 },
        { "ldelem.u4", 0x95, -1, 1, o_single, -1 },
        { "ldelem.u8", 0x96, -1, 1, o_single, -1 },
        { "ldelema", 0x8f, -1, 5, o_index4, -1 },
        { "ldfld", 0x7b, -1, 5, o_index4, 0 },
        { "ldflda", 0x7c, -1, 5, o_index4, 0 },
        { "ldftn", 0xfe, 0x06, 6, o_index4, 1 },
        { "ldind.i", 0x4d, -1, 1, o_single, 0 },
        { "ldind.i1", 0x46, -1, 1, o_single, 0 },
        { "ldind.i2", 0x48, -1, 1, o_single, 0 },
        { "ldind.i4", 0x4a, -1, 1, o_single, 0 },
        { "ldind.i8", 0x4c, -1, 1, o_single, 0 },
        { "ldind.r4", 0x4e, -1, 1, o_single, 0 },
        { "ldind.r8", 0x4f, -1, 1, o_single, 0 },
        { "ldind.ref", 0x50, -1, 1, o_single, 0 },
        { "ldind.u1", 0x47, -1, 1, o_single, 0 },
        { "ldind.u2", 0x49, -1, 1, o_single, 0 },
        { "ldind.u4", 0x4b, -1, 1, o_single, 0 },
        { "ldind.u8", 0x4c, -1, 1, o_single, 0 },
        { "ldlen", 0x8e, -1, 1, o_single, 0 },
        { "ldloc", 0xfe, 0x0c, 4, o_index2, 1 },
        { "ldloc.0", 0x06, -1, 1, o_single, 1 },
        { "ldloc.1", 0x07, -1, 1, o_single, 1 },
        { "ldloc.2", 0x08, -1, 1, o_single, 1 },
        { "ldloc.3", 0x09, -1, 1, o_single, 1 },
        { "ldloc.s", 0x11, -1, 2, o_index1, 1 },
        { "ldloca", 0xfe, 0x0d, 4, o_index2, 1 },
        { "ldloca.s", 0x12, -1, 2, o_index1, 1},
        { "ldnull", 0x14, -1, 1, o_single, 1 },
        { "ldobj", 0x71, -1, 5, o_index4, 0 },
        { "ldsfld", 0x7e, -1, 5, o_index4, 1  },
        { "ldsflda", 0x7f, -1, 5, o_index4, 1  },
        { "ldstr", 0x72, -1, 5, o_index4, 1  },
        { "ldtoken", 0xd0, -1, 5, o_index4, 1  },
        { "ldvirtftn", 0xfe, 0x07, 6, o_index4,1  },
        { "leave", 0xdd, -1, 5, o_rel4, 0  }, // empty eval stack, 
        { "leave.s", 0xde, -1, 2, o_rel1, 0 },  // empty eval stack
        { "localloc", 0xfe, 0x0f, 2, o_single, 0  },
        { "mkrefany", 0xc6, -1, 5, o_index4, 0  },
        { "mul", 0x5a, -1, 1, o_single, -1  },
        { "mul.ovf", 0xd8, -1, 1, o_single, -1 },
        { "mul.ovf.un", 0xd9, -1, 1, o_single, -1 },
        { "neg", 0x65, -1, 1, o_single, 0 },
        { "newarr", 0x8d, -1, 5, o_index4, 0 },
        { "newobj", 0x73, -1, 5, o_index4, 1 },  // - arg list
        { "no.", 0xfe, 0x19, 3, o_immed1, 0 },
        { "nop",0x00, -1,1, o_single, 0 },
        { "not",0x66, -1,1, o_single, 0 },
        { "or",0x60, -1,1, o_single, -1 },
        { "pop",0x26, -1,1, o_single, -1 },
        { "readonly.", 0xfe, 0x1e, 2, o_single, 0},
        { "refanytype",0xfe, 0x1d, 2, o_single, 0 },
        { "refanyval", 0xc2, -1, 5, o_index4, 0 },
        { "rem",0x5d, -1,1, o_single, -1 },
        { "rem.un",0x5e, -1,1, o_single, -1 },
        { "ret",0x2a, -1,1, o_single, 0 },
        { "rethrow",0xfe, 0x1a,2, o_single, 0 },
        { "shl",0x62, -1,1, o_single, -1 },
        { "shr",0x63, -1,1, o_single, -1 },
        { "shr.un",0x64, -1,1, o_single, -1 },
        { "sizeof",0xfe, 0x1c,6, o_index4, 1 },
        { "starg", 0xfe, 0x0b, 4, o_index2,-1 },
        { "starg.s",0x10, -1,2, o_index1,-1 },
        { "stelem", 0xa4, -1, 5, o_index4, -3 },
        { "stelem.i",0x9b, -1,1, o_single, -3 },
        { "stelem.i1",0x9c, -1,1, o_single, -3 },
        { "stelem.i2",0x9d, -1,1, o_single, -3 },
        { "stelem.i4",0x9e, -1,1, o_single, -3 },
        { "stelem.i8",0x9f, -1,1, o_single, -3 },
        { "stelem.r4",0xa0, -1,1, o_single, -3 },
        { "stelem.r8",0xa1, -1,1, o_single, -3 },
        { "stelem.ref",0xa2, -1,1, o_single, -3 },
        { "stfld",0x7d, -1,5, o_index4, -2 },
        { "stind.i",0xdf, -1,1, o_single, -2 },
        { "stind.i1",0x52, -1,1, o_single, -2 },
        { "stind.i2",0x53, -1,1, o_single, -2 },
        { "stind.i4",0x54, -1,1, o_single, -2 },
        { "stind.i8",0x55, -1,1, o_single, -2 },
        { "stind.r4",0x56, -1,1, o_single, -2 },
        { "stind.r8",0x57, -1,1, o_single, -2 },
        { "stind.ref",0x51, -1,1, o_single, -2 },
        { "stloc",0xfe, 0xe,4, o_index2, -1 },
        { "stloc.0",0x0a, -1,1, o_single, -1 },
        { "stloc.1",0x0b, -1,1, o_single, -1 },
        { "stloc.2",0x0c, -1,1, o_single, -1 },
        { "stloc.3",0x0d, -1,1, o_single, -1 },
        { "stloc.s",0x13, -1,2, o_index1, -1 },
        { "stobj",0x81, -1,5, o_index4, -2 },
        { "stsfld",0x80, -1,5, o_index4, -1 },
        { "sub",0x59, -1,1, o_single, -1 },
        { "sub.ovf",0xda, -1,1, o_single, -1 },
        { "sub.ovf.un",0xdb, -1,1, o_single, -1 },
        { "switch",0x45, -1, 0, o_switch, -1 },
        { "tail.",0xfe,0x14, 2, o_single, 0 },
        { "throw",0x7a, -1,1, o_single, 1  },
        { "unaligned.",0xfe, 12,3, o_single, 0 },
        { "unbox",0x79, -1,5, o_index4, 0 },
        { "unbox.any",0xa5, -1,5, o_index4, 0 },
        { "volatile.",0xfe, 13,2, o_single, 0 },
        { "xor",0x61, -1,1, o_single, -1 }
    };
    Instruction::Instruction(iop Op, Operand *Oper) : op_(Op), switches_(nullptr), live_(false)
    {
        switches_ = new std::list<std::string>();
        operand_ = Oper;
    }
    void Instruction::NullOperand(Allocator &allocator)
    {
        operand_ = allocator.AllocateOperand();
    }
    int Instruction::InstructionSize()
    {
        if (op_ == i_switch)
        {
            return 1 + 4 + (switches_ ? switches_->size() * 4 : 0);
        }
        else
        {
            return instructions_[op_].bytes;
        }
    }
    int Instruction::StackUsage()
    {
        switch (op_)
        {
        case i_SEH:
            if (sehBegin_ && (sehType_ == seh_filter  || sehType_ == seh_filter_handler || sehType_ == seh_catch))
                return 1; // for the object
            return 0;
        case i_leave:
        case i_leave_s:
        case i_endfinally:
        case i_endfault:
            return 0; // at this point the eval stack should be empty
        case i_endfilter:
            return -1; // after this pop the eval stack should be empty
        case i_call:
        case i_calli:
        case i_callvirt:
        case i_newobj:
        {
            MethodSignature *sig = ((MethodName *)operand_->GetValue())->Signature();
            int n = sig->ReturnType()->IsVoid() ? 0 : 1;
            n -= sig->ParamCount() + sig->VarargParamCount();
            if (op_ != i_newobj && (sig->Flags() & MethodSignature::InstanceFlag))
                n--;
            return n + instructions_[op_].stackUsage;
        }
        default:
            return instructions_[op_].stackUsage;
        }
    }
    void Instruction::AddCaseLabel(std::string label)
    {
        if (!switches_)
            switches_ = new std::list<std::string>();
        switches_->push_back(label);
    }
    bool Instruction::ILSrcDump(PELib &peLib) const
    {
        if (op_ == i_SEH)
        {
            if (sehBegin_)
            {
                switch (sehType_)
                {
                    case seh_try:
                        peLib.Out() << ".try {" << std::endl;
                        break;
                    case seh_catch:
                        peLib.Out() << "catch ";
                        if (sehCatchType_)
                            sehCatchType_->ILSrcDump(peLib);
                        else
                            peLib.Out() << " [mscorlib]System.Object";
                        peLib.Out() << " {" << std::endl;
                        break;
                    case seh_filter:
                        peLib.Out() << "filter {" << std::endl;
                        break;
                    case seh_filter_handler:
                        peLib.Out() << "{" << std::endl;
                        break;
                    case seh_fault:
                        peLib.Out() << "fault {" << std::endl;
                        break;
                    case seh_finally:
                        peLib.Out() << "finally {" << std::endl;
                        break;
                }
            }
            else
            {
                peLib.Out() << "}" << std::endl;
            }

        }
        else if (op_ == i_label)
        {
            peLib.Out() << Label() << ":" << std::endl;
        }
        else if (op_ == i_comment)
        {
            peLib.Out() << "// " << Text() << std::endl;
        }
        else if (op_ == i_switch)
        {
            int i = 0;
            peLib.Out() << "\tswitch (";
            if (switches_)
            {
                for (auto it = switches_->begin(); it != switches_->end(); )
                {
                    peLib.Out() << (*it);
                    ++it;
                    if (it != switches_->end())
                    {
                        peLib.Out() << ", ";
                        if (++i % 8 == 0)
                            peLib.Out() << "\n\t\t";
                    }
                }
            }
            peLib.Out() << "\t)" << std::endl;
        }
        else
        {
            if (operand_)
            {
                peLib.Out() << "\t" << instructions_[op_].name << "\t";
                operand_->ILSrcDump(peLib);
                peLib.Out() << std::endl;

            }
            else
            {
                peLib.Out() << "\t" << instructions_[op_].name << std::endl;
            }
        }
        return true;
    }
    void Instruction::ObjOut(PELib &peLib, int pass) const
    {
        peLib.Out() << std::endl << "$ib" << op_ << "," << offset_;
        if (op_ == i_SEH)
        {
            peLib.Out() << "," << sehType_ << "," << sehBegin_;
            if (sehCatchType_)
            {
                peLib.Out() << ",";
                sehCatchType_->ObjOut(peLib, pass);
            }
            else
            {
                peLib.Out() << "@";
            }
        }
        else if (op_ == i_label)
        {
            peLib.Out() << "," << peLib.FormatName(Label());
        }
        else if (op_ == i_comment)
        {
            peLib.Out() << "," << peLib.FormatName(Text());
        }
        else if (op_ == i_switch)
        {
            peLib.Out() << "," << switches_->size();
            for (auto sw : *switches_)
            {
                peLib.Out() << "," << peLib.FormatName(sw);
            }
        }
        else
        {
            if (operand_)
            {
                peLib.Out() << ",";
                operand_->ObjOut(peLib, pass);
            }
            else peLib.Out() << "$$";
            {
            }
        }
        peLib.Out() << std::endl << "$ie";
    }
    Instruction *Instruction::ObjIn(PELib &peLib)
    {
        Instruction *rv = nullptr;
        iop op = (iop)peLib.ObjInt();
        char ch;
        ch = peLib.ObjChar();
        if (ch != ',')
            peLib.ObjError(oe_syntax);
        int offset = peLib.ObjInt();
        ch = peLib.ObjChar();
        if (ch == ',')
        {
            switch(op)
            {
                int sehType;
                bool sehBegin;
                Type *sehCatchType;
                case i_SEH:
                    sehType = peLib.ObjInt();
                    ch = peLib.ObjChar();
                    if (ch != ',')
                        peLib.ObjError(oe_syntax);
                    sehBegin = peLib.ObjInt();
                    ch = peLib.ObjChar();
                    if (ch == ',')
                    {
                        sehCatchType = Type::ObjIn(peLib);
                    }
                    else
                    {
                        sehCatchType = nullptr;
                    }
                    rv = peLib.AllocateInstruction((Instruction::iseh) sehType, sehBegin, sehCatchType);
                    break;
                 case i_label:
                 {
                     std::string lbl = peLib.UnformatName();
                     rv = peLib.AllocateInstruction(op, peLib.AllocateOperand(lbl));
                 }
                     break;
                 case i_comment:
                {
                    std::string text = peLib.UnformatName();
                    rv = peLib.AllocateInstruction(op, text);  
                }
                    break;
                case i_switch:
                {
                    rv = peLib.AllocateInstruction(op);
                    int n = peLib.ObjInt();
                    for (int i=0; i < n; i++)
                    {
                        ch = peLib.ObjChar();
                        if (ch != ',')
                            peLib.ObjError(oe_syntax);
                        std::string text = peLib.UnformatName();
                        rv->AddCaseLabel(text);
                    }
                }
                    break;
                default:
                {
                    Operand *operand = Operand::ObjIn(peLib);
                    rv = peLib.AllocateInstruction(op, operand);
                }
                    break;
            }
        }
        else {
            rv = peLib.AllocateInstruction(op);
            if (ch == '$')
            {
                ch = peLib.ObjChar();
            }
            if (ch != '$')
                peLib.ObjError(oe_syntax);
        }
        if (peLib.ObjEnd() != 'i')
            peLib.ObjError(oe_syntax);
        if (rv)
            rv->Offset(offset);
        return rv;
    }
    size_t Instruction::Render(PELib &peLib, Byte *result, std::map<std::string, Instruction *> &labels)
    {

        int sz = 0;
        if (op_ == i_SEH && sehType_ == seh_catch)
        {
            Byte data[4];
            if (sehCatchType_)
                sehCatchType_->Render(peLib, data);
        }
        if (op_ != i_label && op_ != i_comment && op_ != i_SEH)
        {
            result[sz++] = instructions_[op_].op1;
            if (instructions_[op_].op2 != 0xff)
                result[sz++] = instructions_[op_].op2;
            if (op_ == i_switch)
            {
                *(DWord *)(result + sz) = switches_->size();
                sz += sizeof(DWord);
                int base = sz + offset_ + switches_->size() * sizeof(DWord);
                for (auto it = switches_->begin(); it != switches_->end(); ++it)
                {
                    int n = labels[*it]->offset_;
                    *(DWord *)(result + sz) = n - base;
                    sz += sizeof(DWord);
                }
            }
            else if (IsBranch())
            {
                std::string branchLabel = operand_->StringValue();
                int cur = offset_ + 1 + (IsRel4() ? 4 : 1); // calculate source
                int n = labels[branchLabel]->offset_ - cur; // calculate offset to target
                if (IsRel4())
                {
                    *(int *)(result + sz) = n;
                    sz += 4;
                }
                else
                {
                    *(Byte *)(result + sz) = n;
                    sz += 1;
                }
            }
            else
            {
                if (operand_ && instructions_[op_].operandType != o_single)
                {
                    sz += operand_->Render(peLib, op_, instructions_[op_].operandType, result + sz);
                }
            }
        }
        return sz;
    }
}
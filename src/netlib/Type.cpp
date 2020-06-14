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

#include "DotNetPELib.h"
#include "PEFile.h"
#include <stdio.h>
namespace DotNetPELib
{

const char* Type::typeNames_[] = {"",        "",        "", "", "void",   "bool",       "char",
                                  "int8",    "uint8",   "int16",  "uint16",     "int32",
                                  "uint32",  "int64",   "uint64", "native int", "native unsigned int",
                                  "float32", "float64", "object", "string"};
const char* BoxedType::typeNames_[] = {"",        "",       "",       "", "", "Bool",   "Char",  "SByte",  "Byte",
                                       "Int16",   "UInt16", "Int32",  "UInt32", "Int64", "UInt64", "IntPtr",
                                       "UIntPtr", "Single", "Double", "Object", "String"};
bool Type::Matches(Type* right)
{
    if (tp_ != right->tp_)
        return false;
    if (arrayLevel_ != right->arrayLevel_)
        return false;
    if (pointerLevel_ != right->pointerLevel_)
        return false;
    if (byRef_ != right->byRef_)
        return false;
    if (tp_ == cls && typeRef_ != right->typeRef_)
    {
        int n1, n2;
        n1 = typeRef_->Name().find("_empty");
        n2 = right->typeRef_->Name().find("_empty");
        if (n1 != std::string::npos || n2 != std::string::npos)
        {
            bool transfer = false;
            if (n1 == std::string::npos)
            {
                n1 = typeRef_->Name().find("_array_");
            }
            else
            {
                transfer = true;
                n2 = right->typeRef_->Name().find("_array_");
            }
            if (n1 != n2)
                return false;
            if (typeRef_->Name().substr(0, n1) != right->typeRef_->Name().substr(0, n2))
                return false;
            if (transfer)
                typeRef_ = right->typeRef_;
        }
        else
            return false;
    }
    if (tp_ == method && methodRef_ != right->methodRef_)
        return false;
    return true;
}
bool Type::ILSrcDump(PELib& peLib) const
{
    if (tp_ == cls)
    {
        if (showType_)
        {
            if (typeRef_->Flags().Flags() & Qualifiers::Value)
            {
                peLib.Out() << " valuetype ";
            }
            else
            {
                peLib.Out() << " class ";
            }
        }
        std::string name = Qualifiers::GetName("", typeRef_, true);
        if (name[0] != '[')
        {
            peLib.Out() << "'" << name << "'";
            peLib.Out() << static_cast<Class*>(typeRef_)->AdornGenerics(peLib);
        }
        else
        {
            int npos = name.find_first_of("]");
            if (npos != std::string::npos && npos != name.size() - 1)
            {
                peLib.Out() << name.substr(0, npos + 1) + "'" + name.substr(npos + 1) + "'";
                peLib.Out() << static_cast<Class*>(typeRef_)->AdornGenerics(peLib);
            }
            else
            {
                peLib.Out() << "'" << name << "'";
            }
        }
    }
    else if (tp_ == var)
    {
        peLib.Out() << "!" << VarNum();
    }
    else if (tp_ == mvar)
    {
        peLib.Out() << "!!" << VarNum();
    }
    else if (tp_ == method)
    {
        peLib.Out() << "method ";
        methodRef_->ILSrcDump(peLib, false, true, true);
    }
    else
    {
        peLib.Out() << typeNames_[tp_];
    }
    if (arrayLevel_ == 1)
    {
        peLib.Out() << " []";
    }
    else if (arrayLevel_)
    {
        peLib.Out() << " [";
        for (int i = 0; i < arrayLevel_; i++)
        {
            if (i != 0)
                peLib.Out() << ", 0...";
            else
                peLib.Out() << "0...";
        }
        peLib.Out() << "]";
    }
    for (int i = 0; i < pointerLevel_; i++)
        peLib.Out() << " *";
    if (byRef_)
        peLib.Out() << "&";
    if (pinned_)
        peLib.Out() << " pinned";
    return true;
}
void Type::ObjOut(PELib& peLib, int pass) const
{
    peLib.Out() << std::endl << "$tb" << tp_ << "," << byRef_ << "," << arrayLevel_ << "," << pointerLevel_ << "," << pinned_ << "," << showType_;
    if (tp_ == cls)
    {
        typeRef_->ObjOut(peLib, -1);
    }
    else if (tp_ == method)
    {
        methodRef_->ObjOut(peLib, -1);
    }
    peLib.Out() << std::endl << "$te";
}
Type* Type::ObjIn(PELib& peLib)
{
    if (peLib.ObjBegin() == 'B')
    {
        Type* rv = BoxedType::ObjIn(peLib);
        if (peLib.ObjEnd() != 'B')
            peLib.ObjError(oe_syntax);
        return rv;
    }
    else if (peLib.ObjBegin(false) == 't')
    {

        BasicType tp = (BasicType)peLib.ObjInt();
        char ch;
        ch = peLib.ObjChar();
        if (ch != ',')
            peLib.ObjError(oe_syntax);
        int byRef = peLib.ObjInt();
        ch = peLib.ObjChar();
        if (ch != ',')
            peLib.ObjError(oe_syntax);
        int arrayLevel = peLib.ObjInt();
        ch = peLib.ObjChar();
        if (ch != ',')
            peLib.ObjError(oe_syntax);
        int pointerLevel = peLib.ObjInt();
        ch = peLib.ObjChar();
        if (ch != ',')
            peLib.ObjError(oe_syntax);
        int pinned = peLib.ObjInt();
        ch = peLib.ObjChar();
        if (ch != ',')
            peLib.ObjError(oe_syntax);
        int showType = peLib.ObjInt();
        Type* rv = nullptr;
        if (tp == cls)
        {
            std::deque<Type*> generics;
            DataContainer* typeref = nullptr;
            if (peLib.ObjBegin() == 'c')
            {
                typeref = Class::ObjIn(peLib, false);
            }
            else if (peLib.ObjBegin(false) == 'E')
            {
                typeref = Enum::ObjIn(peLib, false);
            }
            else
            {
                peLib.ObjError(oe_syntax);
            }
            rv = peLib.AllocateType(typeref);
        }
        else if (tp == method)
        {
            MethodSignature* methodRef = MethodSignature::ObjIn(peLib, nullptr);
            rv = peLib.AllocateType(methodRef);
        }
        else
        {
            rv = peLib.AllocateType(tp, 0);
        }
        if (showType)
            rv->ShowType();
        rv->Pinned(pinned);
        rv->PointerLevel(pointerLevel);
        rv->ArrayLevel(arrayLevel);
        rv->ByRef(byRef);
        if (peLib.ObjEnd() != 't')
            peLib.ObjError(oe_syntax);
        return rv;
    }
    else
    {
        peLib.ObjError(oe_syntax);
    }
    return nullptr;
}
size_t Type::Render(PELib& peLib, Byte* result)
{
    switch (tp_)
    {
        case cls:
            if (typeRef_->InAssemblyRef())
            {
                typeRef_->PEDump(peLib);
                *(int*)result = typeRef_->PEIndex() | (tTypeRef << 24);
            }
            else
            {
                if (showType_)
                {
                    if (!peIndex_)
                    {
                        size_t sz;
                        Byte* sig = SignatureGenerator::TypeSig(this, sz);
                        size_t signature = peLib.PEOut().HashBlob(sig, sz);
                        delete[] sig;
                        TypeSpecTableEntry* table = new TypeSpecTableEntry(signature);
                        peIndex_ = peLib.PEOut().AddTableEntry(table);
                    }
                    *(int*)result = peIndex_ | (tTypeSpec << 24);
                }
                else
                {
                    *(int*)result = typeRef_->PEIndex() | (tTypeDef << 24);
                }
            }
            return 4;
            break;
        case method:
        default:
        {
            if (!peIndex_)
            {
                // if rendering a method as a type we are always going to put the sig
                // in the type spec table
                size_t sz;
                Byte* sig = SignatureGenerator::TypeSig(this, sz);
                size_t signature = peLib.PEOut().HashBlob(sig, sz);
                delete[] sig;
                TypeSpecTableEntry* table = new TypeSpecTableEntry(signature);
                peIndex_ = peLib.PEOut().AddTableEntry(table);
            }
            *(int*)result = peIndex_ | (tTypeSpec << 24);
            return 4;
        }
        break;
    }
    return true;
}
bool BoxedType::ILSrcDump(PELib& peLib) const
{
    // no point in looking up the type name in the assembly for this...
    peLib.Out() << "[mscorlib]System." << typeNames_[tp_];
    return true;
}
void BoxedType::ObjOut(PELib& peLib, int pass) const { peLib.Out() << std::endl << "$Bb" << tp_ << std::endl << "$Be"; }
BoxedType* BoxedType::ObjIn(PELib& peLib)
{
    Type::BasicType type = (Type::BasicType)peLib.ObjInt();
    return peLib.AllocateBoxedType(type);
}
size_t BoxedType::Render(PELib& peLib, Byte* result)
{
    if (!peIndex_)
    {
        size_t system = peLib.PEOut().SystemName();
        size_t name = peLib.PEOut().HashString(typeNames_[tp_]);
        AssemblyDef* assembly = peLib.MSCorLibAssembly();
        void* result = nullptr;
        peLib.Find(std::string("System.") + typeNames_[tp_], &result, nullptr, assembly);
        if (result)
        {
            static_cast<Class*>(result)->PEDump(peLib);
            peIndex_ = static_cast<Class*>(result)->PEIndex();
        }
    }
    *(int*)result = peIndex_ | (tTypeRef << 24);
    return 4;
}
}  // namespace DotNetPELib

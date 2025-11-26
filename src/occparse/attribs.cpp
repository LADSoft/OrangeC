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

#include <stack>
#include <functional>
#include "compiler.h"
#include "config.h"
#include "lex.h"
#include "ccerr.h"
#include "types.h"
#include "SymbolManager.h"
#include "help.h"
#include "ListFactory.h"
#include "memory.h"
#include "expr.h"
#include "init.h"
#include "initbackend.h"
#include "constopt.h"
#include "namespace.h"
#include "beinterf.h"

namespace Parser
{
    attributes basisAttribs;

    Type* AttributeFinish(SYMBOL* sym, Type* tp)
    {
        sym->sb->attribs = basisAttribs;
        tp->BaseType()->alignment = sym->sb->attribs.inheritable.structAlign;
        // should come first to overwrite all other attributes
        if (sym->sb->attribs.uninheritable.copyFrom)
        {
            if (sym->tp->IsFunction() == sym->sb->attribs.uninheritable.copyFrom->tp->IsFunction() &&
                istype(sym) == istype(sym->sb->attribs.uninheritable.copyFrom))
            {
                sym->sb->attribs.inheritable = sym->sb->attribs.uninheritable.copyFrom->sb->attribs.inheritable;
            }
            else
            {
                error(ERR_INVALID_ATTRIBUTE_COPY);
            }
        }
        if (sym->sb->attribs.inheritable.vectorSize)
        {
            if (tp->IsArithmetic())
            {
                int n = sym->sb->attribs.inheritable.vectorSize % tp->size;
                int m = sym->sb->attribs.inheritable.vectorSize / tp->size;
                if (n || m > 0x10000 || (m & (m - 1)) != 0)
                    error(ERR_INVALID_VECTOR_SIZE);
                tp = Type::MakeType(BasicType::pointer_, tp);
                tp->size = sym->sb->attribs.inheritable.vectorSize;
                tp->array = true;
            }
            else
            {
                error(ERR_INVALID_VECTOR_TYPE);
            }
        }
        if (sym->sb->attribs.inheritable.cleanup && sym->sb->storage_class == StorageClass::auto_)
        {
            CallSite* fc = Allocate<CallSite>();
            fc->arguments = argumentListFactory.CreateList();
            auto arg = Allocate<Argument>();
            arg->tp = &stdpointer;
            arg->exp = MakeExpression(ExpressionNode::auto_, sym);
            fc->arguments->push_back(arg);
            fc->ascall = true;
            fc->functp = sym->sb->attribs.inheritable.cleanup->tp;
            fc->fcall = MakeExpression(ExpressionNode::pc_, sym->sb->attribs.inheritable.cleanup);
            fc->sp = sym->sb->attribs.inheritable.cleanup;
            EXPRESSION* expl = MakeExpression(fc);
            InsertInitializer(&sym->sb->dest, sym->tp, expl, 0, true);
        }
        return tp;
    }
    static const std::unordered_map<std::string, int, StringHash> gccStyleAttribNames = {
        {"alias", 1},    // 1 arg, alias name
        {"aligned", 2},  // arg is alignment; for members only increase unless also packed, otherwise can increase or decrease
        {"warn_if_not_aligned", 3},  // arg is the desired minimum alignment
        {"alloc_size", 4},           // implement by ignoring one or two args
        {"cleanup", 5},              // arg is afunc: similar to a destructor.   Also gets called during exception processing
        //                    { "common", 6 }, // no args, decide whether to support
        //                    { "nocommon", 7 }, // no args, decide whether to support
{"copy", 8},          // one arg, varible/func/type, the two variable kinds must match don't copy alias visibility or weak
{"deprecated", 9},    // zero or one arg, match C++
{"nonstring", 10},    // has no null terminator
{"packed", 11},       // ignore auto-align on this field
//                    { "section", 12 }, // one argument, the section name
//                    { "tls_model", 13 }, // one arg, the model.   Probably shouldn't support
{"unused", 14},       // warning control
{"used", 15},         // warning control
{"vector_size", 16},  // one arg, which must be a power of two multiple of the base size.  implement as fixed-size array
//                    { "visibility", 17 }, // one arg, 'default' ,'hidden', 'internal', 'protected.   don't
//                    support for now as requires linker changes. { "weak", 18 }, // not supporting
{"dllimport", 19},
{"dllexport", 20},
//                    { "selectany", 21 },  // requires linker support
//                    { "shared", 22 },
{"zstring", 23},  // non-gcc, added to support nonstring
{"noreturn", 24},
{"stdcall", 25},
{"always_inline", 26},  // we don't really force inline this is still just a suggestion.   in practice the types of functions
// that get flagged with this will likely always be inlined anyway
{"format", 27},
{"internal_linkage", 28},
{"exclude_from_explicit_instantiation", 29},
{"constructor", 30},
{"destructor", 31},
{"stack_protect", 32},
    };
#define DEFAULT_CONSTRUCTOR_PRIORITY 101
#define DEFAULT_DESTRUCTOR_PRIORITY 101

    void ParseOut___attribute__(SYMBOL* funcsp)
    {
        if (MATCHKW(Keyword::attribute_))
        {
            getsym();
            if (needkw(Keyword::openpa_))
            {
                if (needkw(Keyword::openpa_))
                {
                    if (ISID() || ISKW())
                    {
                        std::string name;
                        if (ISID())
                            name = currentLex->value.s.a;
                        else
                            name = currentLex->kw->name;
                        // get rid of leading and trailing "__" if they both exist
                        if (name.size() >= 5 && name.substr(0, 2) == "__" && name.substr(name.size() - 2, 2) == "__")
                            name = name.substr(2, name.size() - 4);
                        getsym();
                        auto attrib = gccStyleAttribNames.find(name);
                        if (attrib == gccStyleAttribNames.end())
                        {
                            errorstr(ERR_ATTRIBUTE_DOES_NOT_EXIST, name.c_str());
                        }
                        else
                        {
                            switch (attrib->second)
                            {
                            case 1:  // alias
                                if (MATCHKW(Keyword::openpa_))
                                {
                                    getsym();
                                    if (currentLex->type == LexType::l_astr_)
                                    {
                                        char buf[1024];
                                        int i;
                                        Optimizer::SLCHAR* xx = (Optimizer::SLCHAR*)currentLex->value.s.w;
                                        for (i = 0; i < 1024 && i < xx->count; i++)
                                            buf[i] = (char)xx->str[i];
                                        buf[i] = 0;
                                        basisAttribs.uninheritable.alias = litlate(buf);
                                        getsym();
                                    }
                                    needkw(Keyword::closepa_);
                                }
                                break;
                            case 2:  // aligned
                                if (MATCHKW(Keyword::openpa_))
                                {
                                    getsym();
                                    Type* tp = nullptr;
                                    EXPRESSION* exp = nullptr;

                                    optimized_expression(funcsp, nullptr, &tp, &exp, false);
                                    if (!tp || !tp->IsInt())
                                        error(ERR_NEED_INTEGER_TYPE);
                                    else if (!isintconst(exp))
                                        error(ERR_CONSTANT_VALUE_EXPECTED);
                                    basisAttribs.inheritable.structAlign = exp->v.i;
                                    basisAttribs.inheritable.alignedAttribute = true;
                                    needkw(Keyword::closepa_);

                                    if (basisAttribs.inheritable.structAlign > 0x10000 ||
                                        (basisAttribs.inheritable.structAlign & (basisAttribs.inheritable.structAlign - 1)) != 0)
                                        error(ERR_INVALID_ALIGNMENT);
                                }
                                else
                                {
                                    // no argument means use max meaningful size
                                    basisAttribs.inheritable.structAlign =
                                        Optimizer::chosenAssembler->arch->type_align->a_alignedstruct;
                                    basisAttribs.inheritable.alignedAttribute = true;
                                }
                                break;
                            case 3:  // warn_if_not_aligned
                                if (needkw(Keyword::openpa_))
                                {
                                    Type* tp = nullptr;
                                    EXPRESSION* exp = nullptr;

                                    optimized_expression(funcsp, nullptr, &tp, &exp, false);
                                    if (!tp || !tp->IsInt())
                                        error(ERR_NEED_INTEGER_TYPE);
                                    else if (!isintconst(exp))
                                        error(ERR_CONSTANT_VALUE_EXPECTED);
                                    basisAttribs.inheritable.warnAlign = exp->v.i;
                                    needkw(Keyword::closepa_);

                                    if (basisAttribs.inheritable.warnAlign > 0x10000 ||
                                        (basisAttribs.inheritable.warnAlign & (basisAttribs.inheritable.warnAlign - 1)) != 0)
                                        error(ERR_INVALID_ALIGNMENT);
                                }
                                break;
                            case 4:  // alloc_size // doesn't restrict to numbers but maybe should?
                                if (needkw(Keyword::openpa_))
                                {
                                    errskim(skim_comma);
                                    if (MATCHKW(Keyword::comma_))
                                    {
                                        getsym();
                                        errskim(skim_closepa);
                                    }
                                    needkw(Keyword::closepa_);
                                }
                                break;
                            case 5:  // cleanup - needs work, should be in the C++ exception table for the function...
                                if (MATCHKW(Keyword::openpa_))
                                {
                                    getsym();
                                    if (ISID())
                                    {
                                        SYMBOL* sym = gsearch(currentLex->value.s.a);
                                        if (sym)
                                        {
                                            if (sym->tp->type == BasicType::aggregate_)
                                                if (sym->tp->BaseType()->syms->size() == 1)
                                                    sym = sym->tp->BaseType()->syms->front();
                                            if (sym->tp->IsFunction() && sym->tp->BaseType()->btp->IsVoid())
                                            {
                                                if (sym->tp->BaseType()->syms->size() == 0)
                                                {
                                                    basisAttribs.inheritable.cleanup = sym;
                                                }
                                                else if (sym->tp->BaseType()->syms->size() == 1)
                                                {
                                                    auto sp = sym->tp->BaseType()->syms->front();
                                                    if (sp->tp->IsVoid() || sp->tp->IsVoidPtr())
                                                        basisAttribs.inheritable.cleanup = sym;
                                                    else
                                                        error(ERR_INVALID_ATTRIBUTE_CLEANUP);
                                                }
                                                else
                                                {
                                                    error(ERR_INVALID_ATTRIBUTE_CLEANUP);
                                                }
                                            }
                                            else
                                            {
                                                error(ERR_INVALID_ATTRIBUTE_CLEANUP);
                                            }
                                        }
                                        else
                                        {
                                            errorstr(ERR_UNDEFINED_IDENTIFIER, currentLex->value.s.a);
                                        }
                                        getsym();
                                    }
                                    else
                                    {
                                        error(ERR_IDENTIFIER_EXPECTED);
                                    }
                                    needkw(Keyword::closepa_);
                                }
                                break;
                            case 8:  // copy
                                if (MATCHKW(Keyword::openpa_))
                                {
                                    getsym();
                                    if (ISID())
                                    {
                                        SYMBOL* sym = gsearch(currentLex->value.s.a);
                                        if (sym)
                                        {
                                            basisAttribs.uninheritable.copyFrom = sym;
                                        }
                                        else
                                        {
                                            errorstr(ERR_UNDEFINED_IDENTIFIER, currentLex->value.s.a);
                                        }
                                        getsym();
                                    }
                                    else
                                    {
                                        error(ERR_IDENTIFIER_EXPECTED);
                                    }
                                    needkw(Keyword::closepa_);
                                }
                                break;
                            case 9:  // deprecated
                                basisAttribs.uninheritable.deprecationText = (const char*)-1;
                                if (MATCHKW(Keyword::openpa_))
                                {
                                    getsym();
                                    if (currentLex->type == LexType::l_astr_)
                                    {
                                        char buf[1024];
                                        int i;
                                        Optimizer::SLCHAR* xx = (Optimizer::SLCHAR*)currentLex->value.s.w;
                                        for (i = 0; i < 1024 && i < xx->count; i++)
                                            buf[i] = (char)xx->str[i];
                                        buf[i] = 0;
                                        basisAttribs.uninheritable.deprecationText = litlate(buf);
                                        getsym();
                                    }
                                    needkw(Keyword::closepa_);
                                }
                                break;
                            case 10:  // nonstring
                                basisAttribs.inheritable.nonstring = true;
                                break;
                            case 11:  // packed
                                basisAttribs.inheritable.packed = true;
                                break;
                            case 14:  // unused
                                basisAttribs.inheritable.used = true;
                                break;
                            case 15:  // used - this forces emission of static variables.  Since we always emit it is a noop
                                break;
                            case 16:  // vector_size
                                if (needkw(Keyword::openpa_))
                                {
                                    Type* tp = nullptr;
                                    EXPRESSION* exp = nullptr;

                                    optimized_expression(funcsp, nullptr, &tp, &exp, false);
                                    if (!tp || !tp->IsInt())
                                        error(ERR_NEED_INTEGER_TYPE);
                                    else if (!isintconst(exp))
                                        error(ERR_CONSTANT_VALUE_EXPECTED);
                                    basisAttribs.inheritable.vectorSize = exp->v.i;
                                    needkw(Keyword::closepa_);
                                }
                                break;
                            case 19:  // dllimport
                                if (basisAttribs.inheritable.linkage2 != Linkage::none_)
                                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                                basisAttribs.inheritable.linkage2 = Linkage::import_;
                                break;
                            case 20:  // dllexport
                                if (basisAttribs.inheritable.linkage2 != Linkage::none_)
                                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                                basisAttribs.inheritable.linkage2 = Linkage::export_;
                                break;
                            case 23:  // zstring
                                basisAttribs.inheritable.zstring = true;
                                break;
                            case 24:
                                if (basisAttribs.inheritable.linkage3 != Linkage::none_)
                                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                                basisAttribs.inheritable.linkage3 = Linkage::noreturn_;
                                break;
                            case 25:  // stdcall
                                if (basisAttribs.inheritable.linkage != Linkage::none_)
                                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                                basisAttribs.inheritable.linkage = Linkage::stdcall_;
                                break;
                            case 26:  // always inline
                                basisAttribs.inheritable.isInline = true;
                                break;
                            case 27:  // format
                                needkw(Keyword::openpa_);
                                while (currentLex && !MATCHKW(Keyword::closepa_))
                                    getsym();
                                if (currentLex)
                                    getsym();
                                break;
                            case 28:  // internal_linkage
                                basisAttribs.inheritable.linkage2 = Linkage::internal_;
                                break;
                            case 29:  // exclude_from_explicit_instantiation
                                basisAttribs.inheritable.excludeFromExplicitInstantiation = true;
                                break;
                            case 30:  // constructor
                                if (MATCHKW(Keyword::openpa_))
                                {
                                    getsym();
                                    Type* tp = nullptr;
                                    EXPRESSION* exp = nullptr;

                                    optimized_expression(funcsp, nullptr, &tp, &exp, false);
                                    if (!tp || !tp->IsInt())
                                        error(ERR_NEED_INTEGER_TYPE);
                                    else if (!isintconst(exp))
                                        error(ERR_CONSTANT_VALUE_EXPECTED);
                                    basisAttribs.uninheritable.constructorPriority = exp->v.i;
                                    basisAttribs.inheritable.warnAlign = exp->v.i;
                                    needkw(Keyword::closepa_);
                                }
                                else
                                {
                                    basisAttribs.uninheritable.constructorPriority = DEFAULT_CONSTRUCTOR_PRIORITY;
                                }
                                break;
                            case 31:  // destructor
                                if (MATCHKW(Keyword::openpa_))
                                {
                                    getsym();
                                    Type* tp = nullptr;
                                    EXPRESSION* exp = nullptr;

                                    optimized_expression(funcsp, nullptr, &tp, &exp, false);
                                    if (!tp || !tp->IsInt())
                                        error(ERR_NEED_INTEGER_TYPE);
                                    else if (!isintconst(exp))
                                        error(ERR_CONSTANT_VALUE_EXPECTED);
                                    basisAttribs.uninheritable.destructorPriority = exp->v.i;
                                    basisAttribs.inheritable.warnAlign = exp->v.i;
                                    needkw(Keyword::closepa_);
                                }
                                else
                                {
                                    basisAttribs.uninheritable.destructorPriority = DEFAULT_DESTRUCTOR_PRIORITY;
                                }
                                break;
                            case 32:  // stack-protect explicit
                                basisAttribs.uninheritable.stackProtect = true;
                                break;
                            }
                        }
                    }
                    needkw(Keyword::closepa_);
                    needkw(Keyword::closepa_);
                }
                else
                {
                    errskim(skim_closepa);
                    skip(Keyword::closepa_);
                    error(ERR_IGNORING__ATTRIBUTE);
                }
            }
        }
    }
    static const std::unordered_map<std::string, int, StringHash> occCPPStyleAttribNames = {
        {"zstring", 23},  // non-gcc, added to support nonstring
    };
    static const std::unordered_map<std::string, int, StringHash> clangCPPStyleAttribNames = {
        {"internal_linkage", 28},
        {"exclude_from_explicit_instantiation", 29},
    };
    static const std::unordered_map<std::string, int, StringHash> gccCPPStyleAttribNames = {
        {"alloc_size", 4},  // implement by ignoring one or two args
        //                    { "common", 6 }, // no args, decide whether to support
        //                    { "nocommon", 7 }, // no args, decide whether to support
        //                    { "section", 12 }, // one argument, the section name
//                    { "tls_model", 13 }, // one arg, the model.   Probably shouldn't
//                    support { "visibility", 17 }, // one arg, 'default' ,'hidden',
//                    'internal', 'protected.   don't support for now as requires linker
//                    changes. { "weak", 18 }, // not supporting { "selectany", 21 }, //
//                    requires linker support { "shared", 22 },
{"dllexport", 25},
{"dllimport", 26},
{"stdcall", 27} };
    std::string StripUnderscores(std::string str)
    {
        size_t len = str.length();
        if (str[0] == '_' && str[1] == '_' && str[len - 2] == '_' && str[len - 1] == '_')
        {
            return str.substr(2, str.length() - 4);
        }
        return str;
    }
    static void balancedAttributeParameter()
    {
        Keyword start = KW();
        Keyword endp = (Keyword)-1;
        getsym();
        switch (start)
        {
        case Keyword::openpa_:
            endp = Keyword::closepa_;
            break;
        case Keyword::begin_:
            endp = Keyword::end_;
            break;
        case Keyword::openbr_:
            endp = Keyword::closebr_;
            break;
        default:
            break;
        }
        if (endp != (Keyword)-1)
        {
            while (currentLex && !MATCHKW(endp))
                balancedAttributeParameter();
            needkw(endp);
        }
    }
    bool ParseAttributeSpecifiers(SYMBOL* funcsp, bool always)
    {
        (void)always;
        bool rv = false;
        while (MATCHKW(Keyword::alignas_) || MATCHKW(Keyword::attribute_) || MATCHKW(Keyword::openbr_))
        {
            if (MATCHKW(Keyword::attribute_))
            {
                ParseOut___attribute__(funcsp);
            }
            else if (MATCHKW(Keyword::alignas_))
            {
                RequiresDialect::Keyword(Dialect::c23, "alignas");
                rv = true;
                getsym();
                if (needkw(Keyword::openpa_))
                {
                    int align = 1;
                    if (TypeGenerator::StartOfType(nullptr, false))
                    {
                        Type* tp = nullptr;
                        tp = TypeGenerator::TypeId(funcsp, StorageClass::cast_, false, true, false);

                        if (!tp)
                        {
                            error(ERR_TYPE_NAME_EXPECTED);
                        }
                        else if (tp->type == BasicType::templateparam_)
                        {
                            if (tp->templateParam->second->type == TplType::typename_)
                            {
                                if (tp->templateParam->second->packed)
                                {
                                    std::list<TEMPLATEPARAMPAIR>* packs = tp->templateParam->second->byPack.pack;
                                    if (!MATCHKW(Keyword::ellipse_))
                                    {
                                        error(ERR_PACK_SPECIFIER_REQUIRED_HERE);
                                    }
                                    else
                                    {
                                        getsym();
                                    }
                                    if (packs)
                                    {
                                        for (auto&& pack : *packs)
                                        {
                                            int v = getAlign(StorageClass::global_, pack.second->byClass.val);
                                            if (v > align)
                                                align = v;
                                        }
                                    }
                                }
                                else
                                {
                                    // it will only get here while parsing the template...
                                    // when generating the instance the class member will already be
                                    // filled in so it will get to the below...
                                    if (tp->templateParam->second->byClass.val)
                                        align = getAlign(StorageClass::global_, tp->templateParam->second->byClass.val);
                                }
                            }
                        }
                        else
                        {
                            align = getAlign(StorageClass::global_, tp);
                        }
                    }
                    else
                    {
                        Type* tp = nullptr;
                        EXPRESSION* exp = nullptr;

                        optimized_expression(funcsp, nullptr, &tp, &exp, false);
                        if (!tp || !tp->IsInt())
                            error(ERR_NEED_INTEGER_TYPE);
                        else
                        {
                            if (!isintconst(exp))
                            {
                                align = 1;
                                if (exp->type != ExpressionNode::templateparam_)
                                {
                                    error(ERR_CONSTANT_VALUE_EXPECTED);
                                }
                                else if (exp->v.templateParam && exp->v.templateParam->second->byNonType.val)
                                {
                                    exp = exp->v.templateParam->second->byNonType.val;
                                    if (!isintconst(exp))
                                        error(ERR_CONSTANT_VALUE_EXPECTED);
                                    align = exp->v.i;
                                }
                            }
                            else
                            {
                                align = exp->v.i;
                            }
                        }
                    }
                    needkw(Keyword::closepa_);
                    basisAttribs.inheritable.structAlign = align;
                    if (basisAttribs.inheritable.structAlign > 0x10000 ||
                        (basisAttribs.inheritable.structAlign & (basisAttribs.inheritable.structAlign - 1)) != 0)
                        error(ERR_INVALID_ALIGNMENT);
                }
            }
            else if (MATCHKW(Keyword::openbr_))
            {
                getsym();
                if (MATCHKW(Keyword::openbr_))
                {
                    RequiresDialect::Feature(Dialect::c23, "Attribute specifiers");
                    const std::string occNamespace = "occ";
                    const std::string gccNamespace = "gnu";
                    const std::string clangNamespace = "clang";
                    rv = true;
                    getsym();
                    if (!MATCHKW(Keyword::closebr_))
                    {
                        while (currentLex)
                        {
                            bool special = false;
                            if (!ISID())
                            {
                                getsym();
                                error(ERR_IDENTIFIER_EXPECTED);
                            }
                            else
                            {
                                std::string stripped_ver = StripUnderscores((std::string)currentLex->value.s.a);
                                if (stripped_ver == occNamespace)
                                {
                                    getsym();
                                    if (MATCHKW(Keyword::classsel_))
                                    {
                                        getsym();
                                        if (!ISID())
                                        {
                                            getsym();
                                            error(ERR_IDENTIFIER_EXPECTED);
                                        }
                                        else if (currentLex)
                                        {
                                            std::string name = currentLex->value.s.a;
                                            name = StripUnderscores(name);
                                            auto searchedName = occCPPStyleAttribNames.find(name);
                                            if (searchedName != occCPPStyleAttribNames.end())
                                            {
                                                switch (searchedName->second)
                                                {
                                                case 23:
                                                    basisAttribs.inheritable.zstring = true;
                                                    break;
                                                }
                                            }
                                            else
                                            {
                                                errorstr2(ERR_ATTRIBUTE_DOES_NOT_EXIST_IN_NAMESPACE, name.c_str(),
                                                    occNamespace.c_str());
                                            }
                                            getsym();
                                        }
                                    }
                                    else
                                    {
                                        errorstr(ERR_ATTRIBUTE_NAMESPACE_NOT_ATTRIBUTE, occNamespace.c_str());
                                    }
                                }
                                else if (stripped_ver == clangNamespace)
                                {
                                    getsym();
                                    if (MATCHKW(Keyword::classsel_))
                                    {
                                        getsym();
                                        if (!ISID())
                                        {
                                            getsym();
                                            error(ERR_IDENTIFIER_EXPECTED);
                                        }
                                        else if (currentLex)
                                        {
                                            std::string name = currentLex->value.s.a;
                                            name = StripUnderscores(name);
                                            auto searchedName = clangCPPStyleAttribNames.find(name);
                                            if (searchedName != clangCPPStyleAttribNames.end())
                                            {
                                                switch (searchedName->second)
                                                {
                                                case 28:
                                                    basisAttribs.inheritable.linkage2 = Linkage::internal_;
                                                    break;
                                                case 29:
                                                    basisAttribs.inheritable.excludeFromExplicitInstantiation = true;
                                                    break;
                                                }
                                            }
                                            else
                                            {
                                                errorstr2(ERR_ATTRIBUTE_DOES_NOT_EXIST_IN_NAMESPACE, name.c_str(),
                                                    clangNamespace.c_str());
                                            }
                                            getsym();
                                        }
                                    }
                                    else
                                    {
                                        errorstr(ERR_ATTRIBUTE_NAMESPACE_NOT_ATTRIBUTE, clangNamespace.c_str());
                                    }
                                }
                                else if (stripped_ver == gccNamespace)
                                {
                                    getsym();
                                    if (MATCHKW(Keyword::classsel_))
                                    {
                                        getsym();
                                        if (!ISID())
                                        {
                                            getsym();
                                            error(ERR_IDENTIFIER_EXPECTED);
                                        }
                                        else if (currentLex)
                                        {
                                            // note: these are only the namespaced names listed, the ___attribute__ names are
                                            // unlisted here as they don't exist in GCC and we want ours to follow theirs for actual
                                            // consistency reasons.
                                            std::string name = currentLex->value.s.a;
                                            name = StripUnderscores(name);
                                            auto searchedName = gccCPPStyleAttribNames.find(name);
                                            if (searchedName != gccCPPStyleAttribNames.end())
                                            {
                                                switch (searchedName->second)
                                                {
                                                case 25:
                                                    if (basisAttribs.inheritable.linkage2 != Linkage::none_)
                                                        error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                                                    basisAttribs.inheritable.linkage2 = Linkage::export_;
                                                    break;
                                                case 26:
                                                    if (basisAttribs.inheritable.linkage2 != Linkage::none_)
                                                        error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                                                    basisAttribs.inheritable.linkage2 = Linkage::import_;
                                                    break;
                                                case 27:
                                                    if (basisAttribs.inheritable.linkage != Linkage::none_)
                                                        error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                                                    basisAttribs.inheritable.linkage = Linkage::stdcall_;
                                                }
                                            }
                                            else
                                            {
                                                errorstr2(ERR_ATTRIBUTE_DOES_NOT_EXIST_IN_NAMESPACE, name.c_str(),
                                                    gccNamespace.c_str());
                                            }
                                            getsym();
                                        }
                                    }
                                    else
                                    {
                                        errorstr(ERR_ATTRIBUTE_NAMESPACE_NOT_ATTRIBUTE, gccNamespace.c_str());
                                    }
                                }
                                else
                                {

                                    if (stripped_ver == "noreturn")
                                    {
                                        getsym();
                                        special = true;
                                        if (basisAttribs.inheritable.linkage3 != Linkage::none_)
                                            error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                                        basisAttribs.inheritable.linkage3 = Linkage::noreturn_;
                                    }
                                    else if (stripped_ver == "carries_dependency")
                                    {
                                        getsym();
                                        special = true;
                                    }
                                    else if (stripped_ver == "fallthrough")
                                    {
                                        getsym();
                                        basisAttribs.uninheritable.fallthrough = true;
                                    }
                                    else if (stripped_ver == "maybe_unused")
                                    {
                                        getsym();
                                        basisAttribs.uninheritable.maybe_unused = true;
                                    }
                                    else if (stripped_ver == "nodiscard")
                                    {
                                        getsym();
                                        basisAttribs.uninheritable.nodiscard = true;
                                    }
                                    else
                                    {
                                        if (stripped_ver == "deprecated")
                                            basisAttribs.uninheritable.deprecationText = (char*)-1;
                                        getsym();
                                        if (MATCHKW(Keyword::classsel_))
                                        {
                                            getsym();
                                            if (!ISID())
                                                error(ERR_IDENTIFIER_EXPECTED);
                                            getsym();
                                        }
                                    }
                                }
                            }
                            if (MATCHKW(Keyword::openpa_))
                            {
                                if (special)
                                    error(ERR_NO_ATTRIBUTE_ARGUMENT_CLAUSE_HERE);
                                getsym();
                                if (basisAttribs.uninheritable.deprecationText)
                                {
                                    if (currentLex->type == LexType::l_astr_)
                                    {
                                        char buf[1024];
                                        int i;
                                        Optimizer::SLCHAR* xx = (Optimizer::SLCHAR*)currentLex->value.s.w;
                                        for (i = 0; i < 1024 && i < xx->count; i++)
                                            buf[i] = (char)xx->str[i];
                                        buf[i] = 0;
                                        basisAttribs.uninheritable.deprecationText = litlate(buf);
                                        getsym();
                                    }
                                }

                                else
                                    while (currentLex && !MATCHKW(Keyword::closepa_))
                                    {
                                        balancedAttributeParameter();
                                    }
                                needkw(Keyword::closepa_);
                            }
                            if (MATCHKW(Keyword::ellipse_))
                                getsym();
                            if (!MATCHKW(Keyword::comma_))
                                break;
                            getsym();
                        }
                        if (needkw(Keyword::closebr_))
                            needkw(Keyword::closebr_);
                    }
                    else
                    {
                        // empty
                        getsym();
                        needkw(Keyword::closebr_);
                    }
                }
                else
                {
                    --*currentStream;
                    break;
                }
            }
        }
        return rv;
    }

}  // namespace Parser
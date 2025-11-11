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

#include "compiler.h"
#include "config.h"
#include "declare.h"
#include "namespace.h"
#include "lex.h"
#include "class.h"
#include "ccerr.h"
#include "declcpp.h"
#include "ListFactory.h"
#include "templatedecl.h"
#include "templateutil.h"
#include "templateinst.h"
#include "help.h"
#include "memory.h"
#include "initbackend.h"
#include "mangle.h"
#include "symtab.h"
#include "attribs.h"

namespace CompletionCompiler
{
    void ccInsertUsing(Parser::SYMBOL* ns, Parser::SYMBOL* parentns, const char* file, int line);
}

namespace Parser
{
    int parsingUsing;

    static void InsertTag(SYMBOL* sym, StorageClass storage_class, bool allowDups)
    {
        SymbolTable<SYMBOL>* table;
        SYMBOL* ssp = enclosingDeclarations.GetFirst();
        SYMBOL* sp1 = nullptr;
        if (ssp &&
            (storage_class == StorageClass::member_ || storage_class == StorageClass::mutable_ || storage_class == StorageClass::type_))
        {
            table = ssp->tp->tags;
        }
        else if (storage_class == StorageClass::auto_ || storage_class == StorageClass::register_ ||
            storage_class == StorageClass::parameter_ || storage_class == StorageClass::localstatic_)
            table = localNameSpace->front()->tags;
        else
            table = globalNameSpace->front()->tags;
        if (allowDups)
            sp1 = search(table, sym->name);
        if (!allowDups || !sp1 || (sym != sp1 && sym->sb->mainsym && sym->sb->mainsym != sp1->sb->mainsym))
            table->Add(sym);
    }
    void insertUsing(SYMBOL** sp_out, AccessLevel access, StorageClass storage_class, bool inTemplate,
        bool hasAttributes)
    {
        SYMBOL* sp;
        if (MATCHKW(Keyword::namespace_))
        {
            getsym();
            if (ISID())
            {
                // by spec using directives match the current state of
                // the namespace at all times... so we cache pointers to
                // related namespaces
                nestedSearch(&sp, nullptr, nullptr, nullptr, nullptr, false, StorageClass::global_, true, false);
                if (sp)
                {
                    if (sp->sb->storage_class != StorageClass::namespace_ && sp->sb->storage_class != StorageClass::namespace_alias_)
                    {
                        errorsym(ERR_NOT_A_NAMESPACE, sp);
                    }
                    else
                    {
                        bool found = false;
                        if (globalNameSpace->front()->usingDirectives)
                            for (auto t : *globalNameSpace->front()->usingDirectives)
                                if (t == sp)
                                {
                                    found = true;
                                    break;
                                }
                        if (!found)
                        {
                            if (!globalNameSpace->front()->usingDirectives)
                                globalNameSpace->front()->usingDirectives = symListFactory.CreateList();
                            globalNameSpace->front()->usingDirectives->push_front(sp);
                            if (!IsCompiler() && currentLex)
                                CompletionCompiler::ccInsertUsing(sp, nameSpaceList.size() ? nameSpaceList.front() : nullptr,
                                    currentLex->errfile, currentLex->errline);
                        }
                    }
                    getsym();
                }
            }
            else
            {
                error(ERR_EXPECTED_NAMESPACE_NAME);
            }
        }
        else
        {
            bool isTypename = false;
            if (MATCHKW(Keyword::typename_))
            {
                isTypename = true;
                getsym();
            }

            if (hasAttributes)
                error(ERR_NO_ATTRIBUTE_SPECIFIERS_HERE);
            if (!isTypename && ISID())
            {
                auto placeHolder = currentContext->Index();
                getsym();
                attributes oldAttribs = basisAttribs;
                basisAttribs = { 0 };
                ParseAttributeSpecifiers(nullptr, true);
                if (MATCHKW(Keyword::assign_))
                {
                    Type* tp = nullptr;
                    SYMBOL* sp;
                    getsym();
                    std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                    if (inTemplate && (ISID() || MATCHKW(Keyword::classsel_) || MATCHKW(Keyword::typename_)))
                    {
                        SYMBOL* sym = nullptr, * strsym = nullptr;
                        std::list<NAMESPACEVALUEDATA*>* ns = nullptr;
                        bool throughClass = false;
                        parsingTrailingReturnOrUsing++;
                        bool isTemplate = false;
                        id_expression(nullptr, &sym, &strsym, nullptr, &isTemplate, false, false, nullptr, 0, 0);
                        if (sym)
                        {
                            tp = sym->tp;
                            getsym();
                            if (MATCHKW(Keyword::lt_))
                            {
                                GetTemplateArguments(nullptr, sym, &lst);
                            }
                        }
                        else if (strsym && strsym->tp->type == BasicType::templateselector_)
                        {
                            tp = strsym->tp;
                            getsym();
                            if (MATCHKW(Keyword::lt_))
                            {
                                GetTemplateArguments((*strsym->tp->sp->sb->templateSelector)[1].sp, sym, &lst);
                            }
                        }
                        else if (definingTemplate && !instantiatingTemplate && isTemplate && strsym)
                        {
                            while (!MATCHKW(Keyword::classsel_) && !MATCHKW(Keyword::semicolon_))
                            {
                                getsym();
                            }
                            if (MATCHKW(Keyword::classsel_))
                            {
                                getsym();
                                if (MATCHKW(Keyword::template_))
                                {
                                    getsym();
                                }
                            }
                            const char* name = ISID() ? currentLex->value.s.a : "????";
                            getsym();
                            bool hasParams = false;
                            if (MATCHKW(Keyword::lt_))
                            {
                                hasParams = true;
                                GetTemplateArguments(nullptr, sym, &lst);
                            }
                            auto* templateSelector = templateSelectorListFactory.CreateVector();
                            templateSelector->push_back(TEMPLATESELECTOR{});
                            templateSelector->push_back(TEMPLATESELECTOR{});
                            templateSelector->back().sp = strsym;
                            templateSelector->push_back(TEMPLATESELECTOR{});
                            templateSelector->back().name = litlate(name);
                            if (hasParams)
                            {
                                templateSelector->back().isTemplate = true;
                                templateSelector->back().templateParams = lst;
                            }
                            tp = Type::MakeType(BasicType::templateselector_);
                            sp = makeID(StorageClass::global_, tp, nullptr, AnonymousName());
                            sp->sb->templateSelector = templateSelector;
                            tp->sp = sp;
                        }
                        parsingTrailingReturnOrUsing--;
                        if (!isTemplate)
                        {
                            Linkage linkage = Linkage::none_, linkage2 = Linkage::none_, linkage3 = Linkage::none_;
                            bool defd = false;
                            SYMBOL* sp = nullptr;
                            bool notype = false;

                            tp = TypeGenerator::BeforeName(nullptr, tp, &sp, nullptr, nullptr, false, storage_class, &linkage,
                                &linkage2, &linkage3, nullptr, false, false, true,
                                false); /* fixme at file scope init */
                        }
                    }
                    else
                    {
                        parsingUsing++;
                        tp = TypeGenerator::TypeId(nullptr, StorageClass::cast_, false, true, true);
                        parsingUsing--;
                    }
                    if (!tp)
                    {
                        tp = &stdint;
                    }
                    checkauto(tp, ERR_AUTO_NOT_ALLOWED_IN_USING_STATEMENT);
                    sp = makeID(StorageClass::typedef_, tp, nullptr, litlate((*placeHolder)->value.s.a));
                    sp->sb->attribs = basisAttribs;
                    sp->tp = Type::MakeType(BasicType::typedef_, tp);
                    sp->tp->sp = sp;
                    sp->sb->access = access;
                    if (inTemplate)
                    {
                        sp->sb->templateLevel = definingTemplate;
                        sp->templateParams = TemplateGetParams(sp);
                        if (tp->IsStructured() || tp->BaseType()->type == BasicType::templateselector_)
                            sp->sb->typeAlias = lst;
                    }
                    else if (sp->tp->IsDeferred() && (!definingTemplate || instantiatingTemplate))
                    {
                        if (!sp->tp->BaseType()->sp->sb->templateLevel)
                            sp->tp->InstantiateDeferred();
                    }
                    if (storage_class == StorageClass::member_)
                        sp->sb->parentClass = enclosingDeclarations.GetFirst();
                    sp->sb->usingTypedef = true;
                    SetLinkerNames(sp, Linkage::cdecl_);
                    // note that we can redeclare a using statement...
                    // so look it up and if it exists make sure the types match...
                    SYMBOL* spi;
                    if (storage_class == StorageClass::auto_ || storage_class == StorageClass::parameter_)
                    {
                        spi = localNameSpace->front()->syms->Lookup(sp->name);
                    }
                    else
                    {
                        auto ssp = enclosingDeclarations.GetFirst();
                        if (ssp && ssp->sb->templateLevel)
                        {
                            SYMBOL* ssp2 = FindSpecialization(ssp, ssp->templateParams);
                            if (ssp2)
                                ssp = ssp2;
                        }
                        if (ssp && ssp->tp->syms)
                        {
                            spi = ssp->tp->syms->Lookup(sp->name);
                        }
                        else
                        {
                            spi = globalNameSpace->front()->syms->Lookup(sp->name);
                        }
                    }
                    if (spi)
                    {
                        if (!spi->tp->CompatibleType(sp->tp) && !SameTemplate(spi->tp, sp->tp))
                        {
                            errorsym(ERR_TYPE_MISMATCH_IN_REDECLARATION, sp);
                        }
                        sp = spi;
                    }
                    else
                    {
                        sp->sb->parentClass = enclosingDeclarations.GetFirst();
                        InsertSymbol(sp, storage_class, Linkage::cdecl_, false);
                    }
                    if (sp_out)
                        *sp_out = sp;
                    basisAttribs = oldAttribs;
                    return;
                }
                else
                {
                    BackupTokenStream();
                }
                basisAttribs = oldAttribs;
            }
            // if we get here we suppose it is classic using, e.g. import the type or variable from another context...
            std::list<NAMESPACEVALUEDATA*>* nssym = globalNameSpace;
            enum Mode
            {
                parseFirst,
                parseClass,
                parseNamespace
            } mode = parseFirst;
            if (MATCHKW(Keyword::classsel_))
            {
                nssym = rootNameSpace;
                mode = parseNamespace;
                getsym();
            }
            // there is an extension to C++17 that allows packed variables to be used as part of the pack,
            // then the using is applied to all elements of the path.   So we have to gather the path independently of
            // evaluating it...
            std::list<std::pair<std::string, LexToken::iterator>> path;
            while (true)
            {
                if (!ISID() && !MATCHKW(Keyword::operator_))
                {
                    error(ERR_IDENTIFIER_EXPECTED);
                    errskim(skim_semi, false);
                    break;
                }
                char buf[512];
                int ov;
                Type* castType;
                getIdName(theCurrentFunc, buf, sizeof(buf), &ov, &castType);
                getsym();
                LexToken::iterator placeHolder = currentContext->end();;
                if (MATCHKW(Keyword::lt_))
                {
                    placeHolder = currentContext->Index();
                    std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                    GetTemplateArguments(theCurrentFunc, nullptr, &lst);
                }
                path.push_back(std::pair<std::string, LexToken::iterator>(buf, placeHolder));
                if (!MATCHKW(Keyword::classsel_))
                {
                    break;
                }
                getsym();
            }
            bool ellipsis = false;
            if (MATCHKW(Keyword::ellipse_))
            {
                ellipsis = true;
                getsym();
                RequiresDialect::Feature(Dialect::cpp17, "Packed Using Extension");
            }
            SYMBOL* sp = nullptr, * strSym = nullptr;
            bool done = false;
            bool absorbedPack = false;
            std::stack<std::tuple<decltype(path)::iterator, int, int, const char*, SYMBOL*, Mode>> stk;
            const char* lastName = nullptr, * lastRealName = nullptr;
            auto it = path.begin();
            while (!done)
            {
                done = true;
                bool skipProcess = false;
                for (; it != path.end();)
                {
                    const char* buf = it->first.c_str();
                    if (lastName && !strcmp(lastName, buf))
                    {
                        buf = overloadNameTab[CI_CONSTRUCTOR];
                    }
                    else if (lastRealName && !strcmp(lastRealName, buf))
                    {
                        buf = overloadNameTab[CI_CONSTRUCTOR];
                    }
                    strSym = sp;
                    switch (mode)
                    {
                    case parseFirst:
                        sp = namespacesearch(buf, localNameSpace, false, false);
                        if (!sp)
                            sp = classsearch(buf, false, MATCHKW(Keyword::classsel_), false);
                        if (!sp)
                            sp = namespacesearch(buf, globalNameSpace, false, false);
                        break;
                    case parseNamespace:
                        sp = namespacesearch(buf, nssym, true, false);
                        break;
                    case parseClass: {
                        enclosingDeclarations.Add(strSym);
                        sp = classsearch(buf, false, MATCHKW(Keyword::classsel_), false);
                        enclosingDeclarations.Drop();
                        if (!sp)
                            sp = classsearch(buf, false, MATCHKW(Keyword::classsel_), false);
                    }
                                   break;
                    }
                    if (sp)
                    {
                        auto spin = sp;
                        if (sp->tp->type == BasicType::templateparam_ && sp->tp->templateParam->second->packed &&
                            sp->tp->templateParam->second->type == TplType::typename_ && !stk.empty() &&
                            std::get<0>(stk.top())->first == sp->name)
                        {
                            int n = std::get<1>(stk.top());
                            auto it1 = sp->tp->templateParam->second->byPack.pack->begin();
                            for (; it1 != sp->tp->templateParam->second->byPack.pack->end() && n--; ++it1)
                                ;
                            if (it1 == sp->tp->templateParam->second->byPack.pack->end())
                            {
                                // kind of a hokey error but this *should* never happen
                                error(ERR_PACKED_TEMPLATE_TYPE_MISMATCH);
                                return;
                            }
                            else
                            {
                                auto tp1 = (*it1).second->byClass.val;
                                if (!tp1->IsStructured())
                                {
                                    error(ERR_NEED_PACKED_TEMPLATE_OF_TYPE_CLASS);
                                    return;
                                }
                                auto sp1 = tp1->BaseType()->sp;
                                sp = sp1;
                                /*'
                            switch (mode)
                            {
                            case parseFirst:
                                sp = namespacesearch(sp1->name, localNameSpace, false, false);
                                if (!sp)
                                    sp = classsearch(sp1->name, false, MATCHKW(Keyword::classsel_), false);
                                if (!sp)
                                    sp = namespacesearch(sp1->name, globalNameSpace, false, false);
                                break;
                            case parseNamespace:
                                sp = namespacesearch(sp1->name, nssym, true, false);
                                break;
                            case parseClass:
                            {
                                enclosingDeclarations.Add(strSym);
                                sp = classsearch(sp1->name, false, MATCHKW(Keyword::classsel_), false);
                                enclosingDeclarations.Drop();
                                if (!sp)
                                    sp = classsearch(sp1->name, false, MATCHKW(Keyword::classsel_), false);
                            }
                            break;

                            }
                            */
                                if (!sp || strcmp(sp->name, sp1->name) != 0)
                                {
                                    error(ERR_TYPE_NAME_EXPECTED);
                                    return;
                                }
                            }
                        }
                        sp->tp->InstantiateDeferred();
                        sp->tp->InitializeDeferred();
                        if (sp->sb && (sp->sb->storage_class == StorageClass::namespace_ ||
                            sp->sb->storage_class == StorageClass::namespace_alias_))
                        {
                            nssym = sp->sb->nameSpaceValues;
                            mode = parseNamespace;
                        }
                        else if (sp->tp->templateParam && sp->tp->templateParam->second->packed &&
                            sp->tp->templateParam->second->type == TplType::typename_)
                        {
                            if (!ellipsis)
                            {
                                // not unpacking
                                error(ERR_PACK_SPECIFIER_REQUIRED_HERE);
                                return;
                            }
                            else if (!sp->tp->templateParam->second->byPack.pack || !sp->tp->templateParam->second->byPack.pack->size())
                            {
                                // pack is empty, do nothing....
                                return;
                            }
                            else
                            {
                                absorbedPack = true;
                                stk.push(std::tuple<decltype(path)::iterator, int, int, const char*, SYMBOL*, Mode>(
                                    it, -1, (int)sp->tp->templateParam->second->byPack.pack->size(), lastName, strSym, mode));
                                skipProcess = true;
                                break;
                            }
                        }
                        else if (sp->tp->IsStructured())
                        {
                            if (it->second != currentContext->end())
                            {
                                currentContext->PlayAgain(&it->second);
                                std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                                GetTemplateArguments(theCurrentFunc, sp, &lst);
                                currentContext->PlayAgain(nullptr);
                                SYMBOL* sp1;
                                sp1 = GetClassTemplate(sp, lst, false);
                                if (sp1)
                                {
                                    sp1 = TemplateClassInstantiate(sp1, lst, false, StorageClass::global_);
                                    if (sp1)
                                    {
                                        sp = sp1;
                                        sp->tp = sp->tp->InitializeDeferred();
                                    }
                                }
                            }
                            mode = parseClass;
                        }
                        else
                        {
                            break;
                        }
                    }
                    if (it == path.begin() && sp && sp->tp->type == BasicType::typedef_ && sp->tp->IsStructured())
                    {
                        sp = sp->tp->BaseType()->sp;
                        lastRealName = sp->name;
                    }
                    else
                    {
                        lastRealName = nullptr;
                    }
                    lastName = buf;
                    ++it;
                }
                if (!skipProcess)
                {
                    if (!sp || sp->tp->type == BasicType::templateparam_ ||
                        (sp->sb && (!ismemberdata(sp) && !istype(sp) && sp->sb->storage_class != StorageClass::overloads_ &&
                            sp->sb->storage_class != StorageClass::global_ && sp->sb->storage_class != StorageClass::external_ &&
                            sp->sb->storage_class != StorageClass::static_ && sp->sb->storage_class != StorageClass::localstatic_ &&
                            sp->sb->storage_class != StorageClass::constant_)))
                    {
                        if (!definingTemplate || instantiatingTemplate)
                            error(ERR_TYPE_OR_VARIABLE_EXPECTED);
                        errskim(skim_semi, false);
                    }
                    else
                    {
                        if (sp && sp->sb)
                        {
                            if (!definingTemplate)
                            {
                                if (sp->sb->storage_class == StorageClass::overloads_)
                                {
                                    SYMBOL* ssp = enclosingDeclarations.GetFirst(), * ssp1;
                                    for (auto sp2 : *sp->tp->syms)
                                    {
                                        SYMBOL* sp1 = CopySymbol(sp2);
                                        sp1->sb->wasUsing = true;
                                        ssp1 = sp1->sb->parentClass;
                                        if (ssp && ismember(sp1))
                                            sp1->sb->parentClass = ssp;
                                        sp1->sb->mainsym = sp2;
                                        sp1->sb->access = access;
                                        InsertSymbol(sp1, storage_class, sp1->sb->attribs.inheritable.linkage, true);
                                        sp1->sb->parentClass = ssp1;
                                    }
                                }
                                else
                                {
                                    SYMBOL* ssp = enclosingDeclarations.GetFirst(), * ssp1;
                                    SYMBOL* sp1 = CopySymbol(sp);
                                    sp1->sb->wasUsing = true;
                                    sp1->sb->mainsym = sp;
                                    sp1->sb->access = access;
                                    ssp1 = sp1->sb->parentClass;
                                    if (ssp && ismember(sp1))
                                        sp1->sb->parentClass = ssp;
                                    if (isTypename && !istype(sp) && sp->tp->type != BasicType::templateselector_)
                                        error(ERR_TYPE_NAME_EXPECTED);
                                    if (istype(sp))
                                        InsertTag(sp1, storage_class, true);
                                    else
                                        InsertSymbol(sp1, storage_class, Linkage::cdecl_, true);
                                    sp1->sb->parentClass = ssp1;
                                }
                            }
                        }
                    }
                }
                if (!stk.empty())
                {
                    bool done2 = false;
                    while (!done2)
                    {
                        auto&& top = stk.top();
                        int start = std::get<1>(top);
                        ++start;
                        std::get<1>(top) = start;
                        it = std::get<0>(top);
                        lastName = std::get<3>(top);
                        sp = std::get<4>(top);
                        mode = std::get<5>(top);
                        if (start >= std::get<2>(top))
                        {
                            stk.pop();
                            done2 = stk.empty();
                        }
                        else
                        {
                            done2 = true;
                        }
                    }
                    // if we get here we used up all the tokens so exit...
                    done = stk.empty();
                }
            }
            if (!absorbedPack && ellipsis)
            {
                error(ERR_PACK_SPECIFIER_REQUIRES_PACKED_TEMPLATE_PARAMETER);
            }
        }
        return;
    }
}  // namespace Parser
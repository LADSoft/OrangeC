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

#include "compiler.h"
#include "ListFactory.h"

namespace Parser
{
    ListFactory<struct casedata*> casedataListFactory;
    ListFactory<Statement*> stmtListFactory;
    ListFactory<struct expr*> exprListFactory;
    ListFactory<struct sym*> symListFactory;
    ListFactory<NAMESPACEVALUEDATA*> namespaceValueDataListFactory;
    ListFactory<FunctionBlock*> blockDataListFactory;
    ListFactory<Initializer*> initListFactory;
    ListFactory<struct _ccNamespaceData*> ccNameSpaceDataListFactory;
    ListFactory<struct __lambda*> lambdaListFactory;
    ListFactory<struct _memberInitializers*> memberInitializersListFactory;
    ListFactory<struct _baseClass*> baseClassListFactory;
    ListFactory<struct _vtabEntry*> vtabEntryListFactory;
    ListFactory<struct _vbaseEntry*> vbaseEntryListFactory;
    ListFactory<struct _structSym*> structsymListFactory;
    ListFactory<Argument*> initListListFactory;
    ListFactory<struct _errl*> errlListFactory;
    ListFactory<StringData*> stringListFactory;
    ListFactory<struct Optimizer::_linedata*> lineDataListFactory;
    ListFactory<TEMPLATEPARAMPAIR> templateParamPairListFactory;
    ListFactory<TEMPLATESELECTOR> templateSelectorListFactory;

    void ListFactoryInit()
    {
        casedataListFactory.Reset();
        stmtListFactory.Reset();
        exprListFactory.Reset();
        symListFactory.Reset();
        namespaceValueDataListFactory.Reset();
        blockDataListFactory.Reset();
        initListFactory.Reset();
        ccNameSpaceDataListFactory.Reset();
        lambdaListFactory.Reset();
        memberInitializersListFactory.Reset();
        baseClassListFactory.Reset();
        vtabEntryListFactory.Reset();
        vbaseEntryListFactory.Reset();
        structsymListFactory.Reset();
        initListListFactory.Reset();
        errlListFactory.Reset();
        stringListFactory.Reset();
        lineDataListFactory.Reset();
        templateParamPairListFactory.Reset();
        templateSelectorListFactory.Reset();
    }
    }
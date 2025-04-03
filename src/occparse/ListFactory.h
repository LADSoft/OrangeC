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

#include <list>

namespace Parser
{
template <class T>
class ListFactory
{
  public:
    std::list<T>* CreateList()
    {
        data.push_back(std::list<T>());
        return &data.back();
    }
    std::vector<T>* CreateVector()
    {
        vdata.push_back(std::vector<T>());
        return &vdata.back();
    }
    void Reset()
    {
        data.clear();
        vdata.clear();
    }

  private:
    std::list<std::list<T>> data;
    std::list<std::vector<T>> vdata;
};

void ListFactoryInit();

extern ListFactory<struct casedata*> casedataListFactory;
extern ListFactory<Statement*> stmtListFactory;
extern ListFactory<struct expr*> exprListFactory;
extern ListFactory<struct sym*> symListFactory;
extern ListFactory<NAMESPACEVALUEDATA*> namespaceValueDataListFactory;
extern ListFactory<FunctionBlock*> blockDataListFactory;
extern ListFactory<Initializer*> initListFactory;
extern ListFactory<struct _ccNamespaceData*> ccNameSpaceDataListFactory;
extern ListFactory<struct __lambda*> lambdaListFactory;
extern ListFactory<struct _memberInitializers*> memberInitializersListFactory;
extern ListFactory<struct _baseClass*> baseClassListFactory;
extern ListFactory<struct _vtabEntry*> vtabEntryListFactory;
extern ListFactory<struct _vbaseEntry*> vbaseEntryListFactory;
extern ListFactory<struct _structSym*> structsymListFactory;
extern ListFactory<Argument*> argumentListFactory;
extern ListFactory<struct _errl*> errlListFactory;
extern ListFactory<StringData*> stringListFactory;
extern ListFactory<struct Optimizer::_linedata*> lineDataListFactory;
extern ListFactory<TEMPLATEPARAMPAIR> templateParamPairListFactory;
extern ListFactory<TEMPLATESELECTOR> templateSelectorListFactory;

}  // namespace Parser

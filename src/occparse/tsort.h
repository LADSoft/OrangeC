#Software License Agreement
#
#Copyright(C) 1994 - 2025 David Lindauer, (LADSoft)
#
#This file is part of the Orange C Compiler package.
#
#The Orange C Compiler package is free software : you can redistribute it and / or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.
#
#The Orange C Compiler package is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with Orange C.If not, see < http:  // www.gnu.org/licenses/>.
#
#contact information:
#email : TouchStone222 @runbox.com < David Lindauer>
#
#

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

// topological sort for dependency trees
#include <list>
#include <set>
#include <map>

// this code doesn't detect cycles...

template <class T>
void TVisit(T item, std::set<T>& visited, std::list<T>& sorted, std::map<T, std::set<T>>& dependencies)
{
    if (visited.find(item) == visited.end())
    {
        visited.insert(item);

        for (auto&& i : dependencies[item])
            TVisit(i, visited, sorted, dependencies);

        sorted.push_back(item);
    }
    // else
    //{
    // if we get here and 'sorted' doesn't contain the item we have a cycle...
    //}
}

template <class T>
std::list<T> TSort(std::set<T>& items, std::map<T, std::set<T>>& dependencies)
{
    std::list<T> sorted;
    std::set<T> visited;
    for (auto&& i : items)
        TVisit(i, visited, sorted, dependencies);

    return sorted;
}

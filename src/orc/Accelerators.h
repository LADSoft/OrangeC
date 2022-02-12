/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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

#ifndef Accelerators_h
#define Accelerators_h

#include "Resource.h"
#include <deque>
class RCFile;
class ResFile;

class Accelerators : public Resource
{
  public:
    struct Key
    {
        enum
        {
            Virtkey = 1,
            NoInvert = 2,
            Shift = 4,
            Control = 8,
            Alt = 16,
            Last = 128
        };
        Key() : flags(0), key(0), id(0) {}
        int flags;
        int key;
        int id;
        void WriteRes(ResFile& resFile, bool last);
        void ReadRC(RCFile& rcFile);
    };
    Accelerators(const ResourceId& Id, const ResourceInfo& info) : Resource(eAccelerators, Id, info) {}
    virtual ~Accelerators() {}
    virtual void WriteRes(ResFile& resFile);
    virtual void ReadRC(RCFile& rcFile);

    typedef std::deque<Key>::iterator iterator;
    iterator begin() { return keys.begin(); }
    iterator end() { return keys.end(); }

  private:
    std::deque<Key> keys;
};

#endif
/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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

#ifndef LEObjectPage_h
#define LEObjectPage_h

#include <deque>
#include <fstream>
#include <memory>

class LEObject;
class ObjectPage
{
  public:
    ObjectPage(std::deque<LEObject*> Objects) : objects(Objects), data(nullptr), pages(0), size(0) {}
    virtual ~ObjectPage() {}
    virtual void Setup() = 0;
    void Write(std::fstream& stream);
    unsigned GetSize() { return size; }

  protected:
    virtual unsigned CountPages() = 0;

    unsigned pages;
    std::unique_ptr<unsigned char[]> data;
    unsigned size;
    std::deque<LEObject*> objects;
};

class LEObjectPage : public ObjectPage
{
  public:
    LEObjectPage(std::deque<LEObject*> Objects) : ObjectPage(Objects) {}
    virtual ~LEObjectPage() {}
    virtual void Setup();

    struct PageData
    {
        unsigned char high_offs;
        unsigned char med_offs;
        unsigned char low_offs;
        unsigned char flags;
    };

  protected:
    virtual unsigned CountPages();
};
class LXObjectPage : public ObjectPage
{
  public:
    LXObjectPage(std::deque<LEObject*> Objects) : ObjectPage(Objects) {}
    virtual ~LXObjectPage() {}
    virtual void Setup();
    struct PageData
    {
        unsigned data_offset;
        unsigned short data_size;
        unsigned short flags;
    };

  protected:
    virtual unsigned CountPages();
};
#endif

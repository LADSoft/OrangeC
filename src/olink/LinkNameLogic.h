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

#ifndef LinkNameLogic_h
#define LinkNameLogic_h

#include <string>
#include <memory>
class LinkNameLogic
{
  public:
    LinkNameLogic(std::string spec) : top(nullptr) { ParseOut(std::move(spec)); }
    virtual ~LinkNameLogic();

    bool Matches(const std::string& name);

  protected:
    struct ParseItem
    {
        ParseItem() : left(nullptr), right(nullptr), mode(eName) {}
        enum Mode
        {
            eNot,
            eOr,
            eAnd,
            eName
        } mode;
        std::unique_ptr<ParseItem> left, right;
        std::string token;
        bool Matches(const std::string& name);
    };
    void ParseOut(std::string spec);
    std::unique_ptr<ParseItem> ParseOutOr(std::string& spec);
    std::unique_ptr<ParseItem> ParseOutAnd(std::string& spec);
    std::unique_ptr<ParseItem> ParseOutNot(std::string& spec);
    std::unique_ptr<ParseItem> ParseOutPrimary(std::string& spec);

  private:
    std::unique_ptr<ParseItem> top;
};
#endif
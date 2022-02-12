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

#include <string>
class SharedMemory
{
  public:
    SharedMemory(unsigned max, std::string name = "", unsigned window = 1024 * 1024);
    ~SharedMemory();

    bool Open();
    bool Create();
    unsigned ViewWindowSize() const { return windowSize_; }
    std::string Name() { return name_; }
    unsigned char* GetMapping(unsigned pos = 0);
    void CloseMapping();
    bool EnsureCommitted(int size);
    void Flush();

  private:
    void SetName();
    std::string name_;

    unsigned max_;
    unsigned windowSize_;
    unsigned current_;
    void* fileHandle_;
    unsigned regionBase_;
    void* regionHandle;
    unsigned char* regionStart;
};

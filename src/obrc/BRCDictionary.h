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

#ifndef BRCDICTIONARY_H
#define BRCDICTIONARY_H

#include "ObjTypes.h"
#include "BRCLoader.h"
#include <fstream>
#include <map>
#include <memory>

class ObjFile;
class LibFiles;
class SymData;

class BRCDictionary
{
  public:
    typedef std::map<ObjString, ObjInt> Dictionary;
    enum
    {
        LIB_PAGE_SIZE = 512,
        LIB_BUCKETS = 37
    };
    union DICTPAGE
    {
        ObjByte bytes[LIB_PAGE_SIZE]; /* Either 512 ObjBytes */
        struct
        {
            ObjByte htab[LIB_BUCKETS];                      /* Or the 37 buckets */
            ObjByte fflag;                                  /* followed by the next free position */
            ObjByte names[LIB_PAGE_SIZE - LIB_BUCKETS - 1]; /* followed by names section */
        } f;
    };
    BRCDictionary(Symbols& p) : symbols(p), blockCount(0) {}
    ~BRCDictionary() {}
    ObjInt Lookup(std::fstream& stream, ObjInt dictOffset, ObjInt dictPages, const ObjString& str);
    void Write(std::fstream& stream);
    void CreateDictionary(void);
    void Clear()
    {
        dictionary.clear();
        data.release();
        blockCount = 0;
    }
    ObjInt GetBlockCount() const { return blockCount; }
    void SetBlockCount(ObjInt BlockCount) { blockCount = BlockCount; }
    ObjInt casecmp(const char* str1, const char* str2, int n);

  protected:
    int ROTL(int x, int by);
    int ROTR(int x, int by);
    void ComputeHash(const char* name);
    bool InsertInDictionary(SymData* sym);

  private:
    Symbols& symbols;
    Dictionary dictionary;
    int blockCount;
    std::unique_ptr<DICTPAGE[]> data;
    int block_d, bucket_d, block_x, bucket_x;
    int oblock_x;
    int obucket_x;

    static int primes[];
};
#endif  // BRCDICTIONARY_H
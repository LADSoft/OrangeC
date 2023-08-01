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

#include <windows.h>
#include <random>
#include <array>
void *__canary;


typedef void CALLBACK convert_func(const char* name, char* buf);

#pragma startup SetCanary 31

static void SetCanary()
{
    std::array<unsigned char, sizeof(void *)> rnd;

    std::uniform_int_distribution<int> distribution(0, 0xff);
    // note that this whole thing will fall apart if the C++ lib uses
    // a prng with constant seed for the random_device implementation.
    // that shouldn't be a problem on OS we are interested in.
    std::random_device dev;
    std::mt19937 engine(dev());
    auto generator = std::bind(distribution, engine);

    std::generate(rnd.begin(), rnd.end(), generator);
    memcpy(&__canary, &rnd[0], sizeof(void *));
}

// depends on the prototype for ___canary_check
extern "C" void __canary_abort(char *name)
{
    char buf[8000];
    char *ptr = name;
    HMODULE hmod = LoadLibrary("lsdbghelper");
    if (hmod)
    {
        convert_func* f = (convert_func*)GetProcAddress(hmod, "ConvertSymbolName");
        if (f)
        {
            buf[0] = 0;
            f(name, buf);
            ptr = buf;
        }
        FreeLibrary(hmod);
    }
    fprintf(stderr, "'%s' has written past the top of its stack", ptr);
    fflush(stderr);
    _abort();
}

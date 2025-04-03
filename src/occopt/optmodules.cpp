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

#include "optmodules.h"
#include "ioptimizer.h"
#include "ildata.h"
#include "config.h"
#include "Utils.h"
namespace Optimizer
{
struct OptimizerParam
{
    const char* paramName;
    unsigned optMask;
};
std::vector<OptimizerParam> Params{
    {"reshape", OPT_RESHAPE},           {"constant", OPT_CONSTANT}, {"loop-strength", OPT_LSTRENGTH},
    {"move-invariants", OPT_INVARIANT}, {"gcse", OPT_GCSE},
};

std::vector<OptimizerParam> IcdParams{
    {"gcse", ICD_OCP},
    {"qe", ICD_QUITEARLY},
    {"ssa", ICD_STAYSSA},
};

void optimize_setup(char select, const char* string)
{
    (void)select;
    if (!*string || (*string == '+' && string[1] == '\0'))
    {
        Optimizer::cparams.prm_optimize_for_speed = true;
        Optimizer::cparams.prm_optimize_for_size = false;
        Optimizer::cparams.prm_debug = false;
    }
    else if (*string == '-' && string[1] == '\0')
        Optimizer::cparams.prm_optimize_for_speed = Optimizer::cparams.prm_optimize_for_size =
            Optimizer::cparams.prm_optimize_float_access = false;
    else
    {
        Optimizer::cparams.prm_debug = false;
        if (*string == '0')
        {
            Optimizer::cparams.prm_optimize_for_speed = Optimizer::cparams.prm_optimize_for_size =
                Optimizer::cparams.prm_optimize_float_access = false;
        }
        else if (*string == 'f')
            Optimizer::cparams.prm_optimize_float_access = true;
        else if (*string == '2')
        {
            Optimizer::cparams.prm_optimize_for_speed = true;
            Optimizer::cparams.prm_optimize_for_size = false;
        }
        else if (*string == '1')
        {
            Optimizer::cparams.prm_optimize_for_speed = false;
            Optimizer::cparams.prm_optimize_for_size = true;
        }
    }
}

std::string ParseOptimizerParams(std::string in)
{
    std::string rv;
    auto xx = Utils::split(in);
    for (auto a : xx)
    {
        bool working = false;
        bool icd = false;
        if (a.substr(0, 4) == "opt-")
            working = true;
        else if (a.substr(0, 4) == "icd-")
            working = icd = true;

        if (working)
        {
            bool no = false;
            int offs = 4;
            if (a.substr(offs, 2) == "no")
            {
                no = true;
                offs = 6;
            }
            auto test = a.substr(offs);
            bool found = false;

            for (auto&& v : icd ? IcdParams : Params)
            {
                if (v.paramName && test == v.paramName)
                {
                    found = true;
                    if (!icd)
                    {
                        if (no)
                            Optimizer::cparams.optimizer_modules &= ~v.optMask;
                        else
                            Optimizer::cparams.optimizer_modules |= v.optMask;
                    }
                    else
                    {
                        if (no)
                            Optimizer::cparams.icd_flags &= ~v.optMask;
                        else
                            Optimizer::cparams.icd_flags |= v.optMask;
                    }
                    break;
                }
            }
            if (!found)
            {
                if (rv.size())
                    rv += ";";
                rv += a;
            }
        }
        else
        {
            if (rv.size())
                rv += ";";
            rv += a;
        }
    }
    return rv;
}
}  // namespace Optimizer

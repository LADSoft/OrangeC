/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
 *  
 *      This file is part of the Orange C Compiler package.
 *  
 *      The Orange C Compiler package is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *  
 *      The Orange C Compiler package is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *  
 *      You should have received a copy of the GNU General Public License
 *      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *      contact information:
 *          email: TouchStone222@runbox.com <David Lindauer>
 *  
 */

#include <regex>
#include <regex.h>

static const char* errlist[] = {
    "Successful Match.",
    "Not Matched.",
    "A regular expression was invalid.",
    "An invalid collating element was referenced.",
    "An invalid character class type was referenced.",
    "A trailing \ was in the pattern.",
    "A number in \digit was invalid or in error.",
    "A [ ] imbalance exists.",
    "The function is not supported.",
    "A \( \) or ( ) imbalance exists.",
    "A \{ \} imbalance exists."
    "The contents of \{ \} are invalid: not a number, number too large, more than two numbers, first larger than second.",
    "An endpoint in a range expression is invalid.",
    "Out of memory.",
    "?, * or + is not preceded by valid regular expression."};

struct iregex : public std::regex
{
    iregex(const char* str, flag_type flags, bool NewLine) : std::regex(str, flags), newline(NewLine), inosubs(!!(flags & nosubs))
    {
    }
    bool newline;
    bool inosubs;
};

static int errmap(std::regex_error& e)
{
    switch (e.code())
    {
        case std::regex_constants::error_collate:
            return REG_ECOLLATE;
        case std::regex_constants::error_ctype:
            return REG_ECTYPE;
        case std::regex_constants::error_escape:
            return REG_EESCAPE;
        case std::regex_constants::error_backref:
            return REG_BADPAT;
        case std::regex_constants::error_brack:
            return REG_EBRACK;
        case std::regex_constants::error_paren:
            return REG_EPAREN;
        case std::regex_constants::error_brace:
            return REG_EBRACE;
        case std::regex_constants::error_badbrace:
            return REG_BADBR;
        case std::regex_constants::error_range:
            return REG_ERANGE;
        case std::regex_constants::error_space:
            return REG_ESPACE;
        case std::regex_constants::error_badrepeat:
            return REG_BADRPT;
        case std::regex_constants::error_complexity:
            return REG_BADPAT;
        case std::regex_constants::error_stack:
            return REG_ESPACE;
        default:
            return REG_BADPAT;
    }
}
extern "C" int _RTL_FUNC regcomp(regex_t* re, const char* pattern, int flags)
{

    std::regex::flag_type rflags = flags & REG_EXTENDED ? std::regex_constants::extended : std::regex_constants::basic;
    if (flags & REG_ICASE)
        rflags |= std::regex_constants::icase;
    if (flags & REG_NOSUB)
        rflags |= std::regex_constants::nosubs;
    try
    {
        std::regex* inner = new iregex(pattern, rflags, !!(flags & REG_NEWLINE));
        re->re_innerctx = inner;
        re->re_nsub = 0;
        return 0;
    }
    catch (std::regex_error& e)
    {
        re->re_innerctx = NULL;
        return errmap(e);
    }
}
extern "C" int _RTL_FUNC regexec(regex_t* re, const char* str, size_t nmatch, regmatch_t pmatch[], int eflags)
{
    iregex* inner = (iregex*)re->re_innerctx;
    std::regex_constants::match_flag_type rflags = std::regex_constants::match_default;
    if (eflags & REG_NOTBOL)
        rflags |= std::regex_constants::match_not_bol;
    if (eflags & REG_NOTEOL)
        rflags |= std::regex_constants::match_not_eol;
    if (!inner->newline)
        rflags |= std::regex_constants::match_prev_avail;
    if (inner->inosubs || !nmatch)
    {
        try
        {
            bool rv = std::regex_match(str, *static_cast<std::regex*>(inner), rflags);
            return rv ? 0 : REG_NOMATCH;
        }
        catch (std::regex_error& e)
        {
            return errmap(e);
        }
    }
    else
    {
        try
        {
            std::cmatch cm;
            bool matched = std::regex_match(str, cm, *static_cast<std::regex*>(inner), rflags);
            printf("hi");
            if (matched)
            {
                re->re_nsub = cm.size() - 1;
                for (int i = 1; i < cm.size() && i < nmatch + 1; i++)
                {
                    pmatch[i - 1].rm_so = cm[i].first - str;
                    pmatch[i - 1].rm_eo = cm[i].second - str;
                }
            }
            return matched ? 0 : REG_NOMATCH;
        }
        catch (std::regex_error& e)
        {
            printf("bye");
            return errmap(e);
        }
    }
}
extern "C" size_t _RTL_FUNC regerror(int err, const regex_t* re, char* errbuf, size_t errbuf_size)
{
    iregex* inner = (iregex*)re->re_innerctx;
    const char* p = "Invalid error.";
    if (err >= 0 && err < sizeof(errlist) / sizeof(errlist[0]))
        p = errlist[err];
    if (errbuf_size)
    {
        strncpy(errbuf, p, errbuf_size);
        errbuf[errbuf_size - 1] = 0;
        if (strlen(p) < errbuf_size)
            errbuf_size = strlen(p) + 1;
        return errbuf_size;
    }
    else
    {
        return strlen(p) + 1;
    }
}
extern "C" void _RTL_FUNC regfree(regex_t* re)
{
    delete (iregex*)re->re_innerctx;
    re->re_innerctx = NULL;
}

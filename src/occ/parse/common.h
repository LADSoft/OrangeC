#pragma once
#include "compiler.h"
// Grabs the beginning of the symbol table, clears up what you're doing an lessens code smell slightly
#define SYMTABBEGIN(x) x->syms->table[0]
// Simplify macros to make them very much smaller
#define ITERTHING(name, thing) for(name = thing; name; name = name->next)
#define ITERNEWTHING(typ, name, thing) for(typ* name = thing; name; name = name->next)
// Creates a for loop to iterate over a collection of symbols, two variants, one creates a new variable the other reuses one, if it starts with N it creates a new name
#define ITERSYMTAB(name, val) ITERTHING(name, SYMTABBEGIN(val))
#define NITERSYMTAB(name, val) ITERNEWTHING(HASHREC, name, SYMTABBEGIN(val))
// Macros for iterating over base classes
#define ITERBASECLASS(name, val) ITERTHING(name, val->baseClasses)
#define NITERBASECLASS(name, val) ITERNEWTHING(BASECLASS, name, val->baseClasses)
// Macros for iterating over vbaseEntries
#define ITERVBASEENT(name, val) ITERTHING(name, val->vbaseEntries)
#define NITERVBASEENT(name, val) ITERNEWTHING(VBASEENTRY, name, val->vbaseEntries)
// Macros for iterating virtual functions
#define ITERVIRTFUNC(name, val) ITERTHING(name, val->virtuals)
#define NITERVIRTFUNC(name, val) ITERNEWTHING(VIRTUALFUNC, name, val->virtuals)
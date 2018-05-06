#include "statement_base.h"

//---NodeStatement---

StmtClassName NodeStatement::GetClassName() const
{
    return STMT;
}

void NodeStatement::Generate(AsmCode& asm_code)
{
}

/*void NodeStatement::Print(ostream& o, int offset) 
{
    ((const NodeStatement*)this)->Print(o, offset);
    }*/

 /*void NodeStatement::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset) << ";\n";
    }*/

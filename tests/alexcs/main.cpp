#include <iostream>
#include <fstream>
#include <list>
#include "scanner.h"
#include "parser.h"
#include <sstream>
#include <string>
#include "exception.h"

void PrintHelp()
{
    cout << "Usage: compiler [option] filename\n\
Avaible options are:\n\
\n\
optimization off\n\
\t-b\tprint Both syntax tree and symtable\n\
\t-h\tshow this message\n\
\t-g\tGenerate code for x86_32 GNU assembler\n\
\t-l\tshow Lexems stream\n\
\t-s\tprint Syntax tree\n\
\t-t\tprint symTable\n\
\n\
optimization on\n\
\t-B\tprint Both syntax tree and symtable\n\
\t-G\tGenerate code for x86_32 GNU assembler\n\
\t-S\tprint Syntax tree\n\
\t-T\tprint symTable\n";
}
#include <Stdio.h>
int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        PrintHelp();
        return 0;
    }
    try
    {
        if (argc > 3) throw CompilerException("too many parametrs");
        if (argc == 2)
        {
            if (argv[1][1] == 'h')
            {
                PrintHelp();
                return 0;
            }
            if (argv[1][1] == 'l' || argv[1][1] == 's')
                throw CompilerException("no files specified");
            else
                throw CompilerException("uncknown option");
        }
        ifstream in;
        in.open(argv[2], ios::in);
        if (!in.good()) throw CompilerException("can't open file");
        if (argv[1][0] != '-')
            throw CompilerException("invalid option");
        else
            {
                if (!argv[1][1] || argv[1][2]) throw CompilerException("invalid option");
                bool optimize = isupper(argv[1][1]);
                switch (tolower(argv[1][1]))
                {
                    case 'b':
                    {
                        Scanner scan(in);
                        Parser parser(scan, optimize);
                        parser.PrintSymTable(std::cout);
                        parser.PrintSyntaxTree(std::cout);
                    }
                    break;
                    case 's':
                    {
                        Scanner scan(in);
                        Parser parser(scan, optimize);
                        parser.PrintSyntaxTree(std::cout);
                    }
                    break;
                    case 't':
                    {
                        Scanner scan(in);
                        Parser parser(scan, optimize);
                        parser.PrintSymTable(std::cout);
                    }
                    break;
                    case 'g':
                    {
                        Scanner scan(in);
                        Parser parser(scan, optimize);
                        parser.Generate(std::cout);
                    }
                    break;
                    case 'l':
                    {
                        Scanner scan(in);
                        for (Token t; t.GetType() != END_OF_FILE;)
                        {
                           cout << ( t = scan.NextToken() );
                        }
                    }
                    break;
                }
            }
    }
    catch (CompilerException& e)
    {
        cout << e.what() << endl;
        return 1;
    }
    catch (exception& e)
    {
        cout << e.what() << endl;
        return 1;
    }
    return 0;
}

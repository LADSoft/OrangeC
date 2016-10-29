#include <Stdio.h>
#include "DLLExportReader.h"

main(int argc, char *argv[])
{
    if (argc> 1)
    {
        DLLExportReader reader(argv[1]);
        if (reader.Read())
        {
            for (DLLExportReader::iterator it = reader.begin(); it != reader.end(); ++it)
            {
                printf("%s\n", (*it)->name.c_str());
            }
        }
    }
}
HelpIndexer.exe is a Microsoft command-line utility that has two functions.
1. Generate an index fragment (x.mshi) from a help container (x.mshc).
2. Generate a .mshx standalone help file from .mshc & .mshi

At the time of writing the 6.2.8102 has the following command line options.

HelpIndexer <parameters>
Parameters:
  /t[ype] <output type>      - Type of output:  Defaults to 'I'
                                  'I' - Index Fragment only (.mshi)
                                  'E' - Combined file only (.mshx)
  /i[nputfile]  <filepath>   - Input file (.mshc)
  /o[utputpath] <filepath>   - Path for output files
  /f[ilename] <filename>     - Optional filename to be used for output files.
                               Default is to use the same name as the input file
  /v[endor] <vendorname>     - Required vendor name when creating index fragments (/t I)

----

To get the latest HelpIndexer utility download the Windows 8 ADK (Assessment and Deployment Kit)
from MSDN Subscriber downloads.

The tool gets installed to the following folder:
C:\Program Files\Windows Kits\8.0\Assessment and Deployment Kit\Deployment and Imaging Tools\HelpIndexer\


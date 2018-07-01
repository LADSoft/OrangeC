enum eCommentType
{
    eGeneralText = 1,
    eLinkPass = 100,
    eMakePass = 101,
    eBrowsePass = 102,
    eConfig = 104,
    eAbsolute = 105,
    eExport = 200,
    eImport = 201,
    eDefinition = 202,
    eSourceFile = 300,
    eVar = 400,
    eLineNo = 401,
    eBlockStart = 402,
    eBlockEnd = 403,
    eFunctionStart = 404,
    eFunctionEnd = 405,
    eBrowseInfo = 500
};
static const int IeeeBinarySig = 0x4549534c;  // LSIE

enum eCommands
{
    ecMB = 0x80,    // module begin
    ecME,           // module end
    ecCS,           // checksum
    ecAS = 0x90,    // assign start
    ecDT,           // date and time
    ecAD,           // module characteristics
    ecCO = 0x98,    // comment
    ecSA = 0xA0,    // Section alignment
    ecST,           // section attributes
    ecSB,           // section body
    ecNAME = 0xb0,  // symbol
    ecNT,           // type name
    ecAT,           // assign type
    ecLD = 0xc0,    // data
    ecLE,           // enumerated data
    ecLR            // fixup
};

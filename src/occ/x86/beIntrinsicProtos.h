#ifndef PROTO
#define PROTO(PROT, NAME, FUNC) PROT
#endif
PROTO("unsigned __fastcall __builtin_clz(unsigned val);", __builtin_clz, handleCLZ)
PROTO("unsigned __fastcall __builtin_clzl(unsigned long val);", __builtin_clzl, handleCLZ)
PROTO("unsigned __fastcall __builtin_ctz(unsigned val);", __builtin_ctz, handleCTZ)
PROTO("unsigned __fastcall __builtin_ctzl(unsigned long val);", __builtin_ctzl, handleCTZ)
PROTO("unsigned __fastcall __rotl(unsigned val, int count);", __rotl, handleROTL)
PROTO("unsigned __fastcall __rotr(unsigned val, int count);", __rotr, handleROTR)
PROTO("unsigned char __fastcall _BitScanForward(unsigned long* index, unsigned long Mask);", _BitScanForward, handleBSF)
PROTO("unsigned char __fastcall _BitScanReverse(unsigned long* index, unsigned long Mask);", _BitScanReverse, handleBSR)
PROTO("void __fastcall __outbyte(unsigned short Port, unsigned char Data);", __outbyte, handleOUTB)
PROTO("void __fastcall __outword(unsigned short Port, unsigned short Data);", __outword, handleOUTW)
PROTO("void __fastcall __outdword(unsigned short Port, unsigned long  Data);", __outdword, handleOUTD)
PROTO("unsigned char __fastcall __inbyte(unsigned short Port);", __inbyte, handleINB)
PROTO("unsigned short __fastcall __inword(unsigned short Port);", __inword, handleINW)
PROTO("unsigned long __fastcall __indword(unsigned short Port);", __indword, handleIND)
#undef PROTO
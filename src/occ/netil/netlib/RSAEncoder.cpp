/*
    Software License Agreement (BSD License)

    Copyright (c) 2016, David Lindauer, (LADSoft).
    All rights reserved.

    Redistribution and use of this software in source and binary forms,
    with or without modification, are permitted provided that the following
    conditions are met:

    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.

    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <random>
#include <functional>
#include <algorithm>
#include "RSAEncoder.h"
#include "SHA1.h"
#include "bigdigits.h"
#include "limits.h"

namespace DotNetPELib {

    
Byte PKCS1Formatter::DerHeader[] = {
0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2b, 0x0e, 0x03, 0x02, 0x1a, 0x05, 0x00, 0x04, 0x14
};

void PKCS1Formatter::Calculate(ByteArray &result)
{
    memset(result(), 0xff, result.size());
    result[0] = 0;
    result[1] = 1;
    int pos = result.size() - 20 - sizeof(DerHeader) -1;
    result[pos] = 0;
    memcpy(result() + pos +1, DerHeader, sizeof(DerHeader));
    memcpy(result() + result.size() - 20, msg_, 20);
    // reverse it before encryption..
    for (int i=0; i < result.size()/2; i++)
    {
        int temp = result[i];
        result[i] = result[result.size()-i-1];
        result[result.size()-i-1] = temp;
    }
}
size_t RSAEncoder::LoadStrongNameKeys(const std::string &file)
{
    int rv = 0;
    FILE *fil = fopen(file.c_str(), "rb");
    if (fil)
    {
        static Byte test[] = { 0x07, 0x02, 0x00,0x00, 0x00, 0x24, 0x00, 0x00, 
            0x52, 0x53, 0x41, 0x32 };
        Byte buf[0x14];
        fread(buf, 1, 0x14, fil);
        if (!memcmp(buf, test, sizeof(test)))
        {
            modulusBits = *(int *)(buf + sizeof(test));
            modulus = new Byte[2048];
            privateExponent = new Byte[2048];
            keyPair = new Byte[2048];
            publicExponent = *(DWord *)(buf + sizeof(test) + 4);
            fread(modulus, 1, modulusBits/8, fil);
            fseek(fil, 5 * modulusBits/16, SEEK_CUR);
            fread(privateExponent, 1, modulusBits/8, fil);
            fseek(fil, 0, SEEK_SET);
            fread(keyPair, 1, modulusBits/8 + 0x14, fil);
            rv = modulusBits/8;
        }
        fclose(fil);
    }
    return rv;
}
void RSAEncoder::GetPublicKeyData(Byte *key, size_t *keySize)
{
    DIGIT_T *dkey = (DIGIT_T *)key;
    dkey[0] = 0x2400;
    dkey[1] = 0x8004;
    dkey[2] = 0x14 + modulusBits/8;
    memcpy(dkey +3, keyPair, dkey[2]);
    ((char *)dkey)[12 + 0x0b] = '1'; // change to RSA1 (pub key only)
    ((char *)dkey)[12 + 0] = 6; // change to pub key only
    *keySize = dkey[2] + 12;
}
void RSAEncoder::GetStrongNameSignature(Byte *sig, size_t *sigSize, const Byte *hash, size_t hashSize )
{
    ByteArray x(modulusBits/8);
    PKCS1Formatter formatter(hash);
    formatter.Calculate(x);
    mpModExp((DIGIT_T *)sig, (DIGIT_T *)x(), (DIGIT_T *)privateExponent, (DIGIT_T *)modulus, modulusBits/8/sizeof(DIGIT_T));
    *sigSize = modulusBits/8;

}
} // namespace
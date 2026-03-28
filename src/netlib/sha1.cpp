/*
 *  sha1.c
 *
 *  Copyright (C) 1998, 2009xro

 *  Paul E. Jones <paulej@packetizer.com>
 *  All Rights Reserved#
 *
 *****************************************************************************
 *  $Id: sha1.c 12 2009-06-22 19:34:25Z paulej $
 *****************************************************************************
 *
 *  Description:
 *      This file implements the Secure Hashing Standard as defined
 *      in FIPS PUB 180-1 published April 17, 1995.
 *
 *      The Secure Hashing Standard, which uses the Secure Hashing
 *      Algorithm (SHA), produces a 160-bit message digest for a
 *      given data stream.  In theory, it is highly improbable that
 *      two messages will produce the same message digest.  Therefore,
 *      this algorithm can serve as a means of providing a "fingerprint"
 *      for a message.
 *
 *  Portability Issues:
 *      SHA-1 is defined in terms of 32-bit "words".  This code was
 *      written with the expectation that the processor has at least
 *      a 32-bit machine word size.  If the machine word size is larger,
 *      the code should still function properly.  One caveat to that
 *      is that the input functions taking characters and character
 *      arrays assume that only 8 bits of information are stored in each
 *      character.
 *
 *  Caveats:
 *      SHA-1 is designed to work with messages less than 2^64 bits
 *      long. Although SHA-1 allows a message digest to be generated for
 *      messages of any number of bits less than 2^64, this
 *      implementation only works with messages with a length that is a
 *      multiple of the size of an 8-bit character.
 *
 */

#include "sha1.h"
#include <stdio.h>
#include <string.h>

#if !defined(__ORANGEC__)
#include  <immintrin.h>
#endif

/*
 *  Define the circular shift macro
 */
#define SHA1CircularShift(bits, word) ((((word) << (bits)) & 0xFFFFFFFF) | ((word) >> (32 - (bits))))

namespace DotNetPELib
{
/* Function prototypes */
void SHA1ProcessMessage_Block(SHA1Context*);
void SHA1PadMessage(SHA1Context*);

/*
 *  SHA1Reset
 *
 *  Description:
 *      This function will initialize the SHA1Context in preparation
 *      for computing a new message digest.
 *
 *  Parameters:
 *      context: [in/out]
 *          The context to reset.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *
 */
void SHA1Reset(SHA1Context* context)
{
    context->Length_Low = 0;
    context->Length_High = 0;
    context->Message_Block_Index = 0;

    context->Message_Digest[0] = 0x67452301;
    context->Message_Digest[1] = 0xEFCDAB89;
    context->Message_Digest[2] = 0x98BADCFE;
    context->Message_Digest[3] = 0x10325476;
    context->Message_Digest[4] = 0xC3D2E1F0;

    context->Computed = 0;
    context->Corrupted = 0;
}

/*
 *  SHA1Result
 *
 *  Description:
 *      This function will return the 160-bit message digest into the
 *      Message_Digest array within the SHA1Context provided
 *
 *  Parameters:
 *      context: [in/out]
 *          The context to use to calculate the SHA-1 hash.
 *
 *  Returns:
 *      1 if successful, 0 if it failed.
 *
 *  Comments:
 *
 */
int SHA1Result(SHA1Context* context)
{

    if (context->Corrupted)
    {
        return 0;
    }

    if (!context->Computed)
    {
        SHA1PadMessage(context);
        context->Computed = 1;
        // rearrange for littleendian processors
        for (int i = 0; i < 5; i++)
        {
            unsigned n = context->Message_Digest[i];
            int n1 = 0;
            n1 = (n >> 24) + ((n >> 8) & 0xff00) + ((n << 8) & 0xff0000) + (n << 24);
            context->Message_Digest[i] = n1;
        }
    }

    return 1;
}

/*
 *  SHA1Input
 *
 *  Description:
 *      This function accepts an array of octets as the next portion of
 *      the message.
 *
 *  Parameters:
 *      context: [in/out]
 *          The SHA-1 context to update
 *      message_array: [in]
 *          An array of characters representing the next portion of the
 *          message.
 *      length: [in]
 *          The length of the message in message_array
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *
 */
void SHA1Input(SHA1Context* context, const unsigned char* message_array, unsigned length)
{
    if (!length)
    {
        return;
    }

    if (context->Computed || context->Corrupted)
    {
        context->Corrupted = 1;
        return;
    }


    while (length-- && !context->Corrupted)
    {
        context->Message_Block[context->Message_Block_Index++] = (*message_array);

        context->Length_Low += 8;

        #if 0
        /* Force it to 32 bits */
        context->Length_Low &= 0xFFFFFFFF;
        if (context->Length_Low == 0)
        {
            context->Length_High++;
            /* Force it to 32 bits */
            context->Length_High &= 0xFFFFFFFF;
            if (context->Length_High == 0)
            {
                /* Message is too long */
                context->Corrupted = 1;
            }
        }
        #endif

        if (context->Message_Block_Index == 64)
        {
            SHA1ProcessMessage_Block(context);
        }

        message_array++;
    }
}

#if defined(__clang__) || defined(__GNUC__) || defined(__INTEL_COMPILER)
#include <cpuid.h>
static int supports_sha_ni(void)
{
    unsigned int CPUInfo[4];
    __cpuid(0, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);
    if (CPUInfo[0] < 7)
        return 0;

    __cpuid_count(7, 0, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);
    return CPUInfo[1] & (1 << 29); /* SHA */
}

#else
#if defined(_MSC_VER) || defined(__ORANGEC__)
#include <intrin.h>
#endif

static int supports_sha_ni(void)
{
    int CPUInfo[4];
    __cpuid(CPUInfo, 0);  
    if (CPUInfo[0] < 7)
        return 0;

    __cpuidex(CPUInfo, 7, 0);
    return CPUInfo[1] & (1 << 29); /* Check SHA */
}

#endif /* defined(__clang__) || defined(__GNUC__) */


#if defined(_MSC_VER) || defined(__ORANGEC__) || defined(__clang__) || defined(__GNUC__) || defined(__INTEL_COMPILER)
static bool has_sha_ni = supports_sha_ni();
#endif

#if defined(_MSC_VER) || defined(__clang__) || defined(__GNUC__) || defined(__INTEL_COMPILER)
// process one block...
static void sha1_process_sha_ni(unsigned state[5], const unsigned char data[])
{
    __m128i ABCD, ABCD_SAVE, E0, E0_SAVE, E1;
    __m128i MSG0, MSG1, MSG2, MSG3;
    __m128i MASK = _mm_set_epi64x(0x0001020304050607ULL, 0x08090a0b0c0d0e0fULL);

    /* Load initial values */
    ABCD = _mm_loadu_si128((const __m128i*) state);
    E0 = _mm_set_epi32(state[4], 0, 0, 0);
    ABCD = _mm_shuffle_epi32(ABCD, 0x1B);

    /* Save current state  */
    ABCD_SAVE = ABCD;
    E0_SAVE = E0;

    /* Rounds 0-3 */
    MSG0 = _mm_loadu_si128((const __m128i*)(data + 0));
    MSG0 = _mm_shuffle_epi8(MSG0, MASK);
    E0 = _mm_add_epi32(E0, MSG0);
    E1 = ABCD;
    ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 0);

    /* Rounds 4-7 */
    MSG1 = _mm_loadu_si128((const __m128i*)(data + 16));
    MSG1 = _mm_shuffle_epi8(MSG1, MASK);
    E1 = _mm_sha1nexte_epu32(E1, MSG1);
    E0 = ABCD;
    ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 0);
    MSG0 = _mm_sha1msg1_epu32(MSG0, MSG1);

        /* Rounds 8-11 */
    MSG2 = _mm_loadu_si128((const __m128i*)(data + 32));
    MSG2 = _mm_shuffle_epi8(MSG2, MASK);
    E0 = _mm_sha1nexte_epu32(E0, MSG2);
    E1 = ABCD;
    ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 0);
    MSG1 = _mm_sha1msg1_epu32(MSG1, MSG2);
    MSG0 = _mm_xor_si128(MSG0, MSG2);

    /* Rounds 12-15 */
    MSG3 = _mm_loadu_si128((const __m128i*)(data + 48));
    MSG3 = _mm_shuffle_epi8(MSG3, MASK);
    E1 = _mm_sha1nexte_epu32(E1, MSG3);
    E0 = ABCD;
    MSG0 = _mm_sha1msg2_epu32(MSG0, MSG3);
    ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 0);
    MSG2 = _mm_sha1msg1_epu32(MSG2, MSG3);
    MSG1 = _mm_xor_si128(MSG1, MSG3);

    /* Rounds 16-19 */
//    E0 = _mm_sha1nexte_epu32(E0, MSG0);
    E1 = ABCD;
    MSG1 = _mm_sha1msg2_epu32(MSG1, MSG0);
    ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 0);
    MSG3 = _mm_sha1msg1_epu32(MSG3, MSG0);
    MSG2 = _mm_xor_si128(MSG2, MSG0);

    /* Rounds 20-23 */
    E1 = _mm_sha1nexte_epu32(E1, MSG1);
    E0 = ABCD;
    MSG2 = _mm_sha1msg2_epu32(MSG2, MSG1);
    ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 1);
    MSG0 = _mm_sha1msg1_epu32(MSG0, MSG1);
    MSG3 = _mm_xor_si128(MSG3, MSG1);

    /* Rounds 24-27 */
    E0 = _mm_sha1nexte_epu32(E0, MSG2);
    E1 = ABCD;
    MSG3 = _mm_sha1msg2_epu32(MSG3, MSG2);
    ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 1);
    MSG1 = _mm_sha1msg1_epu32(MSG1, MSG2);
    MSG0 = _mm_xor_si128(MSG0, MSG2);

    /* Rounds 28-31 */
    E1 = _mm_sha1nexte_epu32(E1, MSG3);
    E0 = ABCD;
    MSG0 = _mm_sha1msg2_epu32(MSG0, MSG3);
    ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 1);
    MSG2 = _mm_sha1msg1_epu32(MSG2, MSG3);
    MSG1 = _mm_xor_si128(MSG1, MSG3);

    /* Rounds 32-35 */
    E0 = _mm_sha1nexte_epu32(E0, MSG0);
    E1 = ABCD;
    MSG1 = _mm_sha1msg2_epu32(MSG1, MSG0);
    ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 1);
    MSG3 = _mm_sha1msg1_epu32(MSG3, MSG0);
    MSG2 = _mm_xor_si128(MSG2, MSG0);

    /* Rounds 36-39 */
    E1 = _mm_sha1nexte_epu32(E1, MSG1);
    E0 = ABCD;
    MSG2 = _mm_sha1msg2_epu32(MSG2, MSG1);
    ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 1);
    MSG0 = _mm_sha1msg1_epu32(MSG0, MSG1);
    MSG3 = _mm_xor_si128(MSG3, MSG1);

    /* Rounds 40-43 */
    E0 = _mm_sha1nexte_epu32(E0, MSG2);
    E1 = ABCD;
    MSG3 = _mm_sha1msg2_epu32(MSG3, MSG2);
    ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 2);
    MSG1 = _mm_sha1msg1_epu32(MSG1, MSG2);
    MSG0 = _mm_xor_si128(MSG0, MSG2);

    /* Rounds 44-47 */
    E1 = _mm_sha1nexte_epu32(E1, MSG3);
    E0 = ABCD;
    MSG0 = _mm_sha1msg2_epu32(MSG0, MSG3);
    ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 2);
    MSG2 = _mm_sha1msg1_epu32(MSG2, MSG3);
    MSG1 = _mm_xor_si128(MSG1, MSG3);

    /* Rounds 48-51 */
    E0 = _mm_sha1nexte_epu32(E0, MSG0);
    E1 = ABCD;
    MSG1 = _mm_sha1msg2_epu32(MSG1, MSG0);
    ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 2);
    MSG3 = _mm_sha1msg1_epu32(MSG3, MSG0);
    MSG2 = _mm_xor_si128(MSG2, MSG0);

    /* Rounds 52-55 */
    E1 = _mm_sha1nexte_epu32(E1, MSG1);
    E0 = ABCD;
    MSG2 = _mm_sha1msg2_epu32(MSG2, MSG1);
    ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 2);
    MSG0 = _mm_sha1msg1_epu32(MSG0, MSG1);
    MSG3 = _mm_xor_si128(MSG3, MSG1);

    /* Rounds 56-59 */
    E0 = _mm_sha1nexte_epu32(E0, MSG2);
    E1 = ABCD;
    MSG3 = _mm_sha1msg2_epu32(MSG3, MSG2);
    ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 2);
    MSG1 = _mm_sha1msg1_epu32(MSG1, MSG2);
    MSG0 = _mm_xor_si128(MSG0, MSG2);

    /* Rounds 60-63 */
    E1 = _mm_sha1nexte_epu32(E1, MSG3);
    E0 = ABCD;
    MSG0 = _mm_sha1msg2_epu32(MSG0, MSG3);
    ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 3);
    MSG2 = _mm_sha1msg1_epu32(MSG2, MSG3);
    MSG1 = _mm_xor_si128(MSG1, MSG3);

        /* Rounds 64-67 */
    E0 = _mm_sha1nexte_epu32(E0, MSG0);
    E1 = ABCD;
    MSG1 = _mm_sha1msg2_epu32(MSG1, MSG0);
    ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 3);
    MSG3 = _mm_sha1msg1_epu32(MSG3, MSG0);
    MSG2 = _mm_xor_si128(MSG2, MSG0);

    /* Rounds 68-71 */
    E1 = _mm_sha1nexte_epu32(E1, MSG1);
    E0 = ABCD;
    MSG2 = _mm_sha1msg2_epu32(MSG2, MSG1);
    ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 3);
    MSG3 = _mm_xor_si128(MSG3, MSG1);

    /* Rounds 72-75 */
    E0 = _mm_sha1nexte_epu32(E0, MSG2);
    E1 = ABCD;
    MSG3 = _mm_sha1msg2_epu32(MSG3, MSG2);
    ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 3);

    /* Rounds 76-79 */
    E1 = _mm_sha1nexte_epu32(E1, MSG3);
    E0 = ABCD;
    ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 3);

    /* Combine state */
    E0 = _mm_sha1nexte_epu32(E0, E0_SAVE);
    ABCD = _mm_add_epi32(ABCD, ABCD_SAVE);

    /* Save state */
    ABCD = _mm_shuffle_epi32(ABCD, 0x1B);
    _mm_storeu_si128((__m128i*) state, ABCD);
    state[4] = _mm_extract_epi32(E0, 3);
}
#endif
#if defined(__ORANGEC__)
#define __m128i(name) unsigned name [4]
#define ABCD xmm0
#define E0 xmm1
#define E1 xmm2
#define MSG0 xmm3
#define MSG1 xmm4
#define MSG2 xmm5
#define MSG3 xmm6
#define DATA_PTR ecx
#define STATE_PTR ecx


// process one block...
void sha1_process_sha_ni(unsigned state[5], const unsigned char data[])
{
    static const __m128i(MASK) __attribute__((aligned(16))) = { 0x0c0d0e0f, 0x08090a0b, 0x04050607, 0x00010203 } ;
    static __m128i(ABCD_SAVE)  __attribute__((aligned(16)));
    static __m128i(E_SAVE)  __attribute__((aligned(16)));
    __m128i(r3);
    __m128i(r4);
    __m128i(r5);
    __m128i(r6);
    __m128i(temp) = { 0 , 0, 0, state[4] };

    __asm movups      [r3], xmm3
    __asm movups      [r4], xmm4
    __asm movups      [r5], xmm5
    __asm movups      [r6], xmm6

    /* Load initial values */
//    ABCD = _mm_loadu_si128((const __m128i*) state);
//    E0 = _mm_set_epi32(state[4], 0, 0, 0);
//    ABCD = _mm_shuffle_epi32(ABCD, 0x1B);

    __asm mov         ecx, [state]
    __asm movups      ABCD, [ecx] 
    __asm pshufd      ABCD, ABCD, 0x1b
    __asm pxor        E0, E0
    __asm pinsrd      E0, [ecx+12], 3
    __asm movups      E0, [temp]

    __asm mov ecx, [data] // DATA_PTR
    /* Save current state  */
//    ABCD_SAVE = ABCD;
//    E0_SAVE = E0;

    __asm movups       [ABCD_SAVE], ABCD
    __asm movups       [E_SAVE], E0


    /* Rounds 0-3 */
//    MSG0 = _mm_loadu_si128((const __m128i*)(data + 0));
//    MSG0 = _mm_shuffle_epi8(MSG0, MASK);
//    E0 = _mm_add_epi32(E0, MSG0);
//    E1 = ABCD;
//    ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 0);
    __asm movups        MSG0, [DATA_PTR + 0]
    __asm pshufb        MSG0, [MASK]
    __asm paddd         E0, MSG0 
    __asm movups        E1, ABCD
    __asm sha1rnds4     ABCD, E0, 0
   
    /* Rounds 4-7 */
//    MSG1 = _mm_loadu_si128((const __m128i*)(data + 16));
//    MSG1 = _mm_shuffle_epi8(MSG1, MASK);
//    E1 = _mm_sha1nexte_epu32(E1, MSG1);
//    E0 = ABCD;
//    ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 0);
//    MSG0 = _mm_sha1msg1_epu32(MSG0, MSG1);
    __asm movups       MSG1, [DATA_PTR + 16]
    __asm pshufb       MSG1, [MASK]
    __asm sha1nexte    E1, MSG1
    __asm movups       E0, ABCD
    __asm sha1rnds4    ABCD, E1, 0
    __asm sha1msg1     MSG0, MSG1
        /* Rounds 8-11 */
//    MSG2 = _mm_loadu_si128((const __m128i*)(data + 32));
//    MSG2 = _mm_shuffle_epi8(MSG2, MASK);
//    E0 = _mm_sha1nexte_epu32(E0, MSG2);
//    E1 = ABCD;
//    ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 0);
//    MSG1 = _mm_sha1msg1_epu32(MSG1, MSG2);
//    MSG0 = _mm_xor_si128(MSG0, MSG2);
    __asm movups       MSG2, [DATA_PTR + 32]
    __asm pshufb       MSG2, [MASK]
    __asm sha1nexte    E0, MSG2
    __asm movups       E1, ABCD
    __asm sha1rnds4    ABCD, E0, 0
    __asm sha1msg1     MSG1, MSG2
    __asm pxor         MSG0, MSG2    
    /* Rounds 12-15 */
//    MSG3 = _mm_loadu_si128((const __m128i*)(data + 48));
//    MSG3 = _mm_shuffle_epi8(MSG3, MASK);
//    E1 = _mm_sha1nexte_epu32(E1, MSG3);
//    E0 = ABCD;
//    MSG0 = _mm_sha1msg2_epu32(MSG0, MSG3);
//    ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 0);
//    MSG2 = _mm_sha1msg1_epu32(MSG2, MSG3);
//    MSG1 = _mm_xor_si128(MSG1, MSG3);
    __asm movups       MSG3, [DATA_PTR + 48]
    __asm pshufb       MSG3, [MASK]
    __asm sha1nexte    E1, MSG3
    __asm movups       E0, ABCD
    __asm sha1msg2     MSG0, MSG3
    __asm sha1rnds4    ABCD, E1, 0
    __asm sha1msg1     MSG2, MSG3
    __asm pxor         MSG1, MSG3
    /* Rounds 16-19 */
//    E0 = _mm_sha1nexte_epu32(E0, MSG0);
//    E1 = ABCD;
//    MSG1 = _mm_sha1msg2_epu32(MSG1, MSG0);
//    ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 0);
//    MSG3 = _mm_sha1msg1_epu32(MSG3, MSG0);
//    MSG2 = _mm_xor_si128(MSG2, MSG0);

    __asm sha1nexte    E0, MSG0
    __asm movups       E1, ABCD
    __asm sha1msg2     MSG1, MSG0
    __asm sha1rnds4    ABCD, E0, 0
    __asm sha1msg1     MSG3, MSG0
    __asm pxor 	       MSG2, MSG0
    /* Rounds 20-23 */
//    E1 = _mm_sha1nexte_epu32(E1, MSG1);
//    E0 = ABCD;
//    MSG2 = _mm_sha1msg2_epu32(MSG2, MSG1);
//    ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 1);
//    MSG0 = _mm_sha1msg1_epu32(MSG0, MSG1);
//    MSG3 = _mm_xor_si128(MSG3, MSG1);
    __asm sha1nexte    E1, MSG1
    __asm movups       E0, ABCD
    __asm sha1msg2     MSG2, MSG1
    __asm sha1rnds4    ABCD, E1, 1
    __asm sha1msg1     MSG0, MSG1
    __asm pxor 	       MSG3, MSG1
    /* Rounds 24-27 */
//    E0 = _mm_sha1nexte_epu32(E0, MSG2);
//    E1 = ABCD;
//    MSG3 = _mm_sha1msg2_epu32(MSG3, MSG2);
//    ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 1);
//    MSG1 = _mm_sha1msg1_epu32(MSG1, MSG2);
//    MSG0 = _mm_xor_si128(MSG0, MSG2);
    __asm sha1nexte    E0, MSG2
    __asm movups       E1, ABCD
    __asm sha1msg2     MSG3, MSG2
    __asm sha1rnds4    ABCD, E0, 1
    __asm sha1msg1     MSG1, MSG2
    __asm pxor 	       MSG0, MSG2
    /* Rounds 28-31 */
//    E1 = _mm_sha1nexte_epu32(E1, MSG3);
//    E0 = ABCD;
//    MSG0 = _mm_sha1msg2_epu32(MSG0, MSG3);
//    ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 1);
//    MSG2 = _mm_sha1msg1_epu32(MSG2, MSG3);
//    MSG1 = _mm_xor_si128(MSG1, MSG3);
    __asm sha1nexte    E1, MSG3
    __asm movups       E0, ABCD
    __asm sha1msg2     MSG0, MSG3
    __asm sha1rnds4    ABCD, E1, 1
    __asm sha1msg1     MSG2, MSG3
    __asm pxor 	       MSG1, MSG3
    /* Rounds 32-35 */
//    E0 = _mm_sha1nexte_epu32(E0, MSG0);
//    E1 = ABCD;
//    MSG1 = _mm_sha1msg2_epu32(MSG1, MSG0);
//    ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 1);
//    MSG3 = _mm_sha1msg1_epu32(MSG3, MSG0);
//    MSG2 = _mm_xor_si128(MSG2, MSG0);
    __asm sha1nexte    E0, MSG0
    __asm movups       E1, ABCD
    __asm sha1msg2     MSG1, MSG0
    __asm sha1rnds4    ABCD, E0, 1
    __asm sha1msg1     MSG3, MSG0
    __asm pxor 	       MSG2, MSG0
    /* Rounds 36-39 */
//    E1 = _mm_sha1nexte_epu32(E1, MSG1);
//    E0 = ABCD;
//    MSG2 = _mm_sha1msg2_epu32(MSG2, MSG1);
//    ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 1);
//    MSG0 = _mm_sha1msg1_epu32(MSG0, MSG1);
//    MSG3 = _mm_xor_si128(MSG3, MSG1);
    __asm sha1nexte    E1, MSG1
    __asm movups       E0, ABCD
    __asm sha1msg2     MSG2, MSG1
    __asm sha1rnds4    ABCD, E1, 1
    __asm sha1msg1     MSG0, MSG1
    __asm pxor 	       MSG3, MSG1
    /* Rounds 40-43 */
//    E0 = _mm_sha1nexte_epu32(E0, MSG2);
//    E1 = ABCD;
//    MSG3 = _mm_sha1msg2_epu32(MSG3, MSG2);
//    ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 2);
//    MSG1 = _mm_sha1msg1_epu32(MSG1, MSG2);
//    MSG0 = _mm_xor_si128(MSG0, MSG2);
    __asm sha1nexte    E0, MSG2
    __asm movups       E1, ABCD
    __asm sha1msg2     MSG3, MSG2
    __asm sha1rnds4    ABCD, E0, 2
    __asm sha1msg1     MSG1, MSG2
    __asm pxor 	       MSG0, MSG2
    /* Rounds 44-47 */
//    E1 = _mm_sha1nexte_epu32(E1, MSG3);
//    E0 = ABCD;
//    MSG0 = _mm_sha1msg2_epu32(MSG0, MSG3);
//    ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 2);
//    MSG2 = _mm_sha1msg1_epu32(MSG2, MSG3);
//    MSG1 = _mm_xor_si128(MSG1, MSG3);
    __asm sha1nexte    E1, MSG3
    __asm movups       E0, ABCD
    __asm sha1msg2     MSG0, MSG3
    __asm sha1rnds4    ABCD, E1, 2
    __asm sha1msg1     MSG2, MSG3
    __asm pxor 	       MSG1, MSG3
    /* Rounds 48-51 */
//    E0 = _mm_sha1nexte_epu32(E0, MSG0);
//    E1 = ABCD;
//    MSG1 = _mm_sha1msg2_epu32(MSG1, MSG0);
//    ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 2);
//    MSG3 = _mm_sha1msg1_epu32(MSG3, MSG0);
//    MSG2 = _mm_xor_si128(MSG2, MSG0);
    __asm sha1nexte    E0, MSG0
    __asm movups       E1, ABCD
    __asm sha1msg2     MSG1, MSG0
    __asm sha1rnds4    ABCD, E0, 2
    __asm sha1msg1     MSG3, MSG0
    __asm pxor 	       MSG2, MSG0
    /* Rounds 52-55 */
//    E1 = _mm_sha1nexte_epu32(E1, MSG1);
//    E0 = ABCD;
//    MSG2 = _mm_sha1msg2_epu32(MSG2, MSG1);
//    ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 2);
//    MSG0 = _mm_sha1msg1_epu32(MSG0, MSG1);
//    MSG3 = _mm_xor_si128(MSG3, MSG1);
    __asm sha1nexte    E1, MSG1
    __asm movups       E0, ABCD
    __asm sha1msg2     MSG2, MSG1
    __asm sha1rnds4    ABCD, E1, 2
    __asm sha1msg1     MSG0, MSG1
    __asm pxor 	       MSG3, MSG1
    /* Rounds 56-59 */
//    E0 = _mm_sha1nexte_epu32(E0, MSG2);
//    E1 = ABCD;
//    MSG3 = _mm_sha1msg2_epu32(MSG3, MSG2);
//    ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 2);
//    MSG1 = _mm_sha1msg1_epu32(MSG1, MSG2);
//    MSG0 = _mm_xor_si128(MSG0, MSG2);
    __asm sha1nexte    E0, MSG2
    __asm movups       E1, ABCD
    __asm sha1msg2     MSG3, MSG2
    __asm sha1rnds4    ABCD, E0, 2
    __asm sha1msg1     MSG1, MSG2
    __asm pxor 	       MSG0, MSG2
    /* Rounds 60-63 */
//    E1 = _mm_sha1nexte_epu32(E1, MSG3);
//    E0 = ABCD;
//    MSG0 = _mm_sha1msg2_epu32(MSG0, MSG3);
//    ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 3);
//    MSG2 = _mm_sha1msg1_epu32(MSG2, MSG3);
//    MSG1 = _mm_xor_si128(MSG1, MSG3);
    __asm sha1nexte    E1, MSG3
    __asm movups       E0, ABCD
    __asm sha1msg2     MSG0, MSG3
    __asm sha1rnds4    ABCD, E1, 3
    __asm sha1msg1     MSG2, MSG3
    __asm pxor 	       MSG1, MSG3
        /* Rounds 64-67 */
//    E0 = _mm_sha1nexte_epu32(E0, MSG0);
//    E1 = ABCD;
//    MSG1 = _mm_sha1msg2_epu32(MSG1, MSG0);
//    ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 3);
//    MSG3 = _mm_sha1msg1_epu32(MSG3, MSG0);
//    MSG2 = _mm_xor_si128(MSG2, MSG0);
    __asm sha1nexte    E0, MSG0
    __asm movups       E1, ABCD
    __asm sha1msg2     MSG1, MSG0
    __asm sha1rnds4    ABCD, E0, 3
    __asm sha1msg1     MSG3, MSG0
    __asm pxor 	       MSG2, MSG0
    /* Rounds 68-71 */
//    E1 = _mm_sha1nexte_epu32(E1, MSG1);
//    E0 = ABCD;
//    MSG2 = _mm_sha1msg2_epu32(MSG2, MSG1);
//    ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 3);
//    MSG3 = _mm_xor_si128(MSG3, MSG1);
    __asm sha1nexte    E1, MSG1
    __asm movups       E0, ABCD
    __asm sha1msg2     MSG2, MSG1
    __asm sha1rnds4    ABCD, E1, 3
    __asm pxor         MSG3, MSG1

    /* Rounds 72-75 */
//    E0 = _mm_sha1nexte_epu32(E0, MSG2);
//    E1 = ABCD;
//    MSG3 = _mm_sha1msg2_epu32(MSG3, MSG2);
//    ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 3);
    __asm sha1nexte    E0, MSG2
    __asm movups       E1, ABCD
    __asm sha1msg2     MSG3, MSG2
    __asm sha1rnds4    ABCD, E0, 3

    /* Rounds 76-79 */
//    E1 = _mm_sha1nexte_epu32(E1, MSG3);
//    E0 = ABCD;
//    ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 3);
    __asm sha1nexte    E1, MSG3
    __asm movups       E0, ABCD
    __asm sha1rnds4    ABCD, E1, 3
    /* Combine state */
//    E0 = _mm_sha1nexte_epu32(E0, E0_SAVE);
//    ABCD = _mm_add_epi32(ABCD, ABCD_SAVE);
    __asm sha1nexte    E0, [E_SAVE]
    __asm paddd        ABCD, [ABCD_SAVE]
    /* Save state */
//    ABCD = _mm_shuffle_epi32(ABCD, 0x1B);
//    _mm_storeu_si128((__m128i*) state, ABCD);
//    state[4] = _mm_extract_epi32(E0, 3);
    __asm pshufd      ABCD, ABCD, 0x1b
    __asm mov ecx,     [state]
    __asm movups       [ecx], ABCD
    __asm pextrd       [ecx + 16],E0,3

    __asm movups       xmm3, [r3]
    __asm movups       xmm4, [r4]
    __asm movups       xmm5, [r5]
    __asm movups       xmm6, [r6]
}
#endif
/*
 *  SHA1ProcessMessage_Block
 *
 *  Description:
 *      This function will process the next 512 bits of the message
 *      stored in the Message_Block array.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      Many of the variable names in the SHAContext, especially the
 *      single character names, were used because those were the names
 *      used in the publication.
 *
 *
 */
void SHA1ProcessMessage_Block(SHA1Context* context)
{
#if defined(_MSC_VER) || defined(__ORANGEC__) || defined(__clang__) || defined(__GNUC__) || defined(__INTEL_COMPILER)
if (has_sha_ni)
{
#ifdef TEST
    printf("shani\n");
#endif
    sha1_process_sha_ni(context->Message_Digest, context->Message_Block);
    context->Message_Block_Index = 0;
    return;
}
#endif
    const unsigned K[] = /* Constants defined in SHA-1   */
        {0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6};
    int t;                  /* Loop counter                 */
    unsigned temp;          /* Temporary word value         */
    unsigned W[80];         /* Word sequence                */
    unsigned A, B, C, D, E; /* Word buffers                 */

    /*
     *  Initialize the first 16 words in the array W
     */
    for (t = 0; t < 16; t++)
    {
        W[t] = ((unsigned)context->Message_Block[t * 4]) << 24;
        W[t] |= ((unsigned)context->Message_Block[t * 4 + 1]) << 16;
        W[t] |= ((unsigned)context->Message_Block[t * 4 + 2]) << 8;
        W[t] |= ((unsigned)context->Message_Block[t * 4 + 3]);
    }

    for (t = 16; t < 80; t++)
    {
        W[t] = SHA1CircularShift(1, W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16]);
    }

    A = context->Message_Digest[0];
    B = context->Message_Digest[1];
    C = context->Message_Digest[2];
    D = context->Message_Digest[3];
    E = context->Message_Digest[4];

    for (t = 0; t < 20; t++)
    {
        temp = SHA1CircularShift(5, A) + ((B & C) | ((~B) & D)) + E + W[t] + K[0];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    for (t = 20; t < 40; t++)
    {
        temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[1];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    for (t = 40; t < 60; t++)
    {
        temp = SHA1CircularShift(5, A) + ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    for (t = 60; t < 80; t++)
    {
        temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[3];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    context->Message_Digest[0] = (context->Message_Digest[0] + A) & 0xFFFFFFFF;
    context->Message_Digest[1] = (context->Message_Digest[1] + B) & 0xFFFFFFFF;
    context->Message_Digest[2] = (context->Message_Digest[2] + C) & 0xFFFFFFFF;
    context->Message_Digest[3] = (context->Message_Digest[3] + D) & 0xFFFFFFFF;
    context->Message_Digest[4] = (context->Message_Digest[4] + E) & 0xFFFFFFFF;

    context->Message_Block_Index = 0;
}

/*
 *  SHA1PadMessage
 *
 *  Description:
 *      According to the standard, the message must be padded to an even
 *      512 bits.  The first padding bit must be a '1'.  The last 64
 *      bits represent the length of the original message.  All bits in
 *      between should be 0.  This function will pad the message
 *      according to those rules by filling the Message_Block array
 *      accordingly.  It will also call SHA1ProcessMessage_Block()
 *      appropriately.  When it returns, it can be assumed that the
 *      message digest has been computed.
 *
 *  Parameters:
 *      context: [in/out]
 *          The context to pad
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *
 */
void SHA1PadMessage(SHA1Context* context)
{
    /*
     *  Check to see if the current message block is too small to hold
     *  the initial padding bits and length.  If so, we will pad the
     *  block, process it, and then continue padding into a second
     *  block.
     */
    if (context->Message_Block_Index > 55)
    {
        context->Message_Block[context->Message_Block_Index++] = 0x80;
        while (context->Message_Block_Index < 64)
        {
            context->Message_Block[context->Message_Block_Index++] = 0;
        }

        SHA1ProcessMessage_Block(context);

        while (context->Message_Block_Index < 56)
        {
            context->Message_Block[context->Message_Block_Index++] = 0;
        }
    }
    else
    {
        context->Message_Block[context->Message_Block_Index++] = 0x80;
        while (context->Message_Block_Index < 56)
        {
            context->Message_Block[context->Message_Block_Index++] = 0;
        }
    }

    /*
     *  Store the message length as the last 8 octets
     */
    context->Message_Block[56] = (context->Length_High >> 24) & 0xFF;
    context->Message_Block[57] = (context->Length_High >> 16) & 0xFF;
    context->Message_Block[58] = (context->Length_High >> 8) & 0xFF;
    context->Message_Block[59] = (context->Length_High) & 0xFF;
    context->Message_Block[60] = (context->Length_Low >> 24) & 0xFF;
    context->Message_Block[61] = (context->Length_Low >> 16) & 0xFF;
    context->Message_Block[62] = (context->Length_Low >> 8) & 0xFF;
    context->Message_Block[63] = (context->Length_Low) & 0xFF;

    SHA1ProcessMessage_Block(context);
}

#if TEST
static void test()
{
    has_sha_ni = false;
    SHA1Context context;
    SHA1Reset(&context);
    SHA1Input(&context, (unsigned char *)"HI there dave", 13);
    SHA1Result(&context);
    for (int i=0; i < 20; i++)
        printf("%02x", ((unsigned char *)context.Message_Digest)[i]);
printf("\n");
    has_sha_ni = true;
    SHA1Reset(&context);
    SHA1Input(&context, (unsigned char *)"HI there dave", 13);
    SHA1Result(&context);
    for (int i=0; i < 20; i++)
        printf("%02x", ((unsigned char *)context.Message_Digest)[i]);

}
#endif
}  // namespace DotNetPELib

#if TEST
int main()
{
#if 0
    static const __m128i(MASK) __attribute__((aligned(16))) = { 0x0c0d0e0f, 0x08090a0b, 0x04050607, 0x00010203 } ;
static int aa[4] =  { 0xa3a2a1a0, 0xa7a6a5a4, 0xabaaa9a8, 0xafaeacab };
static int bb[4] =  { 1,2,3,4 };
static int *state = aa;
    __asm movups      ABCD, [aa] 
    __asm pshufd      ABCD, ABCD,0x1b
    __asm movups      [bb],ABCD
    for (int i=0; i < 20; i++)
        printf("%02x", ((unsigned char *)bb)[i]);
printf("\n");
#endif
	DotNetPELib::test();
}
#endif

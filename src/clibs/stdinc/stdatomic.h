/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2008, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, with or without modification, are
    permitted provided that the following conditions are met:
    
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
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
    WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
    TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef __STDATOMIC_H
#define __STDATOMIC_H

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#ifndef __STDBOOL_H
#include <stdbool.h>
#endif

#ifndef __STDINT_H
#include <stdint.h>
#endif

#ifdef __cplusplus
namespace __STD_NS__ {
#endif


typedef enum memory_order {
    memory_order_relaxed = 1,
    memory_order_acquire,
    memory_order_release,
    memory_order_acq_rel,
    memory_order_seq_cst   
} memory_order ;

#define ATOMIC_BOOL_LOCK_FREE       1
#define ATOMIC_CHAR_LOCK_FREE       1
#define ATOMIC_CHAR16_T_LOCK_FREE   1
#define ATOMIC_CHAR32_T_LOCK_FREE   1
#define ATOMIC_WCHAR_T_LOCK_FREE    1
#define ATOMIC_SHORT_LOCK_FREE      1
#define ATOMIC_INT_LOCK_FREE        1
#define ATOMIC_LONG_LOCK_FREE       1
#define ATOMIC_LLONG_LOCK_FREE      0
#define ATOMIC_POINTER_LOCK_FREE    1

#define atomic_is_lock_free(A) _Generic(A, \
    bool: ATOMIC_BOOL_LOCK_FREE, \
    char: ATOMIC_CHAR_LOCK_FREE,  \ 
    unsigned char: ATOMIC_CHAR_LOCK_FREE,   \
    short: ATOMIC_SHORT_LOCK_FREE,   \
    unsigned short: ATOMIC_SHORT_LOCK_FREE,   \
    int: ATOMIC_INT_LOCK_FREE,   \
    unsigned int: ATOMIC_INT_LOCK_FREE,   \
    long: ATOMIC_LONG_LOCK_FREE,   \
    unsigned long: ATOMIC_LONG_LOCK_FREE,   \
    long long: ATOMIC_LONG_LONG_LOCK_FREE,   \
    unsigned long long: ATOMIC_LONG_LONG_LOCK_FREE,   \
    char16_t: ATOMIC_CHAR16_T_LOCK_FREE,\
    char32_t: ATOMIC_CHAR32_T_LOCK_FREE,\
    char *: ATOMIC_POINTER_LOCK_FREE, \
    unsigned char *: ATOMIC_POINTER_LOCK_FREE,   \
    short *: ATOMIC_POINTER_LOCK_FREE,   \
    unsigned short *: ATOMIC_POINTER_LOCK_FREE,   \
    int *: ATOMIC_POINTER_LOCK_FREE,   \
    unsigned int *: ATOMIC_POINTER_LOCK_FREE,   \
    long *: ATOMIC_POINTER_LOCK_FREE,   \
    unsigned long *: ATOMIC_POINTER_LOCK_FREE,   \
    long long *: ATOMIC_POINTER_LOCK_FREE,   \
    unsigned long long *: ATOMIC_POINTER_LOCK_FREE,   \
    char16_t *: ATOMIC_POINTER_LOCK_FREE,\
    char32_t *: ATOMIC_POINTER_LOCK_FREE,\
    default: 0)

#define ATOMIC_FLAG_INIT  { 0 }
#define ATOMIC_VAR_INIT(x) __atomic_var_init(x)

typedef struct atomic_flag
{
    unsigned __f__;
} atomic_flag;

#define atomic_flag_test_and_set_explicit(object, order) __atomic_flag_test_set(object, order)
#define atomic_flag_test_and_set(object) __atomic_flag_test_set(object, memory_order_seq_cst)
#define atomic_flag_clear_explicit(object, order) __atomic_flag_clear(object, order)
#define atomic_flag_clear(object) __atomic_flag_clear(object, memory_order_seq_cst)

#define atomic_thread_fence(order) __atomic_fence(order)
#define atomic_signal_fence(order) __atomic_fence(order)

#define __ATOMIC_TYPE__(__x__, __y__) typedef _Atomic(__x__) __y__;

__ATOMIC_TYPE__(unsigned char, atomic_bool);
__ATOMIC_TYPE__(void *, atomic_address);
__ATOMIC_TYPE__(char, atomic_char);
__ATOMIC_TYPE__(signed char, atomic_schar);
__ATOMIC_TYPE__(unsigned char, atomic_uchar);
__ATOMIC_TYPE__(short, atomic_short);
__ATOMIC_TYPE__(unsigned short, atomic_ushort);
__ATOMIC_TYPE__(int, atomic_int);
__ATOMIC_TYPE__(unsigned int, atomic_uint);
__ATOMIC_TYPE__(long, atomic_long);
__ATOMIC_TYPE__(unsigned long, atomic_ulong);
__ATOMIC_TYPE__(long long, atomic_llong);
__ATOMIC_TYPE__(unsigned long long, atomic_ullong);
__ATOMIC_TYPE__(__char16_t, atomic_char16_t);
__ATOMIC_TYPE__(__char32_t, atomic_char32_t);
__ATOMIC_TYPE__(wchar_t, atomic_wchar_t);
__ATOMIC_TYPE__(int_least8_t, atomic_int_least8_t);
__ATOMIC_TYPE__(uint_least8_t, atomic_uint_least8_t);
__ATOMIC_TYPE__(int_least16_t, atomic_int_least16_t);
__ATOMIC_TYPE__(uint_least16_t, atomic_uint_least16_t);
__ATOMIC_TYPE__(int_least32_t, atomic_int_least32_t);
__ATOMIC_TYPE__(uint_least32_t, atomic_uint_least32_t);
__ATOMIC_TYPE__(int_least64_t, atomic_int_least64_t);
__ATOMIC_TYPE__(uint_least64_t, atomic_uint_least64_t);
__ATOMIC_TYPE__(int_fast8_t, atomic_int_fast8_t);
__ATOMIC_TYPE__(uint_fast8_t, atomic_uint_fast8_t);
__ATOMIC_TYPE__(int_fast16_t, atomic_int_fast16_t);
__ATOMIC_TYPE__(uint_fast16_t, atomic_uint_fast16_t);
__ATOMIC_TYPE__(int_fast32_t, atomic_int_fast32_t);
__ATOMIC_TYPE__(uint_fast32_t, atomic_uint_fast32_t);
__ATOMIC_TYPE__(int_fast64_t, atomic_int_fast64_t);
__ATOMIC_TYPE__(uint_fast64_t, atomic_uint_fast64_t);
__ATOMIC_TYPE__(intptr_t, atomic_intptr_t);
__ATOMIC_TYPE__(uintptr_t, atomic_uintptr_t);
__ATOMIC_TYPE__(size_t, atomic_size_t);
__ATOMIC_TYPE__(ptrdiff_t, atomic_ptrdiff_t);
__ATOMIC_TYPE__(intmax_t, atomic_intmax_t);
__ATOMIC_TYPE__(uintmax_t, atomic_uintmax_t);


#define kill_dependency(y) __kill_dependency(y)

#define atomic_init( __a__, __v__) \
__atomic_var_init(__v__, __a__)

#define atomic_load( __a__ ) \
__atomic_load( __a__, memory_order_seq_cst )

#define atomic_load_explicit( __a__, __x__ ) \
__atomic_load( __a__, __x__ )

#define atomic_store( __a__, __m__ ) \
__atomic_store( __a__, __m__, memory_order_seq_cst )

#define atomic_store_explicit( __a__, __m__, __x__ ) \
__atomic_store( __a__, __m__, __x__ )

#define atomic_swap( __a__, __m__ ) \
__atomic_modify( __a__, =, __m__, memory_order_seq_cst )

#define atomic_swap_explicit( __a__, __m__, __x__ ) \
__atomic_modify( __a__, =, __m__, __x__ )

#define atomic_compare_swap( __a__, __e__, __m__ ) \
__atomic_cmpswp( __a__, __e__, __m__, memory_order_seq_cst, memory_order_seq_cst )

#define atomic_compare_swap_explicit( __a__, __e__, __m__, __x__, __y__ ) \
__atomic_cmpswp( __a__, __e__, __m__, __x__, __y__ )

#define atomic_fetch_add_explicit( __a__, __m__, __x__ ) \
__atomic_modify( __a__, +=, __m__, __x__ )

#define atomic_fetch_add( __a__, __m__ ) \
__atomic_modify( __a__, +=, __m__, memory_order_seq_cst )

#define atomic_fetch_sub_explicit( __a__, __m__, __x__ ) \
__atomic_modify( __a__, -=, __m__, __x__ )

#define atomic_fetch_sub( __a__, __m__ ) \
__atomic_modify( __a__, -=, __m__, memory_order_seq_cst )

#define atomic_fetch_or_explicit( __a__, __m__, __x__ ) \
__atomic_modify( __a__, |=, __m__, __x__ )

#define atomic_fetch_or( __a__, __m__ ) \
__atomic_modify( __a__, |=, __m__, memory_order_seq_cst )

#define atomic_fetch_and_explicit( __a__, __m__, __x__ ) \
__atomic_modify( __a__, &=, __m__, __x__ )

#define atomic_fetch_and( __a__, __m__ ) \
__atomic_modify( __a__, &=, __m__, memory_order_seq_cst )

#define atomic_fetch_xor_explicit( __a__, __m__, __x__ ) \
__atomic_modify( __a__, ^=, __m__, __x__ )

#define atomic_fetch_xor( __a__, __m__ ) \
__atomic_modify( __a__, ^=, __m__, memory_order_seq_cst )

#ifdef __cplusplus
}
#endif
#endif



/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
 *  
 *      This file is part of the Orange C Compiler package.
 *  
 *      The Orange C Compiler package is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *  
 *      The Orange C Compiler package is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *  
 *      You should have received a copy of the GNU General Public License
 *      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *      contact information:
 *          email: TouchStone222@runbox.com <David Lindauer>
 *  
 */

#ifndef __STDATOMIC_H
#define __STDATOMIC_H


#if defined(__cplusplus) && !defined(_LIBCPP_BUILDING_LIBRARY)
#    pragma error \
        "This header uses C11 and greater constructs at this time, this error is to inform you of this so you understand why it breaks instantly"
#endif

#ifndef __STDDEF_H
#    include <stddef.h>
#endif

#ifndef __STDBOOL_H
#    include <stdbool.h>
#endif

#ifndef __STDINT_H
#    include <stdint.h>
#endif

#ifndef RC_INVOKED
typedef enum memory_order
{
    memory_order_relaxed,
    memory_order_consume,
    memory_order_acquire,
    memory_order_release,
    memory_order_acq_rel,
    memory_order_seq_cst
} memory_order;
#endif


#ifndef RC_INVOKED
typedef struct atomic_flag
{
    unsigned char __f__;
} atomic_flag;
#endif
#define atomic_is_lock_free(A) \
    _Generic(A, \
    struct atomic_flag: 1, \
    _Bool: ATOMIC_BOOL_LOCK_FREE == 2, \
    char: ATOMIC_CHAR_LOCK_FREE == 2,  \ 
    signed char: ATOMIC_CHAR_LOCK_FREE == 2,  \ 
    unsigned char: ATOMIC_CHAR_LOCK_FREE == 2,   \
    short: ATOMIC_SHORT_LOCK_FREE == 2,   \
    unsigned short: ATOMIC_SHORT_LOCK_FREE == 2,   \
    int: ATOMIC_INT_LOCK_FREE == 2,   \
    unsigned int: ATOMIC_INT_LOCK_FREE == 2,   \
    long: ATOMIC_LONG_LOCK_FREE == 2,   \
    unsigned long: ATOMIC_LONG_LOCK_FREE == 2,   \
    long long: ATOMIC_LLONG_LOCK_FREE == 2,   \
    unsigned long long: ATOMIC_LLONG_LOCK_FREE == 2,   \
    __char16_t: ATOMIC_CHAR16_T_LOCK_FREE == 2,\
    __char32_t: ATOMIC_CHAR32_T_LOCK_FREE == 2,\
    wchar_t: ATOMIC_WCHAR_T_LOCK_FREE == 2,\
    struct atomic_flag *: 1, \ 
    void *: ATOMIC_POINTER_LOCK_FREE == 2, \
    _Bool *: ATOMIC_BOOL_LOCK_FREE == 2, \
    char *: ATOMIC_POINTER_LOCK_FREE == 2, \
    signed char *: ATOMIC_POINTER_LOCK_FREE == 2, \
    unsigned char *: ATOMIC_POINTER_LOCK_FREE == 2,   \
    short *: ATOMIC_POINTER_LOCK_FREE == 2,   \
    unsigned short *: ATOMIC_POINTER_LOCK_FREE == 2,   \
    int *: ATOMIC_POINTER_LOCK_FREE == 2,   \
    unsigned int *: ATOMIC_POINTER_LOCK_FREE == 2,   \
    long *: ATOMIC_POINTER_LOCK_FREE == 2,   \
    unsigned long *: ATOMIC_POINTER_LOCK_FREE == 2,   \
    long long *: ATOMIC_POINTER_LOCK_FREE == 2,   \
    unsigned long long *: ATOMIC_POINTER_LOCK_FREE == 2,   \
    __char16_t *: ATOMIC_POINTER_LOCK_FREE == 2,\
    __char32_t *: ATOMIC_POINTER_LOCK_FREE == 2,\
    wchar_t *: ATOMIC_POINTER_LOCK_FREE == 2,\
    void **: ATOMIC_POINTER_LOCK_FREE == 2, \
    default: 0)

#define ATOMIC_FLAG_INIT \
    {                    \
        0                \
    }
#define ATOMIC_VAR_INIT(x) __c11_atomic_init(x)

#ifndef RC_INVOKED

#ifdef __cplusplus
extern "C"
{
#endif

    void _RTL_FUNC atomic_thread_fence(int);
    void _RTL_FUNC atomic_signal_fence(int);
    unsigned char _RTL_FUNC atomic_flag_test_and_set(atomic_flag*);
    unsigned char _RTL_FUNC atomic_flag_test_and_set_explicit(atomic_flag*, int);
    void _RTL_FUNC atomic_flag_clear(atomic_flag*);
    void _RTL_FUNC atomic_flag_clear_explicit(atomic_flag*, int);
#endif
#ifdef __cplusplus
}
#endif

#define atomic_flag_test_and_set_explicit(object, order) __atomic_flag_test_set(object, order)
#define atomic_flag_test_and_set(object) __atomic_flag_test_set(object, memory_order_seq_cst)
#define atomic_flag_clear_explicit(object, order) __atomic_flag_clear(object, order)
#define atomic_flag_clear(object) __atomic_flag_clear(object, memory_order_seq_cst)

#define atomic_thread_fence(order) __c11_atomic_thread_fence(order)
#define atomic_signal_fence(order) __c11_atomic_signal_fence(order)

#define __ATOMIC_TYPE__(__x__, __y__) typedef _Atomic(__x__) __y__;

#ifndef RC_INVOKED
__ATOMIC_TYPE__(_Bool, atomic_bool);
__ATOMIC_TYPE__(void*, atomic_address);
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
#endif
#define kill_dependency(y) __kill_dependency(y)

#define atomic_init(__a__, __v__) __c11_atomic_init(__a__, __v__)

#define atomic_load(__a__) __c11_atomic_load(__a__, memory_order_seq_cst)

#define atomic_load_explicit(__a__, __x__) __c11_atomic_load(__a__, __x__)

#define atomic_store(__a__, __m__) __c11_atomic_store(__a__, __m__, memory_order_seq_cst)

#define atomic_store_explicit(__a__, __m__, __x__) __c11_atomic_store(__a__, __m__, __x__)

#define atomic_exchange(__a__, __m__) __c11_atomic_exchange(__a__, __m__, memory_order_seq_cst)

#define atomic_exchange_explicit(__a__, __m__, __x__) __c11_atomic_exchange(__a__, __m__, __x__)

#define atomic_compare_exchange_strong(__a__, __e__, __m__) \
    __c11_atomic_compare_exchange_strong(__a__, __e__, __m__, memory_order_seq_cst, memory_order_seq_cst)

#define atomic_compare_exchange_strong_explicit(__a__, __e__, __m__, __x__, __y__) \
    __c11_atomic_compare_exchange_strong(__a__, __e__, __m__, __x__, __y__)

#define atomic_compare_exchange_weak(__a__, __e__, __m__) \
    __c11_atomic_compare_exchange_weak(__a__, __e__, __m__, memory_order_seq_cst, memory_order_seq_cst)

#define atomic_compare_exchange_weak_explicit(__a__, __e__, __m__, __x__, __y__) \
    __c11_atomic_compare_exchange_weak(__a__, __e__, __m__, __x__, __y__)
		
#define atomic_fetch_add_explicit(__a__, __m__, __x__) __c11_atomic_fetch_add(__a__, __m__, __x__)

#define atomic_fetch_add(__a__, __m__) __c11_atomic_fetch_add(__a__, __m__, memory_order_seq_cst)

#define atomic_fetch_sub_explicit(__a__, __m__, __x__) __c11_atomic_fetch_sub(__a__, __m__, __x__)

#define atomic_fetch_sub(__a__, __m__) __c11_atomic_fetch_sub(__a__, __m__, memory_order_seq_cst)

#define atomic_fetch_or_explicit(__a__, __m__, __x__) __c11_atomic_fetch_or(__a__, __m__, __x__)

#define atomic_fetch_or(__a__, __m__) __c11_atomic_fetch_or(__a__, __m__, memory_order_seq_cst)

#define atomic_fetch_and_explicit(__a__, __m__, __x__) __c11_atomic_fetch_and(__a__, __m__, __x__)

#define atomic_fetch_and(__a__, __m__) __c11_atomic_fetch_and(__a__, __m__, memory_order_seq_cst)

#define atomic_fetch_xor_explicit(__a__, __m__, __x__) __c11_atomic_fetch_xor(__a__, __m__, __x__)

#define atomic_fetch_xor(__a__, __m__) __c11_atomic_fetch_xor(__a__, __m__, memory_order_seq_cst)

#endif

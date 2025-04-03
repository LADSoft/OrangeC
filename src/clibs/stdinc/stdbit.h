/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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

#pragma GCC system_header

#ifndef __STDBIT_H
#    define __STDBIT_H

#    include <_defs.h>

#ifdef __cplusplus
extern "C"
{
#endif

unsigned int _RTL_FUNC stdc_leading_zeros_uc(unsigned char value);
unsigned int _RTL_FUNC stdc_leading_zeros_us(unsigned short value);
unsigned int _RTL_FUNC stdc_leading_zeros_ui(unsigned int value);
unsigned int _RTL_FUNC stdc_leading_zeros_ul(unsigned long value);
unsigned int _RTL_FUNC stdc_leading_zeros_ull(unsigned long long value);
#define stdc_leading_zeros(arg) \
   ((__typeid(arg) == 1) ? (stdc_leading_zeros_uc)(arg) : \  
   (__typeid(arg) == 2) ? (stdc_leading_zeros_us)(arg) : \  
   (__typeid(arg) == 3) ? (stdc_leading_zeros_ui)(arg) : \  
   (__typeid(arg) == 4) ? (stdc_leading_zeros_ul)(arg) : \  
   (__typeid(arg) == 5) ? (stdc_leading_zeros_ull)(arg) : 0)

unsigned int _RTL_FUNC stdc_leading_ones_uc(unsigned char value);
unsigned int _RTL_FUNC stdc_leading_ones_us(unsigned short value);
unsigned int _RTL_FUNC stdc_leading_ones_ui(unsigned int value);
unsigned int _RTL_FUNC stdc_leading_ones_ul(unsigned long value);
unsigned int _RTL_FUNC stdc_leading_ones_ull(unsigned long long value);
#define stdc_leading_ones(arg) \
   ((__typeid(arg) == 1) ? (stdc_leading_ones_uc)(arg) : \  
   (__typeid(arg) == 2) ? (stdc_leading_ones_us)(arg) : \  
   (__typeid(arg) == 3) ? (stdc_leading_ones_ui)(arg) : \  
   (__typeid(arg) == 4) ? (stdc_leading_ones_ul)(arg) : \  
   (__typeid(arg) == 5) ? (stdc_leading_ones_ull)(arg) : 0)

unsigned int _RTL_FUNC stdc_trailing_zeros_uc(unsigned char value);
unsigned int _RTL_FUNC stdc_trailing_zeros_us(unsigned short value);
unsigned int _RTL_FUNC stdc_trailing_zeros_ui(unsigned int value);
unsigned int _RTL_FUNC stdc_trailing_zeros_ul(unsigned long value);
unsigned int _RTL_FUNC stdc_trailing_zeros_ull(unsigned long long value);
#define stdc_trailing_zeros(arg) \
   ((__typeid(arg) == 1) ? (stdc_trailing_zeros_uc)(arg) : \  
   (__typeid(arg) == 2) ? (stdc_trailing_zeros_us)(arg) : \  
   (__typeid(arg) == 3) ? (stdc_trailing_zeros_ui)(arg) : \  
   (__typeid(arg) == 4) ? (stdc_trailing_zeros_ul)(arg) : \  
   (__typeid(arg) == 5) ? (stdc_trailing_zeros_ull)(arg) : 0)

unsigned int _RTL_FUNC stdc_trailing_ones_uc(unsigned char value);
unsigned int _RTL_FUNC stdc_trailing_ones_us(unsigned short value);
unsigned int _RTL_FUNC stdc_trailing_ones_ui(unsigned int value);
unsigned int _RTL_FUNC stdc_trailing_ones_ul(unsigned long value);
unsigned int _RTL_FUNC stdc_trailing_ones_ull(unsigned long long value);
#define stdc_trailing_ones(arg) \
   ((__typeid(arg) == 1) ? (stdc_trailing_ones_uc)(arg) : \  
   (__typeid(arg) == 2) ? (stdc_trailing_ones_us)(arg) : \  
   (__typeid(arg) == 3) ? (stdc_trailing_ones_ui)(arg) : \  
   (__typeid(arg) == 4) ? (stdc_trailing_ones_ul)(arg) : \  
   (__typeid(arg) == 5) ? (stdc_trailing_ones_ull)(arg) : 0)

unsigned int _RTL_FUNC stdc_first_leading_zero_uc(unsigned char value);
unsigned int _RTL_FUNC stdc_first_leading_zero_us(unsigned short value);
unsigned int _RTL_FUNC stdc_first_leading_zero_ui(unsigned int value);
unsigned int _RTL_FUNC stdc_first_leading_zero_ul(unsigned long value);
unsigned int _RTL_FUNC stdc_first_leading_zero_ull(unsigned long long value);
#define stdc_first_leading_zero(arg) \
   ((__typeid(arg) == 1) ? (stdc_first_leading_zero_uc)(arg) : \  
   (__typeid(arg) == 2) ? (stdc_first_leading_zero_us)(arg) : \  
   (__typeid(arg) == 3) ? (stdc_first_leading_zero_ui)(arg) : \  
   (__typeid(arg) == 4) ? (stdc_first_leading_zero_ul)(arg) : \  
   (__typeid(arg) == 5) ? (stdc_first_leading_zero_ull)(arg) : 0)

unsigned int _RTL_FUNC stdc_first_leading_one_uc(unsigned char value);
unsigned int _RTL_FUNC stdc_first_leading_one_us(unsigned short value);
unsigned int _RTL_FUNC stdc_first_leading_one_ui(unsigned int value);
unsigned int _RTL_FUNC stdc_first_leading_one_ul(unsigned long value);
unsigned int _RTL_FUNC stdc_first_leading_one_ull(unsigned long long value);
#define stdc_first_leading_one(arg) \
   ((__typeid(arg) == 1) ? (stdc_first_leading_one_uc)(arg) : \  
   (__typeid(arg) == 2) ? (stdc_first_leading_one_us)(arg) : \  
   (__typeid(arg) == 3) ? (stdc_first_leading_one_ui)(arg) : \  
   (__typeid(arg) == 4) ? (stdc_first_leading_one_ul)(arg) : \  
   (__typeid(arg) == 5) ? (stdc_first_leading_one_ull)(arg) : 0)

unsigned int _RTL_FUNC stdc_first_trailing_zero_uc(unsigned char value);
unsigned int _RTL_FUNC stdc_first_trailing_zero_us(unsigned short value);
unsigned int _RTL_FUNC stdc_first_trailing_zero_ui(unsigned int value);
unsigned int _RTL_FUNC stdc_first_trailing_zero_ul(unsigned long value);
unsigned int _RTL_FUNC stdc_first_trailing_zero_ull(unsigned long long value);
#define stdc_first_trailing_zero(arg) \
   ((__typeid(arg) == 1) ? (stdc_first_trailing_zero_uc)(arg) : \  
   (__typeid(arg) == 2) ? (stdc_first_trailing_zero_us)(arg) : \  
   (__typeid(arg) == 3) ? (stdc_first_trailing_zero_ui)(arg) : \  
   (__typeid(arg) == 4) ? (stdc_first_trailing_zero_ul)(arg) : \  
   (__typeid(arg) == 5) ? (stdc_first_trailing_zero_ull)(arg) : 0)

unsigned int _RTL_FUNC stdc_first_trailing_one_uc(unsigned char value);
unsigned int _RTL_FUNC stdc_first_trailing_one_us(unsigned short value);
unsigned int _RTL_FUNC stdc_first_trailing_one_ui(unsigned int value);
unsigned int _RTL_FUNC stdc_first_trailing_one_ul(unsigned long value);
unsigned int _RTL_FUNC stdc_first_trailing_one_ull(unsigned long long value);
#define stdc_first_trailing_one(arg) \
   ((__typeid(arg) == 1) ? (stdc_first_trailing_one_uc)(arg) : \  
   (__typeid(arg) == 2) ? (stdc_first_trailing_one_us)(arg) : \  
   (__typeid(arg) == 3) ? (stdc_first_trailing_one_ui)(arg) : \  
   (__typeid(arg) == 4) ? (stdc_first_trailing_one_ul)(arg) : \  
   (__typeid(arg) == 5) ? (stdc_first_trailing_one_ull)(arg) : 0)

unsigned int _RTL_FUNC stdc_count_zeros_uc(unsigned char value);
unsigned int _RTL_FUNC stdc_count_zeros_us(unsigned short value);
unsigned int _RTL_FUNC stdc_count_zeros_ui(unsigned int value);
unsigned int _RTL_FUNC stdc_count_zeros_ul(unsigned long value);
unsigned int _RTL_FUNC stdc_count_zeros_ull(unsigned long long value);
#define stdc_count_zeros(arg) \
   ((__typeid(arg) == 1) ? (stdc_count_zeros_uc)(arg) : \  
   (__typeid(arg) == 2) ? (stdc_count_zeros_us)(arg) : \  
   (__typeid(arg) == 3) ? (stdc_count_zeros_ui)(arg) : \  
   (__typeid(arg) == 4) ? (stdc_count_zeros_ul)(arg) : \  
   (__typeid(arg) == 5) ? (stdc_count_zeros_ull)(arg) : 0)

unsigned int _RTL_FUNC stdc_count_ones_uc(unsigned char value);
unsigned int _RTL_FUNC stdc_count_ones_us(unsigned short value);
unsigned int _RTL_FUNC stdc_count_ones_ui(unsigned int value);
unsigned int _RTL_FUNC stdc_count_ones_ul(unsigned long value);
unsigned int _RTL_FUNC stdc_count_ones_ull(unsigned long long value);
#define stdc_count_ones(arg) \
   ((__typeid(arg) == 1) ? (stdc_count_ones_uc)(arg) : \  
   (__typeid(arg) == 2) ? (stdc_count_ones_us)(arg) : \  
   (__typeid(arg) == 3) ? (stdc_count_ones_ui)(arg) : \  
   (__typeid(arg) == 4) ? (stdc_count_ones_ul)(arg) : \  
   (__typeid(arg) == 5) ? (stdc_count_ones_ull)(arg) : 0)

bool _RTL_FUNC stdc_has_single_bit_uc(unsigned char value);
bool _RTL_FUNC stdc_has_single_bit_us(unsigned short value);
bool _RTL_FUNC stdc_has_single_bit_ui(unsigned int value);
bool _RTL_FUNC stdc_has_single_bit_ul(unsigned long value);
bool _RTL_FUNC stdc_has_single_bit_ull(unsigned long long value);
#define stdc_has_single_bit(arg) \
   ((__typeid(arg) == 1) ? (stdc_has_single_bit_uc)(arg) : \  
   (__typeid(arg) == 2) ? (stdc_has_single_bit_us)(arg) : \  
   (__typeid(arg) == 3) ? (stdc_has_single_bit_ui)(arg) : \  
   (__typeid(arg) == 4) ? (stdc_has_single_bit_ul)(arg) : \  
   (__typeid(arg) == 5) ? (stdc_has_single_bit_ull)(arg) : 0)

unsigned int _RTL_FUNC stdc_bit_width_uc(unsigned char value);
unsigned int _RTL_FUNC stdc_bit_width_us(unsigned short value);
unsigned int _RTL_FUNC stdc_bit_width_ui(unsigned int value);
unsigned int _RTL_FUNC stdc_bit_width_ul(unsigned long value);
unsigned int _RTL_FUNC stdc_bit_width_ull(unsigned long long value);
#define stdc_bit_width(arg) \
   ((__typeid(arg) == 1) ? (stdc_bit_width_uc)(arg) : \  
   (__typeid(arg) == 2) ? (stdc_bit_width_us)(arg) : \  
   (__typeid(arg) == 3) ? (stdc_bit_width_ui)(arg) : \  
   (__typeid(arg) == 4) ? (stdc_bit_width_ul)(arg) : \  
   (__typeid(arg) == 5) ? (stdc_bit_width_ull)(arg) : 0)

unsigned char _RTL_FUNC stdc_bit_floor_uc(unsigned char value);
unsigned short _RTL_FUNC stdc_bit_floor_us(unsigned short value);
unsigned int _RTL_FUNC stdc_bit_floor_ui(unsigned int value);
unsigned long _RTL_FUNC stdc_bit_floor_ul(unsigned long value);
unsigned long long _RTL_FUNC stdc_bit_floor_ull(unsigned long long value);
#define stdc_bit_floor(arg) \
   ((__typeid(arg) == 1) ? (stdc_bit_floor_uc)(arg) : \  
   (__typeid(arg) == 2) ? (stdc_bit_floor_us)(arg) : \  
   (__typeid(arg) == 3) ? (stdc_bit_floor_ui)(arg) : \  
   (__typeid(arg) == 4) ? (stdc_bit_floor_ul)(arg) : \  
   (__typeid(arg) == 5) ? (stdc_bit_floor_ull)(arg) : 0)

unsigned char _RTL_FUNC stdc_bit_ceil_uc(unsigned char value);
unsigned short _RTL_FUNC stdc_bit_ceil_us(unsigned short value);
unsigned int _RTL_FUNC stdc_bit_ceil_ui(unsigned int value);
unsigned long _RTL_FUNC stdc_bit_ceil_ul(unsigned long value);
unsigned long long _RTL_FUNC stdc_bit_ceil_ull(unsigned long long value);
#define stdc_bit_ceil(arg) \
   ((__typeid(arg) == 1) ? (stdc_bit_ceil_uc)(arg) : \  
   (__typeid(arg) == 2) ? (stdc_bit_ceil_us)(arg) : \  
   (__typeid(arg) == 3) ? (stdc_bit_ceil_ui)(arg) : \  
   (__typeid(arg) == 4) ? (stdc_bit_ceil_ul)(arg) : \  
   (__typeid(arg) == 5) ? (stdc_bit_ceil_ull)(arg) : 0)

#ifdef __cplusplus
}
#endif

#endif /* __STDBIT_H */

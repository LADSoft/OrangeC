/*
 * Copyright (c) 1997
 * Moscow Center for SPARC Technology
 *
 * Copyright (c) 1999
 * Boris Fomitchev
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */

/*
 * Purpose of this file :
 *
 * A list of COMPILER-SPECIFIC portion of STLport settings.
 * This file is provided to help in manual configuration
 * of STLport. This file is being included by stlcomp.h
 * when STLport is unable to identify your compiler.
 * Please remove the error diagnostic below before adjusting
 * macros.
 *
 */
#ifndef _STLP_MYCOMP_H
#define  _STLP_MYCOMP_H

//==========================================================

#undef _STLP_NO_UNEXPECTED_EXCEPT_SUPPORT

// the values choosen here as defaults try to give
// maximum functionality on the most conservative settings

// Uncomment if long long is available
// #  define _STLP_LONG_LONG long long


// Uncomment this if your compiler has limited or no default template arguments for classes
// #  define _STLP_LIMITED_DEFAULT_TEMPLATES 1

// Uncomment this if your compiler support only complete (not dependent on other parameters)
// types as default parameters for class templates
// #  define _STLP_DEFAULT_TYPE_PARAM 1

// Uncomment this if your compiler has problem with not-type
// default template parameters
// #  define _STLP_NO_DEFAULT_NON_TYPE_PARAM 1

// Define if compiler has
// trouble with functions getting non-type-parameterized classes as parameters
// #  define _STLP_NON_TYPE_TMPL_PARAM_BUG 1

// Uncomment this when you are able to detect that the user do not
// want to use the exceptions feature.
#if __RTTI__ != 1
#  define _STLP_DONT_USE_EXCEPTIONS 1
#endif

// Define this if your compiler do not throw bad_alloc from the new operator
// #  define _STLP_NEW_DONT_THROW_BAD_ALLOC  1

// Define this if your compiler has no rtti support or if it has been disabled
#if __RTTI__ != 1
#  define _STLP_NO_RTTI 1
#endif

// Uncomment if member template methods are not available
// #  define _STLP_NO_MEMBER_TEMPLATES   1

// Uncomment if member template classes are not available
// #  define _STLP_NO_MEMBER_TEMPLATE_CLASSES   1

// Uncomment if your compiler do not support the std::allocator rebind technique
// This is a special case of bad member template classes support, it is automatically
// defined if _STLP_NO_MEMBER_TEMPLATE_CLASSES is defined.
// # define _STLP_DONT_SUPPORT_REBIND_MEMBER_TEMPLATE 1

// Compiler has problems specializing members of partially
// specialized class
// #  define _STLP_MEMBER_SPECIALIZATION_BUG

// Uncomment if specialization of methods is not allowed
// #  define _STLP_NO_METHOD_SPECIALIZATION  1

// Uncomment if full  specialization does not use partial spec. syntax : template <> struct ....
// #  define _STLP_NO_PARTIAL_SPECIALIZATION_SYNTAX  1


// Uncomment this if your compiler can't inline while(), for()
// #  define _STLP_LOOP_INLINE_PROBLEMS 1

// Define if the compiler fails to match a template function argument of base
// #  define _STLP_BASE_MATCH_BUG          1

// Define if the compiler fails to match a template function argument of base
// (non-template)
//#  define  _STLP_NONTEMPL_BASE_MATCH_BUG 1

// Define if the compiler rejects outline method definition
// explicitly taking nested types/typedefs
// #  define _STLP_NESTED_TYPE_PARAM_BUG   1

// Compiler requires typename keyword on outline method definition
// explicitly taking nested types/typedefs
// #define  _STLP_TYPENAME_ON_RETURN_TYPE

// if your compiler have serious problems with typedefs, try this one
// #  define _STLP_BASE_TYPEDEF_BUG          1

// Uncomment if getting errors compiling mem_fun* adaptors
// #  define _STLP_MEMBER_POINTER_PARAM_BUG 1

// Defined if the compiler has trouble calling POD-types constructors/destructors
// #  define _STLP_TRIVIAL_CONSTRUCTOR_BUG    1
// #  define _STLP_TRIVIAL_DESTRUCTOR_BUG    1

// Define if having problems specializing maps/sets with
// key type being const
// #  define _STLP_MULTI_CONST_TEMPLATE_ARG_BUG

// Uncomment this to disble at() member functions for containers
// #  define   _STLP_NO_AT_MEMBER_FUNCTION 1

// Define this if compiler lacks <exception> header
// #  define _STLP_NO_EXCEPTION_HEADER 1

// uncomment if new-style headers <new> is available
// #  define _STLP_HAS_NEW_NEW_HEADER 1

// uncomment this if your compiler consider as ambiguous a function imported within
// the stlport namespace and called without scope (:: or std::)
// #  define _STLP_NO_USING_FOR_GLOBAL_FUNCTIONS 1

// uncomment this if your compiler define all the C math functions C++ additional
// overloads in ::namespace and not only in std::.
// #  define _STLP_HAS_GLOBAL_C_MATH_FUNCTIONS 1

// Edit relative path below (or put full path) to get native
// compiler headers included. Default is "../include".
// C headers may reside in different directory, so separate macro is provided.
// Hint : never install STLport in the directory that ends with "include"
# define _STLP_NATIVE_INCLUDE_PATH ../include
# define _STLP_NATIVE_C_INCLUDE_PATH ../include
# define _STLP_NATIVE_CPP_C_INCLUDE_PATH ../include

// This macro constructs header path from directory and name.
// You may change it if your compiler does not understand "/".
#  define _STLP_MAKE_HEADER(path, header) <path/header>

// This macro constructs native include header path from include path and name.
// You may have do define it if experimenting problems with preprocessor
# define _STLP_NATIVE_HEADER(header) _STLP_MAKE_HEADER(_STLP_NATIVE_INCLUDE_PATH,header)

// Same for C headers
#define _STLP_NATIVE_C_HEADER(header) _STLP_MAKE_HEADER(_STLP_NATIVE_INCLUDE_PATH,header)

#define _STLP_EXPORT_DECLSPEC
#define _STLP_IMPORT_DECLSPEC

#define _STLP_CLASS_EXPORT_DECLSPEC
#define _STLP_CLASS_IMPORT_DECLSPEC

#define _STLP_CALL
//==========================================================
#endif

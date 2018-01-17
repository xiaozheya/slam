//
//	Copyright(c) 2017. zzzzRuby(zzzzRuby@hotmail.com)
//
//
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files(the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions :
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.

#pragma once

#include <stdint.h>
#include <stddef.h>

//	Platform macros

#if defined(_WIN32)
#	include <Windows.h>
#	if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#		define SKULD_PLATFORM_WIN32 1
#	else
#		define SKULD_PLATFORM_UWP 1
#	endif
#elif defined(__NX__)
#		define SKULD_PLATFORM_NINTENDO_SWITCH 1
#elif defined(__APPLE__) && defined(__MACH__)
#	include <TargetConditionals.h>
#	if TARGET_IPHONE_SIMULATOR == 1 || TARGET_OS_IPHONE == 1
#		define SKULD_PLATFORM_IOS 1
#	else
#		define SKULD_PLATFORM_MACOSX 1
#	endif
#elif defined(__unix__)
#	if defined(__linux__)
#		if defined(__ANDROID__)
#			define SKULD_PLATFORM_ANDROID 1
#		elif defined(__TIZEN__)
#			define SKULD_PLATFORM_TIZEN 1
#		else 
#			define SKULD_PLATFORM_LINUX 1
#		endif
#	endif
#endif

#if defined(SKULD_PLATFORM_WIN32) || defined(SKULD_PLATFORM_UWP)
#	define SKULD_PLATFORM_IS_WINDOWS 1
#endif

#if defined(SKULD_PLATFORM_IOS) || defined(SKULD_PLATFORM_NINTENDO_SWITCH)
#	define SKULD_PLATFORM_GAME_CONSOLE 1
#endif

#if !defined(SKULD_PLATFORM_ANDROID) && !defined(SKULD_PLATFORM_IOS) && \
	!defined(SKULD_PLATFORM_LINUX) && !defined(SKULD_PLATFORM_WIN32) && \
	!defined(SKULD_PLATFORM_UWP) && !defined(SKULD_PLATFORM_TIZEN) && !defined(SKULD_PLATFORM_MACOSX) && \
	!defined(SKULD_PLATFORM_NINTENDO_SWITCH)
#	error "Unknown platform"
#endif

//	Architecture macros

#if defined(_MSC_VER)
#	if defined(_M_AMD64)
#		define SKULD_ARCHITECTURE_X64 1
#	elif defined(_M_IX86)
#		define SKULD_ARCHITECTURE_X86 1
#	elif defined(_M_ARM64)
#		define SKULD_ARCHITECTURE_ARM64 1
#	elif defined(_M_ARM)
#		define SKULD_ARCHITECTURE_ARM 1
#	endif

#	if defined(SKULD_ARCHITECTURE_X86)
#		if defined(_M_IX86_FP) && _M_IX86_FP == 2
#			define SKULD_SIMD_SSE2 1
#		endif
#	elif defined(SKULD_ARCHITECTURE_X64)
#		define SKULD_SIMD_SSE2 1
#	elif defined(SKULD_ARCHITECTURE_ARM64) || defined(SKULD_ARCHITECTURE_ARM)
#		define SKULD_SIMD_NEON 1
#	endif
#else
#	if defined(__i386__)
#		define SKULD_ARCHITECTURE_X86 1
#	elif defined(__x86_64__)
#		define SKULD_ARCHITECTURE_X64 1
#	elif defined(__arm__)
#		define SKULD_ARCHITECTURE_ARM 1
#	elif defined(__aarch64__)
#		define SKULD_ARCHITECTURE_ARM64 1
#	endif

#	if defined(SKULD_ARCHITECTURE_X64) || defined(SKULD_ARCHITECTURE_X86)
#		if defined(__SSE2__)
#			define SKULD_SIMD_SSE2 1
#		endif
#	endif
#	if defined(SKULD_ARCHITECTURE_ARM) || defined(SKULD_ARCHITECTURE_ARM64)
#		if defined(__ARM_NEON) || defined(__ARM_NEON__)
#			define SKULD_SIMD_NEON 1
#		endif
#	endif
#endif

#if !defined(SKULD_SIMD_SSE2) && !defined(SKULD_SIMD_NEON)
#	define SKULD_SIMD_NONE 1
#endif

#if !defined(SKULD_ARCHITECTURE_ARM) && !defined(SKULD_ARCHITECTURE_ARM64) && \
	!defined(SKULD_ARCHITECTURE_X64) && !defined(SKULD_ARCHITECTURE_X86)
#	error "Unknown architecture"
#endif

//

#if defined(_MSC_VER)
#	define SKULD_CALL _cdecl
#else
#	define SKULD_CALL __attribute__((__cdecl__)) 
#endif

#if defined (_MSC_VER)
#	define SKULD_MATH_CALL __fastcall
#else
#	define SKULD_MATH_CALL __attribute__((fastcall))
#endif

#if defined(_MSC_VER)
#	define SKULD_ALIGN(n) __declspec(align(n))
#else
#	define SKULD_ALIGN(n) __attribute__((aligned(n)))
#endif

#if defined(_MSC_VER)
#define SKULD_FORCE_INLINE __forceinline
#else
#define SKULD_FORCE_INLINE __inline__ __attribute__((always_inline))
#endif
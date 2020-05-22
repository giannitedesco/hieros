#pragma once

#ifdef __i386__
#error "64bit types on 32bit compiler"
#endif

#ifndef __LP64__
#error "Not an LP64 platform"
#endif

#ifndef __ASM__
#define NULL ((void *)0)

typedef unsigned long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef long s64;
typedef int s32;
typedef short s16;
typedef char s8;

typedef unsigned long size_t;
typedef long ssize_t;
typedef long ptrdiff_t;

typedef __INTMAX_TYPE__ intmax_t;
typedef __UINTMAX_TYPE__ uintmax_t;
#ifdef __INTPTR_TYPE__
typedef __INTPTR_TYPE__ intptr_t;
#endif
#ifdef __UINTPTR_TYPE__
typedef __UINTPTR_TYPE__ uintptr_t;
#endif

#define bool	_Bool
#define true	1
#define false	0
#endif

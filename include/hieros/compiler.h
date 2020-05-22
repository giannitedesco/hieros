#pragma once

#if __GNUC__ >= 3 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 96 )
#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif
#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif
#endif

#if __STDC_VERSION__ > 201112L
#define _noreturn _Noreturn
#else
#define _noreturn __attribute__((noreturn))
#endif

#define _asmlinkage __attribute__((used, regparm(0)))

#define static_assert(expr, ...) __static_assert(expr, ##__VA_ARGS__, #expr)
#define __static_assert(expr, msg, ...) _Static_assert(expr, msg)

#define ARRAY_SIZE(_a) ({ \
	static_assert ( \
            ! __builtin_types_compatible_p(typeof(_a), typeof(&_a[0])), \
            # _a " is not an array" \
        ); \
	sizeof(_a) / sizeof((_a)[0]); \
})

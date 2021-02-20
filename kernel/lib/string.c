#include <hieros/types.h>
#include <hieros/string.h>

void *memcpy(void * restrict dst, const void * restrict src, size_t len)
{
	char * restrict d = dst;
	const char * restrict s = src;

	size_t i;

	for (i = 0; i < len; i++)
		d[i] = s[i];

	return dst;
}

void *memset(void *s, int c, size_t n)
{
	char *d = s;
	size_t i;

	for (i = 0; i < n; i++)
		d[i] = c;

	return s;
}

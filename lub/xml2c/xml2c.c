/*
 * xml2c.c
 */
#include "private.h"

#include <stdio.h>
#include <stdarg.h>

static int indent = 0;

/*--------------------------------------------------------- */
const char *xml2c_enum(int value, const char *array[])
{
	return array[value];
}

char *xml2c_esc(const char *str)
{
/*	char *tmp, *d, *s;
	if (!str)
		return NULL;
	tmp = malloc(strlen(str) * 2);
	s = str;
	d = tmp;
	while (*s) {
		switch(*s) {
		case '\"':
		case '\\':
			*d = '\\';
			d++;
			*d = *s;
			break;
		case '%':
			*d = '\\';
			d++;
			*d = *s;
			break;
		default:
			*d = *s;
			break;
		}
		d++;
	}
*/
	return lub_string_encode(str, "\\\"");
}

/*--------------------------------------------------------- */
/*int lub_dump_printf(const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	fprintf(stderr, "%*s", indent, "");
	len = vfprintf(stderr, fmt, args);
	va_end(args);

	return len;
}
*/

/*--------------------------------------------------------- */

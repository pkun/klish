/*
 * --------------------------------------
 * clish.c
 *
 * A console client for libclish
 * --------------------------------------
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#if WITH_INTERNAL_GETOPT
#include "libc/getopt.h"
#else
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif
#endif

#include "lub/list.h"
#include "clish/shell.h"

#define QUOTE(t) #t
#define version(v) printf("%s\n", v)

/*--------------------------------------------------------- */
/* Print help message */
static void help(int status, const char *argv0)
{
	const char *name = NULL;

	if (!argv0)
		return;

	/* Find the basename */
	name = strrchr(argv0, '/');
	if (name)
		name++;
	else
		name = argv0;

	if (status != 0) {
		fprintf(stderr, "Try `%s -h' for more information.\n",
			name);
	} else {
		printf("Usage: %s [options] [script_file] [script_file] ...\n", name);
		printf("CLI utility. Command line shell."
			"The part of the klish project.\n");
		printf("Options:\n");
		printf("\t-v, --version\tPrint version.\n");
		printf("\t-h, --help\tPrint this help.\n");
		printf("\t-x <path>, --xml-path=<path>\tPath to XML scheme files.\n");
	}
}

/*--------------------------------------------------------- */
int main(int argc, char **argv)
{
	int result = -1;
	clish_shell_t *shell = NULL;

	/* Command line options */
	const char *xml_path = getenv("CLISH_PATH");
	FILE *outfd = stdout;

	static const char *shortopts = "hvx:";
#ifdef HAVE_GETOPT_LONG
	static const struct option longopts[] = {
		{"help",	0, NULL, 'h'},
		{"version",	0, NULL, 'v'},
		{"xml-path",	1, NULL, 'x'},
		{NULL,		0, NULL, 0}
	};
#endif

	/* Parse command line options */
	while(1) {
		int opt;
#ifdef HAVE_GETOPT_LONG
		opt = getopt_long(argc, argv, shortopts, longopts, NULL);
#else
		opt = getopt(argc, argv, shortopts);
#endif
		if (-1 == opt)
			break;
		switch (opt) {
		case 'x':
			xml_path = optarg;
			break;
		case 'h':
			help(0, argv[0]);
			exit(0);
			break;
		case 'v':
			version(VERSION);
			exit(0);
			break;
		default:
			help(-1, argv[0]);
			goto end;
			break;
		}
	}

	/* Create shell instance */
	shell = clish_shell_new(NULL, outfd, BOOL_FALSE);
	if (!shell) {
		fprintf(stderr, "Error: Can't create shell instance.\n");
		goto end;
	}
	/* Load the XML files */
	if (clish_shell_load_scheme(shell, xml_path))
		goto end;

	clish_shell_xml2c(shell);

end:
	/* Cleanup */
	if (shell)
		clish_shell_delete(shell);

	return result;
}

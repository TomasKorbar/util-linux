/*
 * setsid.c -- execute a command in a new session
 * Rick Sladkey <jrs@world.std.com>
 * In the public domain.
 *
 * 1999-02-22 Arkadiusz Mi�kiewicz <misiek@pld.ORG.PL>
 * - added Native Language Support
 *
 * 2001-01-18 John Fremlin <vii@penguinpowered.com>
 * - fork in case we are process group leader
 *
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termio.h>

#include "c.h"
#include "nls.h"

static void __attribute__ ((__noreturn__)) usage(FILE * out)
{
	fprintf(out, USAGE_HEADER);
	fprintf(out, _(" %s [options] <program> [arguments ...]\n"),
		program_invocation_short_name);
	fprintf(out, USAGE_OPTIONS);
	fprintf(out, _(
		" -c, --ctty \tset the controlling terminal to the current one\n"
		));
	fprintf(out, USAGE_HELP);
	fprintf(out, USAGE_VERSION);
	fprintf(out, USAGE_MAN_TAIL("setsid(1)"));
	exit(out == stderr ? EXIT_FAILURE : EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
	int ch;
	int ctty = 0;

	static const struct option longopts[] = {
		{"ctty", no_argument, NULL, 'c'},
		{"version", no_argument, NULL, 'V'},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};

	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

	while ((ch = getopt_long(argc, argv, "+Vhc", longopts, NULL)) != -1)
		switch (ch) {
		case 'V':
			printf(UTIL_LINUX_VERSION);
			return EXIT_SUCCESS;
		case 'c':
			ctty=1;
			break;
		case 'h':
			usage(stdout);
		default:
			usage(stderr);
		}

	if (argc < 2)
		usage(stderr);

	if (getpgrp() == getpid()) {
		switch (fork()) {
		case -1:
			err(EXIT_FAILURE, _("fork"));
		case 0:
			/* child */
			break;
		default:
			/* parent */
			return 0;
		}
	}
	if (setsid() < 0)
		/* cannot happen */
		err(EXIT_FAILURE, _("setsid failed"));

	if (ctty) {
		if (ioctl(STDIN_FILENO, TIOCSCTTY, 1))
			warn(_("failed to set the controlling terminal"));
	}
	execvp(argv[optind], argv + optind + 1);
	err(EXIT_FAILURE, _("execvp failed"));
}

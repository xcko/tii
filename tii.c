/*
 * Tiny frontend for ii and ii-like programs.
 * Inspired by sic from suckless
 * http://tools.suckless.org/sic
 * And by lchat from younix
 * https://github.com/younix/lchat
 * Although tii contains code from neither.
 */

#include <sys/types.h>
#include <sys/event.h>	/* kqueue(2) */
#include <sys/time.h>

#include <err.h>
#include <fcntl.h>	/* open(2) */
#include <stdio.h>	/* printf(3) setvbuf(3) */
#include <stdlib.h>	/* getenv(3) */
#include <string.h>	/* strdup(3) */
#include <unistd.h>	/* getopt(3) write(2) read(2) */

static void
usage(void)
{
	fprintf(stderr, "tii [-h] [-i in] [-o out]\n");
	exit(1);
}

int
main(int argc, char *argv[])
{
	struct kevent chlist[2];	/* events to monitor */
	struct kevent evlist[2];	/* events that were triggered */
	int kq, nev;
	int i, ch, in_fd, out_fd;
	char *in_file = NULL, *out_file = NULL;
	char buf[4096];
	ssize_t bufl;

	while ((ch = getopt(argc, argv, "i:o:h")) != -1) {
		switch (ch) {
		case 'i':
			in_file = strdup(optarg);
			if (in_file == NULL)
				err(1, "strdup");
			break;
		case 'o':
			out_file = strdup(optarg);
			if (out_file == NULL)
				err(1, "strdup");
			break;
		case 'h':
		default:
			usage();
			/* NOTREACHED */
		}
	}
	if (argc - optind > 0)
		usage();

	if (in_file == NULL)
		if ((in_file = strdup("./in")) == NULL)
			err(1, "strdup");
	if ((in_fd = open(in_file, O_WRONLY|O_APPEND)) == -1)
		err(1, "open in_file");

	if (out_file == NULL)
		if ((out_file = strdup("./out")) == NULL)
			err(1, "strdup");
	if ((out_fd = open(out_file, O_RDONLY)) == -1)
		err(1, "open out_file");

	free(in_file);
	free(out_file);

	if ((setvbuf(stdin, NULL, _IONBF, 0)) != 0)
		err(1, "setvbuf stdin");
	if ((setvbuf(stdout, NULL, _IONBF, 0)) != 0)
		err(1, "setvbuf stdout");

	if ((kq = kqueue()) == -1)
		err(1, "kqueue()");

	EV_SET(&chlist[0], out_fd, EVFILT_READ, EV_ADD, 0, 0, 0);
	EV_SET(&chlist[1], STDIN_FILENO, EVFILT_READ, EV_ADD, 0, 0, 0);

	for (;;) {
		nev = kevent(kq, chlist, 2, evlist, 2, NULL);	/* block indefinitely */

		if (nev == -1)
			err(1, "kevent()");

		for (i = 0; i < nev; i++) {
			if (evlist[i].flags & EV_ERROR)
				err(1, "evlist EV_ERROR");
			if (evlist[i].data <= 0)
				break;	/* we only append */
			if ((bufl = read(evlist[i].ident, buf, sizeof buf)) == -1)
				err(1, "read");
			if (evlist[i].ident == out_fd) {
				if ((write(STDOUT_FILENO, buf, bufl)) == -1)
					err(1, "write");
			 } else if (evlist[i].ident == STDIN_FILENO) {
				if ((write(in_fd, buf, bufl)) == -1)
					err(1, "write");
			}
		}
	}
	return 0;
}

// SPDX-License-Identifier: GPL-3.0+
// Copyright (C) 2026 Mohamed Khalfella

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <libgen.h>

#define BUF_SIZE	4096
/*
#define ITERATIONS	255
*/
#define ITERATIONS	0

#define DELAY		3		/* second delay between iterations */
#define WRITE_COUNT	10

#define WRITE_OFFSET	0
#define READ_OFFSET	WRITE_OFFSET

int main(int argc, char **argv)
{
	int fd, i, w, off, ret;
	char *buff;

	/* Just return for now */
	fprintf(stdout, "starting %s test program\n", basename(argv[0]));

	if (argc < 2) {
		fprintf(stderr, "usage: %s /dev/nvmeXnY", argv[0]);
		return 1;
	}

	fd = open(argv[1], O_RDWR | O_DIRECT);
	if (fd < 0) {
		fprintf(stderr, "failed to open device, errno = %d\n", errno);
		return 1;
	}

	ret = posix_memalign((void **)&buff, BUF_SIZE, BUF_SIZE);
	if (ret) {
		fprintf(stderr, "failed to allocate buffer, ret = %d\n", ret);
		goto out;
	}

	for (i = 0; i < ITERATIONS; i++) {
		fprintf(stdout, "iteration number %d, writing data\n", i);

		for (w = 0; w < WRITE_COUNT; w++) {
			memset(buff, w, BUF_SIZE);
			ret = pwrite(fd, buff, BUF_SIZE, WRITE_OFFSET);
			if (ret != BUF_SIZE) {
				fprintf(stderr, "failed to write buff, "
						"ret = %d, errno = %d\n",
						ret, errno);
				goto out;
			}
		}

		sleep(5);
		fprintf(stdout, "validating written data\n");

		ret = pread(fd, buff, BUF_SIZE, READ_OFFSET);
		if (ret != BUF_SIZE) {
			fprintf(stderr, "failed to read buff, "
					"ret = %d, errno = %d\n",
					ret, errno);
			goto out;
		}

		for (off = 0; off < BUF_SIZE; off++) {
			if (buff[off] != WRITE_COUNT - 1) {
				fprintf(stderr, "validation failed\n");
				exit(1);
			}
		}

		fprintf(stdout, "successfully validated\n");
		sleep(DELAY);
	}

	fprintf(stdout, "finished %s test program\n", basename(argv[0]));
out:
	close(fd);
	return ret;
}

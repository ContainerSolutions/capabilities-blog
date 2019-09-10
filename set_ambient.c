/*
 * Simple program to start the given process with CAP_NET_BIND_SERVICE in the
 * ambient capabilities. 
 *
 * Based on test program by Christoph Lameter.
 *
 * (C) 2015 Christoph Lameter <cl@linux.com>
 * (C) 2019 Adrian Mouat <adrian.mouat@container-solutions.com>
 *
 * Released under: GPL v3 or later.
 *
 *
 * Compile using:
 *
 *      gcc ./set_ambient.c -o set_ambient -lcap-ng
 *
 * (requires cap-ng headers, which is in libcap-ng-dev in debian)
 *
 * This program must have the CAP_NET_BIND_SERVICE capability in the permitted 
 * set to run properly.
 *
 * This can be set on the file with:
 *
 *	sudo setcap cap_net_bind_service+p set_ambient
 *
 * To get a shell with CAP_NET_BIND_SERVICE that can be inherited by other 
 * processes:
 *
 *	./set_ambient /bin/bash
 *
 * Verifying that it works:
 *
 * From the bash spawed by set_ambient run
 *
 *	cat /proc/$$/status
 *
 * and have a look at the capabilities (use capsh --decode to interpret the
 * hex).
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <cap-ng.h>
#include <sys/prctl.h>
#include <linux/capability.h>

/*
 * Definitions from the kernel header files. These are going to be removed
 * when the /usr/include files have these defined.
 *
 * AM: This should be updated, I was just being lazy.
 */
#define PR_CAP_AMBIENT 47
#define PR_CAP_AMBIENT_IS_SET 1
#define PR_CAP_AMBIENT_RAISE 2
#define PR_CAP_AMBIENT_LOWER 3
#define PR_CAP_AMBIENT_CLEAR_ALL 4

static void set_ambient_cap(int cap)
{
	int rc;

	capng_get_caps_process();
	rc = capng_update(CAPNG_ADD, CAPNG_INHERITABLE, cap);
	if (rc) {
		printf("Cannot add inheritable cap\n");
		exit(2);
	}
	capng_apply(CAPNG_SELECT_CAPS);

	/* Note the two 0s at the end. Kernel checks for these */
	if (prctl(PR_CAP_AMBIENT, PR_CAP_AMBIENT_RAISE, cap, 0, 0)) {
		perror("Cannot set cap");
		exit(1);
	}
}

int main(int argc, char **argv)
{
	int rc;

	set_ambient_cap(CAP_NET_BIND_SERVICE);

	printf("Starting process with CAP_NET_BIND_SERVICE in ambient\n");
	if (execv(argv[1], argv + 1))
		perror("Cannot exec");

	return 0;
}

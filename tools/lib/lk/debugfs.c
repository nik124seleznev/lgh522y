#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/vfs.h>
#include <sys/mount.h>
#include <linux/kernel.h>

#include "debugfs.h"

char debugfs_mountpoint[PATH_MAX + 1] = "/sys/kernel/debug";

static const char * const debugfs_known_mountpoints[] = {
	"/sys/kernel/debug/",
	"/debug/",
	0,
};

static bool debugfs_found;

/*                                      */
const char *debugfs_find_mountpoint(void)
{
	const char * const *ptr;
	char type[100];
	FILE *fp;

	if (debugfs_found)
		return (const char *)debugfs_mountpoint;

	ptr = debugfs_known_mountpoints;
	while (*ptr) {
		if (debugfs_valid_mountpoint(*ptr) == 0) {
			debugfs_found = true;
			strcpy(debugfs_mountpoint, *ptr);
			return debugfs_mountpoint;
		}
		ptr++;
	}

	/*                                */
	fp = fopen("/proc/mounts", "r");
	if (fp == NULL)
		return NULL;

	while (fscanf(fp, "%*s %" STR(PATH_MAX) "s %99s %*s %*d %*d\n",
		      debugfs_mountpoint, type) == 2) {
		if (strcmp(type, "debugfs") == 0)
			break;
	}
	fclose(fp);

	if (strcmp(type, "debugfs") != 0)
		return NULL;

	debugfs_found = true;

	return debugfs_mountpoint;
}

/*                                                         */

int debugfs_valid_mountpoint(const char *debugfs)
{
	struct statfs st_fs;

	if (statfs(debugfs, &st_fs) < 0)
		return -ENOENT;
	else if (st_fs.f_type != (long) DEBUGFS_MAGIC)
		return -ENOENT;

	return 0;
}

/*                                                 */
char *debugfs_mount(const char *mountpoint)
{
	/*                             */
	if (debugfs_find_mountpoint())
		goto out;

	/*                                */
	if (mountpoint == NULL) {
		/*                                 */
		mountpoint = getenv(PERF_DEBUGFS_ENVIRONMENT);
		/*                                         */
		if (mountpoint == NULL)
			mountpoint = "/sys/kernel/debug";
	}

	if (mount(NULL, mountpoint, "debugfs", 0, NULL) < 0)
		return NULL;

	/*                     */
	debugfs_found = true;
	strncpy(debugfs_mountpoint, mountpoint, sizeof(debugfs_mountpoint));
out:
	return debugfs_mountpoint;
}

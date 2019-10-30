/*
 * Copyright 2019, Digi International Inc.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Description: CPU SYSFS API
 *
 */

#define LOG_TAG "sysfs_cpu"

#include <fcntl.h>
#include <stdio.h>
#include <sys/sysinfo.h>
#include <unistd.h>

#include <cutils/klog.h>

#define CPUS_SYSDIR "/sys/devices/system/cpu"

static int sysfs_cpu_set_value(int cpu, int value)
{
	int fd = -1, len;
	char buf[64];

	if (cpu < 0)
		return 0;

	snprintf(buf, sizeof(buf), CPUS_SYSDIR "/cpu%d/online", cpu);
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		KLOG_ERROR(LOG_TAG, "Could not open %s\n", buf);
		return -1;
	}
	len = write(fd, value ? "1" : "0", 1);
	close(fd);
	if (len < 1) {
		KLOG_ERROR(LOG_TAG, "Could not write %s\n", buf);
		return -1;
	}

	return 0;
}

int sysfs_disable_cpu(int cpu)
{
	return sysfs_cpu_set_value(cpu, 0);
}

/* Disable all CPUS but 'cpu0' */
int sysfs_disable_cpu_all(void)
{
	int i, ncores;
	int ret = 0;

	ncores = sysconf(_SC_NPROCESSORS_CONF);
	for (i = 1; i < ncores; i++)
		ret |= sysfs_disable_cpu(i);

	return ret;
}

int sysfs_enable_cpu(int cpu)
{
	return sysfs_cpu_set_value(cpu, 1);
}

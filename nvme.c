#include <dirent.h>
#include <endian.h>
#include <errno.h>
#include <getopt.h>
#include <fcntl.h>
#include <inttypes.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/fs.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "program.h"
#include "os/linux-nvme.h"
#include "os/nvme-ioctl.h"

#include "nvme-print.h"

static int nvme_get_dev(int argc, char **argv)
{
	return open("/dev/nvme0", O_RDONLY);
}

#define CREATE_CMD
#include "nvme-cli.h"

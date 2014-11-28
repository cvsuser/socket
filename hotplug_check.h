#ifndef __MOUNT_USB_H__
#define __MOUNT_USB_H__

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <limits.h>
#include <linux/input.h>
#include <linux/types.h>
#include <mntent.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/mount.h>
//#include <sys/msg.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <linux/netlink.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <libgen.h>

#include <ctype.h>
#include <sys/klog.h>
#include <syslog.h>
#include <pthread.h>
//#include "libzebra.h"

#define MAX_PART 16
#define PID_FILE                "./usbmounter.pid"
#define PRINT_ON 1
#ifdef PRINT_ON
#define PRINTF(x...) printf(x)
#else
#define PRINTF(x...)
#endif

#endif

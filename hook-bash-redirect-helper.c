//
// Created by dingjing on 2025/8/20.
//
#include <dlfcn.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG
#include <syslog.h>
#define DLOG(...) syslog(LOG_ERR, __VA_ARGS__)
#else
#define DLOG(...)
#endif

#ifndef RTLD_NEXT
#define RTLD_NEXT ((void*) -1l)
#endif
#include "hook-common/hook-common.h"

static int gsNewFd = -1;

typedef int (*HookDup2)(int oldFd, int newFd);
int hook_dup2(int oldFd, int newFd)
{
    static HookDup2 hookDup2 = NULL;
    if (!hookDup2) {
        hookDup2 = dlsym(RTLD_NEXT, "dup2");
    }

    char oldPath[PATH_MAX] = {0};
    char newPath[PATH_MAX] = {0};

    hook_common_read_fd_path(oldFd, oldPath, sizeof(oldPath));
    hook_common_read_fd_path(newFd, newPath, sizeof(newPath));

    int myFd = hookDup2(oldFd, newFd);

    char procPath[PATH_MAX] = {0};
    hook_common_read_proc_name(procPath, sizeof(procPath));
    if ((strstr(procPath, "/bash")
        || strstr(procPath, "/sh")
        || strstr(procPath, "/zsh"))
        && (strstr(newPath, "/dev/pts/"))
        && !(strstr(oldPath, "pipe:["))
        && !(strstr(oldPath, "/dev") && strstr(newPath, "/dev"))) {
        gsNewFd = oldFd;
        DLOG("%s, %s --> %s, old:%d new: %d ret: %d", procPath, oldPath, newPath, oldFd, newFd, gsNewFd);
    }

    return myFd;
}

typedef int (*HookClose) (int fd);
int hook_close (int fd)
{
    static HookClose hookClose = NULL;
    if (!hookClose) {
        hookClose = dlsym(RTLD_NEXT, "close");
    }

    const int ret = hookClose(fd);

    if (fd == gsNewFd) {
        char fdPath[PATH_MAX] = {0};
        hook_common_read_fd_path(fd, fdPath, sizeof(fdPath));
        DLOG("fd: %d --> close: %s", gsNewFd, fdPath);
    }

    return ret;
}

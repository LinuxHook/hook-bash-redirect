//
// Created by dingjing on 2025/8/20.
//


extern int hook_dup2(int oldFd, int newFd);
int dup2 (int oldFd, int newFd)
{
    return hook_dup2(oldFd, newFd);
}

extern int hook_close (int fd);
int close (int fd)
{
    return hook_close (fd);
}
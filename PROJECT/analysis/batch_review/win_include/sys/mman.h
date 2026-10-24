/* Windows-only adapter for the existing Linux host_blit_test.c.
 * Anonymous, fixed-address allocation only; no production source changes.
 */
#ifndef BATCH_REVIEW_MMAN_H
#define BATCH_REVIEW_MMAN_H
#include <windows.h>
#include <errno.h>
#include <stddef.h>
#define PROT_READ 1
#define PROT_WRITE 2
#define MAP_PRIVATE 2
#define MAP_ANONYMOUS 0x20
#define MAP_FIXED_NOREPLACE 0x100000
#define MAP_FAILED ((void *)-1)
static inline void *mmap(void *addr, size_t size, int prot, int flags,
                         int fd, long long offset)
{
    void *p;
    if (prot != (PROT_READ | PROT_WRITE) ||
        flags != (MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE) ||
        fd != -1 || offset != 0) {
        errno = EINVAL;
        return MAP_FAILED;
    }
    p = VirtualAlloc(addr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (p != addr) {
        if (p) VirtualFree(p, 0, MEM_RELEASE);
        errno = ENOMEM;
        return MAP_FAILED;
    }
    return p;
}
#endif

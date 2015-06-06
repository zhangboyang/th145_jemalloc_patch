/* robust jemalloc wrapper for th145patch
 * note:
 *    jemalloc must compiled with --disable-munmap
 */

#include <windows.h>
#include <stddef.h>
#include <time.h>

typedef void *(*malloc_t)(size_t);
typedef void *(*calloc_t)(size_t, size_t);
typedef void *(*realloc_t)(void *, size_t);
typedef void (*free_t)(void *);

malloc_t my_malloc;
calloc_t my_calloc;
realloc_t my_realloc, old_realloc;
free_t my_free;

volatile char page_set[512 * 1024];
/* page size is 4K
 * there are at most 2G / 4K = 512K pages
 * no need to lock */

__declspec(dllexport)
void init(void *old_realloc_funcaddr)
{
    HMODULE hLib = LoadLibraryA("jemalloc.dll");
    
    my_malloc = (malloc_t) GetProcAddress(hLib, "je_malloc");
    my_calloc = (calloc_t) GetProcAddress(hLib, "je_calloc");
    my_realloc = (realloc_t) GetProcAddress(hLib, "je_realloc");
    my_free = (free_t) GetProcAddress(hLib, "je_free");
    
    old_realloc = old_realloc_funcaddr;
}

#define to_page(p) (((int) p) >> 12)

/* jemalloc must compiled with --disable-munmap
 * so jemalloc don't return memory to system
 * there is no pageset_remove() */
void pageset_insert(void *p)
{
	page_set[to_page(p)] = 1;
}

int pageset_exists(void *p)
{
	return page_set[to_page(p)];
}

__declspec(dllexport)
void *rmalloc(size_t n)
{
    void *p = my_malloc(n);
    pageset_insert(p);
    return p;
}

__declspec(dllexport)
void *rcalloc(size_t n, size_t m)
{
    void *p = my_calloc(n, m);
    pageset_insert(p);
    return p;
}

__declspec(dllexport)
void *rrealloc(void *p, size_t n)
{
    void *q;
    if (pageset_exists(p)) {
        q = my_realloc(p, n);
        pageset_insert(q);
    } else {
		/* we must call native realloc */
        q = old_realloc(p, n);
    }
    return q;
}

__declspec(dllexport)
void rfree(void *p)
{
	/* if the pointer is not managed by us, let it leak */
    if (pageset_exists(p))
        my_free(p);
}

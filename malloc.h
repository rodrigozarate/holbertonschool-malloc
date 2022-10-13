#ifndef HOLBI_MALLOC
#define HOLBI_MALLOC

#include <stddef.h>

#define ALIGN (2 * sizeof(void *))

void *putPageH(void);
int findFblock(void *addr0, size_t *size, size_t usedBcount,
		    void **freeBaddr, size_t *freeBsize);
void *naive_malloc(size_t size);

#endif /* HOLBI_MALLOC */

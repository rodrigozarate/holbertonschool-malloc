#include "malloc.h"

/**
 * _free - own free
 * @ptr: pointer
 */
void _free(void *ptr)
{
	if (!ptr)
		return;

	freeListAdd(BLK_HEADER(ptr));
	joinFreeBlocks();
}

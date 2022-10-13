#include "malloc.h"
#include <stdio.h>
#include <unistd.h>


static block_t *first_free_blk;

/**
 * freeListAdd - adds a block to the free list
 * @blk: block
 */
void freeListAdd(block_t *blk)
{
	if (!blk)
	{
		fprintf(stderr, "freeListAdd: blk is NULL\n");
		return;
	}

	if (first_free_blk != NULL)
		first_free_blk->prev = blk;
	blk->next = first_free_blk;
	blk->prev = NULL;
	first_free_blk = blk;
}

/**
 * freeListRemove - removes a block from the free list
 * @blk: block to remove
 */
void freeListRemove(block_t *blk)
{
	if (blk == NULL)
	{
		fprintf(stderr, "freeListRemove: blk is NULL\n");
		return;
	}

	if (blk == first_free_blk)
		first_free_blk = blk->next;

	if (blk->prev)
		blk->prev->next = blk->next;

	if (blk->next)
		blk->next->prev = blk->prev;
}

/**
 * splitFreeBlock - splits free block
 * @free_blk: unused blocks
 * @size: block size in bytes
 * Return: pointer
 */
block_t *splitFreeBlock(block_t *free_blk, size_t size)
{
	block_t *new_free_blk;

	if (!free_blk)
	{
		fprintf(stderr, "splitFreeBlock: free_blk is NULL\n");
		return (NULL);
	}

	if (size < sizeof(block_t) || size % ALIGN)
	{
		fprintf(stderr, "splitFreeBlock: size not aligned\n");
		return (NULL);
	}

	new_free_blk = (block_t *)((uint8_t *)free_blk + size);

	new_free_blk->next = free_blk->next;
	new_free_blk->prev = free_blk->prev;
	new_free_blk->size = free_blk->size - size;
	free_blk->size = size;

	return (new_free_blk);
}

/**
 * joinFreeBlocks - join free blocks
 */
void joinFreeBlocks(void)
{
	block_t *curr;
	void *pBrk;
	long pgSize;

	pBrk = sbrk(0);
	if (pBrk == (void *)-1)
	{
		perror("joinFreeBlocks: sbrk");
		return;
	}
	pgSize = sysconf(_SC_PAGESIZE);
	if (pgSize == -1)
	{
		perror("joinFreeBlocks: sysconf");
		return;
	}

	for (curr = first_free_blk; curr; curr = curr->next)
	{
		if ((uint8_t *)curr + curr->size == (uint8_t *)curr->next)
		{
			curr->size += curr->next->size;
			curr->next = curr->next->next;
			if (curr->next)
				curr->next->prev = curr;
		}
	}

	for (curr = first_free_blk; curr; curr = curr->next)
	{
		if ((uint8_t *)curr + curr->size == pBrk &&
		    curr->size >= (size_t)pgSize)
		{
			freeListRemove(curr);
			if (brk(curr) != 0)
				perror("joinFreeBlocks: brk");
			break;
		}
	}
}

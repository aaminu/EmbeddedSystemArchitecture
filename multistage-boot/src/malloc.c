#include <stdint.h>

extern uint32_t _START_HEAP;
extern uint32_t _END_HEAP;

#define NULL (((void *)0))
#define GUARD_SIZE (0x400)
#define HEAP_GUARD ((uint32_t)((char *)(&_END_HEAP) - GUARD_SIZE))

/*Add ifdef flag if wanted optional*/
/**Add endheap limit check*/
#ifdef OWN_MALLOC
#define SIGNATURE_ASSIGNED (0xBBC0FFEE)
#define SIGNATURE_FREED (0xFEEDFACE)

struct malloc_block
{
    uint32_t signature;
    uint32_t size;
};

void *malloc(uint32_t size)
{
    static uint32_t *end_heap = 0;
    struct malloc_block *blk;
    char *ret = NULL;

    /*set heap-end to start for the first heap allocation*/
    if (!end_heap)
    {
        end_heap = &_START_HEAP;
    }

    /*Make the block requested 32-bit Aligned*/
    if (((size >> 2) << 2) != size)
    {
        size = ((size >> 2) + 1) << 2;
    }

    /*use already freed heap and reassign*/
    blk = (struct malloc_block *)&_START_HEAP;
    while ((uint32_t *)blk < end_heap)
    {
        if ((blk->signature == SIGNATURE_FREED) && (blk->size >= size))
        {
            blk->signature = SIGNATURE_ASSIGNED;
            blk->size = size;
            ret = ((char *)blk) + sizeof(struct malloc_block);
            return (void *)ret;
        }
        blk = (struct malloc_block *)(((char *)blk) + sizeof(struct malloc_block) + blk->size);
    }

    /*Use endheap if no freed position available */
    blk = (struct malloc_block *)end_heap;
    blk->signature = SIGNATURE_ASSIGNED;
    blk->size = size;
    ret = ((char *)end_heap) + sizeof(struct malloc_block);
    end_heap = (uint32_t *)(ret + size);

    if ((uint32_t)end_heap >= HEAP_GUARD)
        return NULL;

    return (void *)ret;
}

void free(void *ptr)
{
    if (!ptr)
        return;

    struct malloc_block *blk = (struct malloc_block *)((char *)(ptr) - sizeof(struct malloc_block));

    if (blk->signature != SIGNATURE_ASSIGNED)
        return;

    blk->signature = SIGNATURE_FREED;
}
#else

void *_sbrk(uint32_t size)
{
    static uint8_t *heap = 0;
    void *previos_heap_pos = heap;

    /*Make the block requested 32-bit Aligned*/
    if (((size >> 2) << 2) != size)
    {
        size = ((size >> 2) + 1) << 2;
    }

    if (!heap)
    {
        heap = previos_heap_pos = (uint8_t *)&_START_HEAP;
    }

    if ((heap + size) >= HEAP_GUARD)
        return (void *)(-1);
    else
        heap += size;

    return previos_heap_pos;
}

#endif

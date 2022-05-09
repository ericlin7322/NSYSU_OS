#include "mm.h"

typedef struct block_meta {
    size_t size;
    struct block_meta *next;
    int free;
} Block;

#define META_SIZE sizeof(Block)

// linked list init
void *global_base = NULL;

Block *find_free_block(Block **last, size_t size) {
    Block *current = global_base;
    while (current && !(current->free && current->size >= size)) {
        *last = current;
        current = current->next;
    }
    return current;
}

Block *request_space(Block* last, size_t size) {
    Block *block;
    block = sbrk(0);
    void *request = sbrk(size + META_SIZE);
    if (request == (void*) -1) {
        return NULL; // sbrk failed.
    }

    if (last) { // NULL on first request.
        last->next = block;
    }
    block->size = size;
    block->next = NULL;
    block->free = 0;
    return block;
}

void *mymalloc(size_t size) {
    Block *block;

    if (size <= 0) {
        return NULL;
    }

    if (!global_base) { // First call.
        block = request_space(NULL, size);
        if (!block) {
            return NULL;
        }
        global_base = block;
    } else {
      Block *last = global_base;
      block = find_free_block(&last, size);
      if (!block) { // Failed to find free block.
          block = request_space(last, size);
          if (!block) {
              return NULL;
          }
      } else {      // Found free block
          block->free = 0;
      }
    }

    return(block+1);
}

Block *get_block_ptr(void *ptr) {
    return (Block*)ptr - 1;
}

void myfree(void *ptr) {
    if (!ptr) {
        return;
    }

    Block* block_ptr = get_block_ptr(ptr);
    block_ptr->free = 1;
}

void *myrealloc(void *ptr, size_t size) {
    if (!ptr) {
        // NULL ptr. realloc should act like malloc.
        return mymalloc(size);
    }

    Block* block_ptr = get_block_ptr(ptr);
    if (block_ptr->size >= size) {
        return ptr;
    }

    // Need to really realloc. Malloc new space and free old space.
    // Then copy old data to new space.
    void *new_ptr;
    new_ptr = mymalloc(size);
    if (!new_ptr) {
        return NULL;
    }
    memcpy(new_ptr, ptr, block_ptr->size);
    myfree(ptr);
    return new_ptr;
}

void *mycalloc(size_t nmemb, size_t size) {
    size_t size_total = nmemb * size;
    void *ptr = mymalloc(size_total);
    memset(ptr, 0, size_total);
    return ptr;
}

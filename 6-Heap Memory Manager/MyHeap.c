/*============================================================================
 * @file name      : HeapUtils.c
 * @Author         : Shehab Aldeen Mohammed
 * @Github         : https://github.com/ShehabAldeenMo
 * @LinkedIn       : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Description:
 * This file contains the implementation of utility functions for managing memory 
 * within the simulated heap environment. These functions support various operations 
 * such as memory allocation, resizing, splitting free blocks, removing free blocks, 
 * and updating metadata in the heap. They work together with the heap manager to 
 * provide dynamic memory management using a statically allocated array to represent 
 * the heap.
 *
 =============================================================================
 * @Notes:
 *
 ******************************************************************************
 ==============================================================================
*/

/*===============================  Includes ==============================*/
#include "MyHeap.h"

void* malloc(size_t size) {
    return HeapManager_Malloc(size); // Call the original malloc
}

void free(void* ptr) {
    HeapManager_Free(ptr); // Call the original free
}

void* realloc(void* ptr, size_t new_size) {
    void* new_ptr ;

    // Determine the size of the old memory block
    // This example assumes you have a way to get the old size; in practice, this may be handled differently.
    size_t old_size = HeapManager_GetSize(ptr);

    if (ptr == NULL) {
        return malloc(new_size);
    }
    else if (new_size == 0){
        free(ptr);
        return NULL;
    }
    else {
        // Allocate new memory
        new_ptr = malloc(new_size);
        if (new_ptr == NULL) {
            return NULL;  // Allocation failed
        }

        new_size = HeapManager_GetSize(new_ptr);

        // Copy data from old memory block to new memory block
        memcpy(new_ptr, ptr, (old_size < new_size) ? old_size : new_size);

        // Free old memory block
        free(ptr);
    }

    return new_ptr;
}

void* calloc(size_t num, size_t size) {
    // Calculate the total size required
    size_t total_size = num * size;

    // Allocate memory
    void* ptr = malloc(total_size);
    if (ptr == NULL) {
        return NULL;  // Allocation failed
    }
    
    total_size = HeapManager_GetSize(ptr);

    // Initialize allocated memory to zero
    memset(ptr, 0, total_size);

    return ptr;
}

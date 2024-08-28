/*============================================================================
 * @file name      : HeapUtils.h
 * @Author         : Shehab Aldeen Mohammed
 * @Github         : https://github.com/ShehabAldeenMo
 * @LinkedIn       : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Description:
 * This header file declares utility functions used by the heap manager for 
 * managing memory within the simulated heap. Functions include operations 
 * for allocation, resizing, splitting, and removing free blocks, as well as 
 * updating metadata and pointers in the heap.
 *
 =============================================================================
 * @Notes:
 *
 ******************************************************************************
 ==============================================================================
*/
#ifndef MYHEAP_H_
#define MYHEAP_H_

#include "HeapManager.h"


void* malloc(size_t size);
void free(void* ptr) ;
void* realloc(void* ptr, size_t new_size);
void* calloc(size_t num, size_t size) ;

#endif 

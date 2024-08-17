# Heap Memory Manager

## Table of Contents

- [Description](#description)
- [Features](#features)
- [Function Descriptions](#function-descriptions)
- [Usage](#usage)
- [Flow Chart](#flow-chart)
- [Build Instruction](#build-instruction)
- [Notes](#notes)

## Description

The Heap Memory Manager (HMM) provides dynamic memory allocation services to user-space programs by simulating a heap using a large statically allocated array and a variable representing the program break. This implementation does not use kernel-level memory management but instead operates entirely in user space for simplicity and ease of debugging.

## Features

- **Dynamic Memory Allocation**: Allocate memory blocks of a specified size using `HmmAlloc()`.
- **Memory Deallocation**: Free previously allocated memory blocks with `HmmFree()`.
- **Best Fit Allocation**: Efficiently find the smallest suitable block for a given size.
- **First Fit Allocation**: Quickly find the first block that fits the requested size.
- **Free Space Management**: Handle merging and splitting of free blocks in the heap.
- **Heap Expansion and Shrinking**: Simulate heap size adjustments by modifying the program break.

## Function Descriptions

### `void HMM_Init()`

Initializes the heap memory manager.

### `void *HmmAlloc(size_t size)`

Allocates a block of memory of the specified size.

**Parameters:**
- `size`: The size of the memory block to allocate, in bytes.

**Returns:**
- A pointer to the allocated memory block, or `NULL` if the allocation fails.

### `void HmmFree(void *ptr)`

Frees a previously allocated memory block.

**Parameters:**
- `ptr`: A pointer to the memory block to be freed.

**Returns:**
- None.

### `sint32 Helper_BestFit(uint32 size)`

Finds the best-fit block of memory for the specified size.

**Parameters:**
- `size`: The size of the memory block to find.

**Returns:**
- The index of the best-fit block, or `INVALID` if no suitable block is found.

### `sint32 Helper_FirstFit(uint32 size)`

Finds the first-fit block of memory for the specified size.

**Parameters:**
- `size`: The size of the memory block to find.

**Returns:**
- The index of the first-fit block, or `INVALID` if no suitable block is found.

### `void Helper_FreeOperationAfterTail(uint32 index, uint32 size)`

Manages free space when a new block is freed after the tail of the heap.

**Parameters:**
- `index`: The index of the newly freed block.
- `size`: The size of the newly freed block.

### `void Helper_FreeOperationBeforeHead(uint32 index, uint32 size)`

Manages free space when a new block is freed before the head of the heap.

**Parameters:**
- `index`: The index of the newly freed block.
- `size`: The size of the newly freed block.

### `void Helper_FreeOperationMiddleNode(uint32 index, uint32 size)`

Manages free space when a new block is freed in the middle of the heap.

**Parameters:**
- `index`: The index of the newly freed block.
- `size`: The size of the newly freed block.

### `void Helper_SetFreeSpaceNode(uint32 index, uint32 metadata, uint32 previous_content, uint32 next_content)`

Sets up a new free space node in the heap.

**Parameters:**
- `index`: The index of the new free space node.
- `metadata`: Metadata for the new free space node.
- `previous_content`: The previous free block index.
- `next_content`: The next free block index.

### `sint32 Helper_sbrk(sint32 size)`

Simulates the `sbrk()` system call to increase or decrease the size of the heap.

**Parameters:**
- `size`: The amount to increase or decrease the heap size.

**Returns:**
- The new program break index, or `INVALID` if the operation fails.

## Usage

### Initialization

Initialize the memory manager with `HMM_Init()` before performing any allocation or deallocation operations.

### Allocation and Deallocation

To allocate and free memory:

```c
#include "heap_manager.h"

int main() {
    // Initialize the heap memory manager
    HMM_Init();

    // Allocate memory
    void *ptr1 = HmmAlloc(8);
    void *ptr2 = HmmAlloc(8);
    void *ptr3 = HmmAlloc(8);

    // Use the allocated memory...

    // Free the memory
    HmmFree(ptr1);
    HmmFree(ptr2);
    HmmFree(ptr3);

    return 0;
}
```

## Flow Chart
### First Fit 
![Flow Diagram](https://github.com/user-attachments/assets/d5571c34-8855-4a03-903f-b85793bc1fee)

#### case 1 
![case 1](https://github.com/user-attachments/assets/ee64f89d-fc96-4b2d-a87c-8fe1bbe37137)

#### case 2
![case 2](https://github.com/user-attachments/assets/f0823bbe-2e91-46bf-a7f2-80e3146d34af)

#### case 3
![case 3](https://github.com/user-attachments/assets/b7dd8ea7-057b-4edf-a80a-5723321635e1)

#### case1`
![case 1`](https://github.com/user-attachments/assets/808a4926-9426-433b-b835-3c5d82897a1c)

#### case2`
![case 2`](https://github.com/user-attachments/assets/2456e1f1-95b3-4348-a5c5-18be4fbb9b9f)

#### case3`
![case 3`](https://github.com/user-attachments/assets/4c5c7489-279d-4cc4-8f07-7ed532f3ae00)

#### Extended Break pointer
![Extend Break](https://github.com/user-attachments/assets/31a43104-f978-4ee5-af31-73ba118e8121)


-------------------------------------------------------------------------------------------------------------------

### Best Fit 
![BestFit Flaw Chart 1](https://github.com/user-attachments/assets/d4df84ce-f4ab-493f-b405-b4ac42077962)
![BestFit Flaw Chart 2](https://github.com/user-attachments/assets/ba050377-3f8a-4127-bf2b-391a56acc7c9)

#### case 1 
![case 1](https://github.com/user-attachments/assets/36a9eb0c-45dd-4579-86af-1038fa8e69be)

#### case 2
![case 2](https://github.com/user-attachments/assets/2738d0b5-d037-4379-8050-59f481c60ecc)

#### case 3
![case 3](https://github.com/user-attachments/assets/0f6e81e7-6102-40cc-a055-14dbd6304470)

#### case1`
![case 1`](https://github.com/user-attachments/assets/ca30475b-ab61-407f-aa74-3615b8f2340f)

#### case2`
![case 2`](https://github.com/user-attachments/assets/78fb1c49-dde6-4092-8e41-6153734087e7)

#### case3`
![case 3`](https://github.com/user-attachments/assets/49781a7e-21b9-4df6-aece-ffd6668c30c7)

#### Extended Break pointer
![Extend Break](https://github.com/user-attachments/assets/774aefcb-1bb8-40c7-b014-9757ee76a74c)

-------------------------------------------------------------------------------------------------------------------

### Free Block 
![Free FlowChart](https://github.com/user-attachments/assets/1fe684cc-8207-4e69-93a8-d85b9f10917e)

#### General Cases 
![cases](https://github.com/user-attachments/assets/82beee69-6c88-4df7-8b61-6af25fdadc42)

#### case 1 
![case1 ](https://github.com/user-attachments/assets/b09e81f6-1c42-4869-8c8f-682d6083806d)

#### case 2
![case2](https://github.com/user-attachments/assets/28e0d67e-72bc-4e4f-9e18-5530bea2cc2a)

#### case 3
![case3](https://github.com/user-attachments/assets/2e188503-3247-4128-8e83-1e04db2c01dd)

-------------------------------------------------------------------------------------------------------------------

## Build Instruction
To build the Heap Memory Manager project, follow these steps:

1. Clone the Repository:
  Clone the repository to your local machine using Git:
```
git clone https://github.com/YourUsername/heap-memory-manager.git
cd heap-memory-manager
```

2. Compilation:
  Compile the project using GCC. Navigate to the project directory and use the following command:
```
gcc -o heap_manager HMM.c Helper.c
```

3. Running the Program:
  After successful compilation, you can run the heap manager by executing:
```
./heap_manager
```


## Notes
+ The heap size is simulated using a statically allocated array and a variable representing the program break.
+ This implementation is designed for user-space testing and debugging, with no kernel-level interaction.
+ Ensure that you have the necessary permissions to execute and access the required files.

## Illustrate Videos
For more information, refer to the [BestFit](https://drive.google.com/file/d/1ouaNFC1mB3zyFYNj4ZnmFE_nSQve8DMU/view?usp=drive_link) video. [FirstFit](https://drive.google.com/file/d/1hXbb8YoI0W-jOS7o307Qu74nbWCHRHZM/view?usp=drive_link).
refer to [Free](https://drive.google.com/file/d/1rSVcubXRlauPS18s_mWOQZF-WEHjgn69/view?usp=drive_link).





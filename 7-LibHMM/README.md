# Heap Memory Manager

## Table of Contents
- [Description](#description)
- [Features](#features)
- [Components](#components)
- [Build Instruction](#build-instruction)
- [Notes](#notes)
- [Illustrate Videos](#illustrate-videos)

## Description
MyHeap is a robust and efficient dynamic memory allocation library designed to manage heap memory with custom allocation strategies. It offers a suite of functionalities for memory management, including allocation, deallocation, and heap resizing, tailored for scenarios where fine-grained control over memory is required.

```
7-LibHMM/
├── HeapExtras.h
├── HeapExtras.c
├── HeapManager.h
├── HeapManager.c
├── HeapUtils.c
├── HeapUtils.h
├── MyHeap.h
├── MyHeap.c
├── track_freelist.gdb
```

## Features
- **Custom Memory Management**: <br />
  Provides functions for memory allocation (malloc), deallocation (free), and resizing (realloc), designed to work seamlessly with a custom simulated heap or system heap.
- **Best Fit Allocation**: <br />
  Efficiently find the smallest suitable block for a given size.
- **First Fit Allocation**: <br />
  Quickly find the first block that fits the requested size.
- **Heap Expansion and Shrinking**: <br />
  Simulate heap size adjustments by modifying the program break.
- **Dynamic Heap Management**: <br />
  Utilizes system calls such as sbrk to dynamically adjust the heap size, allowing for flexible memory management tailored to application needs.
- **Memory Initialization**: <br />
  Initializes and manages the heap using a combination of internal data structures and system calls, ensuring efficient memory usage and minimal fragmentation.
- **Memory Verification**: <br />
  Includes functionality for verifying allocated memory blocks to ensure data integrity and consistency throughout the allocation lifecycle.
- **Debugging Support**: <br />
  Features built-in debugging capabilities to track and diagnose memory allocation issues, including detailed error messages and heap state reporting.

## Components
+ **HeapManager**: <br />
  The core module responsible for handling memory allocation and deallocation. Interfaces with HeapUtils and HeapExtras for extended functionality.
+ **HeapUtils**: <br />
  Provides utility functions for managing heap blocks and tracking free space. Includes functionalities for block management and free list operations.
+ **HeapExtras**: <br />
  Contains additional support functions for initializing and managing the heap, including resizing operations and metadata management.
+ **MyHeap**: <br />
  The main interface for custom heap operations, leveraging underlying modules to provide a unified API for memory management.

## Build Instruction
To build the Heap Memory Manager project, follow these steps:

1. Clone the Repository:
  Clone the repository to your local machine using Git:
```
git clone https://github.com/ShehabAldeenMo/STMicroelectronics-Tasks.git
cd 7-LibHMM
```

2. Compilation:
```
gcc -g -c -fPIC *.c
gcc -g -shared -o libmyheap.so *.o
export LD_PRELOAD="/home/shehabaldeen/Desktop/Linux/STMicroelectronics/STMicroelectronics-Tasks/7-LibHMM/libmyheap.so"
```

3. Running the Program:
  After successful compilation, you can enter any command ex: `ls` , `vim` , `bash` , `grep` , `cat` , `clear` , `exit`.
```
vim
```

## Notes
+ The heap size is simulated using a statically allocated array and a variable representing the program break.
+ This implementation is designed for user-space testing and debugging, with no kernel-level interaction.
+ Ensure that you have the necessary permissions to execute and access the required files.

## Illustrate Videos
For more information, refer to the 
+ [BestFit](https://drive.google.com/file/d/1ouaNFC1mB3zyFYNj4ZnmFE_nSQve8DMU/view?usp=drive_link).
+ [FirstFit](https://drive.google.com/file/d/1hXbb8YoI0W-jOS7o307Qu74nbWCHRHZM/view?usp=drive_link).
+ [Free](https://drive.google.com/file/d/1rSVcubXRlauPS18s_mWOQZF-WEHjgn69/view?usp=drive_link).
+ [Simulated Heap Memory Manager Test]()
+ [Real Heap Memory Manager Test]()

# Heap Memory Manager

## Table of Contents
- [Description](#description)
- [Abstration](#abstraction)
- [Features](#features)
- [Flow Chart](#flow-chart)
- [Build Instruction](#build-instruction)
- [Notes](#notes)

## Description

The Heap Memory Manager (HMM) provides dynamic memory allocation services to user-space programs by simulating a heap using a large statically allocated array and a variable representing the program break. This implementation does not use kernel-level memory management but instead operates entirely in user space for simplicity and ease of debugging.

## Abstraction
![Screenshot from 2024-08-21 20-10-59](https://github.com/user-attachments/assets/c1c9d5db-843e-422a-abe9-0b4fbff9a856)

```
6-Heap Memory Manager/
|
├── Level_1/
│   ├── HeapTest.h
│   ├── HeapTest.c
|
├── Level_2/
│   ├── HeapExtras.h
│   ├── HeapExtras.c
│   ├── HeapManager.h
│   ├── HeapManager.c
|
├── Level_3/
│   ├── HeapUtils.h
│   ├── HeapUtils.c
|
└── main.c
└── track_freelist.gdb
└── README.md
└── MakeFile
└── .gitignore
```

## Features

- **Dynamic Memory Allocation**: Allocate memory blocks of a specified size using `HmmAlloc()`.
- **Memory Deallocation**: Free previously allocated memory blocks with `HmmFree()`.
- **Best Fit Allocation**: Efficiently find the smallest suitable block for a given size.
- **First Fit Allocation**: Quickly find the first block that fits the requested size.
- **Free Space Management**: Handle merging and splitting of free blocks in the heap.
- **Heap Expansion and Shrinking**: Simulate heap size adjustments by modifying the program break.

## Flow Chart
### First Fit 
![Screenshot from 2024-08-21 20-31-56](https://github.com/user-attachments/assets/13ada887-5035-4f4c-afe2-74191acdd6c1)

#### Static array show
![Screenshot from 2024-08-21 01-46-11](https://github.com/user-attachments/assets/e5cbd50a-edcf-49c1-893d-7d842f8b7422)


#### case 1 
![Screenshot from 2024-08-21 20-45-01](https://github.com/user-attachments/assets/a6853a71-7a87-4593-8904-c7b830938d43)

#### case 2
![Screenshot from 2024-08-21 20-46-43](https://github.com/user-attachments/assets/2ca76c33-1b6e-459b-a64c-8c0aed579582)

#### case 3
![Screenshot from 2024-08-21 20-49-13](https://github.com/user-attachments/assets/bf775638-2eae-4351-968c-3532546dff78)

#### Extended Break pointer
![Screenshot from 2024-08-21 01-48-25](https://github.com/user-attachments/assets/685594ce-424f-4329-9afc-2e09522a7f67)

![Screenshot from 2024-08-21 01-48-54](https://github.com/user-attachments/assets/efb2223b-80b1-4070-8614-52dd5f567a6b)

![Screenshot from 2024-08-21 01-49-32](https://github.com/user-attachments/assets/daffccb6-5c6a-4c09-84cd-d4a94698baa7)

![Screenshot from 2024-08-21 01-50-07](https://github.com/user-attachments/assets/5a58f471-ee57-40f1-9d90-a962dbcd6b1c)

#### Avoid Padding
![Screenshot from 2024-08-21 01-50-29](https://github.com/user-attachments/assets/e85acd2b-c203-4aa0-82be-95cf28e6edc8)


-------------------------------------------------------------------------------------------------------------------

### Best Fit 
![Screenshot from 2024-08-21 21-13-28](https://github.com/user-attachments/assets/a5985c56-e0b5-4f21-8552-f1b8bd1e8df5)

#### Static array show
+ the same concept of first fit figures

-------------------------------------------------------------------------------------------------------------------

### Free Block 
![Screenshot from 2024-08-21 21-46-51](https://github.com/user-attachments/assets/285e3f78-9fb6-4eec-a29b-37b532321a47)

#### General Cases 
![cases](https://github.com/user-attachments/assets/82beee69-6c88-4df7-8b61-6af25fdadc42)

#### case 1 
![case1 ](https://github.com/user-attachments/assets/b09e81f6-1c42-4869-8c8f-682d6083806d)

#### case 2
![case2](https://github.com/user-attachments/assets/28e0d67e-72bc-4e4f-9e18-5530bea2cc2a)

#### case 3
![Screenshot from 2024-08-21 01-53-25](https://github.com/user-attachments/assets/6340f580-859a-4253-8d2d-3a466101b4ef)

#### case 3-1
![Screenshot from 2024-08-21 01-54-03](https://github.com/user-attachments/assets/06a7c834-e211-4fe1-b309-41ad8bb253bc)

#### case 3-2
![Screenshot from 2024-08-21 01-54-25](https://github.com/user-attachments/assets/64a8be88-9aad-4cd3-8272-1f42d22b3c30)

#### case 3-3
![Screenshot from 2024-08-21 01-54-47](https://github.com/user-attachments/assets/8ddf9a55-0635-4b02-b62d-90a071263d84)

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
   The Makefile supports different build types, such as DEBUG. You can set the build type by modifying the BUILD_TYPE variable in the Makefile:
```
BUILD_TYPE=DEBUG
```

3. Running the Program:
  After successful compilation, you can run the heap manager by executing:
```
./heap_manager
```

4. Clean the Build:
To clean up the compiled files, you can use the clean target:
```
make clean
```


## Notes
+ The heap size is simulated using a statically allocated array and a variable representing the program break.
+ This implementation is designed for user-space testing and debugging, with no kernel-level interaction.
+ Ensure that you have the necessary permissions to execute and access the required files.

## Illustrate Videos
For more information, refer to the [BestFit](https://drive.google.com/file/d/1ouaNFC1mB3zyFYNj4ZnmFE_nSQve8DMU/view?usp=drive_link) video. [FirstFit](https://drive.google.com/file/d/1hXbb8YoI0W-jOS7o307Qu74nbWCHRHZM/view?usp=drive_link).
refer to [Free](https://drive.google.com/file/d/1rSVcubXRlauPS18s_mWOQZF-WEHjgn69/view?usp=drive_link).





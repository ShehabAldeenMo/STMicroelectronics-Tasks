# Fdisk

## Table of Contents
- [Description](#description)
- [Features](#features)
- [Function Descriptions](#function-descriptions)
- [Build Instruction](#build-instruction)
- [Notes](#notes)
- [Illustrative Examples](#illustrative-examples)

## Description

The `Fdisk` utility manages disk partitions by providing functionalities to manipulate partition tables. This tool supports both MBR and GPT partitioning schemes, allowing users to view disk partitions. It operates on raw disk images or real disks.

## Features

- **View Partition Tables**: Display the existing partition tables for both MBR and GPT.
- **Partition Information**: Provides detailed information about partitions, such as type, size, and location.

## Function Descriptions

### `void Gpt_PartitionInfo(char *partition, uint8_t num, GPT_PartitionEntry *entry)`

- **Description**: Retrieves and prints detailed information about a GPT partition entry.
- **Input**: 
  - `partition`: A string representing the partition identifier.
  - `num`: The index of the partition entry.
  - `entry`: A pointer to a `GPT_PartitionEntry` structure to store partition details.
- **Output**: None
- **Notes**: This function is used for displaying information about partitions defined in the GUID Partition Table.

### `void Gpt_ReverseGUID(const uint8_t *type_guid, char *guid)`

- **Description**: Converts a GUID from binary format to a human-readable string format.
- **Input**: 
  - `type_guid`: A pointer to a binary GUID.
  - `guid`: A buffer to store the human-readable GUID string.
- **Output**: None
- **Notes**: This function helps in converting and displaying GUIDs in a readable format.

### `const char* GPT_GetPartitionTypeName(const char *type)`

- **Description**: Retrieves the name of a GPT partition type based on its identifier.
- **Input**: 
  - `type`: A string representing the partition type identifier.
- **Output**: Returns the name of the partition type.
- **Notes**: Useful for mapping partition type codes to descriptive names.

### `void MBR_PartitionInfo(char *partition, uint8_t num, MBR_PartitionEntry *entry, uint32_t base_lba)`

- **Description**: Retrieves and prints detailed information about an MBR partition entry.
- **Input**: 
  - `partition`: A string representing the partition identifier.
  - `num`: The index of the partition entry.
  - `entry`: A pointer to a `MBR_PartitionEntry` structure to store partition details.
  - `base_lba`: The base LBA address for the partition table.
- **Output**: None
- **Notes**: This function is used for displaying information about partitions defined in the Master Boot Record.

### `void MBR_ParseEbr(int fd, char *partition, uint32_t block, uint8_t *num)`

- **Description**: Parses an Extended Boot Record (EBR) from a specified disk file descriptor.
- **Input**: 
  - `fd`: File descriptor for the disk.
  - `partition`: A string representing the partition identifier.
  - `block`: The starting block for parsing.
  - `num`: A pointer to store the number of partitions.
- **Output**: None
- **Notes**: This function extracts and processes extended partitions in the MBR partitioning scheme.

### `const char* MBR_GetPartitionTypeName(uint8_t type)`

- **Description**: Retrieves the name of an MBR partition type based on its identifier.
- **Input**: 
  - `type`: An 8-bit integer representing the partition type identifier.
- **Output**: Returns the name of the partition type.
- **Notes**: Useful for mapping partition type codes to descriptive names.

## Build Instruction

To build the Fdisk project, follow these steps:

1. Clone the Repository:
   Clone the repository to your local machine using Git:
```
   git clone https://github.com/ShehabAldeenMo/STMicroelectronics-Tasks.git
   cd 8- fdisk
   make
   sudo ./Fdisk_Partitions /dev/sdb
   sudo ./Fdisk_Partitions /dev/sda
```

## Illustrative Examples
+ Our program
![image](https://github.com/user-attachments/assets/ea58171e-470e-47ac-9472-56993cfba167)

+ fdisk command
![Screenshot from 2024-08-31 22-20-08](https://github.com/user-attachments/assets/4b8c0392-21a7-4d0d-af61-efa2fdb00a52)

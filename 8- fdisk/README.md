# Fdisk 💾

## Table of Contents 📑
- [Description](#description) 📝
- [Features](#features) ✨
- [Function Descriptions](#function-descriptions) 🛠️
- [Build Instruction](#build-instruction) 🏗️
- [Notes](#notes) 🗒️
- [Illustrative Examples](#illustrative-examples) 📸

## Description 📝

The `Fdisk` utility is a powerful command-line tool designed for managing and inspecting disk partitions on Unix-like systems. It provides users with the ability to view and manipulate partition tables using both the MBR (Master Boot Record) and GPT (GUID Partition Table) partitioning schemes. The utility is versatile, supporting operations on both raw disk images and physical disk drives, making it a valuable tool for system administrators, developers, and anyone involved in disk management.

## Features 🚀

With `Fdisk`, users can:

+ **Inspect Disk Layouts** 🛠️: Obtain a detailed overview of all partitions on a disk, including information about the partition type, size, starting and ending sectors, and more.

+ **Work with Multiple Partition Schemes** 🔄: Seamlessly switch between MBR and GPT, allowing for flexible disk management in different environments.

+ **Convert and Display GUIDs** 🔍: Convert binary GUIDs to a human-readable format, making it easier to understand and work with GPT partition types.

+ **Analyze Partition Details** 📊: Retrieve and display comprehensive details about individual partitions, whether they are defined in MBR or GPT.

## Function Descriptions 🛠️

### **Gpt_ParseInfo**
  + **Description:** Parses and displays partition information from a GPT (GUID Partition Table) disk. The function reads partition entries from a specified file descriptor and prints their details.
  + **Input:**
    + `path`: The file path of the disk image or device.
    + `fd`: The file descriptor for the disk image or device.
  + **Output**:
      + Displays partition information on the console.
  + **Notes:**
    + This function supports GPT partitioning schemes only.
    + The function assumes the first 512 bytes are the protective MBR.

### **Gpt_PrintPartitionInfo**
  + **Description:** Prints detailed information about a specific GPT (GUID Partition Table) partition, including its GUID, size, and type.
  + **Input:**
    + `partition`: The partition label or identifier.
    + `num`: The partition number.
    + `entry`: A pointer to the `GPT_PartitionEntry` structure containing the partition's data.
  + **Output:**
      + Displays partition details on the console.
  + **Notes:**
    + The function skips partitions that have `first_lba` and `end_lba` set to 0.
      
### **Gpt_ReverseGUID**
  + **Description:** Converts and reverses a binary GUID into a human-readable format.
  + **Input:**
    + `type_guid`: A pointer to the binary GUID.
    + `guid`: A buffer to store the formatted GUID string.
  + **Output:**
      + A formatted GUID string in the `guid` buffer.
  + **Notes:**
    + The function reverses the byte order for certain segments of the GUID as per GPT specifications.

### **GPT_GetPartitionTypeName**
  + **Description:** Retrieves the human-readable name associated with a given GUID for a GPT partition.
  + **Input:**
    + `type`: The GUID of the partition type in string format.
  + **Output:**
      + Returns the name of the partition type.
  + **Notes:**
    + Returns "Unresolved Type" if the GUID is not recognized.

### **MBR_ParseInfo**
  + **Description:** Parses and displays partition information from an MBR (Master Boot Record) disk. It handles both primary and extended partitions, including those with logical partitions.
  + **Input:**
    + `path`: The file path of the disk image or device.
    + `fd`: The file descriptor for the disk image or device.
    + `buf`: A buffer containing the MBR sector data.
  + **Output:**
      + Displays partition information on the console.
  + **Notes:**
    + This function supports MBR partitioning schemes only.
    + Extended partitions are handled by recursively parsing the Extended Boot Record (EBR).

### **MBR_PrintPartitionInfo**
  + **Description:** Prints detailed information about a specific MBR partition, including its type, size, and status.
  + **Input:**
    + `partition`: The partition label or identifier.
    + `num`: The partition number.
    + `entry`: A pointer to the `MBR_PartitionEntry` structure containing the partition's data.
    + `base_lba`: The base LBA for the partition (used for logical partitions).
  + **Output:**
      + Displays partition details on the console.
  + **Notes:**
    + The function skips partitions that have `lba` and `sector_count` set to 0.

### **MBR_GetPartitionTypeName**
  + **Description:** Retrieves the human-readable name associated with a given partition type ID for an MBR partition.
  + **Input:**
    + `type`: The partition type ID in hexadecimal format.
  + **Output:**
      + Returns the name of the partition type.
  + **Notes:**
    + Returns "Unresolved Type" if the type ID is not recognized.

### **MBR_ParseEbr**
  + **Description:** Recursively parses and displays partition information from an Extended Boot Record (EBR), handling logical partitions within an extended MBR partition.
  + **Input:**
    + `fd`: The file descriptor for the disk image or device.
    + `partition`: The partition label or identifier.
    + `ebr_start`: The starting LBA of the EBR.
    + `num`: A pointer to the partition number counter.
  + **Output:**
      + Displays logical partition information on the console.
  + **Notes:**
    + This function is specifically designed for handling extended partitions in an MBR disk.

## Build Instruction

To build the Fdisk project, follow these steps:

1. Clone the Repository:
   Clone the repository to your local machine using Git:
```
   git clone https://github.com/ShehabAldeenMo/STMicroelectronics-Tasks.git
   cd "8- fdisk"
   make
   sudo ./Fdisk_Partitions /dev/sdb
   sudo ./Fdisk_Partitions /dev/sda
```

## Illustrative Examples
+ Our program
![image](https://github.com/user-attachments/assets/ea58171e-470e-47ac-9472-56993cfba167)

+ fdisk command
![Screenshot from 2024-08-31 22-20-08](https://github.com/user-attachments/assets/4b8c0392-21a7-4d0d-af61-efa2fdb00a52)

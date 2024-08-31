/*============================================================================
 * @file name      : Fdisk.h
 * @Author         : Shehab Aldeen Mohammed
 * @Github         : https://github.com/ShehabAldeenMo
 * @LinkedIn       : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Description:
 * This file contains functions for handling and displaying partition information
 * from disk images or partition tables. It supports both GPT and MBR formats.
=============================================================================
 * @Notes:
 *
 ******************************************************************************
 ==============================================================================
*/
#ifndef _GPT_H_
#define _GPT_H_

/*===================================  Includes ===============================*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h> 

/*==================================  Definitions =============================*/
#define GPT_FORMATE                 0xEE        /* GPT format identifier */
#define SECTOR_SIZE                 512         /* Size of a sector in bytes */
#define INVALID                     -1          /* Error indicator */
#define GPT_ENTRIES_NUM             128         /* Number of GPT entries */
#define GPT_ENTRY_SIZE              128         /* Size of a single GPT entry */
#define _1MB                        (1024 * 1024) /* 1 Megabyte in bytes */
#define GPT_GUID_TYPE_LENGTH        36          /* Length of a GUID in characters */
#define CHS_EXTENDED_PARTITION      0x05        /* CHS extended partition type code */
#define LBA_EXTENDED_PARTITION      0x0F        /* LBA extended partition type code */
#define LINUX_EXTENDED_PARTITION    0x85        /* Linux extended partition type code */
#define VALID                       0           /* Valid status code */
#define INITIAL_BOOTLOADER          446         /* Initial bootloader offset in MBR */
#define NUM_OF_MBR_PARRTITIONS       4           /* Number of MBR partitions */
#define GPT_OFFEST_HEADER            2           /* GPT header offset in MBR */

/*===================================  Structures ============================*/

/* Structure for GPT Partition Entry */
typedef struct {
    uint8_t  type[16];          /* Partition type GUID */
    uint8_t  guid[16];          /* Partition GUID */
    uint64_t first_lba;         /* First LBA of the partition */
    uint64_t end_lba;           /* End LBA of the partition */
    uint64_t attributes;        /* Attributes of the partition */
    uint16_t name[36];          /* Name of the partition (UTF-16) */
} GPT_PartitionEntry;

/* Structure for GPT Partition Type */
typedef struct {
    const char *guid;           /* GUID of the partition type */
    const char *name;           /* Name of the partition type */
} GPT_PartitionType;

/* Structure for MBR Partition Entry */
typedef struct {
    uint8_t  status;           /* Status of the partition (bootable flag) */
    uint8_t  first_chs[3];     /* CHS address of the first sector */
    uint8_t  partition_type;   /* Partition type code */
    uint8_t  last_chs[3];      /* CHS address of the last sector */
    uint32_t lba;              /* LBA address of the partition */
    uint32_t sector_count;     /* Number of sectors in the partition */
} MBR_PartitionEntry;

/* Structure for MBR Partition Type */
typedef struct {
    uint8_t  id;               /* Partition type ID */
    const char *name;          /* Name of the partition type */
} MBR_PartitionType;

/*==========================  Function Prototypes ===========================*/

/* Print information about a GPT partition */
void Gpt_PartitionInfo(char *partition, uint8_t num, GPT_PartitionEntry *entry);

/* Convert GPT GUID to string format */
void Gpt_ReverseGUID(const uint8_t *type_guid, char *guid);

/* Get the partition type name from the GPT type GUID */
const char* GPT_GetPartitionTypeName(const char *type);

/* Print information about an MBR partition */
void MBR_PartitionInfo(char *partition, uint8_t num, MBR_PartitionEntry *entry, uint32_t base_lba);

/* Parse and print information about extended boot records (EBRs) */
void MBR_ParseEbr(int fd, char *partition, uint32_t block, uint8_t *num);

/* Get the partition type name from the MBR type ID */
const char* MBR_GetPartitionTypeName(uint8_t type);

#endif /* _GPT_H_ */

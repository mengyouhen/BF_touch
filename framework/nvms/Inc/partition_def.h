/******************************************************************************
 Copyright (c) 2018 Flex(Shanghai) Co.,Ltd.
 All rights reserved.

 [Module Name]:
 		partition_def.h
 [Date]:
 		2018-07-09
 [Author]:
 		Tim Lin
 [Reversion History]:
 		v1.0
*******************************************************************************/

#ifndef PARTITION_DEF_H_
#define PARTITION_DEF_H_

/**
 * \brief NVMS Partition IDs
 */
typedef enum
{
	EEPROM_PARAM_PART		= 1,
	EEPROM_LOG_PART		= 2,
	NVMS_GENERIC_PART		= 3,
} nvms_partition_id_t;



/**
 * \brief Partition entry.
 */
typedef struct partition_entry_t {
        uint8_t magic;          /**< Partition magic number 0xEA */
        uint8_t type;           /**< Partition ID */
        uint8_t valid;          /**< Valid marker 0xFF */
        uint8_t flags;          /**< */
        uint16_t start_sector;  /**< Partition start sector */
        uint16_t sector_count;  /**< Number of sectors in partition */
        uint8_t reserved2[8];   /**< Reserved for future use */
} partition_entry_t;

#define PARTITION_FLAG_READ_ONLY	(1 << 0)
#define PARTITION_FLAG_VES			(1 << 1)

#endif /* PARTITION_DEF_H_ */


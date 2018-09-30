/******************************************************************************
 Copyright (c) 2018 Flex(Shanghai) Co.,Ltd.
 All rights reserved.

 [Module Name]:
 		nvms.h
 [Date]:
 		2018-07-09
 [Author]:
 		Tim Lin
 [Reversion History]:
 		v1.0
*******************************************************************************/

#ifndef _NVMS_H_
#define _NVMS_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <partition_def.h>


typedef void *nvms_t;

#if 0
/**
 * \brief NVMS partition type
 */
struct partition_t;

/**
 * \brief Partition driver functions.
 */
typedef struct driver_t {
        bool (* bind)(struct partition_t *part);
        size_t (* get_size)(struct partition_t *part);
        int (* read)(struct partition_t *part, uint32_t addr, uint8_t *buf, uint32_t size);
        int (* write)(struct partition_t *part, uint32_t addr, const uint8_t *buf, uint32_t size);
        bool (* erase)(struct partition_t *part, uint32_t addr, uint32_t size);
        size_t (* get_ptr)(struct partition_t *part, uint32_t addr, uint32_t size, const void **ptr);
        void (* flush)(struct partition_t *part, bool free_mem);
} partition_driver_t;


/**
 * \brief Partition structure for RAM
 */
typedef struct partition_t {
        struct partition_t *next;        /**< Next partition */
        const partition_driver_t *driver;/**< Bound driver */
        void *driver_data;               /** Driver data */
        partition_entry_t data;          /** Partition information data */
} partition_t;
#endif

/**
 * \brief NVMS simple partition type
 */
struct simple_partition_t;

/**
 * \brief Partition driver functions.
 */
typedef struct simple_driver_t {
        int (* read)(struct simple_partition_t *part, uint32_t addr, uint8_t *buf, uint32_t size);
        int (* write)(struct simple_partition_t *part, uint32_t addr, const uint8_t *buf, uint32_t size);
        bool (* erase)(struct simple_partition_t *part, uint32_t addr, uint32_t size);
} simple_partition_driver_t;


/**
 * \brief Simple Partition structure
 */
typedef struct simple_partition_t {
	uint8_t type;           /**< Partition ID */
	size_t start;		/*start address*/
	size_t size;		/*size of partiton*/
        const simple_partition_driver_t *driver;/**< Bound driver */
} simple_partition_t;


void nvms_init(void);
nvms_t nvms_open(nvms_partition_id_t id);
size_t nvms_get_size(nvms_t handle);
int nvms_read(nvms_t handle, uint32_t addr, uint8_t *buf, uint32_t len);
int nvms_write(nvms_t handle, uint32_t addr, const uint8_t *buf, uint32_t size);


#endif /* _NVMS_H_ */


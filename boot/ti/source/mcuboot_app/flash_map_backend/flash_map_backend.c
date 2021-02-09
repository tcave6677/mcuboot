/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 * Copyright (c) 2020 Cypress Semiconductor Corporation
 * Copyright (c) 2021 Texas Instruments
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 /*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
 /*******************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "mcuboot_config/mcuboot_config.h"
#include "flash_map_backend/flash_map_backend.h"
#include <sysflash/sysflash.h>

#include "bootutil/bootutil_log.h"

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/flash.h)
#include DeviceFamily_constructPath(driverlib/vims.h)

#define FLASH_BASE_ADDRESS          0
#define BOOT_BOOTLOADER_SIZE        32768
#define BOOT_PRIMARY_1_SIZE         131072
#define BOOT_SECONDARY_1_SIZE       131072

#define FLASH_ERASE_VALUE           0xFF

#define assert(x) if(x) {BOOT_LOG_ERR("assert: %s line %d", __FILE__, __LINE__);}

#ifdef TI_FLASH_MAP_EXT_DESC
/* Nothing to be there when external FlashMap Descriptors are used */
#else
static struct flash_area bootloader =
{
    .fa_id = FLASH_AREA_BOOTLOADER,
    .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH,
    .fa_off = FLASH_BASE_ADDRESS,
    .fa_size = BOOT_BOOTLOADER_SIZE
};

static struct flash_area primary_1 =
{
    .fa_id = FLASH_AREA_IMAGE_PRIMARY(0),
    .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH,
    .fa_off = FLASH_BASE_ADDRESS + BOOT_BOOTLOADER_SIZE,
    .fa_size = BOOT_PRIMARY_1_SIZE
};

#ifndef TI_BOOT_USE_EXTERNAL_FLASH
static struct flash_area secondary_1 =
{
    .fa_id = FLASH_AREA_IMAGE_SECONDARY(0),
    .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH,
    .fa_off = FLASH_BASE_ADDRESS +\
              BOOT_BOOTLOADER_SIZE +\
              BOOT_PRIMARY_1_SIZE,
    .fa_size = BOOT_SECONDARY_1_SIZE
};
#else
#error "External flash not supported"
#endif
#if (MCUBOOT_IMAGE_NUMBER == 2) /* if dual-image */
#error "Dual-image not supported"
#endif
#endif

#ifdef MCUBOOT_SWAP_USING_SCRATCH
static struct flash_area scratch =
{
#error "MCUBOOT_SWAP_USING_SCRATCH not supported"
};
#endif

#ifdef TI_FLASH_MAP_EXT_DESC
/* Use external Flash Map Descriptors */
extern struct flash_area *boot_area_descs[];
#else
struct flash_area *boot_area_descs[] =
{
    &bootloader,
    &primary_1,
    &secondary_1,
#if (MCUBOOT_IMAGE_NUMBER == 2) /* if dual-image */
    &primary_2,
    &secondary_2,
#endif
#ifdef MCUBOOT_SWAP_USING_SCRATCH
    &scratch,
#endif
    NULL
};
#endif

/* Prepares system for a write to flash */
static uint8_t disableCache(void)
{
  uint8_t state = VIMSModeGet(VIMS_BASE);

  // Check VIMS state
  if (state != VIMS_MODE_DISABLED)
  {
    // Invalidate cache
    VIMSModeSet(VIMS_BASE, VIMS_MODE_DISABLED);

    // Wait for disabling to be complete
    while (VIMSModeGet(VIMS_BASE) != VIMS_MODE_DISABLED);
  }

  return (state);
}

/* Prepares system for a write to flash */
static void enableCache(uint8_t state)
{
  if (state != VIMS_MODE_DISABLED)
  {
    // Enable the Cache.
    VIMSModeSet(VIMS_BASE, VIMS_MODE_ENABLED);
  }
}

/* Returns device flash start based on supported fa_id */
int flash_device_base(uint8_t fd_id, uintptr_t *ret)
{
    if (fd_id != FLASH_DEVICE_INTERNAL_FLASH) {
        BOOT_LOG_ERR("invalid flash ID %d; expected %d",
                     fd_id, FLASH_DEVICE_INTERNAL_FLASH);
        return -1;
    }
    *ret = FLASH_BASE_ADDRESS;
    return 0;
}

/* Opens the area for use. id is one of the `fa_id`s */
int flash_area_open(uint8_t id, const struct flash_area **fa)
{
    int ret = -1;
    uint32_t i = 0;

    while(NULL != boot_area_descs[i])
    {
        if(id == boot_area_descs[i]->fa_id)
        {
            *fa = boot_area_descs[i];
            ret = 0;
            break;
        }
        i++;
    }
    return ret;
}

void flash_area_close(const struct flash_area *fa)
{
    (void)fa;/* Nothing to do there */
}

/*
* Reads `len` bytes of flash memory at `off` to the buffer at `dst`
*/
int flash_area_read(const struct flash_area *fa, uint32_t off, void *dst,
                     uint32_t len)
{
    int rc = 0;
    size_t addr;

    /* check if requested offset not less then flash area (fa) start */
    assert(off < fa->fa_off);
    assert(off + len < fa->fa_off);
    /* convert to absolute address inside a device*/
        addr = fa->fa_off + off;

    if (fa->fa_device_id == FLASH_DEVICE_INTERNAL_FLASH)
    {
        /* flash read by simple memory copying */
        memcpy((void *)dst, (const void*)addr, (size_t)len);
    }
    else
    {
        /* incorrect/non-existing flash device id */
        rc = -1;
    }

    if (rc != 0) {
        BOOT_LOG_ERR("Flash area read error, rc = %d", (int)rc);
    }
    return rc;
}

/*
* Writes `len` bytes of flash memory at `off` from the buffer at `src`
 */
int flash_area_write(const struct flash_area *fa, uint32_t off,
                     const void *src, uint32_t len)
{
    int rc = -1;
    size_t write_start_addr;

    assert(off < fa->fa_off);
    assert(off + len < fa->fa_off);

    /* convert to absolute address inside a device */
    write_start_addr = fa->fa_off + off;

    if (fa->fa_device_id == FLASH_DEVICE_INTERNAL_FLASH)
    {
        uint8_t cacheState;
        uint32_t flashStat;

        cacheState = disableCache();

        flashStat = FlashProgram((uint8_t *)src, write_start_addr, len);
        if(flashStat == FAPI_STATUS_SUCCESS)
        {
            rc = 0;
        }

        enableCache(cacheState);
    }
    else
    {
        /* incorrect/non-existing flash device id */
        rc = -1;
    }

    return rc;
}

/*< Erases `len` bytes of flash memory at `off` */
int flash_area_erase(const struct flash_area *fa, uint32_t off, uint32_t len)
{
    int rc = -1;
    size_t erase_start_addr;

    assert(off < fa->fa_off);
    assert(off + len < fa->fa_off);

    /* convert to absolute address inside a device*/
    erase_start_addr = fa->fa_off + off;

    if (fa->fa_device_id == FLASH_DEVICE_INTERNAL_FLASH)
    {
        uint8_t cacheState;
        uint8_t flashStat;
        uint8_t pageAddr;
        uint32_t sectorSize = FlashSectorSizeGet();

        assert(!(erase_start_addr % sectorSize));

        cacheState = disableCache();

        for(pageAddr = erase_start_addr; pageAddr < erase_start_addr + len; pageAddr+=sectorSize)
        {
            flashStat = FlashSectorErase(pageAddr);
            if(flashStat == FAPI_STATUS_SUCCESS)
            {
                rc = 0;
                enableCache(cacheState);
                return rc;
            }
        }

        enableCache(cacheState);
    }
    else
    {
        /* incorrect/non-existing flash device id */
        rc = -1;
    }
    return (int) rc;
}

/*< Returns this `flash_area`s alignment */
size_t flash_area_align(const struct flash_area *fa)
{
    int ret = -1;
    if (fa->fa_device_id == FLASH_DEVICE_INTERNAL_FLASH)
    {
        ret = FlashSectorSizeGet();
    }
    else
    {
        /* incorrect/non-existing flash device id */
        ret = -1;
    }
    return ret;
}

#ifdef MCUBOOT_USE_FLASH_AREA_GET_SECTORS
/*< Initializes an array of flash_area elements for the slot's sectors */
int     flash_area_to_sectors(int idx, int *cnt, struct flash_area *fa)
{
    int rc = 0;

    if (fa->fa_device_id == FLASH_DEVICE_INTERNAL_FLASH)
    {
        (void)idx;
        (void)cnt;
        rc = 0;
    }
    else
    {
        /* incorrect/non-existing flash device id */
        rc = -1;
    }
    return rc;
}
#endif

/*
 * This depends on the mappings defined in sysflash.h.
 * MCUBoot uses continuous numbering for the primary slot, the secondary slot,
 * and the scratch while zephyr might number it differently.
 */
int flash_area_id_from_multi_image_slot(int image_index, int slot)
{
    switch (slot) {
    case 0: return FLASH_AREA_IMAGE_PRIMARY(image_index);
    case 1: return FLASH_AREA_IMAGE_SECONDARY(image_index);
    case 2: return FLASH_AREA_IMAGE_SCRATCH;
    }

    return -1; /* flash_area_open will fail on that */
}

int flash_area_id_from_image_slot(int slot)
{
    return flash_area_id_from_multi_image_slot(0, slot);
}

int flash_area_id_to_multi_image_slot(int image_index, int area_id)
{
    if (area_id == FLASH_AREA_IMAGE_PRIMARY(image_index)) {
        return 0;
    }
    if (area_id == FLASH_AREA_IMAGE_SECONDARY(image_index)) {
        return 1;
    }

    return -1;
}

int flash_area_id_to_image_slot(int area_id)
{
    return flash_area_id_to_multi_image_slot(0, area_id);
}

uint8_t flash_area_erased_val(const struct flash_area *fap)
{
    int ret = 0;

    if (fap->fa_device_id == FLASH_DEVICE_INTERNAL_FLASH)
    {
        ret = FLASH_ERASE_VALUE;
    }
    else
    {
        assert(false) ;
    }

    return ret ;
}

int flash_area_read_is_empty(const struct flash_area *fa, uint32_t off,
        void *dst, uint32_t len)
{
    uint8_t *mem_dest;
    int rc;

    mem_dest = (uint8_t *)dst;
    rc = flash_area_read(fa, off, dst, len);
    if (rc) {
        return -1;
    }

    for (uint8_t i = 0; i < len; i++) {
        if (mem_dest[i] != flash_area_erased_val(fa)) {
            return 0;
        }
    }
    return 1;
}

#ifdef MCUBOOT_USE_FLASH_AREA_GET_SECTORS
int flash_area_get_sectors(int idx, uint32_t *cnt, struct flash_sector *ret)
{
    int rc = 0;
    uint32_t i = 0;
    struct flash_area *fa = NULL;

    while(NULL != boot_area_descs[i])
    {
        if(idx == boot_area_descs[i]->fa_id)
        {
            fa = boot_area_descs[i];
            break;
        }
        i++;
    }

    if(NULL != boot_area_descs[i])
    {
        size_t sector_size = 0;

        if(fa->fa_device_id == FLASH_DEVICE_INTERNAL_FLASH)
        {
            sector_size = FlashSectorSizeGet();
        }
        else
        {
            rc = -1;
        }

        if(0 == rc)
        {
            uint32_t addr = 0;
            size_t sectors_n = 0;

            sectors_n = (fa->fa_size + (sector_size - 1)) / sector_size;
            assert(sectors_n <= *cnt);

            addr = fa->fa_off;
            for(i = 0; i < sectors_n; i++)
            {
                ret[i].fs_size = sector_size ;
                ret[i].fs_off = addr ;
                addr += sector_size ;
            }

            *cnt = sectors_n;
        }
    }
    else
    {
        rc = -1;
    }

    return rc;
}
#endif

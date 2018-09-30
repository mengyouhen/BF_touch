/******************************************************************************
 Copyright (c) 2018 Flex(Shanghai) Co.,Ltd.
 All rights reserved.

 [Module Name]:
 		nvparam.h
 [Date]:
 		2018-07-09
 [Author]:
 		Tim Lin
 [Reversion History]:
 		v1.0
*******************************************************************************/

#ifndef _NVPARAM_H_
#define _NVPARAM_H_

/*
 * DEFINES
 ****************************************************************************************
 */

/**
 * \brief NV-Parameters area handle
 *
 * This is opaque pointer returned when opening NV-Parameters area and it should not be accessed
 * directly.
 *
 */
typedef void* nvparam_t;

/**
 * \brief Open NV-Parameters area
 *
 * \note
 * In Debug configuration this call will check consistency of area configuration (i.e. whether
 * parameters do not overlap each other) and will assert on error.
 *
 * \param [in] area_name  area configuration name
 *
 * \return area handle or NULL if failed
 *
 */
nvparam_t nvparam_open(const char *area_name);

/**
 * \brief Close NV-Parameters area
 *
 * \param [in] nvparam  area handle
 *
 */
void nvparam_close(nvparam_t nvparam);

/**
 * \brief Erase NV-Parameters area
 *
 * This will erase whole parameters area. It's a shortcut to call ad_nvparam_erase() on each
 * parameter.
 *
 * \param [in] nvparam  area handle
 *
 */
void nvparam_erase_all(nvparam_t nvparam);

/**
 * \brief Erase single NV-Parameter
 *
 * This will erase single NV-Parameter to default value
 *
 * \param [in] nvparam  area handle
 * \param [in] tag      parameter tag
 *
 */
void nvparam_erase(nvparam_t nvparam, uint8_t tag);

/**
 * \brief Read parameter value with offset
 *
 * This will read parameter value starting from \p offset and up to \p length bytes. Number of bytes
 * read may be different than requested if length of parameter is less than requested (including
 * offset).
 *
 * If \p tag points to invalid parameter, 0 will be returned.
 *
 * \param [in]  nvparam  area handle
 * \param [in]  tag      parameter tag
 * \param [in]  offset   requested value offset
 * \param [in]  length   requested length
 * \param [out] data     output buffer
 *
 * \return number of bytes read
 *
 */
uint16_t nvparam_read_offset(nvparam_t nvparam, uint8_t tag, uint16_t offset,
                                                                uint16_t length, void *data);

/**
 * \brief Read parameter value
 *
 * This call has the same effect as calling ad_nvparam_read_offset() with \p offset set to 0.
 *
 * \param [in]  nvparam  area handle
 * \param [in]  tag      parameter tag
 * \param [in]  length   requested length
 * \param [out] data     output buffer
 *
 * \return number of bytes read
 *
 */
static inline uint16_t nvparam_read(nvparam_t nvparam, uint8_t tag, uint16_t length, void *data)
{
        return nvparam_read_offset(nvparam, tag, 0, length, data);
}

/**
 * \brief Write parameter value
 *
 * This will write parameter value of \p length bytes. If parameter length is less than requested
 * length, value will be truncated to fit in parameter area.
 *
 * If \p nvparam is NULL or \p tag points to invalid parameter, 0 will be returned.
 *
 * \param [in] nvparam  area handle
 * \param [in] tag      parameter tag
 * \param [in] length   requested length
 * \param [in] data     input buffer
 *
 * \return number of bytes written
 *
 */
uint16_t nvparam_write(nvparam_t nvparam, uint8_t tag, uint16_t length, const void *data);

/**
 * \brief Get current parameter length
 *
 * For variable length parameters this will return current parameter value length as stored in area.
 * Returned length may be less than parameter length defined. If \p max_length is not \p NULL,
 * maximum parameter length will be returned there.
 *
 * For constant length parameters
 *
 * If \p nvparam is NULL or \p tag points to invalid parameter, 0 will be returned.
 *
 * \param [in]  nvparam      area handle
 * \param [in]  tag          parameter tag
 * \param [out] max_length   maximum parameter length (optional)
 *
 * \return current parameter length
 *
 */
uint16_t nvparam_get_length(nvparam_t nvparam, uint8_t tag, uint16_t *max_length);


#endif /* _NVPARAM_H_ */


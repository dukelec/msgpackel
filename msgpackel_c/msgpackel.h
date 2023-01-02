/*
 * Software License Agreement (MIT License)
 *
 * Copyright (c) 2021, DUKELEC, Inc.
 * All rights reserved.
 *
 * Author: Duke Fong <d@d-l.io>
 */

#ifndef __MSGPACKEL_H__
#define __MSGPACKEL_H__

#include "cd_utils.h"

uint8_t *mpk_w_int(uint8_t *buf, int32_t val);
uint8_t *mpk_w_uint(uint8_t *buf, uint32_t val);
uint8_t *mpk_w_nil(uint8_t *buf);
uint8_t *mpk_w_bool(uint8_t *buf, bool val);
uint8_t *mpk_w_float(uint8_t *buf, float val);
uint8_t *mpk_w_str(uint8_t *buf, char *str);
uint8_t *mpk_w_bin(uint8_t *buf, uint8_t *dat, size_t len);
uint8_t *mpk_w_ext(uint8_t *buf, int8_t type, uint8_t *dat, size_t len);
uint8_t *mpk_w_array_hdr(uint8_t *buf, size_t num);
uint8_t *mpk_w_map_hdr(uint8_t *buf, size_t num);
#ifdef MPK_64BIT
uint8_t *mpk_w_int64(uint8_t *buf, int64_t val);
uint8_t *mpk_w_uint64(uint8_t *buf, uint64_t val);
uint8_t *mpk_w_float64(uint8_t *buf, double val);
#endif


typedef enum {
    MPK_ERR,
    MPK_INT,
    MPK_UINT,
    MPK_NIL,
    MPK_BOOL,
    MPK_FLOAT,
    MPK_STR,
    MPK_BIN,
    MPK_EXT,
    MPK_ARRAY,
    MPK_MAP,
#ifdef MPK_64BIT
    MPK_INT64,
    MPK_UINT64,
    MPK_FLOAT64,
#endif
} mpk_t;

mpk_t mpk_parse(uint8_t *buf, uint8_t **payload, uint8_t **end, void *val, size_t *num);
mpk_t mpk_map_search(uint8_t *buf, uint8_t **payload, uint8_t **end, void *val, size_t *num, ...);

#endif

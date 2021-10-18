/*
 * Software License Agreement (MIT License)
 *
 * Copyright (c) 2021, DUKELEC, Inc.
 * All rights reserved.
 *
 * Author: Duke Fong <d@d-l.io>
 */

#include "msgpackel.h"


uint8_t *mpk_w_int(uint8_t *buf, int32_t val)
{
    if (val >= -32 && val <= 127) { // fix-int
        *(int8_t *)buf = val;
        return buf + 1;
    }
    if (val >= -128 && val <= 127) { // int8
        *buf = 0xd0;
        *(int8_t *)(buf + 1) = val;
        return buf + 1 + 1;
    }
    if (val >= -32768 && val <= 32767) { // int16
        *buf = 0xd1;
        put_unaligned16(val, buf + 1);
        return buf + 1 + 2;
    }
    if (val >= INT32_MIN && val <= INT32_MAX) { // int32
        *buf = 0xd2;
        put_unaligned32(val, buf + 1);
        return buf + 1 + 4;
    }
    return NULL;
}

uint8_t *mpk_w_uint(uint8_t *buf, uint32_t val)
{
    if (val <= 255) { // uint8
        *buf = 0xcc;
        *(buf + 1) = val;
        return buf + 1 + 1;
    }
    if (val <= 65535) { // uint16
        *buf = 0xcd;
        put_unaligned16(val, buf + 1);
        return buf + 1 + 2;
    }
    if (val <= UINT32_MAX) { // uint32
        *buf = 0xce;
        put_unaligned32(val, buf + 1);
        return buf + 1 + 4;
    }
    return NULL;
}

uint8_t *mpk_w_nil(uint8_t *buf)
{
    *buf = 0xc0;
    return buf + 1;
}

uint8_t *mpk_w_bool(uint8_t *buf, bool val)
{
    *buf = val ? 0xc3 : 0xc2;
    return buf + 1;
}

uint8_t *mpk_w_float(uint8_t *buf, float val)
{
    *buf = 0xca;
    memcpy(buf + 1, &val, 4);
    return buf + 1 + 4;
}

uint8_t *mpk_w_str(uint8_t *buf, char *str)
{
    size_t len = strlen(str);
    if (len <= 31) {
        *buf = 0xa0 | len; // fix-str
        memcpy(buf + 1, str, len);
        return buf + 1 + len;
    }
    if (len <= 255) {
        *buf = 0xd9; // str8
        *(buf + 1) = len;
        memcpy(buf + 1 + 1, str, len);
        return buf + 1 + 1 + len;
    }
    if (len <= 65535) {
        *buf = 0xda; // str16
        put_unaligned16(len, buf + 1);
        memcpy(buf + 1 + 2, str, len);
        return buf + 1 + 2 + len;
    }
    return NULL;
}

uint8_t *mpk_w_bin(uint8_t *buf, uint8_t *dat, size_t len)
{
    if (len <= 255) {
        *buf = 0xc4; // bin8
        *(buf + 1) = len;
        memcpy(buf + 1 + 1, dat, len);
        return buf + 1 + 1 + len;
    }
    if (len <= 65535) {
        *buf = 0xc5; // bin16
        put_unaligned16(len, buf + 1);
        memcpy(buf + 1 + 2, dat, len);
        return buf + 1 + 2 + len;
    }

    *buf = 0xc6; // bin32
    put_unaligned32(len, buf + 1);
    memcpy(buf + 1 + 4, dat, len);
    return buf + 1 + 4 + len;
}

uint8_t *mpk_w_array_hdr(uint8_t *buf, size_t num)
{
    if (num <= 15) {
        *buf = 0x90 | num; // fix-array
        return buf + 1;
    }

    if (num <= 65535) {
        *buf = 0xdc; // array16
        put_unaligned16(num, buf + 1);
        return buf + 1 + 2;
    }

    return NULL;
}

uint8_t *mpk_w_map_hdr(uint8_t *buf, size_t num)
{
    if (num <= 15) {
        *buf = 0x80 | num; // fix-map
        return buf + 1;
    }

    if (num <= 65535) {
        *buf = 0xde; // map16
        put_unaligned16(num, buf + 1);
        return buf + 1 + 2;
    }

    return NULL;
}


mpk_t mpk_parse(uint8_t *buf, uint8_t **payload, uint8_t **end, uint8_t *val, size_t *num)
{
    mpk_t type = MPK_ERR;
    uint8_t *_payload = buf + 1;
    uint8_t *_end = buf + 1;
    uint8_t _val[4] = {0};
    size_t _num = 0;

    if (*buf <= 0x7f || *buf >= 0xe0) { // fix-int
        _payload = buf;
        *(int32_t *)_val = *(int8_t *)buf;
        type = MPK_INT;
    } else if (*buf == 0xd0) { // int8
        _end = buf + 1 + 1;
        *(int32_t *)_val = *(int8_t *)(buf + 1);
        type = MPK_INT;
    } else if (*buf == 0xd1) { // int16
        _end = buf + 1 + 2;
        *(int32_t *)_val = (int16_t)get_unaligned16(buf + 1);
        type = MPK_INT;
    } else if (*buf == 0xd2) { // int32
        _end = buf + 1 + 4;
        *(int32_t *)_val = get_unaligned32(buf + 1);
        type = MPK_INT;

    } else if (*buf == 0xcc) { // uint8
        _end = buf + 1 + 1;
        *(uint32_t *)_val = *(buf + 1);
        type = MPK_UINT;
    } else if (*buf == 0xcd) { // uint16
        _end = buf + 1 + 2;
        *(uint32_t *)_val = get_unaligned16(buf + 1);
        type = MPK_UINT;
    } else if (*buf == 0xce) { // uint32
        _end = buf + 1 + 4;
        *(uint32_t *)_val = get_unaligned32(buf + 1);
        type = MPK_UINT;

    } else if (*buf == 0xc0) { // nil
        _payload = buf;
        type = MPK_NIL;
    } else if (*buf == 0xc2 || *buf == 0xc3) { // bool
        _payload = buf;
        *(uint32_t *)_val = *_payload & 1;
        type = MPK_BOOL;

    } else if (*buf == 0xca) { // float
        _end = buf + 1 + 4;
        memcpy(_val, buf + 1, 4);
        type = MPK_FLOAT;

    } else if ((*buf & 0xe0) == 0xa0) { // fix-str
        _end = buf + 1 + (*buf & 0x1f);
        type = MPK_STR;
    } else if (*buf == 0xd9) { // str8
        _payload = buf + 1 + 1;
        _end = _payload + *(buf + 1);
        type = MPK_STR;
    } else if (*buf == 0xda) { // str16
        _payload = buf + 1 + 2;
        _end = _payload + get_unaligned16(buf + 1);
        type = MPK_STR;

    } else if (*buf == 0xc4) { // bin8
        _payload = buf + 1 + 1;
        _end = _payload + *(buf + 1);
        type = MPK_BIN;
    } else if (*buf == 0xc5) { // bin16
        _payload = buf + 1 + 2;
        _end = _payload + get_unaligned16(buf + 1);
        type = MPK_BIN;
    } else if (*buf == 0xc6) { // bin32
        _payload = buf + 1 + 4;
        _end = _payload + get_unaligned32(buf + 1);
        type = MPK_BIN;

    } else if ((*buf & 0xf0) == 0x90 || *buf == 0xdc) { // fix-arr, arr16
        if (*buf != 0xdc) {
            _num = *buf & 0xf;
        } else {
            _num = get_unaligned16(buf + 1);
            _payload = buf + 1 + 2;
        }
        uint8_t *sub_buf = _payload, *sub_end = _payload;
        for (int i = 0; i < _num; i++) {
            if (mpk_parse(sub_buf, NULL, &sub_end, NULL, NULL) == MPK_ERR)
                return MPK_ERR;
            sub_buf = sub_end;
        }
        _end = sub_end;
        type = MPK_ARRAY;
    }

    else if ((*buf & 0xf0) == 0x80 || *buf == 0xde) { // fix-map, map16
        if (*buf != 0xde) {
            _num = *buf & 0xf;
        } else {
            _num = get_unaligned16(buf + 1);
            _payload = buf + 1 + 2;
        }
        uint8_t *sub_buf = _payload, *sub_end = _payload;
        for (int i = 0; i < _num * 2; i++) {
            if (mpk_parse(sub_buf, NULL, &sub_end, NULL, NULL) == MPK_ERR)
                return MPK_ERR;
            sub_buf = sub_end;
        }
        _end = sub_end;
        type = MPK_MAP;
    }

    if (payload) *payload = _payload;
    if (end) *end = _end;
    if (val) memcpy(val, _val, 4);
    if (num) *num = _num;
    return type;
}

mpk_t mpk_map_search(uint8_t *buf, uint8_t **payload, uint8_t **end, uint8_t *val, size_t *num, ...)
{
    va_list va;
    char *key;
    uint8_t *map_payload;
    size_t map_num = 0;
    mpk_t ret = MPK_ERR;
    uint8_t *sub_payload;
    uint8_t *sub_end;
    bool is_found = false;

    va_start(va, num);
    while ((key = va_arg(va, char *))) {
        ret = mpk_parse(buf, &map_payload, NULL, NULL, &map_num);
        if (ret != MPK_MAP)
            return MPK_ERR;

        is_found = false;
        buf = map_payload;
        for (int i = 0; i < map_num; i++) {
            // compare key
            ret = mpk_parse(buf, &sub_payload, &sub_end, NULL, NULL);
            if (ret == MPK_ERR)
                return MPK_ERR;
            if (ret == MPK_STR && strlen(key) == sub_end - sub_payload
                    && memcmp(key, sub_payload, sub_end - sub_payload) == 0) {
                is_found = true;
                buf = sub_end;
                break; // found
            }
            // skip value
            buf = sub_end;
            ret = mpk_parse(buf, NULL, &sub_end, NULL, NULL);
            if (ret == MPK_ERR)
                return MPK_ERR;
            buf = sub_end;
        }
    }
    va_end(va);

    if (is_found)
        return mpk_parse(buf, payload, end, val, num);
    else
        return MPK_ERR;
}

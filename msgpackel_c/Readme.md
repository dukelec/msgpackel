Little-endian version of the msgpack C library
==============================================

This library does not require malloc.

### Usage

Data structure demo:
```
{
    "dev": "abc123",
    "data": {
        "voltage": 3.6,
        "angle": [45, 90]
    }
}
```

Serialization:

```
uint8_t msg_buf[256];

size_t prepare_data(void)
{
    uint8_t *pos = msg_buf;
    pos = mpk_w_map_hdr(pos, 2);        // create top map

    pos = mpk_w_str(pos, "dev");        // create first item
    pos =   mpk_w_str(pos, "abc123");

    pos = mpk_w_str(pos, "data");       // create second item
    pos =   mpk_w_map_hdr(pos, 2);      // create sub map
    pos =   mpk_w_str(pos, "voltage");
    pos =     mpk_w_float(pos, 3.6f);
    pos =   mpk_w_str(pos, "angle");
    pos =     mpk_w_array_hdr(pos, 2);
    pos =       mpk_w_int(pos, 45);
    pos =       mpk_w_int(pos, 90);

    return pos - msg_buf;               // return length
}
```

Deserialization:

```
void check_data(void)
{
    mpk_t st;
    uint8_t *payload, *end;
    size_t num;
    float voltage;


    st = mpk_map_search(msg_buf, NULL, NULL, &voltage, NULL,
                        "data", "voltage", NULL);       // must end with NULL
    if (st == MPK_FLOAT)
        printf("found data.voltage: %f\n", voltage);
    else
        printf("parse data.voltage error: %d\n", st);


    st = mpk_map_search(msg_buf, &payload, &end, NULL, NULL,
                        "dev", NULL);
    if (st == MPK_STR) {
        int str_len = end - payload;
        printf("found dev: %.*s\n", str_len, payload);  // print non-null terminated string
    } else {
        printf("parse dev error: %d\n", st);
    }


    st = mpk_map_search(msg_buf, &payload, NULL, NULL, &num,
                        "data", "angle", NULL);
    if (st == MPK_ARRAY) {
        uint8_t *pos = payload;
        printf("data.angle[] length: %d\n", num);
        for (int i = 0; i < num; i++) {
            int32_t val;                                // val must be at least 4 bytes
            st = mpk_parse(pos, NULL, &end, &val, NULL);
            if (st != MPK_INT && st != MPK_UINT) {
                printf("  parse data.angle error\n");
                break;
            }
            printf("  data.angle[%d]: %ld\n", i, val);
            pos = end;
        }
    } else {
        printf("parse data.angle error: %d\n", st);
    }
}
```

The header file `cd_utils.h` please refer to: https://github.com/dukelec/cdnet/blob/master/utils/cd_utils.h


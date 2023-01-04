MessagePackEL
====================================

This is the little-endian version of MessagePack, except the endianness is different,
the rest is exactly the same as MessagePack.



C library: [msgpackel_c](msgpackel_c)

Python library: [msgpackel_py](msgpackel_py) (base on u-msgpack-python)

Javascript library: [msgpackel_js](msgpackel_js) (base on ygoe/msgpack)


MIME type:
 - original msgpack: `application/msgpack`
 - msgpackel: `application/msgpackel`

Short file extension: (preferred)
 - original msgpack: `.mpk`
 - msgpackel: `.mpkl`

Full file extension: (optional)
 - original msgpack: `.msgpack`
 - msgpackel: `.msgpackel`


By convention, `el` is the initials of "little-endian" read in little-endian order.


Why create the little-endian version: https://github.com/msgpack/msgpack/issues/313


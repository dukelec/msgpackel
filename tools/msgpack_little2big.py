#!/usr/bin/env python3
# Software License Agreement (BSD License)
#
# Copyright (c) 2023, DUKELEC, Inc.
# All rights reserved.
#
# Author: Duke Fong <d@d-l.io>

import os, sys
import umsgpack
import pprint
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from msgpackel_py import umsgpackel

if len(sys.argv) < 2:
    print("Usage: ./msgpack_little2big.py input.mpkl output.mpk")
    exit(-1)

with open(sys.argv[1], 'rb') as f:
    in_file = f.read()

in_data = umsgpackel.unpackb(in_file)
pprint.pp(in_data)
out_data = umsgpack.packb(in_data)

if len(sys.argv) >= 3:
    with open(sys.argv[2], 'wb') as f:
        f.write(out_data)


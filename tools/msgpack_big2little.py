#!/usr/bin/env python3
# Software License Agreement (BSD License)
#
# Copyright (c) 2023, DUKELEC, Inc.
# All rights reserved.
#
# Author: Duke Fong <d@d-l.io>

import os, sys
import umsgpack
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from msgpackel_py import umsgpackel

if len(sys.argv) != 3:
    print("Usage: ./msgpack_big2little.py input_file output_file")
    exit(-1)

with open(sys.argv[1], 'rb') as f:
    in_file = f.read()

in_data = umsgpack.unpackb(in_file)
out_data = umsgpackel.packb(in_data)

with open(sys.argv[2], 'wb') as f:
    f.write(out_data)


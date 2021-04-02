#!/usr/bin/env python3

'''
Original script by: https://gist.github.com/zerog2k
'''

import usb
import hashlib
import time

printer = usb.core.find(idVendor=0x0315)
CONTROL_EP = 0x01
FILE_EP = 0x03
#uncoment to revert back to original firmware
#FILENAME = "dreamerNX_1.9.20191123.bin"
FILENAME = "marlin.bin"
TARGET_FILENAME = "firmware.bin"

if printer is None:
    raise ValueError('device not found')

m = hashlib.md5()

fw = open(FILENAME, 'rb')
m.update(fw.read())
md5sum = m.hexdigest()
fwsize = fw.tell()
fw.seek(0)

printer.set_configuration()

# start control
printer.write(CONTROL_EP, '~M601 S0\r\n')
ret = printer.read(0x81, 500)
print(ret.tobytes())

# start fw write
fw_write_str = "~M28 {} 0:/sys/{}\r\n".format(fwsize, TARGET_FILENAME)
printer.write(CONTROL_EP, fw_write_str)
ret = printer.read(0x81, 500)
print(ret.tobytes())
ret = printer.read(0x81, 500)
print(ret.tobytes())

# write fw to endpoint
printer.write(FILE_EP, fw.read(), 5000) # seems like i was getting timeouts below about 1500ms

# finish fw write
fw_write_str = "~M29 {}\r\n".format(md5sum)
printer.write(CONTROL_EP, fw_write_str)
ret = printer.read(0x81, 500)
print(ret.tobytes())
ret = printer.read(0x81, 500)
print(ret.tobytes())

# trigger fw flash on next boot?
printer.write(CONTROL_EP, '~M600\r\n')
ret = printer.read(0x81, 500)
print(ret.tobytes())
ret = printer.read(0x81, 500)
print(ret.tobytes())

# stop control
printer.write(CONTROL_EP, '~M602\r\n')
ret = printer.read(0x81, 500)
print(ret.tobytes())
ret = printer.read(0x81, 500)
print(ret.tobytes())

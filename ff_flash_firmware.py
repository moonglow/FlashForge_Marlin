#!/usr/bin/env python3

'''
Original script by: https://gist.github.com/zerog2k

Usage:  ./ff_flash_firmware.py /path/to/firmware.bin
'''

import hashlib
import sys
from time import sleep
import usb


def to_string(byt):
  return ''.join([chr(x) for x in byt])


TARGET_FIRMWARE_NAME = "firmware.bin"
FLASHFORGE_VENDOR_ID = 0x0315
MAX_WAIT_TIME = 0.5  # 500 milliseconds
RETRY_COUNT = 20
CONTROL_ENDPOINT_ADDR = 0x01
BULK_OUT_ENDPOINT_ADDR = 0x03
BULK_IN_ENDPOINT_ADDR = 0x81  # by default, search for 'bEndpointAddress' in
# usb details if this is not the case

if len(sys.argv) < 1:
  raise ValueError('expecting firmware file: usage: ./ff_flash_firmware.py '
                   '/path/to/firmware.bin')

firmware = sys.argv[1]

# calculate checksum of the firmware
m = hashlib.md5()
fw = open(firmware, 'rb')
m.update(fw.read())
firmware_checksum = m.hexdigest()
firmware_size = fw.tell()
fw.seek(0)

print('Flashing firmware: ', firmware)
print('***********"')
print('Ready to Flash')
print('***********"')

input('Switch off your printer and connect to this computer via USB. When '
      'your are ready, press enter and switch on printer..')

print('Searching for Flashforge printers ...')

retry = 1
while True:

    if retry >= RETRY_COUNT:
        raise ValueError('Did not find Flashforge compatible printers, '
                         'try again?')

    printer = usb.core.find(idVendor=FLASHFORGE_VENDOR_ID)

    if printer is None:
        retry = retry+1
    else:
        print('Found printer:\n\n', printer)
        break

    sleep(MAX_WAIT_TIME)

# setup the configurations before the communication
printer.set_configuration()


print('Initiating control loop...')
# start control
printer.write(CONTROL_ENDPOINT_ADDR, '~M601 S0\r\n')

# You need to restart the control loop, this is weird but this seems to work
print('Starting control loop...')
sleep(5)
printer.write(CONTROL_ENDPOINT_ADDR, '~M601 S0\r\n')

ret = printer.read(BULK_IN_ENDPOINT_ADDR, 5000)
print(to_string(ret.tobytes()))

print('Writing firmware...')
# start fw write
fw_write_str = "~M28 {} 0:/sys/{}\r\n".format(firmware_size, TARGET_FIRMWARE_NAME)
printer.write(CONTROL_ENDPOINT_ADDR, fw_write_str)
ret = printer.read(BULK_IN_ENDPOINT_ADDR, 1000)
print(to_string(ret.tobytes()))
ret = printer.read(BULK_IN_ENDPOINT_ADDR, 1000)
print(to_string(ret.tobytes()))

# write fw to endpoint
printer.write(BULK_OUT_ENDPOINT_ADDR, fw.read(),
              5000)  # seems like i was getting timeouts below about 1500ms

# finish fw write
fw_write_str = "~M29 {}\r\n".format(firmware_checksum)
printer.write(CONTROL_ENDPOINT_ADDR, fw_write_str)
ret = printer.read(BULK_IN_ENDPOINT_ADDR, 1000)
print(to_string(ret.tobytes()))
ret = printer.read(BULK_IN_ENDPOINT_ADDR, 1000)
print(to_string(ret.tobytes()))


print('Triggering firmware...')
# trigger fw flash on next boot?
printer.write(CONTROL_ENDPOINT_ADDR, '~M600\r\n')
ret = printer.read(BULK_IN_ENDPOINT_ADDR, 1000)
print(to_string(ret.tobytes()))
ret = printer.read(BULK_IN_ENDPOINT_ADDR, 1000)
print(to_string(ret.tobytes()))

print('Ending control loop...')
# stop control
printer.write(CONTROL_ENDPOINT_ADDR, '~M602\r\n')
ret = printer.read(BULK_IN_ENDPOINT_ADDR, 1000)
print(to_string(ret.tobytes()))
ret = printer.read(BULK_IN_ENDPOINT_ADDR, 1000)
print(to_string(ret.tobytes()))

print('Flashing Completed, your printer will now reboot to the new firmware!')

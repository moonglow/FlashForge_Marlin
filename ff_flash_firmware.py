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


""" 
Send command to printer and wait for a reply until we either get 'ok' or timeout

Parameters:
  command : string containing command M/G g-code command
  expectreply : set True to generate exception if printer does not respond
Returns:
  True if the printer responded
"""
def send_command(command, expectreply=True):
  command = '~{}\r\n'.format(command)
  print('>>>' + command.strip())
  printer.write(CONTROL_ENDPOINT_ADDR, command)
  # now get response
  timeout = 1000  # wait up to 1 seconds for initial response
  response = ''
  while True:
    try:
      ret = printer.read(BULK_IN_ENDPOINT_ADDR, timeout).tobytes()
      timeout = 500 # subsequent responses should be quicker
      response = to_string(ret)
      print('<<<' + response.strip().replace('\r', '').replace('\n', '\n<<<'))
      if any(x in response.lower() for x in ['error','failed']):
        raise Exception('printer returned error')
      if 'ok\r\n' in response:
        # printer finished responding
        print()
        return True
    except usb.core.USBTimeoutError:
      # no more response - we can get here after no response or getting a partial response 
      # and then timeout waiting for an 'ok'
      if expectreply:
        raise Exception('printer did not respond')
      print()
      return False


TARGET_FIRMWARE_NAME = 'firmware.bin'
FLASHFORGE_VENDOR_IDS = [0x2a89,    # Dremel
                         0x2b71,    # FlashForge
                         0x0315     # FlashForge bootloader
                        ]
MAX_WAIT_TIME = 0.5  # 500 milliseconds
RETRY_COUNT = 20
CONTROL_ENDPOINT_ADDR = 0x01
BULK_OUT_ENDPOINT_ADDR = 0x03
BULK_IN_ENDPOINT_ADDR = 0x81  # by default, search for 'bEndpointAddress' in
# usb details if this is not the case

if len(sys.argv) != 2:
  # (first argument is this script name)
  print('Expecting firmware file - usage: ./ff_flash_firmware.py '
        '/path/to/firmware.bin')
  print()
  exit()

firmware = sys.argv[1]

# calculate checksum of the firmware
m = hashlib.md5()
try:
  fw = open(firmware, 'rb')
  m.update(fw.read())
  firmware_checksum = m.hexdigest()
  firmware_size = fw.tell()
  fw.seek(0)
except:
  print('Error opening firmware file: ', firmware)
  print()
  exit()

print('Flashing firmware: ', firmware)
print('Searching for Flashforge printers ...')

printer = None
retry = 1
while not printer:

  if retry >= RETRY_COUNT:
    print('Did not find any Flashforge compatible printers, check your connections, '
          'reboot the printer and try again.')
    print()
    exit()

  for FLASHFORGE_VENDOR_ID in FLASHFORGE_VENDOR_IDS:
    printer = usb.core.find(idVendor=FLASHFORGE_VENDOR_ID)
    if printer:
      print('Found printer:\n\n', printer)
      print()
      break

  retry = retry + 1
  sleep(MAX_WAIT_TIME)

try:
  # setup the configurations before the communication
  printer.set_configuration()

  # start control
  retry = 7
  print('Initiating control loop...')
  while True:
    retry -= 1
    # try up to 'retry' times to connect, on last attempt will error if no response
    if send_command('M601 S0', retry == 0):
      break
  
  # start fw write
  print('Writing firmware...')
  send_command('M28 {} 0:/sys/{}'.format(firmware_size, TARGET_FIRMWARE_NAME))

  # write fw to endpoint
  printer.write(BULK_OUT_ENDPOINT_ADDR, fw.read(),
          5000)  # seems like i was getting timeouts below about 1500ms

  # finish fw write
  print('Finished writing, sending file end comand...')
  send_command('M29 {}'.format(firmware_checksum))

  # trigger fw flash on next boot?
  print('Triggering firmware...')
  send_command('M600')

  # stop control
  print('Ending control loop...')
  send_command('M602', False)

  print('Disconnected from printer, Firmware upload complete.')
  print('You may need to restart your printer to complete the firmware update.')

except usb.core.USBError as error:
  print('Firmware update failed, USB error:', error)
except Exception as error:
  print('Firmware update failed, error:', error)

print()

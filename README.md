#  Marlin 2.x Firmware for Flashforge 3D Printers

Current firmware is based on 2.0.7.2  

#### What's works?

- [x] TFT display ( ILI9488 and OTM4802 )
- [x] Touch screen controller ( XPT2046 )
- [x] Chamber and bed temperature sensors ( internal ADC )
- [x] Extruder K-type thermocouple with external ADC ( ADS1118 )
- [x] Chamber RGB light ( PCA9632 PWM controller )
- [x] Stepper motor current setup ( MCP4018 digital potentiometer )
- [x] Print cooling fan, chamber fan, endstops, stepper motor signals control
- [x] External SD card
- [x] USB ( virtual serial port )

#### What doesn't work?
- [ ] WiFi does not work  (see [comment](https://github.com/moonglow/FlashForge_Marlin/issues/3#issuecomment-813024193))
    - Marlin does not support FF stock wifi chip.  
- [ ] Internal Storage not available  (see [comment](https://github.com/moonglow/FlashForge_Marlin/issues/3#issuecomment-813024193))
    - This is disabled to enable restoring to FF stock firmware
- [ ] Power-loss recovery not available for Dreamer & Dreamer Nx  (see [Issue#5](https://github.com/moonglow/FlashForge_Marlin/issues/5))  



#### How to flash
- The releases page contain the pre-compiled  firmware binaries `marlin_*`, these are ready to be flashed to the printer. 
  The suffixes indicate the printer to which it belongs to - 

There's already a handy script which downloads and flashes to the printer automatically

Flashing:

```bash
$ ./install_marlin.sh <printer> <version>

...


# this will download and install the v0.7 firmware for dreamer nx.
# follow the instructions pointed in the script
$ ./install_marlin.sh dreamer_nx 0.7

```

For flashing a specific firmware, you can directly use the python script.
As this can flash any firmware supported by the printer, you can use this to restore the stock firmware. 

```bash
$ ./ff_flash_firmware.py /path/to/firmware.bin
```


#### Supported printers:

- DreamerNX (dreamer_nx)
- Dreamer (dreamer)
- Inventor (inventor)

#### Support me
<a href="https://www.buymeacoffee.com/moonglow" target="_blank"><img src="https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png" alt="Buy Me A Coffee" height="41" width="174"></a>


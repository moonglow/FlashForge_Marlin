#  Marlin 2.x Firmware for Flashforge 3D Printers

![GitHub Workflow Status](https://img.shields.io/github/workflow/status/tckb/FlashForge_Marlin/build-release)

Current firmware is based on 2.0.7.2  


#### What works?

- [x] TFT display ( ILI9488 and OTM4802 )
- [x] Touch screen controller ( XPT2046 )
- [x] Chamber and bed temperature sensors ( internal ADC )
- [x] Extruder K-type thermocouple with external ADC ( ADS1118 )
- [x] Chamber RGB light ( PCA9632 PWM controller )
- [x] Stepper motor current setup ( MCP4018 digital potentiometer )
- [x] Print cooling fan, chamber fan, endstops, stepper motor signals control
- [x] External SD card
- [x] USB ( virtual serial port )
- [ ] Internal storage ( read [discussion](https://github.com/moonglow/FlashForge_Marlin/issues/3#issuecomment-813024193))
- [ ] Power-loss recovery  (read [discussion](https://github.com/moonglow/FlashForge_Marlin/issues/5))


#### Unsupported

- [ ] WiFi  (read [discussion](https://github.com/moonglow/FlashForge_Marlin/issues/3#issuecomment-813024193))


Head on to the [wiki page](https://github.com/tckb/FlashForge_Marlin/wiki) to find more details on flashing the firmware, building from source etc. 


## Supported platforms:
 
The scripts are written to work well with the following platforms -

 - macOS
 - GNU/Linux


## Supported printers:

- DreamerNX
- Dreamer (Untested)
- Inventor (Untested)

## Credits

- This repo is a fork of the excellent effort made by  [@moonglow](https://github.com/moonglow), consider supporting his work.  


#### Disclaimer
The firmware is still in an experimental state and even though it could  work, please exercise your own judgement on using it. I am not liable, nor I bare responsibility in the event of a failure. 

#pragma once

#define ALLOW_STM32DUINO
#include "env_validate.h"

#if HOTENDS > 2 || E_STEPPERS > 2
  #error "STM32F4 supports up to 2 hotends / E-steppers."
#endif

#define BOARD_INFO_NAME      "FlashForge MB CORE"
#define DEFAULT_MACHINE_NAME "FF PRINTER"

/*
notes:
internal SD - SDIO interface
external SD - SPI2 CD(PA15), CS(PG5)

NTC1
 __________
| 1 2 3 4  |
``|_____|```
1 - VDDA
2 - GND
3 - GND
4 - PC5

NTC2
 _   _
| |_| |
|1   2|
```````
1 - GND
2 - ( pull-up 4.7K to VDDA ) PC4

---------------
EX1 right extruder heater + fan
EX2 left extruder heater + fan
*/

/* 
thermosensor K-couple ADS1118 

*/
#define USER_LED1_PIN   PA8
#define USER_LED2_PIN   PG7

/* WARNING: NEW motherboard */
#define EDETECT_PIN         PG8    /* OLD: WIFI_UART6_RTS */
#define CAMERA_PWR_ON_PIN   PF15
/* ........................ */

//#define I2C_EEPROM
//#define SRAM_EEPROM_EMULATION
#define SDCARD_EEPROM_EMULATION

#define DIGIPOTS_I2C_SCL                    PF12    /* shared CLK */
#define DIGIPOTS_I2C_SDA_X                  PB5     /* X */
#define DIGIPOTS_I2C_SDA_Y                  PB9     /* Y */
#define DIGIPOTS_I2C_SDA_Z                  PF0     /* Z */  
#define DIGIPOTS_I2C_SDA_E0                 PA0     /* A */
#define DIGIPOTS_I2C_SDA_E1                 PF10    /* B */


#define PCA9632_SOFT_I2C
//#define PCA9632_RGBW
#define PCA9632_RED      0x00
#define PCA9632_GRN      0x04
#define PCA9632_BLU      0x02
#define PCA9632_ADDRESS  0b01100010 /* 7bit format, 8bit => 0xC4 */
#define PCA9632_I2C_SCK                     PF12
#define PCA9632_I2C_SDA                     PB1

/* SOFTWARE I2C */
#if 0
/* I2C eeprom ( 0xA0 ) and PCA9632 ( 0xC4 ) RGB PWM IC */
#define I2C_SW_SDA                          PB1
#define I2C_SW_SCK                          PF12                  
#endif

#define LED_PIN                             PG1 /* keep alive led */

#define BEEPER_PIN                          PB0 /* ( TIM3/CH3) */

/* XPT2046 */
#define TOUCH_INT_PIN                       PG10 /* optional ? */
#define TOUCH_CS_PIN                        PG9
/* SPI1 */
#define TOUCH_SCK_PIN                       PA5
#define TOUCH_MISO_PIN                      PA6
#define TOUCH_MOSI_PIN                      PA7


/* FSMC */
#define TFT_CS_PIN                        PD7  /* NE1 */
#define TFT_RS_PIN                        PD11 /* RS */
#define TFT_BACKLIGHT_PIN                 PG11
/* PIN is OK, but removed for test purpose */
#define TFT_RESET_PIN                     PD13

#if ENABLED( TFT_CLASSIC_UI )
  #define FSMC_CS_PIN                       PD7   /* NE1 */
  #define FSMC_RS_PIN                       PD11  /* RS -> A0 */
#endif

// Ignore temp readings during development.
//#define BOGUS_TEMPERATURE_GRACE_PERIOD    2000

//
// Limit Switches
//
#define X_MIN_PIN                           PE2
#define X_MAX_PIN                           PF3
#define Y_MIN_PIN                           PE3
#define Y_MAX_PIN                           PF2
#define Z_MIN_PIN                           PE4
#define Z_MAX_PIN                           PF1


#ifndef Z_MIN_PROBE_PIN
  #define Z_MIN_PROBE_PIN  Z_MIN_PIN
#endif

/* Steppers */
#define X_STEP_PIN                          PB4
#define X_DIR_PIN                           PB3
#define X_ENABLE_PIN                        PB6

#define Y_STEP_PIN                          PB8
#define Y_DIR_PIN                           PB7
#define Y_ENABLE_PIN                        PE0

#define Z_STEP_PIN                          PE6
#define Z_DIR_PIN                           PE5
#define Z_ENABLE_PIN                        PF4
/* --------------- */
#if ENABLED( FF_EXTRUDER_SWAP )
/* swap it for beter visual observ */
#define E0_STEP_PIN                         PF8 
#define E0_DIR_PIN                          PF7 
#define E0_ENABLE_PIN                       PF9 

#define E1_STEP_PIN                         PC2
#define E1_DIR_PIN                          PC1
#define E1_ENABLE_PIN                       PC3
#else
#define E0_STEP_PIN                         PC2
#define E0_DIR_PIN                          PC1
#define E0_ENABLE_PIN                       PC3

#define E1_STEP_PIN                         PF8
#define E1_DIR_PIN                          PF7
#define E1_ENABLE_PIN                       PF9
#endif


/* Temperature Sensors */

#define TEMP_0_PIN                          -1
#define TEMP_1_PIN                          -1
#define TEMP_BED_PIN                        PC5 /* NTC1 */
#define TEMP_CHAMBER_PIN                    PC4 /* NTC2 */

/* ADS1118 */
#define HAS_SPI_ADS1118                     1
#define TEMP_0_CS_PIN                       PA4
#define TEMP_1_CS_PIN                       TEMP_0_CS_PIN
#define ADS1118_CS_PIN                      TEMP_0_CS_PIN

/* SPI1 */
#define ADS1118_SCK_PIN                     PA5
#define ADS1118_MISO_PIN                    PA6
#define ADS1118_MOSI_PIN                    PA7

/*  Heaters / Fans */
#if ENABLED( FF_EXTRUDER_SWAP )
#define E0_AUTO_FAN_PIN                     PF6  /* Extruder FAN (EX2)*/
#define E1_AUTO_FAN_PIN                     PC0  /* Extruder FAN (EX1)*/
#define HEATER_0_PIN                        PA2  /* Heater EX2 */
#define HEATER_1_PIN                        PA3  /* Heater EX1 */
#else
#define E0_AUTO_FAN_PIN                     PC0  /* Extruder FAN (EX1)*/
#define E1_AUTO_FAN_PIN                     PF6  /* Extruder FAN (EX2)*/
#define HEATER_0_PIN                        PA3  /* Heater EX1 */
#define HEATER_1_PIN                        PA2  /* Heater EX2 */
#endif
#define HEATER_BED_PIN                      PA1  /* Heat Board */

#define HEATER_CHAMBER_PIN                  USER_LED1_PIN
/* from 2.0.9.3 */
#define FAN2_PIN                            PE1 /* FF rearcase fan */
#define CHAMBER_FAN_INDEX                   2

/* part colling fan aka FAN0 */
#define FAN_PIN                             PF5
//#define FAN1_PIN                            CHAMBER_AUTO_FAN_PIN
#define FAN_SOFT_PWM

/* servo */
#define SERVO0_PIN                          -1
#define PS_ON_PIN                           -1
#define KILL_PIN                            -1
#define PWR_LOSS                            -1   //Power loss / nAC_FAULT

/* external SD CARD */
#define SDCARD_COMMANDS_SPLIT // enable Toshiba FlashAIR support
#define SD_DETECT_PIN                       PA15
#define SDSS                                PG5

#define SS_PIN                              -1
#define CUSTOM_SPI_PINS
#ifdef CUSTOM_SPI_PINS
/* SPI2 */
#define SCK_PIN                             PB13
#define MISO_PIN                            PB14
#define MOSI_PIN                            PB15
#endif

/* filament runout */
#if ENABLED( FF_INVENTOR_MACHINE )
#define FIL_RUNOUT_PIN                      PE4 /* Z_MIN_PIN */
#else
#define FIL_RUNOUT_PIN                      PF1 /* Z_MAX_PIN */
#endif


/* Servo pin for BLTouch */
#define SERVO0_PIN                         -1

/* external SPI(2) FLASH mod  ( MKS UI ONLY ) */
#if ENABLED( USE_MKS_UI )
#define HAS_SPI_FLASH_FONT                   1
#define HAS_GCODE_PREVIEW                    1
#define HAS_GCODE_DEFAULT_VIEW_IN_FLASH      1
#define HAS_LANG_SELECT_SCREEN               1
#define HAS_BAK_VIEW_IN_FLASH                1
#define HAS_LOGO_IN_FLASH                    1

#define HAS_SPI_FLASH     1
#define SPI_FLASH_SIZE    0x1000000
#define W25QXX_MISO_PIN   MISO_PIN
#define W25QXX_MOSI_PIN   MOSI_PIN
#define W25QXX_SCK_PIN    SCK_PIN
#define W25QXX_CS_PIN     PB12

#define SPI_FLASH_MISO_PIN  MISO_PIN
#define SPI_FLASH_MOSI_PIN  MOSI_PIN
#define SPI_FLASH_SCK_PIN   SCK_PIN
#define SPI_FLASH_CS_PIN    PB12
#endif

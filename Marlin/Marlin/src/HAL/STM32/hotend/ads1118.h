#pragma once

#ifdef STM32F1xx
  #include <stm32f1xx_hal.h>
#elif defined(STM32F4xx)
  #include <stm32f4xx_hal.h>
#endif

#include "../../../inc/MarlinConfig.h"

void ads1118_init( void );
int ads1118_read_raw( int ch_id );

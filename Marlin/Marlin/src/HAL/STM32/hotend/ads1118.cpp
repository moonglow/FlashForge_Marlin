#include "../../../inc/MarlinConfig.h"

/* TODO: just for my needs :( need to rework for Marlin official repo */
/* based on xpt2046 driver */

#if HAS_SPI_ADS1118
#include "ads1118.h"
#include "thermocouple.h"
#include "pinconfig.h"

static SPI_HandleTypeDef ads1118_spi = { 0 };
/* someone use DMA for transfer ? check if no active DMA transaction */
static DMA_HandleTypeDef ads1118_dma_tx = { 0 };

#ifdef STM32F1xx
  #define __IS_DMA_ENABLED(__HANDLE__)      ((__HANDLE__)->Instance->CCR & DMA_CCR_EN)
#elif defined(STM32F4xx)
  #define __IS_DMA_ENABLED(__HANDLE__)      ((__HANDLE__)->Instance->CR & DMA_SxCR_EN)
#endif

static bool spi_is_busy( void )
{
  return __IS_DMA_ENABLED( &ads1118_dma_tx );
}

static inline void spi_transfer_begin( void )
{
  HAL_SPI_Init( &ads1118_spi ); 

  WRITE( ADS1118_CS_PIN, LOW );
};

static inline void spi_transfer_end( void )
{
  WRITE( ADS1118_CS_PIN, HIGH );
};

static uint16_t spi_transfer_io( uint16_t data )
{
  __HAL_SPI_ENABLE( &ads1118_spi );
  while ((ads1118_spi.Instance->SR & SPI_FLAG_TXE) != SPI_FLAG_TXE);
  ads1118_spi.Instance->DR = data;
  while ((ads1118_spi.Instance->SR & SPI_FLAG_RXNE) != SPI_FLAG_RXNE);
  __HAL_SPI_DISABLE( &ads1118_spi );

  return ads1118_spi.Instance->DR;
}

#define ENABLE_PULL_UP      (1<<3)
#define ENABLE_SINGLE_SHOT  (1<<15)

/* 64 SPS = ~ 16ms, ±0.256V */
#define CHANNEL_1_CFG   ( 0x0C62 | ENABLE_PULL_UP )
#define CHANNEL_2_CFG   ( 0x3C62 | ENABLE_PULL_UP )
#define INTERNAL_T      ( 0x0072 | ENABLE_PULL_UP )

#define MAX_CHANNELS    (2)
#define USE_POLLING     0

#define SPI_DATA_SIZE SPI_DATASIZE_16BIT

void ads1118_init( void )
{
  static char was_init = 0;
    
  if( was_init )
    return;

  SPI_TypeDef *spiInstance;

  OUT_WRITE( ADS1118_CS_PIN, HIGH );

  spiInstance = (SPI_TypeDef *)pinmap_peripheral(digitalPinToPinName(ADS1118_SCK_PIN),  PinMap_SPI_SCLK );
  if( spiInstance != (SPI_TypeDef *)pinmap_peripheral(digitalPinToPinName(ADS1118_MOSI_PIN), PinMap_SPI_MOSI ) )
  {
    spiInstance = NP;
  }
  if( spiInstance != (SPI_TypeDef *)pinmap_peripheral(digitalPinToPinName(ADS1118_MISO_PIN), PinMap_SPI_MISO) )
  {
    spiInstance = NP;
  }

  if( spiInstance == NP)
    return;

  ads1118_spi.Instance = spiInstance;

  ads1118_spi.State                   = HAL_SPI_STATE_RESET;
  ads1118_spi.Init.NSS                = SPI_NSS_SOFT;
  ads1118_spi.Init.Mode               = SPI_MODE_MASTER;
  ads1118_spi.Init.Direction          = SPI_DIRECTION_2LINES;
  ads1118_spi.Init.BaudRatePrescaler  = SPI_BAUDRATEPRESCALER_64;
  ads1118_spi.Init.CLKPhase           = SPI_PHASE_2EDGE;
  ads1118_spi.Init.CLKPolarity        = SPI_POLARITY_LOW;
  ads1118_spi.Init.DataSize           = SPI_DATA_SIZE;
  ads1118_spi.Init.FirstBit           = SPI_FIRSTBIT_MSB;
  ads1118_spi.Init.TIMode             = SPI_TIMODE_DISABLE;
  ads1118_spi.Init.CRCCalculation     = SPI_CRCCALCULATION_DISABLE;
  ads1118_spi.Init.CRCPolynomial      = 10;

  pinmap_pinout(digitalPinToPinName(ADS1118_SCK_PIN), PinMap_SPI_SCLK);
  pinmap_pinout(digitalPinToPinName(ADS1118_MOSI_PIN), PinMap_SPI_MOSI);
  pinmap_pinout(digitalPinToPinName(ADS1118_MISO_PIN), PinMap_SPI_MISO);

  #ifdef SPI1_BASE
    if( ads1118_spi.Instance == SPI1 )
    {
      __HAL_RCC_SPI1_CLK_ENABLE();
      ads1118_spi.Init.BaudRatePrescaler  = SPI_BAUDRATEPRESCALER_64;
      #ifdef STM32F1xx
        ads1118_dma_tx.Instance = DMA1_Channel3;
      #elif defined(STM32F4xx)
        ads1118_dma_tx.Instance = DMA2_Stream3;
      #endif
    }
  #else
    #error o_O
  #endif


}

static int ads1118_read_adc( uint16_t config, int eoc )
{
  uint16_t result;
 
  spi_transfer_begin();

  if( eoc )
  {
    uint8_t pattern = 0;
    /* debounce a little... for end of conversion signal */
    pattern |= ( READ( ADS1118_MISO_PIN ) == LOW ) ? (1<<0):0;
    pattern |= ( READ( ADS1118_MISO_PIN ) == LOW ) ? (1<<1):0;
    pattern |= ( READ( ADS1118_MISO_PIN ) == LOW ) ? (1<<2):0;
    pattern |= ( READ( ADS1118_MISO_PIN ) == LOW ) ? (1<<3):0;
    if( pattern != 0x0F )
    {
      spi_transfer_end();
      return -1;
    }
  }

#if SPI_DATA_SIZE == SPI_DATASIZE_8BIT
  result = spi_transfer_io( config>>8 );
	result = ( result << 8 ) | spi_transfer_io( config&0xFF );
  (void)spi_transfer_io( 0xFF );
  (void)spi_transfer_io( 0xFF );
#else
  result = spi_transfer_io( config );
  (void)spi_transfer_io( config );
#endif
  spi_transfer_end();

  return result;
}

static int ads1118_it_to_c( uint16_t raw )
{
  /* 14-bit result that is left-justified */
  raw >>= 2;
  /* negative unsupported */
  if( raw & 0x2000 )
    return 25000;
  /* LSB: 0.03125 */
  return ( raw*3125 ) / 100;
}

/* FSR: ±0.256V, LSB: 7.8125μV */
static int ads1118_adc_to_uv( uint16_t raw )
{
  int current_uv;
  /* negative unsupported */
  if( raw & 0x8000 )
    return 0;
  /* (raw*256000)/32768 */
  current_uv = (raw*1000)>>7;
  /* open thermocouple ? 41276uV => 1000℃ */
  if( current_uv >= 41276 )
    return 0; /* show ambient only */
  return current_uv;
}

int ads1118_read_raw( int ch_id )
{
  static int16_t shadow_data[MAX_CHANNELS] = { 10, 10 };
  static int read_fsm = 0;
  static int raw_it = 0, raw_ch = 0;
#if ( USE_POLLING == 0 )
  static millis_t delay_ms = 0;
#endif
  int res;

  #if ENABLED( FF_EXTRUDER_SWAP )
    ch_id ^= 1;
  #endif

  if( !ads1118_spi.Instance )
    return shadow_data[ch_id];

  if( spi_is_busy() )
    return shadow_data[ch_id];

#if ( USE_POLLING == 0 )
  if( (millis()-delay_ms) < 16u )
    return shadow_data[ch_id];
#endif

  switch( read_fsm )
  {
    default:
      read_fsm = 0;
    break;
    case 0:
      (void)ads1118_read_adc( INTERNAL_T | ENABLE_SINGLE_SHOT, 0 );
      read_fsm = 1;
      break;
    case 1:
      res = ads1118_read_adc( CHANNEL_1_CFG | ENABLE_SINGLE_SHOT, USE_POLLING );
      if( res < 0 )
        break;
      raw_it = ads1118_it_to_c( res & 0xFFFF );
      read_fsm = 2;
      break;
    case 2:
      res = ads1118_read_adc( INTERNAL_T | ENABLE_SINGLE_SHOT, USE_POLLING );
      if( res < 0 )
        break;
      raw_ch = ads1118_adc_to_uv( res & 0xFFFF );
      shadow_data[0] = thermocoupleConvertWithCJCompensation( raw_ch, raw_it ) / 100; /* in 0.1C */
      read_fsm = 3;
      break;
    case 3:
      res = ads1118_read_adc( CHANNEL_2_CFG | ENABLE_SINGLE_SHOT, USE_POLLING );
      if( res < 0 )
        break;
      /* read internal temp */
      raw_it = ads1118_it_to_c( res & 0xFFFF );
      read_fsm = 4;
      break;
    case 4:
      res = ads1118_read_adc( INTERNAL_T | ENABLE_SINGLE_SHOT, USE_POLLING );
      if( res < 0 )
        break;
      raw_ch = ads1118_adc_to_uv( res & 0xFFFF );
      shadow_data[1] = thermocoupleConvertWithCJCompensation( raw_ch, raw_it ) / 100; /* in 0.1C */
      read_fsm = 1;
      break;
  }

#if ( USE_POLLING == 0 )
  /* delay to next readout */
  delay_ms = millis();
#endif
  return shadow_data[ch_id];
}

#endif
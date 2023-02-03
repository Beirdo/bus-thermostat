#ifndef __project_h_
#define __project_h_

#include <Arduino.h>

#define PIN_PWR_LED     PIN_PA4
#define PIN_I2C_LED     PIN_PA5
#define PIN_KLINE_LED   PIN_PA6
#define PIN_VBAT_DIV8   PIN_PA7
#define PIN_KLINE_RX    PIN_PB3
#define PIN_KLINE_TX    PIN_PB2
#define PIN_SDA         PIN_PB1
#define PIN_SCL         PIN_PB0
#define PIN_OSC_PRIME   PIN_PA1
#define PIN_RESET       PIN_PA2
#define PIN_KLINE_EN    PIN_PA3

// PCA9557 Pins
#define PIN_FAN_REQ     0
#define PIN_HEAT_REQ    1
#define PIN_COOL_REQ    2
#define PIN_EX_I2C_EN   6
#define PIN_EN_24VAC    7

#define I2C_ADDR_PCA9557  0x18

#define HI_BYTE(x)     ((uint8_t)(((int)(x) >> 8) & 0xFF))
#define LO_BYTE(x)     ((uint8_t)(((int)(x) & 0xFF)))

#define HI_NIBBLE(x)  ((uint8_t)(((int)(x) >> 4) & 0x0F))
#define LO_NIBBLE(x)  ((uint8_t)(((int)(x) & 0x0F)))


template <typename T>
inline T clamp(T value, T minval, T maxval)
{
  return max(min(value, maxval), minval);
}

template <typename T>
inline T map(T x, T in_min, T in_max, T out_min, T out_max)
{
  // the perfect map fonction, with constraining and float handling
  x = clamp<T>(x, in_min, in_max);
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


#endif

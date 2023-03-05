#ifndef __project_h_
#define __project_h_

#include <Arduino.h>
#include <Beirdo-Utilities.h>

#define PIN_UPDI	PIN_PA0
#define PIN_KLINE_EN    PIN_PA1
#define PIN_RESET       PIN_PA2
#define PIN_KLINE_LED   PIN_PA3
#define PIN_PWR_LED     PIN_PA4
#define PIN_I2C_LED     PIN_PA5
#define PIN_FAN_FAULT   PIN_PA6
#define PIN_VBAT_DIV8   PIN_PA7
#define PIN_SCL         PIN_PB0
#define PIN_SDA         PIN_PB1
#define PIN_KLINE_TX    PIN_PB2
#define PIN_KLINE_RX    PIN_PB3

// PCA9557 Pins
#define PIN_FAN_REQ     0
#define PIN_HEAT_REQ    1
#define PIN_COOL_REQ    2
#define PIN_TEMP_OVER	3
#define PIN_FAN_SHDN	5
#define PIN_EX_I2C_EN   6
#define PIN_EN_24VAC    7

#define I2C_ADDR_PCA9557  0x18

#endif

#include <Arduino.h>
#include <Wire.h>
#include <PCA9557.h>

#include "project.h"
#include "kline.h"


bool fan_request = false;
bool heat_request = false;
bool cool_request = false;

bool external_i2c_enable = false;

uint16_t vbat_mv;
bool undervoltage = false;
bool overvoltage = false;

PCA9557 io(I2C_ADDR_PCA9557, &Wire);

void reset_isr(void)
{
  void (* reboot)(void) = 0;
  reboot();
}

void setup() {
  pinMode(PIN_PWR_LED, OUTPUT);
  digitalWrite(PIN_PWR_LED, LOW);
  
  pinMode(PIN_I2C_LED, OUTPUT);
  digitalWrite(PIN_I2C_LED, HIGH);

  pinMode(PIN_KLINE_LED, OUTPUT);
  digitalWrite(PIN_KLINE_LED, HIGH);

  pinMode(PIN_VBAT_DIV8, INPUT);

  pinMode(PIN_OSC_PRIME, OUTPUT);
  digitalWrite(PIN_OSC_PRIME, LOW);

  pinMode(PIN_RESET, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_RESET), reset_isr, FALLING);

  init_kline();

  digitalWrite(PIN_I2C_LED, LOW);
  io.pinMode(PIN_FAN_REQ, INPUT);
  io.pinMode(PIN_HEAT_REQ, INPUT);
  io.pinMode(PIN_COOL_REQ, INPUT);
  
  io.pinMode(PIN_EX_I2C_EN, OUTPUT);
  io.digitalWrite(PIN_EX_I2C_EN, external_i2c_enable);

  io.pinMode(PIN_EN_24VAC, OUTPUT);
  io.digitalWrite(PIN_EN_24VAC, LOW);
  digitalWrite(PIN_I2C_LED, HIGH);

  // Prime the oscillator by giving it 60Hz for 4 cycles
  digitalWrite(PIN_OSC_PRIME, HIGH);
  delayMicroseconds(8333);
  digitalWrite(PIN_OSC_PRIME, LOW);
  delayMicroseconds(8333);

  digitalWrite(PIN_OSC_PRIME, HIGH);
  delayMicroseconds(8333);
  digitalWrite(PIN_OSC_PRIME, LOW);
  delayMicroseconds(8333);

  digitalWrite(PIN_OSC_PRIME, HIGH);
  delayMicroseconds(8333);
  digitalWrite(PIN_OSC_PRIME, LOW);
  delayMicroseconds(8333);

  digitalWrite(PIN_OSC_PRIME, HIGH);
  delayMicroseconds(8333);
  digitalWrite(PIN_OSC_PRIME, LOW);
  delayMicroseconds(8333);

  // give it a total 250ms to get the oscillator running
  delay(184);

  digitalWrite(PIN_I2C_LED, LOW);
  io.digitalWrite(PIN_EN_24VAC, HIGH);
  digitalWrite(PIN_I2C_LED, HIGH);

}

void loop() {
  int topOfLoop = millis();

  bool old_fan = fan_request;
  bool old_heat = heat_request;
  bool old_cool = cool_request;

  digitalWrite(PIN_I2C_LED, LOW);
  fan_request = io.digitalRead(PIN_FAN_REQ);
  heat_request = io.digitalRead(PIN_HEAT_REQ);
  cool_request = io.digitalRead(PIN_COOL_REQ);
  digitalWrite(PIN_I2C_LED, HIGH);

  int reading = analogRead(PIN_VBAT_DIV8);
  vbat_mv = map<int>(reading, 0, 4095, 0, 26394);

  if (vbat_mv > 16000) {
    if (!overvoltage) {
      overvoltage = true;
      undervoltage = false;
      fan_request = false;
      heat_request = false;
      cool_request = false;
      kline_shutdown();
    }
  } else if (vbat_mv < 9500) {
    if (!undervoltage) {
      undervoltage = true;
      overvoltage = false;
      fan_request = false;
      heat_request = false;
      cool_request = false;
      kline_shutdown();
    }
  } else {
    undervoltage = false;
    overvoltage = false;

    if (fan_request != old_fan) {
      kline_fan_request(fan_request);
    }

    if (heat_request != old_heat) {
      kline_heat_request(heat_request);
    }

    if (cool_request != old_cool) {
      kline_cool_request(cool_request);
    }
  }

  int elapsed = millis() - topOfLoop;
  int delayMs = clamp<int>(100 - elapsed, 1, 100);
  delay(delayMs);
}
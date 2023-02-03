#ifndef __kline_h_
#define __kline_h_

#include <Arduino.h>

void kline_shutdown(void);
void kline_fan_request(bool request);
void kline_heat_request(bool request);
void kline_cool_request(bool request);

void init_kline(void);
void process_kline(void);

extern bool kline_enable;

#endif
#include <Arduino.h>
#include <cppQueue.h>

#include "project.h"
#include "kline.h"

#define KLINE_BAUD_RATE   12500
#define KLINE_BUFFER_SIZE 16
#define KLINE_FIFO_SIZE   4
#define KLINE_TX_ADDR     0xF4
#define KLINE_RX_ADDR     0x4F

typedef struct {
  uint8_t command;
  uint8_t value;
} klineCommand_t;

enum {
  CMD_SHUTDOWN,
  CMD_FAN,
  CMD_HEAT,
  CMD_COOL,
};

bool kline_enable = false;
uint8_t rx_buffer[KLINE_BUFFER_SIZE];
uint8_t tx_buffer[KLINE_BUFFER_SIZE];
int rx_tail = 0;
cppQueue kline_tx_q(sizeof(klineCommand_t), KLINE_FIFO_SIZE, FIFO);

void kline_send_break(void);
uint8_t calc_kline_checksum(uint8_t *buf, int len);

void init_kline(void)
{
  kline_enable = false;
 
  pinMode(PIN_KLINE_EN, OUTPUT);
  digitalWrite(PIN_KLINE_EN, kline_enable);

  // KLine is on Serial.  // I forget the baudrate
  Serial.begin(KLINE_BAUD_RATE);
}

void kline_en_set(bool value)
{
  kline_enable = value;
  digitalWrite(PIN_KLINE_LED, !value);
  digitalWrite(PIN_KLINE_EN, kline_enable);
}

void kline_send_break(void)
{
  Serial.end();
  kline_en_set(true);
  digitalWrite(PIN_KLINE_TX, true);
  delay(50);
  digitalWrite(PIN_KLINE_TX, false);
  Serial.begin(KLINE_BAUD_RATE);
  delay(50);
}

uint8_t calc_kline_checksum(uint8_t *buf, int len)
{
  uint8_t checksum = 0x00;
  for (int i; i < len; i++) {
    checksum ^= buf[i];
  }
  return checksum;
}

void process_kline(void)
{
  if (!kline_enable && !kline_tx_q.isEmpty()) {
    klineCommand_t command;

    kline_tx_q.pop(&command);

    int len = -1;

    switch(command.command) {
      case CMD_SHUTDOWN:
        len = 4;
        break;
         
      case CMD_FAN:
      case CMD_HEAT:
      case CMD_COOL:
        len = 4;
        if (command.value) {
          len ++;
          command.command = CMD_SHUTDOWN;
        }
        break;

      default:
        break;
    }    
    
    if (len != -1) {
      int index = 0;
      
      tx_buffer[index++] = KLINE_TX_ADDR;
      tx_buffer[index++] = len - 2;
      
      uint8_t cmd;
      switch (command.command) {
        case CMD_SHUTDOWN:
          cmd = 0x10;
          break;

        case CMD_FAN:
          cmd = 0x22;
          break;

        case CMD_HEAT:
          cmd = 0x21;
          break;

        case CMD_COOL:
          cmd = 0x26;     // Custom to my controller, not in WBUS proper
          break;

        default:
          break;
      }
      
      tx_buffer[index++] = cmd;
      if (len == 5) {
        tx_buffer[index++] = 0;
      }
      tx_buffer[index++] = 0;
      tx_buffer[len - 1] = calc_kline_checksum(tx_buffer, len);

      kline_send_break();
      Serial.write(tx_buffer, len);      
    }
  } 

  while (Serial.available()) {
    uint8_t ch = Serial.read();

    if (!kline_enable) {
      continue;
    }

    if (rx_tail < KLINE_BUFFER_SIZE) {
      rx_buffer[rx_tail++] = ch;
    } 

    if (rx_tail > 2 && rx_tail == rx_buffer[2] + 2) {
      // that's the whole packet.  Check the checksum
      kline_en_set(false);

      int len = rx_tail;
      rx_tail = 0;

      if (calc_kline_checksum(rx_buffer, len)) {
        // bad checksum
        continue;
      }

      // OK, but we also don't give a darn about the response right now.
    }
  }
}

void kline_queue(uint8_t command, bool value)
{
  klineCommand_t item;

  item.command = command;
  item.value = value;

  kline_tx_q.push(&item);
}

void kline_shutdown(void)
{
  kline_queue(CMD_SHUTDOWN, true);
}

void kline_fan_request(bool request)
{
  kline_queue(CMD_FAN, request);
}

void kline_heat_request(bool request)
{
  kline_queue(CMD_HEAT, request);
}

void kline_cool_request(bool request)
{
  kline_queue(CMD_COOL, request);
}

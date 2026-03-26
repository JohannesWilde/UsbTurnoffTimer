/**
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file i2c.c I2C driver implementation.
 * @author Thomas Reidemeister
 */
#include "i2c.h"

static volatile char _busy;

void i2c_init() {
  SFRX_ON();
  I2CCFG = 0xe0;
  I2CMSST = 0x00;
  SFRX_OFF();
  _busy = 0;
}

void i2c_start() {
  _busy = 1;
  I2CMSCR = 0x81;
  while (_busy);
}

void i2c_recv_ack() {
  _busy = 1;
  I2CMSCR = 0x83;
  while (_busy);
}

char i2c_recv_data() {
  _busy = 1;
  I2CMSCR = 0x84;
  while (_busy);
  char data = I2CRXD;
  return data;
}

void i2c_send_ack() {
  I2CMSST = 0x00;
  _busy = 1;
  I2CMSCR = 0x85;
  while (_busy);
}

void i2c_send_nak() {
  I2CMSST = 0x01;
  _busy = 1;
  I2CMSCR = 0x85;
  while (_busy);
}

void i2c_stop() {
  _busy = 1;
  I2CMSCR = 0x86;
  while (_busy);
}

void i2c_send(char dat) {
  I2CTXD = dat;
  _busy = 1;
  I2CMSCR = 0x82;
  while (_busy);
}

void i2c_isr() __interrupt(I2C_VECTOR) {
  int old_pws2 = PSW2;
  SFRX_ON();
  if (I2CMSST & 0x40) {
    I2CMSST &= ~0x40;
    _busy = 0;
  }
  PSW2 = old_pws2;
}
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
 * @file hello.c Hello World project for the STC8H1K08 chip.
 * @author Thomas Reidemeister
 */
#include <stdint.h>
#include <stdio.h>
#include "hardware.h"
#include "usart.h"
#include "i2c.h"

static void delay(unsigned int t);

void main() {
  hardware_init();
  printf("Hello World!\r\n");
  // Chip ID
  for(int i = 0; i < 7; i++) {
    printf_tiny("CID: %x = %x\r\n", i, CHIPID00(SERIES_K08 + i));
  }
  printf_tiny("IRC: %x\r\n", IRCBAND);
  printf_tiny("CLD: %x\r\n", CLKDIV);

  // FIXME: I2C
  printf_tiny("before i2c\r\n");
  SFRX_ON();
  i2c_start();
  printf_tiny("after i2c start\r\n");
  i2c_send(0xa6);
  printf_tiny("send address\r\n");
  i2c_recv_ack();
  printf_tiny("after recv ack\r\n");
  SFRX_OFF();

  P1_0 = 0;
  WKTCL = 0xFE; // Set the power-down wake-up clock to be about 10 seconds
  WKTCH = 0x87;

  while (1) {
    PCON |= 0x02;  // Enter power-down mode
    P1_0 = (P1_0)?0:1;  // Toggle P1.0
    P1_1 = (P1_1)?0:1;  // Toggle P1.1
  }
}

static void delay(unsigned int t) {
  while (t--) {
    volatile int i = 0xFFF;
    while (i--);
  }
}


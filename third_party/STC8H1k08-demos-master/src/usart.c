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
 * @file usart.c USART Driver Implementation.
 * @author Thomas Reidemeister
 */
#include "hardware.h"
#include "usart.h"

static volatile char _busy;
char wptr;
char rptr;
char buffer[16];

void usart0_init() {
  SCON = CONF_SCON0;
  TMOD = 0x00;
  TL1 = BRT & 0xFF;
  TH1 = BRT >> 8;
  TR1 = 1;
  wptr = 0x00;
  rptr = 0x00;
  _busy = 0;
  ES = 1;
}

void usart0_send(char dat) {
  while(_busy);
  _busy = 1;
  SBUF = dat;
}

void usart0_send_str(char *dat) {
  while(*dat) {
    usart0_send(*dat++);
  }
}

// USART0 Interrupt service routine
void usart0_isr() __interrupt (SI0_VECTOR) {
  if (TI) {
    TI = 0;
    _busy = 0;
  }

  if (RI) {
    RI = 0;
    buffer[wptr++] = SBUF;
    wptr &= 0x0f;
  }
}

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
 * @file hardware.c Common Routines for Hardware Configuration.
 * @author Thomas Reidemeister
 */
#include <stdint.h>
#include "hardware.h"
#include "usart.h"
#include "i2c.h"

void hardware_init(void) {
  // Pin configuration
  P1M0 = CONF_PM0M0;
  P1M1 = CONF_PM0M1;

  P3M0 = CONF_PM3M0; // Quasi Bidirectional
  P3M1 = CONF_PM3M1;

  P5M0 = CONF_PM5M0; // Quasi Bidirectional
  P5M1 = CONF_PM5M1;

  // Defined output + Quasibidirectional = lowest power consumption for floating pins
  P1 = 0b11111011; // Note P1.4 is dev-kit led, do not trigger it
  P3 = 0b11111111;
  P5 = 0b11111111;

  // PIN Function
  PSW1 = CONF_PSW1;
  PSW2 = CONF_PSW2;

  // Auxiliary register configuration
  AUXR = CONF_AUXR;

  // Clock configuration
  SFRX_ON();
  int i = 5;
  if (IRCBAND != (CONF_IRCBAND)) {
    IRCBAND = (CONF_IRCBAND);
    while (--i); // Wait a while after clock changed, or it may block the main process
  }
  i = 5;
  if (CLKDIV != (CONF_CLKDIV)) {
    CLKDIV = (CONF_CLKDIV);
    while (--i); // Wait a while after clock changed, or it may block the main process
  }
  SFRX_OFF();

  usart0_init();
  i2c_init();
  EA = 1;
}
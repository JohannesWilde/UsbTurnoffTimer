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
 * @file usart.h
 * @brief USART Driver
 * @author Thomas Reidemeister
 *
 */
#ifndef USART_H
#define USART_H

#include "stc8h.h"

/******************************************************************************\
 * Public macros
\******************************************************************************/

/******************************************************************************\
 * Function Prototypes
\******************************************************************************/

void usart0_init();
void usart0_send(char dat);
void usart0_send_str(char *dat);
void usart0_isr() __interrupt (SI0_VECTOR);

#endif // USART_H
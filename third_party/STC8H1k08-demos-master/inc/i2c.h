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
 * @file i2c.h
 * @brief I2C Driver.
 * @author Thomas Reidemeister
 *
 */
#ifndef I2C_H
#define I2C_H

#include "stc8h.h"

void i2c_init();
void i2c_start();
void i2c_recv_ack();
char i2c_recv_data();
void i2c_send_ack();
void i2c_stop();
void i2c_send(char dat);
void i2c_isr() __interrupt (I2C_VECTOR);

#endif // I2C_H
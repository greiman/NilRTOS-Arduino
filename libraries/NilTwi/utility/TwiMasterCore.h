/* Arduino NilRTOS Library
 * Copyright (C) 2013 by William Greiman
 *
 * This file is part of the Arduino NilRTOS Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino NilRTOS Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef TwiMasterCore_h
#define TwiMasterCore_h
#include <Arduino.h>
#include <TwiMasterConstants.h>

bool twiMstrTransfer(uint8_t add, void* buf, size_t nbytes, uint8_t option = I2C_STOP);
bool twiMstrTransferContinue(void* buf, size_t nbytes, uint8_t option = I2C_STOP);
void twiMstrBegin(bool speed = I2C_400KHZ, bool pullups = I2C_NO_PULLUPS);
void twiMstrPrintInfo(Print* pr);
void twiMstrPrintState(Print* pr, uint8_t state);
void twiMstrPrintFailureState(Print* pr);

inline uint8_t twiMstrFailureState() {
  extern uint8_t TWI_state;
  return TWI_state;
}
inline bool twiMstrReturnStatus() {
  extern uint8_t TWI_status;
  return TWI_status;
}
inline size_t twiMstrBytesTransfered() {
  extern size_t TWI_bufIndex;
return TWI_bufIndex;
}
inline size_t twiMstrRequestSize() {
  size_t TWI_nbytes;
  return TWI_nbytes;
}
inline bool twiMstrBusy() {return TWCR & (1<<TWIE);}
inline uint8_t twiMstrCurrentState() {return TWSR;}
inline uint32_t twiMstrSpeed() {return F_CPU/(2*TWBR + 16);}
#endif TwiMasterCore_h
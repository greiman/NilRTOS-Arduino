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
#include <TwiMasterCore.h>
#include <TwiState.h>
//------------------------------------------------------------------------------
void twiMstrPrintInfo(Print* pr) {
  bool status = twiMstrReturnStatus();
  
  pr->print(F("\r\nLast status: "));
  pr->println(status ? F("Success") : F("Failure"));
  
  pr->print(F("Request size: "));
  pr->println(twiMstrRequestSize());
  
  pr->print(F("Bytes transfered: "));
  pr->println(twiMstrBytesTransfered());
  
  if (!status) {
    pr->print(F("Failure state: "));
    twiMstrPrintState(pr, twiMstrFailureState());
#if TWI_MASTER_DEBUG
    pr->print(F("Failure TWCR: 0X"));
    pr->println(twiMstrFailureControl(), HEX);
    pr->print(F("Current TWCR: 0X"));
    pr->println(TWCR, HEX);
#endif  // TWI_MASTER_DEBUG
  }
  if (status || TWSR != twiMstrFailureState()) {
    pr->print(F("Curent state: "));
    twiMstrPrintState(pr, TWSR);
  }
}
//------------------------------------------------------------------------------
void twiMstrPrintState(Print* pr, uint8_t state) {
  pr->print(F("0X"));
  pr->print(state, HEX);
  pr->print(F(", "));
  switch(state) {
  
  case TWI_START:
    pr->println(F("A START has been transmitted."));
    break;
    
  case TWI_REP_START:
    pr->println(F("A Repeated START has been transmitted."));
    break;
    
  case TWI_ARB_LOST:
    pr->println(F("Arbitration lost."));
    break;
    
  case TWI_MTX_ADR_ACK:
    pr->println(F("SLA+W has been transmitted and ACK received."));
    break;
    
  case TWI_MTX_ADR_NACK:
    pr->println(F("SLA+W has been transmitted and NACK received."));
    break;

  case TWI_MTX_DATA_ACK:
    pr->println(F("Data byte has been transmitted and ACK received."));
    break;

  case TWI_MTX_DATA_NACK:
    pr->println(F("Data byte has been transmitted and NACK received."));
    break;

  case TWI_MRX_ADR_ACK:
    pr->println(F("SLA+R has been transmitted and ACK received."));
    break;

  case TWI_MRX_ADR_NACK:
    pr->println(F("SLA+R has been transmitted and NACK received."));
    break;

  case TWI_MRX_DATA_ACK:
    pr->println(F("Data byte has been received and ACK transmitted."));
    break;

  case TWI_MRX_DATA_NACK:
    pr->println(F("Data byte has been received and NACK transmitted."));
    break;

  case TWI_NO_STATE:
    pr->println(F("Stop condition or bus busy; TWINT == 0."));
    break;

  case TWI_BUS_ERROR:
    pr->println(F("Bus error due to an illegal START or STOP condition."));
    break;

  case TWI_START_TIMEOUT:
    pr->println(F("Start condition timeout.  Pull-up problem?"));
    break;

  default:
    pr->println(F("Invalid state code."));
    break;
  }
}
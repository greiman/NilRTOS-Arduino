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
//#include <Arduino.h>
#include <avr/interrupt.h>
#include <TwiMasterCore.h>
#include <TwiState.h>
//------------------------------------------------------------------------------
const uint8_t TWIBR_400KHZ = ((F_CPU/400000 - 16)/2);

const uint8_t TWIBR_100KHZ = ((F_CPU/100000 - 16)/2);
//------------------------------------------------------------------------------
// ISR variables.
volatile uint8_t* TWI_buf;
volatile size_t TWI_nbytes;
volatile size_t TWI_bufIndex;
volatile uint8_t TWI_option;

volatile bool TWI_status = 0;
volatile uint8_t TWI_fail_state = TWI_NO_STATE;
volatile uint8_t TWI_fail_control;
//------------------------------------------------------------------------------
// Function called from ISR to signal done.
void twiMstrSignal() __attribute__((weak));
void twiMstrSignal() {}

// Function called by library functions to wait for ISR to finish.
void twiMstrWait() __attribute__((weak));
void twiMstrWait() {
  uint32_t m = millis();
  uint16_t n = 0;
  while (twiMstrBusy()) {
//   if ((millis() - m) > 100) TWCR = (1 << TWEN);
  }
}
//------------------------------------------------------------------------------
void twiMstrSpeed(bool speedCode) {
  // prescaller zero
  TWSR = 0;
  TWBR = speedCode == I2C_400KHZ ? TWIBR_400KHZ : TWIBR_100KHZ;
}
//------------------------------------------------------------------------------
void twiMstrPullups(bool pullupState) {
  digitalWrite(SDA, pullupState == I2C_INTERNAL_PULLUPS);
  digitalWrite(SCL, pullupState == I2C_INTERNAL_PULLUPS);
}
//------------------------------------------------------------------------------
void twiMstrBegin(bool speed, bool pullups) {
  // enable pull-ups if requested

  digitalWrite(SDA, pullups == I2C_INTERNAL_PULLUPS);
  digitalWrite(SCL, pullups == I2C_INTERNAL_PULLUPS);
  
  TWSR = 0;
  TWBR = speed == I2C_400KHZ ? TWIBR_400KHZ : TWIBR_100KHZ;
  
  return;///////////////////////////////////////////////////////////////////////////////
  TWCR = (1<<TWEN)|(0<<TWIE)|(1<<TWINT)|
         (0<<TWEA)|(0<<TWSTA)|(1<<TWSTO);
}
//------------------------------------------------------------------------------
bool twiMstrTransfer(uint8_t add, void* buf, size_t nbytes, uint8_t option) {

  TWI_nbytes = nbytes;
  TWI_buf = (uint8_t*)buf;
  TWI_option = option;

  TWI_bufIndex = 0;
  TWI_status = 0;
  
  if (TWSR != TWI_REP_START) {
    // Need to issue START condition.
    TWCR = (1<<TWEN)|(0<<TWIE)|(1<<TWINT)|
           (0<<TWEA)|(1<<TWSTA)|(0<<TWSTO);
    // Wait for Start to complete.
    uint16_t nt = 1000;
    while (!(TWCR & (1 << TWINT)) && --nt);
    if (nt == 0) {
      TWI_fail_control = TWCR;
      TWI_fail_state = TWI_START_TIMEOUT;
      return TWI_status;
    }
  }
  TWDR = add;
  // Send address and R/W. Enable interrupts.
  TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|
         (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO);

  // Wait for ISR to complete transfer.
  twiMstrWait();

  return TWI_status;
}
//------------------------------------------------------------------------------

bool twiMstrTransferContinue(void* buf, size_t nbytes, uint8_t option) {
  TWI_nbytes = nbytes;
  TWI_buf = (uint8_t*)buf;
  TWI_option = option;
  TWI_bufIndex = 0;
  
  if (nbytes == 0) {
    TWI_status = 1;
    return TWI_status;
  }
  TWI_status = 0;
  
  switch (TWSR) {
  case TWI_MTX_ADR_ACK:
  case TWI_MTX_DATA_ACK:

    TWDR = TWI_buf[TWI_bufIndex++];
      // Start TX interrupts enabled.
    TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|
           (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO);
    break;
    
  case TWI_MRX_ADR_ACK:
  case TWI_MRX_DATA_ACK:
    if (nbytes == 1 && (option & (I2C_STOP | I2C_REP_START))) {
      // Last byte with NACK for stop or repeated start.
      TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|
             (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO);
    } else {
      // ACK for continue or more than one byte.
      TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|
             (1<<TWEA)|(0<<TWSTA)|(0<<TWSTO);
    }
    break;
    
  default:
    TWI_fail_control = TWCR;
    TWI_fail_state = TWSR;
    return TWI_status;
  }
  twiMstrWait();
  
  return TWI_status;
}
//------------------------------------------------------------------------------
static void twiDone() {
      if (TWI_option & I2C_STOP) {
        TWCR = (1<<TWEN)|(0<<TWIE)|(1<<TWINT)|
               (0<<TWEA)|(0<<TWSTA)|(1<<TWSTO);
      } else if (TWI_option & I2C_REP_START) {
        TWCR = (1<<TWEN)|(0<<TWIE)|(1<<TWINT)|
               (0<<TWEA)|(1<<TWSTA)|(0<<TWSTO);
      } else {
        TWCR = (1<<TWEN)|(0<<TWIE)|(0<<TWINT)|
               (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO);
      }
      TWI_status = true;
      twiMstrSignal();
}
//------------------------------------------------------------------------------
ISR(TWI_vect) {
  int nb;
  
  switch (TWSR) {
  
  case TWI_MTX_DATA_ACK:
  case TWI_MTX_ADR_ACK:
    if (TWI_bufIndex < TWI_nbytes) {
      TWDR = TWI_buf[TWI_bufIndex++];
      // Start TX interrupts enabled.
      TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|
             (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO);
    } else {
      twiDone();
    }
    break;
  
  case TWI_MRX_DATA_ACK:
      TWI_buf[TWI_bufIndex++] = TWDR;

  case TWI_MRX_ADR_ACK:
    // Bytes remaining to be received.
    nb = TWI_nbytes - TWI_bufIndex;
    if (nb > 1) {
      // RX with ACK
      TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|
             (1<<TWEA)|(0<<TWSTA)|(0<<TWSTO);
    } else if (nb == 1) {
      if (TWI_option & (I2C_STOP | I2C_REP_START)) {
        // RX with NACK
        TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|
               (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO);
      } else {
        // RX with ACK
        TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|
               (1<<TWEA)|(0<<TWSTA)|(0<<TWSTO);
      }
    } else {
      twiDone();
    }

    break;

  case TWI_MRX_DATA_NACK:
    // Last byte of message.
    TWI_buf[TWI_bufIndex++] = TWDR;
    twiDone();
    break;
    
  default:
    TWI_fail_control = TWCR;
    TWI_fail_state = TWSR;
    TWCR = (1<<TWEN)|(0<<TWIE)|(1<<TWINT)|
           (0<<TWEA)|(0<<TWSTA)|(1<<TWSTO);
    twiMstrSignal();
  }
}

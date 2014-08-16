/*
  TwoWireMaster.cpp - TWI/I2C library for Wiring & Arduino
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 
  Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts
  
  Modified 2013 by William Greiman for RTOS use
*/
#if ORG_FILE
extern "C" {
  #include <stdlib.h>
  #include <string.h>
  #include <inttypes.h>
  #include "twi.h"
}
#include "Wire.h"
#else  // ORG_FILE
#include <TwiMasterCore.h>
#include <TwiState.h>
#include <WireMaster.h>
#endif  // ORG_FILE


// Initialize Class Variables //////////////////////////////////////////////////

uint8_t TwoWireMaster::rxBuffer[BUFFER_LENGTH];
uint8_t TwoWireMaster::rxBufferIndex = 0;
uint8_t TwoWireMaster::rxBufferLength = 0;

uint8_t TwoWireMaster::txAddress = 0;
uint8_t TwoWireMaster::txBuffer[BUFFER_LENGTH];
uint8_t TwoWireMaster::txBufferIndex = 0;
uint8_t TwoWireMaster::txBufferLength = 0;
uint8_t TwoWireMaster::transmitting = 0;

#if ORG_FILE
void (*TwoWireMaster::user_onRequest)(void);
void (*TwoWireMaster::user_onReceive)(int);
#endif // ORG_FILE
// Constructors ////////////////////////////////////////////////////////////////

TwoWireMaster::TwoWireMaster()
{
}

// Public Methods //////////////////////////////////////////////////////////////

void TwoWireMaster::begin(void)
{
  rxBufferIndex = 0;
  rxBufferLength = 0;

  txBufferIndex = 0;
  txBufferLength = 0;
#if ORG_FILE
  twi_init();
#else ORG_FILE
  twiMstrBegin(I2C_100KHZ, I2C_INTERNAL_PULLUPS);
#endif  // ORG_FILE
}
#if ORG_FILE
void TwoWireMaster::begin(uint8_t address)
{
  twi_setAddress(address);
  twi_attachSlaveTxEvent(onRequestService);
  twi_attachSlaveRxEvent(onReceiveService);
  begin();
}

void TwoWireMaster::begin(int address)
{
  begin((uint8_t)address);
}
#endif  // ORG_FILE
uint8_t TwoWireMaster::requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop)
{
  // clamp to buffer length
  if(quantity > BUFFER_LENGTH){
    quantity = BUFFER_LENGTH;
  }
#if ORG_FILE
  // perform blocking read into buffer
  uint8_t read = twi_readFrom(address, rxBuffer, quantity, sendStop);
#else  // ORG_FILE
  bool s = twiMstrTransfer((address << 1) | I2C_READ, rxBuffer, quantity,
                           sendStop ? I2C_STOP : I2C_REP_START);
  uint8_t read = twiMstrBytesTransfered();
#endif  // ORG_FILE
  // set rx buffer iterator vars
  rxBufferIndex = 0;
  rxBufferLength = read;

  return read;
}

uint8_t TwoWireMaster::requestFrom(uint8_t address, uint8_t quantity)
{
  return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)true);
}

uint8_t TwoWireMaster::requestFrom(int address, int quantity)
{
  return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)true);
}

uint8_t TwoWireMaster::requestFrom(int address, int quantity, int sendStop)
{
  return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)sendStop);
}

void TwoWireMaster::beginTransmission(uint8_t address)
{
  // indicate that we are transmitting
  transmitting = 1;
  // set address of targeted slave
  txAddress = address;
  // reset tx buffer iterator vars
  txBufferIndex = 0;
  txBufferLength = 0;
}

void TwoWireMaster::beginTransmission(int address)
{
  beginTransmission((uint8_t)address);
}

//
//	Originally, 'endTransmission' was an f(void) function.
//	It has been modified to take one parameter indicating
//	whether or not a STOP should be performed on the bus.
//	Calling endTransmission(false) allows a sketch to 
//	perform a repeated start. 
//
//	WARNING: Nothing in the library keeps track of whether
//	the bus tenure has been properly ended with a STOP. It
//	is very possible to leave the bus in a hung state if
//	no call to endTransmission(true) is made. Some I2C
//	devices will behave oddly if they do not see a STOP.
//
uint8_t TwoWireMaster::endTransmission(uint8_t sendStop)
{
#if ORG_FILE
  // transmit buffer (blocking)
  int8_t ret = twi_writeTo(txAddress, txBuffer, txBufferLength, 1, sendStop);
#else  // ORG_FILE
  bool s = twiMstrTransfer((txAddress << 1) | I2C_WRITE, txBuffer,
                           txBufferLength, sendStop ? I2C_STOP : I2C_REP_START);
   int8_t ret = 0;
   if (!s) {
     if (twiMstrFailureState() == TWI_MTX_ADR_NACK) {
       ret = 2;
     }else if (twiMstrFailureState() == TWI_MTX_DATA_NACK) {
       ret = 3;
     } else {
       ret = 4;
     }
   }
#endif  // ORG_FILE
  // reset tx buffer iterator vars
  txBufferIndex = 0;
  txBufferLength = 0;
  // indicate that we are done transmitting
  transmitting = 0;
  return ret;
}

//	This provides backwards compatibility with the original
//	definition, and expected behaviour, of endTransmission
//
uint8_t TwoWireMaster::endTransmission(void)
{
  return endTransmission(true);
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
size_t TwoWireMaster::write(uint8_t data)
{
  if(transmitting){
  // in master transmitter mode
    // don't bother if buffer is full
    if(txBufferLength >= BUFFER_LENGTH){
      setWriteError();
      return 0;
    }
    // put byte in tx buffer
    txBuffer[txBufferIndex] = data;
    ++txBufferIndex;
    // update amount in buffer   
    txBufferLength = txBufferIndex;
  }else{
  // in slave send mode
    // reply to master
#if ORG_FILE
    twi_transmit(&data, 1);
#endif  // ORG_FILE
  }
  return 1;
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
size_t TwoWireMaster::write(const uint8_t *data, size_t quantity)
{
  if(transmitting){
  // in master transmitter mode
    for(size_t i = 0; i < quantity; ++i){
      write(data[i]);
    }
  }else{
  // in slave send mode
    // reply to master
#if ORG_FILE
    twi_transmit(data, quantity);
#endif  // ORG_FILE
  }
  return quantity;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWireMaster::available(void)
{
  return rxBufferLength - rxBufferIndex;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWireMaster::read(void)
{
  int value = -1;
  
  // get each successive byte on each call
  if(rxBufferIndex < rxBufferLength){
    value = rxBuffer[rxBufferIndex];
    ++rxBufferIndex;
  }

  return value;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWireMaster::peek(void)
{
  int value = -1;
  
  if(rxBufferIndex < rxBufferLength){
    value = rxBuffer[rxBufferIndex];
  }

  return value;
}

void TwoWireMaster::flush(void)
{
  // XXX: to be implemented.
}
#if ORG_FILE
// behind the scenes function that is called when data is received
void TwoWireMaster::onReceiveService(uint8_t* inBytes, int numBytes)
{
  // don't bother if user hasn't registered a callback
  if(!user_onReceive){
    return;
  }
  // don't bother if rx buffer is in use by a master requestFrom() op
  // i know this drops data, but it allows for slight stupidity
  // meaning, they may not have read all the master requestFrom() data yet
  if(rxBufferIndex < rxBufferLength){
    return;
  }
  // copy twi rx buffer into local read buffer
  // this enables new reads to happen in parallel
  for(uint8_t i = 0; i < numBytes; ++i){
    rxBuffer[i] = inBytes[i];    
  }
  // set rx iterator vars
  rxBufferIndex = 0;
  rxBufferLength = numBytes;
  // alert user program
  user_onReceive(numBytes);
}

// behind the scenes function that is called when data is requested
void TwoWireMaster::onRequestService(void)
{
  // don't bother if user hasn't registered a callback
  if(!user_onRequest){
    return;
  }
  // reset tx buffer iterator vars
  // !!! this will kill any pending pre-master sendTo() activity
  txBufferIndex = 0;
  txBufferLength = 0;
  // alert user program
  user_onRequest();
}

// sets function called on slave write
void TwoWireMaster::onReceive( void (*function)(int) )
{
  user_onReceive = function;
}

// sets function called on slave read
void TwoWireMaster::onRequest( void (*function)(void) )
{
  user_onRequest = function;
}
#endif  // ORG_FILE
// Preinstantiate Objects //////////////////////////////////////////////////////

TwoWireMaster Wire = TwoWireMaster();


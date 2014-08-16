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
#ifndef TwiMaster_h
#define TwiMaster_h
 /**
 * @file    TwiMaster.h
 * @brief   Nil RTOS Two Wire Interface library header.
 *
 * @defgroup twoWire TwiMaster
 * @details RTOS Two Wire Interface library.
 * @{
 */
#include <Arduino.h>
#include <TwiMasterCore.h>
#include <WireMaster.h>
//------------------------------------------------------------------------------
/** TwiMaster version YYYYMMDD */
#define TWI_MASTER_VERSION 20130719
//------------------------------------------------------------------------------
/**
 * @class TwiMaster
 * @brief Two Wire Interface class for I2C master mode.
 */
class TwiMaster {
 public:
  /**
   * Initialize the AVR 2-wire Serial Interface.
   *  @param[in] speed   I2C bus speed, one of:
   *                     - I2C_400KHZ
   *                     - I2C_100KHZ
   *                     .
   *
   *  @param[in] pullups  State of internal AVR pull-ups, one of:
   *                      - I2C_NO_PULLUPS
   *                      - I2C_PULLUPS
   *                      .
   */
  void begin(bool speed = I2C_400KHZ, bool pullups = I2C_NO_PULLUPS) {
    twiMstrBegin(speed, pullups);
  }
  /**
   * Start an I2C transfer with possible continuation.
   *
   * @param[in] addrRW    I2C slave address plus R/W bit.
   *                      The I2C slave address is in the high seven bits
   *                      and is ORed with on of the following:
   *                      - I2C_READ for a read transfer.
   *                      - I2C_WRITE for a write transfer.
   *                      .
   * @param[in,out] buf   Source or destination for transfer.
   * @param[in] nbytes    Number of bytes to transfer (may be zero).
   * @param[in] option    Option for ending the transfer, one of:
   *                      - I2C_STOP end the transfer with an I2C stop
   *                        condition.
   *                      - I2C_REP_START end the transfer with an I2C
   *                        repeated start condition.
   *                      - I2C_CONTINUE allow additional transferContinue()
   *                        calls.
   *                      .
   * @return true for success else false.
   */
  bool transfer(uint8_t addrRW, void* buf,
                size_t nbytes, uint8_t option = I2C_STOP) {
    return twiMstrTransfer(addrRW, buf, nbytes, option);
  }
  /**
   * Continue an I2C transfer.
   *
   * @param[in,out] buf   Source or destination for transfer.
   * @param[in] nbytes    Number of bytes to transfer (may be zero).
   * @param[in] option    Option for ending the transfer, one of:
   *                      - I2C_STOP end the transfer with an I2C stop
   *                        condition.
   *                      - I2C_REP_START end the transfer with an I2C
   *                        repeated start condition.
   *                      - I2C_CONTINUE allow additional transferContinue()
   *                        calls.
   *                      .
   * @return true for success else false.
   */
  bool transferContinue(void* buf, size_t nbytes, uint8_t option = I2C_STOP) {
    return twiMstrTransferContinue(buf, nbytes, option);
  }
  /**
   *  Set the I2C bus speed.
   *
   *  @param[in] speedCode  I2C bus speed, one of:
   *                        - I2C_400KHZ
   *                        - I2C_100KHZ
   *                        .
   */
  void speed(bool speedCode) {twiMstrSpeed(speedCode);}
  /**
   *  Enable or disable internal pull-up resistors.
   *
   *  @param[in] pullupState  State of internal AVR pull-ups, one of:
   *                          - I2C_NO_PULLUPS
   *                          - I2C_PULLUPS
   *                          .
   */
  void pullups(bool pullupState) {twiMstrPullups(pullupState);}
  
  /**
   * @return The I2C bus state when failue was detected in the
   *         last transfer() or transferContinue() call.
   */
  uint8_t failureState() {return twiMstrFailureState();}
  /**
   * Print internal information from the
   * last transfer() or transferContinue() call.
   *
   * @param[in] pr The print stream for the output.
   */
  void printInfo(Print* pr) {twiMstrPrintInfo(pr);}
  /**
   * @return The current I2C bus state.
   */
  uint8_t currentState() {return twiMstrCurrentState();}
  /**
   * @return The status returned by the
   *         last transfer() or transferContinue() call.
   */
  bool returnStatus() {return twiMstrReturnStatus();}

  /**
   * @return The number of bytes of data requested for transfer in the
   *         last transfer() or transferContinue() call.
   */
  uint8_t requestSize() {return twiMstrRequestSize();}
  /**
   * @return The actual number of data bytes transfered in the
   *         last transfer() or transferContinue() call.
   */
  uint8_t bytesTransfered() {return twiMstrBytesTransfered();}
};
/** @} */
#endif  // TwiMaster_h

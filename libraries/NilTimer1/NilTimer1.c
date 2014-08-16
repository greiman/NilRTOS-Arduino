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
/**
 * @file    NilTimer1.c
 * @brief   Nil RTOS timer 1 source
 *
 * @defgroup Timer1 NilTimer1
 * @details Nil RTOS timer 1 functions.
 * @{
 */
#include <NilTimer1.h>

#if !defined(__DOXYGEN__)

// Declare and initialize the semaphore.
static SEMAPHORE_DECL(timerSem, 0);

/** ADC ISR. */
NIL_IRQ_HANDLER(TIMER1_COMPB_vect) {

  NIL_IRQ_PROLOGUE();

  /* Nop on AVR.*/
  nilSysLockFromISR();

  /* Invocation of some I-Class system APIs, never preemptable.*/

  /* Signal handler thread. */
  nilSemSignalI(&timerSem);

  /* Nop on AVR.*/
  nilSysUnlockFromISR();

  NIL_IRQ_EPILOGUE();
}
#endif  // __DOXYGEN

/**
 * Start periodic timer 1 signals.
 *
 * @param[in] microseconds The period in microseconds between signals.
 * @note The maximum value for microseconds is 4,194,304 for a 16 MHz CPU.
 */
 void nilTimer1Start(uint32_t microseconds){
 // CPU cycles per time period.
 long cycles = (F_CPU / 1000000) * microseconds;

  nilTimer1Stop();
  
  uint8_t tshift;
  if (cycles <= 0X10000) {
    // no prescale, CTC mode
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10);
    tshift = 0;
  } else if (cycles <= 0X10000*8L) {
    // prescale 8, CTC mode
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);
    tshift = 3;
  } else if (cycles <= 0X10000*64L) {
    // prescale 64, CTC mode
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10);
    tshift = 6;
  } else if (cycles <= 0X10000*256L) {
    // prescale 256, CTC mode
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS12);
    tshift = 8;
  } else {
    // prescale 1024, CTC mode
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS12) | (1 << CS10);
    tshift = 10;
    // microseconds too large, use max time.
    if (cycles > 0X10000*1024L) cycles = 0X10000*1024L;
  }
  // divide by prescaler
  cycles >>= tshift;
  
  // set TOP for timer reset
  nilSysLock();
  ICR1 = cycles - 1;
  TCNT1 = 0;
  // clear pending interrupt
  TIFR1 |= (1 << OCF1B);
  // enable compare B interrupt
  TIMSK1 = (1 << OCIE1B);
  nilSysUnlock();

 }
 /** Stop timer1 signals. */
 void nilTimer1Stop() {
  TCCR1A = 0;
  TCCR1B = 0;
  TIMSK1 = 0;
 }
/** Sleep while waiting for timer1 signal.
 * @note This function should not be used in the idle thread.
 * @return true if success or false if overrun or error.
 */
 bool nilTimer1Wait() {
  // Idle thread can't sleep.
 	if (nilIsIdleThread()) return false;
 	if (nilSemWaitTimeout(&timerSem, TIME_IMMEDIATE) != NIL_MSG_TMO) return false;
  nilSemWait(&timerSem);
  return true;
 }
 /** @} */
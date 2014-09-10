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
 * @file    NilAnalog.c
 * @brief   Nil RTOS ADC read source
 *
 * @defgroup Analog NilAnalog
 * @details Nil RTOS ADC functions.
 * @{
 */
#include <NilRTOS.h>
#include <Arduino.h>
#include <NilAnalog.h>
static uint8_t nil_analog_reference = DEFAULT;

#if !defined(__DOXYGEN__)

// Declare and initialize the semaphore.
static SEMAPHORE_DECL(adcSem, 0);

/** ADC ISR. */
NIL_IRQ_HANDLER(ADC_vect) {

  NIL_IRQ_PROLOGUE();

  /* Nop on AVR.*/
  nilSysLockFromISR();

  /* Invocation of some I-Class system APIs, never preemptable.*/

  /* Signal handler thread. */
  nilSemSignalI(&adcSem);

  /* Nop on AVR.*/
  nilSysUnlockFromISR();

  NIL_IRQ_EPILOGUE();
}
#endif  // DOXYGEN
//------------------------------------------------------------------------------
/**
 * Set the ADC prescalar factor.
 * @param[in] ps Prescalar bits.
 *  - ADC_PS_128: Arduino default (125 kHz on a 16 MHz CPU)
 *  - ADC_PS_64: (250 kHz on a 16 MHz CPU)
 *  - ADC_PS_32: (500 kHz on a 16 MHz CPU)
 */
void nilAnalogPrescalar(uint8_t ps) {
  ADCSRA &= ~ADC_PS_BITS;
  ADCSRA |= ps;
}
//------------------------------------------------------------------------------
/**
 * Configures the reference voltage used for analog input
 * (i.e. the value used as the top of the input range). The options are:
 * @param[in] mode the ADC reference mode.
    - DEFAULT: the default analog reference of 5 volts (on 5V Arduino boards)
      or 3.3 volts (on 3.3V Arduino boards)
    - INTERNAL: an built-in reference, equal to 1.1 volts on the ATmega168 or
      ATmega328 and 2.56 volts on the ATmega8 (not available on the Arduino Mega)
    - INTERNAL1V1: a built-in 1.1V reference (Arduino Mega only)
    - INTERNAL2V56: a built-in 2.56V reference (Arduino Mega only)
    - EXTERNAL: the voltage applied to the AREF pin (0 to 5V only) is used
      as the reference.
*/
void nilAnalogReference(uint8_t mode)
{
	// can't actually set the register here because the default setting
	// will connect AVCC and the AREF pin, which would cause a short if
	// there's something connected to AREF.
	nil_analog_reference = mode;
}

/**
 * Read the value from the specified analog pin.
 * @note nilAnalogRead() sleeps, if possible, while the ADC conversion
 *                       is in progress.
 * @param[in] pin the number of the analog input pin to read from (0 to 5 on
 *            most boards, 0 to 7 on the Mini and Nano, 0 to 15 on the Mega)
 *
 * @return pin ADC conversion value (0 to 1023).
 *
 */
int nilAnalogRead(uint8_t pin) {
#if defined(__AVR_ATmega32U4__)
	pin = analogPinToChannel(pin);
	ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#elif defined(ADCSRB) && defined(MUX5)
	// the MUX5 bit of ADCSRB selects whether we're reading from channels
	// 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high).
	ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#endif

	// set the analog reference (high two bits of ADMUX) and select the
	// channel (low 4 bits).  this also sets ADLAR (left-adjust result)
	// to 0 (the default).
#if defined(ADMUX)
	ADMUX = (nil_analog_reference << 6) | (pin & 0x07);
#endif

  if (!nilIsIdleThread()) {
    // Not idle thread so use interrupt and sleep.
	  ADCSRA |= (1 << ADIE) | (1 << ADSC);
    nilSemWait(&adcSem);
    ADCSRA &= ~(1 << ADIE);
	} else {
    ADCSRA |= (1 << ADSC);
	  // ADSC is cleared when the conversion finishes
	  while (ADCSRA & (1 << ADSC));
	}
  // this will access ADCL first.
  return ADC;
}
/** @} */
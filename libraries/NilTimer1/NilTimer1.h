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
 * @file    NilTimer1.h
 * @brief   Nil RTOS timer 1 source
 *
 * @defgroup Timer1 NilTimer1
 * @details Nil RTOS timer 1 functions.
 * @{
 */
#ifndef NilTimer1_h
#define NilTimer1_h
#include <NilRTOS.h>
//------------------------------------------------------------------------------
/** NilRTimer1 version YYYYMMDD */
#define NIL_TIMER1_VERSION 20130719
#ifdef __cplusplus
extern "C" {
#endif
 void nilTimer1Start(uint32_t microseconds);
 void nilTimer1Stop();
 bool nilTimer1Wait();
#ifdef __cplusplus
}
#endif
#endif // NilTimer1
/** @} */
/**
 * @file
 *
 * @brief Atomic operations.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once
#include <stdint.h>

/**
 * @brief Performs a thread safe exchange of two values.
 * @param val Reference to the value to be exchanged.
 * @param xchg Value to exchange for.
 * @return Previous value of val.
 */
void *RZThreadSafeExchange(void *volatile &val, void *xchg);
/**
 * @brief Performs a thread safe addition of one value to another.
 * @param val Reference to the value to be added to.
 * @param inc Value to add.
 * @return New value of val.
 */
uint32_t RZThreadSafeAdd(volatile uint32_t &val, uint32_t inc);

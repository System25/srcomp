/* Bit manipulation functions test definition.

   Copyright (C) 2022 Abraham Macias Paredes.
   
   This program is free software: you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef TEST_BITM_H
#define TEST_BITM_H


#include "bitm.h"

/** 
 * Test the allocation of a bit manipulation array.
 */
void alloc_test();

/**
 * Test the writting of single bits.
 */
void write_bit_test();

/**
 * Test the reading of single bits.
 */
void read_bit_test();

/**
 * Test the reading of multiple bits.
 */
void read_nbit_test();

/**
 * Test the reading of multiple bits between elements.
 */
void read_nbit_between_elements_test();

/**
 * Test the writting of multiple bits.
 */
void write_nbit_test();

/**
 * Test the writting of multiple bits between elements.
 */
void write_nbit_between_elements_test();

/**
 * Test the reading of unary data.
 */
void read_unary_test();

/**
 * Test the writting of unary data.
 */
void write_unary_test();

/**
 * Test the reading of Elias-Gamma coded data.
 */
void read_eg_test();

/**
 * Test the writting of Elias-Gamma coded data.
 */
void write_eg_test();




#endif
/* Bit manipulation functions tests implementation.

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

#include "test_bitm.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>



/* ======================================================================== */
/** 
 * Test the allocation of a bit manipulation array.
 */
void alloc_test() {
  int length;
  bitm_array *bma;
  
  // given
  length = 16;
  
  // when
  bma = bitm_alloc(length);
  
  // then
	assert_non_null(bma);
  assert_int_equal(length, bitm_get_length(bma));
  assert_int_equal(0, bitm_get_index(bma));
  
  // cleanup
  bitm_free(bma);
}

/* ======================================================================== */
/**
 * Test the writting of single bits.
 */
void write_bit_test() {
  bitm_array *bma;
  
  // given
  bma = bitm_alloc(16);
  
  // when
  bitm_write_bit(bma, 1);
  bitm_write_bit(bma, 1);
  bitm_write_bit(bma, 0);
  bitm_write_bit(bma, 0);
  bitm_write_bit(bma, 1);
  bitm_write_bit(bma, 0);
  bitm_write_bit(bma, 1);
  bitm_write_bit(bma, 0);
  bitm_flush(bma);
  
  // then
  assert_int_equal(1, bitm_get_index(bma));
  assert_int_equal(0xCA000000, *bitm_get_data(bma));
  
  // cleanup
  bitm_free(bma);
}

/* ======================================================================== */
/**
 * Test the reading of single bits.
 */
void read_bit_test() {
  ELEMENT *data;
  bitm_array *bma;
  int length;
  
  // given
  length = 16;
  data = (ELEMENT *) malloc(sizeof(ELEMENT) * length);
  *data = 0xCA000000;
  bma =  bitm_wrap(data, length);
  
  // when - then
  assert_int_equal(1, bitm_read_bit(bma));
  assert_int_equal(1, bitm_read_bit(bma));
  assert_int_equal(0, bitm_read_bit(bma));
  assert_int_equal(0, bitm_read_bit(bma));
  assert_int_equal(1, bitm_read_bit(bma));
  assert_int_equal(0, bitm_read_bit(bma));
  assert_int_equal(1, bitm_read_bit(bma));
  assert_int_equal(0, bitm_read_bit(bma));
  
  // cleanup
  bitm_free(bma);
}

/* ======================================================================== */
/**
 * Test the reading of multiple bits.
 */
void read_nbit_test() {
  ELEMENT *data;
  bitm_array *bma;
  int length;
  
  // given
  length = 16;
  data = (ELEMENT *) malloc(sizeof(ELEMENT) * length);
  *data = 0x12345678;
  bma =  bitm_wrap(data, length);
  
  // when - then
  assert_int_equal(1, bitm_read_nbits(bma, 4));
  assert_int_equal(2, bitm_read_nbits(bma, 4));
  assert_int_equal(3, bitm_read_nbits(bma, 4));
  assert_int_equal(4, bitm_read_nbits(bma, 4));
  assert_int_equal(5, bitm_read_nbits(bma, 4));
  assert_int_equal(6, bitm_read_nbits(bma, 4));
  assert_int_equal(7, bitm_read_nbits(bma, 4));
  assert_int_equal(8, bitm_read_nbits(bma, 4));
  
  // cleanup
  bitm_free(bma);
}

/* ======================================================================== */
/**
 * Test the reading of multiple bits between elements.
 */
void read_nbit_between_elements_test() {
  ELEMENT *data;
  bitm_array *bma;
  int length;
  
  // given
  length = 16;
  data = (ELEMENT *) malloc(sizeof(ELEMENT) * length);
  *data = 0x12345678;
  data[1] = 0x12345678;
  bma =  bitm_wrap(data, length);
  
  // when - then
  assert_int_equal(0x1234567, bitm_read_nbits(bma, 28));
  assert_int_equal(0x81, bitm_read_nbits(bma, 8));
  assert_int_equal(0x2345678, bitm_read_nbits(bma, 28));
  
  // cleanup
  bitm_free(bma);
}

/* ======================================================================== */
/**
 * Test the writting of multiple bits.
 */
void write_nbit_test() {
  bitm_array *bma;
  
  // given
  bma = bitm_alloc(16);
  
  // when
  bitm_write_nbits(bma, 4, 0xC);
  bitm_write_nbits(bma, 4, 0xA);
  bitm_flush(bma);
  
  // then
  assert_int_equal(1, bitm_get_index(bma));
  assert_int_equal(0xCA000000, *bitm_get_data(bma));
  
  // cleanup
  bitm_free(bma);
}

/* ======================================================================== */
/**
 * Test the writting of multiple bits between elements.
 */
void write_nbit_between_elements_test() {
  bitm_array *bma;
  
  // given
  bma = bitm_alloc(16);
  
  // when
  bitm_write_nbits(bma, 28, 0x1234567);
  bitm_write_nbits(bma, 8, 0x81);
  bitm_write_nbits(bma, 28, 0x2345678);  
  bitm_flush(bma);
  
  // then
  assert_int_equal(2, bitm_get_index(bma));
  assert_int_equal(0x12345678, bitm_get_data(bma)[0]);
  assert_int_equal(0x12345678, bitm_get_data(bma)[1]);
  
  // cleanup
  bitm_free(bma);
}


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

/* ======================================================================== */
/**
 * Test the reading of unary data.
 */
void read_unary_test() {
  ELEMENT *data;
  bitm_array *bma;
  int length;
  
  // given
  length = 16;
  data = (ELEMENT *) malloc(sizeof(ELEMENT) * length);
  data[0] = 0x01810000;
  data[1] = 0x80000000;
  data[2] = 0x00000000;
  data[3] = 0x00000001;
  bma =  bitm_wrap(data, length);
  
  // when - then
  assert_int_equal(7, bitm_read_unary(bma));
  assert_int_equal(0, bitm_read_unary(bma)); 
  assert_int_equal(6, bitm_read_unary(bma)); 
  assert_int_equal(16, bitm_read_unary(bma)); 
  assert_int_equal(94, bitm_read_unary(bma)); 
  
  // cleanup
  bitm_free(bma);
}

/* ======================================================================== */
/**
 * Test the writting of unary data.
 */
void write_unary_test() {
  bitm_array *bma;
  
  // given
  bma = bitm_alloc(16);
  
  // when
  bitm_write_unary(bma, 7);
  bitm_write_unary(bma, 0);
  bitm_write_unary(bma, 6);
  bitm_write_unary(bma, 16);
  bitm_write_unary(bma, 94);
  bitm_flush(bma);
  
  // then
  assert_int_equal(4, bitm_get_index(bma));
  assert_int_equal(0x01810000, bitm_get_data(bma)[0]);
  assert_int_equal(0x80000000, bitm_get_data(bma)[1]);
  assert_int_equal(0x00000000, bitm_get_data(bma)[2]);
  assert_int_equal(0x00000001, bitm_get_data(bma)[3]); 
  
  // cleanup
  bitm_free(bma);
}

/* ======================================================================== */
/**
 * Test the reading of Elias-Gamma coded data.
 */
void read_eg_test() {
  ELEMENT *data;
  bitm_array *bma;
  int length;
  
  // given
  length = 16;
  data = (ELEMENT *) malloc(sizeof(ELEMENT) * length);
  data[0] = 0xB1E00000;
  bma =  bitm_wrap(data, length);
  
  // when - then
  assert_int_equal(1, bitm_read_eg(bma));
  assert_int_equal(3, bitm_read_eg(bma)); 
  assert_int_equal(15, bitm_read_eg(bma));
  
  // cleanup
  bitm_free(bma);
}

/* ======================================================================== */
/**
 * Test the writting of Elias-Gamma coded data.
 */
void write_eg_test() {
  bitm_array *bma;
  
  // given
  bma = bitm_alloc(16);
  
  // when
  bitm_write_eg(bma, 1);
  bitm_write_eg(bma, 3);
  bitm_write_eg(bma, 15);
  bitm_flush(bma);
  
  // then
  assert_int_equal(1, bitm_get_index(bma));
  assert_int_equal(0xB1E00000, *bitm_get_data(bma));
  
  // cleanup
  bitm_free(bma);
}

/* ======================================================================== */
/**
 * Test the reset method functionality.
 */
void reset_test() {
  bitm_array *bma;
  
  // given
  bma = bitm_alloc(16);
  bitm_write_unary(bma, 7);
  bitm_write_unary(bma, 0);
  bitm_flush(bma);
  assert_int_equal(1, bitm_get_index(bma));

  // when
  bitm_reset(bma);
  bitm_write_unary(bma, 7);
  bitm_write_unary(bma, 0);
  bitm_write_unary(bma, 6);
  bitm_write_unary(bma, 16);
  bitm_write_unary(bma, 94);
  bitm_flush(bma);
  
  // then
  assert_int_equal(4, bitm_get_index(bma));
  assert_int_equal(0x01810000, bitm_get_data(bma)[0]);
  assert_int_equal(0x80000000, bitm_get_data(bma)[1]);
  assert_int_equal(0x00000000, bitm_get_data(bma)[2]);
  assert_int_equal(0x00000001, bitm_get_data(bma)[3]);
  
  // cleanup
  bitm_free(bma);
}


/* ======================================================================== */
/**
 * Test to write and read all the numbers from 1 to 256 in unary coding.
 */
void unsigned_char_unary_test() {
  bitm_array *bma;
  int i;
  
  // given
  bma = bitm_alloc(10000);
  for (i=1; i<=256; i++) {
    bitm_write_unary(bma, i);
  }
  
  bitm_flush(bma);
  
  bitm_reset(bma);

  // when - then
  for (i=1; i<=256; i++) {
    assert_int_equal(bitm_read_unary(bma), i);
  }
  
  // cleanup
  bitm_free(bma);
}

/* ======================================================================== */
/**
 * Test to write and read all the numbers from 1 to 256 in Elias-Gamma coding.
 */
void unsigned_char_eg_test() {
  bitm_array *bma;
  int i;
  
  // given
  bma = bitm_alloc(10000);
  for (i=1; i<=256; i++) {
    bitm_write_eg(bma, i);
  }
  
  bitm_flush(bma);
  
  bitm_reset(bma);

  // when - then
  for (i=1; i<=256; i++) {
    assert_int_equal(bitm_read_eg(bma), i);
  }
  
  // cleanup
  bitm_free(bma);
}
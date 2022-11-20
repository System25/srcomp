/* Bit manipulation functions.

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

#ifndef BITM_H
#define BITM_H

#include <stdlib.h>
#include <string.h>

#define BITS_PER_ELEMENT 32
typedef unsigned int ELEMENT;

/**
 * Bit manipuation array.
 */
typedef struct { 
  size_t length;
  ELEMENT *data;
  size_t index;
  ELEMENT current;
  int bit;
} bitm_array;


/** 
 * Allocates an array of elements. 
 * @param length Length (in elements) of the array to allocate.
 * @return A bitm_array structure.
 */
bitm_array *bitm_alloc(size_t length);

/** 
 * Wraps an array of elements. 
 * @param data Data array to wrap.
 * @param length Length (in elements) of the data array.
 * @return A bitm_array structure. 
 */
bitm_array *bitm_wrap(ELEMENT *data, size_t length);

/** 
 * Resets the internal status of the data array.
 * @param arr The bitm_array to use.
 */
void bitm_reset(bitm_array *arr);

/** 
 * Releases the memory of a bitm_array. 
 * @param arr The bitm_array to release.
 */
void bitm_free(bitm_array *arr);

/** 
 * Writes the last bits into de data. 
 * @param arr The bitm_array to use.
 */
void bitm_flush(bitm_array *arr);

/** 
 * Gets the data array inside the bitm_array structure. 
 * @param arr The bitm_array to use. 
 * @return Data array. 
 */
ELEMENT *bitm_get_data(bitm_array *arr);

/** 
 * Gets the length (in elements) for the data array.
 * @param arr The bitm_array to use. 
 * @return The data length. 
 */
size_t bitm_get_length(bitm_array *arr);

/** 
 * Gets the current index (in elements) for the data array.
 * @param arr The bitm_array to use. 
 * @return Current index. 
 */
size_t bitm_get_index(bitm_array *arr);

/** 
 * Reads the next bit from the data array.
 * @param arr The bitm_array to use. 
 * @return One bit of information. 
 */
int bitm_read_bit(bitm_array *arr);

/** 
 * Writes a bit into the data array.
 * @param arr The bitm_array to use. 
 * @param b The bit to write.  
 */
void bitm_write_bit(bitm_array *arr, int b);

/** 
 * Reads the next N bits from the data array.
 * @param arr The bitm_array to use. 
 * @param n The number of bits. 
 * @return N bits of information (-1 in case of error).
 */
int bitm_read_nbits(bitm_array *arr, int n);

/** 
 * Writes N bits into the data array.
 * @param arr The bitm_array to use. 
 * @param n The number of bits.  
 * @param b The bits to write.  
 */
void bitm_write_nbits(bitm_array *arr, int n, int b);

/** 
 * Reads the next unary coding encoded number from the data array.
 * @see https://en.wikipedia.org/wiki/Unary_coding 
 * @param arr The bitm_array to use. 
 * @return A possitive number (or -1 in case of error). 
 */
int bitm_read_unary(bitm_array *arr);

/** 
 * Writes a possitive number into the data array using unary coding.
 * @see https://en.wikipedia.org/wiki/Unary_coding
 * @param arr The bitm_array to use. 
 * @param n The possitive number to write.    
 */
void bitm_write_unary(bitm_array *arr, int n);

/** 
 * Reads the next elias-gamma encoded number from the data array.
 * @see https://en.wikipedia.org/wiki/Elias_gamma_coding 
 * @param arr The bitm_array to use. 
 * @return A possitive number (or -1 in case of error). 
 */
int bitm_read_eg(bitm_array *arr);

/** 
 * Writes a possitive number into the data array using elias gamma coding.
 * @see https://en.wikipedia.org/wiki/Elias_gamma_coding
 * @param arr The bitm_array to use. 
 * @param n The possitive number to write.    
 */
void bitm_write_eg(bitm_array *arr, int n);

#endif
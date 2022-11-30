/* Simple Real-time compression library.

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

#ifndef SRZ_H
#define SRZ_H

#include <stdlib.h>
#include <stdbool.h>

/** 
 * Compress a data block. 
 * @param src The source array of words (to be compressed).
 * @param dst The destination array of bytes (its size must be a multiple of 4
 *            and it must be bigger than the source array size).
 * @param length The number of words in the source array.
 * @param last_byte Value of the last byte (before second separation). 
 * @param use_previous_byte Use the median value of the previous byte
 *                          to achieve a better sorting.
 * @return The compressed data length (in bytes) or negative number
 *         in case of an error.  
 */
size_t compress_block(unsigned short *src, unsigned char *dst, size_t length,
                    unsigned char *last_byte, bool use_previous_byte);


/* ======================================================================== */
/** 
 * Decompress a data block.
 * @param src The source array of bytes (to be decopressed).
 * @param dst The destination array of words.
 * @param last_word Value of the last word (before separation).
 * @param last_byte Value of the last byte (before second separation).
 * @param length The number of words in the destination array.
 * @param use_previous_byte Use the median value of the previous byte
 *                          to achieve a better sorting.
 * @return The decompressed data length (in bytes) or negative number
 *         in case of an error.   
 */
size_t decompress_block(unsigned char *src, unsigned short *dst,
                      unsigned short last_word, unsigned char last_byte,
                      size_t length, bool use_previous_byte);


#endif
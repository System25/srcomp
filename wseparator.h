/* A function to split the data into 65536 2-byte groups based on 
   the previous 2 bytes.

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

#ifndef WSEPARATOR_H
#define WSEPARATOR_H

#include <stdlib.h>
#include <stdbool.h>

/** 
 * Separate words. 
 * @param src The source array of words (to be separated into groups).
 * @param dst The destination array of words.
 * @param length The number of words in the source array.
 * @param use_previous_byte Use the median value of the previous byte
 *                          to achieve a better sorting.
 */
void separate_words(unsigned short *src, unsigned short *dst, size_t length,
                    bool use_previous_byte);


/** 
 * Join words. (Reverse the separate_words operation).
 * @param src The source array of words (to be joined from groups).
 * @param dst The destination array of words.
 * @param last Value of the last word (before separation).
 * @param length The number of words in the source array.
 * @param use_previous_byte Use the median value of the previous byte
 *                          to achieve a better sorting.
 */
void join_words(unsigned short *src, unsigned short *dst, unsigned short last,
                size_t length, bool use_previous_byte);


#endif
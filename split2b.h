/* A function to split 2-byte data elements (words) into two separated
   groups.

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

#ifndef SDSPLIT_H
#define SDSPLIT_H

#include <stdlib.h>

/** 
 * Separate bytes. 
 * @param src The source array of words (to be separated into groups).
 * @param dst The destination array of bytes.
 * @param length The number of words in the source array.
 */
void separate_bytes(unsigned short *src, unsigned char *dst, int length);

/** 
 * Join bytes. (Reverse the separate_bytes operation).
 * @param src The source array of bytes (to be joined from groups).
 * @param dst The destination array of words.
 * @param last Value of the last byte (before separation).
 * @param length The number of words in the source array.
 */
void join_bytes(unsigned char *src, unsigned short *dst, unsigned char last,
                int length);


#endif
/* Move-To-Front algorithm implementation.

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

#ifndef MTF_H
#define MTF_H

#define BYTE_SYMBOLS 256
#define BYTE_SYMBOLS_1 255

/**
 * MTF data.
 */
typedef struct { 
  unsigned char distances[BYTE_SYMBOLS];
  unsigned char symbols[BYTE_SYMBOLS];
} mtf_status;


/** 
 * Resets the MTF status. 
 * @param mtf_status The status to reset.
 */
void mtf_reset(mtf_status *status);

/** 
 * Codes the source data by using Move-To-Front. 
 * @param src The source array of data.
 * @param dst The destination array of data.
 * @param length The number of bytes in the source array.
 * @param mtf_status The MTF coding status.
 */
void mtf_code(unsigned char *src, unsigned char *dst, int length,
                    mtf_status *status);

/** 
 * Decodes the source data by using Move-ToFront. 
 * @param src The source array of data.
 * @param dst The destination array of data.
 * @param length The number of bytes in the source array.
 * @param mtf_status The MTF coding status.
 */
void mtf_decode(unsigned char *src, unsigned char *dst, int length,
                    mtf_status *status);


#endif
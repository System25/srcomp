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

#include "srz.h"
#include "mtf.h"
#include "bitm.h"
#include "split2b.h"
#include "wseparator.h"

#include <string.h>
#include <stdio.h>

/* ======================================================================== */
/** 
 * Compress a data block. 
 * @param src The source array of words (to be compressed).
 * @param dst The destination array of bytes (its size must be double the
 *            source array size).
 * @param length The number of words in the source array.
 * @param last_byte Value of the last byte (before second separation). 
 * @param use_previous_byte Use the median value of the previous byte
 *                          to achieve a better sorting.
 * @return The compressed data length (in bytes) or negative number
 *         in case of an error.  
 */
size_t compress_block(unsigned short *src, unsigned char *dst, size_t length,
                    unsigned char *last_byte, bool use_previous_byte) {
  bitm_array *bitma;
  size_t compressed_length;
  mtf_status status;
  unsigned short *tmp0, *tmp1;
  size_t l, i;
  
  // Allocate resources
  tmp0 = (unsigned short *) malloc(length<<1);
  if (tmp0 == NULL) {
    perror("Error allocating memory\n");
    return -1;
  }

  tmp1 = (unsigned short *) malloc(length<<1);
  if (tmp1 == NULL) {
    perror("Error allocating memory\n");
    free(tmp0);
    return -1;
  }  
  
  // destination array size = 2 x source array size
  l = (length>>1);
  bitma = bitm_wrap((ELEMENT *)dst, l); 
  
  mtf_reset(&status);
  
  // Separate words
  separate_words(src, tmp0, length, use_previous_byte);
  *last_byte = ((unsigned char *) tmp0)[(length<<1) - 1];
  
  // Separate bytes
  separate_bytes(tmp0, (unsigned char *) tmp1, length);
  
  // MTF coding
  mtf_code((unsigned char *) tmp1, (unsigned char *) tmp0, (length << 1),
           &status);
  
  // Elias-Gamma coding
  bitm_reset(bitma);
    
  for (i = 0; i<(length<<1); i++) {
    bitm_write_eg(bitma, ((unsigned char *)tmp0)[i] + 1);
  }
    
  bitm_flush(bitma);
  compressed_length = bitm_get_index(bitma);  
  
  // Free resources
  free(tmp0);
  free(tmp1);
  bitm_unwrap(bitma);
  
  return (compressed_length<<2);
}


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
                      size_t length, bool use_previous_byte) {

  bitm_array *bitma;
  mtf_status status;
  unsigned short *tmp0, *tmp1;
  size_t l, i;
  
  // Allocate resources
  tmp0 = (unsigned short *) malloc(length<<1);
  if (tmp0 == NULL) {
    perror("Error allocating memory\n");
    return -1;
  }

  tmp1 = (unsigned short *) malloc(length<<1);
  if (tmp1 == NULL) {
    perror("Error allocating memory\n");
    free(tmp0);
    return -1;
  }  
  
  // source array size = 2 x destination array size
  l = (length>>1);
  bitma = bitm_wrap((ELEMENT *)src, l); 
  
  mtf_reset(&status);

  // Read Elias-Gamma data
  for (i = 0; i<(length<<1); i++) {
    ((unsigned char *)tmp0)[i] = (unsigned char) (bitm_read_eg(bitma) - 1);
  }
  
  // MTF decode
  mtf_decode((unsigned char *) tmp0, (unsigned char *) tmp1, (length<<1),
             &status);
  
  // Join bytes
  join_bytes((unsigned char *) tmp1, tmp0, last_byte, length);
  
  // Join words
  join_words(tmp0, dst, last_word, length, use_previous_byte);
  
  // Free resources
  free(tmp0);
  free(tmp1);
  bitm_unwrap(bitma);

  
  return (length<<1);
}
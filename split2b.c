/* A function to split 2-byte data elements (words) into two separated.

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

#include "split2b.h"

#include <stdio.h>
#include <string.h>

//#define DEBUG
//#define DEBUG_COUNTS
//#define DEBUG_INDEX

/* number of symbols */
#define NSYMBOLS 256

/* ======================================================================== */
/** 
 * Count bytes in an array. 
 * @param src The source data whose bytes will be counted.
 * @param len The number of bytes in the source data.
 * @param count An array of 256*sizeof(int) bytes that will
 *              contain the counts of the higher byte.
 */
void count_bytes(unsigned char *src, int len, int *count) {
  int i;

  memset(count, 0, NSYMBOLS*sizeof(int));

  for (i = 0; i < len; i++) {  
    count[ src[i] ]++;
  }
  
#ifdef DEBUG_COUNTS
  for (i = 0; i < NSYMBOLS; i++) {
    fprintf(stdout, "Count[%i]=%i\n", i, count[i]);
  }
#endif
  
}

/* ======================================================================== */
/** 
 * Count bytes in an array. 
 * @param src The source data whose bytes will be counted.
 * @param len The number of words in the source data.
 * @param count An array of 256*sizeof(int) bytes that will
 *              contain the counts of the higher byte.
 */
void count_word_bytes(unsigned short *src, int len, int *count) {
  int i;
  unsigned char *src_h;

  memset(count, 0, NSYMBOLS*sizeof(int));
  src_h = (unsigned char *) src;

  for (i = 0; i < len; i++) {
    count[ *src_h ]++;
    src_h += 2;
  }
    
#ifdef DEBUG_COUNTS
  for (i = 0; i < NSYMBOLS; i++) {
    fprintf(stdout, "Count[%i]=%i\n", i, count[i]);
  }
#endif  
  
}

/* ======================================================================== */
/** 
 * Calculate the indexes of the destination array. 
 * @param count An array of 256*sizeof(int) bytes that will
 *              contain the counts.
 * @param index An array of 256*sizeof(int) bytes that will
 *              contain the indexes. 
 */
void calculate_byte_indexes(int *count, int *index) {
  int i;

  index[0] = 0;
  for (i = 1; i < NSYMBOLS; i++) {
    index[i] = index[i-1] + count[i-1];
  }
	
#ifdef DEBUG_INDEX
  for (i = 0; i < NSYMBOLS; i++) {
    fprintf(stdout, "Index[%i] = %i\n", i, index[i]);
  }
#endif	
}

/* ======================================================================== */
/** 
 * Separate bytes. 
 * @param src The source array of words (to be separated into groups).
 * @param dst The destination array of bytes.
 * @param length The number of words in the source array.
 */
void separate_bytes(unsigned short *src, unsigned char *dst, int length) {
  long i;
  unsigned char current_h, current_l;
  int count_h[NSYMBOLS];
  int index_h[NSYMBOLS];
  unsigned char *dst_h;
  unsigned char *dst_l;
  unsigned char *src_h;
  unsigned char *src_l;  

  dst_h = dst;
  dst_l = dst + length;  
  src_h = (unsigned char *) src;
  src_l = ((unsigned char *) src) + 1;  
  
  // Count the bytes
  count_word_bytes(src, length, count_h);
  
  // Calculate the indexes
  calculate_byte_indexes(count_h, index_h);
  
  // Separate the bytes 
  for (i = 0; i < length; i++) {
    current_h = *src_h;
    current_l = *src_l;
    src_h += 2;
    src_l += 2;
 
#ifdef DEBUG
  fprintf(stdout, "dst_h[%06x] = %i\n", i, current_h);
  fprintf(stdout, "dst_l[%06x] = %i\n", index_h[current_h], current_l);
#endif	    
    dst_h[i] = current_h;
    dst_l[index_h[current_h]++] = current_l;
  }

}

/* ======================================================================== */
/** 
 * Join bytes. (Reverse the separate_bytes operation).
 * @param src The source array of bytes (to be joined from groups).
 * @param dst The destination array of words.
 * @param last Value of the last byte (before separation).
 * @param length The number of words in the source array.
 */
void join_bytes(unsigned char *src, unsigned short *dst, unsigned char last,
                int length) {
  long i;
  unsigned char current_h, current_l;
  int count_h[NSYMBOLS];
  int index_h[NSYMBOLS];
  unsigned char *dst_h;
  unsigned char *dst_l;
  unsigned char *src_h;
  unsigned char *src_l;  
  
  // Count the bytes
  count_bytes(src, length, count_h);  
  
  // Calculate the indexes
  calculate_byte_indexes(count_h, index_h);

  // join the bytes
  dst_h = (unsigned char *) dst;
  dst_l = ((unsigned char *) dst) + 1;  
  src_h = src;
  src_l = src + length;
  

  for (i = 0; i < length; i++) {
    current_h = src_h[i];
    current_l = src_l[index_h[current_h]++];
    
#ifdef DEBUG
  fprintf(stdout, "dst_h[%06x] = %i\n", i, current_h);
  fprintf(stdout, "dst_l[%06x] = %i\n", index_h[current_h]-1, current_l);
#endif

    *dst_h = current_h;
    *dst_l = current_l;
    dst_h += 2;
    dst_l += 2;
  }

}

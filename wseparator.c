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

#include "wseparator.h"

#include <stdio.h>
#include <string.h>



/* number of symbols */
#define NSYMBOLS 65536

/* Median value of the byte previous to each symbol. */
unsigned char median[NSYMBOLS];

/* ======================================================================== */
/** 
 * Count words in an array. 
 * @param src The source data whose words will be counted.
 * @param len The number of workds in the source data.
 * @param count An array of 65536*sizeof(int) bytes that will
 *              contain the counts.
 */
void count_words(unsigned short *src, int len, int *count) {
  int i;

  memset(count, 0, NSYMBOLS*sizeof(int));

  for (i = 0; i < len; i++) {
    count[ src[i] ]++;
  }
  
#ifdef DEBUG_COUNTS
  for (i = 0; i < NSYMBOLS; i++) {
    fprintf(stdout, "Count[%i] = %li\n", i, count[i]);
  }
#endif
  
}

/* ======================================================================== */
/** 
 * Calculate the indexes of the destination array. 
 * @param count An array of 65536*sizeof(int) bytes that will
 *              contain the counts.
 * @param index An array of 65536*sizeof(int) bytes that will
 *              contain the indexes. 
 */
void calculate_indexes(int *count, int *index) {
  int i;

  index[0] = 0;
  for (i = 1; i < NSYMBOLS; i++) {
    index[i] = index[i-1] + count[i-1];
  }
	
#ifdef DEBUG_INDEX
  for (i = 0; i < NSYMBOLS; i++) {
    fprintf(stdout, "Index[%i] = %li\n", i, index[i]);
  }
#endif	
}

/* ======================================================================== */
/** 
 * Calculate the last indexes of the destination array. 
 * @param count An array of 65536*sizeof(int) bytes that will
 *              contain the counts.
 * @param lindex An array of 65536*sizeof(int) bytes that will
 *              contain the indexes. 
 * @param length The number of words in the source array.
 */
void calculate_last_indexes(int *count, int *lindex, int length) {
  int i;

  lindex[NSYMBOLS-1] = length-1;
  for (i=NSYMBOLS-2; i>=0; i--) {
    lindex[i] = lindex[i+1] - count[i+1];
  }

#ifdef DEBUG_INDEX
  for (i = 0; i < NSYMBOLS; i++) {
    fprintf(stdout, "Last Index[%i] = %li\n", i, lindex[i]);
  }
#endif  
}


/* ======================================================================== */
/** 
 * Separate words. 
 * @param src The source array of words (to be separated into groups).
 * @param dst The destination array of words.
 * @param length The number of words in the source array.
 * @param use_previous_byte Use the median value of the previous byte
 *                          to achieve a better sorting.
 */
void separate_words(unsigned short *src, unsigned short *dst, int length,
                    bool use_previous_byte) {
  long i;
  unsigned short previous;
  unsigned short current;
  int count[NSYMBOLS];
  int index[NSYMBOLS];
  int lindex[NSYMBOLS];
  int prev_byte;

  if (use_previous_byte) {
      memset(median, 127, NSYMBOLS*sizeof(unsigned char));
  }
  
  // Count the words
  count_words(src, length, count);
  // (There is no word after last!)
  count[src[length -1]]--;
  count[0]++;  
  
  
  // Calculate the indexes
  calculate_indexes(count, index);
  
  if (use_previous_byte) {
	calculate_last_indexes(count, lindex, length);
  } 

  // Separate the bytes
  previous = 0;
  prev_byte = 0; 
  for (i = 0; i < length; i++) {
    current = src[i];

    if (use_previous_byte) {
#ifdef DEBUG
      fprintf(stdout, "median[previous]=%i)\n", median[previous]);
#endif    
      if (median[previous] >= prev_byte) {
#ifdef DEBUG
        fprintf(stdout, "dst[%li]=%i (previous=%i)\n", index[previous],
            current, previous);
#endif
        dst[ index[previous]++ ] = current;
      }
      else {
        dst[ lindex[previous]-- ] = current;
      }
	} 
    else {   
#ifdef DEBUG
      fprintf(stdout, "dst[%li]=%i (previous=%i)\n", index[previous],
          current, previous);
#endif
      dst[ index[previous]++ ] = current;
    }   
    
    if (use_previous_byte) {   
      if (median[previous] < prev_byte) {
        median[previous]++;
      }
      else if (median[previous] > prev_byte) {
        median[previous]--;
      }
      prev_byte = (previous & 0xFF);
    }
    previous = current;
  }

}

/* ======================================================================== */
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
                int length, bool use_previous_byte) {
  long i;
  unsigned short previous;
  unsigned short current;
  int count[NSYMBOLS];
  int index[NSYMBOLS];
  int lindex[NSYMBOLS];
  int prev_byte; 

  if (use_previous_byte) {
    memset(median, 127, NSYMBOLS*sizeof(unsigned char));
  }    
  
  // Count the words
  count_words(src, length, count);
  // (There is no word after last!)
  count[last]--;
  count[0]++;
  
  // Calculate the indexes
  calculate_indexes(count, index);
  
  if (use_previous_byte) {
	calculate_last_indexes(count, lindex, length);
  } 
  
  // join the words    
  previous = 0;
  prev_byte = 0; 
  for (i = 0; i < length; i++) {
    
    if (use_previous_byte) {
#ifdef DEBUG
      fprintf(stdout, "median[previous]=%i)\n", median[previous]);
#endif    
      if (median[previous] >= prev_byte) {
#ifdef DEBUG
        fprintf(stdout, "dst[%li]=%i (previous=%i)\n", index[previous],
                src[index[previous]], previous);
#endif
        current = src[ index[previous]++ ];
      }
      else {
        current = src[ lindex[previous]-- ];
      }
    }
    else {
#ifdef DEBUG
      fprintf(stdout, "dst[%li]=%i (previous=%i)\n", index[previous],
              src[index[previous]], previous);
#endif
      current = src[ index[previous]++ ];
    }    
    
    if (use_previous_byte) {    
      if (median[previous] < prev_byte) {
        median[previous]++;
      }
      else if (median[previous] > prev_byte) {
        median[previous]--;
      }
      prev_byte = (previous & 0xFF);
    }

    dst[i] = current;
    previous = current;
  }

}

/* Bit manipulation functions implementation.

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

#include "bitm.h"

#define USE_FAST_EG

#ifdef USE_FAST_EG
#include "fast_eg.h"
#endif

#include <stdio.h>

unsigned int mask[] = { 0x00000000,
  0x00000001, 0x00000003, 0x00000007, 0x0000000F,
  0x0000001F, 0x0000003F, 0x0000007F, 0x000000FF,
  0x000001FF, 0x000003FF, 0x000007FF, 0x00000FFF,
  0x00001FFF, 0x00003FFF, 0x00007FFF, 0x0000FFFF,
  0x0001FFFF, 0x0003FFFF, 0x0007FFFF, 0x000FFFFF,
  0x001FFFFF, 0x003FFFFF, 0x007FFFFF, 0x00FFFFFF,
  0x01FFFFFF, 0x03FFFFFF, 0x07FFFFFF, 0x0FFFFFFF,
  0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF
};

/* ======================================================================== */
/** 
 * Allocates an array of elements. 
 * @param length Length (in elements) of the array to allocate.
 * @return A bitm_array structure.
 */
bitm_array *bitm_alloc(size_t length) {
  if (length == 0) {
    return NULL;
  }
  
  ELEMENT *data = (ELEMENT *) malloc(sizeof(ELEMENT) * length);
  
  return bitm_wrap(data, length);
}

/* ======================================================================== */
/** 
 * Wraps an array of elements. 
 * @param data Data array to wrap.
 * @param length Length (in elements) of the data array.
 * @return A bitm_array structure. 
 */
bitm_array *bitm_wrap(ELEMENT *data, size_t length) {
  if (length == 0 || data == NULL) {
    return NULL;
  }
  
  bitm_array *arr = (bitm_array *) malloc(sizeof(bitm_array));
  if (arr == NULL) {
    return NULL;
  }  
  memset(arr, 0, sizeof(bitm_array));
  
  arr->data = data;
  arr->length = length;  
  
  return arr;
}

/* ======================================================================== */
/** 
 * Resets the internal status of the data array.
 * @param arr The bitm_array to use.
 */
void bitm_reset(bitm_array *arr) {
  if (arr == NULL) {
    return;
  }
  
  arr->index = 0;
  arr->current = 0;
  arr->bit = 0;
}

/* ======================================================================== */
/** 
 * Releases the memory of a bitm_array. 
 * @param arr The bitm_array to release.
 */
void bitm_free(bitm_array *arr) {
  if (arr == NULL) {
    return;
  }
  
  if (arr->data != NULL) {
    free(arr->data);
  }
  
  free(arr);
}

/* ======================================================================== */
/** 
 * Writes the last bits into de data. 
 * @param arr The bitm_array to use.
 */
void bitm_flush(bitm_array *arr) {
  if (arr == NULL) {
    return;
  }
  
  arr->data[arr->index++] = (arr->current << (BITS_PER_ELEMENT - arr->bit));
  arr->current = 0;
  arr->bit = 0;
}

/* ======================================================================== */
/** 
 * Gets the data array inside the bitm_array structure. 
 * @param arr The bitm_array to use. 
 * @return Data array. 
 */
ELEMENT *bitm_get_data(bitm_array *arr) {
  if (arr == NULL) {
    return NULL;
  }
  
  return arr->data;
}

/* ======================================================================== */
/** 
 * Gets the length (in elements) for the data array.
 * @param arr The bitm_array to use. 
 * @return The data length. 
 */
size_t bitm_get_length(bitm_array *arr) {
  if (arr == NULL) {
    return 0;
  }
  
  return arr->length;
}

/* ======================================================================== */
/** 
 * Gets the current index (in elements) for the data array.
 * @param arr The bitm_array to use. 
 * @return Current index. 
 */
size_t bitm_get_index(bitm_array *arr) {
  if (arr == NULL) {
    return 0;
  }
  
  return arr->index;
}

/* ======================================================================== */
/** 
 * Reads the next bit from the data array.
 * @param arr The bitm_array to use. 
 * @return One bit of information. 
 */
int bitm_read_bit(bitm_array *arr) {
  if (arr == NULL || arr->data == NULL) {
    return -1;
  }  
  
  if (arr->bit == 0) {
    arr->current = arr->data[arr->index++];
    arr->bit = BITS_PER_ELEMENT;
  }
  
  return (arr->current >> (--(arr->bit))) & 1;
}

/* ======================================================================== */
/** 
 * Writes a bit into the data array.
 * @param arr The bitm_array to use. 
 * @param b The bit to write.  
 */
void bitm_write_bit(bitm_array *arr, int b) {
  if (arr == NULL || arr->data == NULL) {
    return;
  }
  
  if (arr->bit == BITS_PER_ELEMENT) {
    arr->data[arr->index++] = arr->current;
    arr->bit = 0;
    arr->current = 0;
  }  
  
  arr->current = ((arr->current<<1) | (b & 1));
  arr->bit++;
}

/* ======================================================================== */
/** 
 * Reads the next N bits from the data array.
 * @param arr The bitm_array to use. 
 * @param n The number of bits. 
 * @return N bits of information (-1 in case of error). 
 */
int bitm_read_nbits(bitm_array *arr, int n) {
  int b = 0;
  if (arr == NULL || arr->data == NULL || n <= 0 || n > BITS_PER_ELEMENT) {
    return -1;
  }  
  
  if (arr->bit == 0) {
    arr->current = arr->data[arr->index++];
    arr->bit = BITS_PER_ELEMENT;
  }
  
  if (n > arr->bit) {
    b = (arr->current & mask[arr->bit]);
    n -= arr->bit;

    arr->current = arr->data[arr->index++];
    arr->bit = BITS_PER_ELEMENT;
  }
  
  arr->bit -= n;
  b = ((b<<n) | ((arr->current >> arr->bit) & mask[n]));
  return b;
}

/* ======================================================================== */
/** 
 * Writes N bits into the data array.
 * @param arr The bitm_array to use. 
 * @param n The number of bits.  
 * @param b The bits to write.  
 */
void bitm_write_nbits(bitm_array *arr, int n, int b) {
  int l;
  if (arr == NULL || arr->data == NULL || n <= 0 || n > BITS_PER_ELEMENT) {
    return;
  }
  
  if (arr->bit == BITS_PER_ELEMENT) {
    arr->data[arr->index++] = arr->current;
    arr->bit = 0;
    arr->current = 0;
  }  
  
  l = (BITS_PER_ELEMENT - arr->bit);
  if (n > l) {
    n -= l;
    arr->current = ((arr->current<<l) | (b>>n));
      
    arr->data[arr->index++] = arr->current;
    arr->bit = 0;
    arr->current = 0;
  }

  arr->current = ((arr->current<<n) | (b & mask[n]));
  arr->bit += n;
}

/** 
 * Reads the next unary coding encoded number from the data array.
 * @see https://en.wikipedia.org/wiki/Unary_coding 
 * @param arr The bitm_array to use. 
 * @return A possitive number (or -1 in case of error). 
 */
int bitm_read_unary(bitm_array *arr) {
  int n, d;
  
  n = 0;
  d = bitm_read_bit(arr);
  if (d<0) return -1;
  
  while (d != 1) {
    n++;
    d = bitm_read_bit(arr);
    if (d<0) return -1;
  }

  return n;
}

/** 
 * Writes a possitive number into the data array using unary coding.
 * @see https://en.wikipedia.org/wiki/Unary_coding
 * @param arr The bitm_array to use. 
 * @param n The possitive number to write.    
 */
void bitm_write_unary(bitm_array *arr, int n) {
  int l;
  
  for (l = 0; l<n; l++) {
    bitm_write_bit(arr, 0);
  }

  bitm_write_bit(arr, 1);
}

/** 
 * Reads the next elias-gamma encoded number from the data array.
 * @see https://en.wikipedia.org/wiki/Elias_gamma_coding 
 * @param arr The bitm_array to use. 
 * @return A possitive number (or -1 in case of error). 
 */
int bitm_read_eg(bitm_array *arr) {
  int e, r;  

  /* Read the exponent in unary code */  
  e = bitm_read_unary(arr);
  if (e < 0) return -1;
  r = 0;
  if (e>0) {
    /* Read the rest of the binary digits */
    r = bitm_read_nbits(arr, e);
    if (r < 0) return -1;
  }
  
  return (1<<e) | r;
}

#ifdef USE_FAST_EG
/** 
 * Writes a possitive number into the data array using elias gamma coding.
 * @see https://en.wikipedia.org/wiki/Elias_gamma_coding
 * @param arr The bitm_array to use. 
 * @param n The possitive number to write.    
 */
void bitm_write_eg(bitm_array *arr, int n) {
  if (n <=0) {
    return;
  }
  
  bitm_write_nbits(arr, fastEliasGamma[n-1][1], fastEliasGamma[n-1][0]);
}

#else

/** 
 * Writes a possitive number into the data array using elias gamma coding.
 * @see https://en.wikipedia.org/wiki/Elias_gamma_coding
 * @param arr The bitm_array to use. 
 * @param n The possitive number to write.    
 */
void bitm_write_eg(bitm_array *arr, int n) {
  int e, r, i, m;
  if (n <=0) {
    return;
  }
  
 /* Calulate exponent and the remaining binary digits */
 e = 0;
 i = n;
 m = 0;
 while (i > 0) {
   e++;
   i = (i>>1);
   m = ((m<<1) | 1);
 }
 e--;
 m = (m>>1);
  
 
 r = (n & m);
  
 /* Write que exponent in unary code */
 bitm_write_unary(arr, e);
  
 if (e>0) {
   /* Write the remaining binary digits */
   bitm_write_nbits(arr, e, r);
 }
}

#endif

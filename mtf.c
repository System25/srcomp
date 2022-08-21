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

#include "mtf.h"

#include <string.h>

/* ======================================================================== */
/** 
 * Resets the MTF status. 
 * @param mtf_status The status to reset.
 */
void mtf_reset(mtf_status *status) {
  int i;
  
  for (i = 0; i<BYTE_SYMBOLS; i++) {
    status->distances[i] = i;
    status->symbols[i] = BYTE_SYMBOLS_1-i;
  }
}

/* ======================================================================== */
/** 
 * Moves to front a character from a position.
 * @param position Position of the character in the MTF status.
 * @param mtf_status The MTF coding status. 
 */
void _mtf_move_to_front_pos(int position, mtf_status *status) {
  int c, i;
  unsigned char *symbols;
  
  if (position == BYTE_SYMBOLS_1) {
    return;
  }
  
  symbols = status->symbols;
  c = symbols[position];
  
  for (i = position; i<BYTE_SYMBOLS_1; i++) {
    symbols[i] = symbols[i+1];
  }
  
  symbols[BYTE_SYMBOLS_1] = c;
}

/* ======================================================================== */
/** 
 * Moves to front a character.
 * @param c Character to move to front.
 * @param mtf_status The MTF coding status.
 */
#ifdef __OPTIMIZE__
void _mtf_move_to_front(int c, mtf_status *status) {
  int d, i;
  unsigned char *distances;
  
  distances = status->distances;
  d = distances[c];
  if (d==0) {
    return;
  }
  
  for (i = 0; i<BYTE_SYMBOLS; i++) {
    distances[i] += ((distances[i] < d) ? 1 : 0);
  }
  
  distances[c] = 0;
}
#endif

/* ======================================================================== */
/**
 * Calculates MTF position of a character. 
 * @param c Character to move to front.
 * @param mtf_status The MTF coding status.
 * @return Move-To-Front position.
 */
#ifdef __OPTIMIZE__
/* Optimized version */
int _mtf_get_position(int c, mtf_status *status) {
  int pos; 
  
  pos = status->distances[c];
  
  _mtf_move_to_front(c, status);
  
  return pos;
}
#else
/* Base version */
int _mtf_get_position(int c, mtf_status *status) {
  int pos;
  
  /* Get current character position */
  pos = BYTE_SYMBOLS_1;
  while (pos>=0 && status->symbols[pos] != c) {
    pos--;
  }
  
  _mtf_move_to_front_pos(pos, status);
  
  return BYTE_SYMBOLS_1 - pos;
}
#endif




/* ======================================================================== */
/** 
 * Codes the source data by using Move-To-Front. 
 * @param src The source array of data.
 * @param dst The destination array of data.
 * @param length The number of bytes in the source array.
 * @param mtf_status The MTF coding status.
 */
void mtf_code(unsigned char *src, unsigned char *dst, int length,
                    mtf_status *status) {
  int i;
  
  for (i=0; i<length; i++) {
    dst[i] = _mtf_get_position(src[i], status);
  }
  
}

/* ======================================================================== */
/** 
 * Decodes the source data by using Move-ToFront. 
 * @param src The source array of data.
 * @param dst The destination array of data.
 * @param length The number of bytes in the source array.
 * @param mtf_status The MTF coding status.
 */
void mtf_decode(unsigned char *src, unsigned char *dst, int length,
                    mtf_status *status) {
  int i, p;
  
  for (i=0; i<length; i++) {
    p = BYTE_SYMBOLS_1 - src[i];
    dst[i] = status->symbols[p];
    _mtf_move_to_front_pos(p, status);
  }  
}
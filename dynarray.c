/* Dynamically growing byte array.

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

#include <stdio.h>
#include <stdlib.h>
#include "dynarray.h"

/* ======================================================================== */
/**
 * Creates an empty dynamic array.
 */
dynarray *dynarray_new() {
  dynarray *d;
  
  if ( (d = (dynarray *) malloc(sizeof(dynarray))) == NULL) {
    perror("Error allocating memory: ");
    return NULL;
  }

  d->length = 0;
  d->next = NULL;
  d->last = NULL;
  
  return d;
}

/* ======================================================================== */
/**
 * Deletes a dynamic array.
 * @param d Dynamic array to delete.
 */
void dynarray_delete(dynarray *d) {
  if (d == NULL) {
    return;
  }
  
  if (d->next != NULL) {
    dynarray_delete(d->next);
  }
  
  free(d);
}

/* ======================================================================== */
/**
 * Appends a byte to a dynamic array.
 * @param d Dynamic array.
 * @param b byte to add.
 */
void dynarray_append_byte(dynarray *d, unsigned char b) {
  dynarray *base;
  if (d == NULL) {
    fprintf(stderr, "ERROR: Null dynamic array!\n");
    return;
  }
  
  base = d;
  if (d->last != NULL) {
    d = d->last;
  }
  
  if (d->length < DYNARRAY_BUF_LEN) {
    d->buffer[d->length++] = b;
  }
  else {
    d->next = dynarray_new();
    base->last = d->next;
    d = d->next;
    d->buffer[0] = b;
    d->length = 1;
  }
  
}

/* ======================================================================== */
/**
 * Returns the length of a dynamic array.
 * @param d Dynamic array.
 * @return Length of the dynamic array.
 */
unsigned int dynarray_get_length(dynarray *d) {
  unsigned int l;
  if (d == NULL) {
    return 0;
  }
  
  l = d->length;
  while (d->next != NULL) {
    d = d->next;
    l += d->length;
  }
  
  return l;
}


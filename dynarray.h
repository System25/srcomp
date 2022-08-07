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
 
#ifndef _DYNARRAY_
#define _DYNARRAY_

#define DYNARRAY_BUF_LEN 1024

/**
 * Dynamic array.
 */
typedef struct {
    unsigned char buffer[DYNARRAY_BUF_LEN];
    unsigned int length;
    void *next;
    void *last;
} dynarray;

/**
 * Creates an empty dynamic array.
 */
dynarray *dynarray_new();

/**
 * Deletes a dynamic array.
 * @param d Dynamic array to delete.
 */
void dynarray_delete(dynarray *d);

/**
 * Appends a byte to a dynamic array.
 * @param d Dynamic array.
 * @param b byte to add.
 */
void dynarray_append_byte(dynarray *d, unsigned char b);

/**
 * Returns the length of a dynamic array.
 * @param d Dynamic array.
 * @return Length of the dynamic array.
 */
unsigned int dynarray_get_length(dynarray *d);


#endif

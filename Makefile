# Make srcomp (Simple realtime text files compressor).

# Copyright (C) 2022 Abraham Macias Paredes

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

ALL: srcomp

DEPENDENCIES=\
	wseparator.o\
	wseparator.h\
#	split2b.o\
#	split2b.h\
        dynarray.h\
	dynarray.o

CFLAGS=-Wall -pg
LDFLAGS=

srcomp: srcomp.c $(DEPENDENCIES) -liberty

clean:
	rm -f *.o
	rm -f srcomp



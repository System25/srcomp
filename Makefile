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

SOURCES=\
	wseparator.o\
	wseparator.h\
	split2b.o\
	split2b.h\
	bitm.o\
	bitm.h\
	mtf.o\
	mtf.h

TESTS=\
	test_bitm.o\
	test_bitm.h
	


#CFLAGS=-Wall -pg -O3
CFLAGS=-O3
LDFLAGS=

srcomp: srcomp.c $(SOURCES) -liberty

tests: $(SOURCES) $(TESTS) tests.c -lcmocka

test: tests
	./tests

docs:
	doxygen doxygen.conf

clean:
	rm -f *.o
	rm -f tests
	rm -f srcomp
	rm -rf doc



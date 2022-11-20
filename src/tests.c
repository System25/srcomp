/* CMocka test suite.

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


#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "test_bitm.h"


/* ======================================================================== */
/** 
 * Runs CMocka tests.
 * @return 0 in case of success. 
 */
int main(void) {
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(alloc_test),
    cmocka_unit_test(write_bit_test),
    cmocka_unit_test(read_bit_test),
    cmocka_unit_test(read_nbit_test),
    cmocka_unit_test(read_nbit_between_elements_test),
    cmocka_unit_test(write_nbit_test),
    cmocka_unit_test(write_nbit_between_elements_test),
    cmocka_unit_test(read_unary_test),
    cmocka_unit_test(read_eg_test),
    cmocka_unit_test(write_eg_test),
    cmocka_unit_test(reset_test),
    cmocka_unit_test(unsigned_char_unary_test),
    cmocka_unit_test(unsigned_char_eg_test),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}


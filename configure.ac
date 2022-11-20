AC_INIT([srcomp], [1.0])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CC
AC_PROG_RANLIB
AM_PROG_AR

AC_CHECK_LIB(iberty, xcrc32, [], [
        echo "Error! Please install libiberty."
        exit -1
        ])


AC_CHECK_LIB(cmocka, _cmocka_run_group_tests, [], [
        echo "Error! Please install libcmocka."
        exit -1
        ])

AC_CONFIG_FILES([
 Makefile
 src/Makefile
 tests/Makefile
])

AC_OUTPUT
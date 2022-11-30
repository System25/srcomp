noinst_LIBRARIES = libsrz.a
libsrz_a_SOURCES = \
	srz.c\
	srz.h\
	wseparator.c\
	wseparator.h\
	split2b.c\
	split2b.h\
	bitm.c\
	bitm.h\
	mtf.c\
	mtf.h
libsrz_a_CPPFLAGS = -Wall -O3	
	
bin_PROGRAMS = srcomp
srcomp_SOURCES = srcomp.c
srcomp_CPPFLAGS = -Wall -O3
srcomp_LDADD = libsrz.a $(LIBOBJS)

# srcomp
Simple realtime text files compressor


# Build the software
In order to build the software you will need libtool, autoconf and automake (and
of course GCC). Also you will need libiberty and cmocka (for the unit tests).

So the building process is:
```
aclocal
autoconf
automake

./configure
make
```

After a successful build you can run the unit tests:
```
tests/tests
```

# srcomp
Simple realtime text files compressor


# Build the software
In order to build the software you will need libtool, autoconf and automake (and
of course GCC). Also you will need libiberty and cmocka (for the unit tests).

So the building process is:
```
aclocal
autoconf
automake --add-missing

./configure
make
```

After a successful build you can run the unit tests:
```
tests/tests
```

# Run the software
The software can run in the command line with the following options:
```
USAGE: srcomp [c|d] -i <input_file> -o <output_file> 
 -h           print this message.
 -c           compress.
 -d           decompress.
 -p           use previous data to compress more.
 -i <file>    specify the input file.
 -o <file>    specify the output file.
 -b <size>    specify the block size (in kilobytes).
```

## Compressing a file
You can compress a file by running it like:
```
./srcomp -c -i ../../enwik8 -o enwik8.srz
```

Or alternatively, you can compress the file by using stdin
and stdout:
```
./srcomp -c -i ../../enwik8 > enwik8.srz
```

In compression mode, you can choose how big do you want the block size to be
(up to 64 megabytes). The bigger the block size the better the compression.
```
# Example with 1Mb block size
./srcomp -c -b 1024 -i ../../enwik8 -o enwik8.srz
```

You can also use the previous data to compress a little bit more.
```
# Example with 1Mb block size and previous data usage
./srcomp -c -p -b 1024 -i ../../enwik8 -o enwik8.srz
```

## Decompressing a file
You can compress a file by running it like:
```
./srcomp -d -i enwik8.srz -o enwik8.txt
```

Or alternatively, you can compress the file by using stdin
and stdout:
```
cat enwik8.srz | ./srcomp -d > enwik8.txt
```




# srcomp
**S**imple **r**eal-time text files **comp**ressor.

This is an experimental text files compressor that is simple enough to be implemented by hardware.

It is not the best compressor but it has an acceptable compression ratio and is real-time.
Using a 1Mb block size it has a similar compression ratio to [LZOP](https://www.lzop.org/) (based in [LZO algorithm](https://en.wikipedia.org/wiki/Lempel%E2%80%93Ziv%E2%80%93Oberhumer)) for text files.

# The compression algorithm
The compression algorithm is a simplification of the Bzip2 compression.

Bzip2 compression uses the [Burrows-Wheeler Transform](https://en.wikipedia.org/wiki/Burrows%E2%80%93Wheeler_transform) followed by the [Move-To-Front transform](https://en.wikipedia.org/wiki/Move-to-front_transform) and after that compresses the information by using [Huffman-coding](https://en.wikipedia.org/wiki/Huffman_coding).

Srcomp uses a simple 16-bits word sorting (every word is sorted according to the previous one) followed by a byte sorting (the lower byte of every 16-bits word is sorted according to the upper byte) to replace the Burrows-Wheeler Transform.

This replacement is the main cause of losing compression ratio but gaining compression speed. And it is so simple that it can be implemented by hardware.

After this first stage, the Move-To-Front transform is used because, although slow in software implementations, it can be implemented by hardware very easily (hardware implementations are real-time). And last but not least, the Huffman coding is replaced by the [Elias gamma coding](https://en.wikipedia.org/wiki/Elias_gamma_coding).

So, every step of the compression algorithm is real-time and simple enough to be implemented by hardware.

One of the advantages over other real-time compressors is that the compression speed is slightly faster than the decompression speed.


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

After a successful build you can run the unit tests if you want:
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
./srcomp -c -i enwik8 -o enwik8.srz
```

Or alternatively, you can compress the file by using stdin
and stdout:
```
cat enwik8 | ./srcomp -c > enwik8.srz
```

In compression mode, you can choose how big do you want the block size to be
(up to 64 megabytes). The bigger the block size the better the compression.
```
# Example with 1Mb block size
./srcomp -c -b 1024 -i enwik8 -o enwik8.srz
```

You can also use the previous data to compress a little bit more.
```
# Example with 1Mb block size and previous data usage
./srcomp -c -p -b 1024 -i enwik8 -o enwik8.srz
```

## Decompressing a file
You can decompress a file by running it like:
```
./srcomp -d -i enwik8.srz -o enwik8.txt
```

Or alternatively, you can compress the file by using stdin
and stdout:
```
cat enwik8.srz | ./srcomp -d > enwik8.txt
```




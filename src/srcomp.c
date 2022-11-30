/* srcomp. A simple real-time compressor.

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

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <libiberty/libiberty.h>

#include "srz.h"

#define DEFAULT_BLOCK_SIZE 1
#define INTERNAL_VERSION 1
#define USE_CHECKSUM

#ifdef USE_CHECKSUM
#include <libiberty/libiberty.h>
#endif

#define BASE_BLOCK_SIZE 1024

/**
 * File header.
 */
typedef struct {
    unsigned char magic_number[2];
    unsigned char version;
    unsigned char use_previous_byte;
    unsigned short block_size;
} sr_header;

/**
 * Block header.
 */
typedef struct {
    size_t length;
    size_t compressed_length;
#ifdef USE_CHECKSUM	
    unsigned int checksum;
#endif	
    unsigned short last_word;
    unsigned char last_byte;
} sr_block_header;

/* ======================================================================== */
/** 
 * Usage.
 */
void usage() {
  fprintf(stdout, "USAGE: srcomp [c|d] -i <input_file> -o <output_file> \n");
  fprintf(stdout, " -h           print this message.\n");
  fprintf(stdout, " -c           compress.\n");
  fprintf(stdout, " -d           decompress.\n");
  fprintf(stdout, " -p           use previous data to compress more.\n");    
  fprintf(stdout, " -i <file>    specify the input file.\n");
  fprintf(stdout, " -o <file>    specify the output file.\n");    
  fprintf(stdout, " -b <size>    specify the block size (in kilobytes).\n");
  fprintf(stdout, "\n");
  fprintf(stdout, "Version " VERSION "\n");
  fprintf(stdout, "Copyright (C) 2022 Abraham Macias Paredes.\n");  
  fprintf(stdout, "GNU General Public License v3.\n");  
}

/* ======================================================================== */
/** 
 * Compress the input file into the output file.
 * @param infile Input file.
 * @param outfile Output file.
 * @param block_size Block size (in megabytes).
 * @param use_previous_byte Use the data to compress more.
 * @return 0 if everything goes OK.
 */
int compress_data(FILE *infile, FILE *outfile, int block_size,
                  bool use_previous_byte) {
  sr_header header;
  sr_block_header block_header;
  size_t cl, read;
  int bs, l, padding;
  unsigned short *src;
  unsigned char *dst;

  // Allocate memory
  bs = block_size * BASE_BLOCK_SIZE;
  src = (unsigned short *) malloc(bs);
  if (src == NULL) {
    perror("Error allocating memory\n");
    return -1;
  }
    
  dst = (unsigned char *) malloc(bs);
  if (dst == NULL) {
    perror("Error allocating memory\n");
    free(src);
    return -1;
  }

  // Write the file header
  memset(&header, 0, sizeof(header));
  header.magic_number[0] = 'S';
  header.magic_number[1] = 'R';
  header.version = INTERNAL_VERSION;
  header.use_previous_byte = use_previous_byte;
  header.block_size = block_size;

  if (fwrite(&header, sizeof(header), 1, outfile) != 1) {
    perror("Error writing file header");
    free(src);
    free(dst);
    return -1;
  }

  // Code each block
  read = fread(src, 1, bs, infile);
  while (read > 0) {
    // If the number of bytes read is odd we need 1 byte of padding
    padding = 0;
    if ( (read & 1) == 1 ) {
      padding = 1;
      src[((read+padding)>>1)-1] = 0;
    }    
    l = ((read+padding) >> 1);
    
    // Compress block
    block_header.length = read;
#ifdef USE_CHECKSUM	  
    block_header.checksum = xcrc32((unsigned char *) src, read, 0x80000000);
#endif	  
    block_header.last_word = src[l-1];
    
    cl = compress_block(src, dst, l, &block_header.last_byte, use_previous_byte);

    if (cl < 0) {
      fprintf(stderr, "Error compressing data block!\n");
      free(src);
      free(dst);      
      return -1;        
    }    
    
    block_header.compressed_length = cl;
    
    // Write block header
    if (fwrite(&block_header, sizeof(block_header), 1, outfile) != 1) {
      perror("Error writing block header");
      free(src);
      free(dst);      
      return -1;
    }
    
    // Write compressed block
    if (fwrite(dst, 1, cl, outfile) != cl) {
      perror("Error writing data to output file");
      free(src);
      free(dst);      
      return -1;
    }    
  
    // read next block to compress
    read = fread(src, 1, bs, infile);
  }
  
  if (read < 0)  {
    perror("Error reading input data");
    free(src);
    free(dst);
    return -1;
  }
  
  // Release memory
  free(src);
  free(dst);
    
  return 0;
}


/* ======================================================================== */
/** 
 * Decompress the input file into the output file.
 * @param infile Input file.
 * @param outfile Output file.
 * @return 0 if everything goes OK.
 */
int decompress_data(FILE *infile, FILE *outfile) {
  sr_header header;
  sr_block_header block_header;
  size_t decompressed_length, read;
  int bs, l, cl;
  unsigned char *src;
  unsigned short *dst;
  int block_size;
  bool use_previous_byte;

  // Read the file header
  if (fread(&header, sizeof(header), 1, infile) != 1) {
    perror("Error reading file header");
    return -1;
  }

  // Check the magic number
  if (header.magic_number[0] != 'S' || header.magic_number[1] != 'R') {
    fprintf(stderr, "Bad magic number!\n");
    return -1;
  }

  // Check the version
  if (header.version != INTERNAL_VERSION) {
    fprintf(stderr, "Wrong version!\n");
    return -1;
  }    
  
  use_previous_byte = header.use_previous_byte;
  block_size = header.block_size;      
    
  // Allocate memory
  bs = block_size * BASE_BLOCK_SIZE;
  src = (unsigned char *) malloc(bs);
  if (src == NULL) {
    perror("Error allocating memory\n");
    return -1;
  }
    
  dst = (unsigned short *) malloc(bs);
  if (dst == NULL) {
    perror("Error allocating memory\n");
    free(src);
    return -1;
  }

  // Read the block header
  read = fread(&block_header, sizeof(block_header), 1, infile);
  
  while (read > 0) {
    cl = block_header.compressed_length;    
    
    // Read input data
    if (fread(src, 1, cl, infile) != cl) {
      perror("Error reading input data");
      free(src);
      free(dst);      
      return -1;
    }
    
    // Decompress the data
    l = (block_header.length >> 1) + (block_header.length & 1);
    decompressed_length = decompress_block(src, dst, block_header.last_word,
                     block_header.last_byte, l, use_previous_byte);  

    if (decompressed_length < 0) {
      fprintf(stderr, "Error decompressing data block!\n");
      free(src);
      free(dst);      
      return -1;        
    }    
    
#ifdef USE_CHECKSUM	  
    // Check the checksum
    if (block_header.checksum != xcrc32(
          (unsigned char *) dst, block_header.length, 0x80000000)) {
      fprintf(stderr, "Bad checksum!\n");
      free(src);
      free(dst);      
      return -1;        
    }
#endif	  
      
    // Write the decompressed block data into the output file
    if (fwrite(dst, 1, block_header.length, outfile) != block_header.length) {
      perror("Error writing data to output file");
      free(src);
      free(dst);       
      return -1;
    }
  
    // Read the next block header
    read = fread(&block_header, sizeof(block_header), 1, infile);    
  }
  
  if (read < 0) {
    perror("Error reading block header");
    free(src);
    free(dst);    
    return -1;
  }

  // Release memory
  free(src);
  free(dst);
    
  return 0;
}

/* ======================================================================== */
/** 
 * Main program.
 * @param argc Number of arguments.
 * @param argv Arguments.
 * @return 0 if everything goes OK.
 */
int main(int argc, char *argv[]) {
  int compress = -1;
  int block_size = DEFAULT_BLOCK_SIZE;
  bool use_previous_byte = false;
  FILE *infile, *outfile;
  int opt;

  infile = stdin;
  outfile = stdout;

  // Check the arguments
  while ((opt = getopt(argc, argv, "hcdpi:o:b:")) != -1) {
    switch (opt) {
      case 'h':
        usage();
        return 0;
      case 'c':
        compress = 1;
        break;
      case 'd':
        compress = 0;
        break;
      case 'p':
        use_previous_byte = true;
        break;
      case 'i':
        if ( (infile = fopen(optarg, "rb")) == NULL) {
          fprintf(stderr, "Error opening input file: %s ", optarg);
          perror(":");
          return -1;
        }
        break;
      case 'o':
        if ( (outfile = fopen(optarg, "wb")) == NULL) {
          fprintf(stderr, "Error opening output file: %s ", optarg);
          perror(":");
          return -1;
        }
        break;            
      case 'b':
        block_size = atoi(optarg);
        if (block_size < 1 || block_size > 65536) {
          fprintf(stderr, "Wrong block size: %s (must be between 1K and 65536K)\n",
                  optarg);
          return -1;
        }
        break;
      default:
        fprintf(stderr, "Unknown option: %c\n", opt);
        usage();
        return -1;
    }
  }

  if (compress < 0) {
    fprintf(stderr, "You must specify if you want to compress or decompress"
            " the data\n");
    usage();
    return -1;  
  }

  if (compress) {
    return compress_data(infile, outfile, block_size, use_previous_byte);
  }
  else {
    return decompress_data(infile, outfile);
  }
}

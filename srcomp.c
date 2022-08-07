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

#include "wseparator.h"

#define DEFAULT_BLOCK_SIZE 1
#define VERSION 1

/**
 * File header.
 */
typedef struct {
    unsigned char magic_number[2];
    unsigned char version;
    size_t length;
    unsigned char use_previous_byte;
    unsigned char block_size;
} sr_header;

/**
 * Block header.
 */
typedef struct {
    size_t length;
    unsigned int checksum;
    unsigned short last_word;
    unsigned char first_byte;
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
  fprintf(stdout, " -b <size>    specify the block size (in megabytes).\n");
    
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
  size_t length, bs, l;
  size_t nblocks;
  int i;
  unsigned short *src;
  unsigned short *dst;

  // Allocate memory
  bs = block_size * 1024 * 1024;
  src = (unsigned short *) malloc(bs);
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

  // Calculate the file length
  fseek(infile, 0L, SEEK_END);
  length = ftell(infile);
  rewind(infile);

  // Write the file header
  header.magic_number[0] = 'S';
  header.magic_number[1] = 'R';
  header.version = VERSION;
  header.length = length;
  header.use_previous_byte = use_previous_byte;
  header.block_size = block_size;

  if (fwrite(&header, sizeof(header), 1, outfile) != 1) {
    perror("Error writing file header");
    return -1;
  }

  // Code each block
  nblocks = (length / bs);
  l = (bs >> 1);
  for (i = 0; i<nblocks; i++) {
    if (fread(src, 1, bs, infile) != bs) {
      perror("Error reading input data");    
      return -1;
    }
    
    block_header.length = bs;
    block_header.checksum = xcrc32((unsigned char *) src, bs, 0x80000000);
    block_header.last_word = src[l-1];
    block_header.first_byte = ((unsigned char *) src)[0];
      
    if (fwrite(&block_header, sizeof(block_header), 1, outfile) != 1) {
      perror("Error writing block header");
      return -1;
    }
  
    separate_words(src, dst, l, use_previous_byte);
    
    if (fwrite(dst, 1, bs, outfile) != bs) {
      perror("Error writing data to output file");
      return -1;
    }

  }

  if ( (length % bs) > 0 ) {
    // Code the last block
    bs = (length % bs);
    if ( (l % 1) == 1 ) {
      bs++;
	  src[(bs>>1)-1] = 0;
    }
	l = (bs >> 1);
	
    if (fread(src, 1, bs, infile) != bs) {
      perror("Error reading input data");        
      return -1;
    }
    
    block_header.length = bs;
    block_header.checksum = xcrc32((unsigned char *) src, bs, 0x80000000);
    block_header.last_word = src[l-1];
    block_header.first_byte = ((unsigned char *) src)[0];
      
    if (fwrite(&block_header, sizeof(block_header), 1, outfile) != 1) {
      perror("Error writing block header");
      return -1;
    }
  
    separate_words(src, dst, l, use_previous_byte);
    
    if (fwrite(dst, 1, bs, outfile) != bs) {
      perror("Error writing data to output file");
      return -1;
    }
    
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
  size_t length, bs, l;
  size_t nblocks;
  int i;
  unsigned short *src;
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
  if (header.version != VERSION) {
    fprintf(stderr, "Wrong version!\n");
    return -1;
  }	
	
  length = header.length;
  use_previous_byte = header.use_previous_byte;
  block_size = header.block_size;	  
	
  // Allocate memory
  bs = block_size * 1024 * 1024;
  src = (unsigned short *) malloc(bs);
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

  // Decode each block
  nblocks = (length / bs);
  l = (bs >> 1);
  for (i = 0; i<nblocks; i++) {
	// Read the block header
    if (fread(&block_header, sizeof(block_header), 1, infile) != 1) {
      perror("Error reading block header");
      return -1;
    }	  

    // Check the magic number
    if (block_header.length != bs) {
      fprintf(stderr, "Bad block size!\n");
      return -1;
    }	
    
	// Read input data
    if (fread(src, 1, bs, infile) != bs) {
      perror("Error reading input data");    
      return -1;
    }
	
	join_words(src, dst, block_header.last_word, l, use_previous_byte);
  
	// Check the checksum
    if (block_header.checksum != xcrc32(
          (unsigned char *) dst, bs, 0x80000000)) {
      fprintf(stderr, "Bad checksum!\n");
      return -1;		
	}
	  
    if (fwrite(dst, 1, bs, outfile) != bs) {
      perror("Error writing data to output file");
      return -1;
    }

  }

  if ( (length % bs) > 0 ) {
    // Code the last block
    bs = (length % bs);
    if ( (l % 1) == 1 ) {
      bs++;
	  src[(bs>>1)-1] = 0;
    }
	l = (bs >> 1);
	
    // Read the block header
    if (fread(&block_header, sizeof(block_header), 1, infile) != 1) {
      perror("Error reading block header");
      return -1;
    }	  

    // Check the magic number
    if (block_header.length != bs) {
      fprintf(stderr, "Bad block size!\n");
      return -1;
    }	
    
	// Read input data
    if (fread(src, 1, bs, infile) != bs) {
      perror("Error reading input data");    
      return -1;
    }
	
	join_words(src, dst, block_header.last_word, l, use_previous_byte);
  
	// Check the checksum
    if (block_header.checksum != xcrc32(
          (unsigned char *) dst, bs, 0x80000000)) {
      fprintf(stderr, "Bad checksum!\n");
      return -1;		
	}
	  
    if (fwrite(dst, 1, bs, outfile) != bs) {
      perror("Error writing data to output file");
      return -1;
    }
    
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
        break;
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
        if (block_size < 1 || block_size > 64) {
          fprintf(stderr, "Wrong block size: %s (must be between 1 and 64)\n",
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

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
#include "split2b.h"
#include "mtf.h"
#include "bitm.h"

#define DEFAULT_BLOCK_SIZE 1
#define VERSION 1
//#define USE_CHECKSUM

#ifdef USE_CHECKSUM
#include <libiberty/libiberty.h>
#endif

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
  fprintf(stdout, " -b <size>    specify the block size (in megabytes).\n");
    
}

/** 
 * Compress a data block. 
 * @param src The source array of words (to be separated into groups).
 * @param dst The destination array of words.
 * @param length The number of words in the source array.
 * @param last_byte Value of the last byte (before second separation). 
 * @param use_previous_byte Use the median value of the previous byte
 *                          to achieve a better sorting.
 */
void compress_block(unsigned short *src, unsigned short *dst, size_t length,
                    unsigned char *last_byte, bool use_previous_byte) {
  mtf_status status;
  
  mtf_reset(&status);
  
  separate_words(src, dst, length, use_previous_byte);
  *last_byte = ((unsigned char *) dst)[(length<<1) - 1];
  separate_bytes(dst, (unsigned char *) src, length);
  mtf_code((unsigned char *) src, (unsigned char *) dst, (length << 1),
           &status); 
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
  size_t length, cl;
  int bs, l;
  int nblocks;
  int i, j;
  unsigned short *src;
  unsigned short *dst;
  bitm_array *bitma;

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

  bitma = bitm_alloc(bs / sizeof(ELEMENT));
  if (bitma == NULL) {
    perror("Error allocating memory\n");
    free(src);
    free(dst);
    return -1;
  }  
  
  // Calculate the file length
  fseek(infile, 0L, SEEK_END);
  length = ftell(infile);
  rewind(infile);

  // Write the file header
  memset(&header, 0, sizeof(header));
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
    
    memset(&block_header, 0, sizeof(block_header));
    block_header.length = bs;
#ifdef USE_CHECKSUM	  
    block_header.checksum = xcrc32((unsigned char *) src, bs, 0x80000000);
#endif	  
    block_header.last_word = src[l-1];
    
        
    compress_block(src, dst, l, &block_header.last_byte, use_previous_byte);
    
    bitm_reset(bitma);
    
    for (j = 0; j<bs; j++) {
      bitm_write_eg(bitma, ((unsigned char *)dst)[j] + 1);
    }
    
    bitm_flush(bitma);
    cl = bitm_get_index(bitma);
    block_header.compressed_length = cl;
    
    if (fwrite(&block_header, sizeof(block_header), 1, outfile) != 1) {
      perror("Error writing block header");
      return -1;
    }
    
    
    if (fwrite(bitm_get_data(bitma), sizeof(ELEMENT), cl, outfile) != cl) {
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
#ifdef USE_CHECKSUM	  
    block_header.checksum = xcrc32((unsigned char *) src, bs, 0x80000000);
#endif	  
    block_header.last_word = src[l-1];
      
    compress_block(src, dst, l, &block_header.last_byte, use_previous_byte);

    bitm_reset(bitma);
    
    for (j = 0; j<bs; j++) {
      bitm_write_eg(bitma, ((unsigned char *)dst)[j] + 1);
    }
    
    bitm_flush(bitma);
    cl = bitm_get_index(bitma);
    block_header.compressed_length = cl;
    
    if (fwrite(&block_header, sizeof(block_header), 1, outfile) != 1) {
      perror("Error writing block header");
      return -1;
    }
    
    if (fwrite(bitm_get_data(bitma), sizeof(ELEMENT), cl, outfile) != cl) {
      perror("Error writing data to output file");
      return -1;
    }
    
  }
  
  // Release memory
  free(src);
  free(dst);
  bitm_free(bitma);
    
  return 0;
}

/* ======================================================================== */
/** 
 * Decompress a data block.
 * @param src The source array of words (to be joined from groups).
 * @param dst The destination array of words.
 * @param last_word Value of the last word (before separation).
 * @param last_byte Value of the last byte (before second separation).
 * @param length The number of words in the source array.
 * @param use_previous_byte Use the median value of the previous byte
 *                          to achieve a better sorting.
 */
void decompress_block(unsigned short *src, unsigned short *dst,
                      unsigned short last_word, unsigned char last_byte,
                      size_t length, bool use_previous_byte) {
  mtf_status status;
  
  mtf_reset(&status);
  
  mtf_decode((unsigned char *) src, (unsigned char *) dst, (length<<1),
             &status);
  join_bytes((unsigned char *) dst, src, last_byte, length);
  join_words(src, dst, last_word, length, use_previous_byte);
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
  size_t length;
  int bs, l, cl;
  int nblocks;
  int i, j;
  unsigned short *src;
  unsigned short *dst;
  int block_size;
  bool use_previous_byte;
  bitm_array *bitma;

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

  bitma = bitm_alloc(bs / sizeof(ELEMENT));
  if (bitma == NULL) {
    perror("Error allocating memory\n");
    free(src);
    free(dst);
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
    
    cl = block_header.compressed_length;
    bitm_reset(bitma);
    
    // Read input data
    if (fread(bitm_get_data(bitma), sizeof(ELEMENT), cl, infile) != cl) {
      perror("Error reading input data");    
      return -1;
    }
    
    for (j = 0; j<bs; j++) {
      ((unsigned char *)src)[j] = (unsigned char) (bitm_read_eg(bitma) - 1);
    }
    
    
    decompress_block(src, dst, block_header.last_word,
                     block_header.last_byte, l, use_previous_byte);
  
#ifdef USE_CHECKSUM	  
    // Check the checksum
    if (block_header.checksum != xcrc32(
          (unsigned char *) dst, bs, 0x80000000)) {
      fprintf(stderr, "Bad checksum!\n");
      return -1;        
    }
#endif	  
      
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
    
    cl = block_header.compressed_length;
    bitm_reset(bitma);
    
    // Read input data
    if (fread(bitm_get_data(bitma), sizeof(ELEMENT), cl, infile) != cl) {
      perror("Error reading input data");    
      return -1;
    }
    
    for (j = 0; j<bs; j++) {
      ((unsigned char *)src)[j] = (unsigned char) (bitm_read_eg(bitma) - 1);
    }
    
    decompress_block(src, dst, block_header.last_word,
                     block_header.last_byte, l, use_previous_byte);
  
#ifdef USE_CHECKSUM	  
    // Check the checksum
    if (block_header.checksum != xcrc32(
          (unsigned char *) dst, bs, 0x80000000)) {
      fprintf(stderr, "Bad checksum!\n");
      return -1;        
    }
#endif	  
      
    if (fwrite(dst, 1, bs, outfile) != bs) {
      perror("Error writing data to output file");
      return -1;
    }
    
  }

  // Release memory
  free(src);
  free(dst);
  bitm_free(bitma);
    
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

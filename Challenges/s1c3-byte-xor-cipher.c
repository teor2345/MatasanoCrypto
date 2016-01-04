//
//  s1c3-byte-xor-cipher.c
//  Find the key for a single-byte XOR
//  MatasanoCrypto
//
//  Created by Tim Wilson-Brown on 4/01/2016.
//  Copyright © 2016 teor.
//  GPLv3
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bit_ops.h"
#include "bytearray.h"
#include "hex.h"
#include "score.h"

/* Challenge-Specific Constants */

const char *input_hexstr  = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";

/* Scoring Heuristics */

/* My estimate of the average English line length is 40 characters */
#define ENGLISH_LINE_LENGTH 40

/* The average English word length is 5
 * (The frequency of English spaces is 1 in 6 characters)
 * http://www.quora.com/Whats-the-average-length-of-English-words */
#define ENGLISH_WORD_LENGTH 5
#define ENGLISH_SPACE_LENGTH (ENGLISH_WORD_LENGTH+1)

/* The average number of letters between punctuation in English text
 * The frequency of English punctuation is 208.7 per 1000 words
 * https://en.wikipedia.org/wiki/Punctuation_of_English#Frequency */
#define ENGLISH_PUNCTUATION_LENGTH ((ENGLISH_SPACE_LENGTH*1000)/209)

/* Allow for a few unprintable characters, like tabs or newlines.
 * This allows for CRLF-terminated lines 20 characters long, with an
 * additional allowance for a terminating newline.
 * (This is double my estimate of English line length, and a quarter of
 * typical terminal widths.) */
#define MAX_UNPRINTABLE_COUNT(length) (((length)/(ENGLISH_LINE_LENGTH/4))+2)
/* Check we actually have words, and not just blocks of text.
 * This enforces an average word length of 10 or fewer letters.
 * (This is double the typical English average word length of 5 letters.) */
#define MIN_SPACE_COUNT(length) ((length)/(ENGLISH_SPACE_LENGTH*2))
/* Non-letters include punctuation but exclude spaces. This allows for
 * punctuation every 14 characters. (This is double the typical English
 * punctuation frequency of 28 characters.) */
#define MAX_NONLETTER_COUNT(length) ((length)/(ENGLISH_PUNCTUATION_LENGTH/2))

/* Implementation */

int
main(int argc, const char * argv[])
{
  /* Unused */
  (void)argc;
  (void)argv;

  /* Hex -> Bytes */
  printf("Hex:                 %s\n", input_hexstr);

  bytearray_t *input_bytearray = hexstr_to_bytearray(input_hexstr);
  printf("Bytes:               %s\n", (char *)input_bytearray->bytes);

  char *input_asciistr = bytearray_to_asciistr(input_bytearray);
  printf("Escaped Bytes:       %s\n", input_asciistr);

  /* Try every different XOR value
   * use do ... while to get every single byte value in the loop */
  uint8_t byte = 0;
  do {
    bytearray_t *output_bytearray = bytearray_xor_byte(input_bytearray, byte);

    /* avoid printing candiates decryptions that contain unprintable
     * characters, or too many non-letters */
    size_t max_unprint = MAX_UNPRINTABLE_COUNT(output_bytearray->length);
    size_t max_nonletter = MAX_NONLETTER_COUNT(output_bytearray->length);
    size_t min_space = MIN_SPACE_COUNT(output_bytearray->length);
    if (count_unprintable(output_bytearray) <= max_unprint
        && count_nonletter(output_bytearray, 0) <= max_nonletter
        && count_space(output_bytearray) >= min_space) {
      /* Bytes -> Hex */
      printf("\n");

      printf("XOR Byte:              %hhu %c 0x%hhx\n", byte, byte, byte);

      char *output_hexstr = bytearray_to_hexstr(output_bytearray);
      printf("Hex XOR:               %s\n", output_hexstr);

      printf("Bytes XOR:             %s\n", (char *)output_bytearray->bytes);

      char *output_asciistr = bytearray_to_asciistr(output_bytearray);
      printf("Escaped Bytes XOR:     %s\n", output_asciistr);

      /* Cleanup conditional loop allocations */
      free(output_hexstr);
      free(output_asciistr);
    }

    /* Cleanup loop allocations */
    bytearray_free(output_bytearray);

    byte++;

    /* rely on unsigned integers wrapping to 0 on overflow to exit the loop */
  } while (byte != 0);

  /* Cleanup input allocations */
  bytearray_free(input_bytearray);
  free(input_asciistr);
  
  return 0;
}

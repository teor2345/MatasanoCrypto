//
//  s1c4-xor-cipher-detect.c
//  Find a string that's been encrypted with single-byte XOR
//  MatasanoCrypto
//
//  Created by Tim Wilson-Brown on 5/01/2016.
//  Copyright © 2016 teor.
//  GPLv3
//

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#include "bit_ops.h"
#include "bytearray.h"
#include "hex.h"
#include "score.h"

/* Challenge-Specific Constants */

/* The "Copy Files" build phase ensures this file is in the products directory
 */
const char *input_file_path  = "4.txt";

/* Implementation */

#define MIN_ENGLISH_TEXT_SCORE 0.1

#define MAX_LINE_LENGTH 1024

int
main(int argc, const char * argv[])
{
  /* Unused */
  (void)argc;
  (void)argv;

  /* Open the file */
  FILE *f = fopen(input_file_path, "r");
  assert(f != NULL);

  while (!feof(f)) {

    /* Read each line from the file */
    char input_hexstr[MAX_LINE_LENGTH];
    char *result = NULL;

    result = fgets(input_hexstr, MAX_LINE_LENGTH, f);
    if (result == NULL) {
      assert(!ferror(f));
      /* on EOF, exit the loop */
      break;
    }

    /* Strip the trailing newline character(s) */
    char *final_char = &input_hexstr[strlen(input_hexstr) - 1];
    while (final_char >= input_hexstr
           && !is_hexchar_valid(*final_char, HEXCHAR_ACCEPT_ANY_CASE)) {
      *final_char = 0;
      final_char--;
    }

    /* Skip zero-length strings */
    if (strlen(input_hexstr) == 0) {
      continue;
    }

    /* Check if it decrypts to English text with any XOR value */
    bytearray_t *input_bytearray = hexstr_to_bytearray(input_hexstr);

    /* Try every different XOR value
     * use do ... while to get every single byte value in the loop */
    uint8_t byte = 0;
    do {
      bytearray_t *output_bytearray = bytearray_xor_byte(input_bytearray, byte);

      double score = score_english_text(output_bytearray);

      if (score >= MIN_ENGLISH_TEXT_SCORE) {
        printf("Hex:                 %s\n", input_hexstr);

        char *input_asciistr = bytearray_to_escstr(input_bytearray);
        printf("Escaped Bytes:       %s\n", input_asciistr);

        /* Bytes -> Hex */
        printf("XOR Byte:              %hhu %c 0x%hhx\n", byte, byte, byte);
        printf("Overall Score:         %.3f\n", score);

        char *output_hexstr = bytearray_to_hexstr(output_bytearray);
        printf("Hex XOR:               %s\n", output_hexstr);

        /*printf("Bytes XOR:             %s\n", (char *)output_bytearray->bytes);
         */

        char *output_asciistr = bytearray_to_escstr(output_bytearray);
        printf("Escaped Bytes XOR:     %s\n", output_asciistr);
        printf("\n");

        /* Cleanup conditional loop allocations */
        free(input_asciistr);
        free(output_hexstr);
        free(output_asciistr);
      }

      /* Cleanup loop allocations */
      bytearray_free(output_bytearray);

      byte++;

      /* rely on unsigned integer wrapping to 0 on overflow to exit the loop */
    } while (byte != 0);

    /* Cleanup input allocations */
    bytearray_free(input_bytearray);
  }
  
  return 0;
}

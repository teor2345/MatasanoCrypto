//
//  s1c6-xor-repeat-break
//  Break repeating-key XOR
//  MatasanoCrypto
//
//  Created by Tim Wilson-Brown on 17/01/2016.
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
#include "base64.h"
#include "score.h"

/* Challenge-Specific Constants */

/* The "Copy Files" build phase ensures this file is in the products directory
 */
const char *input_file_path  = "6.txt";

const char *input_hamming_str_1 = "this is a test";
const char *input_hamming_str_2 = "wokka wokka!!!";
const size_t input_hamming_result = 37;

/* Implementation */

#define MIN_ENGLISH_TEXT_SCORE 0.1

#define MAX_LINE_LENGTH 1024

int
main(int argc, const char * argv[])
{
  /* Unused */
  (void)argc;
  (void)argv;

  /* Check that hamming works */
  printf("Hamming 1: %s\n", input_hamming_str_1);
  printf("Hamming 2: %s\n", input_hamming_str_2);

  bytearray_t *input_hamming_bytearray_1 = str_to_bytearray(
                                                          input_hamming_str_1);
  bytearray_t *input_hamming_bytearray_2 = str_to_bytearray(
                                                          input_hamming_str_2);

  char *input_hamming_escstr_1 = bytearray_to_escstr(
                                                    input_hamming_bytearray_1);
  printf("Escaped Hamming 1: %s\n", input_hamming_escstr_1);
  free(input_hamming_escstr_1);

  char *input_hamming_escstr_2 = bytearray_to_escstr(
                                                    input_hamming_bytearray_2);
  printf("Escaped Hamming 2: %s\n", input_hamming_escstr_2);
  free(input_hamming_escstr_2);

  bytearray_t *output_xor_bytearray = bytearray_xor(input_hamming_bytearray_1,
                                                    input_hamming_bytearray_2);


  char *output_xor_escstr = bytearray_to_escstr(output_xor_bytearray);
  printf("Escaped XOR: %s\n", output_xor_escstr);
  free(output_xor_escstr);

  bytearray_free(output_xor_bytearray);

  size_t output_hamming_result = bytearray_hamming(input_hamming_bytearray_1,
                                                   input_hamming_bytearray_2);

  printf("Expected Hamming: %zu\n", input_hamming_result);
  printf("Hamming Result: %zu\n", output_hamming_result);

  if (output_hamming_result == input_hamming_result) {
    printf("Expected hamming result matches output hamming result.\n");
  } else {
    printf("Expected hamming result does NOT match output hamming result.\n");
  }

  /* Open the file */
  FILE *f = fopen(input_file_path, "r");
  assert(f != NULL);

  /* Each line is decoded then appended to this bytearray */
  bytearray_t *input_bytearray = bytearray_alloc(0);

  while (!feof(f)) {

    /* Read each line from the file */
    char line_base64str[MAX_LINE_LENGTH];
    char *result = NULL;

    result = fgets(line_base64str, MAX_LINE_LENGTH, f);
    if (result == NULL) {
      assert(!ferror(f));
      /* on EOF, exit the loop */
      break;
    }

    /* Strip the trailing newline character(s) */
    char *final_char = &line_base64str[strlen(line_base64str) - 1];
    /* Strip padding bytes as well */
    while (final_char >= line_base64str
           && !is_base64char_valid(*final_char, BASE64_ACCEPT_ANY_VARIANT,
                                   false)) {
      *final_char = 0;
      final_char--;
    }

    /* Skip zero-length strings */
    if (strlen(line_base64str) == 0) {
      continue;
    }

    /* Decode the base64 string */
    bytearray_t *line_bytearray = base64str_to_bytearray(line_base64str);

    /* Append it to the previous base64 strings */
    bytearray_t *new_input_bytearray = bytearray_cat(input_bytearray,
                                                     line_bytearray);
    /* Get rid of the old input and line bytearrays, and keep the new input
     * bytearray */
    bytearray_free(input_bytearray);
    bytearray_free(line_bytearray);

    input_bytearray = new_input_bytearray;
    new_input_bytearray = NULL;
  }

  /* Close the file */
  int rv = fclose(f);
  assert(rv == 0);

  /* This should match the input file, excluding whitespace, and with trailing
   * padding replaced with 0 bits encoded in base64 characters. 000000 = A. */
  char *input_base64str = bytearray_to_base64str(input_bytearray);
  printf("Base64:  %s\n", input_base64str);
  free(input_base64str);

  char *input_escstr = bytearray_to_escstr(input_bytearray);
  printf("Escaped: %s\n", input_escstr);
  free(input_escstr);

  bytearray_free(input_bytearray);

#if 0

  /* Try every different XOR value
   * use do ... while to get every single byte value in the loop */
  uint8_t byte = 0;
  do {
    bytearray_t *output_bytearray = bytearray_xor_byte(input_bytearray, byte);

    double score = score_english_text(output_bytearray);

    if (score >= MIN_ENGLISH_TEXT_SCORE) {
      printf("Hex:                 %s\n", input_base64str);

      char *input_escstr = bytearray_to_escstr(input_bytearray);
      printf("Escaped Bytes:       %s\n", input_escstr);

      /* Bytes -> Hex */
      printf("XOR Byte:              %hhu %c 0x%hhx\n", byte, byte, byte);
      printf("Overall Score:         %.3f\n", score);

      char *output_hexstr = bytearray_to_hexstr(output_bytearray);
      printf("Hex XOR:               %s\n", output_hexstr);

      /*printf("Bytes XOR:             %s\n", (char *)output_bytearray->bytes);
       */

      char *output_escstr = bytearray_to_escstr(output_bytearray);
      printf("Escaped Bytes XOR:     %s\n", output_escstr);
      printf("\n");

      /* Cleanup conditional loop allocations */
      free(input_escstr);
      free(output_hexstr);
      free(output_escstr);
    }

    /* Cleanup loop allocations */
    bytearray_free(output_bytearray);

    byte++;

    /* rely on unsigned integer wrapping to 0 on overflow to exit the loop */
  } while (byte != 0);

#endif

  return 0;
}

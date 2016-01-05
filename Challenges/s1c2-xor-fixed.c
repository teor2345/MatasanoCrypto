//
//  s1c2-xor-fixed.c
//  XOR two byte buffers of equal length
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

const char *input_hexstr_1  = "1c0111001f010100061a024b53535009181c";
const char *input_hexstr_2  = "686974207468652062756c6c277320657965";

const char *expected_hexstr = "746865206b696420646f6e277420706c6179";

/* Implementation */

int
main(int argc, const char * argv[])
{
  /* Unused */
  (void)argc;
  (void)argv;

  /* Hex -> Bytes */
  printf("Hex 1:                 %s\n", input_hexstr_1);
  printf("Hex 2:                 %s\n", input_hexstr_2);

  bytearray_t *input_bytearray_1 = hexstr_to_bytearray(input_hexstr_1);
  /*printf("Bytes 1:               %s\n", (char *)input_bytearray_1->bytes);*/

  bytearray_t *input_bytearray_2 = hexstr_to_bytearray(input_hexstr_2);
  /*printf("Bytes 2:               %s\n", (char *)input_bytearray_2->bytes);*/

  char *input_asciistr_1 = bytearray_to_escstr(input_bytearray_1);
  printf("Escaped Bytes 1:       %s\n", input_asciistr_1);

  char *input_asciistr_2 = bytearray_to_escstr(input_bytearray_2);
  printf("Escaped Bytes 2:       %s\n", input_asciistr_2);

  /* XOR */
  bytearray_t *output_bytearray = bytearray_xor(input_bytearray_1,
                                                input_bytearray_2);

  /* Bytes -> Hex */
  char *output_hexstr = bytearray_to_hexstr(output_bytearray);
  printf("Hex XOR:               %s\n", output_hexstr);

  /*printf("Bytes XOR:             %s\n", (char *)output_bytearray->bytes);*/

  char *output_asciistr = bytearray_to_escstr(output_bytearray);
  printf("Escaped Bytes XOR:     %s\n", output_asciistr);

  if (!strcmp(output_hexstr, expected_hexstr)) {
    printf("Calculated Output matches Expected Output\n");
  } else {
    printf("Calculated Output differs from Expected Output\n");
  }

  /* Cleanup */
  bytearray_free(input_bytearray_1);
  bytearray_free(input_bytearray_2);
  free(input_asciistr_1);
  free(input_asciistr_2);
  bytearray_free(output_bytearray);
  free(output_hexstr);
  free(output_asciistr);

  return 0;
}

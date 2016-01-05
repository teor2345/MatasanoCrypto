//
//  s1c5-xor-repeat
//  XOR a byte buffer against a short key
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

const char *input_asciistr_1  = "Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal";
const char *input_asciistr_2  = "ICE";

const char *expected_hexstr = "0b3637272a2b2e63622c2e69692a23693a2a3c6324202d623d63343c2a26226324272765272a282b2f20430a652e2c652a3124333a653e2b2027630c692b20283165286326302e27282f";

/* Implementation */

int
main(int argc, const char * argv[])
{
  /* Unused */
  (void)argc;
  (void)argv;

  /* ASCII -> Bytes */
  printf("ASCII 1:               %s\n", input_asciistr_1);
  printf("ASCII 2:               %s\n", input_asciistr_2);

  /* Manually */
  bytearray_t *input_bytearray_1 = str_to_bytearray(input_asciistr_1);
  /*printf("Bytes 1:               %s\n", (char *)input_bytearray_1->bytes);*/

  bytearray_t *input_bytearray_2 = str_to_bytearray(input_asciistr_2);
  /*printf("Bytes 2:               %s\n", (char *)input_bytearray_2->bytes);*/

  char *input_escstr_1 = bytearray_to_escstr(input_bytearray_1);
  printf("Escaped Bytes 1:       %s\n", input_escstr_1);

  char *input_escstr_2 = bytearray_to_escstr(input_bytearray_2);
  printf("Escaped Bytes 2:       %s\n", input_escstr_2);

  /* XOR */
  bytearray_t *output_bytearray = bytearray_xor(input_bytearray_1,
                                                input_bytearray_2);

  /* Bytes -> Hex */
  char *output_hexstr = bytearray_to_hexstr(output_bytearray);
  printf("Hex XOR:               %s\n", output_hexstr);
  /*printf("Bytes XOR:             %s\n", (char *)output_bytearray->bytes);*/

  char *output_escstr = bytearray_to_escstr(output_bytearray);
  printf("Escaped Bytes XOR:     %s\n", output_escstr);

  if (!strcmp(output_hexstr, expected_hexstr)) {
    printf("Calculated Output matches Expected Output\n");
  } else {
    printf("Calculated Output differs from Expected Output\n");
  }

  /* Cleanup */
  bytearray_free(input_bytearray_1);
  bytearray_free(input_bytearray_2);
  free(input_escstr_1);
  free(input_escstr_2);
  bytearray_free(output_bytearray);
  free(output_hexstr);
  free(output_escstr);
  
  return 0;
}

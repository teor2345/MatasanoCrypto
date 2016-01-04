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
     * characters */
    if (count_unprintable(output_bytearray) == 0) {
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

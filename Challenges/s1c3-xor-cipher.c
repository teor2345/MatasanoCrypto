//
//  s1c3-xor-cipher.c
//  Find the key for a single-byte XOR
//  MatasanoCrypto
//
//  Created by Tim Wilson-Brown on 4/01/2016.
//  Copyright © 2016 teor - gmail: teor2345
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

#define MIN_ENGLISH_TEXT_SCORE 0.1

int
main(int argc, const char * argv[])
{
  /* Unused */
  (void)argc;
  (void)argv;

  /* Hex -> Bytes */
  printf("Hex:                 %s\n", input_hexstr);

  bytearray_t *input_bytearray = hexstr_to_bytearray(input_hexstr);
  /*printf("Bytes:               %s\n", (char *)input_bytearray->bytes);*/

  char *input_escstr = bytearray_to_escstr(input_bytearray);
  printf("Escaped Bytes:       %s\n", input_escstr);

  /* Try every different XOR value
   * use do ... while to get every single byte value in the loop */
  uint8_t byte = 0;
  do {
    bytearray_t *output_bytearray = bytearray_xor_byte(input_bytearray, byte);

    double score = score_english_text(output_bytearray);

    if (score >= MIN_ENGLISH_TEXT_SCORE) {
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
      free(output_hexstr);
      free(output_escstr);
    }

    /* Cleanup loop allocations */
    bytearray_free(output_bytearray);

    byte++;

    /* rely on unsigned integer wrapping to 0 on overflow to exit the loop */
  } while (byte != 0);

  /* Cleanup input allocations */
  bytearray_free(input_bytearray);
  free(input_escstr);
  
  return 0;
}

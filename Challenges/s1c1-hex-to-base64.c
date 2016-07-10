//
//  s1p1-hex-to-base64.c
//  Convert Hexadecimal to Base64
//  MatasanoCrypto
//
//  Created by Tim Wilson-Brown on 1/01/2016.
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

#include "base64.h"
#include "bytearray.h"
#include "hex.h"
#include "score.h"

/* Challenge-Specific Constants */

const char *input_hexstr = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";

const char *expected_base64str = "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t";

/* Implementation */

int
main(int argc, const char * argv[])
{
  /* Unused */
  (void)argc;
  (void)argv;

  /* Hex -> Bytes -> Base64 */
  printf("Hex:                 %s\n", input_hexstr);

  bytearray_t *input_bytearray = hexstr_to_bytearray(input_hexstr);
  /*printf("Bytes:               %s\n", (char *)input_bytearray->bytes);*/

  char *input_escstr = bytearray_to_escstr(input_bytearray);
  printf("Escaped Bytes:       %s\n", input_escstr);

  char *input_base64str = bytearray_to_base64str(input_bytearray);
  printf("Base64:              %s\n", input_base64str);

  printf("Base64 Expected:     %s\n", expected_base64str);

  if (!strcmp(input_base64str, expected_base64str)) {
    printf("Calculated Output matches Expected Output\n");
  } else {
    printf("Calculated Output differs from Expected Output\n");
  }

  /* Base64 -> Bytes -> Hex */
  bytearray_t *expected_bytearray = base64str_to_bytearray(expected_base64str);
  /*printf("Bytes Expected:      %s\n", (char *)expected_bytearray->bytes);*/

  char *expected_asciistr = bytearray_to_escstr(expected_bytearray);
  printf("Escaped Expected:    %s\n", expected_asciistr);

  char *expected_hexstr = bytearray_to_hexstr(expected_bytearray);
  printf("Expected Hex:        %s\n", expected_hexstr);

  if (!strcmp(input_hexstr, expected_hexstr)) {
    printf("Calculated Input matches Actual Input\n");
  } else {
    printf("Calculated Input differs from Actual Input\n");
  }

  /* Cleanup */
  bytearray_free(input_bytearray);
  free(input_escstr);
  free(input_base64str);
  bytearray_free(expected_bytearray);
  free(expected_asciistr);
  free(expected_hexstr);

  return 0;
}

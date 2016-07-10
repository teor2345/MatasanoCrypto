//
//  hex.h
//  MatasanoCrypto
//
//  Created by Tim Wilson-Brown on 2/01/2016.
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

#ifndef hex_h
#define hex_h

#include <limits.h>
#include <stdbool.h>
#include <sys/types.h>

/* Forward Declarations */

typedef struct bytearray_t bytearray_t;

/* Hexadecimal Constants */

static const uint8_t HEX_BIT = 4;
static const uint8_t HEX_BASE = 1 << HEX_BIT;
static const uint8_t HEX_LSB_MASK = HEX_BASE - 1;
static const uint8_t HEX_MSB_MASK = HEX_LSB_MASK << HEX_BIT;

static const uint8_t HEXCHARS_PER_BYTE = CHAR_BIT / HEX_BIT;
/* How many characters does it take to encode each byte as \xHH? */
static const uint8_t ESCAPED_HEXCHARS_PER_BYTE = HEXCHARS_PER_BYTE + 2;

static const char INVALID_HEXCHAR = 0;

/* Accept hex inputs or produce hex outputs with the specified letter case(s).
 */
typedef enum {
  HEXCHAR_ACCEPT_ANY_CASE,
  HEXCHAR_ACCEPT_LOWERCASE_ONLY,
  HEXCHAR_OUTPUT_LOWERCASE = HEXCHAR_ACCEPT_LOWERCASE_ONLY,
  HEXCHAR_ACCEPT_UPPERCASE_ONLY,
  HEXCHAR_OUTPUT_UPPERCASE = HEXCHAR_ACCEPT_UPPERCASE_ONLY
} hex_case_t;

/* Hexadecimal Characters */

bool is_hexchar_lowercase_accepted(hex_case_t hexcase);
bool is_hexchar_lowercase_output(hex_case_t hexcase);

bool is_hexchar_uppercase_accepted(hex_case_t hexcase);
bool is_hexchar_uppercase_output(hex_case_t hexcase);

bool is_hexchar_valid(char hexchar, hex_case_t hexcase);

uint8_t hexchar_to_nybble(char hexchar, hex_case_t hexcase);
char nybble_to_hexchar(uint8_t nybble, hex_case_t hexcase);

/* Hexadecimal Strings */

uint8_t hexpair_to_byte(char hexchar_msb, char hexchar_lsb);
void byte_to_hexpair(uint8_t byte, char* hexchar_msb_out,
                     char* hexchar_lsb_out);

bytearray_t *hexstr_to_bytearray(const char *hexstr);
char *bytearray_to_hexstr(const bytearray_t *bytearray);

#endif /* hex_h */

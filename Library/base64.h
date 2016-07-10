//
//  base64.h
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

#ifndef base64_h
#define base64_h

#include <limits.h>
#include <stdbool.h>
#include <sys/types.h>

#include "char.h"

/* Forward Declarations */
typedef struct bytearray_t bytearray_t;

/* Base64 Constants */

static const uint8_t BASE64_BIT = 6;
static const uint8_t BASE64_BASE = 1 << BASE64_BIT;
static const uint8_t BASE64_MASK = BASE64_BASE - 1;

/* lowest_common_multiple(BASE64_BIT, BYTE_BIT) */
static const uint8_t BASE64_BLOCK_BIT = 24;
static const uint8_t BASE64_CHARS_PER_BLOCK = BASE64_BLOCK_BIT / BASE64_BIT;
static const uint8_t BASE64_BYTES_PER_BLOCK = BASE64_BLOCK_BIT / BYTE_BIT;

static const char INVALID_BASE64CHAR = 0;

/* Accept base64 inputs or produce base64 outputs with the specified character
 * sets. See is_base64char_valid_padding for valid padding characters. */
typedef enum {
  /* Any compatible variant listed below */
  BASE64_ACCEPT_ANY_VARIANT,
  /* Standard */
  BASE64_ACCEPT_PLUS_SLASH_ONLY,
  /* Base64url */
  BASE64_OUTPUT_PLUS_SLASH = BASE64_ACCEPT_PLUS_SLASH_ONLY,
  BASE64_ACCEPT_DASH_UNDERSCORE_ONLY,
  BASE64_OUTPUT_DASH_UNDERSCORE = BASE64_ACCEPT_DASH_UNDERSCORE_ONLY,
  /* YUI/Programming Language Identifiers variant 2 */
  BASE64_ACCEPT_PERIOD_UNDERSCORE_ONLY,
  BASE64_OUTPUT_PERIOD_UNDERSCORE = BASE64_ACCEPT_PERIOD_UNDERSCORE_ONLY,
  /* All other variants are incompatible with one of the listed variants,
   * because they re-use one of the characters in another position */
} base64_variant_t;

/* Base64 Characters */

bool is_base64char_plus_accepted(base64_variant_t variant);
bool is_base64char_plus_output(base64_variant_t variant);

bool is_base64char_slash_accepted(base64_variant_t variant);
bool is_base64char_slash_output(base64_variant_t variant);

bool is_base64char_dash_accepted(base64_variant_t variant);
bool is_base64char_dash_output(base64_variant_t variant);

bool is_base64char_period_accepted(base64_variant_t variant);
bool is_base64char_period_output(base64_variant_t variant);

bool is_base64char_underscore_accepted(base64_variant_t variant);
bool is_base64char_underscore_output(base64_variant_t variant);

bool is_base64char_valid_padding(char base64char);
bool is_base64char_valid(char base64char, base64_variant_t variant,
                         bool accept_padding);

uint8_t base64char_to_value(char base64char, base64_variant_t variant);
char value_to_base64char(uint8_t value, base64_variant_t variant);

/* Base64 Strings */

void base64chars_to_bytes(const char base64chars[BASE64_CHARS_PER_BLOCK],
                          uint8_t bytes_out[BASE64_BYTES_PER_BLOCK]);
void bytes_to_base64chars(const uint8_t bytes[BASE64_BYTES_PER_BLOCK],
                          char base64chars_out[BASE64_CHARS_PER_BLOCK]);

bytearray_t *base64str_to_bytearray(const char *base64str);
char *bytearray_to_base64str(const bytearray_t *bytearray);

#endif /* base64_h */

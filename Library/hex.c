//
//  hex.c
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

#include "hex.h"

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "bytearray.h"
#include "calc.h"
#include "char.h"

/* Does hexcase accept lowercase hex characters? */
bool
is_hexchar_lowercase_accepted(hex_case_t hexcase)
{
  return (hexcase == HEXCHAR_ACCEPT_ANY_CASE
          || hexcase == HEXCHAR_ACCEPT_LOWERCASE_ONLY);
}

/* Output lowercase hex characters based on hexcase? */
bool
is_hexchar_lowercase_output(hex_case_t hexcase)
{
  assert(hexcase != HEXCHAR_ACCEPT_ANY_CASE);
  return hexcase == HEXCHAR_OUTPUT_LOWERCASE;
}

/* Does hexcase accept uppercase hex characters? */
bool
is_hexchar_uppercase_accepted(hex_case_t hexcase)
{
  return (hexcase == HEXCHAR_ACCEPT_ANY_CASE
          || hexcase == HEXCHAR_ACCEPT_UPPERCASE_ONLY);
}

/* Output uppercase hex characters based on hexcase? */
bool
is_hexchar_uppercase_output(hex_case_t hexcase)
{
  assert(hexcase != HEXCHAR_ACCEPT_ANY_CASE);
  return hexcase == HEXCHAR_OUTPUT_UPPERCASE;
}

/* Is hexchar a valid hex character?
 * Accepts lowercase and/or uppercase based on hexcase. */
bool
is_hexchar_valid(char hexchar, hex_case_t hexcase)
{
  const bool accept_lowercase = is_hexchar_lowercase_accepted(hexcase);
  const bool accept_uppercase = is_hexchar_uppercase_accepted(hexcase);

  if (hexchar >= '0' && hexchar <= '9') {
    return true;
  }

  if (accept_lowercase && hexchar >= 'a' && hexchar <= 'f') {
    return true;
  }

  if (accept_uppercase && hexchar >= 'A' && hexchar <= 'F') {
    return true;
  }

  return false;
}

/* Convert the character hexchar into the equivalent value.
 * Accepts lowercase and/or uppercase based on hexcase. */
uint8_t
hexchar_to_nybble(char hexchar, hex_case_t hexcase)
{
  assert(is_hexchar_valid(hexchar, hexcase));

  uint8_t nybble = 0;

  /* because we want to convert using lowercase letters, we do this conversion
   * regardless of hexcase. (It's checked in the precondition.) */
  char low_hexchar = (char)tolower(hexchar);
  assert((low_hexchar >= '0' && low_hexchar <= '9')
         || (low_hexchar >= 'a' && low_hexchar <= 'f'));

  if (char_to_value(low_hexchar, '0', '9', 0, &nybble)) {
    /* nybble has been set to the correct value */
  } else if (char_to_value(low_hexchar, 'a', 'f', ('9' - '0' + 1), &nybble)) {
    /* nybble has been set to the correct value */
  } else {
    abort();
  }

  assert(nybble < HEX_BASE);
  return nybble;
}

/* Convert value into its equivalent hex character.
 * Outputs lowercase or uppercase based on hexcase. */
char
nybble_to_hexchar(uint8_t nybble, hex_case_t hexcase)
{
  assert(nybble < HEX_BASE);

  char hexchar = INVALID_HEXCHAR;

  if (value_to_char(nybble, 0, 9, '0', &hexchar)) {
    /* hexchar has been set to the correct char */
  } else if (value_to_char(nybble, 10, 15, 'a', &hexchar)) {
    /* hexchar has been set to the correct char */
  } else {
    abort();
  }

  if (is_hexchar_uppercase_output(hexcase)) {
    hexchar = (char)toupper(hexchar);
  }

  assert(is_hexchar_valid(hexchar, hexcase));
  return hexchar;
}

/* Convert the characters hexchar_msb and hexchar_lsb into the equivalent byte
 * value.
 * hexchar_msb becomes the high-order 4 bits, and hexchar_lsb becomes the
 * low-order 4 bits.
 * Accepts lowercase and uppercase hexadecimal characters. */
uint8_t
hexpair_to_byte(char hexchar_msb, char hexchar_lsb)
{
  assert(is_hexchar_valid(hexchar_msb, HEXCHAR_ACCEPT_ANY_CASE));
  assert(is_hexchar_valid(hexchar_lsb, HEXCHAR_ACCEPT_ANY_CASE));

  const uint8_t msb = hexchar_to_nybble(hexchar_msb, HEXCHAR_ACCEPT_ANY_CASE);
  const uint8_t lsb = hexchar_to_nybble(hexchar_lsb, HEXCHAR_ACCEPT_ANY_CASE);
  uint8_t byte = 0;

  /* These assertions mean that we don't have to apply the HEX_MSB/LSB_MASKs */
  assert(msb < HEX_BASE);
  assert(lsb < HEX_BASE);
  byte = (uint8_t)(msb << HEX_BIT) | lsb;

  /* byte can take any valid value for the type */
  return byte;
}

/* Convert the value byte into the equivalent hexadecimal characters
 * *hexchar_msb_out and *hexchar_lsb_out.
 * hexchar_msb_out is taken from the high-order 4 bits, and hexchar_lsb_out
 * is taken from the low-order 4 bits.
 * Outputs lowercase hexadecimal characters. */
void
byte_to_hexpair(uint8_t byte, char* hexchar_msb_out, char* hexchar_lsb_out)
{
  /* byte can take any valid value for the type */
  assert(hexchar_msb_out != NULL);
  assert(hexchar_lsb_out != NULL);

  /* We don't need to apply HEX_MSB_MASK because the right shift erases the
   * LSB nybble */
  const uint8_t msb = byte >> HEX_BIT;
  const uint8_t lsb = byte & HEX_LSB_MASK;

  *hexchar_msb_out = nybble_to_hexchar(msb, HEXCHAR_OUTPUT_LOWERCASE);
  assert(is_hexchar_valid(*hexchar_msb_out, HEXCHAR_ACCEPT_LOWERCASE_ONLY));

  *hexchar_lsb_out = nybble_to_hexchar(lsb, HEXCHAR_OUTPUT_LOWERCASE);
  assert(is_hexchar_valid(*hexchar_lsb_out, HEXCHAR_ACCEPT_LOWERCASE_ONLY));
}

/* Convert the nul-terminated hexadecimal string hexstr into a newly allocated
 * array of bytes.
 * If hexstr partially fills the final byte, the remaining bits are zero
 * (that is, we act like it has an extra '0' at the end of the string).
 * Never returns a NULL bytearray_t *. If hexstr is "", the returned
 * bytearray_t * has a zero length and NULL bytes pointer.
 * Accepts lowercase and uppercase hexadecimal characters.
 * Strings most not have an "0x" prefix, or any other non-hex characters,
 * including whitespace.
 * The caller must bytearray_free() the returned bytearray_t. */
bytearray_t *
hexstr_to_bytearray(const char *hexstr)
{
  /* hexstr can be of arbitrary length, including zero */
  const size_t hexstr_len = strlen(hexstr);

  bytearray_t *bytearray = NULL;

  /* round up the length if there is an odd number of hex characters */
  bytearray = bytearray_alloc(ceil_div(hexstr_len, HEXCHARS_PER_BYTE));
  assert(bytearray_length(bytearray) == ((hexstr_len / HEXCHARS_PER_BYTE)
                                         + (hexstr_len % HEXCHARS_PER_BYTE)));
  assert(is_bytearray_consistent(bytearray));

  size_t i = 0;
  for (i = 0; i < bytearray_length(bytearray); i++) {
    const size_t hexstr_pos = i * HEXCHARS_PER_BYTE;

    assert(hexstr_pos < hexstr_len);
    const char hexchar_msb = hexstr[hexstr_pos];

    /* if we're missing a hexchar for the final byte, act like it's '0' */
    char hexchar_lsb = '0';
    if (hexstr_pos + 1 < hexstr_len) {
      hexchar_lsb = hexstr[hexstr_pos + 1];
    }

    uint8_t byte = hexpair_to_byte(hexchar_msb, hexchar_lsb);
    bytearray_set_checked(bytearray, i, byte);

    assert(is_bytearray_consistent(bytearray));
  }

  /* Did we actually look at everything? */
  assert(i == bytearray_length(bytearray));
  assert(i == ceil_div(hexstr_len, HEXCHARS_PER_BYTE));
  assert(is_bytearray_consistent(bytearray));

  return bytearray;
}

/* Convert the byte array bytearray into a newly allocated hexadecimal
 * nul-terminated string.
 * Never returns a NULL char *. If bytearray has a zero length, the returned
 * char * is "".
 * Outputs lowercase hexadecimal characters.
 * Outputs raw hexadecimal without an "0x" prefix or whitespace.
 * The caller must free() the returned string. */
char *
bytearray_to_hexstr(const bytearray_t *bytearray)
{
  assert(bytearray != NULL);
  assert(is_bytearray_consistent(bytearray));

  /* One extra byte for the terminating nul */
  const size_t hexstr_len = (
                          bytearray_length(bytearray) * HEXCHARS_PER_BYTE + 1);
  char * const hexstr = malloc(hexstr_len);
  assert(hexstr != NULL);
  /* Avoid having to add the terminating nul later */
  memset(hexstr, 0, hexstr_len);

  size_t i = 0;
  for (i = 0; i < bytearray_length(bytearray); i++) {
    const size_t hexstr_pos = i * HEXCHARS_PER_BYTE;

    /* Don't ever overwrite the terminating nul, and allow for the second
     * hexchar */
    assert(hexstr_pos + 1 < hexstr_len - 1);
    const uint8_t byte = bytearray_get_checked(bytearray, i);
    byte_to_hexpair(byte, &hexstr[hexstr_pos], &hexstr[hexstr_pos + 1]);
  }

  /* Did we actually look at everything, except the terminating nul? */
  assert(i == bytearray_length(bytearray));
  assert(i * HEXCHARS_PER_BYTE == hexstr_len - 1);

  return hexstr;
}

//
//  s1p1-hex-to-base64.c
//  Convert Hexadecimal to Base64
//  MatasanoCrypto
//
//  Created by Tim Wilson-Brown on 1/01/2016.
//  Copyright © 2016 teor.
//  GPLv3
//

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Challenge-Specific Constants */

const char *input_hexstr = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";

const char *expected_output_base64str = "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t";

/* Declarations */

/* Utility */

size_t ceil_div(size_t dividend, size_t divisor);

/* Hexadecimal Characters */

const uint8_t HEX_BITS = 4;
const uint8_t HEX_BASE = 1 << HEX_BITS;
const uint8_t HEX_LSB_MASK = HEX_BASE - 1;
const uint8_t HEX_MSB_MASK = HEX_LSB_MASK << HEX_BITS;

const uint8_t CHAR_BITS = 8;
const uint8_t HEXCHARS_PER_BYTE = CHAR_BITS / HEX_BITS;

/* Accept hex inputs or produce hex outputs with the specified letter case(s). */
typedef enum {
  HEXCHAR_ACCEPT_ANY_CASE,
  HEXCHAR_ACCEPT_LOWERCASE_ONLY,
  HEXCHAR_OUTPUT_LOWERCASE = HEXCHAR_ACCEPT_LOWERCASE_ONLY,
  HEXCHAR_ACCEPT_UPPERCASE_ONLY,
  HEXCHAR_OUTPUT_UPPERCASE = HEXCHAR_ACCEPT_UPPERCASE_ONLY
} hexchar_case_t;

bool is_hexchar_lowercase_accepted(hexchar_case_t hexcase);
bool is_hexchar_uppercase_accepted(hexchar_case_t hexcase);

bool is_hexchar_lowercase_output(hexchar_case_t hexcase);
bool is_hexchar_uppercase_output(hexchar_case_t hexcase);

bool is_valid_hexchar(char hexchar, hexchar_case_t hexcase);

uint8_t hexchar_to_nybble(char hexchar, hexchar_case_t hexcase);
char nybble_to_hexchar(uint8_t nybble, hexchar_case_t hexcase);

/* Hexadecimal Strings */

uint8_t hexpair_to_byte(char hexchar_msb, char hexchar_lsb);
void byte_to_hexpair(uint8_t byte, char* hexchar_msb_out, char* hexchar_lsb_out);

typedef struct {
  size_t length;
  uint8_t *bytes;
} byte_array_t;

bool is_bytearray_consistent(const byte_array_t *bytearray);
byte_array_t* bytearray_alloc(size_t length);
void bytearray_free_(byte_array_t *bytearray);
#define bytearray_free(bytearray) do { bytearray_free_(bytearray); bytearray = NULL; } while (0)
#define bytearray_checked(bytearray, index) (assert(index < bytearray->length), bytearray->bytes[index])

byte_array_t *hexstr_to_bytearray(const char *hexstr);
char *bytearray_to_hexstr(const byte_array_t *bytearray);

/* Definitions */

/* Return the ceiling (rounded up) value of dividend / divisor. */
size_t
ceil_div(size_t dividend, size_t divisor)
{
  assert(divisor != 0);

  const size_t result = (dividend + divisor - 1) / divisor;

  assert(((dividend % divisor) == 0 && result == (dividend / divisor))
         || (dividend % divisor) != 0 && result == ((dividend / divisor) + 1));

  return result;
}

/* Does hexchar_case allow lowercase hex characters? */
bool
is_hexchar_lowercase_accepted(hexchar_case_t hexcase)
{
  return (hexcase == HEXCHAR_ACCEPT_ANY_CASE
          || hexcase == HEXCHAR_ACCEPT_LOWERCASE_ONLY);
}

/* Does hexchar_case allow uppercase hex characters? */
bool
is_hexchar_uppercase_accepted(hexchar_case_t hexcase)
{
  return (hexcase == HEXCHAR_ACCEPT_ANY_CASE
          || hexcase == HEXCHAR_ACCEPT_UPPERCASE_ONLY);
}

/* Output lowercase hex characters based on hexchar_case? */
bool
is_hexchar_lowercase_output(hexchar_case_t hexcase)
{
  assert(hexcase != HEXCHAR_ACCEPT_ANY_CASE);
  return hexcase == HEXCHAR_OUTPUT_LOWERCASE;
}

/* Output uppercase hex characters based on hexchar_case? */
bool
is_hexchar_uppercase_output(hexchar_case_t hexcase)
{
  assert(hexcase != HEXCHAR_ACCEPT_ANY_CASE);
  return hexcase == HEXCHAR_OUTPUT_UPPERCASE;
}

/* Is hexchar a valid hex character?
 * Accepts lowercase and/or uppercase based on hexchar_case. */
bool
is_valid_hexchar(char hexchar, hexchar_case_t hexcase)
{
  const bool allow_lowercase = is_hexchar_lowercase_accepted(hexcase);
  const bool allow_uppercase = is_hexchar_uppercase_accepted(hexcase);

  if (hexchar >= '0' && hexchar <= '9') {
    return true;
  }

  if (allow_lowercase && hexchar >= 'a' && hexchar <= 'f') {
    return true;
  }

  if (allow_uppercase && hexchar >= 'A' && hexchar <= 'F') {
    return true;
  }

  return false;
}

/* Convert the character hexchar into its equivalent base 16 value.
 * Accepts lowercase and/or uppercase based on hexchar_case. */
uint8_t
hexchar_to_nybble(char hexchar, hexchar_case_t hexcase)
{
  assert(is_valid_hexchar(hexchar, hexcase));

  uint8_t nybble = 0;

  /* because we want to convert using lowercase letters, we do this conversion
   * regardless of hexchar_case. (It's checked in the precondition.) */
  char low_hexchar = (char)tolower(hexchar);
  assert((low_hexchar >= '0' && low_hexchar <= '9')
         || (low_hexchar >= 'a' && low_hexchar <= 'f'));

  if (low_hexchar >= '0' && low_hexchar <= '9') {
    nybble = (uint8_t)(low_hexchar - '0');
  } else if (low_hexchar >= 'a' && low_hexchar <= 'f') {
    nybble = 10 + (uint8_t)(low_hexchar - 'a');
  } else {
    abort();
  }

  assert(nybble < HEX_BASE);
  return nybble;
}

/* Convert the base 16 value into its equivalent hex character.
 * Outputs lowercase or uppercase based on hexchar_case. */
char
nybble_to_hexchar(uint8_t nybble, hexchar_case_t hexcase)
{
  assert(nybble < HEX_BASE);

  char hexchar = 0;

  if (nybble >= 0 && nybble <= 9) {
    hexchar = (char)(nybble + '0');
  } else if (nybble >= 10 && nybble <= 15) {
    hexchar = (char)(nybble - 10 + 'a');
  } else {
    abort();
  }

  if (is_hexchar_uppercase_output(hexcase)) {
    hexchar = (char)toupper(hexchar);
  }

  assert(is_valid_hexchar(hexchar, hexcase));
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
  assert(is_valid_hexchar(hexchar_msb, HEXCHAR_ACCEPT_ANY_CASE));
  assert(is_valid_hexchar(hexchar_lsb, HEXCHAR_ACCEPT_ANY_CASE));

  const uint8_t msb = hexchar_to_nybble(hexchar_msb, HEXCHAR_ACCEPT_ANY_CASE);
  const uint8_t lsb = hexchar_to_nybble(hexchar_lsb, HEXCHAR_ACCEPT_ANY_CASE);
  uint8_t byte = 0;

  /* These assertions mean that we don't have to apply the HEX_MSB/LSB_MASKs */
  assert(msb < HEX_BASE);
  assert(lsb < HEX_BASE);
  byte = (uint8_t)(msb << HEX_BITS) | lsb;

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
  const uint8_t msb = byte >> HEX_BITS;
  const uint8_t lsb = byte & HEX_LSB_MASK;

  *hexchar_msb_out = nybble_to_hexchar(msb, HEXCHAR_OUTPUT_LOWERCASE);
  assert(is_valid_hexchar(*hexchar_msb_out, HEXCHAR_ACCEPT_LOWERCASE_ONLY));

  *hexchar_lsb_out = nybble_to_hexchar(lsb, HEXCHAR_OUTPUT_LOWERCASE);
  assert(is_valid_hexchar(*hexchar_lsb_out, HEXCHAR_ACCEPT_LOWERCASE_ONLY));
}

/* Are the length and bytes fields of bytearray consistent? */
bool
is_bytearray_consistent(const byte_array_t *bytearray)
{
  return (bytearray != NULL
          && ((bytearray->length > 0 && bytearray->bytes != NULL)
              || (bytearray->length == 0 && bytearray->bytes == NULL)));
}

/* Allocate and return a byte_array_t of length using malloc().
 * Must be freed using bytearray_free(). */
byte_array_t* bytearray_alloc(size_t length)
{
  assert(length > 0);

  byte_array_t *bytearray = malloc(sizeof(*bytearray));
  bytearray->length = length;
  if (length > 0) {
    bytearray->bytes = malloc(bytearray->length);
    assert(bytearray->bytes != NULL);
    memset(bytearray->bytes, 0, bytearray->length);
  } else {
    bytearray->bytes = NULL;
  }

  assert(bytearray != NULL);
  assert(bytearray->length == length);

  assert(is_bytearray_consistent(bytearray));

  return bytearray;
}

/* Free a byte_array_t allocated using bytearray_alloc().
 * If bytearray is NULL, nothing happens.
 * Use bytearray_free to set bytearray to NULL as well.
 * (Setting a byte_array_t * to NULL after each free is a great way to avoid
 * use-after-free errors - as long as there are no other copies of the pointer.)
 */
void bytearray_free_(byte_array_t *bytearray)
{
  if (bytearray == NULL) {
    return;
  }

  assert(is_bytearray_consistent(bytearray));

  if (bytearray->bytes != NULL) {
    /* I just can't spell 0xfree */
    memset(bytearray->bytes, 0xfe, bytearray->length);
    free(bytearray->bytes);
    bytearray->bytes = NULL;
    bytearray->length = 0;

    assert(is_bytearray_consistent(bytearray));
  }

  free(bytearray);
}

/* Convert the nul-terminated hexadecimal string hexstr into a newly allocated
 * array of bytes.
 * If hexstr partially fills the final byte, the remaining bits are zero
 * (that is, we act like it has an extra '0' at the end of the string).
 * Never returns a NULL byte_array_t *. If hexstr is "", the returned
 * byte_array_t * has a zero length and NULL bytes pointer.
 * Accepts lowercase and uppercase hexadecimal characters.
 * Strings most not have an "0x" prefix, or any other non-hex characters,
 * including spaces.
 * The caller must bytearray_free() the returned byte_array_t. */
byte_array_t *hexstr_to_bytearray(const char *hexstr)
{
  /* hexstr can be of arbitrary length, including zero */
  const size_t hexstr_len = strlen(hexstr);

  byte_array_t *bytearray = NULL;

  /* round up the length if there is an odd number of hex characters */
  bytearray = bytearray_alloc(ceil_div(hexstr_len, HEXCHARS_PER_BYTE));
  assert(bytearray->length == (hexstr_len / 2) + (hexstr_len % 2));
  assert(is_bytearray_consistent(bytearray));

  size_t i = 0;
  for (i = 0; i < bytearray->length; i++) {
    const size_t hexstr_pos = i * 2;

    assert(hexstr_pos < hexstr_len);
    const char hexchar_msb = hexstr[hexstr_pos];

    /* if we're missing a hexchar for the final byte, act like it's '0' */
    char hexchar_lsb = '0';
    if (hexstr_pos + 1 < hexstr_len) {
      hexchar_lsb = hexstr[hexstr_pos + 1];
    }

    assert(i < bytearray->length);
    bytearray_checked(bytearray, i) = hexpair_to_byte(hexchar_msb, hexchar_lsb);

    assert(is_bytearray_consistent(bytearray));
  }

  /* Did we actually look at everything? */
  assert(i == bytearray->length);
  assert(i * 2 == hexstr_len || i * 2 == hexstr_len + 1);

  assert(is_bytearray_consistent(bytearray));

  return bytearray;
}

/* Convert the byte array bytearray into a newly allocated hexadecimal
 * nul-terminated string.
 * Never returns a NULL char *. If bytearray has a zero length, the returned
 * char * is a copy of "".
 * Outputs lowercase hexadecimal characters.
 * Outputs raw hexadecimal without an "0x" prefix or spaces.
 * The caller must free() the returned string. */
char *bytearray_to_hexstr(const byte_array_t *bytearray)
{
  assert(bytearray != NULL);
  assert(is_bytearray_consistent(bytearray));

  /* One extra byte for the terminating nul */
  size_t hexstr_len = bytearray->length * 2 + 1;
  char *hexstr = malloc(hexstr_len);
  assert(hexstr != NULL);
  /* Avoid having to add the terminating nul later */
  memset(hexstr, 0, hexstr_len);

  size_t i = 0;
  for (i = 0; i < bytearray->length; i++) {
    const size_t hexstr_pos = i * 2;

    /* Don't ever overwrite the terminating nul, and allow for the second
     * hexchar */
    assert(hexstr_pos + 1 < hexstr_len - 1);
    assert(i < bytearray->length);
    byte_to_hexpair(bytearray->bytes[i], &hexstr[hexstr_pos],
                    &hexstr[hexstr_pos + 1]);
  }

  /* Did we actually look at everything, except the terminating nul? */
  assert(i == bytearray->length);
  assert(i * 2 == hexstr_len - 1);

  return hexstr;
}

int
main(int argc, const char * argv[])
{
  (void)argc;
  (void)argv;

  size_t input_byte_len = (strlen(input_hexstr) + 1)/2;
  uint8_t *input_byte_data = malloc(input_byte_len);

  for (size_t i = 0; i < input_byte_len; i++) {
    uint8_t h1 = 0;
    uint8_t h2 = 0;

  }

  free(input_byte_data);
  return 0;
}

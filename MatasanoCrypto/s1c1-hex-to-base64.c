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
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Challenge-Specific Constants */

const char *input_hexstr = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";

const char *expected_base64str = "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t";

/* Declarations */

/* Utility */

size_t ceil_div(size_t dividend, size_t divisor);

/* Byte Arrays */

typedef struct {
  size_t length;
  uint8_t *bytes;
} bytearray_t;

bool is_bytearray_consistent(const bytearray_t *bytearray);

bytearray_t* bytearray_alloc(size_t length);
void bytearray_free_(bytearray_t *bytearray);
#define bytearray_free(bytearray) \
  do { \
    bytearray_free_(bytearray); \
    bytearray = NULL; \
  } while (0)

void bytearray_set_checked(bytearray_t *bytearray, size_t index, uint8_t byte);
uint8_t bytearray_get_checked(const bytearray_t *bytearray, size_t index);
uint8_t *bytearray_pointer_checked(bytearray_t *bytearray, size_t index,
                                   size_t range);

/* Bytes & ASCII Characters */

const uint8_t BYTE_BIT = 8;
const uint8_t ASCII_CHARS_PER_BYTE = BYTE_BIT / CHAR_BIT;

bool char_to_value(char c, char min, char max, uint8_t basis,
                   uint8_t *value_out);
bool value_to_char(uint8_t value, uint8_t min, uint8_t max, char basis,
                   char *char_out);

/* Hexadecimal Characters */

const uint8_t HEX_BIT = 4;
const uint8_t HEX_BASE = 1 << HEX_BIT;
const uint8_t HEX_LSB_MASK = HEX_BASE - 1;
const uint8_t HEX_MSB_MASK = HEX_LSB_MASK << HEX_BIT;

const uint8_t HEXCHARS_PER_BYTE = CHAR_BIT / HEX_BIT;
/* How many characters does it take to encode each byte as \xHH? */
const uint8_t ESCAPED_HEXCHARS_PER_BYTE = HEXCHARS_PER_BYTE + 2;

const char INVALID_HEXCHAR = 0;

/* Accept hex inputs or produce hex outputs with the specified letter case(s).
 */
typedef enum {
  HEXCHAR_ACCEPT_ANY_CASE,
  HEXCHAR_ACCEPT_LOWERCASE_ONLY,
  HEXCHAR_OUTPUT_LOWERCASE = HEXCHAR_ACCEPT_LOWERCASE_ONLY,
  HEXCHAR_ACCEPT_UPPERCASE_ONLY,
  HEXCHAR_OUTPUT_UPPERCASE = HEXCHAR_ACCEPT_UPPERCASE_ONLY
} hex_case_t;

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

bool is_byte_ascii_printable(uint8_t byte);
/* Uses hexadecimal conversion internally */
char *bytearray_to_asciistr(const bytearray_t *bytearray);

/* Base64 Characters */

const uint8_t BASE64_BIT = 6;
const uint8_t BASE64_BASE = 1 << BASE64_BIT;
const uint8_t BASE64_MASK = BASE64_BASE - 1;

/* lowest_common_multiple(BASE64_BIT, BYTE_BIT) */
const uint8_t BASE64_BLOCK_BIT = 24;
const uint8_t BASE64_CHARS_PER_BLOCK = BASE64_BLOCK_BIT / BASE64_BIT;
const uint8_t BASE64_BYTES_PER_BLOCK = BASE64_BLOCK_BIT / BYTE_BIT;

const char INVALID_BASE64CHAR = 0;

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

/* Are the length and bytes fields of bytearray consistent? */
bool
is_bytearray_consistent(const bytearray_t *bytearray)
{
  return (bytearray != NULL
          && ((bytearray->length > 0 && bytearray->bytes != NULL)
              || (bytearray->length == 0 && bytearray->bytes == NULL)));
}

/* Allocate and return a bytearray_t of length using malloc().
 * Must be freed using bytearray_free(). */
bytearray_t *
bytearray_alloc(size_t length)
{
  assert(length > 0);

  bytearray_t * const bytearray = malloc(sizeof(*bytearray));
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

/* Free a bytearray_t allocated using bytearray_alloc().
 * If bytearray is NULL, nothing happens.
 * Use bytearray_free to set bytearray to NULL as well.
 * (Setting a bytearray_t * to NULL after each free is a great way to avoid
 * use-after-free errors - as long as there are no other copies of the
 * pointer.) */
void
bytearray_free_(bytearray_t *bytearray)
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

/* Set bytearray->bytes[index] to byte, checking that bytearray is valid and
 * index is within the bytearray's length. */
void
bytearray_set_checked(bytearray_t *bytearray, size_t index, uint8_t byte)
{
  assert(bytearray);
  assert(is_bytearray_consistent(bytearray));
  assert(index < bytearray->length);
  /* byte can take any valid value for the type */

  bytearray->bytes[index] = byte;

  assert(is_bytearray_consistent(bytearray));
}

/* Return bytearray->bytes[index], checking that bytearray is valid and index
 * is within the bytearray's length. */
uint8_t
bytearray_get_checked(const bytearray_t *bytearray, size_t index)
{
  assert(bytearray);
  assert(is_bytearray_consistent(bytearray));
  assert(index < bytearray->length);

  return bytearray->bytes[index];
}

/* Return &bytearray->bytes[index], checking that bytearray is valid and
 * accesses to index and range bytes starting at index are within the
 * bytearray's length.
 * Accesses to bytearray[index + range] and higher are not checked.
 * range must not be 0. */
uint8_t *
bytearray_pointer_checked(bytearray_t *bytearray, size_t index, size_t range)
{
  assert(bytearray);
  assert(is_bytearray_consistent(bytearray));
  assert(index < bytearray->length);
  assert(range > 0);
  /* This checks access to bytearray[index + range - 1], but not
   * bytearray[index + range] */
  assert(index + range <= bytearray->length);

  return &bytearray->bytes[index];
}

/* If c is between min and max, convert c to an integer, store it 
 * in *value_out, and return true. *value_out is set to basis plus the
 * difference between c and min.
 * If c is not between min and max, leave *value_out unmodified and return
 * false. */
bool
char_to_value(char c, char min, char max, uint8_t basis, uint8_t *value_out)
{
  assert(value_out != NULL);

  if (c >= min && c <= max) {
    /* Check for overflow using a larger type
     * The subtraction can't overflow because c >= min */
    uint16_t value = (uint16_t)basis + ((uint16_t)c - (uint16_t)min);
    assert(value <= UINT8_MAX);
    *value_out = (uint8_t)value;
    return true;
  }

  return false;
}

/* If value is between min and max, convert value to a char, store it
 * in *char_out, and return true. *char_out is set to basis plus the
 * difference between value and min.
 * If value is not between min and max, leave *char_out unmodified and return
 * false. */
bool
value_to_char(uint8_t value, uint8_t min, uint8_t max, char basis,
              char *char_out)
{
  assert(char_out != NULL);

  if (value >= min && value <= max) {
    /* Check for overflow using a larger type
     * The subtraction can't overflow because c >= min */
    uint16_t char_value = (uint16_t)basis + ((uint16_t)value - (uint16_t)min);
    /* If you're using signed chars, you'll get a maximum of 127 here
     * (no wrapping to negative) */
    assert(char_value <= CHAR_MAX);
    *char_out = (char)char_value;
    return true;
  }

  return false;
}

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
  assert(bytearray->length == ((hexstr_len / HEXCHARS_PER_BYTE)
                               + (hexstr_len % HEXCHARS_PER_BYTE)));
  assert(is_bytearray_consistent(bytearray));

  size_t i = 0;
  for (i = 0; i < bytearray->length; i++) {
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
  assert(i == bytearray->length);
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
  const size_t hexstr_len = bytearray->length * HEXCHARS_PER_BYTE + 1;
  char * const hexstr = malloc(hexstr_len);
  assert(hexstr != NULL);
  /* Avoid having to add the terminating nul later */
  memset(hexstr, 0, hexstr_len);

  size_t i = 0;
  for (i = 0; i < bytearray->length; i++) {
    const size_t hexstr_pos = i * HEXCHARS_PER_BYTE;

    /* Don't ever overwrite the terminating nul, and allow for the second
     * hexchar */
    assert(hexstr_pos + 1 < hexstr_len - 1);
    const uint8_t byte = bytearray_get_checked(bytearray, i);
    byte_to_hexpair(byte, &hexstr[hexstr_pos], &hexstr[hexstr_pos + 1]);
  }

  /* Did we actually look at everything, except the terminating nul? */
  assert(i == bytearray->length);
  assert(i * HEXCHARS_PER_BYTE == hexstr_len - 1);

  return hexstr;
}

/* Is byte a printable ASCII character?
 * Assumes that byte will be type cast into an ASCII char.
 * Avoids identifying control characters as printable, because printing them
 * can make a mess of aligned text. (For example: tabs take up multiple
 * columns when printed.) */
bool
is_byte_ascii_printable(uint8_t byte)
{
  return byte >= ' ' && byte <= '~';
}

/* Convert the byte array bytearray into a newly allocated ASCII
 * nul-terminated string, escaping non-printable characters using "\xHH".
 * Never returns a NULL char *. If bytearray has a zero length, the returned
 * char * is "".
 * Outputs lowercase hexadecimal characters in escapes.
 * The caller must free() the returned string. */
char *
bytearray_to_asciistr(const bytearray_t *bytearray)
{
  assert(bytearray != NULL);
  assert(is_bytearray_consistent(bytearray));

  /* One extra byte for the terminating nul */
  const size_t max_asciistr_len = (bytearray->length * ESCAPED_HEXCHARS_PER_BYTE
                                   + 1);
  /* If any bytes are printable ASCII, we will use 1 character for them rather
   * than 4 characters. This wastage is ok. */
  char * const asciistr = malloc(max_asciistr_len);
  assert(asciistr != NULL);
  /* Avoid having to add the terminating nul later */
  memset(asciistr, 0, max_asciistr_len);

  size_t i = 0;
  size_t asciistr_pos = 0;
  for (i = 0; i < bytearray->length; i++) {
    /* Don't ever overwrite the terminating nul, and allow up to
     * ESCAPED_HEXCHARS_PER_BYTE */
    assert(asciistr_pos + (ESCAPED_HEXCHARS_PER_BYTE - 1)
           < max_asciistr_len - 1);

    const uint8_t byte = bytearray_get_checked(bytearray, i);
    if (is_byte_ascii_printable(byte)) {
      asciistr[asciistr_pos] = (char)byte;
      asciistr_pos += ASCII_CHARS_PER_BYTE;
    } else {
      asciistr[asciistr_pos] = '\\';
      asciistr[asciistr_pos + 1] = 'x';
      byte_to_hexpair(byte, &asciistr[asciistr_pos + 2],
                      &asciistr[asciistr_pos + 3]);
      asciistr_pos += ESCAPED_HEXCHARS_PER_BYTE;
    }
  }

  /* Did we actually look at everything, except the terminating nul? */
  assert(i == bytearray->length);
  assert(i * ASCII_CHARS_PER_BYTE <= max_asciistr_len - 1);
  assert(i * ESCAPED_HEXCHARS_PER_BYTE == max_asciistr_len - 1);

  /* Did we end up with a printable string? */
  for (size_t j = 0; j < strlen(asciistr); j++) {
    assert(is_byte_ascii_printable((uint8_t)asciistr[j]));
  }

  return asciistr;
}

/* Does variant accept + as a base64 character? */
bool
is_base64char_plus_accepted(base64_variant_t variant)
{
  return (variant == BASE64_ACCEPT_ANY_VARIANT
          || variant == BASE64_ACCEPT_PLUS_SLASH_ONLY);
}

/* Output + as a base64 character based on variant? */
bool
is_base64char_plus_output(base64_variant_t variant)
{
  assert(variant != BASE64_ACCEPT_ANY_VARIANT);
  return variant == BASE64_ACCEPT_PLUS_SLASH_ONLY;
}

/* Does variant accept / as a base64 character? */
bool
is_base64char_slash_accepted(base64_variant_t variant)
{
  return (variant == BASE64_ACCEPT_ANY_VARIANT
          || variant == BASE64_ACCEPT_PLUS_SLASH_ONLY);
}

/* Output / as a base64 character based on variant? */
bool
is_base64char_slash_output(base64_variant_t variant)
{
  assert(variant != BASE64_ACCEPT_ANY_VARIANT);
  return variant == BASE64_ACCEPT_PLUS_SLASH_ONLY;
}

/* Does variant accept - as a base64 character? */
bool
is_base64char_dash_accepted(base64_variant_t variant)
{
  return (variant == BASE64_ACCEPT_ANY_VARIANT
          || variant == BASE64_ACCEPT_DASH_UNDERSCORE_ONLY);
}

/* Output - as a base64 character based on variant? */
bool
is_base64char_dash_output(base64_variant_t variant)
{
  assert(variant != BASE64_ACCEPT_ANY_VARIANT);
  return variant == BASE64_ACCEPT_DASH_UNDERSCORE_ONLY;
}

/* Does variant accept . as a base64 character? */
bool
is_base64char_period_accepted(base64_variant_t variant)
{
  return (variant == BASE64_ACCEPT_ANY_VARIANT
          || variant == BASE64_ACCEPT_PERIOD_UNDERSCORE_ONLY);
}

/* Output . as a base64 character based on variant? */
bool
is_base64char_period_output(base64_variant_t variant)
{
  assert(variant != BASE64_ACCEPT_ANY_VARIANT);
  return variant == BASE64_ACCEPT_PERIOD_UNDERSCORE_ONLY;
}

/* Does variant accept _ as a base64 character? */
bool
is_base64char_underscore_accepted(base64_variant_t variant)
{
  return (variant == BASE64_ACCEPT_ANY_VARIANT
          || variant == BASE64_ACCEPT_DASH_UNDERSCORE_ONLY
          || variant == BASE64_ACCEPT_PERIOD_UNDERSCORE_ONLY);
}

/* Output _ as a base64 character based on variant? */
bool
is_base64char_underscore_output(base64_variant_t variant)
{
  assert(variant != BASE64_ACCEPT_ANY_VARIANT);
  return (variant == BASE64_ACCEPT_DASH_UNDERSCORE_ONLY
          || variant == BASE64_ACCEPT_PERIOD_UNDERSCORE_ONLY);
}

/* Is base64char a valid base64 padding character? */
bool
is_base64char_valid_padding(char base64char)
{
  /* We only support '=' as a valid base64 padding character */
  return base64char == '=';
}

/* Is base64char a valid base64 character from variant?
 * Also accepts padding bytes if accept_padding is true. */
bool
is_base64char_valid(char base64char, base64_variant_t variant,
                    bool accept_padding)
{
  const bool accept_plus = is_base64char_plus_accepted(variant);
  const bool accept_slash = is_base64char_slash_accepted(variant);
  const bool accept_dash = is_base64char_dash_accepted(variant);
  const bool accept_period = is_base64char_period_accepted(variant);
  const bool accept_underscore = is_base64char_underscore_accepted(variant);

  /* 0 - 25 */
  if (base64char >= 'A' && base64char <= 'Z') {
    return true;
  }

  /* 26 - 51 */
  if (base64char >= 'a' && base64char <= 'z') {
    return true;
  }

  /* 52 - 61 */
  if (base64char >= '0' && base64char <= '9') {
    return true;
  }

  /* Now handle the variants for 62 & 63 */

  /* 62 standard */
  if (accept_plus && base64char == '+') {
    return true;
  }

  /* 63 standard */
  if (accept_slash && base64char == '/') {
    return true;
  }

  /* 62 base64url */
  if (accept_dash && base64char == '-') {
    return true;
  }

  /* 62 YUI / Programming Language Identifiers variant 2 */
  if (accept_period && base64char == '.') {
    return true;
  }

  /* 63 base64url & YUI / Programming Language Identifiers variant 2 */
  if (accept_underscore && base64char == '_') {
    return true;
  }

  if (accept_padding && is_base64char_valid_padding(base64char)) {
    return 1;
  }
  
  return false;
}

/* Convert the character base64char into the equivalent value.
 * Accepts base64 characters based on variant.
 * Does not accept base64 padding characters. */
uint8_t
base64char_to_value(char base64char, base64_variant_t variant)
{
  assert(is_base64char_valid(base64char, variant, false));

  uint8_t value = 0;

  /* We convert all base64 variants, regardless of variant.
   * (It's checked in the precondition.) */

  if (char_to_value(base64char, 'A', 'Z', 0, &value)) {
    /* value has been set to the correct value */
  } else if (char_to_value(base64char, 'a', 'z', ('Z' - 'A' + 1), &value)) {
    /* value has been set to the correct value */
  } else if (char_to_value(base64char, '0', '9',
                           ('Z' - 'A' + 1) + ('z' - 'a' + 1),
                          &value)) {
    /* value has been set to the correct value */
  } else if (base64char == '+' || base64char == '-' || base64char == '.') {
    value = 62;
  } else if (base64char == '/' || base64char == '_') {
    value = 63;
  } else {
    abort();
  }

  assert(value < BASE64_BASE);
  return value;
}

/* Convert value into its equivalent base64 character.
 * Outputs base64 characters based on variant. */
char
value_to_base64char(uint8_t value, base64_variant_t variant)
{
  assert(value < BASE64_BASE);

  char base64char = INVALID_BASE64CHAR;

  if (value_to_char(value, 0, 25, 'A', &base64char)) {
    /* base64char has been set to the correct char */
  } else if (value_to_char(value, 26, 51, 'a', &base64char)) {
    /* base64char has been set to the correct char */
  } else if (value_to_char(value, 52, 61, '0', &base64char)) {
    /* base64char has been set to the correct char */
  } else if (value == 62) {
    if (is_base64char_plus_output(variant)) {
      base64char = '+';
    } else if (is_base64char_dash_output(variant)) {
      base64char = '-';
    } else if (is_base64char_period_output(variant)) {
      base64char = '.';
    } else {
      abort();
    }
  } else if (value == 63) {
    if (is_base64char_slash_output(variant)) {
      base64char = '/';
    } else if (is_base64char_underscore_output(variant)) {
      base64char = '_';
    } else {
      abort();
    }
  } else {
    abort();
  }

  assert(is_base64char_valid(base64char, variant, false));
  return base64char;
}

/* Convert the 4 base64 characters base64chars into the equivalent 3 byte
 * values bytes_out.
 * base64chars[0] becomes the high-order 6 bits, and so on...
 * Accepts any of the compatible variants in base64_variant_t.
 * Ignores padding characters, leaving the corresponding bits 0. */
void
base64chars_to_bytes(const char base64chars[BASE64_CHARS_PER_BLOCK],
                     uint8_t bytes_out[BASE64_BYTES_PER_BLOCK])
{
  /* Check inputs are valid */
  assert(base64chars != NULL);
  assert(bytes_out != NULL);
  const base64_variant_t variant = BASE64_ACCEPT_ANY_VARIANT;

  for (size_t i = 0; i < BASE64_CHARS_PER_BLOCK; i++) {
    assert(is_base64char_valid(base64chars[i], variant,
                               true));
  }

  /* Initialise output array
   * This means we don't need special handing when we skip padding characters */
  for (size_t i = 0; i < BASE64_BYTES_PER_BLOCK; i++) {
    bytes_out[i] = 0;
  }

  /* Convert from characters */
  const uint8_t v0 = base64char_to_value(base64chars[0], variant);
  const uint8_t v1 = base64char_to_value(base64chars[1], variant);
  const uint8_t v2 = base64char_to_value(base64chars[2], variant);
  const uint8_t v3 = base64char_to_value(base64chars[3], variant);

  /* Then rearrange some bits */
  bytes_out[0] = (uint8_t)((v0 << 2) | (v1 >> 4));
  bytes_out[1] = (uint8_t)((v1 << 4) | (v2 >> 2));
  bytes_out[2] = (uint8_t)((v2 << 6) | v3);

  /* bytes_out[0..2] can take any valid value for the type */
}

/* Convert the 3 bytes into the equivalent 4 base64 characters base64chars_out.
 * bytes[0] becomes base64chars_out[0] and the high-order 2 bits of
 * base64chars_out[1], and so on...
 * Outputs the BASE64_OUTPUT_PLUS_SLASH variant base64 characters. */
void
bytes_to_base64chars(const uint8_t bytes[BASE64_BYTES_PER_BLOCK],
                     char base64chars_out[BASE64_CHARS_PER_BLOCK])
{
  assert(bytes != NULL);
  /* bytes[0..2] can take any valid value for the type */
  assert(base64chars_out != NULL);
  const base64_variant_t variant = BASE64_OUTPUT_PLUS_SLASH;

  /* Initialise output array to invalid values
   * Later, we check that we've written a valid value to each base64 char */
  for (size_t i = 0; i < BASE64_CHARS_PER_BLOCK; i++) {
    base64chars_out[i] = INVALID_BASE64CHAR;
  }

  /* Rearrange some bits */
  const uint8_t v0 = (uint8_t)(bytes[0] >> 2);
  const uint8_t v1 = (uint8_t)(((bytes[0] & (BASE64_MASK >> 4)) << 4) | (bytes[1] >> 4));
  const uint8_t v2 = (uint8_t)(((bytes[1] & (BASE64_MASK >> 2)) << 2) | (bytes[2] >> 6));
  const uint8_t v3 = (uint8_t)(bytes[2] & BASE64_MASK);

  /* Then convert to characters */
  base64chars_out[0] = value_to_base64char(v0, variant);
  base64chars_out[1] = value_to_base64char(v1, variant);
  base64chars_out[2] = value_to_base64char(v2, variant);
  base64chars_out[3] = value_to_base64char(v3, variant);

  /* Check each output character is valid */
  for (size_t i = 0; i < BASE64_CHARS_PER_BLOCK; i++) {
    assert(is_base64char_valid(base64chars_out[i], variant,
                               false));
  }
}

/* Convert the nul-terminated base64 string into a newly allocated
 * array of bytes.
 * If base64str partially fills the final bytes, the remaining bits are zero
 * (that is, we act like it has extra 'A's at the end of the string).
 * Never returns a NULL bytearray_t *. If base64str is "", the returned
 * bytearray_t * has a zero length and NULL bytes pointer.
 * Accepts any of the compatible variants in base64_variant_t.
 * Strings must not have any non-base64 characters, including whitespace.
 * The caller must bytearray_free() the returned bytearray_t. */
bytearray_t *
base64str_to_bytearray(const char *base64str)
{
  /* base64str can be of arbitrary length, including zero */
  const size_t base64str_len = strlen(base64str);

  bytearray_t *bytearray = NULL;

  /* round up the length to the nearest block (3 bytes) if the base64 characters
   * don't fit evenly into a block */
  const size_t base64_block_count = ceil_div(base64str_len,
                                             BASE64_CHARS_PER_BLOCK);
  assert(base64_block_count == ceil_div(base64str_len * BASE64_BIT,
                                        BASE64_BLOCK_BIT));
  bytearray = bytearray_alloc(base64_block_count * BASE64_BYTES_PER_BLOCK);
  assert(bytearray->length / base64_block_count == BASE64_BYTES_PER_BLOCK);
  assert(is_bytearray_consistent(bytearray));

  size_t i = 0;
  for (i = 0; i < base64_block_count; i++) {
    const size_t base64str_pos = i * BASE64_CHARS_PER_BLOCK;
    const size_t bytearray_pos = i * BASE64_BYTES_PER_BLOCK;

    assert(base64str_pos < base64str_len);
    /* Allow for the entire block */
    assert(bytearray_pos + BASE64_BYTES_PER_BLOCK - 1 < bytearray->length);

    char base64_char_block[BASE64_CHARS_PER_BLOCK];

    for (size_t j = 0; j < BASE64_CHARS_PER_BLOCK; j++) {
      if (base64str_pos + j < base64str_len) {
        assert(is_base64char_valid(base64str[base64str_pos + j],
                                   BASE64_ACCEPT_ANY_VARIANT, true));
        base64_char_block[j] = base64str[base64str_pos + j];
      } else {
        /* if we're missing a base64char for the final block, act like it's 'A' */
        base64_char_block[j] = 'A';
      }
    }

    uint8_t * const bytearray_pointer = bytearray_pointer_checked(
                                                      bytearray,
                                                      bytearray_pos,
                                                      BASE64_BYTES_PER_BLOCK);
    base64chars_to_bytes(base64_char_block, bytearray_pointer);

    assert(is_bytearray_consistent(bytearray));
  }

  /* Did we actually look at everything? */
  assert(i == bytearray->length / BASE64_BYTES_PER_BLOCK);
  assert(i == ceil_div(base64str_len, BASE64_CHARS_PER_BLOCK));

  assert(is_bytearray_consistent(bytearray));

  /* The bytes in bytearray can take on any value */
  return bytearray;
}

/* Convert the byte array bytearray into a newly allocated base64
 * nul-terminated string.
 * Never returns a NULL char *. If bytearray has a zero length, the returned
 * char * is "".
 * Outputs the BASE64_OUTPUT_PLUS_SLASH variant base64 characters.
 * Outputs raw base64 without trailing padding characters or whitespace.
 * (The output is rounded up to the nearest 24 bits, and any bits without
 * corresponding bytearray bytes are set to zero.)
 * The caller must free() the returned string. */
char *
bytearray_to_base64str(const bytearray_t *bytearray)
{
  assert(bytearray != NULL);
  assert(is_bytearray_consistent(bytearray));

  /* round up the length to the nearest block (4 base64 chars) if the bytes
   * don't fit evenly into a block */
  const size_t base64_block_count = ceil_div(bytearray->length,
                                             BASE64_BYTES_PER_BLOCK);
  assert(base64_block_count == ceil_div(bytearray->length * BYTE_BIT,
                                        BASE64_BLOCK_BIT));
  /* One extra byte for the terminating nul */
  const size_t base64str_len = base64_block_count * BASE64_CHARS_PER_BLOCK + 1;
  if (base64_block_count > 0) {
    assert((base64str_len - 1) / base64_block_count == BASE64_CHARS_PER_BLOCK);
  } else {
    assert((base64str_len - 1) == 0);
  }
  char * const base64str = malloc(base64str_len);
  assert(base64str != NULL);
  /* Avoid having to add the terminating nul later */
  memset(base64str, 0, base64str_len);

  size_t i = 0;
  for (i = 0; i < base64_block_count; i++) {
    const size_t base64str_pos = i * BASE64_CHARS_PER_BLOCK;
    const size_t bytearray_pos = i * BASE64_BYTES_PER_BLOCK;

    /* Allow for the entire block */
    assert(base64str_pos + BASE64_CHARS_PER_BLOCK - 1 < base64str_len - 1);
    assert(bytearray_pos < bytearray->length);

    uint8_t base64_byte_block[BASE64_BYTES_PER_BLOCK];

    for (size_t j = 0; j < BASE64_BYTES_PER_BLOCK; j++) {
      if (bytearray_pos + j < bytearray->length) {
        base64_byte_block[j] = bytearray_get_checked(bytearray,
                                                     bytearray_pos + j);
      } else {
        /* if we're missing a byte for the final block, act like it's 0 */
        base64_byte_block[j] = 0;
      }
    }

    assert(base64str_pos < base64str_len - 1);
    assert(base64str_pos + BASE64_CHARS_PER_BLOCK <= base64str_len - 1);
    bytes_to_base64chars(base64_byte_block, &base64str[base64str_pos]);

    assert(is_bytearray_consistent(bytearray));
  }

  /* Did we actually look at everything? */
  assert(i == ceil_div(bytearray->length, BASE64_BYTES_PER_BLOCK));
  assert(i == (base64str_len - 1) / BASE64_CHARS_PER_BLOCK);

  /* Check each character is valid base64 */
  for (size_t k = 0; k < strlen(base64str); k++) {
    assert(is_base64char_valid(base64str[k], BASE64_OUTPUT_PLUS_SLASH,
                               false));
  }

  return base64str;
}

int
main(int argc, const char * argv[])
{
  (void)argc;
  (void)argv;

  printf("Hex:                 %s\n", input_hexstr);

  bytearray_t *input_bytearray = hexstr_to_bytearray(input_hexstr);
  printf("Bytes:               %s\n", (char *)input_bytearray->bytes);

  char *input_asciistr = bytearray_to_asciistr(input_bytearray);
  printf("Escaped Bytes:       %s\n", input_asciistr);

  char *input_base64str = bytearray_to_base64str(input_bytearray);
  printf("Base64:              %s\n", input_base64str);

  printf("Base64 Expected:     %s\n", expected_base64str);

  if (!strcmp(input_base64str, expected_base64str)) {
    printf("Calculated Output matches Expected Output\n");
  } else {
    printf("Calculated Output differs from Expected Output\n");
  }

  bytearray_t *expected_bytearray = base64str_to_bytearray(expected_base64str);
  printf("Bytes Expected:      %s\n", (char *)expected_bytearray->bytes);

  char *expected_asciistr = bytearray_to_asciistr(expected_bytearray);
  printf("Escaped Expected:    %s\n", expected_asciistr);

  char *expected_hexstr = bytearray_to_hexstr(expected_bytearray);
  printf("Expected Hex:        %s\n", expected_hexstr);

  if (!strcmp(input_hexstr, expected_hexstr)) {
    printf("Calculated Input matches Actual Input\n");
  } else {
    printf("Calculated Input differs from Actual Input\n");
  }

  bytearray_free(input_bytearray);
  free(input_asciistr);
  free(input_base64str);
  bytearray_free(expected_bytearray);
  free(expected_asciistr);
  free(expected_hexstr);
  return 0;
}

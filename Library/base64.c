//
//  base64.c
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

#include "base64.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "bytearray.h"
#include "calc.h"

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
  assert(bytearray_length(bytearray) / base64_block_count
         == BASE64_BYTES_PER_BLOCK);
  assert(is_bytearray_consistent(bytearray));

  size_t i = 0;
  for (i = 0; i < base64_block_count; i++) {
    const size_t base64str_pos = i * BASE64_CHARS_PER_BLOCK;
    const size_t bytearray_pos = i * BASE64_BYTES_PER_BLOCK;

    assert(base64str_pos < base64str_len);
    /* Allow for the entire block */
    assert(bytearray_pos + BASE64_BYTES_PER_BLOCK - 1
           < bytearray_length(bytearray));

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
  assert(i == bytearray_length(bytearray) / BASE64_BYTES_PER_BLOCK);
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
  const size_t base64_block_count = ceil_div(bytearray_length(bytearray),
                                             BASE64_BYTES_PER_BLOCK);
  assert(base64_block_count == ceil_div(bytearray_length(bytearray) * BYTE_BIT,
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
    assert(bytearray_pos < bytearray_length(bytearray));

    uint8_t base64_byte_block[BASE64_BYTES_PER_BLOCK];

    for (size_t j = 0; j < BASE64_BYTES_PER_BLOCK; j++) {
      if (bytearray_pos + j < bytearray_length(bytearray)) {
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
  assert(i == ceil_div(bytearray_length(bytearray), BASE64_BYTES_PER_BLOCK));
  assert(i == (base64str_len - 1) / BASE64_CHARS_PER_BLOCK);

  /* Check each character is valid base64 */
  for (size_t k = 0; k < strlen(base64str); k++) {
    assert(is_base64char_valid(base64str[k], BASE64_OUTPUT_PLUS_SLASH,
                               false));
  }
  
  return base64str;
}

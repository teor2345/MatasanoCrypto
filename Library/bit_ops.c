//
//  bit_ops.c
//  MatasanoCrypto
//
//  Created by Tim Wilson-Brown on 4/01/2016.
//  Copyright © 2016 teor.
//  GPLv3
//

#include "bit_ops.h"

#include <assert.h>
#include <sys/param.h>
#include <sys/types.h>

#include "bytearray.h"

#include "char.h"

/* XOR the bytearrays b1 and b2 into a newly allocated bytearray.
 * If b1 and b2 are different lengths, the shorter bytearray is XORed
 * repeatedly into the longer bytearray. The returned bytearray is as long as
 * the longer input bytearray.
 * If either bytearray has zero length, the returned bytearray is a copy of the
 * other bytearray. If both bytearrays have zero length, the returned
 * bytearray has zero lengt and NULL bytes pointer.
 * Never returns a NULL bytearray *.
 * The caller must bytearray_free() the returned bytearray_t. */
bytearray_t *
bytearray_xor(const bytearray_t *b1, const bytearray_t *b2)
{
  assert(b1 != NULL);
  assert(b2 != NULL);

  assert(is_bytearray_consistent(b1));
  assert(is_bytearray_consistent(b2));

  /* If either bytearray is zero length, return a copy of the other bytearray
   * (if both are zero length, this returns a zero length bytearray) */
  if (bytearray_length(b1) == 0 || bytearray_length(b2) == 0) {
    const bytearray_t *src = (bytearray_length(b1) > 0 ? b1 : b2);

    /* don't repeat the assertions in bytearray_dup */
    return bytearray_dup(src);
  } else {
    bytearray_t * const result = bytearray_alloc(MAX(bytearray_length(b1),
                                                     bytearray_length(b2)));
    assert(result != NULL);
    assert(is_bytearray_consistent(result));

    for (size_t i = 0; i < bytearray_length(result); i++) {
      uint8_t byte1 = bytearray_get_checked(b1, i % bytearray_length(b1));
      uint8_t byte2 = bytearray_get_checked(b2, i % bytearray_length(b2));

      uint8_t byte_result = byte1 ^ byte2;
      bytearray_set_checked(result, i, byte_result);

      assert(is_bytearray_consistent(result));
    }

    assert(is_bytearray_consistent(result));
    return result;
  }
}

/* Like bytearray_xor, but takes a single byte to XOR for convenience. */
bytearray_t *
bytearray_xor_byte(const bytearray_t *bytearray, uint8_t byte)
{
  /* Since this is a convenience function, we avoid repeating the assertions
   * from bytearray_xor. */

  bytearray_t *b_byte = bytearray_alloc(sizeof(byte));
  bytearray_set_checked(b_byte, 0, byte);

  bytearray_t * const result = bytearray_xor(bytearray, b_byte);

  bytearray_free(b_byte);

  return result;
}

/* Count the number of bits set in b, and return it. */
size_t
bytearray_get_bit_count(const bytearray_t *b)
{
  size_t result = 0;

  for (size_t i = 0; i < bytearray_length(b); i++) {
    uint8_t byte = bytearray_get_checked(b, i);
    result += byte_get_bit_count(byte);
  }

  /* The result is at most the number of bits in b */
  assert(result <= bytearray_length(b) * BYTE_BIT);
  return result;
}


/* Return the hamming distance between b1 and b2.
 * b1 and b2 must be the same length, and have at most size_t bits. */
size_t
bytearray_hamming(const bytearray_t *b1, const bytearray_t *b2)
{
  assert(b1 != NULL);
  assert(b2 != NULL);

  assert(bytearray_length(b1) == bytearray_length(b2));
  assert(bytearray_length(b1) <= SIZE_T_MAX / BYTE_BIT);

  bytearray_t *nonmatching = bytearray_xor(b1, b2);
  assert(nonmatching != NULL);
  assert(is_bytearray_consistent(nonmatching));

  return bytearray_get_bit_count(nonmatching);
}

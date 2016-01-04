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
  if (b1->length == 0 || b2->length == 0) {
    const bytearray_t *src = (b1->length > 0 ? b1 : b2);

    /* don't repeat the assertions in bytearray_dup */
    return bytearray_dup(src);
  } else {
    bytearray_t * const result = bytearray_alloc(MAX(b1->length, b2->length));
    assert(result != NULL);
    assert(is_bytearray_consistent(result));

    for (size_t i = 0; i < result->length; i++) {
      uint8_t byte1 = bytearray_get_checked(b1, i % b1->length);
      uint8_t byte2 = bytearray_get_checked(b2, i % b2->length);

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

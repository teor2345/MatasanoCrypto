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
 * the longer
 * Never returns a NULL bytearray *. If either bytearray has a zero length,
 * the returned bytearray has a zero length and NULL bytes pointer.
 * The caller must bytearray_free() the returned bytearray_t. */
bytearray_t *
bytearray_xor(const bytearray_t *b1, const bytearray_t *b2)
{
  assert(b1 != NULL);
  assert(b2 != NULL);

  assert(is_bytearray_consistent(b1));
  assert(is_bytearray_consistent(b2));

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

//
//  bytearray.c
//  MatasanoCrypto
//
//  Created by Tim Wilson-Brown on 2/01/2016.
//  Copyright © 2016 teor.
//  GPLv3
//

#include "bytearray.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

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

/* Return a newly allocated bytearray that has the same length and content as
 * src.
 * Must be freed using bytearray_free(). */
bytearray_t *
bytearray_dup(const bytearray_t *src)
{
  assert(src != NULL);
  assert(is_bytearray_consistent(src));

  bytearray_t * const result = bytearray_alloc(src->length);
  assert(result != NULL);
  assert(is_bytearray_consistent(result));
  assert(result->length == src->length);

  for (size_t i = 0; i < result->length; i++) {
    uint8_t byte = bytearray_get_checked(src, i);
    bytearray_set_checked(result, i, byte);
  }

  assert(is_bytearray_consistent(result));
  return result;
}

/* Return a newly allocated bytearray that is length long and has the same
 * content as buf.
 * bytes can only be NULL if length is zero, in this case, a non-NULL, empty
 * bytearray is returned.
 * Must be freed using bytearray_free(). */
bytearray_t *
bytes_to_bytearray(const uint8_t *bytes, size_t length)
{
  if (length > 0) {
    assert(bytes != NULL);
  }

  bytearray_t * const bytearray = bytearray_alloc(length);
  assert(bytearray != NULL);
  assert(is_bytearray_consistent(bytearray));

  memcpy(bytearray->bytes, bytes, length);
  bytearray->length = length;
  assert(is_bytearray_consistent(bytearray));

  return bytearray;
}

/* Return a newly allocated bytearray that has the same content as str,
 * excluding the terminating nul character.
 * If str is NULL or "", a non-NULL, empty bytearray is returned.
 * Must be freed using bytearray_free(). */
bytearray_t *
str_to_bytearray(const char *str)
{
  return bytes_to_bytearray((uint8_t *)str, strlen(str));
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

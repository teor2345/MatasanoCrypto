//
//  score.c
//  MatasanoCrypto
//
//  Created by Tim Wilson-Brown on 4/01/2016.
//  Copyright © 2016 teor.
//  GPLv3
//

#include "score.h"

#include <assert.h>

#include "bytearray.h"

/* For is_byte_ascii_printable() */
#include "hex.h"


size_t
count_printable(const bytearray_t *bytearray)
{
  assert(bytearray != NULL);
  assert(is_bytearray_consistent(bytearray));

  size_t result = 0;

  for (size_t i = 0; i < bytearray->length; i++) {
    uint8_t byte = bytearray_get_checked(bytearray, i);
    
    if (is_byte_ascii_printable(byte)) {
      result++;
    }
  }

  assert(result <= bytearray->length);

  return result;
}

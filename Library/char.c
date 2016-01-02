//
//  char.c
//  MatasanoCrypto
//
//  Created by Tim Wilson-Brown on 2/01/2016.
//  Copyright © 2016 teor. All rights reserved.
//

#include "char.h"

#include <assert.h>

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

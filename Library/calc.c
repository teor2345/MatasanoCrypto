//
//  calc.c
//  MatasanoCrypto
//
//  Created by Tim Wilson-Brown on 2/01/2016.
//  Copyright © 2016 teor.
//  GPLv3
//

#include "calc.h"

#include <assert.h>

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

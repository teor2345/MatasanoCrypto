//
//  safeint.c
//  MatasanoCrypto
//
//  Created by Tim Wilson-Brown on 17/01/2016.
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

#include "safeint.h"

#include <assert.h>
#include <stdint.h>

/* If adding two size_t values cannot overflow a uintmax_t value */
#if SIZE_MAX <= UINTMAX_MAX/2
typedef uintmax_t uwideint_t;
#else
typedef __uint128_t uwideint_t;
#endif

/* Check if a + b overflows, returning 1 on overflow.
 * Set rp to a + b if the sum does not overflow.
 * Based on checked_add_1 from http://blog.regehr.org/archives/1139 */
bool checked_add(size_t a, size_t b, size_t *rp) {
  /* Check that adding two size_t values cannot overflow a uwideint_t value */
  assert(sizeof(uwideint_t) > sizeof(size_t));
  assert(rp != NULL);
  
  uwideint_t lr = (uwideint_t)a + (uwideint_t)b;
  *rp = (size_t)lr;
  return lr > SIZE_MAX;
}

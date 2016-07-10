//
//  calc.c
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

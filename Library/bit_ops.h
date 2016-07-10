//
//  bit_ops.h
//  MatasanoCrypto
//
//  Created by Tim Wilson-Brown on 4/01/2016.
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

#ifndef bit_ops_h
#define bit_ops_h

#include <sys/types.h>

/* Forward Declarations */

typedef struct bytearray_t bytearray_t;

/* Function Declarations */

bytearray_t *bytearray_xor(const bytearray_t *b1, const bytearray_t *b2);
bytearray_t *bytearray_xor_byte(const bytearray_t *bytearray, uint8_t byte);

size_t bytearray_get_bit_count(const bytearray_t *b);
size_t bytearray_hamming(const bytearray_t *b1, const bytearray_t *b2);

#endif /* bit_ops_h */

//
//  bytearray.h
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

#ifndef bytearray_h
#define bytearray_h

#include <stdbool.h>
#include <sys/types.h>

/* Forward Declarations */

typedef struct bytearray_t bytearray_t;

/* Function Declarations */

bool is_bytearray_consistent(const bytearray_t *bytearray);

bytearray_t* bytearray_alloc(size_t length);
void bytearray_free_(bytearray_t *bytearray);
#define bytearray_free(bytearray) \
  do { \
    bytearray_free_(bytearray); \
    bytearray = NULL; \
  } while (0)

bytearray_t *bytearray_dup(const bytearray_t *src);
bytearray_t *bytearray_cat(const bytearray_t *src1, const bytearray_t *src2);

size_t bytearray_length(const bytearray_t *bytearray);

bytearray_t *bytes_to_bytearray(const uint8_t *bytes, size_t length);
bytearray_t *str_to_bytearray(const char *str);

void bytearray_set_checked(bytearray_t *bytearray, size_t index, uint8_t byte);
uint8_t bytearray_get_checked(const bytearray_t *bytearray, size_t index);
uint8_t *bytearray_pointer_checked(bytearray_t *bytearray, size_t index,
                                   size_t range);

#endif /* bytearray_h */

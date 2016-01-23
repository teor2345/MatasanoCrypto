//
//  bit_ops.h
//  MatasanoCrypto
//
//  Created by Tim Wilson-Brown on 4/01/2016.
//  Copyright © 2016 teor.
//  GPLv3
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

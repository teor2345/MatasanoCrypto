//
//  score.h
//  MatasanoCrypto
//
//  Created by Tim Wilson-Brown on 4/01/2016.
//  Copyright © 2016 teor.
//  GPLv3
//

#ifndef score_h
#define score_h

#include <sys/types.h>

/* Forward Declarations */
typedef struct bytearray_t bytearray_t;

size_t count_printable(const bytearray_t *bytearray);
#define count_unprintable(b) ((b)->length - count_printable(b))

#endif /* score_h */

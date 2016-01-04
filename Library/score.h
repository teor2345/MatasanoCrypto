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

#include <stdbool.h>
#include <sys/types.h>

/* Forward Declarations */
typedef struct bytearray_t bytearray_t;

bool is_byte_ascii_printable(uint8_t byte);
/* Uses hexadecimal conversion internally */
char *bytearray_to_asciistr(const bytearray_t *bytearray);

size_t count_printable(const bytearray_t *bytearray);
/* Return the number of unprintable ASCII characters in bytearray. */
#define count_unprintable(b) ((b)->length - count_printable(b))

#endif /* score_h */

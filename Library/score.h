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
bool is_byte_ascii_letter(uint8_t byte);
bool is_byte_ascii_space(uint8_t byte);

char *bytearray_to_asciistr(const bytearray_t *bytearray);

size_t count_printable(const bytearray_t *bytearray);
/* Return the number of unprintable ASCII characters in bytearray. */
#define count_unprintable(b) ((b)->length - count_printable(b))

size_t count_space(const bytearray_t *bytearray);
/* Return the number of non-space ASCII characters in bytearray. */
#define count_nonspace(b) ((b)->length - count_space(b))

size_t count_letter(const bytearray_t *bytearray, bool include_space);
/* Return the number of non-letter ASCII characters in bytearray. */
#define count_nonletter(b, include_space) \
                            ((b)->length - count_letter(b, !(include_space)))

size_t count_byte(const bytearray_t *bytearray, uint8_t byte);
/* Return the number of non-matching bytes in bytearray. */
#define count_nonbyte(b, byte) \
                               ((b)->length - count_byte(b, byte))

double score_english_letter_frequency(const bytearray_t *bytearray);

#endif /* score_h */

//
//  bytearray.h
//  MatasanoCrypto
//
//  Created by Tim Wilson-Brown on 2/01/2016.
//  Copyright © 2016 teor.
//  GPLv3
//

#ifndef bytearray_h
#define bytearray_h

#include <stdbool.h>
#include <sys/types.h>

typedef struct bytearray_t {
  size_t length;
  uint8_t *bytes;
} bytearray_t;

bool is_bytearray_consistent(const bytearray_t *bytearray);

bytearray_t* bytearray_alloc(size_t length);
void bytearray_free_(bytearray_t *bytearray);
#define bytearray_free(bytearray) \
  do { \
    bytearray_free_(bytearray); \
    bytearray = NULL; \
  } while (0)

bytearray_t *bytearray_dup(const bytearray_t *src);

void bytearray_set_checked(bytearray_t *bytearray, size_t index, uint8_t byte);
uint8_t bytearray_get_checked(const bytearray_t *bytearray, size_t index);
uint8_t *bytearray_pointer_checked(bytearray_t *bytearray, size_t index,
                                   size_t range);

#endif /* bytearray_h */

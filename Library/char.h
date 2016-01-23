//
//  char.h
//  MatasanoCrypto
//
//  Created by Tim Wilson-Brown on 2/01/2016.
//  Copyright © 2016 teor.
//  GPLv3
//

#ifndef char_h
#define char_h

#include <limits.h>
#include <stdbool.h>
#include <sys/types.h>

/* Function Declarations */

static const uint8_t BYTE_BIT = CHAR_BIT;
static const uint8_t ASCII_CHARS_PER_BYTE = 1;

bool char_to_value(char c, char min, char max, uint8_t basis,
                   uint8_t *value_out);
bool value_to_char(uint8_t value, uint8_t min, uint8_t max, char basis,
                   char *char_out);

uint8_t byte_get_bit_count(uint8_t byte);

#endif /* char_h */

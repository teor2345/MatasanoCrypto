//
//  score.c
//  MatasanoCrypto
//
//  Created by Tim Wilson-Brown on 4/01/2016.
//  Copyright © 2016 teor.
//  GPLv3
//

#include "score.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "bytearray.h"
#include "char.h"
#include "hex.h"

/* Is byte a printable ASCII character?
 * Assumes that byte will be type cast into an ASCII char.
 * Avoids identifying control characters as printable, because printing them
 * can make a mess of aligned text. (For example: tabs take up multiple
 * columns when printed.) */
bool
is_byte_ascii_printable(uint8_t byte)
{
  return byte >= ' ' && byte <= '~';
}

/* Is byte an ASCII letter character?
 * Case-insensitive. */
bool
is_byte_ascii_letter(uint8_t byte)
{
  return ((byte >= 'A' && byte <= 'Z')
          || (byte >= 'a' && byte <= 'z'));
}

/* Is byte an ASCII space character? */
bool
is_byte_ascii_space(uint8_t byte)
{
  return byte == ' ';
}

/* Convert the byte array bytearray into a newly allocated ASCII
 * nul-terminated string, escaping non-printable characters using "\xHH".
 * Never returns a NULL char *. If bytearray has a zero length, the returned
 * char * is "".
 * Outputs lowercase hexadecimal characters in escapes.
 * The caller must free() the returned string. */
char *
bytearray_to_asciistr(const bytearray_t *bytearray)
{
  assert(bytearray != NULL);
  assert(is_bytearray_consistent(bytearray));

  /* One extra byte for the terminating nul */
  const size_t max_asciistr_len = (bytearray->length * ESCAPED_HEXCHARS_PER_BYTE
                                   + 1);
  /* If any bytes are printable ASCII, we will use 1 character for them rather
   * than 4 characters. This wastage is ok. */
  char * const asciistr = malloc(max_asciistr_len);
  assert(asciistr != NULL);
  /* Avoid having to add the terminating nul later */
  memset(asciistr, 0, max_asciistr_len);

  size_t i = 0;
  size_t asciistr_pos = 0;
  for (i = 0; i < bytearray->length; i++) {
    /* Don't ever overwrite the terminating nul, and allow up to
     * ESCAPED_HEXCHARS_PER_BYTE */
    assert(asciistr_pos + (ESCAPED_HEXCHARS_PER_BYTE - 1)
           < max_asciistr_len - 1);

    const uint8_t byte = bytearray_get_checked(bytearray, i);
    if (is_byte_ascii_printable(byte)) {
      asciistr[asciistr_pos] = (char)byte;
      asciistr_pos += ASCII_CHARS_PER_BYTE;
    } else {
      asciistr[asciistr_pos] = '\\';
      asciistr[asciistr_pos + 1] = 'x';
      byte_to_hexpair(byte, &asciistr[asciistr_pos + 2],
                      &asciistr[asciistr_pos + 3]);
      asciistr_pos += ESCAPED_HEXCHARS_PER_BYTE;
    }
  }

  /* Did we actually look at everything, except the terminating nul? */
  assert(i == bytearray->length);
  assert(i * ASCII_CHARS_PER_BYTE <= max_asciistr_len - 1);
  assert(i * ESCAPED_HEXCHARS_PER_BYTE == max_asciistr_len - 1);

  /* Did we end up with a printable string? */
  for (size_t j = 0; j < strlen(asciistr); j++) {
    assert(is_byte_ascii_printable((uint8_t)asciistr[j]));
  }

  return asciistr;
}

typedef bool (*byte_test_func)(uint8_t);

/* Return the number of bytes in bytearray satisfying byte_test. */
static size_t
count_byte_test(const bytearray_t *bytearray, byte_test_func byte_test)
{
  assert(bytearray != NULL);
  assert(is_bytearray_consistent(bytearray));

  size_t result = 0;

  for (size_t i = 0; i < bytearray->length; i++) {
    uint8_t byte = bytearray_get_checked(bytearray, i);

    if (byte_test(byte)) {
      result++;
    }
  }

  assert(result <= bytearray->length);

  return result;
}

/* Return the number of printable ASCII characters in bytearray. */
size_t
count_printable(const bytearray_t *bytearray)
{
  return count_byte_test(bytearray, &is_byte_ascii_printable);
}

/* Return the number of ASCII space characters in bytearray. */
size_t
count_space(const bytearray_t *bytearray)
{
  return count_byte_test(bytearray, &is_byte_ascii_space);
}

/* Return the number of ASCII letter characters in bytearray.
 * Case-insensitive. Inclues spaces if count_space is true. */
size_t
count_letter(const bytearray_t *bytearray, bool count_space)
{
  size_t result = count_byte_test(bytearray, &is_byte_ascii_letter);

  if (count_space) {
    result += count_byte_test(bytearray, &is_byte_ascii_space);
  }

  assert(result <= bytearray->length);

  return result;
}

/* Return the number of bytes matching byte in bytearray. */
size_t
count_byte(const bytearray_t *bytearray, uint8_t byte)
{
  /* I'd love to use count_byte_test, but C doesn't have partially-applied
   * functions */
  assert(bytearray != NULL);
  assert(is_bytearray_consistent(bytearray));

  size_t result = 0;

  for (size_t i = 0; i < bytearray->length; i++) {
    uint8_t ba_byte = bytearray_get_checked(bytearray, i);

    if (byte == ba_byte) {
      result++;
    }
  }

  assert(result <= bytearray->length);

  return result;
}

#define LETTER_COUNT 26

/* Calculate the frequencies of each letter in bytearray and place them in
 * frequencies_out.
 * Case-insensitive.
 * Disregards non-letter characters when calculating frequencies. */
static void
calculate_letter_frequency(const bytearray_t *bytearray,
                                   double frequencies_out[LETTER_COUNT])
{
  assert(bytearray != NULL);
  assert(is_bytearray_consistent(bytearray));

  size_t letter_count[LETTER_COUNT];
  size_t total_letter_count = count_letter(bytearray, 0);

  for (uint8_t i = 0; i < LETTER_COUNT; i++) {
    /* I'd love to use count_byte_test with a case-insensitive letter-matching
     * function, but C doesn't have partially-applied functions */
    char letter = 0;
    bool success = false;

    /* Initialise with lowercase count */
    success = value_to_char(i, 0, LETTER_COUNT - 1, 'a', &letter);
    assert(success);
    letter_count[i] = count_byte(bytearray, (uint8_t)letter);

    /* Add uppercase count */
    success = value_to_char(i, 0, LETTER_COUNT - 1, 'A', &letter);
    assert(success);
    letter_count[i] += count_byte(bytearray, (uint8_t)letter);

    /* Calculate frequency */
    if (total_letter_count > 0) {
      frequencies_out[i] = (double)letter_count[i]/(double)total_letter_count;
    } else {
      frequencies_out[i] = 0.0;
    }
  }
}

/* The average frequencies of the letters a-z in English.
 * https://en.wikipedia.org/wiki/Letter_frequency */
static double english_letter_frequency[LETTER_COUNT] = {
  /* a */ 0.08167,
  /* b */ 0.01492,
  /* c */ 0.02782,
  /* d */ 0.04253,
  /* e */ 0.12702,
  /* f */ 0.02228,
  /* g */ 0.02015,
  /* h */ 0.06094,
  /* i */ 0.06966,
  /* j */ 0.00153,
  /* k */ 0.00772,
  /* l */ 0.04025,
  /* m */ 0.02406,
  /* n */ 0.06749,
  /* o */ 0.07507,
  /* p */ 0.01929,
  /* q */ 0.00095,
  /* r */ 0.05987,
  /* s */ 0.06327,
  /* t */ 0.09056,
  /* u */ 0.02758,
  /* v */ 0.00978,
  /* w */ 0.02361,
  /* x */ 0.00150,
  /* y */ 0.01974,
  /* z */ 0.00074
};

/* Calculate the frequencies of each letter in bytearray and scores them
 * against typical English text frequencies. Lower values are better.
 * Case-insensitive.
 * Disregards non-letter characters when calculating frequencies. */
double
score_english_letter_frequency(const bytearray_t *bytearray)
{
  assert(bytearray != NULL);
  assert(is_bytearray_consistent(bytearray));

  double letter_frequency[LETTER_COUNT];
  calculate_letter_frequency(bytearray, letter_frequency);

  /* Calculate root-mean-squares using the differences between each letter's
   * actual frequency, and the average English frequency.
   * TODO: is there a better statistical function for this? */
  double sum_of_squares = 0.0;
  for (uint8_t i = 0; i < LETTER_COUNT; i++) {
    double difference = letter_frequency[i] - english_letter_frequency[i];
    sum_of_squares += difference*difference;
  }

  double root_mean_squares = sqrt(sum_of_squares/LETTER_COUNT);

  assert(root_mean_squares >= 0.0);
  return root_mean_squares;
}

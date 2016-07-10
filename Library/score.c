//
//  score.c
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

#include "score.h"

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#if SCORE_DEBUG
#include <stdio.h>
#endif

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
  char c = (char)byte;
  return c >= ' ' && c <= '~';
}

/* Is byte an ASCII letter character?
 * Case-insensitive. */
bool
is_byte_ascii_letter(uint8_t byte)
{
  char c = (char)byte;
  return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

/* Is byte an ASCII space character? */
bool
is_byte_ascii_space(uint8_t byte)
{
  char c = (char)byte;
  return c == ' ';
}

/* Convert the byte array bytearray into a newly allocated ASCII
 * nul-terminated string, escaping non-printable characters using "\xHH".
 * Never returns a NULL char *. If bytearray has a zero length, the returned
 * char * is "".
 * Outputs lowercase hexadecimal characters in escapes.
 * The caller must free() the returned string. */
char *
bytearray_to_escstr(const bytearray_t *bytearray)
{
  assert(bytearray != NULL);
  assert(is_bytearray_consistent(bytearray));

  /* One extra byte for the terminating nul */
  const size_t max_asciistr_len = (
                  bytearray_length(bytearray) * ESCAPED_HEXCHARS_PER_BYTE + 1);
  /* If any bytes are printable ASCII, we will use 1 character for them rather
   * than 4 characters. This wastage is ok. */
  char * const asciistr = malloc(max_asciistr_len);
  assert(asciistr != NULL);
  /* Avoid having to add the terminating nul later */
  memset(asciistr, 0, max_asciistr_len);

  size_t i = 0;
  size_t asciistr_pos = 0;
  for (i = 0; i < bytearray_length(bytearray); i++) {
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
  assert(i == bytearray_length(bytearray));
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
  assert(byte_test != NULL);

  size_t result = 0;

  for (size_t i = 0; i < bytearray_length(bytearray); i++) {
    uint8_t byte = bytearray_get_checked(bytearray, i);

    if (byte_test(byte)) {
      result++;
    }
  }

  assert(result <= bytearray_length(bytearray));

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
count_letter(const bytearray_t *bytearray, bool include_space)
{
  size_t result = count_byte_test(bytearray, &is_byte_ascii_letter);

  if (include_space) {
    result += count_space(bytearray);
  }

  assert(result <= bytearray_length(bytearray));

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

  for (size_t i = 0; i < bytearray_length(bytearray); i++) {
    uint8_t ba_byte = bytearray_get_checked(bytearray, i);

    if (byte == ba_byte) {
      result++;
    }
  }

  assert(result <= bytearray_length(bytearray));

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
  size_t total_letter_count = count_letter(bytearray, false);

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
    assert(frequencies_out[i] >= 0.0);
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

/* Score the actual frequency against the expected frequency by returning
 * a positive value that increases as actual deviates more from expected. */
static double
score_letter_frequency(double actual, double expected)
{
  assert(actual >= 0.0);
  assert(expected >= 0.0);

  /* Use the absolute value of the difference */
  return fabs(actual - expected);
}

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
    double score = score_letter_frequency(letter_frequency[i],
                                          english_letter_frequency[i]);
    assert(score >= 0.0);
    assert(score <= 1.0);
    sum_of_squares += score*score;
    assert(sum_of_squares >= 0.0);
  }

  double root_mean_squares = sqrt(sum_of_squares/LETTER_COUNT);

  assert(root_mean_squares >= 0.0);
  return root_mean_squares;
}

/* Scoring Heuristics */

/* My estimate of the average English line length is 40 characters */
#define ENGLISH_LINE_LENGTH 40

/* The average English word length is 5
 * (The frequency of English spaces is 1 in 6 characters)
 * http://www.quora.com/Whats-the-average-length-of-English-words */
#define ENGLISH_WORD_LENGTH 5
#define ENGLISH_SPACE_LENGTH (ENGLISH_WORD_LENGTH+1)

/* The average number of letters between punctuation in English text
 * The frequency of English punctuation is 208.7 per 1000 words
 * https://en.wikipedia.org/wiki/Punctuation_of_English#Frequency */
#define ENGLISH_PUNCTUATION_LENGTH ((ENGLISH_SPACE_LENGTH*1000)/209)

/* Allow for a few unprintable characters, like tabs or newlines.
 * This allows for CRLF-terminated lines 20 characters long.
 * (This is double my estimate of English line length, and a quarter of
 * typical terminal widths.) */
#define MAX_UNPRINTABLE(length) ((length)/(ENGLISH_LINE_LENGTH/4))

/* Non-letters include punctuation but exclude spaces. This allows for
 * punctuation every 4 characters. (This is seven times the typical English
 * punctuation frequency of 28 characters.) */
#define MAX_NONLETTER(length) ((length)/(ENGLISH_PUNCTUATION_LENGTH/7))

/* Check we actually have words, and not just blocks of text or lots of
 * whitespace. */
#define EXPECTED_SPACE_FREQUENCY (1.0/ENGLISH_SPACE_LENGTH)

/* The maximum RMS variation from the typical English space frequency.
 * The score is independent of the length of the text.
 * It's between 0 and 1, with good scores being around 0.01,
 * potential sentences starting around 0.08,
 * and the largest scores being around 0.167.
 */
#define GOOD_SPACE_DEVIATION (0.008)
#define MAX_SPACE_DEVIATION (0.08)

/* The maximum RMS variation from typical English letter frequencies.
 * The score is independent of the length of the text.
 * It's between 0 and 1, with good scores being around 0.0045,
 * potential English starting around 0.055,
 * and the largest scores being around 0.202.
 */
#define GOOD_ENGLISH_DEVIATION (0.04)
#define MAX_ENGLISH_DEVIATION (0.055)

/* If deviation is less than or equal to good_deviation, return zero.
 * Otherwise, reduce deviation by good_deviation. */
static double
scale_good_deviation(double deviation, double good_deviation)
{
  if (deviation <= good_deviation) {
    return 0.0;
  } else {
    return deviation - good_deviation;
  }
}

/* Score deviation: lower values are better; exceeding max_deviation scores
 * zero.
 * Returns a value between 0.0 and 1.0, larger values are better scores.
 * Returns 0.0 when deviation is greater than max_deviation.
 * Returns 1.0 when deviation is zero, including when max_deviation is also
 * zero. */
static double
score_max_deviation(double deviation, double max_deviation)
{
  assert(deviation >= 0.0);
  assert(max_deviation >= 0.0);
  if (deviation > max_deviation) {
    return 0.0;
  } else if (deviation <= DBL_EPSILON && max_deviation <= DBL_EPSILON) {
    return 1.0;
  } else {
    double result = (max_deviation - deviation)/max_deviation;
    assert(isfinite(result));
    assert(result >= 0.0);
    assert(result <= 1.0);
    return result;
  }
}

/* Like score_max_deviation, but with size_t inputs. */
static double
score_max_count(size_t count, size_t max_count)
{
  return score_max_deviation((double)count, (double)max_count);
}

/* How likely is it that bytearray is English text?
 * The output is between 0.0 and 1.0, higher scores are better.
 */
double
score_english_text(const bytearray_t *bytearray)
{
  /* English generally doesn't contain unprintables or non-letters.
   * On average, English text has certain letter and space frequencies. */

  /* Unprintable Maximum */
  size_t max_unprint = MAX_UNPRINTABLE(bytearray_length(bytearray));
  size_t unprint = count_unprintable(bytearray);
  double unprint_factor = score_max_count(unprint, max_unprint);
  assert(unprint_factor >= 0.0);

  /* Non-letter Maximum */
  size_t max_nonlet = MAX_NONLETTER(bytearray_length(bytearray));
  size_t nonlet = count_nonletter(bytearray, false);
  double nonlet_factor = score_max_count(nonlet, max_nonlet);
  assert(nonlet_factor >= 0.0);

  /* Space Frequency */
  size_t space_count = count_space(bytearray);
  double space_freq = space_count / (double)bytearray_length(bytearray);
  double space_dev = score_letter_frequency(space_freq,
                                              EXPECTED_SPACE_FREQUENCY);
  double scaled_space_dev = scale_good_deviation(space_dev,
                                                 GOOD_SPACE_DEVIATION);
  double space_factor = score_max_deviation(scaled_space_dev,
                                            MAX_SPACE_DEVIATION);

  /* Letter Frequency */
  double english_dev = score_english_letter_frequency(bytearray);
  double scaled_english_dev = scale_good_deviation(english_dev,
                                                   GOOD_ENGLISH_DEVIATION);
  double english_factor = score_max_deviation(scaled_english_dev,
                                              MAX_ENGLISH_DEVIATION);

  double result = (unprint_factor * nonlet_factor * space_factor
                   * english_factor);

#if SCORE_DEBUG
  printf("Unprintable:           %zu <= %zu -> %.3f\n", unprint, max_unprint, unprint_factor);
  printf("Non-Letter:            %zu <= %zu -> %.3f\n", nonlet, max_nonlet, nonlet_factor);
  printf("Space (Word Length):   %.3f <= %.3f <= %.3f -> %.3f\n", GOOD_SPACE_DEVIATION, space_dev, MAX_SPACE_DEVIATION, space_factor);
  printf("English Text Score:    %.3f <= %.3f <= %.3f -> %.3f\n", GOOD_ENGLISH_DEVIATION, english_dev, MAX_ENGLISH_DEVIATION, english_factor);
#endif

  assert(result >= 0.0);
  assert(result <= 1.0);
  return result;
}

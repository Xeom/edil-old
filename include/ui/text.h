#if !defined(UI_TEXT)
# define UI_TEXT
# include "head.h"

# include <stddef.h>
# include <stdint.h>

# include "ui/face.h"

/* In order for edil to handle coloured text, cursors, unicode characters,    *
 * etc. without entirely ruining its memory usage, strings are encoded in     *
 * utf-8, and \n is used as an escape for various colour and formatting       *
 * codes.                                                                     *
 *                                                                            *
 * This creates the problem that in strings, bytes do not map one-to-one with *
 * the characters they represent, so the functions in this file are full of   *
 * functions for manipulating utf-8 encoded, \n escaped strings.              *
 *                                                                            *
 * The basic concept behind most of these functions is splitting up strings   *
 * into symbols, each of which represent either a character, which may be     *
 * one, two, three or four bytes wide, or a face, which changes the           *
 * formatting of a number of following characters. Treating strings in this   *
 * way allows us to abstract over the issues associated with encoding strings *
 * in this weird way.                                                         *
 *                                                                            *
 * Note that everything here is generally horrifically O(n), and is designed  *
 * with iterating over strings in mind, to, for example, render them to a     *
 * screen.                                                                    */

typedef enum {
    ascii      = 0x01,
    utf8_2wide = 0x02,
    utf8_3wide = 0x03,
    utf8_4wide = 0x04,
    face_start = 0x81,
    utf8_mid   = 0x82,
    utf8_big   = 0x83
} text_symbol_type;

/*
 * Categorize a character into a symbol_type.
 *
 * Each possible character has a symbol_type associated with it. This function
 * gets the symbol_type for any particular char.
 *
 * @param c The character to get a symbol_type for.
 *
 * @return  The symbol_type of the character.
 *
 */
text_symbol_type ui_text_symbol(char c);

/*
 * Get the width of any particular symbol.
 *
 * Many symbol_types are several bytes long. Although each byte of the sequence
 * has an individual symbol_type, they can all be associated with the first one
 * and its type.
 *
 * This function just returns the width of any particular symbol, in bytes, from
 * the symbol_type of the first char of any symbol.
 *
 * @param sym The symbol_type to find a width for.
 *
 * @return    The width of the symbol, in bytes.
 *
 */
size_t ui_text_symbol_width(text_symbol_type sym);

/*
 * Check whether a particular symbol represents a character.
 *
 * utf-8 and ascii character symbols count as characters. Others do not.
 *
 * @param sym The symbol type to inspect
 *
 * @return    1 if the symbol is a character, 0 otherwise.
 *
 */
int ui_text_symbol_is_char(text_symbol_type sym);

/*
 * Extract a unicode character from a utf-8 encoding.
 *
 * The string must start with a character. This can be tested with
 * ui_text_symbol and ui_text_symbol_is_char. The end of the string may be
 * beyond the end of this character.
 *
 * The character can be either encoded as a multibyte unicode character, or
 * plain ascii. This function will handle either.
 *
 * @param str A pointer to the start of the string to extract a character from.
 * @param end A pointer to the last character of the string plus one. Equiv to
 *            str plus the length of the string.
 *
 * @return    The unicode codepoint of the character, or -1 if either the string
 *            ended unexpectedly, or the string did not start with a character.
 *
 */
int32_t ui_text_decode_utf8(const char *str, const char *end);

size_t ui_text_encode_utf8(int32_t chr, char *buf);

char *ui_text_next_symbol(const char *str, const char *end);

char *ui_text_next_char(const char *str, const char *end);
char *ui_text_first_char(const char *str, const char *end);

char *ui_text_next_face(const char *str, const char *end);
char *ui_text_first_face(const char *str, const char *end);

char *ui_text_get_char(const char *str, const char *end, size_t n);

size_t ui_text_len(const char *str, const char *end);

short ui_text_face_overflow(const char *str, const char *end, face *f);

int ui_text_draw_h(
    const char *str,
    const char *end,
    uint sizelim,
    chtype filler,
    short facen);
#endif /* UI_TEXT */

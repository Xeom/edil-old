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
    ascii      = 0x01, /* Plain ascii characters                              */
    utf8_2wide = 0x02, /* The start chars of utf-8 encodings ...              */
    utf8_3wide = 0x03,
    utf8_4wide = 0x04,
    face_start = 0x81, /* The start char of a formatting code                 */
    utf8_mid   = 0x82, /* A char from midway through a utf-8 encoding         */
    utf8_big   = 0x83  /* The start char from a utf-8 encoding that's too big */
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
 * The character can be either encoded as a multibyte utf-8 character, or plain
 * ascii. This function will handle either.
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

/*
 * Encode a unicode character into a utf-8 encoding.
 *
 * The encoded character is placed into memory at a pointer passed to the
 * function. There must always be at least 4 chars of writable memory at this
 * location, as even if the character is not 4 chars long, all 4 bytes will be
 * set to 0.
 *
 * The encoded character will be encoded as either ascii or a multibyte utf-8
 * character, depending on its size.
 *
 * @param chr The unicode codepoint to encode.
 * @param buf A pointer to at least 4 chars' of memory to encode the character
o *            in.
 *
 * @return    The number of bytes long that the character is long, or 0 on
 *            failure. When nonzero, equivilent to
 *            ui_text_symbol_width(ui_text_symbol(*buf)).
 *
 */
size_t ui_text_encode_utf8(int32_t chr, char *buf);

/*
 * Get a pointer to the second symbol in a string.
 *
 * If the string starts with a symbol partway through, like utf8_mid, the next
 * symbol may be the next utf8_mid, rather than the next actual symbol.
 *
 * @param str A pointer to the start of the string.
 * @param end A pointer to the location in memory just after the end of the
 *            string - i.e. end[-1] is the last valid char.
 *
 * @return    A pointer to the next symbol in the string, or NULL if we have run
 *            out of string or there was an error.
 *
 */
char *ui_text_next_symbol(const char *str, const char *end);

/*
 * Get a pointer to the next character in a string.
 *
 * If the string starts with a character, return a pointer to the second
 * character. Otherwise return a pointer to the first character.
 *
 * @param str A pointer to the start of the string.
 * @param end A pointer to the location in memory just after the end of the
 *            string - i.e. end[-1] is the last valid char.
 *
 * @return    A pointer to the first byte of the next character in the string,
 *            or NULL if we have run out of string, or if there was an error.
 *
 */
char *ui_text_next_char(const char *str, const char *end);

/*
 * Get a pointer to the first character in a string.
 *
 * @param str A pointer to the start of the string.
 * @param end A pointer to the location in memory just after the end of the
 *            string - i.e. end[-1] is the last valid char.
 *
 * @return    A pointer to the first byte of the first character in the string,
 *            or NULL if we have run out of string, or if there was an error.
 *
 */
char *ui_text_first_char(const char *str, const char *end);

/*
 * Get a pointer to the next symbol of type face_start in a string.
 *
 * If the string starts with a symbol of type face_start, return a pointer to
 * the second symbol of type face_start. Otherwise return a pointer to the first
 * symbol of type face_start.
 *
 * @param str A pointer to the start of the string.
 * @param end A pointer to the location in memory just after the end of the
 *            string - i.e. end[-1] is the last valid char.
 *
 * @return    A pointer to the first byte of the first face in the string, or
 *            NULL if we have run out of string, or if there was an error.
 *
 */
char *ui_text_next_face(const char *str, const char *end);

/*
 * Get a pointer to the first symbol of type face_start in a string.
 *
 * @param str A pointer to the start of the string.
 * @param end A pointer to the location in memory just after the end of the
 *            string - i.e. end[-1] is the last valid char.
 *
 * @return    A pointer to the first byte of the first face in the string,
 *            NULL if we have run out of string, or if there was an error.
 *
 */
char *ui_text_first_face(const char *str, const char *end);

/*
 *
 * Get a pointer to the first byte of the nth character in a string.
 *
 * @param str A pointer to the start of the string.
 * @param end A pointer to the location in memory just after the end of the
 *            string - i.e. end[-1] is the last valid char.
 * @param n   The index of the character to get, starting from 0.
 *
 * @return    A pointer to the first byte of the nth character, NULL if the
 *            string contains less than n + 1 characters
 *
 */
char *ui_text_get_char(const char *str, const char *end, size_t n);

/*
 * Get the number of character symbols in a string.
 *
 * @param str A pointer to the start of the string.
 * @param end A pointer to the location in memory just after the end of the
 *            string - i.e. end[-1] is the last valid char.
 *
 * @return    The number of characters in the string.
 *
 */
size_t ui_text_len(const char *str, const char *end);

/*
 * Get the face formatting overflowing off the end of a string.
 *
 * A pointer to a face is given to the function, and if non-null, the function
 * will set the contents of the face to the deserialiszed overflowing face.
 *
 * @param str A pointer to the start of the string.
 * @param end A pointer to the location in memory just after the end of the
 *            string - i.e. end[-1] is the last valid char.
 * @param f   A pointer to a face to set the contents of.
 *
 * @return    The remaining characters of formatting past the end of the string.
 *
 */
short ui_text_face_overflow(const char *str, const char *end, face *f);

/*
 * Print the formatted contents of a string, to stdscr.
 *
 * the _h function prints horizontally rightwards, the _v function prints
 * vertically downwards.
 *
 * The string is printed starting at the current position of the cursor,
 * settable by the ncurses move function.
 *
 * The function will set the contents of sizelim spaces on the screen, either
 * all with the contents of the string, or with the filler chtype.
 *
 * If str is NULL, the function will fill the area with the filler character.
 * If end is NULL, the function will print until a null terminator.
 *
 * @param str     A pointer to the start of the string.
 * @param end     A pointer to the location in memory just after the end of the
 *                string - i.e. end[-1] is the last valid char.
 * @param sizelim The number of spaces on the screen to update.
 * @param filler  The character to place after the end of the string.
 * @param facen   A number of characters to print before resetting the 
 *                attributes of the cursor. This allows the first n characters
 *                of the string to have specific attributes.
 *
 * @return        0 on success, -1 on failure.
 *
 */
int ui_text_draw_h(
    const char *str,
    const char *end,
    uint sizelim,
    chtype filler,
    short facen);
int ui_text_draw_v(
    const char *str,
    const char *end,
    uint sizelim,
    chtype filler,
    short facen);

#endif /* UI_TEXT */

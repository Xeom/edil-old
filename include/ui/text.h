#if !defined(UI_TEXT)
# define UI_TEXT
# include "head.h"

# include <stddef.h>
# include <stdint.h>

# include "ui/face.h"

typedef enum {
    ascii      = 0x01,
    utf8_2wide = 0x02,
    utf8_3wide = 0x03,
    utf8_4wide = 0x04,
    face_start = 0x81,
    utf8_mid   = 0x82,
    utf8_big   = 0x83
} text_symbol_type;

text_symbol_type ui_text_symbol(char c);

size_t ui_text_symbol_width(text_symbol_type sym);

int ui_text_symbol_is_char(text_symbol_type sym);

int32_t ui_text_decode_utf8(const char *str, const char *end);

char *ui_text_next_symbol(const char *str);

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

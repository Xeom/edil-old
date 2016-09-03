#if !defined(UI_TEXT)
# define UI_TEXT
# include "head.h"

# include "stddef.h"
# include "stdint.h"

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

char *ui_text_next_symbol(char *str);

int ui_text_symbol_is_char(text_symbol_type sym);

char *ui_text_next_char(char *str, char *end);

char *ui_text_next_face(char *str, char *end);

size_t ui_text_len(char *str, char *end);

char *ui_text_get_char(char *str, char *end, size_t n);

int32_t ui_text_decode_utf8(char *str, char *end);

#endif /* UI_TEXT */

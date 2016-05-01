#include <limits.h>
#include <string.h>
#include <stdint.h>

#include "io/key.h"

#include "container/hashes.h"

#define hash_high_nibl_off (8 * (sizeof(hash)) - 4)
#define hash_high_byte_off (8 * (sizeof(hash) - 1))

int hashes_eq_str(void *a, void *b)
{
    return strcmp(*(char **)a, *(char **)b) == 0;
}

hash hashes_key_str(void *k)
{
    hash  hsh, high;
    char *str;

    str = *(char **)k;
    hsh = 0;

    while (*str)
    {
        hsh  = (hsh << 4) + (hash)*(str++);
        high = (((hash)0x0f << hash_high_nibl_off) & hsh);
        hsh ^= high >> hash_high_nibl_off;
    }

    hsh ^= hsh * 3;

    return hsh;
}

/* We need nested if/else cos all the elif values get evalulated *
 * that causes warnings where the const value is larger than an  *
 * actual long.                                                  */

#if   ULONG_MAX == 0xffffffffl
# define hashes_random_seed 0xfc64c9bd;
#else
# if ULONG_MAX == 0xffffffffffffffffl
#   define hashes_random_seed 0xfc64c9bd39ea2141;
# else
#  if ULONG_MAX == 0xffffffffffffffffffffffffffffffffl
#   define hashes_random_seed 0xfc64c9bd39ea214178c6a000618e9c51;
#  else
    /* This will cause a warning, but meh */
#   define hashes_random_seed (hash)0xfc64c9bd39ea214178c6a000618e9c51;
#  endif
# endif
#endif

static const unsigned char hashes_random_bytes[256] =
{
    0x96, 0xDC, 0x52, 0xEF, 0xBF, 0x8F, 0x69, 0x1C,
    0x84, 0x8D, 0xBF, 0x92, 0xDB, 0xB1, 0x67, 0x68,
    0xA0, 0x5A, 0x27, 0x92, 0xC8, 0xCB, 0xAB, 0xDB,
    0x6A, 0x8B, 0xAF, 0xB4, 0x90, 0x36, 0xF2, 0xA7,
    0x79, 0x92, 0x85, 0x62, 0x80, 0x27, 0x6F, 0xF9,
    0x2B, 0xE5, 0xD5, 0x11, 0x92, 0x33, 0x8C, 0x85,
    0xBA, 0xA5, 0x1F, 0x34, 0x05, 0x6A, 0xAC, 0xDD,
    0x19, 0xDB, 0x98, 0xAB, 0xB9, 0xFF, 0xB9, 0xB6,
    0x03, 0xB1, 0xF1, 0xCC, 0xA4, 0xD6, 0x02, 0x6A,
    0xC7, 0x51, 0x79, 0x99, 0x8A, 0x16, 0x2A, 0x86,
    0x99, 0xF0, 0x1D, 0x2C, 0x8E, 0x14, 0x54, 0xBD,
    0x20, 0x07, 0xE3, 0xFD, 0x14, 0xBC, 0xE4, 0xB3,
    0x1F, 0x9B, 0x22, 0x89, 0x71, 0xBC, 0xE2, 0xC9,
    0xD8, 0x02, 0xD4, 0x2D, 0x64, 0xD3, 0x2F, 0x7F,
    0x67, 0x07, 0xC1, 0xED, 0xBC, 0xCD, 0xB2, 0x22,
    0x65, 0x3D, 0xB4, 0x1C, 0xCD, 0xA9, 0x07, 0x6D,
    0xFC, 0xAC, 0xF1, 0x4D, 0x9D, 0x22, 0x8C, 0xDD,
    0x97, 0xB2, 0x4E, 0x45, 0x86, 0x32, 0xB9, 0xBF,
    0x36, 0x7F, 0x9F, 0xDD, 0x86, 0x9C, 0xD8, 0x32,
    0x95, 0x55, 0x0F, 0x3E, 0x8D, 0x3F, 0x4F, 0xCF,
    0x35, 0xED, 0xFA, 0xB1, 0xE9, 0x7C, 0x8C, 0xEF,
    0x67, 0x7E, 0xE2, 0x0B, 0x58, 0xD9, 0x2A, 0xFE,
    0x8A, 0xEA, 0x84, 0x9E, 0x2F, 0x78, 0x1E, 0x5D,
    0xCC, 0x05, 0xF5, 0xF4, 0xDD, 0x99, 0xA5, 0xB1,
    0x76, 0xF0, 0x16, 0x98, 0xD2, 0xD4, 0xBC, 0xF8,
    0xF4, 0xDB, 0x15, 0xDF, 0xEC, 0x4F, 0x5E, 0xF3,
    0x9E, 0x2D, 0x52, 0x61, 0x72, 0x09, 0x36, 0x61,
    0xE4, 0x3F, 0xF5, 0x07, 0x31, 0xF6, 0xC3, 0x0B,
    0xC5, 0xDA, 0x43, 0x97, 0x62, 0x8B, 0x98, 0x40,
    0xCA, 0x00, 0x40, 0xE2, 0xA7, 0x3C, 0x81, 0x21,
    0x01, 0xED, 0x86, 0x8B, 0xD3, 0xDE, 0x8A, 0x68,
    0x15, 0xCE, 0x55, 0xD1, 0x23, 0xD5, 0x70, 0x52
};

hash hashes_key_str_trans(void *k)
{
    hash  hsh, high;
    char *str;

    str = *(char **)k;
    hsh = hashes_random_seed;

    while (*str)
    {
        hsh  = (hsh << 4) + (hash)hashes_random_bytes[(size_t)*(str++)];
        high = (((hash)0xff << hash_high_byte_off) & hsh);
        hsh ^= high >> hash_high_byte_off;
    }

    return hsh;
}

hash hashes_mem(char *mem, size_t n)
{
    hash hsh;
    char *end;

    /* Start with a const random seed */
    hsh = hashes_random_seed;

    while (n >= 4)
    {
        uint32_t i;
        double   recp, den;
        void    *fptr;

        i    = *(uint32_t *)mem;
        mem += 4;
        n   -= 4;

        /* We use doubles rather than divide a *
         * large integer for a couple reasons. *
         *                                     *
         * - Their fraction never changes in   *
         *   magnitude by more than 2x.        *
         * - They can easily divide by values  *
         *   above 2^64 and produce useful     *
         *   sequences of bits. Integers just  *
         *   produce 0 in that situation.      *
         *                                     *
         * Overall, they produce consistent    *
         * and useful values with any input.   */

        /* 2n - 1 is not a multiple of two.    *
         *                                     *
         * The fractional portion of 1/(2^n)k  *
         * is the same as 1/k, so by making    *
         * the denominator not a multiple of   *
         * 2, there should be far fewer        *
         * collisions. It also avoids numbers  *
         * that are represented well in binary *
         * 1/2, 1/4 etc, and makes division    *
         * by zero a nonissue.                 */
        den  = ((double)i * 2.0) - 1;

        /* Find the reciprocal.                */
        recp = 1 / (double)den;
        fptr = &recp;

        /* xor the lower 52 bits of the double *
         * (the fractional part).              */
        hsh ^= ((1l << 52) - 1) & *(long long *)fptr;

        /* The upper bits lack entropy, as     *
         * they change slowly with 1/n. This   *
         * also serves to smoosh the hash      *
         * to higher bits.                     */
        hsh ^= (hsh << 36);

        /* xor in the original value. This to  *
         * some degree stops values of the     *
         * hash acting proportional to 1/n.    */
        hsh ^= i;
    }

    return hsh;
}

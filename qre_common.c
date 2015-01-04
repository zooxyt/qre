/*
   Quick Regular Expression Engine : Common
   Copyright (c) 2013-2014 Cheryl Natsu 
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
   3. The name of the authors may not be used to endorse or promote products
   derived from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY EXPRESS OR
   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
   IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   */

#include "qre_common.h"
#include "qre_err.h"
#include "qre_char.h"

int utf_8_get_unit_length(const char ch_first)
{
    if ((ch_first & 0x80) == 0) return 1; /* 0xxxxxxx */
    else if ((ch_first & 0xe0) == 0xc0) return 2; /* 110xxxxx, 10xxxxxx */
    else if ((ch_first & 0xf0) == 0xe0) return 3; /* 1110xxxx, 10xxxxxx, 10xxxxxx */
    else if ((ch_first & 0xf8) == 0xf0) return 4; /* 11110xxx, 10xxxxxx, 10xxxxxx, 10xxxxxx */
    else if ((ch_first & 0xfc) == 0xf8) return 5; /* 111110xx, 10xxxxxx, 10xxxxxx, 10xxxxxx, 10xxxxxx */
    else if ((ch_first & 0xfe) == 0xfc) return 6; /* 1111110x, 10xxxxxx, 10xxxxxx, 10xxxxxx, 10xxxxxx, 10xxxxxx */
    else return 0;
}

#define QRE_LEXER_FROM_UTF_8_NEXT(str_p) \
    do { ++str_p; } while (FALSE)
#define QRE_LEXER_FROM_UTF_8_READ_FOLLOW(value, str_p) \
    do { value = (value << 6) | ((*str_p) & 0x3f); str_p++; } while (FALSE)

int utf_8_get_string_length(size_t *length_out, char *str, size_t size)
{
    int ret = 0;
    char *str_p = str, *str_endp = str_p + size;
    size_t length = 0;
    int unit_length = 0;
    char ch_first;

    while (str_p != str_endp)
    {
        ch_first = *str_p;

        unit_length = utf_8_get_unit_length(ch_first);
        if (str_endp - str_p < unit_length)
        {
            ret = -QRE_ERR_CORRUPT;
            goto fail;
        }
        length += 1;
        str_p += unit_length;
    }

    *length_out = length;

    goto done;
fail:

done:
    return ret;
}


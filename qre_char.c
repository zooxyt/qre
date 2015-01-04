/*
   Quick Regular Expression Engine : Char
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

#include <stdio.h>

#include "qre_char.h"

#define QRE_CHAR_PRINT_UTF_8_LEN_MAX 7
void qre_char_print_utf_8(qre_char_t value)
{
    char result[QRE_CHAR_PRINT_UTF_8_LEN_MAX];
    char *result_p = result;

    if (value <= 0x7F) *result_p++ = (char)(value);
    else if (value <= 0x7FF)
    {
        *result_p++ = (char)(0xc0 | ((value) >> 6));
        *result_p++ = (char)(0x80 | ((value) & 0x3f));
    }
    else if (value <= 0xFFFF)
    {
        *result_p++ = (char)(0xe0 | ((value) >> 12));
        *result_p++ = (char)(0x80 | (((value) >> 6) & 0x3f));
        *result_p++ = (char)(0x80 | ((value) & 0x3f));
    }
    else if (value <= 0x1FFFFF)
    {
        *result_p++ = (char)(0xf0 | ((value) >> 18));
        *result_p++ = (char)(0x80 | (((value) >> 12) & 0x3f));
        *result_p++ = (char)(0x80 | (((value) >> 6) & 0x3f));
        *result_p++ = (char)(0x80 | ((value) & 0x3f));
    }
    else if (value <= 0x3FFFFFF)
    {
        *result_p++ = (char)(0xf8 | ((value) >> 24));
        *result_p++ = (char)(0x80 | (((value) >> 18) & 0x3f));
        *result_p++ = (char)(0x80 | (((value) >> 12) & 0x3f));
        *result_p++ = (char)(0x80 | (((value) >> 6) & 0x3f));
        *result_p++ = (char)(0x80 | ((value) & 0x3f));
    }
    else if (value <= 0x7FFFFFFF)
    {
        *result_p++ = (char)(0xfc | ((value) >> 30));
        *result_p++ = (char)(0x80 | (((value) >> 24) & 0x3f));
        *result_p++ = (char)(0x80 | (((value) >> 18) & 0x3f));
        *result_p++ = (char)(0x80 | (((value) >> 12) & 0x3f));
        *result_p++ = (char)(0x80 | (((value) >> 6) & 0x3f));
        *result_p++ = (char)(0x80 | ((value) & 0x3f));
    }

    fwrite(result, result_p - result, 1, stdout);
}

void qre_str_print_utf_8(qre_char_t *str, size_t str_len)
{
    while (str_len-- != 0)
    {
        qre_char_print_utf_8(*str);
        str++;
    }
}

void qre_char_memcpy(qre_char_t *dst, qre_char_t *src, size_t len)
{
    while (len-- > 0)
    {
        *dst++ = *src++;
    }
}

int qre_char_strncmp(qre_char_t *s1, qre_char_t *s2, size_t len)
{
    while (len-- != 0)
    {
        if (*s1 > *s2) return 1;
        else if (*s1 < *s2) return -1;
        s1++; s2++;
    }
    return 0;
}


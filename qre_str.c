/*
   Quick Regular Expression Engine : String
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

#ifdef _MSC_VER
/* disable conditional expression is constant warning */
#pragma warning(disable:4127)
#endif

#include <stdio.h>
#include <stdlib.h>

#include "qre_common.h"
#include "qre_err.h"
#include "qre_char.h"
#include "qre_str.h"

int qre_string_new_from_utf_8(qre_string_t **qre_string_out, char *str, size_t size)
{
    int ret = 0;
    qre_string_t *new_qre_string = NULL;
    size_t idx;
    qre_char_t value;
    char *str_p, *str_endp;
    char ch_first;
    int bytes_number;

    new_qre_string = (qre_string_t *)malloc(sizeof(qre_string_t));
    if (new_qre_string == NULL) { goto fail; }
    new_qre_string->body = NULL;
    new_qre_string->size = 0;

    ret = utf_8_get_string_length(&new_qre_string->size, str, size);
    if (ret != 0) { goto fail; }

    new_qre_string->body = (qre_char_t *)malloc(sizeof(qre_char_t) * new_qre_string->size);
    if (new_qre_string->body == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail;
    }

    str_p = str;
    str_endp = str_p + size;
    for (idx = 0; idx != new_qre_string->size; idx++)
    {
        ch_first = *str_p;

        bytes_number = utf_8_get_unit_length(ch_first);
        QRE_EAT_FROM_UTF_8_CHECK_LENGTH(bytes_number);
        switch (bytes_number)
        {
            case 1:
                value = ch_first & 0x7f;
                QRE_EAT_FROM_UTF_8_NEXT(str_p);
                break;
            case 2:
                value = ch_first & 0x1f;
                QRE_EAT_FROM_UTF_8_NEXT(str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                break;
            case 3:
                value = ch_first & 0x1f;
                QRE_EAT_FROM_UTF_8_NEXT(str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                break;
            case 4:
                value = ch_first & 0x1f;
                QRE_EAT_FROM_UTF_8_NEXT(str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                break;
            case 5:
                value = ch_first & 0x1f;
                QRE_EAT_FROM_UTF_8_NEXT(str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                break;
            case 6:
                value = ch_first & 0x1f;
                QRE_EAT_FROM_UTF_8_NEXT(str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                break;
            default:
                { ret = -QRE_ERR_CORRUPT; goto fail; }
        }

        new_qre_string->body[idx] = value;
    }

    *qre_string_out = new_qre_string;

    goto done;
fail:
    if (new_qre_string != NULL)
    {
        qre_string_destroy(new_qre_string);
        new_qre_string = NULL;
    }
done:
    return ret;
}

int qre_string_new(qre_string_t **qre_string_out, qre_char_t *str, size_t size)
{
    int ret = 0;
    qre_string_t *new_qre_string = NULL;
    qre_char_t *body_p;

    new_qre_string = (qre_string_t *)malloc(sizeof(qre_string_t));
    if (new_qre_string == NULL) { goto fail; }
    new_qre_string->body = NULL;
    new_qre_string->body = (qre_char_t *)malloc(sizeof(qre_char_t) * size);
    if (new_qre_string->body == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail;
    }
    body_p = new_qre_string->body;
    new_qre_string->size = size;

    while (size-- != 0)
    {
        *body_p++ = *str++;
    }
    *qre_string_out = new_qre_string;

    goto done;
fail:
    if (new_qre_string != NULL)
    {
        qre_string_destroy(new_qre_string);
        new_qre_string = NULL;
    }
done:
    return ret;
}

int qre_string_destroy(qre_string_t *qre_string)
{
    if (qre_string->body != NULL) { free(qre_string->body); }
    free(qre_string);

    return 0;
}

int qre_string_eq(qre_string_t *s1, qre_string_t *s2)
{
    if (s1->size != s2->size) return FALSE;

    return (qre_char_strncmp(s1->body, s2->body, s2->size) == 0) ? TRUE : FALSE;
}


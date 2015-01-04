/*
   Quick Regular Expression Engine : Token
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

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "qre_common.h"
#include "qre_char.h"
#include "qre_token.h"

/* QRE Token */

qre_token_t *qre_token_new(int type, const uint32_t value)
{
    qre_token_t *new_token = NULL;

    new_token = (qre_token_t *)malloc(sizeof(qre_token_t));
    if (new_token == NULL) goto fail; 

    new_token->type = type;
    new_token->value = value;
    new_token->next = NULL;

    goto done;
fail:
    if (new_token != NULL) 
    {
        free(new_token);
        new_token = NULL;
    }
done:
    return new_token;
}

int qre_token_destroy(qre_token_t *qre_token)
{
    free(qre_token);

    return 0;
}

qre_token_t *qre_token_clone(qre_token_t *qre_token)
{
    return qre_token_new(qre_token->type, qre_token->value);
}

int qre_token_has_enough_tail(qre_token_t *qre_token, int length)
{
    while (length-- > 0)
    {
        if (qre_token == NULL) return FALSE;
        qre_token = qre_token->next;
    }
    return TRUE;
}

qre_token_t *qre_token_look_ahead(qre_token_t *qre_token, int length)
{
    while (length-- > 0)
    {
        if (qre_token == NULL) return NULL;
        qre_token = qre_token->next;
    }
    return qre_token;
}

int qre_token_length(qre_token_t *qre_token)
{
    int result = 0;
    while (qre_token != NULL) 
    {
        result += 1;
        qre_token = qre_token->next;
    }
    return result;
}


/* QRE Token List */

qre_token_list_t *qre_token_list_new(void)
{
    qre_token_list_t *new_token_list = NULL;

    new_token_list = (qre_token_list_t *)malloc(sizeof(qre_token_list_t));
    if (new_token_list == NULL) goto fail; 

    new_token_list->begin = new_token_list->end = NULL;
    new_token_list->size = 0;

    goto done;
fail:
    if (new_token_list != NULL)
    {
        free(new_token_list);
        new_token_list = NULL;
    }
done:
    return new_token_list;
}

int qre_token_list_destroy(qre_token_list_t *qre_token_list)
{
    qre_token_t *qre_token_cur = NULL, *qre_token_next = NULL;

    qre_token_cur = qre_token_list->begin;
    while (qre_token_cur != NULL)
    {
        qre_token_next = qre_token_cur->next;
        qre_token_destroy(qre_token_cur);
        qre_token_cur = qre_token_next;
    }
    free(qre_token_list);

    return 0;
}

int qre_token_list_append(qre_token_list_t *qre_token_list, qre_token_t *new_qre_token)
{
    if (qre_token_list->begin == NULL)
    {
        qre_token_list->begin = qre_token_list->end = new_qre_token;
    }
    else
    {
        qre_token_list->end->next = new_qre_token;
        qre_token_list->end = new_qre_token;
    }
    qre_token_list->size += 1;

    return 0;
}

int qre_token_list_print(qre_token_list_t *qre_token_list)
{
    qre_token_t *qre_token_cur = NULL, *qre_token_next = NULL;

    qre_token_cur = qre_token_list->begin;
    while (qre_token_cur != NULL)
    {
        qre_token_next = qre_token_cur->next;
        switch (qre_token_cur->type)
        {
            case QRE_TOKEN_CHAR:
                qre_char_print_utf_8(qre_token_cur->value);
                printf(" (char)");
                break;
            case QRE_TOKEN_CONTROL:
                qre_char_print_utf_8(qre_token_cur->value);
                printf(" (control)");
                break;
            case QRE_TOKEN_SPECIAL:
                printf("%u", (unsigned int)qre_token_cur->value);
                printf(" (special)");
                break;
        }
        printf("\n");
        qre_token_cur = qre_token_next;
    }

    return 0;
}


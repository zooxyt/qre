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

#ifndef _QRE_TOKEN_H_
#define _QRE_TOKEN_H_

#include <stdio.h>
#include <stdint.h>

#include "qre_char.h"

/* Token Types */
enum
{
    QRE_TOKEN_CHAR = 0,
    QRE_TOKEN_CONTROL,
    QRE_TOKEN_SPECIAL,
};

/* Special Tokens */
enum
{
    QRE_TOKEN_SPECIAL_DIGIT = 0,
    QRE_TOKEN_SPECIAL_NONDIGIT,
    QRE_TOKEN_SPECIAL_FORM_FEED,
    QRE_TOKEN_SPECIAL_NEWLINE,
    QRE_TOKEN_SPECIAL_CARRIAGE,
    QRE_TOKEN_SPECIAL_WHITESPACE,
    QRE_TOKEN_SPECIAL_NONWHITESPACE,
    QRE_TOKEN_SPECIAL_TAB,
    QRE_TOKEN_SPECIAL_VERTICAL_TAB,
    QRE_TOKEN_SPECIAL_WORD,
    QRE_TOKEN_SPECIAL_NONWORD,
};

/* Token */

struct qre_token
{
    int type;
    qre_char_t value;

    struct qre_token *next;
};

typedef struct qre_token qre_token_t;

qre_token_t *qre_token_new(int type, const uint32_t value);
int qre_token_destroy(qre_token_t *qre_token);
qre_token_t *qre_token_clone(qre_token_t *qre_token);

int qre_token_has_enough_tail(qre_token_t *qre_token, int length);
qre_token_t *qre_token_look_ahead(qre_token_t *qre_token, int length);
int qre_token_length(qre_token_t *qre_token);


/* Token List */

struct qre_token_list
{
    qre_token_t *begin;
    qre_token_t *end;
    size_t size;
};

typedef struct qre_token_list qre_token_list_t;

qre_token_list_t *qre_token_list_new(void);
int qre_token_list_destroy(qre_token_list_t *qre_token_list);
int qre_token_list_append(qre_token_list_t *qre_token_list, qre_token_t *new_qre_token);
int qre_token_list_print(qre_token_list_t *qre_token_list);

#endif


/*
   Quick Regular Expression Engine : Virtual Machine : IL
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

#ifndef _QRE_IL_H_
#define _QRE_IL_H_

#include <stdio.h>
#include <stdint.h>

#include "qre_char.h"


/* .data */

struct qre_il_data_item_int
{
    int value;
};
typedef struct qre_il_data_item_int qre_il_data_item_int_t;
qre_il_data_item_int_t *qre_il_data_item_int_new(int value);
int qre_il_data_item_int_destroy(qre_il_data_item_int_t *qre_il_data_item_int);

struct qre_il_data_item_str
{
    qre_char_t *str;
    size_t str_len;
};
typedef struct qre_il_data_item_str qre_il_data_item_str_t;
qre_il_data_item_str_t *qre_il_data_item_str_new(qre_char_t *str, size_t str_len);
int qre_il_data_item_str_destroy(qre_il_data_item_str_t *qre_il_data_item_str);

enum
{
    QRE_IL_DATA_ITEM_TYPE_INT = 0,
    QRE_IL_DATA_ITEM_TYPE_STR,
};
struct qre_il_data_item
{
    uint32_t id;
    int type;
    void *ptr;

    struct qre_il_data_item *next;
};
typedef struct qre_il_data_item qre_il_data_item_t;
qre_il_data_item_t *qre_il_data_item_new(uint32_t id, int type, void *ptr);
int qre_il_data_item_destroy(qre_il_data_item_t *qre_il_data_item);

struct qre_il_data
{
    struct qre_il_data_item *begin;
    struct qre_il_data_item *end;

    /* id pool */
    uint32_t id_pool;

    size_t size;
};
typedef struct qre_il_data qre_il_data_t;
qre_il_data_t *qre_il_data_new(void);
int qre_il_data_destroy(qre_il_data_t *qre_il_data);
int qre_il_data_append(qre_il_data_t *qre_il_data, \
        qre_il_data_item_t *new_qre_il_data_item);
uint32_t qre_il_data_get_id(qre_il_data_t *qre_il_data);
int qre_il_data_assign_int(uint32_t *id_out, qre_il_data_t *qre_il_data, int value);
int qre_il_data_assign_str(uint32_t *id_out, qre_il_data_t *qre_il_data, qre_char_t *str, size_t str_len);


/* .text */

struct qre_il_text_item
{
    uint32_t opcode;
    uint32_t operand;

    struct qre_il_text_item *next;
};
typedef struct qre_il_text_item qre_il_text_item_t;
qre_il_text_item_t *qre_il_text_item_new(uint32_t opcode, uint32_t operand);
int qre_il_text_item_destroy(qre_il_text_item_t *qre_il_text_item);
int qre_il_text_item_print(qre_il_text_item_t *qre_il_text_item);

struct qre_il_text
{
    struct qre_il_text_item *begin;
    struct qre_il_text_item *end;
    size_t size;
};
typedef struct qre_il_text qre_il_text_t;
qre_il_text_t *qre_il_text_new(void);
int qre_il_text_destroy(qre_il_text_t *qre_il_text);
int qre_il_text_append(qre_il_text_t *qre_il_text, \
        qre_il_text_item_t *new_qre_il_text_item);
int qre_il_text_append_with_configure(qre_il_text_t *qre_il_text, \
        uint32_t opcode, uint32_t operand);
uint32_t qre_il_text_get_pc(qre_il_text_t *qre_il_text);


/* IL */

struct qre_il
{
    qre_il_data_t *data;
    qre_il_text_t *text;
    uint32_t pc_entrance;
};
typedef struct qre_il qre_il_t;
qre_il_t *qre_il_new(void);
int qre_il_destroy(qre_il_t *qre_il);
int qre_il_print(qre_il_t *qre_il);

#endif


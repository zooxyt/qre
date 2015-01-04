/*
   Quick Regular Expression Engine : Result
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

#ifndef _QRE_RESULT_H_
#define _QRE_RESULT_H_

#include "qre_char.h"
#include "qre_str.h"

/* Group */

struct qre_result_group_num
{
    int num;
};
typedef struct qre_result_group_num qre_result_group_num_t;
qre_result_group_num_t *qre_result_group_num_new(int num);
int qre_result_group_num_destroy(qre_result_group_num_t *qre_result_group_num);

struct qre_result_group_lbl
{
    qre_string_t *str;
};
typedef struct qre_result_group_lbl qre_result_group_lbl_t;
qre_result_group_lbl_t *qre_result_group_lbl_new(qre_char_t *str, size_t str_len);
int qre_result_group_lbl_destroy(qre_result_group_lbl_t *qre_result_group_lbl);

enum
{
    QRE_RESULT_GROUP_NUM = 0,
    QRE_RESULT_GROUP_LBL,
};
struct qre_result_group
{
    int type;
    void *ptr;

    int start;
    int length;

    struct qre_result_group *next;
};
typedef struct qre_result_group qre_result_group_t;
qre_result_group_t *qre_result_group_new(int type, void *ptr);
int qre_result_group_destroy(qre_result_group_t *qre_result_group);
int qre_result_group_print(qre_result_group_t *qre_result_group);

struct qre_result_group_list
{
    qre_result_group_t *begin;
    qre_result_group_t *end;
};
typedef struct qre_result_group_list qre_result_group_list_t;
qre_result_group_list_t *qre_result_group_list_new(void);
int qre_result_group_list_destroy(qre_result_group_list_t *qre_result_group_list);
int qre_result_group_list_append(qre_result_group_list_t *qre_result_group_list, \
        qre_result_group_t *new_qre_result_group);

/* Result */

struct qre_result
{
    int success;

    /* Global match result */
    int start;
    int length;

    /* Captured match result */
    qre_result_group_list_t *groups;
};
typedef struct qre_result qre_result_t;
qre_result_t *qre_result_new(void);
int qre_result_destroy(qre_result_t *qre_result);
int qre_result_print(qre_result_t *qre_result);
int qre_result_group_by_num( \
        qre_result_group_t **qre_result_group_out, \
        qre_result_t *qre_result, \
        int num);
int qre_result_group_by_lbl( \
        qre_result_group_t **qre_result_group_out, \
        qre_result_t *qre_result, \
        qre_string_t *lbl);

#endif



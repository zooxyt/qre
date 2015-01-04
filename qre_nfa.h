/*
   Quick Regular Expression Engine : Nondeterministic Finite Automaton 
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

#ifndef _QRE_NFA_H_
#define _QRE_NFA_H_

#include <stdint.h>
#include <stdio.h>

#include "qre_char.h"
#include "qre_token.h"

enum
{
    QRE_NFA_CONDITION_CHAR = 1,
    QRE_NFA_CONDITION_ANY,
    QRE_NFA_CONDITION_RANGE,
    QRE_NFA_CONDITION_SPECIAL,
};

/* Char */
struct qre_nfa_condition_char
{
    qre_char_t value;
};
typedef struct qre_nfa_condition_char qre_nfa_condition_char_t;
qre_nfa_condition_char_t *qre_nfa_condition_char_new_with_value(qre_char_t value);
int qre_nfa_condition_char_destroy(qre_nfa_condition_char_t *qre_nfa_condition_char);
int qre_nfa_condition_char_print(qre_nfa_condition_char_t *qre_nfa_condition_char);

/* Any */
struct qre_nfa_condition_any
{
    int dummy;
};
typedef struct qre_nfa_condition_any qre_nfa_condition_any_t;
qre_nfa_condition_any_t *qre_nfa_condition_any_new(void);
int qre_nfa_condition_any_destroy(qre_nfa_condition_any_t *qre_nfa_condition_any);
int qre_nfa_condition_any_print(qre_nfa_condition_any_t *qre_nfa_condition_any);

/* Range */
struct qre_nfa_condition_range
{
    qre_char_t value_from;
    qre_char_t value_to;
};
typedef struct qre_nfa_condition_range qre_nfa_condition_range_t;
qre_nfa_condition_range_t *qre_nfa_condition_range_new_with_value(qre_char_t value_from, qre_char_t value_to);
int qre_nfa_condition_range_destroy(qre_nfa_condition_range_t *qre_nfa_condition_range);
int qre_nfa_condition_range_print(qre_nfa_condition_range_t *qre_nfa_condition_range);

/* None */
struct qre_nfa_condition_none
{
    int dummy;
};
typedef struct qre_nfa_condition_none qre_nfa_condition_none_t;
qre_nfa_condition_none_t *qre_nfa_condition_none_new(void);
int qre_nfa_condition_none_destroy(qre_nfa_condition_none_t *qre_nfa_condition_none);
int qre_nfa_condition_none_print(qre_nfa_condition_none_t *qre_nfa_condition_none);

/* Special */
enum
{
    QRE_NFA_CONDITION_SPECIAL_DIGIT = 0,
    QRE_NFA_CONDITION_SPECIAL_NONDIGIT,
    QRE_NFA_CONDITION_SPECIAL_FORM_FEED,
    QRE_NFA_CONDITION_SPECIAL_NEWLINE,
    QRE_NFA_CONDITION_SPECIAL_CARRIAGE,
    QRE_NFA_CONDITION_SPECIAL_WHITESPACE,
    QRE_NFA_CONDITION_SPECIAL_NONWHITESPACE,
    QRE_NFA_CONDITION_SPECIAL_TAB,
    QRE_NFA_CONDITION_SPECIAL_VERTICAL_TAB,
    QRE_NFA_CONDITION_SPECIAL_WORD,
    QRE_NFA_CONDITION_SPECIAL_NONWORD,
};
struct qre_nfa_condition_special
{
    int value;
};
typedef struct qre_nfa_condition_special qre_nfa_condition_special_t;
qre_nfa_condition_special_t *qre_nfa_condition_special_new_with_value(int value);
int qre_nfa_condition_special_destroy(qre_nfa_condition_special_t *qre_nfa_condition_special);
int qre_nfa_condition_special_print(qre_nfa_condition_special_t *qre_nfa_condition_special);

/* Condition */
struct qre_nfa_condition
{
    int type;
    void *ptr;

    struct qre_nfa_condition *next;
};
typedef struct qre_nfa_condition qre_nfa_condition_t;
qre_nfa_condition_t *qre_nfa_condition_new(int type, void *ptr);
int qre_nfa_condition_destroy(qre_nfa_condition_t *qre_nfa_condition);
int qre_nfa_condition_print(qre_nfa_condition_t *qre_nfa_condition);

struct qre_nfa_condition_list
{
    int eliminate;
    qre_nfa_condition_t *begin;
    qre_nfa_condition_t *end;
    size_t size;
};
typedef struct qre_nfa_condition_list qre_nfa_condition_list_t;
qre_nfa_condition_list_t *qre_nfa_condition_list_new(void);
int qre_nfa_condition_list_destroy(qre_nfa_condition_list_t *qre_nfa_condition_list);
int qre_nfa_condition_list_append(qre_nfa_condition_list_t *qre_nfa_condition_list, \
        qre_nfa_condition_t *new_qre_nfa_condition);
int qre_nfa_condition_list_print(qre_nfa_condition_list_t *qre_nfa_condition_list);

/* Attribute */
enum
{
    QRE_NFA_ATTR_CAPTURE_NUM_BEGIN = 0,
    QRE_NFA_ATTR_CAPTURE_NUM_END,
    QRE_NFA_ATTR_CAPTURE_LBL_BEGIN,
    QRE_NFA_ATTR_CAPTURE_LBL_END,
};
struct qre_nfa_attr_capture_num
{
    uint32_t num;
};
typedef struct qre_nfa_attr_capture_num qre_nfa_attr_capture_num_t;
qre_nfa_attr_capture_num_t *qre_nfa_attr_capture_num_new(uint32_t num);
int qre_nfa_attr_capture_num_destroy(qre_nfa_attr_capture_num_t *qre_nfa_attr_capture_num);

struct qre_nfa_attr_capture_label
{
    qre_char_t *label;
    size_t label_len;
};
typedef struct qre_nfa_attr_capture_label qre_nfa_attr_capture_label_t; 
qre_nfa_attr_capture_label_t *qre_nfa_attr_capture_label_new(qre_char_t *label, size_t label_len);
int qre_nfa_attr_capture_label_destroy(qre_nfa_attr_capture_label_t *qre_nfa_attr_capture_label);

struct qre_nfa_attr
{
    int type;
    void *ptr;

    struct qre_nfa_attr *next;
};
typedef struct qre_nfa_attr qre_nfa_attr_t;
qre_nfa_attr_t *qre_nfa_attr_new(int type, void *ptr);
int qre_nfa_attr_destroy(qre_nfa_attr_t *qre_nfa_attr);

struct qre_nfa_attr_list
{
    struct qre_nfa_attr *begin;
    struct qre_nfa_attr *end;
};
typedef struct qre_nfa_attr_list qre_nfa_attr_list_t;
qre_nfa_attr_list_t *qre_nfa_attr_list_new(void);
int qre_nfa_attr_list_destroy(qre_nfa_attr_list_t *qre_nfa_attr_list);
int qre_nfa_attr_list_append(qre_nfa_attr_list_t *qre_nfa_attr_list, \
        qre_nfa_attr_t *new_qre_nfa_attr);

struct qre_nfa_state_list;
/* State */
#define QRE_NFA_STATE_REPEAT_INF (-1)
struct qre_nfa_state
{
    uint32_t state;
    struct qre_nfa_condition_list *conditions;
    struct qre_nfa_state_list *next_states;

    int repeat_min;
    int repeat_max;
    int greedy;

    struct qre_nfa_state *repeat_from;
    struct qre_nfa_state *repeat_to;

    qre_nfa_attr_list_t *attrs;

    struct qre_nfa_state *next;
};
typedef struct qre_nfa_state qre_nfa_state_t;
qre_nfa_state_t *qre_nfa_state_new(void);
int qre_nfa_state_destroy(qre_nfa_state_t *qre_nfa_state);
int qre_nfa_state_print(qre_nfa_state_t *qre_nfa_state);


/* State List */
struct qre_nfa_state_list_item
{
    qre_nfa_state_t *ptr;

    struct qre_nfa_state_list_item *next;
};
typedef struct qre_nfa_state_list_item qre_nfa_state_list_item_t;
qre_nfa_state_list_item_t *qre_nfa_state_list_item_new(struct qre_nfa_state *ptr);
int qre_nfa_state_list_item_destroy(qre_nfa_state_list_item_t *qre_nfa_state_list_item);
int qre_nfa_state_list_item_print(qre_nfa_state_list_item_t *qre_nfa_state_list_item);

struct qre_nfa_state_list
{
    qre_nfa_state_list_item_t *begin;
    qre_nfa_state_list_item_t *end;

    size_t size;
};
typedef struct qre_nfa_state_list qre_nfa_state_list_t;
qre_nfa_state_list_t *qre_nfa_state_list_new(void);
int qre_nfa_state_list_destroy(qre_nfa_state_list_t *qre_nfa_state_list);
int qre_nfa_state_list_append(qre_nfa_state_list_t *qre_nfa_state_list, \
        qre_nfa_state_list_item_t *new_qre_nfa_state_list_item);
int qre_nfa_state_list_append_with_configure(qre_nfa_state_list_t *qre_nfa_state_list, \
        struct qre_nfa_state *ptr);
int qre_nfa_state_list_print(qre_nfa_state_list_t *qre_nfa_state_list);


/* State Table */
struct qre_nfa_state_table
{
    struct qre_nfa_state *begin;
    struct qre_nfa_state *end;
    size_t size;

    uint32_t state_num_start;
    uint32_t state_num_end;

    uint32_t state_num_pool;
};
typedef struct qre_nfa_state_table qre_nfa_state_table_t;

qre_nfa_state_table_t *qre_nfa_state_table_new(void);
int qre_nfa_state_table_destroy(qre_nfa_state_table_t *qre_nfa_state_table);
int qre_nfa_state_table_append(qre_nfa_state_table_t *qre_nfa_state_table, \
        qre_nfa_state_t *new_qre_nfa_state);
int qre_nfa_state_table_print(qre_nfa_state_table_t *qre_nfa_state_table);
uint32_t qre_nfa_state_table_state_num_alloc(qre_nfa_state_table_t *qre_nfa_state_table);

#endif


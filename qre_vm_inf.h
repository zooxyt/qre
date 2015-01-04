/*
   Quick Regular Expression Engine : VM : Infrastructure
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

#ifndef _QRE_VM_INF_H_
#define _QRE_VM_INF_H_

#include <stdio.h>
#include <stdint.h>

#include "qre_char.h"
#include "qre_str.h"

/* .data */
struct qre_vm_data_item_num
{
    int value;
};
typedef struct qre_vm_data_item_num qre_vm_data_item_num_t;
qre_vm_data_item_num_t *qre_vm_data_item_num_new(int value);
int qre_vm_data_item_num_destroy(qre_vm_data_item_num_t *qre_vm_data_item_num);

struct qre_vm_data_item_str
{
    qre_char_t *str;
    size_t str_len;
};
typedef struct qre_vm_data_item_str qre_vm_data_item_str_t;
qre_vm_data_item_str_t *qre_vm_data_item_str_new(qre_char_t *str, size_t str_len);
int qre_vm_data_item_str_destroy(qre_vm_data_item_str_t *qre_vm_data_item_str);

enum
{
    QRE_VM_DATA_ITEM_NONE = 0,
    QRE_VM_DATA_ITEM_NUM,
    QRE_VM_DATA_ITEM_STR,
};
struct qre_vm_data_item
{
    uint32_t id;
    int type;
    void *ptr;
};
typedef struct qre_vm_data_item qre_vm_data_item_t;

struct qre_vm_data
{
    struct qre_vm_data_item *items;
    size_t size;
};
typedef struct qre_vm_data qre_vm_data_t;

/* .text */
struct qre_vm_text_item
{
    uint32_t opcode;
    uint32_t operand;
};
typedef struct qre_vm_text_item qre_vm_text_item_t;

struct qre_vm_text
{
    struct qre_vm_text_item *items;
    size_t size;
};
typedef struct qre_vm_text qre_vm_text_t;
qre_vm_text_t *qre_vm_text_new(void);
int qre_vm_text_destroy(qre_vm_text_t *qre_vm_text);


/* Capture */
struct qre_vm_capture
{
    uint32_t id;
    qre_char_t *str_p;
    qre_char_t *str_endp;
    struct qre_vm_capture *next; 
};
typedef struct qre_vm_capture qre_vm_capture_t;
qre_vm_capture_t *qre_vm_capture_new(uint32_t id);
int qre_vm_capture_destroy(qre_vm_capture_t *qre_vm_capture);

struct qre_vm_capture_list
{
    qre_vm_capture_t *begin;
    qre_vm_capture_t *end;
};
typedef struct qre_vm_capture_list qre_vm_capture_list_t;
qre_vm_capture_list_t *qre_vm_capture_list_new(void);
int qre_vm_capture_list_destroy(qre_vm_capture_list_t *qre_vm_capture_list);
int qre_vm_capture_list_append(qre_vm_capture_list_t *qre_vm_capture_list, \
        qre_vm_capture_t *new_qre_vm_capture);
qre_vm_capture_t *qre_vm_capture_list_lookup( \
        qre_vm_capture_list_t *qre_vm_capture_list, \
        uint32_t id);


/* Running Stack */
#define QRE_VM_REPEAT_INF (-1)
#define QRE_VM_REPEAT_MIN_DEFAULT (1)
#define QRE_VM_REPEAT_MAX_DEFAULT (1)
enum
{
    QRE_VM_REPEAT_TYPE_NONE = 0, 
    QRE_VM_REPEAT_TYPE_GREEDY,
    QRE_VM_REPEAT_TYPE_NON_GREEDY,
};
struct qre_vm_run_stack_frame
{
    uint32_t pc;
    qre_char_t *str_p;
    qre_char_t *str_startp;

    /* Greedy or not */
    int type;

    /* Repeat */
    int repeat_min;
    int repeat_max;
    int repeat;
    int repeat_fail;

    /* Capture */
    qre_vm_capture_list_t *captures;

    struct qre_vm_run_stack_frame *prev;
    struct qre_vm_run_stack_frame *next;
};
typedef struct qre_vm_run_stack_frame qre_vm_run_stack_frame_t;
qre_vm_run_stack_frame_t *qre_vm_run_stack_frame_new(void);
int qre_vm_run_stack_frame_destroy(qre_vm_run_stack_frame_t *qre_vm_run_stack_frame);

struct qre_vm_run_stack
{
    struct qre_vm_run_stack_frame *top;
    struct qre_vm_run_stack_frame *bottom;
    size_t size;
};
typedef struct qre_vm_run_stack qre_vm_run_stack_t;
qre_vm_run_stack_t *qre_vm_run_stack_new(void);
int qre_vm_run_stack_destroy(qre_vm_run_stack_t *qre_vm_run_stack);
int qre_vm_run_stack_push(qre_vm_run_stack_t *qre_vm_run_stack, \
        qre_vm_run_stack_frame_t *new_qre_vm_run_stack_frame);
int qre_vm_run_stack_push_with_configure(qre_vm_run_stack_t *qre_vm_run_stack, \
        uint32_t pc, qre_char_t *str_p, int type);
int qre_vm_run_stack_pop(qre_vm_run_stack_t *qre_vm_run_stack);
qre_vm_capture_t *qre_vm_run_stack_capture_list_lookup( \
        qre_vm_run_stack_t *qre_vm_run_stack, \
        uint32_t id);


/* Flags */
enum
{
    QRE_VM_TEST_RESULT = 0,
    QRE_VM_TEST_FAIL, 
    QRE_VM_TEST_SUCCESS, 
};

/* Virtual Machine */
struct qre_vm
{
    qre_string_t *str;

    /* .data */
    qre_vm_data_t *data;

    /* .text */
    qre_vm_text_t *text;

    /* Test Flags */
    uint32_t flags;

    /* Running Stack */
    qre_vm_run_stack_t *run_stack;
};
typedef struct qre_vm qre_vm_t;
qre_vm_t *qre_vm_new(void);
int qre_vm_destroy(qre_vm_t *qre_vm);

int qre_vm_flags_set(qre_vm_t *qre_vm, int idx);
int qre_vm_flags_clear(qre_vm_t *qre_vm, int idx);
int qre_vm_flags_set_by_value(qre_vm_t *qre_vm, int idx, int value);
int qre_vm_flags_test(qre_vm_t *qre_vm, int idx);

#endif


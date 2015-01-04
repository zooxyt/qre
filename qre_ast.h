/*
   Quick Regular Expression Engine : AST
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

#ifndef _QRE_AST_H_
#define _QRE_AST_H_

#include <stdint.h>

#include "qre_char.h"
#include "qre_token.h"

/* Capture */
enum
{
    QRE_AST_NODE_CAPTURE_TYPE_NONE = 1,
    QRE_AST_NODE_CAPTURE_TYPE_NUM,
    QRE_AST_NODE_CAPTURE_TYPE_LABEL,
};

struct qre_ast_node_capture_num
{
    uint32_t num;
};
typedef struct qre_ast_node_capture_num qre_ast_node_capture_num_t;
qre_ast_node_capture_num_t *qre_ast_node_capture_num_new(uint32_t num);
int qre_ast_node_capture_num_destroy(qre_ast_node_capture_num_t *qre_ast_node_capture_num);

struct qre_ast_node_capture_label
{
    qre_char_t *label;
    size_t label_len;
};
typedef struct qre_ast_node_capture_label qre_ast_node_capture_label_t;
qre_ast_node_capture_label_t *qre_ast_node_capture_label_new(qre_char_t *label, size_t label_len);
int qre_ast_node_capture_label_destroy(qre_ast_node_capture_label_t *qre_ast_node_capture_label);

struct qre_ast_node_capture
{
    int type;
    void *ptr;
    struct qre_ast_node_capture *next;
};
typedef struct qre_ast_node_capture qre_ast_node_capture_t;
qre_ast_node_capture_t *qre_ast_node_capture_new(int type, void *ptr);
int qre_ast_node_capture_destroy(qre_ast_node_capture_t *qre_ast_node_capture);

struct qre_ast_node_capture_list
{
    qre_ast_node_capture_t *begin;
    qre_ast_node_capture_t *end;
};
typedef struct qre_ast_node_capture_list qre_ast_node_capture_list_t;
qre_ast_node_capture_list_t *qre_ast_node_capture_list_new(void);
int qre_ast_node_capture_list_destroy(qre_ast_node_capture_list_t *qre_ast_node_capture_list);
int qre_ast_node_capture_list_append(qre_ast_node_capture_list_t *qre_ast_node_capture_list, \
        qre_ast_node_capture_t *new_qre_ast_node_capture);


enum 
{
    QRE_AST_NODE_TYPE_SEQ = 1, /* Sequence of Node */
    QRE_AST_NODE_TYPE_PLAIN,   /* Plain Char */
    QRE_AST_NODE_TYPE_ANY,     /* Any Char . */
    QRE_AST_NODE_TYPE_GROUP,   /* Group Char [] */
    QRE_AST_NODE_TYPE_BRANCH,  /* Branch (|) */
    QRE_AST_NODE_TYPE_SPECIAL, /* Special Char */
};
/* Definitions */
struct qre_ast_node;
/* AST Generic Node */
#define QRE_AST_NODE_REPEAT_INF (-1)
struct qre_ast_node
{
    int type;
    void *ptr;

    int repeat_min;
    int repeat_max;
    int greedy;

    int capture;

};
typedef struct qre_ast_node qre_ast_node_t;

struct qre_ast_node *qre_ast_node_new(int type, void *ptr);
int qre_ast_node_destroy(struct qre_ast_node *ast_node);


/* Specialized AST Node */

/* AST Node : Seq */
struct qre_ast_node_seq_node
{
    qre_ast_node_t *node;
    struct qre_ast_node_seq_node *next;
};
typedef struct qre_ast_node_seq_node qre_ast_node_seq_node_t;

qre_ast_node_seq_node_t *qre_ast_node_seq_node_new(qre_ast_node_t *qre_ast_node);
int qre_ast_node_seq_node_destroy(qre_ast_node_seq_node_t *qre_ast_node_seq_node);

struct qre_ast_node_seq
{
    struct qre_ast_node_seq_node *begin;
    struct qre_ast_node_seq_node *end;
    size_t size;
};
typedef struct qre_ast_node_seq qre_ast_node_seq_t;

qre_ast_node_seq_t *qre_ast_node_seq_new(void);
int qre_ast_node_seq_destroy(qre_ast_node_seq_t *qre_ast_node_seq);
int qre_ast_node_seq_append(qre_ast_node_seq_t *qre_ast_node_seq, \
        qre_ast_node_seq_node_t *new_qre_ast_node_seq_node);
int qre_ast_node_seq_append_with_node(qre_ast_node_seq_t *qre_ast_node_seq, \
        qre_ast_node_t *new_qre_ast_node);

/* AST Node : Plain */
struct qre_ast_node_plain
{
    qre_token_t *token;
};
typedef struct qre_ast_node_plain qre_ast_node_plain_t;

qre_ast_node_plain_t *qre_ast_node_plain_new(qre_token_t *token);
int qre_ast_node_plain_destroy(qre_ast_node_plain_t *qre_ast_node_plain);

/* AST Node : Any */
struct qre_ast_node_any
{
    qre_token_t *token;
};
typedef struct qre_ast_node_any qre_ast_node_any_t;

qre_ast_node_any_t *qre_ast_node_any_new(qre_token_t *token);
int qre_ast_node_any_destroy(qre_ast_node_any_t *qre_ast_node_any);

/* AST Node : Group */
enum 
{
    QRE_AST_NODE_GROUP_OPTION_TYPE_SOLO = 1,
    QRE_AST_NODE_GROUP_OPTION_TYPE_RANGE,
};

struct qre_ast_node_group_option_solo
{
    qre_token_t *token;
};
typedef struct qre_ast_node_group_option_solo qre_ast_node_group_option_solo_t;
qre_ast_node_group_option_solo_t *qre_ast_node_group_option_solo_new(qre_token_t *token);
int qre_ast_node_group_option_solo_destroy(qre_ast_node_group_option_solo_t *qre_ast_node_group_option_solo);

struct qre_ast_node_group_option_range
{
    qre_token_t *token_from;
    qre_token_t *token_to;
};
typedef struct qre_ast_node_group_option_range qre_ast_node_group_option_range_t;
qre_ast_node_group_option_range_t *qre_ast_node_group_option_range_new(qre_token_t *token_from, qre_token_t *token_to);
int qre_ast_node_group_option_range_destroy(qre_ast_node_group_option_range_t *qre_ast_node_group_option_range);

struct qre_ast_node_group_option
{
    int type;
    void *ptr;
    struct qre_ast_node_group_option *next;
};
typedef struct qre_ast_node_group_option qre_ast_node_group_option_t;
qre_ast_node_group_option_t *qre_ast_node_group_option_new(int type, void *ptr);
int qre_ast_node_group_option_destroy(qre_ast_node_group_option_t *qre_ast_node_group_option);

struct qre_ast_node_group_option_list
{
    qre_ast_node_group_option_t *begin;
    qre_ast_node_group_option_t *end;
};
typedef struct qre_ast_node_group_option_list qre_ast_node_group_option_list_t;
qre_ast_node_group_option_list_t *qre_ast_node_group_option_list_new(void);
int qre_ast_node_group_option_list_destroy(qre_ast_node_group_option_list_t *qre_ast_node_group_option_list);
int qre_ast_node_group_option_list_append(qre_ast_node_group_option_list_t *qre_ast_node_group_option_list, \
        qre_ast_node_group_option_t *new_qre_ast_node_group_option);

struct qre_ast_node_group
{
    int elliminate;
    qre_ast_node_group_option_list_t *options;
};
typedef struct qre_ast_node_group qre_ast_node_group_t;
qre_ast_node_group_t *qre_ast_node_group_new(void);
int qre_ast_node_group_destroy(qre_ast_node_group_t *qre_ast_node_group);


/* AST Node : Branch */
struct qre_ast_node_branch_sub
{
    qre_ast_node_t *node;
    struct qre_ast_node_branch_sub *next;
};
typedef struct qre_ast_node_branch_sub qre_ast_node_branch_sub_t;
qre_ast_node_branch_sub_t *qre_ast_node_branch_sub_new(qre_ast_node_t *qre_ast_node);
int qre_ast_node_branch_sub_destroy(qre_ast_node_branch_sub_t *qre_ast_node_branch_sub);

struct qre_ast_node_branch_sub_list
{
    qre_ast_node_branch_sub_t *begin;
    qre_ast_node_branch_sub_t *end;
};
typedef struct qre_ast_node_branch_sub_list qre_ast_node_branch_sub_list_t;
qre_ast_node_branch_sub_list_t *qre_ast_node_branch_sub_list_new(void);
int qre_ast_node_branch_sub_list_destroy(qre_ast_node_branch_sub_list_t *qre_ast_node_branch_sub_list);
int qre_ast_node_branch_sub_list_append(qre_ast_node_branch_sub_list_t *qre_ast_node_branch_sub_list, \
        qre_ast_node_branch_sub_t *new_qre_ast_node_branch_sub);

struct qre_ast_node_branch
{
    qre_ast_node_branch_sub_list_t *branches;

    qre_ast_node_capture_list_t *capture_list;
};
typedef struct qre_ast_node_branch qre_ast_node_branch_t;
qre_ast_node_branch_t *qre_ast_node_branch_new(void);
int qre_ast_node_branch_destroy(qre_ast_node_branch_t *qre_ast_node_branch);

/* AST Node : Special */
enum
{
    QRE_AST_NODE_SPECIAL_DIGIT = 0,
    QRE_AST_NODE_SPECIAL_NONDIGIT,
    QRE_AST_NODE_SPECIAL_FORM_FEED,
    QRE_AST_NODE_SPECIAL_NEWLINE,
    QRE_AST_NODE_SPECIAL_CARRIAGE,
    QRE_AST_NODE_SPECIAL_WHITESPACE,
    QRE_AST_NODE_SPECIAL_NONWHITESPACE,
    QRE_AST_NODE_SPECIAL_TAB,
    QRE_AST_NODE_SPECIAL_VERTICAL_TAB,
    QRE_AST_NODE_SPECIAL_WORD,
    QRE_AST_NODE_SPECIAL_NONWORD,
};
struct qre_ast_node_special
{
    int value;
};
typedef struct qre_ast_node_special qre_ast_node_special_t;
qre_ast_node_special_t *qre_ast_node_special_new(int value);
int qre_ast_node_special_destroy(qre_ast_node_special_t *qre_ast_node_special);

/* AST */
struct qre_ast
{
    struct qre_ast_node *root;
};

typedef struct qre_ast qre_ast_t;

struct qre_ast *qre_ast_new(struct qre_ast_node *root);
int qre_ast_destroy(struct qre_ast *ast);


#endif


/*
   Quick Regular Expression Engine : Parsing
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
#include <stdio.h>
#include <stdlib.h>

#include "qre_common.h"
#include "qre_token.h"
#include "qre_ast.h"
#include "qre_parser.h"
#include "qre_err.h"

enum
{
    QRE_PARSER_OPT_TOPLEVEL = 0,
};

/* Declarations */
static int qre_parse_plain(qre_ast_node_t **qre_ast_node_out, qre_token_t **qre_token_cur_io, uint32_t options);
static int qre_parse_any(qre_ast_node_t **qre_ast_node_out, qre_token_t **qre_token_cur_io, uint32_t options);
static int qre_parse_group(qre_ast_node_t **qre_ast_node_out, qre_token_t **qre_token_cur_io, uint32_t options);
static int qre_parse_branch(qre_ast_node_t **qre_ast_node_out, qre_token_t **qre_token_cur_io, uint32_t options);
static int qre_parse_seq(qre_ast_node_t **qre_ast_node_out, qre_token_t **qre_token_cur_io, uint32_t options);

/* Plain */
static int qre_parse_plain(qre_ast_node_t **qre_ast_node_out, qre_token_t **qre_token_cur_io, uint32_t options)
{
    int ret = 0;
    qre_token_t *qre_token_cur = *qre_token_cur_io;
    qre_ast_node_t *new_qre_ast_node = NULL;
    qre_ast_node_plain_t *new_qre_ast_node_plain = NULL;

    (void)options;

    new_qre_ast_node_plain = qre_ast_node_plain_new(qre_token_cur);
    if (new_qre_ast_node_plain == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail;
    }
    new_qre_ast_node = qre_ast_node_new(QRE_AST_NODE_TYPE_PLAIN, new_qre_ast_node_plain);
    if (new_qre_ast_node == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail;
    }
    new_qre_ast_node_plain = NULL;

    *qre_ast_node_out = new_qre_ast_node; new_qre_ast_node = NULL;

    qre_token_cur = qre_token_cur->next; 
    *qre_token_cur_io = qre_token_cur;

    goto done;
fail:
    if (new_qre_ast_node_plain != NULL) qre_ast_node_plain_destroy(new_qre_ast_node_plain);
    if (new_qre_ast_node != NULL) qre_ast_node_destroy(new_qre_ast_node);
done:
    return ret;
}

/* Any */
static int qre_parse_any(qre_ast_node_t **qre_ast_node_out, qre_token_t **qre_token_cur_io, uint32_t options)
{
    int ret = 0;
    qre_token_t *qre_token_cur = *qre_token_cur_io;
    qre_ast_node_t *new_qre_ast_node = NULL;
    qre_ast_node_any_t *new_qre_ast_node_any = NULL;

    (void)options;

    new_qre_ast_node_any = qre_ast_node_any_new(qre_token_cur);
    if (new_qre_ast_node_any == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail;
    }
    new_qre_ast_node = qre_ast_node_new(QRE_AST_NODE_TYPE_ANY, new_qre_ast_node_any);
    if (new_qre_ast_node == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail;
    }
    new_qre_ast_node_any = NULL;

    *qre_ast_node_out = new_qre_ast_node; new_qre_ast_node = NULL;

    qre_token_cur = qre_token_cur->next; 
    *qre_token_cur_io = qre_token_cur;

    goto done;
fail:
    if (new_qre_ast_node_any != NULL) qre_ast_node_any_destroy(new_qre_ast_node_any);
    if (new_qre_ast_node != NULL) qre_ast_node_destroy(new_qre_ast_node);
done:
    return ret;
}

/* Group */
static int qre_parse_group(qre_ast_node_t **qre_ast_node_out, qre_token_t **qre_token_cur_io, uint32_t options)
{
    int ret = 0;
    qre_token_t *qre_token_cur = *qre_token_cur_io;
    qre_token_t *qre_token_from, *qre_token_to;

    qre_ast_node_t *new_qre_ast_node = NULL;
    qre_ast_node_group_t *new_qre_ast_node_group = NULL;
    qre_ast_node_group_option_range_t *new_qre_ast_node_group_option_range = NULL;
    qre_ast_node_group_option_solo_t *new_qre_ast_node_group_option_solo = NULL;
    qre_ast_node_group_option_t *new_qre_ast_node_group_option = NULL;

    (void)options;

    new_qre_ast_node_group = qre_ast_node_group_new();
    if (new_qre_ast_node_group == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail;
    }

    /* Skip '[' */
    qre_token_cur = qre_token_cur->next; 

    /* Elliminate */
    if (qre_token_cur->value == '^')
    {
        new_qre_ast_node_group->elliminate = TRUE;
        qre_token_cur = qre_token_cur->next;
    }

    while ((qre_token_cur != NULL) && (qre_token_cur->value != ']'))
    {
        if ((qre_token_has_enough_tail(qre_token_cur, 3)) && \
                (qre_token_look_ahead(qre_token_cur, 1)->value == '-'))
        {
            /* Range */
            qre_token_from = qre_token_cur;
            qre_token_to = qre_token_look_ahead(qre_token_cur, 2);

            new_qre_ast_node_group_option_range = qre_ast_node_group_option_range_new(qre_token_from, qre_token_to);
            if (new_qre_ast_node_group_option_range == NULL)
            {
                ret = -QRE_ERR_OUT_OF_MEMORY;
                goto fail;
            }
            new_qre_ast_node_group_option = qre_ast_node_group_option_new( \
                    QRE_AST_NODE_GROUP_OPTION_TYPE_RANGE, \
                    new_qre_ast_node_group_option_range);
            if (new_qre_ast_node_group_option == NULL)
            {
                ret = -QRE_ERR_OUT_OF_MEMORY;
                goto fail;
            }
            new_qre_ast_node_group_option_range = NULL;

            qre_ast_node_group_option_list_append(new_qre_ast_node_group->options, new_qre_ast_node_group_option);
            new_qre_ast_node_group_option = NULL;
            qre_token_cur = qre_token_cur->next->next->next;
        }
        else
        {
            /* Solo */
            new_qre_ast_node_group_option_solo = qre_ast_node_group_option_solo_new(qre_token_cur);
            if (new_qre_ast_node_group_option_solo == NULL)
            {
                ret = -QRE_ERR_OUT_OF_MEMORY;
                goto fail;
            }
            new_qre_ast_node_group_option = qre_ast_node_group_option_new( \
                    QRE_AST_NODE_GROUP_OPTION_TYPE_SOLO, \
                    new_qre_ast_node_group_option_solo);
            if (new_qre_ast_node_group_option == NULL)
            {
                ret = -QRE_ERR_OUT_OF_MEMORY;
                goto fail;
            }
            new_qre_ast_node_group_option_solo = NULL;

            qre_ast_node_group_option_list_append(new_qre_ast_node_group->options, new_qre_ast_node_group_option);
            new_qre_ast_node_group_option = NULL;
            qre_token_cur = qre_token_cur->next;
        }
    }

    new_qre_ast_node = qre_ast_node_new(QRE_AST_NODE_TYPE_GROUP, new_qre_ast_node_group);
    if (new_qre_ast_node == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail;
    }
    new_qre_ast_node_group = NULL;

    *qre_ast_node_out = new_qre_ast_node; new_qre_ast_node = NULL;

    /* Skip ']' */
    if (qre_token_cur != NULL) qre_token_cur = qre_token_cur->next; 
    *qre_token_cur_io = qre_token_cur;

    goto done;
fail:
    if (new_qre_ast_node_group_option_range != NULL) qre_ast_node_group_option_range_destroy(new_qre_ast_node_group_option_range);
    if (new_qre_ast_node_group_option_solo != NULL) qre_ast_node_group_option_solo_destroy(new_qre_ast_node_group_option_solo);
    if (new_qre_ast_node_group_option != NULL) qre_ast_node_group_option_destroy(new_qre_ast_node_group_option);
    if (new_qre_ast_node_group != NULL) qre_ast_node_group_destroy(new_qre_ast_node_group);
    if (new_qre_ast_node != NULL) qre_ast_node_destroy(new_qre_ast_node);
done:
    return ret;
}

/* Branch */
static int qre_parse_branch(qre_ast_node_t **qre_ast_node_out, qre_token_t **qre_token_cur_io, uint32_t options)
{
    int ret = 0;
    qre_token_t *qre_token_cur = *qre_token_cur_io;

    qre_ast_node_t *new_qre_ast_node = NULL;
    qre_ast_node_branch_t *new_qre_ast_node_branch = NULL;
    qre_ast_node_branch_sub_t *new_qre_ast_node_branch_sub = NULL;

    qre_token_t *qre_token_cur2 = NULL;
    size_t label_length;
    size_t idx;
    qre_char_t *new_named_capture_label = NULL, *new_named_capture_label_p;
    qre_ast_node_capture_label_t *new_qre_ast_node_capture_label = NULL;
    qre_ast_node_capture_t *new_qre_ast_node_capture = NULL;

    (void)options;

    new_qre_ast_node_branch = qre_ast_node_branch_new();
    if (new_qre_ast_node_branch == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail;
    }

    /* Skip '(' */
    qre_token_cur = qre_token_cur->next; 

    if ((qre_token_cur != NULL) && (qre_token_cur->value == '?'))
    {
        /* Skip '?' */
        qre_token_cur = qre_token_cur->next; 

        if (qre_token_cur != NULL)
        {
            switch (qre_token_cur->value)
            {
                case '<':
                    /* Named Capture (?<name>...) */

                    /* Skip '<' */
                    qre_token_cur = qre_token_cur->next; 

                    /* Length of the label */
                    label_length = 0;
                    qre_token_cur2 = qre_token_cur;
                    while ((qre_token_cur2 != NULL) && (qre_token_cur2->value != '>'))
                    {
                        qre_token_cur2 = qre_token_cur2->next;
                        label_length += 1;
                    }
                    /* Zero-length */
                    if (label_length == 0)
                    {
                        goto skip_zero_length;
                    }
                    if (qre_token_cur2 == NULL)
                    {
                        ret = -QRE_ERR_CORRUPT;
                        goto fail;
                    }

                    /* Create the label */
                    new_named_capture_label = (qre_char_t *)malloc(sizeof(qre_char_t) * label_length);
                    if (new_named_capture_label == NULL)
                    {
                        ret = -QRE_ERR_OUT_OF_MEMORY;
                        goto fail;
                    }
                    new_named_capture_label_p = new_named_capture_label;
                    for (idx = 0; idx != label_length; idx++)
                    {
                        *new_named_capture_label_p++ = qre_token_cur->value;
                        qre_token_cur = qre_token_cur->next;
                    }

skip_zero_length:
                    /* Skip '>' */
                    if (qre_token_cur == NULL)
                    {
                        ret = -QRE_ERR_CORRUPT;
                        goto fail;
                    }
                    qre_token_cur = qre_token_cur->next; 

                    /* Create new capture */
                    new_qre_ast_node_capture_label = qre_ast_node_capture_label_new(new_named_capture_label, label_length);
                    if (new_qre_ast_node_capture_label == NULL) { goto fail; }
                    free(new_named_capture_label); new_named_capture_label = NULL;
                    new_qre_ast_node_capture = qre_ast_node_capture_new( \
                            QRE_AST_NODE_CAPTURE_TYPE_LABEL, \
                            new_qre_ast_node_capture_label);
                    if (new_qre_ast_node_capture == NULL) { goto fail; }
                    new_qre_ast_node_capture_label = NULL;

                    qre_ast_node_capture_list_append(new_qre_ast_node_branch->capture_list, \
                            new_qre_ast_node_capture);
                    new_qre_ast_node_capture = NULL;

                    break;
                default:
                    ret = -QRE_ERR_INTERNAL;
                    goto fail;
            }
        }
        else
        {
            ret = -QRE_ERR_INTERNAL;
            goto fail;
        }
    }

    for (;;)
    {
        ret = qre_parse_seq(&new_qre_ast_node, &qre_token_cur, options);
        if (ret != 0) goto fail;

        new_qre_ast_node_branch_sub = qre_ast_node_branch_sub_new(new_qre_ast_node);
        if (new_qre_ast_node_branch_sub == NULL) 
        {
            ret = -QRE_ERR_OUT_OF_MEMORY;
            goto fail;
        }
        new_qre_ast_node = NULL;

        qre_ast_node_branch_sub_list_append(new_qre_ast_node_branch->branches, \
                new_qre_ast_node_branch_sub);
        new_qre_ast_node_branch_sub = NULL;
        if (qre_token_cur->value == ')') break;
        else if (qre_token_cur->value == '|') 
        {
            /* Skip '|' */
            qre_token_cur = qre_token_cur->next; 
        }
    }

    new_qre_ast_node = qre_ast_node_new(QRE_AST_NODE_TYPE_BRANCH, new_qre_ast_node_branch);
    if (new_qre_ast_node == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail;
    }
    new_qre_ast_node_branch = NULL;

    *qre_ast_node_out = new_qre_ast_node; new_qre_ast_node = NULL;

    /* Skip ')' */
    qre_token_cur = qre_token_cur->next; 
    *qre_token_cur_io = qre_token_cur;

    goto done;
fail:
    if (new_qre_ast_node != NULL) qre_ast_node_destroy(new_qre_ast_node);
    if (new_qre_ast_node_branch_sub != NULL) qre_ast_node_branch_sub_destroy(new_qre_ast_node_branch_sub);
    if (new_qre_ast_node_branch != NULL) qre_ast_node_branch_destroy(new_qre_ast_node_branch);
    if (new_qre_ast_node_capture_label != NULL) qre_ast_node_capture_label_destroy(new_qre_ast_node_capture_label);
    if (new_qre_ast_node_capture != NULL) qre_ast_node_capture_destroy(new_qre_ast_node_capture);
    if (new_named_capture_label != NULL) free(new_named_capture_label);
done:
    return ret;
}

/* Special */
static int qre_parse_special(qre_ast_node_t **qre_ast_node_out, qre_token_t **qre_token_cur_io, uint32_t options)
{
    int ret = 0;
    qre_token_t *qre_token_cur = *qre_token_cur_io;
    qre_ast_node_t *new_qre_ast_node = NULL;
    qre_ast_node_special_t *new_qre_ast_node_special = NULL;
    int value;

    (void)options;

    switch (qre_token_cur->value)
    {
        case QRE_TOKEN_SPECIAL_DIGIT:
            value = QRE_AST_NODE_SPECIAL_DIGIT;
            break;
        case QRE_TOKEN_SPECIAL_NONDIGIT:
            value = QRE_AST_NODE_SPECIAL_NONDIGIT;
            break;
        case QRE_TOKEN_SPECIAL_FORM_FEED:
            value = QRE_AST_NODE_SPECIAL_FORM_FEED;
            break;
        case QRE_TOKEN_SPECIAL_NEWLINE:
            value = QRE_AST_NODE_SPECIAL_NEWLINE;
            break;
        case QRE_TOKEN_SPECIAL_CARRIAGE:
            value = QRE_AST_NODE_SPECIAL_CARRIAGE;
            break;
        case QRE_TOKEN_SPECIAL_WHITESPACE:
            value = QRE_AST_NODE_SPECIAL_WHITESPACE;
            break;
        case QRE_TOKEN_SPECIAL_NONWHITESPACE:
            value = QRE_AST_NODE_SPECIAL_NONWHITESPACE;
            break;
        case QRE_TOKEN_SPECIAL_TAB:
            value = QRE_AST_NODE_SPECIAL_TAB;
            break;
        case QRE_TOKEN_SPECIAL_VERTICAL_TAB:
            value = QRE_AST_NODE_SPECIAL_VERTICAL_TAB;
            break;
        case QRE_TOKEN_SPECIAL_WORD:
            value = QRE_AST_NODE_SPECIAL_WORD;
            break;
        case QRE_TOKEN_SPECIAL_NONWORD:
            value = QRE_AST_NODE_SPECIAL_NONWORD;
            break;
        default:
            ret = -QRE_ERR_INTERNAL;
            goto fail;
    }
    new_qre_ast_node_special = qre_ast_node_special_new(value);
    if (new_qre_ast_node_special == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail;
    }
    new_qre_ast_node = qre_ast_node_new(QRE_AST_NODE_TYPE_SPECIAL, new_qre_ast_node_special);
    if (new_qre_ast_node == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail;
    }
    new_qre_ast_node_special = NULL;

    *qre_ast_node_out = new_qre_ast_node; new_qre_ast_node = NULL;

    qre_token_cur = qre_token_cur->next; 
    *qre_token_cur_io = qre_token_cur;

    goto done;
fail:
    if (new_qre_ast_node_special != NULL) qre_ast_node_special_destroy(new_qre_ast_node_special);
    if (new_qre_ast_node != NULL) qre_ast_node_destroy(new_qre_ast_node);
done:
    return ret;
}

/* Repeat */
static int qre_parse_repeat(qre_ast_node_t *new_qre_ast_node, qre_token_t **qre_token_cur_io)
{
    int ret = 0;
    qre_token_t *qre_token_cur = *qre_token_cur_io;
    int value;

    if (qre_token_cur != NULL)
    {
        if (qre_token_cur->type == QRE_TOKEN_CONTROL)
        {
            switch (qre_token_cur->value)
            {
                case '{':
                    qre_token_cur = qre_token_cur->next;

                    /* Min */
                    value = 0;
                    for (;;)
                    {
                        if (qre_token_cur->type == QRE_TOKEN_CHAR)
                        {
                            if (('0' <= (qre_token_cur->value)) && ((qre_token_cur->value) <= '9'))
                            {
                                value = value * 10 + (qre_token_cur->value - '0');
                                qre_token_cur = qre_token_cur->next;
                            }
                            else
                            {
                                ret = -QRE_ERR_CORRUPT;
                                goto fail;
                            }
                        }
                        else
                        {
                            if (qre_token_cur->value == ',')
                            {
                                break;
                            }
                            else if (qre_token_cur->value == '}')
                            {
                                break;
                            }
                            else
                            {
                                ret = -QRE_ERR_CORRUPT;
                                goto fail;
                            }
                        }
                    }
                    new_qre_ast_node->repeat_min = value;
                    /* Max */
                    if (qre_token_cur->type == QRE_TOKEN_CHAR)
                    {
                        ret = -QRE_ERR_CORRUPT;
                        goto fail;
                    }
                    else
                    {
                        if (qre_token_cur->value == '}')
                        {
                            qre_token_cur = qre_token_cur->next;
                            new_qre_ast_node->repeat_max = new_qre_ast_node->repeat_min;
                        }
                        else if (qre_token_cur->value == ',')
                        {
                            qre_token_cur = qre_token_cur->next;

                            /* Min */
                            value = 0;
                            for (;;)
                            {
                                if (qre_token_cur->type == QRE_TOKEN_CHAR)
                                {
                                    if (('0' <= (qre_token_cur->value)) && ((qre_token_cur->value) <= '9'))
                                    {
                                        value = value * 10 + (qre_token_cur->value - '0');
                                        qre_token_cur = qre_token_cur->next;
                                    }
                                    else
                                    {
                                        ret = -QRE_ERR_CORRUPT;
                                        goto fail;
                                    }
                                }
                                else
                                {
                                    if (qre_token_cur->value == ',')
                                    {
                                        qre_token_cur = qre_token_cur->next;
                                        break;
                                    }
                                    else if (qre_token_cur->value == '}')
                                    {
                                        qre_token_cur = qre_token_cur->next;
                                        break;
                                    }
                                    else
                                    {
                                        ret = -QRE_ERR_CORRUPT;
                                        goto fail;
                                    }
                                }
                            }
                            new_qre_ast_node->repeat_max = value;
                        }
                        else
                        {
                            ret = -QRE_ERR_CORRUPT;
                            goto fail;
                        }
                    }
                    break;

                case '?':
                    qre_token_cur = qre_token_cur->next;
                    new_qre_ast_node->repeat_min = 0;
                    new_qre_ast_node->repeat_max = 1;
                    break;

                case '*':
                    qre_token_cur = qre_token_cur->next;
                    new_qre_ast_node->repeat_min = 0;
                    new_qre_ast_node->repeat_max = QRE_AST_NODE_REPEAT_INF;
                    break;

                case '+':
                    qre_token_cur = qre_token_cur->next;
                    new_qre_ast_node->repeat_min = 1;
                    new_qre_ast_node->repeat_max = QRE_AST_NODE_REPEAT_INF;
                    break;
                default:
                    break;
            }
        }
    }

    *qre_token_cur_io = qre_token_cur;

    goto done;
fail:
done:
    return ret;
}

static int qre_parse_seq(qre_ast_node_t **qre_ast_node_out, qre_token_t **qre_token_cur_io, uint32_t options)
{
    int ret = 0;
    qre_token_t *qre_token_cur = *qre_token_cur_io;
    qre_ast_node_t *new_qre_ast_node = NULL;
    qre_ast_node_seq_t *new_qre_ast_node_seq = NULL;

    qre_ast_node_capture_num_t *new_qre_ast_node_capture_num = NULL;
    qre_ast_node_capture_t *new_qre_ast_node_capture = NULL;
    qre_ast_node_branch_t *qre_ast_node_branch = NULL;

    int toplevel = options & (1 << QRE_PARSER_OPT_TOPLEVEL);
    int branch_idx = 1;

    /* Clear Top Level */
    options &= ~(1 << QRE_PARSER_OPT_TOPLEVEL);

    new_qre_ast_node_seq = qre_ast_node_seq_new();
    if (new_qre_ast_node_seq == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail;
    }

    while (qre_token_cur != NULL)
    {
        /* Str Literal */
        switch (qre_token_cur->type)
        {
            case QRE_TOKEN_CHAR:
                switch (qre_token_cur->value)
                {
                    case '.':
                        ret = qre_parse_any(&new_qre_ast_node, &qre_token_cur, options);
                        if (ret != 0) goto fail;
                        break;
                    default:
                        ret = qre_parse_plain(&new_qre_ast_node, &qre_token_cur, options);
                        if (ret != 0) goto fail;
                        break;
                }
                break;
            case QRE_TOKEN_CONTROL:
                switch (qre_token_cur->value)
                {
                    case '[':
                        ret = qre_parse_group(&new_qre_ast_node, &qre_token_cur, options);
                        if (ret != 0) goto fail;
                        break;
                    case '(':
                        ret = qre_parse_branch(&new_qre_ast_node, &qre_token_cur, options);
                        if (ret != 0) goto fail;

                        /* Capture Num */
                        if (toplevel != 0)
                        {
                            /* Create new capture */
                            new_qre_ast_node_capture_num = qre_ast_node_capture_num_new(branch_idx);
                            if (new_qre_ast_node_capture_num == NULL) { goto fail; }
                            new_qre_ast_node_capture = qre_ast_node_capture_new( \
                                    QRE_AST_NODE_CAPTURE_TYPE_NUM, \
                                    new_qre_ast_node_capture_num);
                            if (new_qre_ast_node_capture == NULL) { goto fail; }
                            new_qre_ast_node_capture_num = NULL;

                            qre_ast_node_branch = new_qre_ast_node->ptr;
                            qre_ast_node_capture_list_append(qre_ast_node_branch->capture_list, \
                                        new_qre_ast_node_capture);
                            new_qre_ast_node_capture = NULL;

                            branch_idx += 1;
                        }

                        break;
                    case '|':
                        /* Skip '|' */
                        goto finish;
                        break;
                    case ')':
                        /* Skip ')' */
                        goto finish;
                        break;
                    default:
                        ret = -QRE_ERR_INTERNAL;
                        goto fail;
                        break;
                }
                break;
            case QRE_TOKEN_SPECIAL:
                ret = qre_parse_special(&new_qre_ast_node, &qre_token_cur, options);
                if (ret != 0) goto fail;
                break;
            default:
                ret = -QRE_ERR_INTERNAL;
                goto fail;
        }

        /* Repeat */
        if (qre_token_cur != NULL)
        {
            switch (qre_token_cur->type)
            {
                case QRE_TOKEN_CONTROL:
                    switch (qre_token_cur->value)
                    {
                        case '{':
                        case '?':
                        case '*':
                        case '+':
                            ret = qre_parse_repeat(new_qre_ast_node, &qre_token_cur);
                            if (ret != 0) { goto fail; }
                            break;
                    }
                    break;
            }
        }
        /* Non-greddy */
        if (qre_token_cur != NULL)
        {
            switch (qre_token_cur->type)
            {
                case QRE_TOKEN_CONTROL:
                    switch (qre_token_cur->value)
                    {
                        case '?':
                            qre_token_cur = qre_token_cur->next;
                            new_qre_ast_node->greedy = FALSE;
                            break;
                    }
            }
        }

        ret = qre_ast_node_seq_append_with_node(new_qre_ast_node_seq, new_qre_ast_node);
        if (ret != 0) goto fail;
        new_qre_ast_node = NULL;
    }
finish:

    new_qre_ast_node = qre_ast_node_new(QRE_AST_NODE_TYPE_SEQ, new_qre_ast_node_seq);
    if (new_qre_ast_node == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail;
    }
    new_qre_ast_node_seq = NULL;
    *qre_ast_node_out = new_qre_ast_node; new_qre_ast_node = NULL;

    *qre_token_cur_io = qre_token_cur;

    goto done;
fail:
    if (new_qre_ast_node_seq != NULL) qre_ast_node_seq_destroy(new_qre_ast_node_seq);
    if (new_qre_ast_node != NULL) qre_ast_node_destroy(new_qre_ast_node);
done:
    if (new_qre_ast_node_capture_num != NULL) qre_ast_node_capture_num_destroy(new_qre_ast_node_capture_num);
    if (new_qre_ast_node_capture != NULL) qre_ast_node_capture_destroy(new_qre_ast_node_capture);
    return ret;
}

int qre_parse(qre_ast_t **qre_ast_out, qre_token_list_t *qre_token_list)
{
    int ret = 0;
    qre_token_t *qre_token_cur = qre_token_list->begin;
    qre_ast_t *new_qre_ast = NULL;
    qre_ast_node_t *new_qre_ast_node = NULL;
    uint32_t options = 0;

    /* Top Level */
    options |= (1 << QRE_PARSER_OPT_TOPLEVEL);

    ret = qre_parse_seq(&new_qre_ast_node, &qre_token_cur, options);
    if (ret != 0) goto fail;

    new_qre_ast = qre_ast_new(new_qre_ast_node);
    if (new_qre_ast == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail;
    }
    new_qre_ast_node = NULL;
    *qre_ast_out = new_qre_ast;

    goto done;
fail:
    if (new_qre_ast_node != NULL) qre_ast_node_destroy(new_qre_ast_node);
    if (new_qre_ast != NULL) qre_ast_destroy(new_qre_ast);
done:
    return ret;
}


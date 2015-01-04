/*
   Quick Regular Expression Engine : Convert from AST to NFA
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

#include "qre_common.h"
#include "qre_err.h"
#include "qre_token.h"
#include "qre_ast.h"
#include "qre_nfa.h"

/* Declarations */
static int qre_convert_to_nfa_node(qre_nfa_state_table_t *qre_nfa_state_table, \
        qre_nfa_state_t **qre_nfa_state_out, \
        qre_nfa_state_t **qre_nfa_state_in, \
        qre_ast_node_t *qre_ast_node);


/* Seq */
static int qre_convert_to_nfa_node_seq(qre_nfa_state_table_t *qre_nfa_state_table, \
        qre_nfa_state_t **qre_nfa_state_out, \
        qre_nfa_state_t **qre_nfa_state_in, \
        qre_ast_node_t *qre_ast_node)
{
    int ret = 0;
    qre_ast_node_seq_t *qre_ast_node_seq = qre_ast_node->ptr;
    qre_ast_node_seq_node_t *qre_ast_node_seq_node_cur; 
    qre_nfa_state_t *new_qre_nfa_state_out = NULL;
    qre_nfa_state_t *new_qre_nfa_state_in = NULL;
    qre_nfa_state_t *qre_nfa_state_prev = NULL;
    qre_nfa_state_t *new_qre_nfa_state = NULL;
    int first = 1;

    if (qre_ast_node_seq->begin == NULL)
    {
        new_qre_nfa_state = qre_nfa_state_new();
        if (new_qre_nfa_state == NULL) 
        { 
            ret = -QRE_ERR_OUT_OF_MEMORY;
            goto fail; 
        }
        new_qre_nfa_state->state = qre_nfa_state_table_state_num_alloc(qre_nfa_state_table);
        qre_nfa_state_table_append(qre_nfa_state_table, new_qre_nfa_state);
        *qre_nfa_state_out = *qre_nfa_state_in = new_qre_nfa_state;
    }
    else
    {
        qre_ast_node_seq_node_cur = qre_ast_node_seq->begin;
        while (qre_ast_node_seq_node_cur != NULL)
        {
            ret = qre_convert_to_nfa_node(qre_nfa_state_table, \
                    &new_qre_nfa_state_out, \
                    &new_qre_nfa_state_in, \
                    qre_ast_node_seq_node_cur->node);
            if (ret != 0) { goto fail; }

            /* Connect previous to current */
            if (first != 0)
            {
                *qre_nfa_state_in = new_qre_nfa_state_in;
                first = 0;
            }
            else
            {
                ret = qre_nfa_state_list_append_with_configure(qre_nfa_state_prev->next_states, new_qre_nfa_state_in);
                if (ret != 0) goto fail; 
            }
            qre_nfa_state_prev = new_qre_nfa_state_out;

            qre_ast_node_seq_node_cur = qre_ast_node_seq_node_cur->next; 
        }
        *qre_nfa_state_out = new_qre_nfa_state_out;
    }

    goto done;
fail:
done:
    return ret;
}

/* Plain */
static int qre_convert_to_nfa_node_plain(qre_nfa_state_table_t *qre_nfa_state_table, \
        qre_nfa_state_t **qre_nfa_state_out, \
        qre_nfa_state_t **qre_nfa_state_in, \
        qre_ast_node_t *qre_ast_node)
{
    int ret = 0;
    qre_ast_node_plain_t *qre_ast_node_plain = qre_ast_node->ptr;
    qre_nfa_condition_char_t *new_qre_nfa_condition_char = NULL;
    qre_nfa_condition_t *new_qre_nfa_condition = NULL;
    qre_nfa_state_t *new_qre_nfa_state = NULL;

    new_qre_nfa_state = qre_nfa_state_new();
    if (new_qre_nfa_state == NULL) 
    { 
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail; 
    }

    new_qre_nfa_state->state = qre_nfa_state_table_state_num_alloc(qre_nfa_state_table);
    new_qre_nfa_condition_char = qre_nfa_condition_char_new_with_value(qre_ast_node_plain->token->value);
    if (new_qre_nfa_condition_char == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail; 
    }

    new_qre_nfa_condition = qre_nfa_condition_new(QRE_NFA_CONDITION_CHAR, new_qre_nfa_condition_char);
    if (new_qre_nfa_condition == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail; 
    }
    new_qre_nfa_condition_char = NULL;

    qre_nfa_condition_list_append(new_qre_nfa_state->conditions, new_qre_nfa_condition);
    new_qre_nfa_condition = NULL;

    qre_nfa_state_table_append(qre_nfa_state_table, new_qre_nfa_state);

    *qre_nfa_state_out = new_qre_nfa_state;
    *qre_nfa_state_in = new_qre_nfa_state;

    goto done;
fail:
    if (new_qre_nfa_state != NULL) { qre_nfa_state_destroy(new_qre_nfa_state); }
    if (new_qre_nfa_condition != NULL) { qre_nfa_condition_destroy(new_qre_nfa_condition); }
    if (new_qre_nfa_condition_char != NULL) { qre_nfa_condition_char_destroy(new_qre_nfa_condition_char); }
done:
    return ret;
}

/* Any */
static int qre_convert_to_nfa_node_any(qre_nfa_state_table_t *qre_nfa_state_table, \
        qre_nfa_state_t **qre_nfa_state_out, \
        qre_nfa_state_t **qre_nfa_state_in, \
        qre_ast_node_t *qre_ast_node)
{
    int ret = 0;
    qre_ast_node_any_t *qre_ast_node_any = qre_ast_node->ptr;
    qre_nfa_condition_any_t *new_qre_nfa_condition_any = NULL;
    qre_nfa_condition_t *new_qre_nfa_condition = NULL;
    qre_nfa_state_t *new_qre_nfa_state = NULL;
    uint32_t state_num;

    (void)qre_ast_node_any;

    new_qre_nfa_state = qre_nfa_state_new();
    if (new_qre_nfa_state == NULL) 
    { 
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail; 
    }

    state_num = qre_nfa_state_table_state_num_alloc(qre_nfa_state_table);
    new_qre_nfa_state->state = state_num;
    new_qre_nfa_condition_any = qre_nfa_condition_any_new();
    if (new_qre_nfa_condition_any == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail; 
    }

    new_qre_nfa_condition = qre_nfa_condition_new(QRE_NFA_CONDITION_ANY, new_qre_nfa_condition_any);
    if (new_qre_nfa_condition == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail; 
    }
    new_qre_nfa_condition_any = NULL;

    qre_nfa_condition_list_append(new_qre_nfa_state->conditions, new_qre_nfa_condition);
    new_qre_nfa_condition = NULL;

    qre_nfa_state_table_append(qre_nfa_state_table, new_qre_nfa_state);

    *qre_nfa_state_out = new_qre_nfa_state;
    *qre_nfa_state_in = new_qre_nfa_state;

    goto done;
fail:
    if (new_qre_nfa_state != NULL) { qre_nfa_state_destroy(new_qre_nfa_state); }
    if (new_qre_nfa_condition != NULL) { qre_nfa_condition_destroy(new_qre_nfa_condition); }
    if (new_qre_nfa_condition_any != NULL) { qre_nfa_condition_any_destroy(new_qre_nfa_condition_any); }
done:
    return ret;
}

/* Group */
static int qre_convert_to_nfa_node_group(qre_nfa_state_table_t *qre_nfa_state_table, \
        qre_nfa_state_t **qre_nfa_state_out, \
        qre_nfa_state_t **qre_nfa_state_in, \
        qre_ast_node_t *qre_ast_node)
{
    int ret = 0;
    qre_ast_node_group_t *qre_ast_node_group = qre_ast_node->ptr;
    qre_ast_node_group_option_t *ast_node_group_option_cur;
    qre_ast_node_group_option_solo_t *ast_node_group_option_solo;
    qre_ast_node_group_option_range_t *ast_node_group_option_range;
    qre_nfa_condition_char_t *new_qre_nfa_condition_char = NULL;
    qre_nfa_condition_range_t *new_qre_nfa_condition_range = NULL;
    qre_nfa_condition_t *new_qre_nfa_condition = NULL;
    qre_nfa_state_t *new_qre_nfa_state = NULL;
    uint32_t state_num;

    new_qre_nfa_state = qre_nfa_state_new();
    if (new_qre_nfa_state == NULL) 
    { 
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail; 
    }

    state_num = qre_nfa_state_table_state_num_alloc(qre_nfa_state_table);
    new_qre_nfa_state->state = state_num;

    if (qre_ast_node_group->elliminate != FALSE)
    {
        new_qre_nfa_state->conditions->eliminate = TRUE;
    }

    ast_node_group_option_cur = qre_ast_node_group->options->begin; 
    while (ast_node_group_option_cur != NULL)
    {
        switch (ast_node_group_option_cur->type)
        {
            case QRE_AST_NODE_GROUP_OPTION_TYPE_SOLO:
                ast_node_group_option_solo = ast_node_group_option_cur->ptr;
                new_qre_nfa_condition_char = qre_nfa_condition_char_new_with_value(ast_node_group_option_solo->token->value);
                if (new_qre_nfa_condition_char == NULL)
                {
                    ret = -QRE_ERR_OUT_OF_MEMORY;
                    goto fail; 
                }
                new_qre_nfa_condition = qre_nfa_condition_new(QRE_NFA_CONDITION_CHAR, new_qre_nfa_condition_char);
                if (new_qre_nfa_condition == NULL)
                {
                    ret = -QRE_ERR_OUT_OF_MEMORY;
                    goto fail; 
                }
                new_qre_nfa_condition_char = NULL;
                break;
            case QRE_AST_NODE_GROUP_OPTION_TYPE_RANGE:
                ast_node_group_option_range = ast_node_group_option_cur->ptr;
                new_qre_nfa_condition_range = qre_nfa_condition_range_new_with_value( \
                        ast_node_group_option_range->token_from->value, 
                        ast_node_group_option_range->token_to->value);
                if (new_qre_nfa_condition_range == NULL)
                {
                    ret = -QRE_ERR_OUT_OF_MEMORY;
                    goto fail; 
                }
                new_qre_nfa_condition = qre_nfa_condition_new(QRE_NFA_CONDITION_RANGE, new_qre_nfa_condition_range);
                if (new_qre_nfa_condition == NULL)
                {
                    ret = -QRE_ERR_OUT_OF_MEMORY;
                    goto fail; 
                }
                new_qre_nfa_condition_char = NULL;
                break;
        }

        qre_nfa_condition_list_append(new_qre_nfa_state->conditions, new_qre_nfa_condition);
        new_qre_nfa_condition = NULL;

        ast_node_group_option_cur = ast_node_group_option_cur->next; 
    }

    qre_nfa_state_table_append(qre_nfa_state_table, new_qre_nfa_state);

    *qre_nfa_state_out = new_qre_nfa_state;
    *qre_nfa_state_in = new_qre_nfa_state;

    goto done;
fail:
    if (new_qre_nfa_state != NULL) { qre_nfa_state_destroy(new_qre_nfa_state); }
    if (new_qre_nfa_condition != NULL) { qre_nfa_condition_destroy(new_qre_nfa_condition); }
    if (new_qre_nfa_condition_char != NULL) { qre_nfa_condition_char_destroy(new_qre_nfa_condition_char); }
    if (new_qre_nfa_condition_range != NULL) { qre_nfa_condition_range_destroy(new_qre_nfa_condition_range); }
done:
    return ret;
}

/* Branch */
static int qre_convert_to_nfa_node_branch(qre_nfa_state_table_t *qre_nfa_state_table, \
        qre_nfa_state_t **qre_nfa_state_out, \
        qre_nfa_state_t **qre_nfa_state_in, \
        qre_ast_node_t *qre_ast_node)
{
    int ret = 0;
    qre_ast_node_branch_t *qre_ast_node_branch = qre_ast_node->ptr;
    qre_ast_node_branch_sub_t *qre_ast_node_branch_sub_cur;
    qre_ast_node_capture_t *qre_ast_node_capture_cur;
    qre_ast_node_capture_num_t *qre_ast_node_capture_num;
    qre_ast_node_capture_label_t *qre_ast_node_capture_label;

    qre_nfa_state_t *new_qre_nfa_state_head = NULL;
    qre_nfa_state_t *new_qre_nfa_state_tail = NULL;
    qre_nfa_state_t *new_qre_nfa_state_out = NULL;
    qre_nfa_state_t *new_qre_nfa_state_in = NULL;

    qre_nfa_attr_t *new_qre_nfa_attr = NULL; 
    qre_nfa_attr_capture_num_t *new_qre_nfa_attr_capture_num = NULL; 
    qre_nfa_attr_capture_label_t *new_qre_nfa_attr_capture_label = NULL; 

    new_qre_nfa_state_head = qre_nfa_state_new();
    if (new_qre_nfa_state_head == NULL) 
    { 
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail; 
    }
    new_qre_nfa_state_head->state = qre_nfa_state_table_state_num_alloc(qre_nfa_state_table);

    new_qre_nfa_state_tail = qre_nfa_state_new();
    if (new_qre_nfa_state_tail == NULL) 
    { 
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail; 
    }

    /* Branches */
    qre_ast_node_branch_sub_cur = qre_ast_node_branch->branches->begin;
    while (qre_ast_node_branch_sub_cur != NULL)
    {
        ret = qre_convert_to_nfa_node_seq(qre_nfa_state_table, \
                &new_qre_nfa_state_out, \
                &new_qre_nfa_state_in, \
                qre_ast_node_branch_sub_cur->node);
        if (ret != 0) { goto fail; }

        ret = qre_nfa_state_list_append_with_configure(new_qre_nfa_state_head->next_states, \
                new_qre_nfa_state_in);
        if (ret != 0) { goto fail; }
        ret = qre_nfa_state_list_append_with_configure(new_qre_nfa_state_out->next_states, \
                new_qre_nfa_state_tail);
        if (ret != 0) { goto fail; }

        qre_ast_node_branch_sub_cur = qre_ast_node_branch_sub_cur->next; 
    }

    /* Attributes */
    qre_ast_node_capture_cur = qre_ast_node_branch->capture_list->begin;
    while (qre_ast_node_capture_cur != NULL)
    {
        switch (qre_ast_node_capture_cur->type)
        {
            case QRE_AST_NODE_CAPTURE_TYPE_NONE:
                break;
            case QRE_AST_NODE_CAPTURE_TYPE_NUM:
                qre_ast_node_capture_num = qre_ast_node_capture_cur->ptr;
                /* Capture End */
                new_qre_nfa_attr_capture_num = qre_nfa_attr_capture_num_new(qre_ast_node_capture_num->num);
                if (new_qre_nfa_attr_capture_num == NULL) { ret = -QRE_ERR_OUT_OF_MEMORY; goto fail; }
                new_qre_nfa_attr = qre_nfa_attr_new(QRE_NFA_ATTR_CAPTURE_NUM_END, new_qre_nfa_attr_capture_num);
                if (new_qre_nfa_attr == NULL) { ret = -QRE_ERR_OUT_OF_MEMORY; goto fail; }
                new_qre_nfa_attr_capture_num = NULL;
                qre_nfa_attr_list_append(new_qre_nfa_state_tail->attrs, new_qre_nfa_attr);
                new_qre_nfa_attr = NULL;
                /* Capture Begin */ 
                new_qre_nfa_attr_capture_num = qre_nfa_attr_capture_num_new(qre_ast_node_capture_num->num);
                if (new_qre_nfa_attr_capture_num == NULL) { ret = -QRE_ERR_OUT_OF_MEMORY; goto fail; }
                new_qre_nfa_attr = qre_nfa_attr_new(QRE_NFA_ATTR_CAPTURE_NUM_BEGIN, new_qre_nfa_attr_capture_num);
                if (new_qre_nfa_attr == NULL) { ret = -QRE_ERR_OUT_OF_MEMORY; goto fail; }
                new_qre_nfa_attr_capture_num = NULL;
                qre_nfa_attr_list_append(new_qre_nfa_state_head->attrs, new_qre_nfa_attr);
                new_qre_nfa_attr = NULL;
                break;
            case QRE_AST_NODE_CAPTURE_TYPE_LABEL:
                qre_ast_node_capture_label = qre_ast_node_capture_cur->ptr;
                /* Capture End */
                new_qre_nfa_attr_capture_label = qre_nfa_attr_capture_label_new( \
                        qre_ast_node_capture_label->label, qre_ast_node_capture_label->label_len);
                if (new_qre_nfa_attr_capture_label == NULL) { ret = -QRE_ERR_OUT_OF_MEMORY; goto fail; }
                new_qre_nfa_attr = qre_nfa_attr_new(QRE_NFA_ATTR_CAPTURE_LBL_END, new_qre_nfa_attr_capture_label);
                if (new_qre_nfa_attr == NULL) { ret = -QRE_ERR_OUT_OF_MEMORY; goto fail; }
                new_qre_nfa_attr_capture_label = NULL;
                qre_nfa_attr_list_append(new_qre_nfa_state_tail->attrs, new_qre_nfa_attr);
                new_qre_nfa_attr = NULL;
                /* Capture Begin */
                new_qre_nfa_attr_capture_label = qre_nfa_attr_capture_label_new( \
                        qre_ast_node_capture_label->label, qre_ast_node_capture_label->label_len);
                if (new_qre_nfa_attr_capture_label == NULL) { ret = -QRE_ERR_OUT_OF_MEMORY; goto fail; }
                new_qre_nfa_attr = qre_nfa_attr_new(QRE_NFA_ATTR_CAPTURE_LBL_BEGIN, new_qre_nfa_attr_capture_label);
                if (new_qre_nfa_attr == NULL) { ret = -QRE_ERR_OUT_OF_MEMORY; goto fail; }
                new_qre_nfa_attr_capture_label = NULL;
                qre_nfa_attr_list_append(new_qre_nfa_state_head->attrs, new_qre_nfa_attr);
                new_qre_nfa_attr = NULL;
                break;
        }
        qre_ast_node_capture_cur = qre_ast_node_capture_cur->next;
    }

    /*if (capture != FALSE)*/
    /*{*/
    /*new_qre_nfa_attr = qre_nfa_attr_new(QRE_NFA_ATTR_CAPTURE_BEGIN, NULL);*/
    /*if (new_qre_nfa_attr == NULL)*/
    /*{*/
    /*ret = -QRE_ERR_OUT_OF_MEMORY;*/
    /*goto fail; */
    /*}*/
    /*qre_nfa_attr_list_append(new_qre_nfa_state_head->attrs, new_qre_nfa_attr);*/
    /*new_qre_nfa_attr = NULL;*/
    /*}*/

    qre_nfa_state_table_append(qre_nfa_state_table, new_qre_nfa_state_head);

    new_qre_nfa_state_tail->state = qre_nfa_state_table_state_num_alloc(qre_nfa_state_table);
    qre_nfa_state_table_append(qre_nfa_state_table, new_qre_nfa_state_tail);

    *qre_nfa_state_in = new_qre_nfa_state_head;
    *qre_nfa_state_out = new_qre_nfa_state_tail;

    goto done;
fail:
    if (new_qre_nfa_state_head != NULL) { qre_nfa_state_destroy(new_qre_nfa_state_head); }
    if (new_qre_nfa_state_tail != NULL) { qre_nfa_state_destroy(new_qre_nfa_state_tail); }
    if (new_qre_nfa_attr != NULL) { qre_nfa_attr_destroy(new_qre_nfa_attr); }
    if (new_qre_nfa_attr_capture_num != NULL) { qre_nfa_attr_capture_num_destroy(new_qre_nfa_attr_capture_num); }
    if (new_qre_nfa_attr_capture_label != NULL) { qre_nfa_attr_capture_label_destroy(new_qre_nfa_attr_capture_label); }
done:
    return ret;
}

/* Special */
static int qre_convert_to_nfa_node_special(qre_nfa_state_table_t *qre_nfa_state_table, \
        qre_nfa_state_t **qre_nfa_state_out, \
        qre_nfa_state_t **qre_nfa_state_in, \
        qre_ast_node_t *qre_ast_node)
{
    int ret = 0;
    qre_ast_node_special_t *qre_ast_node_special = qre_ast_node->ptr;
    qre_nfa_condition_special_t *new_qre_nfa_condition_special = NULL;
    qre_nfa_condition_t *new_qre_nfa_condition = NULL;
    qre_nfa_state_t *new_qre_nfa_state = NULL;
    int value;

    new_qre_nfa_state = qre_nfa_state_new();
    if (new_qre_nfa_state == NULL) 
    { 
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail; 
    }

    switch (qre_ast_node_special->value)
    {
        case QRE_AST_NODE_SPECIAL_DIGIT:
            value = QRE_NFA_CONDITION_SPECIAL_DIGIT;
            break;
        case QRE_AST_NODE_SPECIAL_NONDIGIT:
            value = QRE_NFA_CONDITION_SPECIAL_NONDIGIT;
            break;
        case QRE_AST_NODE_SPECIAL_FORM_FEED:
            value = QRE_NFA_CONDITION_SPECIAL_FORM_FEED;
            break;
        case QRE_AST_NODE_SPECIAL_NEWLINE:
            value = QRE_NFA_CONDITION_SPECIAL_NEWLINE;
            break;
        case QRE_AST_NODE_SPECIAL_CARRIAGE:
            value = QRE_NFA_CONDITION_SPECIAL_CARRIAGE;
            break;
        case QRE_AST_NODE_SPECIAL_WHITESPACE:
            value = QRE_NFA_CONDITION_SPECIAL_WHITESPACE;
            break;
        case QRE_AST_NODE_SPECIAL_NONWHITESPACE:
            value = QRE_NFA_CONDITION_SPECIAL_NONWHITESPACE;
            break;
        case QRE_AST_NODE_SPECIAL_TAB:
            value = QRE_NFA_CONDITION_SPECIAL_TAB;
            break;
        case QRE_AST_NODE_SPECIAL_VERTICAL_TAB:
            value = QRE_NFA_CONDITION_SPECIAL_VERTICAL_TAB;
            break;
        case QRE_AST_NODE_SPECIAL_WORD:
            value = QRE_NFA_CONDITION_SPECIAL_WORD;
            break;
        case QRE_AST_NODE_SPECIAL_NONWORD:
            value = QRE_NFA_CONDITION_SPECIAL_NONWORD;
            break;
        default:
            ret = -QRE_ERR_INTERNAL;
            goto fail;
    }

    new_qre_nfa_state->state = qre_nfa_state_table_state_num_alloc(qre_nfa_state_table);
    new_qre_nfa_condition_special = qre_nfa_condition_special_new_with_value(value);
    if (new_qre_nfa_condition_special == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail; 
    }

    new_qre_nfa_condition = qre_nfa_condition_new(QRE_NFA_CONDITION_SPECIAL, new_qre_nfa_condition_special);
    if (new_qre_nfa_condition == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail; 
    }
    new_qre_nfa_condition_special = NULL;

    qre_nfa_condition_list_append(new_qre_nfa_state->conditions, new_qre_nfa_condition);
    new_qre_nfa_condition = NULL;

    qre_nfa_state_table_append(qre_nfa_state_table, new_qre_nfa_state);

    *qre_nfa_state_out = new_qre_nfa_state;
    *qre_nfa_state_in = new_qre_nfa_state;

    goto done;
fail:
    if (new_qre_nfa_state != NULL) { qre_nfa_state_destroy(new_qre_nfa_state); }
    if (new_qre_nfa_condition != NULL) { qre_nfa_condition_destroy(new_qre_nfa_condition); }
    if (new_qre_nfa_condition_special != NULL) { qre_nfa_condition_special_destroy(new_qre_nfa_condition_special); }
done:
    return ret;
}

static int qre_convert_to_nfa_node(qre_nfa_state_table_t *qre_nfa_state_table, \
        qre_nfa_state_t **qre_nfa_state_out, \
        qre_nfa_state_t **qre_nfa_state_in, \
        qre_ast_node_t *qre_ast_node)
{
    int ret = 0;
    qre_nfa_state_t *new_qre_nfa_state = NULL;

    switch (qre_ast_node->type)
    {
        case QRE_AST_NODE_TYPE_SEQ:
            ret = qre_convert_to_nfa_node_seq(qre_nfa_state_table, \
                    qre_nfa_state_out, qre_nfa_state_in, \
                    qre_ast_node);
            break;
        case QRE_AST_NODE_TYPE_PLAIN:
            ret = qre_convert_to_nfa_node_plain(qre_nfa_state_table, \
                    qre_nfa_state_out, qre_nfa_state_in, \
                    qre_ast_node);
            break;
        case QRE_AST_NODE_TYPE_ANY:
            ret = qre_convert_to_nfa_node_any(qre_nfa_state_table, \
                    qre_nfa_state_out, qre_nfa_state_in, \
                    qre_ast_node);
            break;
        case QRE_AST_NODE_TYPE_GROUP:
            ret = qre_convert_to_nfa_node_group(qre_nfa_state_table, \
                    qre_nfa_state_out, qre_nfa_state_in, \
                    qre_ast_node);
            break;
        case QRE_AST_NODE_TYPE_BRANCH:
            ret = qre_convert_to_nfa_node_branch(qre_nfa_state_table, \
                    qre_nfa_state_out, qre_nfa_state_in, \
                    qre_ast_node);
            break;
        case QRE_AST_NODE_TYPE_SPECIAL:
            ret = qre_convert_to_nfa_node_special(qre_nfa_state_table, \
                    qre_nfa_state_out, qre_nfa_state_in, \
                    qre_ast_node);
            break;
        default:
            ret = -QRE_ERR_INTERNAL;
            goto fail;
    }
    if (ret != 0) goto fail;

    if (!((qre_ast_node->repeat_min == 1) && (qre_ast_node->repeat_max == 1)))
    {
        (*qre_nfa_state_out)->repeat_to = (*qre_nfa_state_in);
        (*qre_nfa_state_in)->repeat_from = (*qre_nfa_state_out);
        (*qre_nfa_state_out)->repeat_min = qre_ast_node->repeat_min;
        (*qre_nfa_state_out)->repeat_max = qre_ast_node->repeat_max;
    }

    if (qre_ast_node->greedy == FALSE) (*qre_nfa_state_out)->greedy = FALSE;

    goto done;
fail:
    if (new_qre_nfa_state != NULL) qre_nfa_state_destroy(new_qre_nfa_state);
done:
    return ret;
}

int qre_convert_to_nfa(qre_nfa_state_table_t **qre_nfa_state_table_out, qre_ast_t *qre_ast)
{
    int ret = 0;
    qre_nfa_state_table_t *new_qre_nfa_state_table = NULL;
    qre_nfa_state_t *new_qre_nfa_state_out = NULL;
    qre_nfa_state_t *new_qre_nfa_state_in = NULL;

    new_qre_nfa_state_table = qre_nfa_state_table_new();
    if (new_qre_nfa_state_table == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail;
    }

    /* Compile from the root node */
    ret = qre_convert_to_nfa_node(new_qre_nfa_state_table, \
            &new_qre_nfa_state_out, \
            &new_qre_nfa_state_in, \
            qre_ast->root);
    if (ret != 0) { goto fail; }

    /* Set entrance and exit point */
    new_qre_nfa_state_table->state_num_start = new_qre_nfa_state_in->state;
    new_qre_nfa_state_table->state_num_end = new_qre_nfa_state_out->state;

    *qre_nfa_state_table_out = new_qre_nfa_state_table;

    goto done;
fail:
    if (new_qre_nfa_state_table != NULL) { qre_nfa_state_table_destroy(new_qre_nfa_state_table); }
done:
    return ret;
}


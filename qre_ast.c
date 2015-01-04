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

#include <string.h>
#include <stdlib.h>

#include "qre_common.h"
#include "qre_ast.h"

/* Capture */
qre_ast_node_capture_num_t *qre_ast_node_capture_num_new(uint32_t num)
{
    qre_ast_node_capture_num_t *new_qre_ast_node_capture_num = NULL;

    new_qre_ast_node_capture_num = (qre_ast_node_capture_num_t *)malloc(sizeof(qre_ast_node_capture_num_t));
    if (new_qre_ast_node_capture_num == NULL) { goto fail; }
    new_qre_ast_node_capture_num->num = num;

    goto done;
fail:
    if (new_qre_ast_node_capture_num != NULL)
    {
        free(new_qre_ast_node_capture_num);
        new_qre_ast_node_capture_num = NULL;
    }
done:
    return new_qre_ast_node_capture_num;
}

int qre_ast_node_capture_num_destroy(qre_ast_node_capture_num_t *qre_ast_node_capture_num)
{
    free(qre_ast_node_capture_num);

    return 0;
}

qre_ast_node_capture_label_t *qre_ast_node_capture_label_new(qre_char_t *label, size_t label_len)
{
    qre_ast_node_capture_label_t *new_qre_ast_node_capture_label = NULL;

    new_qre_ast_node_capture_label = (qre_ast_node_capture_label_t *)malloc(sizeof(qre_ast_node_capture_label_t));
    if (new_qre_ast_node_capture_label == NULL) { goto fail; }
    new_qre_ast_node_capture_label->label = NULL;
    new_qre_ast_node_capture_label->label = (qre_char_t *)malloc(sizeof(qre_char_t) * (label_len));
    if (new_qre_ast_node_capture_label->label == NULL) { goto fail; }
    qre_char_memcpy(new_qre_ast_node_capture_label->label, label, label_len);
    new_qre_ast_node_capture_label->label_len = label_len;

    goto done;
fail:
    if (new_qre_ast_node_capture_label != NULL)
    {
        if (new_qre_ast_node_capture_label->label == NULL)
        {
            free(new_qre_ast_node_capture_label->label);
        }
        free(new_qre_ast_node_capture_label);
        new_qre_ast_node_capture_label = NULL;
    }
done:
    return new_qre_ast_node_capture_label;
}

int qre_ast_node_capture_label_destroy(qre_ast_node_capture_label_t *qre_ast_node_capture_label)
{
    if (qre_ast_node_capture_label->label != NULL)
    {
        free(qre_ast_node_capture_label->label);
    }
    free(qre_ast_node_capture_label);

    return 0;
}

qre_ast_node_capture_t *qre_ast_node_capture_new(int type, void *ptr)
{
    qre_ast_node_capture_t *new_qre_ast_node_capture = NULL;

    new_qre_ast_node_capture = (qre_ast_node_capture_t *)malloc(sizeof(qre_ast_node_capture_t));
    if (new_qre_ast_node_capture == NULL) { goto fail; }
    new_qre_ast_node_capture->type = type;
    new_qre_ast_node_capture->ptr = ptr;
    new_qre_ast_node_capture->next = NULL;

    goto done;
fail:
    if (new_qre_ast_node_capture != NULL)
    {
        free(new_qre_ast_node_capture);
        new_qre_ast_node_capture = NULL;
    }
done:
    return new_qre_ast_node_capture;
}

int qre_ast_node_capture_destroy(qre_ast_node_capture_t *qre_ast_node_capture)
{
    switch (qre_ast_node_capture->type)
    {
        case QRE_AST_NODE_CAPTURE_TYPE_NONE:
            break;
        case QRE_AST_NODE_CAPTURE_TYPE_NUM:
            qre_ast_node_capture_num_destroy(qre_ast_node_capture->ptr);
            break;
        case QRE_AST_NODE_CAPTURE_TYPE_LABEL:
            qre_ast_node_capture_label_destroy(qre_ast_node_capture->ptr);
            break;
    }
    free(qre_ast_node_capture);

    return 0;
}

qre_ast_node_capture_list_t *qre_ast_node_capture_list_new(void)
{
    qre_ast_node_capture_list_t *new_qre_ast_node_capture_list = NULL;

    new_qre_ast_node_capture_list = (qre_ast_node_capture_list_t *)malloc(sizeof(qre_ast_node_capture_list_t));
    if (new_qre_ast_node_capture_list == NULL) { goto fail; }
    new_qre_ast_node_capture_list->begin = NULL;
    new_qre_ast_node_capture_list->end = NULL;

    goto done;
fail:
    if (new_qre_ast_node_capture_list != NULL)
    {
        free(new_qre_ast_node_capture_list);
        new_qre_ast_node_capture_list = NULL;
    }
done:
    return new_qre_ast_node_capture_list; 
}

int qre_ast_node_capture_list_destroy(qre_ast_node_capture_list_t *qre_ast_node_capture_list)
{
    qre_ast_node_capture_t *qre_ast_node_capture_cur, *qre_ast_node_capture_next;

    qre_ast_node_capture_cur = qre_ast_node_capture_list->begin;
    while (qre_ast_node_capture_cur != NULL)
    {
        qre_ast_node_capture_next = qre_ast_node_capture_cur->next;
        qre_ast_node_capture_destroy(qre_ast_node_capture_cur);
        qre_ast_node_capture_cur = qre_ast_node_capture_next;
    }
    free(qre_ast_node_capture_list);

    return 0;
}

int qre_ast_node_capture_list_append(qre_ast_node_capture_list_t *qre_ast_node_capture_list, \
        qre_ast_node_capture_t *new_qre_ast_node_capture)
{
    if (qre_ast_node_capture_list->begin == NULL)
    {
        qre_ast_node_capture_list->begin = qre_ast_node_capture_list->end = new_qre_ast_node_capture;
    }
    else
    {
        qre_ast_node_capture_list->end->next = new_qre_ast_node_capture;
        qre_ast_node_capture_list->end = new_qre_ast_node_capture;
    }
    
    return 0;
}


/* AST Generic Node */
struct qre_ast_node *qre_ast_node_new(int type, void *ptr)
{
    struct qre_ast_node *new_ast_node = NULL;

    if ((new_ast_node = (struct qre_ast_node *)malloc(sizeof(struct qre_ast_node))) == NULL)
    { goto fail; }
    new_ast_node->type = type;
    new_ast_node->ptr = ptr;
    new_ast_node->repeat_min = 1;
    new_ast_node->repeat_max = 1;
    new_ast_node->greedy = TRUE;

    goto done;
fail:
    if (new_ast_node != NULL)
    {
        free(new_ast_node);
        new_ast_node = NULL;
    }
done:
    return new_ast_node;
}

int qre_ast_node_destroy(struct qre_ast_node *node)
{
    switch (node->type)
    {
        case QRE_AST_NODE_TYPE_SEQ:
            qre_ast_node_seq_destroy(node->ptr);
            break;
        case QRE_AST_NODE_TYPE_PLAIN:
            qre_ast_node_plain_destroy(node->ptr);
            break;
        case QRE_AST_NODE_TYPE_ANY:
            qre_ast_node_any_destroy(node->ptr);
            break;
        case QRE_AST_NODE_TYPE_GROUP:
            qre_ast_node_group_destroy(node->ptr);
            break;
        case QRE_AST_NODE_TYPE_BRANCH:
            qre_ast_node_branch_destroy(node->ptr);
            break;
        case QRE_AST_NODE_TYPE_SPECIAL:
            qre_ast_node_special_destroy(node->ptr);
            break;
    }
    free(node);

    return 0;
}


/* Specialized AST Node */

/* AST Node : Seq */
qre_ast_node_seq_node_t *qre_ast_node_seq_node_new(qre_ast_node_t *qre_ast_node)
{
    qre_ast_node_seq_node_t *new_qre_ast_node_seq_node = NULL;

    new_qre_ast_node_seq_node = (qre_ast_node_seq_node_t *)malloc(sizeof(qre_ast_node_seq_node_t));
    if (new_qre_ast_node_seq_node == NULL) goto fail;
    new_qre_ast_node_seq_node->node = qre_ast_node;
    new_qre_ast_node_seq_node->next = NULL;

    if (qre_ast_node == NULL)
    {
        printf("DEBUG\n");
    }

    goto done;
fail:
    if (new_qre_ast_node_seq_node != NULL)
    {
        free(new_qre_ast_node_seq_node);
        new_qre_ast_node_seq_node = NULL;
    }
done:
    return new_qre_ast_node_seq_node;
}

int qre_ast_node_seq_node_destroy(qre_ast_node_seq_node_t *qre_ast_node_seq_node)
{
    if (qre_ast_node_seq_node->node != NULL)
    {
        qre_ast_node_destroy(qre_ast_node_seq_node->node);
    }
    free(qre_ast_node_seq_node);

    return 0;
}

qre_ast_node_seq_t *qre_ast_node_seq_new(void)
{
    qre_ast_node_seq_t *new_qre_ast_node_seq = NULL;

    new_qre_ast_node_seq = (qre_ast_node_seq_t *)malloc(sizeof(qre_ast_node_seq_t));
    if (new_qre_ast_node_seq == NULL) goto fail;

    new_qre_ast_node_seq->begin = new_qre_ast_node_seq->end = NULL;
    new_qre_ast_node_seq->size = 0;

    goto done;
fail:
    if (new_qre_ast_node_seq != NULL)
    {
        free(new_qre_ast_node_seq);
        new_qre_ast_node_seq = NULL;
    }
done:
    return new_qre_ast_node_seq;
}


int qre_ast_node_seq_destroy(qre_ast_node_seq_t *qre_ast_node_seq)
{
    qre_ast_node_seq_node_t *qre_ast_node_seq_node_cur, *qre_ast_node_seq_node_next;

    qre_ast_node_seq_node_cur = qre_ast_node_seq->begin;
    while (qre_ast_node_seq_node_cur != NULL)
    {
        qre_ast_node_seq_node_next = qre_ast_node_seq_node_cur->next; 
        qre_ast_node_seq_node_destroy(qre_ast_node_seq_node_cur);
        qre_ast_node_seq_node_cur = qre_ast_node_seq_node_next; 
    }
    free(qre_ast_node_seq);

    return 0;
}

int qre_ast_node_seq_append(qre_ast_node_seq_t *qre_ast_node_seq, \
        qre_ast_node_seq_node_t *new_qre_ast_node_seq_node)
{
    if (qre_ast_node_seq->begin == NULL)
    {
        qre_ast_node_seq->begin = qre_ast_node_seq->end = new_qre_ast_node_seq_node;
    }
    else
    {
        qre_ast_node_seq->end->next = new_qre_ast_node_seq_node;
        qre_ast_node_seq->end = new_qre_ast_node_seq_node;
    }
    qre_ast_node_seq->size += 1;

    return 0;
}

int qre_ast_node_seq_append_with_node(qre_ast_node_seq_t *qre_ast_node_seq, \
        qre_ast_node_t *new_qre_ast_node)
{
    int ret = 0;
    qre_ast_node_seq_node_t *new_qre_ast_node_seq_node = NULL;

    new_qre_ast_node_seq_node = qre_ast_node_seq_node_new(new_qre_ast_node);
    if (new_qre_ast_node_seq_node == NULL)
    {
        ret = -QRE_AST_NODE_TYPE_SEQ;
        goto fail;
    }
    qre_ast_node_seq_append(qre_ast_node_seq, new_qre_ast_node_seq_node);
    new_qre_ast_node_seq_node = NULL;

    goto done;
fail:
    if (new_qre_ast_node_seq_node != NULL)
    {
        qre_ast_node_seq_node_destroy(new_qre_ast_node_seq_node);
    }
done:
    return ret;
}

/* AST Node : Plain */
qre_ast_node_plain_t *qre_ast_node_plain_new(qre_token_t *token)
{
    qre_ast_node_plain_t *new_qre_ast_node_plain = NULL;

    new_qre_ast_node_plain = (qre_ast_node_plain_t *)malloc(sizeof(qre_ast_node_plain_t)); 
    if (new_qre_ast_node_plain == NULL) { goto fail; }
    new_qre_ast_node_plain->token = qre_token_clone(token);

    goto done;
fail:
    if (new_qre_ast_node_plain != NULL)
    {
        free(new_qre_ast_node_plain);
        new_qre_ast_node_plain = NULL;
    }
done:
    return new_qre_ast_node_plain;
}

int qre_ast_node_plain_destroy(qre_ast_node_plain_t *qre_ast_node_plain)
{
    if (qre_ast_node_plain->token != NULL)
    {
        qre_token_destroy(qre_ast_node_plain->token);
    }
    free(qre_ast_node_plain);

    return 0;
}

/* AST Node : Any */

qre_ast_node_any_t *qre_ast_node_any_new(qre_token_t *token)
{
    qre_ast_node_any_t *new_qre_ast_node_any = NULL;

    new_qre_ast_node_any = (qre_ast_node_any_t *)malloc(sizeof(qre_ast_node_any_t)); 
    if (new_qre_ast_node_any == NULL) { goto fail; }
    new_qre_ast_node_any->token = qre_token_clone(token);

    goto done;
fail:
    if (new_qre_ast_node_any != NULL)
    {
        free(new_qre_ast_node_any);
        new_qre_ast_node_any = NULL;
    }
done:
    return new_qre_ast_node_any;
}

int qre_ast_node_any_destroy(qre_ast_node_any_t *qre_ast_node_any)
{
    if (qre_ast_node_any->token != NULL)
    {
        qre_token_destroy(qre_ast_node_any->token);
    }
    free(qre_ast_node_any);

    return 0;
}


/* AST Node : Group */
qre_ast_node_group_option_solo_t *qre_ast_node_group_option_solo_new(qre_token_t *token)
{
    qre_ast_node_group_option_solo_t *new_qre_ast_node_group_option_solo = NULL;

    new_qre_ast_node_group_option_solo = (qre_ast_node_group_option_solo_t *)malloc(sizeof(qre_ast_node_group_option_solo_t)); 
    if (new_qre_ast_node_group_option_solo == NULL) { goto fail; }
    new_qre_ast_node_group_option_solo->token = qre_token_clone(token);

    goto done;
fail:
    if (new_qre_ast_node_group_option_solo != NULL)
    {
        free(new_qre_ast_node_group_option_solo);
        new_qre_ast_node_group_option_solo = NULL;
    }
done:
    return new_qre_ast_node_group_option_solo;
}

int qre_ast_node_group_option_solo_destroy(qre_ast_node_group_option_solo_t *qre_ast_node_group_option_solo)
{
    if (qre_ast_node_group_option_solo->token != NULL)
    {
        qre_token_destroy(qre_ast_node_group_option_solo->token);
    }
    free(qre_ast_node_group_option_solo);

    return 0;
}

qre_ast_node_group_option_range_t *qre_ast_node_group_option_range_new(qre_token_t *token_from, qre_token_t *token_to)
{
    qre_ast_node_group_option_range_t *new_qre_ast_node_group_option_range = NULL;

    new_qre_ast_node_group_option_range = (qre_ast_node_group_option_range_t *)malloc(sizeof(qre_ast_node_group_option_range_t)); 
    if (new_qre_ast_node_group_option_range == NULL) { goto fail; }
    new_qre_ast_node_group_option_range->token_from = NULL;
    new_qre_ast_node_group_option_range->token_to = NULL;
    new_qre_ast_node_group_option_range->token_from = qre_token_clone(token_from);
    new_qre_ast_node_group_option_range->token_to = qre_token_clone(token_to);

    goto done;
fail:
    if (new_qre_ast_node_group_option_range != NULL)
    {
        qre_ast_node_group_option_range_destroy(new_qre_ast_node_group_option_range);
        new_qre_ast_node_group_option_range = NULL;
    }
done:
    return new_qre_ast_node_group_option_range;
}

int qre_ast_node_group_option_range_destroy(qre_ast_node_group_option_range_t *qre_ast_node_group_option_range)
{
    if (qre_ast_node_group_option_range->token_from != NULL)
    { qre_token_destroy(qre_ast_node_group_option_range->token_from); }
    if (qre_ast_node_group_option_range->token_to != NULL)
    { qre_token_destroy(qre_ast_node_group_option_range->token_to); }
    free(qre_ast_node_group_option_range);

    return 0;
}

qre_ast_node_group_option_t *qre_ast_node_group_option_new(int type, void *ptr)
{
    qre_ast_node_group_option_t *new_qre_ast_node_group_option = NULL;

    new_qre_ast_node_group_option = (qre_ast_node_group_option_t *)malloc(sizeof(qre_ast_node_group_option_t)); 
    if (new_qre_ast_node_group_option == NULL) { goto fail; }
    new_qre_ast_node_group_option->type = type;
    new_qre_ast_node_group_option->ptr = ptr;
    new_qre_ast_node_group_option->next = NULL;

    goto done;
fail:
    if (new_qre_ast_node_group_option != NULL)
    {
        free(new_qre_ast_node_group_option);
        new_qre_ast_node_group_option = NULL;
    }
done:
    return new_qre_ast_node_group_option;
}

int qre_ast_node_group_option_destroy(qre_ast_node_group_option_t *qre_ast_node_group_option)
{
    switch (qre_ast_node_group_option->type)
    {
        case QRE_AST_NODE_GROUP_OPTION_TYPE_SOLO:
            qre_ast_node_group_option_solo_destroy(qre_ast_node_group_option->ptr);
            break;
        case QRE_AST_NODE_GROUP_OPTION_TYPE_RANGE:
            qre_ast_node_group_option_range_destroy(qre_ast_node_group_option->ptr);
            break;
    }
    free(qre_ast_node_group_option);

    return 0;
}

qre_ast_node_group_option_list_t *qre_ast_node_group_option_list_new(void)
{
    qre_ast_node_group_option_list_t *new_qre_ast_node_group_option_list = NULL;

    new_qre_ast_node_group_option_list = (qre_ast_node_group_option_list_t *)malloc(sizeof(qre_ast_node_group_option_list_t)); 
    if (new_qre_ast_node_group_option_list == NULL) { goto fail; }
    new_qre_ast_node_group_option_list->begin = NULL;
    new_qre_ast_node_group_option_list->end = NULL;

    goto done;
fail:
    if (new_qre_ast_node_group_option_list != NULL)
    {
        free(new_qre_ast_node_group_option_list);
        new_qre_ast_node_group_option_list = NULL;
    }
done:
    return new_qre_ast_node_group_option_list;
}

int qre_ast_node_group_option_list_destroy(qre_ast_node_group_option_list_t *qre_ast_node_group_option_list)
{
    qre_ast_node_group_option_t *qre_ast_node_group_option_cur, *qre_ast_node_group_option_next;

    qre_ast_node_group_option_cur = qre_ast_node_group_option_list->begin;
    while (qre_ast_node_group_option_cur != NULL)
    {
        qre_ast_node_group_option_next = qre_ast_node_group_option_cur->next;
        qre_ast_node_group_option_destroy(qre_ast_node_group_option_cur);
        qre_ast_node_group_option_cur = qre_ast_node_group_option_next;
    }
    free(qre_ast_node_group_option_list);

    return 0;
}

int qre_ast_node_group_option_list_append(qre_ast_node_group_option_list_t *qre_ast_node_group_option_list, \
        qre_ast_node_group_option_t *new_qre_ast_node_group_option)
{
    if (qre_ast_node_group_option_list->begin == NULL)
    {
        qre_ast_node_group_option_list->begin = qre_ast_node_group_option_list->end = new_qre_ast_node_group_option;
    }
    else
    {
        qre_ast_node_group_option_list->end->next = new_qre_ast_node_group_option;
        qre_ast_node_group_option_list->end = new_qre_ast_node_group_option;
    }

    return 0;
}

qre_ast_node_group_t *qre_ast_node_group_new(void)
{
    qre_ast_node_group_t *new_qre_ast_node_group = NULL;

    new_qre_ast_node_group = (qre_ast_node_group_t *)malloc(sizeof(qre_ast_node_group_t)); 
    if (new_qre_ast_node_group == NULL) { goto fail; }
    new_qre_ast_node_group->elliminate = 0;
    new_qre_ast_node_group->options = qre_ast_node_group_option_list_new();
    if (new_qre_ast_node_group->options == NULL) { goto fail; }

    goto done;
fail:
    if (new_qre_ast_node_group != NULL)
    {
        if (new_qre_ast_node_group->options != NULL)
        {
            qre_ast_node_group_option_list_destroy(new_qre_ast_node_group->options);
        }
        free(new_qre_ast_node_group);
        new_qre_ast_node_group = NULL;
    }
done:
    return new_qre_ast_node_group;
}

int qre_ast_node_group_destroy(qre_ast_node_group_t *qre_ast_node_group)
{
    if (qre_ast_node_group->options != NULL)
    {
        qre_ast_node_group_option_list_destroy(qre_ast_node_group->options);
    }
    free(qre_ast_node_group);

    return 0;
}

/* AST Node : Branch */
qre_ast_node_branch_sub_t *qre_ast_node_branch_sub_new(qre_ast_node_t *qre_ast_node)
{
    qre_ast_node_branch_sub_t *new_qre_ast_node_branch_sub = NULL;

    new_qre_ast_node_branch_sub = (qre_ast_node_branch_sub_t *)malloc(sizeof(qre_ast_node_branch_sub_t));
    if (new_qre_ast_node_branch_sub == NULL) { goto fail; }

    new_qre_ast_node_branch_sub->node = qre_ast_node;
    new_qre_ast_node_branch_sub->next = NULL;

    goto done;
fail:
    if (new_qre_ast_node_branch_sub != NULL)
    {
        free(new_qre_ast_node_branch_sub);
        new_qre_ast_node_branch_sub = NULL;
    }
done:
    return new_qre_ast_node_branch_sub;
}

int qre_ast_node_branch_sub_destroy(qre_ast_node_branch_sub_t *qre_ast_node_branch_sub)
{
    if (qre_ast_node_branch_sub->node != NULL) qre_ast_node_destroy(qre_ast_node_branch_sub->node);
    free(qre_ast_node_branch_sub);

    return 0;
}

qre_ast_node_branch_sub_list_t *qre_ast_node_branch_sub_list_new(void)
{
    qre_ast_node_branch_sub_list_t *new_qre_ast_node_branch_sub_list = NULL;

    new_qre_ast_node_branch_sub_list = (qre_ast_node_branch_sub_list_t *)malloc(sizeof(qre_ast_node_branch_sub_list_t));
    if (new_qre_ast_node_branch_sub_list == NULL) { goto fail; }

    new_qre_ast_node_branch_sub_list->begin = NULL;
    new_qre_ast_node_branch_sub_list->end = NULL;

    goto done;
fail:
    if (new_qre_ast_node_branch_sub_list != NULL)
    {
        free(new_qre_ast_node_branch_sub_list);
        new_qre_ast_node_branch_sub_list = NULL;
    }
done:
    return new_qre_ast_node_branch_sub_list;
}

int qre_ast_node_branch_sub_list_destroy(qre_ast_node_branch_sub_list_t *qre_ast_node_branch_sub_list)
{
    qre_ast_node_branch_sub_t *qre_ast_node_branch_sub_cur, *qre_ast_node_branch_sub_next;

    qre_ast_node_branch_sub_cur = qre_ast_node_branch_sub_list->begin;
    while (qre_ast_node_branch_sub_cur != NULL)
    {
        qre_ast_node_branch_sub_next = qre_ast_node_branch_sub_cur->next;
        qre_ast_node_branch_sub_destroy(qre_ast_node_branch_sub_cur);
        qre_ast_node_branch_sub_cur = qre_ast_node_branch_sub_next;
    }
    free(qre_ast_node_branch_sub_list);

    return 0;
}

int qre_ast_node_branch_sub_list_append(qre_ast_node_branch_sub_list_t *qre_ast_node_branch_sub_list, \
        qre_ast_node_branch_sub_t *new_qre_ast_node_branch_sub)
{
    if (qre_ast_node_branch_sub_list->begin == NULL)
    {
        qre_ast_node_branch_sub_list->begin = qre_ast_node_branch_sub_list->end = new_qre_ast_node_branch_sub;
    }
    else
    {
        qre_ast_node_branch_sub_list->end->next = new_qre_ast_node_branch_sub;
        qre_ast_node_branch_sub_list->end = new_qre_ast_node_branch_sub;
    }

    return 0;
}

qre_ast_node_branch_t *qre_ast_node_branch_new(void)
{
    qre_ast_node_branch_t *new_qre_ast_node_branch = NULL;

    new_qre_ast_node_branch = (qre_ast_node_branch_t *)malloc(sizeof(qre_ast_node_branch_t));
    if (new_qre_ast_node_branch == NULL) { goto fail; }
    new_qre_ast_node_branch->branches = NULL;
    new_qre_ast_node_branch->capture_list = NULL;

    new_qre_ast_node_branch->branches = qre_ast_node_branch_sub_list_new();
    if (new_qre_ast_node_branch->branches == NULL) { goto fail; }

    new_qre_ast_node_branch->capture_list = qre_ast_node_capture_list_new();
    if (new_qre_ast_node_branch->capture_list == NULL) { goto fail; }

    goto done;
fail:
    if (new_qre_ast_node_branch != NULL)
    {
        if (new_qre_ast_node_branch->branches != NULL) qre_ast_node_branch_sub_list_destroy(new_qre_ast_node_branch->branches);
        if (new_qre_ast_node_branch->capture_list != NULL) qre_ast_node_capture_list_destroy(new_qre_ast_node_branch->capture_list);
        free(new_qre_ast_node_branch);
        new_qre_ast_node_branch = NULL;
    }
done:
    return new_qre_ast_node_branch;
}

int qre_ast_node_branch_destroy(qre_ast_node_branch_t *qre_ast_node_branch)
{
    if (qre_ast_node_branch->branches != NULL) qre_ast_node_branch_sub_list_destroy(qre_ast_node_branch->branches);
    if (qre_ast_node_branch->capture_list != NULL) qre_ast_node_capture_list_destroy(qre_ast_node_branch->capture_list);
    free(qre_ast_node_branch);

    return 0;
}

/* AST Node : Special */
qre_ast_node_special_t *qre_ast_node_special_new(int value)
{
    qre_ast_node_special_t *new_qre_ast_node_special = NULL;

    new_qre_ast_node_special = (qre_ast_node_special_t *)malloc(sizeof(qre_ast_node_special_t)); 
    if (new_qre_ast_node_special == NULL) { goto fail; }
    new_qre_ast_node_special->value = value;

    goto done;
fail:
    if (new_qre_ast_node_special != NULL)
    {
        free(new_qre_ast_node_special);
        new_qre_ast_node_special = NULL;
    }
done:
    return new_qre_ast_node_special;
}

int qre_ast_node_special_destroy(qre_ast_node_special_t *qre_ast_node_special)
{
    free(qre_ast_node_special);

    return 0;
}


/* AST */
struct qre_ast *qre_ast_new(struct qre_ast_node *root)
{
    struct qre_ast *new_ast = NULL;

    if ((new_ast = (struct qre_ast *)malloc(sizeof(struct qre_ast))) == NULL)
    { goto fail; }
    new_ast->root = root;

    goto done;
fail:
    if (new_ast != NULL)
    {
        free(new_ast);
        new_ast = NULL;
    }
done:
    return new_ast;
}

int qre_ast_destroy(struct qre_ast *ast)
{
    qre_ast_node_destroy(ast->root);
    free(ast);

    return 0;
}


/*
   Quick Regular Expression Engine : Compiler : From NFA to IL
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

#include <stdlib.h>

#include "qre_common.h"
#include "qre_err.h"
#include "qre_nfa.h"
#include "qre_il.h"
#include "qre_vm_op.h"


/* IL Block */
struct qre_il_block
{
    int state;
    uint32_t pc_start;
    uint32_t pc_end;
    uint32_t pc_reptest;
    qre_il_text_item_t *qre_il_text_item_reptest;
    uint32_t pc_repjmp;
    qre_il_text_item_t *qre_il_text_item_repjmp;
    qre_nfa_state_t *qre_nfa_state_from;
    qre_nfa_state_t *qre_nfa_state_to;

    struct qre_il_block *next;
};
typedef struct qre_il_block qre_il_block_t;

struct qre_il_block_list
{
    struct qre_il_block *begin;
    struct qre_il_block *end;
};
typedef struct qre_il_block_list qre_il_block_list_t;

static struct qre_il_block *qre_il_block_new(int state, \
        uint32_t pc_start, uint32_t pc_end, \
        uint32_t pc_reptest, \
        qre_il_text_item_t *qre_il_text_item_reptest, \
        uint32_t pc_repjmp, \
        qre_il_text_item_t *qre_il_text_item_repjmp, \
        qre_nfa_state_t *qre_nfa_state_from, \
        qre_nfa_state_t *qre_nfa_state_to)
{
    struct qre_il_block *new_qre_il_block = NULL;

    new_qre_il_block = (struct qre_il_block *)malloc(sizeof(struct qre_il_block));
    if (new_qre_il_block == NULL) { goto fail; }
    new_qre_il_block->state = state;
    new_qre_il_block->pc_start = pc_start;
    new_qre_il_block->pc_end = pc_end;
    new_qre_il_block->pc_reptest = pc_reptest;
    new_qre_il_block->qre_il_text_item_reptest = qre_il_text_item_reptest;
    new_qre_il_block->pc_repjmp = pc_repjmp;
    new_qre_il_block->qre_il_text_item_repjmp = qre_il_text_item_repjmp;
    new_qre_il_block->qre_nfa_state_from = qre_nfa_state_from;
    new_qre_il_block->qre_nfa_state_to = qre_nfa_state_to;
    new_qre_il_block->next = NULL;

    goto done;
fail:
    if (new_qre_il_block != NULL)
    {
        free(new_qre_il_block);
        new_qre_il_block = NULL;
    }
done:
    return new_qre_il_block;
}

static int qre_il_block_destroy(struct qre_il_block *qre_il_block)
{
    free(qre_il_block);

    return 0;
}

static struct qre_il_block_list *qre_il_block_list_new(void)
{
    struct qre_il_block_list *new_qre_il_block_list = NULL;

    new_qre_il_block_list = (struct qre_il_block_list *)malloc(sizeof(struct qre_il_block_list));
    if (new_qre_il_block_list == NULL) { goto fail; }
    new_qre_il_block_list->begin = NULL;
    new_qre_il_block_list->end = NULL;

    goto done;
fail:
done:
    return new_qre_il_block_list;
}

static int qre_il_block_list_destroy(struct qre_il_block_list *qre_il_block_list)
{
    struct qre_il_block *qre_il_block_cur, *qre_il_block_next;

    qre_il_block_cur = qre_il_block_list->begin;
    while (qre_il_block_cur != NULL)
    {
        qre_il_block_next = qre_il_block_cur->next;
        qre_il_block_destroy( qre_il_block_cur);

        qre_il_block_cur = qre_il_block_next;
    }
    free(qre_il_block_list);

    return 0;
}

static int qre_il_block_list_append(struct qre_il_block_list *qre_il_block_list, qre_il_block_t *new_qre_il_block)
{
    if (qre_il_block_list->begin == NULL)
    {
        qre_il_block_list->begin = qre_il_block_list->end = new_qre_il_block;
    }
    else
    {
        qre_il_block_list->end->next = new_qre_il_block;
        qre_il_block_list->end = new_qre_il_block;
    }

    return 0;
}

/* PC Pack */
enum
{
    QRE_IL_PC_PACK_TYPE_BLOCK_IDX = 0,
    QRE_IL_PC_PACK_TYPE_RELATIVE_UP = 1,
    QRE_IL_PC_PACK_TYPE_RELATIVE_DOWN = 2
};

struct qre_il_pc_pack_item
{
    int type;
    uint32_t pc;

    struct qre_il_pc_pack_item *next;
};
typedef struct qre_il_pc_pack_item qre_il_pc_pack_item_t;

struct qre_il_pc_pack
{
    struct qre_il_pc_pack_item *begin;
    struct qre_il_pc_pack_item *end;
};
typedef struct qre_il_pc_pack qre_il_pc_pack_t;

static struct qre_il_pc_pack_item *qre_il_pc_pack_item_new(int type, uint32_t pc)
{
    struct qre_il_pc_pack_item *new_qre_il_pc_pack_item = NULL;

    new_qre_il_pc_pack_item = (struct qre_il_pc_pack_item *)malloc(sizeof(struct qre_il_pc_pack_item));
    if (new_qre_il_pc_pack_item == NULL) { goto fail; }
    new_qre_il_pc_pack_item->type = type;
    new_qre_il_pc_pack_item->pc = pc;
    new_qre_il_pc_pack_item->next = NULL;

    goto done;
fail:
    if (new_qre_il_pc_pack_item != NULL)
    {
        free(new_qre_il_pc_pack_item);
        new_qre_il_pc_pack_item = NULL;
    }
done:
    return new_qre_il_pc_pack_item;
}

static int qre_il_pc_pack_item_destroy(struct qre_il_pc_pack_item *qre_il_pc_pack_item)
{
    free(qre_il_pc_pack_item);

    return 0;
}

static struct qre_il_pc_pack *qre_il_pc_pack_new(void)
{
    struct qre_il_pc_pack *new_qre_il_pc_pack = NULL;

    new_qre_il_pc_pack = (struct qre_il_pc_pack *)malloc(sizeof(struct qre_il_pc_pack));
    if (new_qre_il_pc_pack == NULL) { goto fail; }
    new_qre_il_pc_pack->begin = NULL;
    new_qre_il_pc_pack->end = NULL;

    goto done;
fail:
done:
    return new_qre_il_pc_pack;
}

static int qre_il_pc_pack_destroy(struct qre_il_pc_pack *qre_il_pc_pack)
{
    struct qre_il_pc_pack_item *qre_il_pc_pack_item_cur, *qre_il_pc_pack_item_next;

    qre_il_pc_pack_item_cur = qre_il_pc_pack->begin;
    while (qre_il_pc_pack_item_cur != NULL)
    {
        qre_il_pc_pack_item_next = qre_il_pc_pack_item_cur->next;
        qre_il_pc_pack_item_destroy( qre_il_pc_pack_item_cur);

        qre_il_pc_pack_item_cur = qre_il_pc_pack_item_next;
    }
    free(qre_il_pc_pack);

    return 0;
}

static int qre_il_pc_pack_append( \
        struct qre_il_pc_pack *qre_il_pc_pack, \
        qre_il_pc_pack_item_t *new_qre_il_pc_pack_item)
{
    if (qre_il_pc_pack->begin == NULL)
    {
        qre_il_pc_pack->begin = qre_il_pc_pack->end = new_qre_il_pc_pack_item;
    }
    else
    {
        qre_il_pc_pack->end->next = new_qre_il_pc_pack_item;
        qre_il_pc_pack->end = new_qre_il_pc_pack_item;
    }

    return 0;
}

static int qre_il_pc_pack_append_with_configure( \
        struct qre_il_pc_pack *qre_il_pc_pack, \
        int type, uint32_t pc)
{
    int ret = 0;
    qre_il_pc_pack_item_t *new_qre_il_pc_pack_item = NULL;

    new_qre_il_pc_pack_item = qre_il_pc_pack_item_new(type, pc);
    if (new_qre_il_pc_pack_item == NULL) 
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail;
    }
    qre_il_pc_pack_append(qre_il_pc_pack, new_qre_il_pc_pack_item);

    goto done;
fail:
    if (new_qre_il_pc_pack_item != NULL)
    {
        free(new_qre_il_pc_pack_item);
    }
done:
    return ret;
}


struct qre_il_context
{
    qre_il_block_list_t *blocks;
    qre_il_pc_pack_t *pc_pack;
    uint32_t state_num_start;
    uint32_t state_num_end;
    uint32_t pc_entrance;
};
typedef struct qre_il_context qre_il_context_t; 

static int qre_il_context_destroy(struct qre_il_context *qre_il_context);
static struct qre_il_context *qre_il_context_new(void)
{
    struct qre_il_context *new_qre_il_context = NULL;

    new_qre_il_context = (struct qre_il_context *)malloc(sizeof(struct qre_il_context));
    if (new_qre_il_context == NULL) { goto fail; }
    new_qre_il_context->blocks = NULL;
    new_qre_il_context->pc_pack = NULL;
    new_qre_il_context->state_num_start = 0;
    new_qre_il_context->state_num_end = 0;
    new_qre_il_context->pc_entrance = 0;

    new_qre_il_context->blocks = qre_il_block_list_new();
    if (new_qre_il_context->blocks == NULL) { goto fail; }

    new_qre_il_context->pc_pack = qre_il_pc_pack_new();
    if (new_qre_il_context->pc_pack == NULL) { goto fail; }

    goto done;
fail:
    if (new_qre_il_context != NULL)
    {
        qre_il_context_destroy(new_qre_il_context);
        new_qre_il_context = NULL;
    }
done:
    return new_qre_il_context;
}

static int qre_il_context_destroy(struct qre_il_context *qre_il_context)
{
    if (qre_il_context->blocks != NULL) { qre_il_block_list_destroy(qre_il_context->blocks); }
    if (qre_il_context->pc_pack != NULL) { qre_il_pc_pack_destroy(qre_il_context->pc_pack); }
    free(qre_il_context);

    return 0;
}


static int qre_compile_from_nfa_to_il_condition_special( \
        qre_il_context_t *qre_il_context, \
        qre_il_t *qre_il, \
        qre_nfa_condition_t *qre_nfa_condition)
{
    int ret = 0;
    qre_nfa_condition_special_t *qre_nfa_condition_special;

    (void)qre_il_context;

    qre_nfa_condition_special = qre_nfa_condition->ptr;
    switch (qre_nfa_condition_special->value)
    {
        case QRE_NFA_CONDITION_SPECIAL_DIGIT:
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_MSD, 0); break;
        case QRE_NFA_CONDITION_SPECIAL_NONDIGIT:
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_MSND, 0); break;
        case QRE_NFA_CONDITION_SPECIAL_FORM_FEED:
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_MSF, 0); break;
        case QRE_NFA_CONDITION_SPECIAL_NEWLINE:
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_MSNL, 0); break;
        case QRE_NFA_CONDITION_SPECIAL_CARRIAGE:
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_MSCR, 0); break;
        case QRE_NFA_CONDITION_SPECIAL_WHITESPACE:
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_MSS, 0); break;
        case QRE_NFA_CONDITION_SPECIAL_NONWHITESPACE:
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_MSNS, 0); break;
        case QRE_NFA_CONDITION_SPECIAL_TAB:
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_MST, 0); break;
        case QRE_NFA_CONDITION_SPECIAL_VERTICAL_TAB:
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_MSV, 0); break;
        case QRE_NFA_CONDITION_SPECIAL_WORD:
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_MSW, 0); break;
        case QRE_NFA_CONDITION_SPECIAL_NONWORD:
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_MSNW, 0); break;
    }
    if (ret != 0) { goto fail; } 

    goto done;
fail:
done:
    return ret;
}

static int qre_compile_from_nfa_to_il_condition_solo( \
        qre_il_context_t *qre_il_context, \
        qre_il_t *qre_il, \
        qre_nfa_condition_t *qre_nfa_condition)
{
    int ret = 0;
    qre_nfa_condition_char_t *qre_nfa_condition_char;
    qre_nfa_condition_range_t *qre_nfa_condition_range;

    (void)qre_il_context;

    switch (qre_nfa_condition->type)
    {
        case QRE_NFA_CONDITION_CHAR:
            qre_nfa_condition_char = qre_nfa_condition->ptr;
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_ME, qre_nfa_condition_char->value);
            if (ret != 0) { goto fail; } 
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_TEST, 0);
            if (ret != 0) { goto fail; } 
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_PASS, 0);
            if (ret != 0) { goto fail; } 
            break;
        case QRE_NFA_CONDITION_ANY:
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_MA, 0);
            if (ret != 0) { goto fail; } 
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_TEST, 0);
            if (ret != 0) { goto fail; } 
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_PASS, 0);
            if (ret != 0) { goto fail; } 
            break;
        case QRE_NFA_CONDITION_RANGE:
            qre_nfa_condition_range = qre_nfa_condition->ptr;
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_MG, qre_nfa_condition_range->value_from);
            if (ret != 0) { goto fail; } 
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_TEST, 0);
            if (ret != 0) { goto fail; } 
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_ML, qre_nfa_condition_range->value_to);
            if (ret != 0) { goto fail; } 
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_TEST, 0);
            if (ret != 0) { goto fail; } 
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_PASS, 0);
            if (ret != 0) { goto fail; } 
            break;
        case QRE_NFA_CONDITION_SPECIAL:
            ret = qre_compile_from_nfa_to_il_condition_special(qre_il_context, qre_il, qre_nfa_condition);
            if (ret != 0) { goto fail; } 
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_TEST, 0);
            if (ret != 0) { goto fail; } 
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_PASS, 0);
            if (ret != 0) { goto fail; } 
            break;
    }

    goto done;
fail:
done:
    return ret;
}

static int qre_compile_from_nfa_to_il_condition( \
        qre_il_context_t *qre_il_context, \
        qre_il_t *qre_il, \
        qre_nfa_condition_t *qre_nfa_condition)
{
    int ret = 0;
    qre_nfa_condition_char_t *qre_nfa_condition_char;
    qre_nfa_condition_range_t *qre_nfa_condition_range;

    qre_il_text_item_t *text_item_jmpf = NULL;
    uint32_t pc_tail;

    (void)qre_il_context;

    switch (qre_nfa_condition->type)
    {
        case QRE_NFA_CONDITION_CHAR:
            qre_nfa_condition_char = qre_nfa_condition->ptr;
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_ME, qre_nfa_condition_char->value);
            if (ret != 0) { goto fail; } 
            break;
        case QRE_NFA_CONDITION_ANY:
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_MA, 0);
            if (ret != 0) { goto fail; } 
            break;
        case QRE_NFA_CONDITION_RANGE:
            qre_nfa_condition_range = qre_nfa_condition->ptr;
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_MG, qre_nfa_condition_range->value_from);
            if (ret != 0) { goto fail; } 

            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_JMPF, 0);
            if (ret != 0) { goto fail; } 
            text_item_jmpf = qre_il->text->end;

            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_ML, qre_nfa_condition_range->value_to);
            if (ret != 0) { goto fail; } 

            pc_tail = qre_il_text_get_pc(qre_il->text);
            text_item_jmpf->operand = pc_tail;

            break;
        case QRE_NFA_CONDITION_SPECIAL:
            ret = qre_compile_from_nfa_to_il_condition_special(qre_il_context, qre_il, qre_nfa_condition);
            if (ret != 0) { goto fail; } 
            break;
    }

    ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_TEST, 0);
    if (ret != 0) { goto fail; } 

    goto done;
fail:
done:
    return ret;
}

static int qre_compile_from_nfa_to_il_state_capture_begin( \
        qre_il_t *qre_il, \
        qre_nfa_state_t *qre_nfa_state)
{
    int ret = 0;
    qre_nfa_attr_t *qre_nfa_attr_cur = NULL;
    qre_nfa_attr_capture_num_t *qre_nfa_attr_capture_num = NULL;
    qre_nfa_attr_capture_label_t *qre_nfa_attr_capture_label = NULL;
    uint32_t id;

    qre_nfa_attr_cur = qre_nfa_state->attrs->begin;
    while (qre_nfa_attr_cur != NULL)
    {
        switch (qre_nfa_attr_cur->type)
        {
            case QRE_NFA_ATTR_CAPTURE_NUM_BEGIN:
                qre_nfa_attr_capture_num = qre_nfa_attr_cur->ptr;
                ret = qre_il_data_assign_int(&id, qre_il->data, qre_nfa_attr_capture_num->num);
                if (ret != 0) { goto fail; } 
                ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_CAPNBEG, id);
                if (ret != 0) { goto fail; } 
                break;
            case QRE_NFA_ATTR_CAPTURE_LBL_BEGIN:
                qre_nfa_attr_capture_label = qre_nfa_attr_cur->ptr;
                ret = qre_il_data_assign_str(&id, qre_il->data, \
                        qre_nfa_attr_capture_label->label, \
                        qre_nfa_attr_capture_label->label_len);
                if (ret != 0) { goto fail; } 
                ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_CAPLBEG, id);
                if (ret != 0) { goto fail; } 
                break;
        }
        qre_nfa_attr_cur = qre_nfa_attr_cur->next;
    }

fail:
    return ret;
}

static int qre_compile_from_nfa_to_il_state( \
        qre_il_context_t *qre_il_context, \
        qre_il_t *qre_il, \
        qre_nfa_state_t *qre_nfa_state)
{
    int ret = 0;
    qre_nfa_condition_t *qre_nfa_condition_cur = NULL;
    uint32_t pc_start = 0, pc_end = 0, pc_jmp = 0;
    uint32_t pc_reptest = 0, pc_repjmp = 0;
    qre_il_block_t *new_qre_il_block = NULL;
    qre_nfa_state_list_item_t *qre_nfa_state_list_item_cur;
    qre_il_text_item_t *qre_il_text_item_reptest = NULL;
    qre_il_text_item_t *qre_il_text_item_repjmp = NULL;
    qre_nfa_state_t *qre_nfa_state_from = NULL;
    qre_nfa_state_t *qre_nfa_state_to = NULL;
    qre_nfa_attr_t *qre_nfa_attr_cur = NULL;
    qre_nfa_attr_capture_num_t *qre_nfa_attr_capture_num = NULL;
    qre_nfa_attr_capture_label_t *qre_nfa_attr_capture_label = NULL;
    uint32_t id = 0;
    qre_il_data_item_int_t *new_qre_il_data_item_int = NULL;
    qre_il_data_item_str_t *new_qre_il_data_item_str = NULL;
    qre_il_data_item_t *new_qre_il_data_item = NULL;

    /* Conditions */
    pc_start = qre_il_text_get_pc(qre_il->text);

    /* Attributes */
    ret = qre_compile_from_nfa_to_il_state_capture_begin(qre_il, qre_nfa_state);
    if (ret != 0) { goto fail; }

    qre_nfa_state_from = qre_nfa_state->repeat_from;
    qre_nfa_state_to = qre_nfa_state->repeat_to;
    /* Repeat Prepare */
    if (qre_nfa_state_from != NULL)
    {
        if (!((qre_nfa_state_from->repeat_min == 1) && (qre_nfa_state_from->repeat_max == 1)))
        {
            if (qre_nfa_state_from->greedy != FALSE)
            {
                ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_REPBP, 0);
                if (ret != 0) { goto fail; } 
            }
            else
            {
                ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_REPBPN, 0);
                if (ret != 0) { goto fail; } 
            }
            if (qre_nfa_state_from->repeat_min != 1)
            {
                ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_MINNUM, qre_nfa_state_from->repeat_min);
                if (ret != 0) { goto fail; } 
            }
            switch (qre_nfa_state_from->repeat_max)
            {
                case -1:
                    ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_MAXINF, 0);
                    if (ret != 0) { goto fail; } 
                    break;
                case 1:
                    break;
                default:
                    ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_MAXNUM, qre_nfa_state_from->repeat_max);
                    if (ret != 0) { goto fail; } 
                    break;
            }
            pc_reptest = qre_il_text_get_pc(qre_il->text);
            ret = qre_il_pc_pack_append_with_configure(qre_il_context->pc_pack, \
                    QRE_IL_PC_PACK_TYPE_BLOCK_IDX, pc_reptest);
            if (ret != 0) { goto fail; } 
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_REPTEST, 0);
            if (ret != 0) { goto fail; } 
            qre_il_text_item_reptest = qre_il->text->end;
        }
    }

    if (qre_nfa_state->conditions->size == 0)
    {
        /* Nothing has to be done */
    }
    else if (qre_nfa_state->conditions->size == 1)
    {
        qre_nfa_condition_cur = qre_nfa_state->conditions->begin;
        ret = qre_compile_from_nfa_to_il_condition_solo(qre_il_context, qre_il, qre_nfa_condition_cur);
        if (ret != 0) { goto fail; } 
    }
    else
    {
        qre_nfa_condition_cur = qre_nfa_state->conditions->begin;
        while (qre_nfa_condition_cur != NULL)
        {
            /* Test a condition */
            ret = qre_compile_from_nfa_to_il_condition(qre_il_context, qre_il, qre_nfa_condition_cur);
            if (ret != 0) { goto fail; } 

            qre_nfa_condition_cur = qre_nfa_condition_cur->next; 
        }

        /* Pass when all tests return true */
        ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_PASS, 0);
        if (ret != 0) { goto fail; } 
    }

    /* Attributes */
    qre_nfa_attr_cur = qre_nfa_state->attrs->begin;
    while (qre_nfa_attr_cur != NULL)
    {
        switch (qre_nfa_attr_cur->type)
        {
            case QRE_NFA_ATTR_CAPTURE_NUM_END:
                qre_nfa_attr_capture_num = qre_nfa_attr_cur->ptr;
                ret = qre_il_data_assign_int(&id, qre_il->data, qre_nfa_attr_capture_num->num);
                if (ret != 0) { goto fail; } 
                ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_CAPNEND, id);
                if (ret != 0) { goto fail; } 
                break;
            case QRE_NFA_ATTR_CAPTURE_LBL_END:
                qre_nfa_attr_capture_label = qre_nfa_attr_cur->ptr;
                ret = qre_il_data_assign_str(&id, qre_il->data, \
                        qre_nfa_attr_capture_label->label, \
                        qre_nfa_attr_capture_label->label_len);
                if (ret != 0) { goto fail; } 
                ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_CAPLEND, id);
                if (ret != 0) { goto fail; } 
                break;
        }

        qre_nfa_attr_cur = qre_nfa_attr_cur->next; 
    }

    pc_end = qre_il_text_get_pc(qre_il->text);

    /* Entrance */
    if (qre_il_context->state_num_start == qre_nfa_state->state)
    {
        qre_il_context->pc_entrance = pc_start;
    }

    /* Repeat Perform */
    if (!((qre_nfa_state->repeat_min == 1) && (qre_nfa_state->repeat_max == 1)))
    {
        pc_repjmp = qre_il_text_get_pc(qre_il->text);
        ret = qre_il_pc_pack_append_with_configure(qre_il_context->pc_pack, \
                QRE_IL_PC_PACK_TYPE_BLOCK_IDX, pc_repjmp);
        if (ret != 0) { goto fail; } 
        ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_REPJMP, 0);
        if (ret != 0) { goto fail; } 
        qre_il_text_item_repjmp = qre_il->text->end;
    }

    /* Record the beginning and end of a block */
    new_qre_il_block = qre_il_block_new(qre_nfa_state->state, \
            pc_start, pc_end, \
            pc_reptest, \
            qre_il_text_item_reptest, \
            pc_repjmp, \
            qre_il_text_item_repjmp, \
            qre_nfa_state_from, \
            qre_nfa_state_to);
    if (new_qre_il_block == NULL) goto fail;
    qre_il_block_list_append(qre_il_context->blocks, new_qre_il_block);
    new_qre_il_block = NULL;

    /* Jump to the next state */
    if (qre_nfa_state->next_states->size == 0)
    {
        /* Nothing to do? */
        if (qre_il_context->state_num_end == qre_nfa_state->state)
        {
            ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_SUCC, 0);
            if (ret != 0) { goto fail; } 
        }
    }
    else if (qre_nfa_state->next_states->size == 1)
    {
        pc_jmp = qre_il_text_get_pc(qre_il->text);
        ret = qre_il_pc_pack_append_with_configure(qre_il_context->pc_pack, \
                QRE_IL_PC_PACK_TYPE_BLOCK_IDX, pc_jmp);
        if (ret != 0) { goto fail; } 

        ret = qre_il_text_append_with_configure(qre_il->text, \
                QRE_OP_JMP, qre_nfa_state->next_states->begin->ptr->state);
        if (ret != 0) { goto fail; } 
    }
    else
    {
        qre_nfa_state_list_item_cur = qre_nfa_state->next_states->begin;
        while (qre_nfa_state_list_item_cur != NULL)
        {
            pc_jmp = qre_il_text_get_pc(qre_il->text);
            ret = qre_il_pc_pack_append_with_configure(qre_il_context->pc_pack, \
                    QRE_IL_PC_PACK_TYPE_BLOCK_IDX, pc_jmp);
            if (ret != 0) { goto fail; } 

            ret = qre_il_text_append_with_configure(qre_il->text, \
                    QRE_OP_CALL, qre_nfa_state_list_item_cur->ptr->state);
            if (ret != 0) { goto fail; } 

            qre_nfa_state_list_item_cur = qre_nfa_state_list_item_cur->next; 
        }

        /* Missed all calls, fail */
        ret = qre_il_text_append_with_configure(qre_il->text, QRE_OP_FAIL, 0);
        if (ret != 0) { goto fail; } 
    }

    goto done;
fail:
    if (new_qre_il_block == NULL)
    {
        qre_il_block_destroy(new_qre_il_block);
    }
done:
    if (new_qre_il_data_item_int != NULL) qre_il_data_item_int_destroy(new_qre_il_data_item_int);
    if (new_qre_il_data_item_str != NULL) qre_il_data_item_str_destroy(new_qre_il_data_item_str);
    if (new_qre_il_data_item != NULL) qre_il_data_item_destroy(new_qre_il_data_item);
    return ret;
}

static qre_il_text_item_t *qre_compile_from_nfa_to_il_lookup_il_text_item( \
        qre_il_t *qre_il, \
        uint32_t pc)
{
    qre_il_text_item_t *qre_il_text_item_cur = qre_il->text->begin;

    while (qre_il_text_item_cur != NULL)
    {
        if (pc == 0) return qre_il_text_item_cur;
        qre_il_text_item_cur = qre_il_text_item_cur->next;
        pc--;
    }

    return NULL;
}

static uint32_t qre_compile_from_nfa_to_il_lookup_pc_by_block_idx( \
        qre_il_context_t *qre_il_context, \
        int state)
{
    qre_il_block_t *qre_il_block_cur = qre_il_context->blocks->begin;

    while (qre_il_block_cur != NULL)
    {
        if (qre_il_block_cur->state == state)
        {
            return qre_il_block_cur->pc_start;
        }
        qre_il_block_cur = qre_il_block_cur->next; 
    }

    return 0;
}

static qre_il_block_t *qre_compile_from_nfa_to_il_lookup_block_by_block_idx( \
        qre_il_context_t *qre_il_context, \
        int state)
{
    qre_il_block_t *qre_il_block_cur = qre_il_context->blocks->begin;

    while (qre_il_block_cur != NULL)
    {
        if (qre_il_block_cur->state == state)
        {
            return qre_il_block_cur;
        }
        qre_il_block_cur = qre_il_block_cur->next; 
    }

    return NULL;
}

static int qre_compile_from_nfa_to_il_fix( \
        qre_il_context_t *qre_il_context, \
        qre_il_t *qre_il)
{
    int ret = 0;
    qre_il_pc_pack_item_t *qre_il_pc_pack_item_cur =  qre_il_context->pc_pack->begin;
    qre_il_text_item_t *qre_il_text_item_target = NULL;
    uint32_t pc;

    while (qre_il_pc_pack_item_cur != NULL)
    {
        switch (qre_il_pc_pack_item_cur->type)
        {
            case QRE_IL_PC_PACK_TYPE_BLOCK_IDX:
                qre_il_text_item_target = qre_compile_from_nfa_to_il_lookup_il_text_item(qre_il, \
                        qre_il_pc_pack_item_cur->pc);
                if (qre_il_text_item_target == NULL)
                {
                    ret = -QRE_ERR_CORRUPT;
                    goto fail;
                }
                pc = qre_compile_from_nfa_to_il_lookup_pc_by_block_idx(qre_il_context, qre_il_text_item_target->operand);
                qre_il_text_item_target->operand = pc;
                break;
        }
        qre_il_pc_pack_item_cur = qre_il_pc_pack_item_cur->next; 
    }

    goto done;
fail:
done:
    return ret;
}

static int qre_compile_from_nfa_to_il_fix_rep( \
        qre_il_context_t *qre_il_context)
{
    int ret = 0;
    qre_il_block_t *qre_il_block_cur;
    qre_il_block_t *qre_il_block_from;
    qre_il_block_t *qre_il_block_to;
    int state_from;
    int state_to;

    qre_il_block_cur = qre_il_context->blocks->begin;
    while (qre_il_block_cur != NULL)
    {
        if (qre_il_block_cur->qre_nfa_state_from != NULL)
        {
            state_from = qre_il_block_cur->qre_nfa_state_from->state;
            qre_il_block_from = qre_compile_from_nfa_to_il_lookup_block_by_block_idx(qre_il_context, state_from);
            qre_il_block_cur->qre_il_text_item_reptest->operand = qre_il_block_from->pc_repjmp + 1;
        }
        qre_il_block_cur = qre_il_block_cur->next; 
    }

    qre_il_block_cur = qre_il_context->blocks->begin;
    while (qre_il_block_cur != NULL)
    {
        if (qre_il_block_cur->qre_nfa_state_to != NULL)
        {
            state_to = qre_il_block_cur->qre_nfa_state_to->state;
            qre_il_block_to = qre_compile_from_nfa_to_il_lookup_block_by_block_idx(qre_il_context, state_to);
            qre_il_block_cur->qre_il_text_item_repjmp->operand = qre_il_block_to->pc_reptest;
        }
        qre_il_block_cur = qre_il_block_cur->next; 
    }

    return ret;
}

int qre_compile_from_nfa_to_il(qre_il_t **qre_il_out, qre_nfa_state_table_t *qre_nfa_state_table)
{
    int ret = 0;
    qre_nfa_state_t *qre_nfa_state_cur; 
    qre_il_t *new_qre_il = NULL;
    qre_il_context_t *new_qre_il_context = NULL;

    new_qre_il_context = qre_il_context_new();
    if (new_qre_il_context == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail;
    }
    new_qre_il_context->state_num_start = qre_nfa_state_table->state_num_start;
    new_qre_il_context->state_num_end = qre_nfa_state_table->state_num_end;

    new_qre_il = qre_il_new(); 
    if (new_qre_il == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail;
    }

    /* Compile branchs */
    qre_nfa_state_cur = qre_nfa_state_table->begin;
    while (qre_nfa_state_cur != NULL)
    {
        ret = qre_compile_from_nfa_to_il_state( \
                new_qre_il_context, \
                new_qre_il, \
                qre_nfa_state_cur);
        if (ret != 0) { goto fail; } 

        qre_nfa_state_cur = qre_nfa_state_cur->next; 
    }

    /* Fix */
    qre_compile_from_nfa_to_il_fix(new_qre_il_context, new_qre_il);

    /* Fix repeat */
    qre_compile_from_nfa_to_il_fix_rep(new_qre_il_context);

    /* Entrance */
    new_qre_il->pc_entrance = new_qre_il_context->pc_entrance;

    *qre_il_out = new_qre_il;

    goto done;
fail:
    if (new_qre_il != NULL)
    {
        qre_il_destroy(new_qre_il);
        new_qre_il = NULL;
    }
done:
    if (new_qre_il_context != NULL) { qre_il_context_destroy(new_qre_il_context); }
    return ret;
}



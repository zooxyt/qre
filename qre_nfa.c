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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qre_common.h"
#include "qre_err.h"
#include "qre_token.h"
#include "qre_nfa.h"


/* Condition Char */

qre_nfa_condition_char_t *qre_nfa_condition_char_new(void)
{
    qre_nfa_condition_char_t *new_qre_nfa_condition_char = NULL;

    new_qre_nfa_condition_char = (qre_nfa_condition_char_t *)malloc(sizeof(qre_nfa_condition_char_t));
    if (new_qre_nfa_condition_char == NULL) { goto fail; }
    new_qre_nfa_condition_char->value = 0;

    goto done;
fail:
    if (new_qre_nfa_condition_char != NULL) 
    {
        free(new_qre_nfa_condition_char);
        new_qre_nfa_condition_char = NULL;
    }
done:
    return new_qre_nfa_condition_char;
}

qre_nfa_condition_char_t *qre_nfa_condition_char_new_with_value(qre_char_t value)
{
    qre_nfa_condition_char_t *new_qre_nfa_condition_char = NULL;

    new_qre_nfa_condition_char = qre_nfa_condition_char_new();
    if (new_qre_nfa_condition_char == NULL) { goto fail; }
    new_qre_nfa_condition_char->value = value;

    goto done;
fail:
    if (new_qre_nfa_condition_char != NULL) 
    {
        free(new_qre_nfa_condition_char);
        new_qre_nfa_condition_char = NULL;
    }
done:
    return new_qre_nfa_condition_char;
}

int qre_nfa_condition_char_destroy(qre_nfa_condition_char_t *qre_nfa_condition_char)
{
    free(qre_nfa_condition_char);

    return 0;
}

int qre_nfa_condition_char_print(qre_nfa_condition_char_t *qre_nfa_condition_char)
{
    printf("solo(");
    qre_char_print_utf_8(qre_nfa_condition_char->value);
    printf(")");

    return 0;
}

/* Condition Any */

qre_nfa_condition_any_t *qre_nfa_condition_any_new(void)
{
    qre_nfa_condition_any_t *new_qre_nfa_condition_any = NULL;

    new_qre_nfa_condition_any = (qre_nfa_condition_any_t *)malloc(sizeof(qre_nfa_condition_any_t));
    if (new_qre_nfa_condition_any == NULL) { goto fail; }
    new_qre_nfa_condition_any->dummy = 0;

    goto done;
fail:
    if (new_qre_nfa_condition_any != NULL) 
    {
        free(new_qre_nfa_condition_any);
        new_qre_nfa_condition_any = NULL;
    }
done:
    return new_qre_nfa_condition_any;
}

int qre_nfa_condition_any_destroy(qre_nfa_condition_any_t *qre_nfa_condition_any)
{
    free(qre_nfa_condition_any);

    return 0;
}

int qre_nfa_condition_any_print(qre_nfa_condition_any_t *qre_nfa_condition_any)
{
    (void)qre_nfa_condition_any;

    printf("any");

    return 0;
}

/* Condition Range */

qre_nfa_condition_range_t *qre_nfa_condition_range_new_with_value(qre_char_t value_from, qre_char_t value_to)
{
    qre_nfa_condition_range_t *new_qre_nfa_condition_range = NULL;

    new_qre_nfa_condition_range = (qre_nfa_condition_range_t *)malloc(sizeof(qre_nfa_condition_range_t));
    if (new_qre_nfa_condition_range == NULL) { goto fail; }
    new_qre_nfa_condition_range->value_from = value_from;
    new_qre_nfa_condition_range->value_to = value_to;

    goto done;
fail:
    if (new_qre_nfa_condition_range != NULL) 
    {
        free(new_qre_nfa_condition_range);
        new_qre_nfa_condition_range = NULL;
    }
done:
    return new_qre_nfa_condition_range;
}

int qre_nfa_condition_range_destroy(qre_nfa_condition_range_t *qre_nfa_condition_range)
{
    free(qre_nfa_condition_range);

    return 0;
}

int qre_nfa_condition_range_print(qre_nfa_condition_range_t *qre_nfa_condition_range)
{
    printf("range(");
    qre_char_print_utf_8(qre_nfa_condition_range->value_from);
    printf("-");
    qre_char_print_utf_8(qre_nfa_condition_range->value_to);
    printf(")");

    return 0;
}

/* Condition None */

qre_nfa_condition_none_t *qre_nfa_condition_none_new(void)
{
    qre_nfa_condition_none_t *new_qre_nfa_condition_none = NULL;

    new_qre_nfa_condition_none = (qre_nfa_condition_none_t *)malloc(sizeof(qre_nfa_condition_none_t));
    if (new_qre_nfa_condition_none == NULL) { goto fail; }
    new_qre_nfa_condition_none->dummy = 0;

    goto done;
fail:
    if (new_qre_nfa_condition_none != NULL) 
    {
        free(new_qre_nfa_condition_none);
        new_qre_nfa_condition_none = NULL;
    }
done:
    return new_qre_nfa_condition_none;
}

int qre_nfa_condition_none_destroy(qre_nfa_condition_none_t *qre_nfa_condition_none)
{
    free(qre_nfa_condition_none);

    return 0;
}

int qre_nfa_condition_none_print(qre_nfa_condition_none_t *qre_nfa_condition_none)
{
    (void)qre_nfa_condition_none;

    printf("none");

    return 0;
}

/* Condition Special */

qre_nfa_condition_special_t *qre_nfa_condition_special_new(void)
{
    qre_nfa_condition_special_t *new_qre_nfa_condition_special = NULL;

    new_qre_nfa_condition_special = (qre_nfa_condition_special_t *)malloc(sizeof(qre_nfa_condition_special_t));
    if (new_qre_nfa_condition_special == NULL) { goto fail; }
    new_qre_nfa_condition_special->value = 0;

    goto done;
fail:
    if (new_qre_nfa_condition_special != NULL) 
    {
        free(new_qre_nfa_condition_special);
        new_qre_nfa_condition_special = NULL;
    }
done:
    return new_qre_nfa_condition_special;
}

qre_nfa_condition_special_t *qre_nfa_condition_special_new_with_value(int value)
{
    qre_nfa_condition_special_t *new_qre_nfa_condition_special = NULL;

    new_qre_nfa_condition_special = qre_nfa_condition_special_new();
    if (new_qre_nfa_condition_special == NULL) { goto fail; }
    new_qre_nfa_condition_special->value = value;

    goto done;
fail:
    if (new_qre_nfa_condition_special != NULL) 
    {
        free(new_qre_nfa_condition_special);
        new_qre_nfa_condition_special = NULL;
    }
done:
    return new_qre_nfa_condition_special;
}

int qre_nfa_condition_special_destroy(qre_nfa_condition_special_t *qre_nfa_condition_special)
{
    free(qre_nfa_condition_special);

    return 0;
}

int qre_nfa_condition_special_print(qre_nfa_condition_special_t *qre_nfa_condition_special)
{
    printf("special(");
    printf("%d", qre_nfa_condition_special->value);
    printf(")");

    return 0;
}

/* Condition */

qre_nfa_condition_t *qre_nfa_condition_new(int type, void *ptr)
{
    qre_nfa_condition_t *new_qre_nfa_condition = NULL;

    new_qre_nfa_condition = (qre_nfa_condition_t *)malloc(sizeof(qre_nfa_condition_t));
    if (new_qre_nfa_condition == NULL) { goto fail; }
    new_qre_nfa_condition->type = type;
    new_qre_nfa_condition->ptr = ptr;
    new_qre_nfa_condition->next = NULL;

    goto done;
fail:
    if (new_qre_nfa_condition != NULL) 
    {
        free(new_qre_nfa_condition);
        new_qre_nfa_condition = NULL;
    }
done:
    return new_qre_nfa_condition;
}

int qre_nfa_condition_destroy(qre_nfa_condition_t *qre_nfa_condition)
{
    switch (qre_nfa_condition->type)
    {
        case QRE_NFA_CONDITION_CHAR:
            qre_nfa_condition_char_destroy(qre_nfa_condition->ptr);
            break;
        case QRE_NFA_CONDITION_ANY:
            qre_nfa_condition_any_destroy(qre_nfa_condition->ptr);
            break;
        case QRE_NFA_CONDITION_RANGE:
            qre_nfa_condition_range_destroy(qre_nfa_condition->ptr);
            break;
    }

    free(qre_nfa_condition);

    return 0;
}

int qre_nfa_condition_print(qre_nfa_condition_t *qre_nfa_condition)
{
    switch (qre_nfa_condition->type)
    {
        case QRE_NFA_CONDITION_CHAR:
            qre_nfa_condition_char_print(qre_nfa_condition->ptr);
            break;
        case QRE_NFA_CONDITION_ANY:
            qre_nfa_condition_any_print(qre_nfa_condition->ptr);
            break;
        case QRE_NFA_CONDITION_RANGE:
            qre_nfa_condition_range_print(qre_nfa_condition->ptr);
            break;
    }

    return 0;
}


/* Condition List */

qre_nfa_condition_list_t *qre_nfa_condition_list_new(void)
{
    qre_nfa_condition_list_t *new_qre_nfa_condition_list = NULL;

    new_qre_nfa_condition_list = (qre_nfa_condition_list_t *)malloc(sizeof(qre_nfa_condition_list_t));
    if (new_qre_nfa_condition_list == NULL) { goto fail; }
    new_qre_nfa_condition_list->begin = NULL;
    new_qre_nfa_condition_list->end = NULL;
    new_qre_nfa_condition_list->eliminate = FALSE;
    new_qre_nfa_condition_list->size = 0;

    goto done;
fail:
    if (new_qre_nfa_condition_list != NULL) 
    {
        free(new_qre_nfa_condition_list);
        new_qre_nfa_condition_list = NULL;
    }
done:
    return new_qre_nfa_condition_list;
}

int qre_nfa_condition_list_destroy(qre_nfa_condition_list_t *qre_nfa_condition_list)
{
    qre_nfa_condition_t *qre_nfa_condition_cur, *qre_nfa_condition_next;

    qre_nfa_condition_cur = qre_nfa_condition_list->begin;
    while (qre_nfa_condition_cur != NULL)
    {
        qre_nfa_condition_next = qre_nfa_condition_cur->next; 
        qre_nfa_condition_destroy(qre_nfa_condition_cur);
        qre_nfa_condition_cur = qre_nfa_condition_next; 
    }
    free(qre_nfa_condition_list);

    return 0;
}

int qre_nfa_condition_list_print(qre_nfa_condition_list_t *qre_nfa_condition_list)
{
    qre_nfa_condition_t *qre_nfa_condition_cur, *qre_nfa_condition_next;
    int first = 1;

    if (qre_nfa_condition_list->eliminate != FALSE)
    { printf("not("); }

    qre_nfa_condition_cur = qre_nfa_condition_list->begin;
    while (qre_nfa_condition_cur != NULL)
    {
        if (first != 0) { first = 0; }
        else { printf(", "); }
        qre_nfa_condition_next = qre_nfa_condition_cur->next; 
        qre_nfa_condition_print(qre_nfa_condition_cur);
        qre_nfa_condition_cur = qre_nfa_condition_next; 
    }

    if (qre_nfa_condition_list->eliminate != FALSE)
    { printf(")"); }

    return 0;
}

int qre_nfa_condition_list_append(qre_nfa_condition_list_t *qre_nfa_condition_list, \
        qre_nfa_condition_t *new_qre_nfa_condition)
{
    if (qre_nfa_condition_list->begin == NULL)
    {
        qre_nfa_condition_list->begin = qre_nfa_condition_list->end = new_qre_nfa_condition;
    }
    else
    {
        qre_nfa_condition_list->end->next = new_qre_nfa_condition;
        qre_nfa_condition_list->end = new_qre_nfa_condition;
    }
    qre_nfa_condition_list->size += 1;

    return 0;
}


/* Attribute */
qre_nfa_attr_capture_num_t *qre_nfa_attr_capture_num_new(uint32_t num)
{
    qre_nfa_attr_capture_num_t *new_qre_nfa_attr_capture_num = NULL;

    new_qre_nfa_attr_capture_num = (qre_nfa_attr_capture_num_t *)malloc(sizeof(qre_nfa_attr_capture_num_t));
    if (new_qre_nfa_attr_capture_num == NULL) { goto fail; }
    new_qre_nfa_attr_capture_num->num = num;

    goto done;
fail:
    if (new_qre_nfa_attr_capture_num != NULL)
    {
        free(new_qre_nfa_attr_capture_num);
        new_qre_nfa_attr_capture_num = NULL;
    }
done:
    return new_qre_nfa_attr_capture_num;
}

int qre_nfa_attr_capture_num_destroy(qre_nfa_attr_capture_num_t *qre_nfa_attr_capture_num)
{
    free(qre_nfa_attr_capture_num);

    return 0;
}

qre_nfa_attr_capture_label_t *qre_nfa_attr_capture_label_new(qre_char_t *label, size_t label_len)
{
    qre_nfa_attr_capture_label_t *new_qre_nfa_attr_capture_label = NULL;

    new_qre_nfa_attr_capture_label = (qre_nfa_attr_capture_label_t *)malloc(sizeof(qre_nfa_attr_capture_label_t));
    if (new_qre_nfa_attr_capture_label == NULL) { goto fail; }
    new_qre_nfa_attr_capture_label->label = NULL;
    new_qre_nfa_attr_capture_label->label = (qre_char_t *)malloc(sizeof(qre_char_t) * (label_len));
    if (new_qre_nfa_attr_capture_label->label == NULL) { goto fail; }
    qre_char_memcpy(new_qre_nfa_attr_capture_label->label, label, label_len);
    new_qre_nfa_attr_capture_label->label_len = label_len;

    goto done;
fail:
    if (new_qre_nfa_attr_capture_label != NULL)
    {
        if (new_qre_nfa_attr_capture_label->label == NULL)
        {
            free(new_qre_nfa_attr_capture_label->label);
        }
        free(new_qre_nfa_attr_capture_label);
        new_qre_nfa_attr_capture_label = NULL;
    }
done:
    return new_qre_nfa_attr_capture_label;
}

int qre_nfa_attr_capture_label_destroy(qre_nfa_attr_capture_label_t *qre_nfa_attr_capture_label)
{
    if (qre_nfa_attr_capture_label->label != NULL)
    {
        free(qre_nfa_attr_capture_label->label);
    }
    free(qre_nfa_attr_capture_label);

    return 0;
}

qre_nfa_attr_t *qre_nfa_attr_new(int type, void *ptr)
{
    qre_nfa_attr_t *new_qre_nfa_attr = NULL;

    new_qre_nfa_attr = (qre_nfa_attr_t *)malloc(sizeof(qre_nfa_attr_t));
    if (new_qre_nfa_attr == NULL) { goto fail; }
    new_qre_nfa_attr->type = type;
    new_qre_nfa_attr->ptr = ptr;
    new_qre_nfa_attr->next = NULL;

    goto done;
fail:
    if (new_qre_nfa_attr != NULL)
    {
        free(new_qre_nfa_attr);
        new_qre_nfa_attr = NULL;
    }
done:
    return new_qre_nfa_attr;
}

int qre_nfa_attr_destroy(qre_nfa_attr_t *qre_nfa_attr)
{
    switch (qre_nfa_attr->type)
    {
        case QRE_NFA_ATTR_CAPTURE_NUM_BEGIN:
        case QRE_NFA_ATTR_CAPTURE_NUM_END:
            qre_nfa_attr_capture_num_destroy(qre_nfa_attr->ptr);
            break;
        case QRE_NFA_ATTR_CAPTURE_LBL_BEGIN:
        case QRE_NFA_ATTR_CAPTURE_LBL_END:
            qre_nfa_attr_capture_label_destroy(qre_nfa_attr->ptr);
            break;
    }
    free(qre_nfa_attr);

    return 0;
}

qre_nfa_attr_list_t *qre_nfa_attr_list_new(void)
{
    qre_nfa_attr_list_t *new_qre_nfa_attr_list = NULL;

    new_qre_nfa_attr_list = (qre_nfa_attr_list_t *)malloc(sizeof(qre_nfa_attr_list_t));
    if (new_qre_nfa_attr_list == NULL) { goto fail; }
    new_qre_nfa_attr_list->begin = new_qre_nfa_attr_list->end = NULL;

    goto done;
fail:
    if (new_qre_nfa_attr_list != NULL) 
    {
        free(new_qre_nfa_attr_list); 
        new_qre_nfa_attr_list = NULL;
    }
done:
    return new_qre_nfa_attr_list;
}

int qre_nfa_attr_list_destroy(qre_nfa_attr_list_t *qre_nfa_attr_list)
{
    qre_nfa_attr_t *qre_nfa_attr_cur, *qre_nfa_attr_next;

    qre_nfa_attr_cur = qre_nfa_attr_list->begin;
    while (qre_nfa_attr_cur != NULL)
    {
        qre_nfa_attr_next = qre_nfa_attr_cur->next;
        qre_nfa_attr_destroy(qre_nfa_attr_cur);
        qre_nfa_attr_cur = qre_nfa_attr_next;
    }
    free(qre_nfa_attr_list);

    return 0;
}

int qre_nfa_attr_list_append(qre_nfa_attr_list_t *qre_nfa_attr_list, \
        qre_nfa_attr_t *new_qre_nfa_attr)
{
    if (qre_nfa_attr_list->begin == NULL)
    {
        qre_nfa_attr_list->begin = qre_nfa_attr_list->end = new_qre_nfa_attr;
    }
    else
    {
        qre_nfa_attr_list->end->next = new_qre_nfa_attr;
        qre_nfa_attr_list->end = new_qre_nfa_attr;
    }

    return 0;
}


/* State */

qre_nfa_state_t *qre_nfa_state_new(void)
{
    qre_nfa_state_t *new_qre_nfa_state = NULL;

    new_qre_nfa_state = (qre_nfa_state_t *)malloc(sizeof(qre_nfa_state_t));
    if (new_qre_nfa_state == NULL) { goto fail; }

    new_qre_nfa_state->state = 0;
    new_qre_nfa_state->conditions = NULL;
    new_qre_nfa_state->next_states = NULL;
    new_qre_nfa_state->attrs = NULL;

    new_qre_nfa_state->repeat_min = 1;
    new_qre_nfa_state->repeat_max = 1;
    new_qre_nfa_state->repeat_from = NULL;
    new_qre_nfa_state->repeat_to = NULL;
    new_qre_nfa_state->greedy = TRUE;

    new_qre_nfa_state->conditions = qre_nfa_condition_list_new();
    if (new_qre_nfa_state->conditions == NULL) { goto fail; }
    new_qre_nfa_state->next_states = qre_nfa_state_list_new();
    if (new_qre_nfa_state->next_states == NULL) { goto fail; }
    new_qre_nfa_state->attrs = qre_nfa_attr_list_new();
    if (new_qre_nfa_state->attrs == NULL) { goto fail; }

    new_qre_nfa_state->next = NULL;

    goto done;
fail:
    if (new_qre_nfa_state != NULL)
    {
        if (new_qre_nfa_state->next_states != NULL) { qre_nfa_state_list_destroy(new_qre_nfa_state->next_states); }
        if (new_qre_nfa_state->conditions != NULL) { qre_nfa_condition_list_destroy(new_qre_nfa_state->conditions); }
        if (new_qre_nfa_state->attrs != NULL) { qre_nfa_attr_list_destroy(new_qre_nfa_state->attrs); }
        free(new_qre_nfa_state);
        new_qre_nfa_state = NULL;
    }
done:
    return new_qre_nfa_state;
}

int qre_nfa_state_destroy(qre_nfa_state_t *qre_nfa_state)
{
    if (qre_nfa_state->next_states != NULL)  qre_nfa_state_list_destroy(qre_nfa_state->next_states);
    if (qre_nfa_state->conditions != NULL) qre_nfa_condition_list_destroy(qre_nfa_state->conditions);
    if (qre_nfa_state->attrs != NULL) { qre_nfa_attr_list_destroy(qre_nfa_state->attrs); }
    free(qre_nfa_state);

    return 0;
}

int qre_nfa_state_print(qre_nfa_state_t *qre_nfa_state)
{
    printf("%u--", (unsigned int)qre_nfa_state->state);
    qre_nfa_condition_list_print(qre_nfa_state->conditions);
    printf("-->");
    qre_nfa_state_list_print(qre_nfa_state->next_states);
    if (qre_nfa_state->repeat_from != NULL)
    {
        printf(" (repeat from %d) ", qre_nfa_state->repeat_from->state);
    }
    if ((qre_nfa_state->repeat_min != 1) || (qre_nfa_state->repeat_max != 1))
    {
        printf(" (repeat to %d for ", qre_nfa_state->repeat_to->state);
        printf("{");
        if (qre_nfa_state->repeat_min == qre_nfa_state->repeat_max)
        {
            if (qre_nfa_state->repeat_min == QRE_NFA_STATE_REPEAT_INF)
            { printf("inf"); }
            else 
            { printf("%d", qre_nfa_state->repeat_min); }
        }
        else
        {
            if (qre_nfa_state->repeat_min == QRE_NFA_STATE_REPEAT_INF)
            { printf("inf"); }
            else 
            { printf("%d", qre_nfa_state->repeat_min); }
            printf(",");
            if (qre_nfa_state->repeat_max == QRE_NFA_STATE_REPEAT_INF)
            { printf("inf"); }
            else 
            { printf("%d", qre_nfa_state->repeat_max); }
        }
        printf("}");
        if (qre_nfa_state->greedy == FALSE)
        {
            printf(", non-greedy");
        }
        printf(")");
    }
    printf("\n");

    return 0;
}


/* State List */
qre_nfa_state_list_item_t *qre_nfa_state_list_item_new(struct qre_nfa_state *ptr)
{
    qre_nfa_state_list_item_t *new_qre_nfa_state_list_item = NULL;

    new_qre_nfa_state_list_item = (qre_nfa_state_list_item_t *)malloc(sizeof(qre_nfa_state_list_item_t));
    if (new_qre_nfa_state_list_item == NULL) { goto fail; }
    new_qre_nfa_state_list_item->ptr = ptr;
    new_qre_nfa_state_list_item->next = NULL;

    goto done;
fail:
    if (new_qre_nfa_state_list_item != NULL)
    {
        free(new_qre_nfa_state_list_item);
        new_qre_nfa_state_list_item = NULL;
    }
done:
    return new_qre_nfa_state_list_item;
}

int qre_nfa_state_list_item_destroy(qre_nfa_state_list_item_t *qre_nfa_state_list_item)
{
    free(qre_nfa_state_list_item);

    return 0;
}

int qre_nfa_state_list_item_print(qre_nfa_state_list_item_t *qre_nfa_state_list_item)
{
    printf("%u", qre_nfa_state_list_item->ptr->state);
    
    return 0;
}

qre_nfa_state_list_t *qre_nfa_state_list_new(void)
{
    qre_nfa_state_list_t *new_qre_nfa_state_list = NULL;

    new_qre_nfa_state_list = (qre_nfa_state_list_t *)malloc(sizeof(qre_nfa_state_list_t));
    if (new_qre_nfa_state_list == NULL) { goto fail; }
    new_qre_nfa_state_list->begin = NULL;
    new_qre_nfa_state_list->end = NULL;
    new_qre_nfa_state_list->size = 0;

    goto done;
fail:
    if (new_qre_nfa_state_list != NULL)
    {
        free(new_qre_nfa_state_list);
        new_qre_nfa_state_list = NULL;
    }
done:
    return new_qre_nfa_state_list;
}

int qre_nfa_state_list_destroy(qre_nfa_state_list_t *qre_nfa_state_list)
{
    qre_nfa_state_list_item_t *qre_nfa_state_list_item_cur, *qre_nfa_state_list_item_next;

    qre_nfa_state_list_item_cur = qre_nfa_state_list->begin;
    while (qre_nfa_state_list_item_cur != NULL)
    {
        qre_nfa_state_list_item_next = qre_nfa_state_list_item_cur->next;
        qre_nfa_state_list_item_destroy(qre_nfa_state_list_item_cur);
        qre_nfa_state_list_item_cur = qre_nfa_state_list_item_next;
    }
    free(qre_nfa_state_list);

    return 0;
}

int qre_nfa_state_list_append(qre_nfa_state_list_t *qre_nfa_state_list, \
        qre_nfa_state_list_item_t *new_qre_nfa_state_list_item)
{
    if (qre_nfa_state_list->begin == NULL)
    {
        qre_nfa_state_list->begin = qre_nfa_state_list->end = new_qre_nfa_state_list_item;
    }
    else
    {
        qre_nfa_state_list->end->next = new_qre_nfa_state_list_item;
        qre_nfa_state_list->end = new_qre_nfa_state_list_item;
    }
    qre_nfa_state_list->size += 1;

    return 0;
}

int qre_nfa_state_list_append_with_configure(qre_nfa_state_list_t *qre_nfa_state_list, \
        struct qre_nfa_state *ptr)
{
    int ret = 0;
    qre_nfa_state_list_item_t *new_qre_nfa_state_list_item = NULL;

    new_qre_nfa_state_list_item = qre_nfa_state_list_item_new(ptr);
    if (new_qre_nfa_state_list_item == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail;
    }
    qre_nfa_state_list_append(qre_nfa_state_list, new_qre_nfa_state_list_item);

    goto done;
fail:
    if (new_qre_nfa_state_list_item != NULL)
    {
        qre_nfa_state_list_item_destroy(new_qre_nfa_state_list_item);
        new_qre_nfa_state_list_item = NULL;
    }
done:
    return ret;
}

int qre_nfa_state_list_print(qre_nfa_state_list_t *qre_nfa_state_list)
{
    int first = 1;
    qre_nfa_state_list_item_t *qre_nfa_state_list_item_cur, *qre_nfa_state_list_item_next;

    qre_nfa_state_list_item_cur = qre_nfa_state_list->begin;
    while (qre_nfa_state_list_item_cur != NULL)
    {
        qre_nfa_state_list_item_next = qre_nfa_state_list_item_cur->next;

        if (first != 0) { first = 0; }
        else { printf(", "); }

        qre_nfa_state_list_item_print(qre_nfa_state_list_item_cur);
        qre_nfa_state_list_item_cur = qre_nfa_state_list_item_next;
    }

    return 0;
}


/* State Table */
qre_nfa_state_table_t *qre_nfa_state_table_new(void)
{
    qre_nfa_state_table_t *new_qre_nfa_state_table = NULL;

    new_qre_nfa_state_table = (qre_nfa_state_table_t *)malloc(sizeof(qre_nfa_state_table_t));
    if (new_qre_nfa_state_table == NULL) { goto fail; }
    new_qre_nfa_state_table->begin = new_qre_nfa_state_table->end = NULL;
    new_qre_nfa_state_table->size = 0;

    new_qre_nfa_state_table->state_num_pool = 0;
    new_qre_nfa_state_table->state_num_start = new_qre_nfa_state_table->state_num_end = 0;

    goto done;
fail:
    if (new_qre_nfa_state_table != NULL)
    {
        free(new_qre_nfa_state_table);
        new_qre_nfa_state_table = NULL;
    }
done:
    return new_qre_nfa_state_table;
}

int qre_nfa_state_table_destroy(qre_nfa_state_table_t *qre_nfa_state_table)
{
    qre_nfa_state_t *qre_nfa_state_cur, *qre_nfa_state_next;

    qre_nfa_state_cur = qre_nfa_state_table->begin;
    while (qre_nfa_state_cur != NULL)
    {
        qre_nfa_state_next = qre_nfa_state_cur->next; 
        qre_nfa_state_destroy(qre_nfa_state_cur);
        qre_nfa_state_cur = qre_nfa_state_next; 
    }

    free(qre_nfa_state_table);

    return 0;
}

int qre_nfa_state_table_append(qre_nfa_state_table_t *qre_nfa_state_table, \
        qre_nfa_state_t *new_qre_nfa_state)
{
    if (qre_nfa_state_table->begin == NULL)
    {
        qre_nfa_state_table->begin = qre_nfa_state_table->end = new_qre_nfa_state;
    }
    else
    {
        qre_nfa_state_table->end->next = new_qre_nfa_state;
        qre_nfa_state_table->end = new_qre_nfa_state;
    }
    qre_nfa_state_table->size += 1;

    return 0;
}

int qre_nfa_state_table_print(qre_nfa_state_table_t *qre_nfa_state_table)
{
    qre_nfa_state_t *qre_nfa_state_cur, *qre_nfa_state_next;

    qre_nfa_state_cur = qre_nfa_state_table->begin;
    while (qre_nfa_state_cur != NULL)
    {
        qre_nfa_state_next = qre_nfa_state_cur->next; 
        qre_nfa_state_print(qre_nfa_state_cur);
        qre_nfa_state_cur = qre_nfa_state_next; 
    }

    return 0;
}

uint32_t qre_nfa_state_table_state_num_alloc(qre_nfa_state_table_t *qre_nfa_state_table)
{
    uint32_t value = qre_nfa_state_table->state_num_pool;

    qre_nfa_state_table->state_num_pool += 1;

    return value;
}


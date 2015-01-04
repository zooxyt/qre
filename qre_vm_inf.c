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

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "qre_common.h"
#include "qre_err.h"
#include "qre_vm.h"

/* .data */
qre_vm_data_item_num_t *qre_vm_data_item_num_new(int value)
{
    qre_vm_data_item_num_t *new_qre_vm_data_item_num = NULL;

    new_qre_vm_data_item_num = (qre_vm_data_item_num_t *)malloc(sizeof(qre_vm_data_item_num_t));
    if (new_qre_vm_data_item_num == NULL) { goto fail; }
    new_qre_vm_data_item_num->value = value;

    goto done;
fail:
    if (new_qre_vm_data_item_num != NULL)
    {
        free(new_qre_vm_data_item_num);
        new_qre_vm_data_item_num = NULL;
    }
done:
    return new_qre_vm_data_item_num;
}

int qre_vm_data_item_num_destroy(qre_vm_data_item_num_t *qre_vm_data_item_num)
{
    free(qre_vm_data_item_num);

    return 0;
}

qre_vm_data_item_str_t *qre_vm_data_item_str_new(qre_char_t *str, size_t str_len)
{
    qre_vm_data_item_str_t *new_qre_vm_data_item_str = NULL;

    new_qre_vm_data_item_str = (qre_vm_data_item_str_t *)malloc(sizeof(qre_vm_data_item_str_t));
    if (new_qre_vm_data_item_str == NULL) { goto fail; }
    new_qre_vm_data_item_str->str = NULL;
    new_qre_vm_data_item_str->str = (qre_char_t *)malloc(sizeof(qre_char_t) * str_len);
    if (new_qre_vm_data_item_str->str == NULL) { goto fail; }
    qre_char_memcpy(new_qre_vm_data_item_str->str, str, str_len);
    new_qre_vm_data_item_str->str_len = str_len;

    goto done;
fail:
    if (new_qre_vm_data_item_str != NULL)
    {
        if (new_qre_vm_data_item_str->str != NULL) free(new_qre_vm_data_item_str->str);
        free(new_qre_vm_data_item_str);
        new_qre_vm_data_item_str = NULL;
    }
done:
    return new_qre_vm_data_item_str;
}

int qre_vm_data_item_str_destroy(qre_vm_data_item_str_t *qre_vm_data_item_str)
{
    if (qre_vm_data_item_str->str != NULL) free(qre_vm_data_item_str->str);
    free(qre_vm_data_item_str);

    return 0;
}

qre_vm_data_t *qre_vm_data_new(void)
{
    qre_vm_data_t *new_qre_vm_data = NULL;

    new_qre_vm_data = (qre_vm_data_t *)malloc(sizeof(qre_vm_data_t));
    if (new_qre_vm_data == NULL) { goto fail; }
    new_qre_vm_data->items = NULL;
    new_qre_vm_data->size = 0;

    goto done;
fail:
    if (new_qre_vm_data != NULL)
    {
        free(new_qre_vm_data);
        new_qre_vm_data = NULL;
    }
done:
    return new_qre_vm_data;
}

int qre_vm_data_destroy(qre_vm_data_t *qre_vm_data)
{
    qre_vm_data_item_t *qre_vm_data_item_cur;
    size_t idx;

    if (qre_vm_data->items != NULL) 
    {
        qre_vm_data_item_cur = qre_vm_data->items;
        for (idx = 0; idx != qre_vm_data->size; idx++)
        {
            switch (qre_vm_data_item_cur->type)
            {
                case QRE_VM_DATA_ITEM_NUM:
                    qre_vm_data_item_num_destroy(qre_vm_data_item_cur->ptr);
                    break;
                case QRE_VM_DATA_ITEM_STR:
                    qre_vm_data_item_str_destroy(qre_vm_data_item_cur->ptr);
                    break;
            }
            qre_vm_data_item_cur++;
        }
        free(qre_vm_data->items); 
    }
    free(qre_vm_data);

    return 0;
}

/* .text */
qre_vm_text_t *qre_vm_text_new(void)
{
    qre_vm_text_t *new_qre_vm_text = NULL;

    new_qre_vm_text = (qre_vm_text_t *)malloc(sizeof(qre_vm_text_t));
    if (new_qre_vm_text == NULL) { goto fail; }
    new_qre_vm_text->items = NULL;
    new_qre_vm_text->size = 0;

    goto done;
fail:
    if (new_qre_vm_text != NULL)
    {
        free(new_qre_vm_text);
        new_qre_vm_text = NULL;
    }
done:
    return new_qre_vm_text;
}

int qre_vm_text_destroy(qre_vm_text_t *qre_vm_text)
{
    if (qre_vm_text->items != NULL) { free(qre_vm_text->items); }
    free(qre_vm_text);

    return 0;
}

/* Capture */
qre_vm_capture_t *qre_vm_capture_new(uint32_t id)
{
    qre_vm_capture_t *new_qre_vm_capture = NULL;

    new_qre_vm_capture = (qre_vm_capture_t *)malloc(sizeof(qre_vm_capture_t));
    if (new_qre_vm_capture == NULL) { goto fail; }

    new_qre_vm_capture->id = id;
    new_qre_vm_capture->str_p = NULL;
    new_qre_vm_capture->str_endp = NULL;
    new_qre_vm_capture->next = NULL;

    goto done;
fail:
    if (new_qre_vm_capture != NULL)
    {
        free(new_qre_vm_capture);
        new_qre_vm_capture = NULL;
    }
done:
    return new_qre_vm_capture;
}

int qre_vm_capture_destroy(qre_vm_capture_t *qre_vm_capture)
{
    free(qre_vm_capture);

    return 0;
}

qre_vm_capture_list_t *qre_vm_capture_list_new(void)
{
    qre_vm_capture_list_t *new_qre_vm_capture_list = NULL;

    new_qre_vm_capture_list = (qre_vm_capture_list_t *)malloc(sizeof(qre_vm_capture_list_t));
    if (new_qre_vm_capture_list == NULL) { goto fail; }
    new_qre_vm_capture_list->begin = new_qre_vm_capture_list->end = NULL;

    goto done;
fail:
    if (new_qre_vm_capture_list != NULL)
    {
        free(new_qre_vm_capture_list);
        new_qre_vm_capture_list = NULL;
    }
done:
    return new_qre_vm_capture_list;
}

int qre_vm_capture_list_destroy(qre_vm_capture_list_t *qre_vm_capture_list)
{
    qre_vm_capture_t *qre_vm_capture_cur, *qre_vm_capture_next;

    qre_vm_capture_cur = qre_vm_capture_list->begin;
    while (qre_vm_capture_cur != NULL)
    {
        qre_vm_capture_next = qre_vm_capture_cur->next; 
        qre_vm_capture_destroy(qre_vm_capture_cur);
        qre_vm_capture_cur = qre_vm_capture_next; 
    }
    free(qre_vm_capture_list);

    return 0;
}

int qre_vm_capture_list_append(qre_vm_capture_list_t *qre_vm_capture_list, \
        qre_vm_capture_t *new_qre_vm_capture)
{
    if (qre_vm_capture_list->begin == NULL)
    {
        qre_vm_capture_list->begin = qre_vm_capture_list->end = new_qre_vm_capture;
    }
    else
    {
        qre_vm_capture_list->end->next = new_qre_vm_capture;
        qre_vm_capture_list->end = new_qre_vm_capture;
    }

    return 0;
}

qre_vm_capture_t *qre_vm_capture_list_lookup( \
        qre_vm_capture_list_t *qre_vm_capture_list, \
        uint32_t id)
{
    qre_vm_capture_t *qre_vm_capture_cur;

    qre_vm_capture_cur = qre_vm_capture_list->begin;
    while (qre_vm_capture_cur != NULL)
    {
        if (qre_vm_capture_cur->id == id)
        {
            return qre_vm_capture_cur;
        }
        qre_vm_capture_cur = qre_vm_capture_cur->next;
    }

    return NULL;
}


/* Running Stack */
qre_vm_run_stack_frame_t *qre_vm_run_stack_frame_new(void)
{
    qre_vm_run_stack_frame_t *new_qre_vm_run_stack_frame = NULL;

    new_qre_vm_run_stack_frame = (qre_vm_run_stack_frame_t *)malloc(sizeof(qre_vm_run_stack_frame_t));
    if (new_qre_vm_run_stack_frame == NULL)
    { goto fail; }
    new_qre_vm_run_stack_frame->captures = NULL;
    new_qre_vm_run_stack_frame->pc = 0;
    new_qre_vm_run_stack_frame->str_p = NULL;
    new_qre_vm_run_stack_frame->str_startp = NULL;

    new_qre_vm_run_stack_frame->type = QRE_VM_REPEAT_TYPE_NONE;

    new_qre_vm_run_stack_frame->repeat_min = QRE_VM_REPEAT_MIN_DEFAULT;
    new_qre_vm_run_stack_frame->repeat_max = QRE_VM_REPEAT_MAX_DEFAULT;
    new_qre_vm_run_stack_frame->repeat = 0;
    new_qre_vm_run_stack_frame->repeat_fail = FALSE;

    new_qre_vm_run_stack_frame->captures = qre_vm_capture_list_new();
    if (new_qre_vm_run_stack_frame->captures == NULL) { goto fail; }

    new_qre_vm_run_stack_frame->prev = NULL;
    new_qre_vm_run_stack_frame->next = NULL;

    goto done;
fail:
    if (new_qre_vm_run_stack_frame != NULL)
    {
        if (new_qre_vm_run_stack_frame->captures != NULL) 
        { qre_vm_capture_list_destroy(new_qre_vm_run_stack_frame->captures); }
        qre_vm_run_stack_frame_destroy(new_qre_vm_run_stack_frame);
        new_qre_vm_run_stack_frame = NULL;
    }
done:
    return new_qre_vm_run_stack_frame;
}

int qre_vm_run_stack_frame_destroy(qre_vm_run_stack_frame_t *qre_vm_run_stack_frame)
{
    if (qre_vm_run_stack_frame->captures != NULL) 
    { qre_vm_capture_list_destroy(qre_vm_run_stack_frame->captures); }
    free(qre_vm_run_stack_frame);

    return 0;
}

qre_vm_run_stack_t *qre_vm_run_stack_new(void)
{
    qre_vm_run_stack_t *new_qre_vm_run_stack = NULL;

    new_qre_vm_run_stack = (qre_vm_run_stack_t *)malloc(sizeof(qre_vm_run_stack_t));
    if (new_qre_vm_run_stack == NULL) { goto fail; }
    new_qre_vm_run_stack->bottom = NULL;
    new_qre_vm_run_stack->top = NULL;
    new_qre_vm_run_stack->size = 0;

    goto done;
fail:
    if (new_qre_vm_run_stack != NULL)
    {
        qre_vm_run_stack_destroy(new_qre_vm_run_stack);
        new_qre_vm_run_stack = NULL;
    }
done:
    return new_qre_vm_run_stack;
}

int qre_vm_run_stack_destroy(qre_vm_run_stack_t *qre_vm_run_stack)
{
    qre_vm_run_stack_frame_t *qre_vm_run_stack_frame_cur, *qre_vm_run_stack_frame_next;

    qre_vm_run_stack_frame_cur = qre_vm_run_stack->bottom;
    while (qre_vm_run_stack_frame_cur != NULL)
    {
        qre_vm_run_stack_frame_next = qre_vm_run_stack_frame_cur->next; 
        qre_vm_run_stack_frame_destroy( qre_vm_run_stack_frame_cur);
        qre_vm_run_stack_frame_cur = qre_vm_run_stack_frame_next; 
    }
    free(qre_vm_run_stack);

    return 0;
}

int qre_vm_run_stack_push(qre_vm_run_stack_t *qre_vm_run_stack, \
        qre_vm_run_stack_frame_t *new_qre_vm_run_stack_frame)
{
    if (qre_vm_run_stack->bottom == NULL)
    {
        qre_vm_run_stack->bottom = qre_vm_run_stack->top = new_qre_vm_run_stack_frame;
    }
    else
    {
        new_qre_vm_run_stack_frame->prev = qre_vm_run_stack->top;
        qre_vm_run_stack->top->next = new_qre_vm_run_stack_frame;
        qre_vm_run_stack->top = new_qre_vm_run_stack_frame;
    }
    qre_vm_run_stack->size += 1;

    return 0;
}

int qre_vm_run_stack_push_with_configure(qre_vm_run_stack_t *qre_vm_run_stack, \
        uint32_t pc, qre_char_t *str_p, int type)
{
    int ret = 0;
    qre_vm_run_stack_frame_t *new_qre_vm_run_stack_frame = NULL;

    new_qre_vm_run_stack_frame = qre_vm_run_stack_frame_new();
    if (new_qre_vm_run_stack_frame == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail;
    }
    new_qre_vm_run_stack_frame->pc = pc;
    new_qre_vm_run_stack_frame->str_p = str_p;
    new_qre_vm_run_stack_frame->str_startp = str_p;
    new_qre_vm_run_stack_frame->type = type;

    qre_vm_run_stack_push(qre_vm_run_stack, new_qre_vm_run_stack_frame);

    goto done;
fail:
    if (new_qre_vm_run_stack_frame != NULL)
    {
        qre_vm_run_stack_frame_destroy(new_qre_vm_run_stack_frame);
        new_qre_vm_run_stack_frame = NULL;
    }
done:
    return ret;
}

int qre_vm_run_stack_pop(qre_vm_run_stack_t *qre_vm_run_stack)
{
    qre_vm_run_stack_frame_t *qre_vm_run_stack_frame_prev;

    if (qre_vm_run_stack->size == 0) { return -QRE_ERR_RUN_STACK_EMPTY; }

    qre_vm_run_stack_frame_prev = qre_vm_run_stack->top->prev;
    qre_vm_run_stack_frame_destroy(qre_vm_run_stack->top);
    qre_vm_run_stack_frame_prev->next = NULL;
    qre_vm_run_stack->top = qre_vm_run_stack_frame_prev;
    qre_vm_run_stack->size -= 1;
    if (qre_vm_run_stack->top == NULL) qre_vm_run_stack->bottom = NULL;

    return 0;
}

qre_vm_capture_t *qre_vm_run_stack_capture_list_lookup( \
        qre_vm_run_stack_t *qre_vm_run_stack, \
        uint32_t id)
{
    qre_vm_run_stack_frame_t *qre_vm_run_stack_frame_cur;
    qre_vm_capture_t *target_qre_vm_capture = NULL;

    qre_vm_run_stack_frame_cur = qre_vm_run_stack->top;
    while (qre_vm_run_stack_frame_cur != NULL)
    {
        target_qre_vm_capture = qre_vm_capture_list_lookup(qre_vm_run_stack_frame_cur->captures, id);
        if (target_qre_vm_capture != NULL) { return target_qre_vm_capture; }

        qre_vm_run_stack_frame_cur = qre_vm_run_stack_frame_cur->prev; 
    }

    return NULL;
}


/* VM */
qre_vm_t *qre_vm_new(void)
{
    qre_vm_t *new_qre_vm = NULL;

    new_qre_vm = (qre_vm_t *)malloc(sizeof(qre_vm_t));
    if (new_qre_vm == NULL) { goto fail; }
    new_qre_vm->str = NULL;
    new_qre_vm->data = NULL;
    new_qre_vm->text = NULL;
    new_qre_vm->run_stack = NULL;
    new_qre_vm->flags = 0;

    new_qre_vm->data = qre_vm_data_new();
    if (new_qre_vm->data == NULL) { goto fail; }
    new_qre_vm->text = qre_vm_text_new();
    if (new_qre_vm->text == NULL) { goto fail; }
    new_qre_vm->run_stack = qre_vm_run_stack_new();
    if (new_qre_vm->run_stack == NULL) { goto fail; }

    goto done;
fail:
    if (new_qre_vm != NULL)
    {
        qre_vm_destroy(new_qre_vm);
        new_qre_vm = NULL;
    }
done:
    return new_qre_vm;
}

int qre_vm_destroy(qre_vm_t *qre_vm)
{
    if (qre_vm->data != NULL) { qre_vm_data_destroy(qre_vm->data); }
    if (qre_vm->text != NULL) { qre_vm_text_destroy(qre_vm->text); }
    if (qre_vm->run_stack != NULL) { qre_vm_run_stack_destroy(qre_vm->run_stack); }
    free(qre_vm);

    return 0;
}

int qre_vm_flags_set(qre_vm_t *qre_vm, int idx)
{
    qre_vm->flags |= (1 << idx);

    return 0;
}

int qre_vm_flags_clear(qre_vm_t *qre_vm, int idx)
{
    qre_vm->flags &= ~(1 << idx);

    return 0;
}

int qre_vm_flags_set_by_value(qre_vm_t *qre_vm, int idx, int value)
{
    if (value != 0) { qre_vm_flags_set(qre_vm, idx); }
    else { qre_vm_flags_clear(qre_vm, idx); }

    return 0;
}

int qre_vm_flags_test(qre_vm_t *qre_vm, int idx)
{
    return ((qre_vm->flags & (1 << idx)) != 0) ? 1 : 0;
}


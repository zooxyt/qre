/*
   Quick Regular Expression Engine : Virtual Machine
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
#include <string.h>

#include "qre_common.h"
#include "qre_err.h"
#include "qre_il.h"

#include "qre_vm_inf.h"
#include "qre_vm_op.h"

#include "qre_vm.h"

/*#define QRE_DEBUG*/

int qre_vm_new_from_il(qre_vm_t **qre_vm_out, \
        qre_il_t *qre_il, \
        qre_string_t *qre_string)
{
    int ret = 0;
    qre_vm_t *new_qre_vm = NULL;
    qre_vm_text_item_t *qre_vm_text_item_cur;
    qre_il_text_item_t *qre_il_text_item_cur;
    qre_vm_data_item_t *qre_vm_data_item_cur;
    qre_il_data_item_t *qre_il_data_item_cur;

    qre_il_data_item_int_t *qre_il_data_item_int;
    qre_il_data_item_str_t *qre_il_data_item_str;

    new_qre_vm = qre_vm_new();
    if (new_qre_vm == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail;
    }

    /* Fill .data */
    if (qre_il->data->size != 0)
    {
        new_qre_vm->data->items = (qre_vm_data_item_t *)malloc(sizeof(qre_vm_data_item_t) * qre_il->data->size);
        if (new_qre_vm->data->items == NULL)
        {
            ret = -QRE_ERR_OUT_OF_MEMORY;
            goto fail;
        }

        /* Clear ptr */
        qre_il_data_item_cur = qre_il->data->begin;
        qre_vm_data_item_cur = new_qre_vm->data->items;
        while (qre_il_data_item_cur != NULL)
        {
            qre_vm_data_item_cur->type = QRE_VM_DATA_ITEM_NONE;
            qre_vm_data_item_cur->ptr = NULL;

            qre_il_data_item_cur = qre_il_data_item_cur->next; 
            qre_vm_data_item_cur++;
        }

        /* Fill ptr */
        qre_il_data_item_cur = qre_il->data->begin;
        qre_vm_data_item_cur = new_qre_vm->data->items;
        while (qre_il_data_item_cur != NULL)
        {
            qre_vm_data_item_cur->id = qre_il_data_item_cur->id;

            switch (qre_il_data_item_cur->type)
            {
                case QRE_IL_DATA_ITEM_TYPE_INT:
                    qre_il_data_item_int = qre_il_data_item_cur->ptr;
                    qre_vm_data_item_cur->ptr = qre_vm_data_item_num_new( \
                            qre_il_data_item_int->value);
                    if (qre_vm_data_item_cur->ptr == NULL)
                    {
                        ret = -QRE_ERR_OUT_OF_MEMORY;
                        goto fail;
                    }
                    qre_vm_data_item_cur->type = QRE_VM_DATA_ITEM_NUM;
                    break;
                case QRE_IL_DATA_ITEM_TYPE_STR:
                    qre_il_data_item_str = qre_il_data_item_cur->ptr;
                    qre_vm_data_item_cur->ptr = qre_vm_data_item_str_new( \
                            qre_il_data_item_str->str, \
                            qre_il_data_item_str->str_len);
                    if (qre_vm_data_item_cur->ptr == NULL)
                    {
                        ret = -QRE_ERR_OUT_OF_MEMORY;
                        goto fail;
                    }
                    qre_vm_data_item_cur->type = QRE_VM_DATA_ITEM_STR;
                    break;
            }

            qre_il_data_item_cur = qre_il_data_item_cur->next; 
            qre_vm_data_item_cur++;
        }

        new_qre_vm->data->size = qre_il->data->size;
    }


    /* Fill .text */
    if (qre_il->text->size != 0)
    {
        new_qre_vm->text->items = (qre_vm_text_item_t *)malloc(sizeof(qre_vm_text_item_t) * qre_il->text->size);
        if (new_qre_vm->text->items == NULL)
        {
            ret = -QRE_ERR_OUT_OF_MEMORY;
            goto fail;
        }

        qre_il_text_item_cur = qre_il->text->begin;
        qre_vm_text_item_cur = new_qre_vm->text->items;
        while (qre_il_text_item_cur != NULL)
        {
            qre_vm_text_item_cur->opcode = qre_il_text_item_cur->opcode;
            qre_vm_text_item_cur->operand = qre_il_text_item_cur->operand;

            qre_il_text_item_cur = qre_il_text_item_cur->next; 
            qre_vm_text_item_cur++;
        }

        new_qre_vm->text->size = qre_il->text->size;
    }

    new_qre_vm->str = qre_string;

    /* Create first stack frame */
    ret = qre_vm_run_stack_push_with_configure(new_qre_vm->run_stack, \
            qre_il->pc_entrance, qre_string->body, QRE_VM_REPEAT_TYPE_NONE);
    if (ret != 0) { goto fail; }

    *qre_vm_out = new_qre_vm;

    goto done;
fail:
    if (new_qre_vm != NULL)
    {
        qre_vm_destroy(new_qre_vm);
    }
done:
    return ret;
}

int instrument_print(uint32_t opcode, uint32_t operand)
{
    const char *str;
    int str_len;
    int type;

    qre_vm_op_cvt_to_name(&str, opcode);
    qre_vm_op_cvt_to_type(&type, opcode);

    printf("%s ", str);
    str_len = strlen(str);
    str_len = 7 - str_len;
    while (str_len-- != 0)
    {
        printf(" ");
    }

    switch (type)
    {
        case QRE_VM_OP_OPERAND_TYPE_RAW:
            printf("%u ", (unsigned int)operand);
            break;
        case QRE_VM_OP_OPERAND_TYPE_CHAR:
            printf("\'");
            qre_char_print_utf_8(operand);
            printf("\'");
            break;
        case QRE_VM_OP_OPERAND_TYPE_NONE:
            break;
    }
    printf("\n");

    return 0;
}

struct qre_vm_context
{
    qre_result_t *qre_result;
    qre_string_t *qre_string;
};
typedef struct qre_vm_context qre_vm_context_t;

#define IS_WHITESPACE(x) \
    (((x)==' ')|| \
     ((x)=='\f')|| \
     ((x)=='\n')|| \
     ((x)=='\r')|| \
     ((x)=='\t')|| \
     ((x)=='\v'))
#define IS_WORD(x) \
    (((x)=='_') || \
     (('a'<=(x))&&((x)<='z')) || \
     (('A'<=(x))&&((x)<='Z')) || \
     (('0'<=(x))&&((x)<='9')))

static int qre_vm_step_capture(qre_vm_context_t *context, qre_vm_t *qre_vm)
{
    int ret = 0;
    uint32_t pc = qre_vm->run_stack->top->pc;
    uint32_t opcode = qre_vm->text->items[pc].opcode;
    uint32_t operand = qre_vm->text->items[pc].operand;
    qre_vm_run_stack_frame_t *new_qre_vm_run_stack_frame = NULL; 
    qre_vm_run_stack_frame_t *top_frame = qre_vm->run_stack->top; 
    qre_vm_capture_t *new_qre_vm_capture = NULL;
    qre_vm_capture_t *target_qre_vm_capture = NULL;

    (void)context;

    switch (opcode)
    {
        case QRE_OP_CAPNBEG:
        case QRE_OP_CAPLBEG:
            new_qre_vm_capture = qre_vm_capture_new(operand);
            if (new_qre_vm_capture == NULL)
            { ret = -QRE_ERR_OUT_OF_MEMORY; goto fail; }
            new_qre_vm_capture->str_p = top_frame->str_p;
            qre_vm_capture_list_append(top_frame->captures, \
                    new_qre_vm_capture);

            qre_vm->run_stack->top->pc++;
            break;
        case QRE_OP_CAPNEND:
        case QRE_OP_CAPLEND:
            target_qre_vm_capture = qre_vm_run_stack_capture_list_lookup( \
                    qre_vm->run_stack, \
                    operand);
            if (target_qre_vm_capture == NULL)
            { ret = -QRE_ERR_INTERNAL; goto fail; }
            target_qre_vm_capture->str_endp = top_frame->str_p;

            qre_vm->run_stack->top->pc++;
            break;
        default:
            ret = -QRE_ERR_INTERNAL;
            goto fail;
            break;
    }

    goto done;
fail:
    if (new_qre_vm_capture != NULL) { qre_vm_capture_destroy(new_qre_vm_capture); }
done:
    if (new_qre_vm_run_stack_frame != NULL)
    { qre_vm_run_stack_frame_destroy(new_qre_vm_run_stack_frame); }
    return ret;
}

int qre_vm_step(qre_vm_context_t *context, qre_vm_t *qre_vm)
{
    int ret = 0;
    uint32_t pc = qre_vm->run_stack->top->pc;
    uint32_t opcode = qre_vm->text->items[pc].opcode;
    uint32_t operand = qre_vm->text->items[pc].operand;
    qre_vm_run_stack_frame_t *new_qre_vm_run_stack_frame = NULL; 
    qre_vm_run_stack_frame_t *top_frame = qre_vm->run_stack->top; 

    (void)context;

#ifdef QRE_DEBUG
    printf("%3u: ", pc);
    instrument_print(opcode, operand);
#endif

    switch (opcode)
    {
        case QRE_OP_NOP:
            /* Do nothing */
            break;
        case QRE_OP_SUCC:
            qre_vm_flags_set(qre_vm, QRE_VM_TEST_SUCCESS);
            top_frame->pc++;
            break;
        case QRE_OP_FAIL:
            qre_vm_flags_set(qre_vm, QRE_VM_TEST_FAIL);
            top_frame->pc++;
            break;
        case QRE_OP_ME:
            qre_vm_flags_set_by_value(qre_vm, QRE_VM_TEST_RESULT, *top_frame->str_p == operand);
            top_frame->pc++;
            break;
        case QRE_OP_ML:
            qre_vm_flags_set_by_value(qre_vm, QRE_VM_TEST_RESULT, *top_frame->str_p <= operand);
            top_frame->pc++;
            break;
        case QRE_OP_MG:
            qre_vm_flags_set_by_value(qre_vm, QRE_VM_TEST_RESULT, *top_frame->str_p >= operand);
            top_frame->pc++;
            break;
        case QRE_OP_MA:
            qre_vm_flags_set(qre_vm, QRE_VM_TEST_RESULT);
            top_frame->pc++;
            break;
        case QRE_OP_MSD:
            qre_vm_flags_set_by_value(qre_vm, QRE_VM_TEST_RESULT, ('0' <= *top_frame->str_p) && (*top_frame->str_p <= '9'));
            top_frame->pc++;
            break;
        case QRE_OP_MSND:
            qre_vm_flags_set_by_value(qre_vm, QRE_VM_TEST_RESULT, !(('0' <= *top_frame->str_p) && (*top_frame->str_p <= '9')));
            top_frame->pc++;
            break;
        case QRE_OP_MSF:
            qre_vm_flags_set_by_value(qre_vm, QRE_VM_TEST_RESULT, *top_frame->str_p == 0x0C);
            top_frame->pc++;
            break;
        case QRE_OP_MSNL:
            qre_vm_flags_set_by_value(qre_vm, QRE_VM_TEST_RESULT, *top_frame->str_p == 0x0A);;
            top_frame->pc++;
            break;
        case QRE_OP_MSCR:
            qre_vm_flags_set_by_value(qre_vm, QRE_VM_TEST_RESULT, *top_frame->str_p == 0x0D);
            top_frame->pc++;
            break;
        case QRE_OP_MSS:
            qre_vm_flags_set_by_value(qre_vm, QRE_VM_TEST_RESULT, IS_WHITESPACE(*top_frame->str_p));
            top_frame->pc++;
            break;
        case QRE_OP_MSNS:
            qre_vm_flags_set_by_value(qre_vm, QRE_VM_TEST_RESULT, (!(IS_WHITESPACE(*top_frame->str_p))));
            top_frame->pc++;
            break;
        case QRE_OP_MST:
            qre_vm_flags_set_by_value(qre_vm, QRE_VM_TEST_RESULT, *top_frame->str_p == 0x09);
            top_frame->pc++;
            break;
        case QRE_OP_MSV:
            qre_vm_flags_set_by_value(qre_vm, QRE_VM_TEST_RESULT, *top_frame->str_p == 0x0B);
            top_frame->pc++;
            break;
        case QRE_OP_MSW:
            qre_vm_flags_set_by_value(qre_vm, QRE_VM_TEST_RESULT, IS_WORD(*top_frame->str_p));
            top_frame->pc++;
            break;
        case QRE_OP_MSNW:
            qre_vm_flags_set_by_value(qre_vm, QRE_VM_TEST_RESULT, (!(IS_WORD(*top_frame->str_p))));
            top_frame->pc++;
            break;
        case QRE_OP_TEST:
            /* Failed */
            if (qre_vm_flags_test(qre_vm, QRE_VM_TEST_RESULT) == FALSE)
            {
                if (qre_vm->run_stack->size == 1)
                {
                    qre_vm_flags_set(qre_vm, QRE_VM_TEST_FAIL); 
                    top_frame->pc++;
                }
                else
                {
                    ret = qre_vm_run_stack_pop(qre_vm->run_stack);
                    if (ret != 0) { goto fail; }

                    top_frame = qre_vm->run_stack->top; 

                    switch (top_frame->type)
                    {
                        case QRE_VM_REPEAT_TYPE_NONE:
                            break;
                        case QRE_VM_REPEAT_TYPE_GREEDY:
                            if (top_frame->repeat >= top_frame->repeat_min)
                            {
                                top_frame->repeat_fail = TRUE;
                            }
                            else
                            {
                                ret = qre_vm_run_stack_pop(qre_vm->run_stack);
                                if (ret != 0) { goto fail; }
                                top_frame = qre_vm->run_stack->top; 
                                if (qre_vm->run_stack->size == 1)
                                {
                                    qre_vm_flags_set(qre_vm, QRE_VM_TEST_FAIL); 
                                    top_frame->pc++;
                                }
                            }
                            break;
                        case QRE_VM_REPEAT_TYPE_NON_GREEDY:
                            if (top_frame->repeat >= top_frame->repeat_min)
                            {
                                top_frame->repeat_fail = TRUE;
                            }
                            else
                            {
                                ret = qre_vm_run_stack_pop(qre_vm->run_stack);
                                if (ret != 0) { goto fail; }
                                top_frame = qre_vm->run_stack->top; 
                                if (qre_vm->run_stack->size == 1)
                                {
                                    qre_vm_flags_set(qre_vm, QRE_VM_TEST_FAIL); 
                                    top_frame->pc++;
                                }
                            }
                            break;
                    }
                }
            }
            else
            {
                top_frame->pc++;
            }
            break;
        case QRE_OP_PASS:
            top_frame->str_p++;
            top_frame->pc++;
            break;
        case QRE_OP_JMP:
            top_frame->pc = operand;
            break;
        case QRE_OP_JMPT:
            if (qre_vm_flags_test(qre_vm, QRE_VM_TEST_RESULT) != FALSE)
            { top_frame->pc = operand; }
            else
            { top_frame->pc++; }
            break;
        case QRE_OP_JMPF:
            if (qre_vm_flags_test(qre_vm, QRE_VM_TEST_RESULT) == FALSE)
            { top_frame->pc = operand; }
            else
            { top_frame->pc++; }
            break;
        case QRE_OP_CALL:
            top_frame->pc++;
            ret = qre_vm_run_stack_push_with_configure(qre_vm->run_stack, \
                    operand, top_frame->str_p, QRE_VM_REPEAT_TYPE_NONE);
            if (ret != 0) { goto fail; }
            break;
        case QRE_OP_MINNUM:
            qre_vm->run_stack->top->repeat_min = (int)operand;
            qre_vm->run_stack->top->pc++;
            break;
        case QRE_OP_MAXNUM:
            qre_vm->run_stack->top->repeat_max = (int)operand;
            qre_vm->run_stack->top->pc++;
            break;
        case QRE_OP_MAXINF:
            qre_vm->run_stack->top->repeat_max = QRE_VM_REPEAT_INF;
            qre_vm->run_stack->top->pc++;
            break;
        case QRE_OP_REPBP:
            top_frame->pc++;
            ret = qre_vm_run_stack_push_with_configure(qre_vm->run_stack, \
                    top_frame->pc, top_frame->str_p, QRE_VM_REPEAT_TYPE_GREEDY);
            if (ret != 0) { goto fail; }
            break;
        case QRE_OP_REPBPN:
            top_frame->pc++;
            ret = qre_vm_run_stack_push_with_configure(qre_vm->run_stack, \
                    top_frame->pc, top_frame->str_p, QRE_VM_REPEAT_TYPE_NON_GREEDY);
            if (ret != 0) { goto fail; }
            break;
        case QRE_OP_REPTEST:

            /* End of haystack? */
            if (top_frame->str_p == \
                    context->qre_string->body + context->qre_string->size)
            {
                qre_vm->run_stack->top->pc = operand;
                break;
            }

            switch (qre_vm->run_stack->top->type)
            {
                case QRE_VM_REPEAT_TYPE_GREEDY:
                    if (top_frame->repeat_fail == TRUE)
                    {
                        qre_vm->run_stack->top->pc = operand;
                        top_frame->repeat_fail = FALSE;
                    }
                    else
                    {
                        if (qre_vm->run_stack->top->repeat >= qre_vm->run_stack->top->repeat_min)
                        {
                            if (qre_vm->run_stack->top->repeat == qre_vm->run_stack->top->repeat_max)
                            {
                                qre_vm->run_stack->top->pc = operand;
                            }
                            else
                            {
                                qre_vm->run_stack->top->pc++;
                                qre_vm->run_stack->top->repeat += 1;
                            }
                        }
                        else
                        {
                            qre_vm->run_stack->top->pc++;
                            qre_vm->run_stack->top->repeat += 1;
                        }
                    }
                    break;
                case QRE_VM_REPEAT_TYPE_NON_GREEDY:
                    if (top_frame->repeat_fail == TRUE)
                    {
                        qre_vm->run_stack->top->pc++;
                        qre_vm->run_stack->top->repeat += 1;
                        top_frame->repeat_fail = FALSE;
                    }
                    else
                    {
                        if (qre_vm->run_stack->top->repeat >= qre_vm->run_stack->top->repeat_min)
                        {
                            if (qre_vm->run_stack->top->repeat <= qre_vm->run_stack->top->repeat_max)
                            {
                                qre_vm->run_stack->top->pc = operand;
                            }
                            else
                            {
                                qre_vm->run_stack->top->pc++;
                                qre_vm->run_stack->top->repeat += 1;
                            }
                        }
                        else
                        {
                            qre_vm->run_stack->top->pc++;
                            qre_vm->run_stack->top->repeat += 1;
                        }
                    }
                    break;
                default:
                    break;
            }
            break;
        case QRE_OP_REPJMP:
            qre_vm->run_stack->top->pc = operand;

            new_qre_vm_run_stack_frame = qre_vm_run_stack_frame_new();
            if (new_qre_vm_run_stack_frame == NULL)
            {
                ret = -QRE_ERR_OUT_OF_MEMORY;
                goto fail;
            }
            new_qre_vm_run_stack_frame->pc = operand;
            new_qre_vm_run_stack_frame->type = top_frame->type;
            new_qre_vm_run_stack_frame->repeat = top_frame->repeat;
            new_qre_vm_run_stack_frame->repeat_min = top_frame->repeat_min;
            new_qre_vm_run_stack_frame->repeat_max = top_frame->repeat_max;
            new_qre_vm_run_stack_frame->str_p = top_frame->str_p;
            new_qre_vm_run_stack_frame->str_startp = top_frame->str_p;

            qre_vm_run_stack_push(qre_vm->run_stack, new_qre_vm_run_stack_frame);
            new_qre_vm_run_stack_frame = NULL;
            break;
        case QRE_OP_CAPNBEG:
        case QRE_OP_CAPLBEG:
        case QRE_OP_CAPNEND:
        case QRE_OP_CAPLEND:
            ret = qre_vm_step_capture(context, qre_vm);
            if (ret != 0) { goto fail; }
            break;
        default:
            ret = -QRE_ERR_INTERNAL;
            goto fail;
            break;
    }

    goto done;
fail:
done:
    if (new_qre_vm_run_stack_frame != NULL)
    {
        qre_vm_run_stack_frame_destroy(new_qre_vm_run_stack_frame);
    }
    return ret;
}

static int qre_vm_run_generate_group_capture( \
        qre_vm_context_t *context, \
        qre_vm_t *qre_vm)
{
    int ret = 0;
    qre_vm_run_stack_frame_t *qre_vm_run_stack_frame_cur;
    qre_vm_capture_t *qre_vm_capture_cur = NULL;

    qre_vm_data_item_num_t *qre_vm_data_item_num;
    qre_vm_data_item_str_t *qre_vm_data_item_str;

    qre_result_group_num_t *new_qre_result_group_num = NULL;
    qre_result_group_lbl_t *new_qre_result_group_lbl = NULL;
    qre_result_group_t  *new_qre_result_group = NULL;

    size_t idx;

    qre_vm_run_stack_frame_cur = qre_vm->run_stack->top;
    while (qre_vm_run_stack_frame_cur != NULL)
    {
        qre_vm_capture_cur = qre_vm_run_stack_frame_cur->captures->begin; 
        while (qre_vm_capture_cur != NULL)
        {
            if (qre_vm_capture_cur->id >= qre_vm->data->size)
            {
                ret = -QRE_ERR_INTERNAL;
                goto fail;
            }

            for (idx = 0; idx != qre_vm->data->size; idx++)
            {
                if (qre_vm->data->items[idx].id == qre_vm_capture_cur->id)
                {
                    switch (qre_vm->data->items[idx].type)
                    {
                        case QRE_VM_DATA_ITEM_NUM:
                            qre_vm_data_item_num = qre_vm->data->items[idx].ptr;

                            new_qre_result_group_num = qre_result_group_num_new(qre_vm_data_item_num->value);
                            if (new_qre_result_group_num == NULL) { ret = -QRE_ERR_OUT_OF_MEMORY; goto fail; }

                            new_qre_result_group = qre_result_group_new(QRE_RESULT_GROUP_NUM, new_qre_result_group_num);
                            if (new_qre_result_group == NULL) { ret = -QRE_ERR_OUT_OF_MEMORY; goto fail; }
                            new_qre_result_group_num = NULL;

                            new_qre_result_group->start = qre_vm_capture_cur->str_p - context->qre_string->body;
                            new_qre_result_group->length = qre_vm_capture_cur->str_endp - qre_vm_capture_cur->str_p;

                            qre_result_group_list_append(context->qre_result->groups, new_qre_result_group);
                            new_qre_result_group = NULL;

                            break;
                        case QRE_VM_DATA_ITEM_STR:
                            qre_vm_data_item_str = qre_vm->data->items[idx].ptr;

                            new_qre_result_group_lbl = qre_result_group_lbl_new(qre_vm_data_item_str->str, qre_vm_data_item_str->str_len);
                            if (new_qre_result_group_lbl == NULL) { ret = -QRE_ERR_OUT_OF_MEMORY; goto fail; }

                            new_qre_result_group = qre_result_group_new(QRE_RESULT_GROUP_LBL, new_qre_result_group_lbl);
                            if (new_qre_result_group == NULL) { ret = -QRE_ERR_OUT_OF_MEMORY; goto fail; }
                            new_qre_result_group_lbl = NULL;

                            new_qre_result_group->start = qre_vm_capture_cur->str_p - context->qre_string->body;
                            new_qre_result_group->length = qre_vm_capture_cur->str_endp - qre_vm_capture_cur->str_p;

                            qre_result_group_list_append(context->qre_result->groups, new_qre_result_group);
                            new_qre_result_group = NULL;

                            break;
                    }
                    break;
                }
            }

            qre_vm_capture_cur = qre_vm_capture_cur->next; 
        }

        qre_vm_run_stack_frame_cur = qre_vm_run_stack_frame_cur->prev; 
    }

    goto done;
fail:
    if (new_qre_result_group_num != NULL) qre_result_group_num_destroy(new_qre_result_group_num);
    if (new_qre_result_group_lbl != NULL) qre_result_group_lbl_destroy(new_qre_result_group_lbl);
    if (new_qre_result_group != NULL) qre_result_group_destroy(new_qre_result_group);
done:
    return ret;
}

int qre_vm_run(qre_result_t **qre_result_out, \
        qre_vm_t *qre_vm)
{
    int ret = 0;
    qre_result_t *new_qre_result = NULL;
    qre_vm_context_t context;

    new_qre_result = qre_result_new();
    if (new_qre_result == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail;
    }

    /* Set context */
    context.qre_result = new_qre_result;
    context.qre_string = qre_vm->str;

    for (;;)
    {
        /* Perform a step */
        ret = qre_vm_step(&context, qre_vm);
        if (ret != 0) { goto fail; }

        /* Has result */
        if (qre_vm_flags_test(qre_vm, QRE_VM_TEST_SUCCESS) != FALSE)
        {
            new_qre_result->success = TRUE; 
            new_qre_result->start = 0; 
            new_qre_result->length = qre_vm->run_stack->top->str_p - qre_vm->str->body;

            /* Captured groups */
            ret = qre_vm_run_generate_group_capture(&context, qre_vm);
            if (ret != 0) { goto fail; }

            break; 
        }
        if (qre_vm_flags_test(qre_vm, QRE_VM_TEST_FAIL) != FALSE)
        { new_qre_result->success = FALSE; break; }

    }

    *qre_result_out = new_qre_result;

    goto done;
fail:
    if (new_qre_result != NULL) { qre_result_destroy(new_qre_result); }
done:
    return ret;
}


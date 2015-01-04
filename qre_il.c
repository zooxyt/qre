/*
   Quick Regular Expression Engine : Virtual Machine : IL
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
#include <string.h>

#include "qre_err.h"
#include "qre_common.h"
#include "qre_char.h"
#include "qre_il.h"
#include "qre_vm_op.h"


/* .data */

qre_il_data_item_int_t *qre_il_data_item_int_new(int value)
{
    qre_il_data_item_int_t *new_qre_il_data_item_int = NULL;

    new_qre_il_data_item_int = (qre_il_data_item_int_t *)malloc(sizeof(qre_il_data_item_int_t));
    if (new_qre_il_data_item_int == NULL) { goto fail; }

    new_qre_il_data_item_int->value = value;

    goto done;
fail:
    if (new_qre_il_data_item_int != NULL)
    {
        free(new_qre_il_data_item_int);
        new_qre_il_data_item_int = NULL;
    }
done:
    return new_qre_il_data_item_int;
}

int qre_il_data_item_int_destroy(qre_il_data_item_int_t *qre_il_data_item_int)
{
    free(qre_il_data_item_int);

    return 0;
}

qre_il_data_item_str_t *qre_il_data_item_str_new(qre_char_t *str, size_t str_len)
{
    qre_il_data_item_str_t *new_qre_il_data_item_str = NULL;

    new_qre_il_data_item_str = (qre_il_data_item_str_t *)malloc(sizeof(qre_il_data_item_str_t));
    if (new_qre_il_data_item_str == NULL) { goto fail; }
    new_qre_il_data_item_str->str = NULL;
    new_qre_il_data_item_str->str = (qre_char_t *)malloc(sizeof(qre_char_t) * (str_len));
    if (new_qre_il_data_item_str->str == NULL) { goto fail; }
    qre_char_memcpy(new_qre_il_data_item_str->str, str, str_len);

    new_qre_il_data_item_str->str_len = str_len;

    goto done;
fail:
    if (new_qre_il_data_item_str != NULL)
    {
        if (new_qre_il_data_item_str->str != NULL) { free(new_qre_il_data_item_str->str); }
        free(new_qre_il_data_item_str);
        new_qre_il_data_item_str = NULL;
    }
done:
    return new_qre_il_data_item_str;
}

int qre_il_data_item_str_destroy(qre_il_data_item_str_t *qre_il_data_item_str)
{
    if (qre_il_data_item_str->str != NULL) { free(qre_il_data_item_str->str); }
    free(qre_il_data_item_str);

    return 0;
}

qre_il_data_item_t *qre_il_data_item_new(uint32_t id, int type, void *ptr)
{
    qre_il_data_item_t *new_qre_il_data_item = NULL;

    new_qre_il_data_item = (qre_il_data_item_t *)malloc(sizeof(qre_il_data_item_t));
    if (new_qre_il_data_item == NULL) { goto fail; } 
    new_qre_il_data_item->id = id;
    new_qre_il_data_item->type = type;
    new_qre_il_data_item->ptr = ptr;
    new_qre_il_data_item->next = NULL;

    goto done;
fail:
    if (new_qre_il_data_item != NULL)
    {
        free(new_qre_il_data_item);
        new_qre_il_data_item = NULL;
    }
done:
    return new_qre_il_data_item;
}

int qre_il_data_item_destroy(qre_il_data_item_t *qre_il_data_item)
{
    switch (qre_il_data_item->type)
    {
        case QRE_IL_DATA_ITEM_TYPE_INT:
            qre_il_data_item_int_destroy(qre_il_data_item->ptr);
            break;
        case QRE_IL_DATA_ITEM_TYPE_STR:
            qre_il_data_item_str_destroy(qre_il_data_item->ptr);
            break;
    }
    free(qre_il_data_item);

    return 0;
}

int qre_il_data_item_print(qre_il_data_item_t *qre_il_data_item)
{
    qre_il_data_item_int_t *qre_il_data_item_int;
    qre_il_data_item_str_t *qre_il_data_item_str;

    switch (qre_il_data_item->type)
    {
        case QRE_IL_DATA_ITEM_TYPE_INT:
            qre_il_data_item_int = qre_il_data_item->ptr;
            printf("%d", qre_il_data_item_int->value);
            printf(":int");
            break;
        case QRE_IL_DATA_ITEM_TYPE_STR:
            qre_il_data_item_str = qre_il_data_item->ptr;
            qre_str_print_utf_8(qre_il_data_item_str->str, qre_il_data_item_str->str_len);
            printf(":str");
            break;
    }
    printf("\n");

    return 0;
}

qre_il_data_t *qre_il_data_new(void)
{
    qre_il_data_t *new_qre_il_data = NULL;

    new_qre_il_data = (qre_il_data_t *)malloc(sizeof(qre_il_data_t));
    if (new_qre_il_data == NULL) { goto fail; }
    new_qre_il_data->begin = new_qre_il_data->end = NULL;
    new_qre_il_data->size = 0;

    new_qre_il_data->id_pool = 0;

    goto done;
fail:
    if (new_qre_il_data != NULL)
    {
        free(new_qre_il_data);
        new_qre_il_data = NULL;
    }
done:
    return new_qre_il_data;
}

int qre_il_data_destroy(qre_il_data_t *qre_il_data)
{
    qre_il_data_item_t *qre_il_data_item_cur, *qre_il_data_item_next;

    qre_il_data_item_cur = qre_il_data->begin;
    while (qre_il_data_item_cur != NULL)
    {
        qre_il_data_item_next = qre_il_data_item_cur->next;
        qre_il_data_item_destroy(qre_il_data_item_cur);
        qre_il_data_item_cur = qre_il_data_item_next;
    }

    free(qre_il_data);

    return 0;
}

uint32_t qre_il_data_get_id(qre_il_data_t *qre_il_data)
{
    uint32_t id = qre_il_data->id_pool++;
    return id;
}

int qre_il_data_assign_int(uint32_t *id_out, qre_il_data_t *qre_il_data, int value)
{
    int ret = 0;
    qre_il_data_item_t *qre_il_data_item_cur;
    qre_il_data_item_int_t *qre_il_data_item_int;
    qre_il_data_item_t *new_qre_il_data_item = NULL;
    qre_il_data_item_int_t *new_qre_il_data_item_int = NULL;
    uint32_t matched_id = 0;
    int matched = FALSE;

    qre_il_data_item_cur = qre_il_data->begin;
    while (qre_il_data_item_cur != NULL)
    {
        if (qre_il_data_item_cur->type == QRE_IL_DATA_ITEM_TYPE_INT)
        {
            qre_il_data_item_int = qre_il_data_item_cur->ptr;
            if (qre_il_data_item_int->value == value)
            {
                matched_id = qre_il_data_item_cur->id;
                matched = TRUE;
                break;
            }
        }
        qre_il_data_item_cur = qre_il_data_item_cur->next;
    }

    if (matched == TRUE) 
    {
        *id_out = matched_id;
        return 0;
    }

    matched_id = qre_il_data_get_id(qre_il_data);

    new_qre_il_data_item_int = qre_il_data_item_int_new(value);
    if (new_qre_il_data_item_int == NULL) { ret = -QRE_ERR_OUT_OF_MEMORY; goto fail; }
    new_qre_il_data_item = qre_il_data_item_new(matched_id, QRE_IL_DATA_ITEM_TYPE_INT, new_qre_il_data_item_int);
    if (new_qre_il_data_item == NULL) { ret = -QRE_ERR_OUT_OF_MEMORY; goto fail; }
    new_qre_il_data_item_int = NULL;
    qre_il_data_append(qre_il_data, new_qre_il_data_item);
    new_qre_il_data_item_int = NULL;

    *id_out = matched_id;

    goto done;
fail:
    if (new_qre_il_data_item_int != NULL)  qre_il_data_item_int_destroy(new_qre_il_data_item_int);
    if (new_qre_il_data_item != NULL)  qre_il_data_item_destroy(new_qre_il_data_item);
done:
    return ret;
}

int qre_il_data_assign_str(uint32_t *id_out, qre_il_data_t *qre_il_data, qre_char_t *str, size_t str_len)
{
    int ret = 0;
    qre_il_data_item_t *qre_il_data_item_cur;
    qre_il_data_item_str_t *qre_il_data_item_str;
    qre_il_data_item_t *new_qre_il_data_item = NULL;
    qre_il_data_item_str_t *new_qre_il_data_item_str = NULL;
    uint32_t matched_id = 0;
    int matched = FALSE;

    qre_il_data_item_cur = qre_il_data->begin;
    while (qre_il_data_item_cur != NULL)
    {
        if (qre_il_data_item_cur->type == QRE_IL_DATA_ITEM_TYPE_STR)
        {
            qre_il_data_item_str = qre_il_data_item_cur->ptr;
            if ((qre_il_data_item_str->str_len == str_len) &&
                    (qre_char_strncmp(qre_il_data_item_str->str, str, str_len) == 0))
            {
                matched_id = qre_il_data_item_cur->id;
                matched = TRUE;
                break;
            }
        }
        qre_il_data_item_cur = qre_il_data_item_cur->next;
    }

    if (matched == TRUE) 
    {
        *id_out = matched_id;
        return 0;
    }

    matched_id = qre_il_data_get_id(qre_il_data);

    new_qre_il_data_item_str = qre_il_data_item_str_new(str, str_len);
    if (new_qre_il_data_item_str == NULL) { ret = -QRE_ERR_OUT_OF_MEMORY; goto fail; }
    new_qre_il_data_item = qre_il_data_item_new(matched_id, QRE_IL_DATA_ITEM_TYPE_STR, new_qre_il_data_item_str);
    if (new_qre_il_data_item == NULL) { ret = -QRE_ERR_OUT_OF_MEMORY; goto fail; }
    new_qre_il_data_item_str = NULL;
    qre_il_data_append(qre_il_data, new_qre_il_data_item);
    new_qre_il_data_item_str = NULL;

    *id_out = matched_id;

    goto done;
fail:
    if (new_qre_il_data_item_str != NULL)  qre_il_data_item_str_destroy(new_qre_il_data_item_str);
    if (new_qre_il_data_item != NULL)  qre_il_data_item_destroy(new_qre_il_data_item);
done:
    return ret;
}

int qre_il_data_print(qre_il_data_t *qre_il_data)
{
    qre_il_data_item_t *qre_il_data_item_cur, *qre_il_data_item_next;
    uint32_t num = 0;
    
    qre_il_data_item_cur = qre_il_data->begin;
    while (qre_il_data_item_cur != NULL)
    {
        printf("%3u: ", (unsigned int)num);

        qre_il_data_item_next = qre_il_data_item_cur->next;
        qre_il_data_item_print(qre_il_data_item_cur);

        num++;
        qre_il_data_item_cur = qre_il_data_item_next;
    }

    return 0;
}

int qre_il_data_append(qre_il_data_t *qre_il_data, \
        qre_il_data_item_t *new_qre_il_data_item)
{
    if (qre_il_data->begin == NULL)
    {
        qre_il_data->begin = qre_il_data->end = new_qre_il_data_item;
    }
    else
    {
        qre_il_data->end->next = new_qre_il_data_item;
        qre_il_data->end = new_qre_il_data_item;
    }
    qre_il_data->size += 1;

    return 0;
}


/* .text */

qre_il_text_item_t *qre_il_text_item_new(uint32_t opcode, uint32_t operand)
{
    qre_il_text_item_t *new_qre_il_text_item = NULL;

    new_qre_il_text_item = (qre_il_text_item_t *)malloc(sizeof(qre_il_text_item_t));
    if (new_qre_il_text_item == NULL) { goto fail; } 
    new_qre_il_text_item->opcode = opcode;
    new_qre_il_text_item->operand = operand;
    new_qre_il_text_item->next = NULL;

    goto done;
fail:
    if (new_qre_il_text_item != NULL)
    {
        free(new_qre_il_text_item);
        new_qre_il_text_item = NULL;
    }
done:
    return new_qre_il_text_item;
}

int qre_il_text_item_destroy(qre_il_text_item_t *qre_il_text_item)
{
    free(qre_il_text_item);

    return 0;
}

int qre_il_text_item_print(qre_il_text_item_t *qre_il_text_item)
{
    const char *str;
    int str_len;
    int type;

    qre_vm_op_cvt_to_name(&str, qre_il_text_item->opcode);
    qre_vm_op_cvt_to_type(&type, qre_il_text_item->opcode);

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
            printf("%u ", (unsigned int)qre_il_text_item->operand);
            break;
        case QRE_VM_OP_OPERAND_TYPE_CHAR:
            printf("\'");
            qre_char_print_utf_8(qre_il_text_item->operand);
            printf("\'");
            break;
        case QRE_VM_OP_OPERAND_TYPE_NONE:
            break;
    }
    printf("\n");

    return 0;
}

qre_il_text_t *qre_il_text_new(void)
{
    qre_il_text_t *new_qre_il_text = NULL;

    new_qre_il_text = (qre_il_text_t *)malloc(sizeof(qre_il_text_t));
    if (new_qre_il_text == NULL) { goto fail; }
    new_qre_il_text->begin = new_qre_il_text->end = NULL;
    new_qre_il_text->size = 0;

    goto done;
fail:
    if (new_qre_il_text != NULL)
    {
        free(new_qre_il_text);
        new_qre_il_text = NULL;
    }
done:
    return new_qre_il_text;
}

int qre_il_text_destroy(qre_il_text_t *qre_il_text)
{
    qre_il_text_item_t *qre_il_text_item_cur, *qre_il_text_item_next;

    qre_il_text_item_cur = qre_il_text->begin;
    while (qre_il_text_item_cur != NULL)
    {
        qre_il_text_item_next = qre_il_text_item_cur->next;
        qre_il_text_item_destroy(qre_il_text_item_cur);
        qre_il_text_item_cur = qre_il_text_item_next;
    }

    free(qre_il_text);

    return 0;
}

int qre_il_text_print(qre_il_text_t *qre_il_text)
{
    qre_il_text_item_t *qre_il_text_item_cur, *qre_il_text_item_next;
    uint32_t num = 0;
    
    qre_il_text_item_cur = qre_il_text->begin;
    while (qre_il_text_item_cur != NULL)
    {
        printf("%3u: ", (unsigned int)num);

        qre_il_text_item_next = qre_il_text_item_cur->next;
        qre_il_text_item_print(qre_il_text_item_cur);

        num++;
        qre_il_text_item_cur = qre_il_text_item_next;
    }

    return 0;
}

int qre_il_text_append(qre_il_text_t *qre_il_text, \
        qre_il_text_item_t *new_qre_il_text_item)
{
    if (qre_il_text->begin == NULL)
    {
        qre_il_text->begin = qre_il_text->end = new_qre_il_text_item;
    }
    else
    {
        qre_il_text->end->next = new_qre_il_text_item;
        qre_il_text->end = new_qre_il_text_item;
    }
    qre_il_text->size += 1;

    return 0;
}

int qre_il_text_append_with_configure(qre_il_text_t *qre_il_text, \
        uint32_t opcode, uint32_t operand)
{
    int ret = 0;
    qre_il_text_item_t *new_qre_il_text_item = NULL;

    new_qre_il_text_item = qre_il_text_item_new(opcode, operand);
    if (new_qre_il_text_item == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail;
    }
    qre_il_text_append(qre_il_text, new_qre_il_text_item);
    new_qre_il_text_item = NULL;

    goto done;
fail:
    if (new_qre_il_text_item == NULL)
    {
        qre_il_text_item_destroy(new_qre_il_text_item);
    }
done:
    return ret;
}

uint32_t qre_il_text_get_pc(qre_il_text_t *qre_il_text)
{
    return qre_il_text->size;
}


/* IL */

qre_il_t *qre_il_new(void)
{
    qre_il_t *new_qre_il = NULL;
    
    new_qre_il = (qre_il_t *)malloc(sizeof(qre_il_t));
    if (new_qre_il == NULL) { goto fail; }
    new_qre_il->data = NULL;
    new_qre_il->text = NULL;

    new_qre_il->data = qre_il_data_new();
    if (new_qre_il->data == NULL) { goto fail; }
    new_qre_il->text = qre_il_text_new();
    if (new_qre_il->text == NULL) { goto fail; }

    goto done;
fail:
    if (new_qre_il != NULL)
    {
        qre_il_destroy(new_qre_il);
        new_qre_il = NULL;
    }
done:
    return new_qre_il;
}

int qre_il_destroy(qre_il_t *qre_il)
{
    if (qre_il->data != NULL) { qre_il_data_destroy(qre_il->data); }
    if (qre_il->text != NULL) { qre_il_text_destroy(qre_il->text); }
    free(qre_il);

    return 0;
}

int qre_il_print(qre_il_t *qre_il)
{
    printf(".data\n");
    qre_il_data_print(qre_il->data);
    printf(".text\n");
    qre_il_text_print(qre_il->text);
    printf(".pcinit %u\n", (unsigned int)qre_il->pc_entrance);

    return 0;
}


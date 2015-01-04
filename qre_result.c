/*
   Quick Regular Expression Engine : Result
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

#include "qre_str.h"
#include "qre_common.h"
#include "qre_result.h"
#include "qre_err.h"

/* Group */

qre_result_group_num_t *qre_result_group_num_new(int num)
{
    qre_result_group_num_t *new_qre_result_group_num = NULL;

    new_qre_result_group_num = (qre_result_group_num_t *)malloc(sizeof(qre_result_group_num_t));
    if (new_qre_result_group_num == NULL) { goto fail; }
    new_qre_result_group_num->num = num;

    goto done;
fail:
    if (new_qre_result_group_num != NULL)
    {
        free(new_qre_result_group_num);
        new_qre_result_group_num = NULL;
    }
done:
    return new_qre_result_group_num;
}

int qre_result_group_num_destroy(qre_result_group_num_t *qre_result_group_num)
{
    free(qre_result_group_num);
    
    return 0;
}

int qre_result_group_num_print(qre_result_group_num_t *qre_result_group_num)
{
    printf("%d", qre_result_group_num->num);
    
    return 0;
}


qre_result_group_lbl_t *qre_result_group_lbl_new(qre_char_t *str, size_t str_len)
{
    qre_result_group_lbl_t *new_qre_result_group_lbl = NULL;

    new_qre_result_group_lbl = (qre_result_group_lbl_t *)malloc(sizeof(qre_result_group_lbl_t));
    if (new_qre_result_group_lbl == NULL) { goto fail; }

    if (qre_string_new(&new_qre_result_group_lbl->str, str, str_len) != 0)
    { goto fail; }

    goto done;
fail:
    if (new_qre_result_group_lbl != NULL)
    {
        if (new_qre_result_group_lbl->str != NULL) qre_string_destroy(new_qre_result_group_lbl->str);
        free(new_qre_result_group_lbl);
        new_qre_result_group_lbl = NULL;
    }
done:
    return new_qre_result_group_lbl;
}

int qre_result_group_lbl_destroy(qre_result_group_lbl_t *qre_result_group_lbl)
{
    if (qre_result_group_lbl->str != NULL) qre_string_destroy(qre_result_group_lbl->str);
    free(qre_result_group_lbl);

    return 0;
}

int qre_result_group_lbl_print(qre_result_group_lbl_t *qre_result_group_lbl)
{
    if (qre_result_group_lbl->str != NULL) qre_str_print_utf_8(qre_result_group_lbl->str->body, qre_result_group_lbl->str->size);

    return 0;
}


qre_result_group_t *qre_result_group_new(int type, void *ptr)
{
    qre_result_group_t *new_qre_result_group = NULL;

    new_qre_result_group = (qre_result_group_t *)malloc(sizeof(qre_result_group_t));
    if (new_qre_result_group == NULL) { goto fail; }
    new_qre_result_group->type = type;
    new_qre_result_group->ptr = ptr;
    new_qre_result_group->start = 0;
    new_qre_result_group->length = 0;
    new_qre_result_group->next = NULL;

    goto done;
fail:
    if (new_qre_result_group != NULL)
    {
        free(new_qre_result_group);
        new_qre_result_group = NULL;
    }
done:
    return new_qre_result_group;
}

int qre_result_group_destroy(qre_result_group_t *qre_result_group)
{
    switch (qre_result_group->type)
    {
        case QRE_RESULT_GROUP_NUM:
            qre_result_group_num_destroy(qre_result_group->ptr);
            break;
        case QRE_RESULT_GROUP_LBL:
            qre_result_group_lbl_destroy(qre_result_group->ptr);
            break;
    }
    free(qre_result_group);

    return 0;
}

int qre_result_group_print(qre_result_group_t *qre_result_group)
{
    printf("group match "); 
    switch (qre_result_group->type)
    {
        case QRE_RESULT_GROUP_NUM:
            printf("by num #");
            qre_result_group_num_print(qre_result_group->ptr);
            break;
        case QRE_RESULT_GROUP_LBL:
            printf("by label \'"); 
            qre_result_group_lbl_print(qre_result_group->ptr);
            printf("\'"); 
            break;
    }
    printf(" from %d, at %d of length\n", qre_result_group->start, qre_result_group->length);

    return 0;
}

qre_result_group_list_t *qre_result_group_list_new(void)
{
    qre_result_group_list_t *new_qre_result_group_list = NULL;

    new_qre_result_group_list = (qre_result_group_list_t *)malloc(sizeof(qre_result_group_list_t));
    if (new_qre_result_group_list == NULL) { goto fail; }
    new_qre_result_group_list->begin = new_qre_result_group_list->end = NULL;

    goto done;
fail:
    if (new_qre_result_group_list != NULL)
    {
        free(new_qre_result_group_list);
        new_qre_result_group_list = NULL;
    }
done:
    return new_qre_result_group_list;

}

int qre_result_group_list_destroy(qre_result_group_list_t *qre_result_group_list)
{
    qre_result_group_t *qre_result_group_cur, *qre_result_group_next;

    qre_result_group_cur = qre_result_group_list->begin;
    while (qre_result_group_cur != NULL)
    {
        qre_result_group_next = qre_result_group_cur->next;
        qre_result_group_destroy(qre_result_group_cur);
        qre_result_group_cur = qre_result_group_next;
    }
    free(qre_result_group_list);

    return 0;
}

int qre_result_group_list_append(qre_result_group_list_t *qre_result_group_list, \
        qre_result_group_t *new_qre_result_group)
{
    if (qre_result_group_list->begin == NULL)
    {
        qre_result_group_list->begin = qre_result_group_list->end = new_qre_result_group;
    }
    else
    {
        qre_result_group_list->end->next = new_qre_result_group;
        qre_result_group_list->end = new_qre_result_group;
    }

    return 0;
}

int qre_result_group_list_print(qre_result_group_list_t *qre_result_group_list)
{
    qre_result_group_t *qre_result_group_cur;

    qre_result_group_cur = qre_result_group_list->begin;
    while (qre_result_group_cur != NULL)
    {
        qre_result_group_print(qre_result_group_cur);
        qre_result_group_cur = qre_result_group_cur->next;
    }

    return 0;
}

/* Result */

qre_result_t *qre_result_new(void)
{
    qre_result_t *new_qre_result = NULL;

    new_qre_result = (qre_result_t *)malloc(sizeof(qre_result_t));
    if (new_qre_result == NULL) { goto fail; }
    new_qre_result->success = 0;
    new_qre_result->start = 0;
    new_qre_result->length = 0;
    new_qre_result->groups = qre_result_group_list_new();
    if (new_qre_result->groups == NULL) { goto fail; }

    goto done;
fail:
    if (new_qre_result != NULL)
    {
        qre_result_destroy(new_qre_result);
        new_qre_result = NULL;
    }
done:
    return new_qre_result;
}


int qre_result_destroy(qre_result_t *qre_result)
{
    if (qre_result->groups != NULL)  qre_result_group_list_destroy(qre_result->groups);
    free(qre_result);

    return 0;
}

int qre_result_print(qre_result_t *qre_result)
{
    if (qre_result->success != FALSE)
    {
        printf("success: true\n");
        printf("global match from %d, at %d of length\n", qre_result->start, qre_result->length);
        qre_result_group_list_print(qre_result->groups);
    }
    else
    {
        printf("success: false\n");
    }

    return 0;
}

int qre_result_group_by_num( \
        qre_result_group_t **qre_result_group_out, \
        qre_result_t *qre_result, \
        int num)
{
    qre_result_group_t *qre_result_group_cur; 
    qre_result_group_num_t *qre_result_group_num;

    qre_result_group_cur = qre_result->groups->begin;
    while (qre_result_group_cur != NULL)
    {
        if (qre_result_group_cur->type == QRE_RESULT_GROUP_NUM)
        {
            qre_result_group_num = qre_result_group_cur->ptr;
            if (qre_result_group_num->num == num)
            {
                *qre_result_group_out = qre_result_group_cur;
                return 0;
            }
        }
        qre_result_group_cur = qre_result_group_cur->next; 
    }
    return -QRE_ERR_NOT_FOUND;
}

int qre_result_group_by_lbl( \
        qre_result_group_t **qre_result_group_out, \
        qre_result_t *qre_result, \
        qre_string_t *lbl)
{
    qre_result_group_t *qre_result_group_cur; 
    qre_result_group_lbl_t *qre_result_group_lbl;

    qre_result_group_cur = qre_result->groups->begin;
    while (qre_result_group_cur != NULL)
    {
        if (qre_result_group_cur->type == QRE_RESULT_GROUP_LBL)
        {
            qre_result_group_lbl = qre_result_group_cur->ptr;
            if (qre_string_eq(qre_result_group_lbl->str, lbl) == TRUE)
            {
                *qre_result_group_out = qre_result_group_cur;
                return 0;
            }
        }
        qre_result_group_cur = qre_result_group_cur->next; 
    }
    return -QRE_ERR_NOT_FOUND;
}


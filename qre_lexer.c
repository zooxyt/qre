/*
   Quick Regular Expression Engine : Lexer
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

#ifdef _MSC_VER
/* disable conditional expression is constant warning */
#pragma warning(disable:4127)
#endif


#include "qre_common.h"
#include "qre_token.h"
#include "qre_lexer.h"
#include "qre_err.h"

enum
{
    QRE_LEXER_STATE_INIT = 0,
    QRE_LEXER_STATE_ESC,
    QRE_LEXER_STATE_ESC_C,
    QRE_LEXER_STATE_ESC_X,
};

int qre_lexer_from_utf_8(qre_token_list_t **qre_token_list_out, const char *str, const size_t len)
{
    int ret = 0;
    const char *str_p = str, *str_endp = str_p + len;
    char ch_first;
    qre_char_t value; 
    int bytes_number = 0;
    qre_token_list_t *new_qre_token_list = NULL;
    qre_token_t *new_qre_token = NULL;
    int state = QRE_LEXER_STATE_INIT; 
    unsigned int num_arg = 0;

    *qre_token_list_out = NULL;

    if ((new_qre_token_list = qre_token_list_new()) == NULL)
    {
        ret = -QRE_ERR_OUT_OF_MEMORY;
        goto fail;
    }

    while (str_p != str_endp)
    {
        /* Extra Char */
        ch_first = *str_p;
        bytes_number = utf_8_get_unit_length(ch_first);
        QRE_EAT_FROM_UTF_8_CHECK_LENGTH(bytes_number);
        switch (bytes_number)
        {
            case 1:
                value = ch_first & 0x7f;
                QRE_EAT_FROM_UTF_8_NEXT(str_p);
                break;
            case 2:
                value = ch_first & 0x1f;
                QRE_EAT_FROM_UTF_8_NEXT(str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                break;
            case 3:
                value = ch_first & 0x1f;
                QRE_EAT_FROM_UTF_8_NEXT(str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                break;
            case 4:
                value = ch_first & 0x1f;
                QRE_EAT_FROM_UTF_8_NEXT(str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                break;
            case 5:
                value = ch_first & 0x1f;
                QRE_EAT_FROM_UTF_8_NEXT(str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                break;
            case 6:
                value = ch_first & 0x1f;
                QRE_EAT_FROM_UTF_8_NEXT(str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                QRE_EAT_FROM_UTF_8_READ_FOLLOW(value, str_p);
                break;
            default:
                { ret = -QRE_ERR_CORRUPT; goto fail; }
        }
retry:

        switch (state)
        {
            case QRE_LEXER_STATE_INIT:
                switch (value)
                {
                    case '\\':
                        state = QRE_LEXER_STATE_ESC;
                        break;
                    case '(': case '|': case ')':
                    case '[': case ']':
                    case '{': case '}': case ',': case '?': case '*': case '+':
                        if ((new_qre_token = qre_token_new(QRE_TOKEN_CONTROL, value)) == NULL)
                        {
                            ret = -QRE_ERR_OUT_OF_MEMORY;
                            goto fail;
                        }
                        qre_token_list_append(new_qre_token_list, new_qre_token);
                        break;
                    default:
                        if ((new_qre_token = qre_token_new(QRE_TOKEN_CHAR, value)) == NULL)
                        {
                            ret = -QRE_ERR_OUT_OF_MEMORY;
                            goto fail;
                        }
                        qre_token_list_append(new_qre_token_list, new_qre_token);
                        break;
                }
                break;

            case QRE_LEXER_STATE_ESC:
                switch (value)
                {
                    case 'c':
                        state = QRE_LEXER_STATE_ESC_C;
                        break;
                    case 'x':
                    case 'u':
                        state = QRE_LEXER_STATE_ESC_X;
                        num_arg = 0;
                        break;
                    case 'd': case 'D':
                    case 'f': case 'n': case 'r':
                    case 's': case 'S':
                    case 't': case 'v':
                    case 'w': case 'W':
                        switch (value)
                        {
                            case 'd': value = QRE_TOKEN_SPECIAL_DIGIT; break;
                            case 'D': value = QRE_TOKEN_SPECIAL_NONDIGIT; break; 
                            case 'f': value = QRE_TOKEN_SPECIAL_FORM_FEED; break;
                            case 'n': value = QRE_TOKEN_SPECIAL_NEWLINE; break;
                            case 'r': value = QRE_TOKEN_SPECIAL_CARRIAGE; break;
                            case 's': value = QRE_TOKEN_SPECIAL_WHITESPACE; break;
                            case 'S': value = QRE_TOKEN_SPECIAL_NONWHITESPACE; break;
                            case 't': value = QRE_TOKEN_SPECIAL_TAB; break;
                            case 'v': value = QRE_TOKEN_SPECIAL_VERTICAL_TAB; break;
                            case 'w': value = QRE_TOKEN_SPECIAL_WORD; break;
                            case 'W': value = QRE_TOKEN_SPECIAL_NONWORD; break;
                        }
                        if ((new_qre_token = qre_token_new(QRE_TOKEN_SPECIAL, value)) == NULL)
                        {
                            ret = -QRE_ERR_OUT_OF_MEMORY;
                            goto fail;
                        }
                        qre_token_list_append(new_qre_token_list, new_qre_token);
                        state = QRE_LEXER_STATE_INIT;
                        break;
                    default:
                        if ((new_qre_token = qre_token_new(QRE_TOKEN_CHAR, value)) == NULL)
                        {
                            ret = -QRE_ERR_OUT_OF_MEMORY;
                            goto fail;
                        }
                        qre_token_list_append(new_qre_token_list, new_qre_token);
                        state = QRE_LEXER_STATE_INIT;
                        break;
                }

                break;

            case QRE_LEXER_STATE_ESC_C:
                /* Control Chars */
                /* http://en.wikipedia.org/wiki/Control_character */
                if (('a' <= value) && (value <= 'z')) { value = value - 'a'; }
                else if (('A' <= value) && (value <= 'Z')) { value = value - 'a'; }
                else if (value == '[') { value = 27; }
                else if (value == '\\') { value = 28; }
                else if (value == ']') { value = 29; }
                else if (value == '^') { value = 30; }
                else if (value == '_') { value = 31; }

                /* Append new char */
                if ((new_qre_token = qre_token_new(QRE_TOKEN_CHAR, value)) == NULL)
                {
                    ret = -QRE_ERR_OUT_OF_MEMORY;
                    goto fail;
                }
                qre_token_list_append(new_qre_token_list, new_qre_token);
                state = QRE_LEXER_STATE_INIT;
                break;

            case QRE_LEXER_STATE_ESC_X:
                if (('0' <= value) && (value <= '9'))
                {
                    num_arg = num_arg * 16 + (value - '0');
                }
                else if (('a' <= value) && (value <= 'f'))
                {
                    num_arg = num_arg * 16 + ((value - 'a') + 10);
                }
                else if (('A' <= value) && (value <= 'F'))
                {
                    num_arg = num_arg * 16 + ((value - 'A') + 10);
                }
                else
                {
                    if ((new_qre_token = qre_token_new(QRE_TOKEN_CHAR, num_arg)) == NULL)
                    {
                        ret = -QRE_ERR_OUT_OF_MEMORY;
                        goto fail;
                    }
                    qre_token_list_append(new_qre_token_list, new_qre_token);
                    state = QRE_LEXER_STATE_INIT;
                    goto retry;
                }

                if (str_p == str_endp)
                {
                    if ((new_qre_token = qre_token_new(QRE_TOKEN_CHAR, num_arg)) == NULL)
                    {
                        ret = -QRE_ERR_OUT_OF_MEMORY;
                        goto fail;
                    }
                    qre_token_list_append(new_qre_token_list, new_qre_token);
                    state = QRE_LEXER_STATE_INIT;
                }
                break;

            default:
                break;
        }

    }

    *qre_token_list_out = new_qre_token_list;

    goto done;
fail:
    if (new_qre_token_list != NULL) qre_token_list_destroy(new_qre_token_list);
    if (new_qre_token != NULL) qre_token_destroy(new_qre_token);
done:
    return ret;
}


/*
   Quick Regular Expression Engine 
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
#include <stdio.h>
#include <string.h>

#include "qre_char.h"
#include "qre_str.h"
#include "qre_token.h"
#include "qre_ast.h"
#include "qre_nfa.h"
#include "qre_il.h"

#include "qre_lexer.h"
#include "qre_parser.h"
#include "qre_convert.h"
#include "qre_compiler.h"

#include "qre_vm.h"

#include "qre.h"

/* Compile a regular expression */
int qre_compile(qre_t **qre_out, char *str_regex)
{
    int ret = 0;
    qre_t *new_qre = NULL;
    size_t str_regex_len = strlen(str_regex);

    (void)str_regex;

    if ((new_qre = (qre_t *)malloc(sizeof(qre_t))) == NULL)
    { goto fail; }
    new_qre->token_list = NULL;
    new_qre->ast = NULL;
    new_qre->nfa_state_table = NULL;
    new_qre->il = NULL;

    /* Lexeme Scan */
    ret = qre_lexer_from_utf_8(&new_qre->token_list, str_regex, str_regex_len);
    if (ret != 0) { goto fail; }

    /* Parsing (Tokens->AST) */
    ret = qre_parse(&new_qre->ast, new_qre->token_list);
    if (ret != 0) { goto fail; }

    /* Convering (AST->NFA) */
    ret = qre_convert_to_nfa(&new_qre->nfa_state_table, new_qre->ast);
    if (ret != 0) { goto fail; }

    /* Compiling (NFA->IL) */
    ret = qre_compile_from_nfa_to_il(&new_qre->il, new_qre->nfa_state_table);
    if (ret != 0) { goto fail; }

    *qre_out = new_qre;

    goto done;
fail:
    if (new_qre != NULL)
    {
        qre_destroy(new_qre);
        new_qre = NULL;
    }
done:
    return ret;
}

/* Destroy a regex */
int qre_destroy(qre_t *re)
{
    if (re->token_list != NULL) qre_token_list_destroy(re->token_list);
    if (re->ast != NULL) qre_ast_destroy(re->ast);
    if (re->nfa_state_table != NULL) qre_nfa_state_table_destroy(re->nfa_state_table);
    if (re->il != NULL) qre_il_destroy(re->il);
    free(re);

    return 0;
}


/* Print debug info */
int qre_debug(qre_t *qre)
{
    printf("Tokens:\n");
    qre_token_list_print(qre->token_list);
    printf("\n");

    printf("State Table:\n");
    qre_nfa_state_table_print(qre->nfa_state_table);
    printf("\n");

    printf("IL:\n");
    qre_il_print(qre->il);
    printf("\n");

    return 0;
}


/* Match */
int qre_match(qre_t *qre, qre_result_t **result_out, char *str_haystack)
{
    int ret = 0;
    qre_vm_t *new_qre_vm = NULL;
    qre_result_t *new_qre_result = NULL;
    qre_string_t *new_qre_string = NULL;
    size_t str_haystack_len = strlen(str_haystack);

    ret = qre_string_new_from_utf_8(&new_qre_string, str_haystack, str_haystack_len);
    if (ret != 0) { goto fail; }

    ret = qre_vm_new_from_il(&new_qre_vm, qre->il, new_qre_string);
    if (ret != 0) { goto fail; }

    ret = qre_vm_run(&new_qre_result, new_qre_vm);
    if (ret != 0) { goto fail; }

    *result_out = new_qre_result;

    goto done;
fail:
    if (new_qre_result != NULL) { qre_result_destroy(new_qre_result); }
done:
    if (new_qre_vm != NULL) { qre_vm_destroy(new_qre_vm); }
    if (new_qre_string != NULL) { qre_string_destroy(new_qre_string); }
    return ret;
}


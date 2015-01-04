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

#ifndef _QRE_H_
#define _QRE_H_

#include <stdint.h>

#include "qre_char.h"
#include "qre_str.h"
#include "qre_token.h"
#include "qre_ast.h"
#include "qre_nfa.h"
#include "qre_il.h"
#include "qre_result.h"

/* The main structure for containing a regex */
struct qre
{
    qre_token_list_t *token_list;
    qre_ast_t *ast;
    qre_nfa_state_table_t *nfa_state_table;
    qre_il_t *il;
};
typedef struct qre qre_t;

/* Compile a regex from a string */
int qre_compile(qre_t **qre_out, char *str_regex);

/* Destroy a regex */
int qre_destroy(qre_t *re);

/* Match */
int qre_match(qre_t *qre, qre_result_t **result_out, char *str_haystack);

/* Print debug info */
int qre_debug(qre_t *re);

#endif


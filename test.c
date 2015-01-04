/*
   Quick Regular Expression Engine : Test Program
   Copyright (c) 2013-2015 Cheryl Natsu 
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

#include <stdio.h>
#include <stdlib.h>

#include "argsparse.h"

#include "qre.h"


static int show_version(void)
{
    const char *info = ""
        "Quick Regular Expression Demo Program\n"
        "Copyright (c) 2013-2015 Cheryl Natsu\n";
    puts(info);
    return 0;
}

static int show_help(void)
{
    const char *info = ""
        "Usage : qre <testname> <arguments>\n"
        "\n"
        "analysis  <pattern>\n"
        "match     <pattern> <haystack>\n"
        "";
    show_version();
    puts(info);
    return 0;
}


static int test_match(char *pattern, char *haystack)
{
    int ret = 0;
    qre_t *new_qre = NULL;
    qre_result_t *new_qre_result = NULL;

    /* Compile the regexp */
    ret = qre_compile(&new_qre, pattern);
    if (ret != 0) goto fail;

    /* Match */
    ret = qre_match(new_qre, &new_qre_result, haystack);
    if (ret != 0) goto fail;

    /* Print the result */
    qre_result_print(new_qre_result);

fail:
    if (new_qre != NULL) { qre_destroy(new_qre); }
    if (new_qre_result != NULL) { qre_result_destroy(new_qre_result); }
    return ret;
}

static int test_analysis(char *pattern)
{
    int ret = 0;
    qre_t *new_qre = NULL;

    /* Compile the regexp */
    ret = qre_compile(&new_qre, pattern);
    if (ret != 0) goto fail;

    /* Analysis */
    ret = qre_debug(new_qre);
    if (ret != 0) goto fail;

fail:
    if (new_qre != NULL) { qre_destroy(new_qre); }
    return ret;
}


int main(int argc, char *argv[])
{
    int ret = 0;
    argsparse_t argsparse;
    char *pattern, *haystack;

    argsparse_init(&argsparse, (int)argc, (char **)argv);

    if (argsparse_available(&argsparse) == 0)
    { show_help(); goto done; }

    if (argsparse_match_str(&argsparse, "--version"))
    { show_version(); goto done; }
    else if (argsparse_match_str(&argsparse, "--help"))
    { show_help(); goto done; }
    else if (argsparse_match_str(&argsparse, "match"))
    { 
        argsparse_next(&argsparse);
        if (argsparse_available_count(&argsparse, 2) == 0)
        { show_help(); goto done; }
        else
        { 
            pattern = argsparse_fetch(&argsparse); 
            argsparse_next(&argsparse);
            haystack = argsparse_fetch(&argsparse); 
            ret = test_match(pattern, haystack);
            if (ret != 0) goto fail;
        }
    }
    else if (argsparse_match_str(&argsparse, "analysis"))
    { 
        argsparse_next(&argsparse);
        if (argsparse_available_count(&argsparse, 1) == 0)
        { show_help(); goto done; }
        else
        { 
            pattern = argsparse_fetch(&argsparse); 
            ret = test_analysis(pattern);
            if (ret != 0) goto fail;
        }
    }
    else
    { show_help(); goto done; }

    goto done;
fail:
    if (ret != 0)
    {
        fprintf(stderr, "Error: %d\n", ret);
    }
done:
    return ret;
}


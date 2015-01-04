/*
   Quick Regular Expression Engine : VM : Operations
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

#include "qre_vm_op.h"

struct qre_vm_op_cvt_tbl
{
    const char *str;
    const int type;
};

struct qre_vm_op_cvt_tbl qre_vm_op_cvt_tbl[] = 
{
    { "nop"     , QRE_VM_OP_OPERAND_TYPE_RAW }  ,
    { "succ"    , QRE_VM_OP_OPERAND_TYPE_NONE } ,
    { "fail"    , QRE_VM_OP_OPERAND_TYPE_NONE } ,
    { "me"      , QRE_VM_OP_OPERAND_TYPE_CHAR } ,
    { "ml"      , QRE_VM_OP_OPERAND_TYPE_CHAR } ,
    { "mg"      , QRE_VM_OP_OPERAND_TYPE_CHAR } ,
    { "ma"      , QRE_VM_OP_OPERAND_TYPE_NONE } ,
    { "msd"     , QRE_VM_OP_OPERAND_TYPE_NONE } ,
    { "msnd"    , QRE_VM_OP_OPERAND_TYPE_NONE } ,
    { "msf"     , QRE_VM_OP_OPERAND_TYPE_NONE } ,
    { "msnl"    , QRE_VM_OP_OPERAND_TYPE_NONE } ,
    { "mscr"    , QRE_VM_OP_OPERAND_TYPE_NONE } ,
    { "mss"     , QRE_VM_OP_OPERAND_TYPE_NONE } ,
    { "msns"    , QRE_VM_OP_OPERAND_TYPE_NONE } ,
    { "mst"     , QRE_VM_OP_OPERAND_TYPE_NONE } ,
    { "msv"     , QRE_VM_OP_OPERAND_TYPE_NONE } ,
    { "msw"     , QRE_VM_OP_OPERAND_TYPE_NONE } ,
    { "msnw"    , QRE_VM_OP_OPERAND_TYPE_NONE } ,
    { "test"    , QRE_VM_OP_OPERAND_TYPE_NONE } ,
    { "pass"    , QRE_VM_OP_OPERAND_TYPE_NONE } ,
    { "jmp"     , QRE_VM_OP_OPERAND_TYPE_RAW }  ,
    { "jmpt"    , QRE_VM_OP_OPERAND_TYPE_RAW }  ,
    { "jmpf"    , QRE_VM_OP_OPERAND_TYPE_RAW }  ,
    { "call"    , QRE_VM_OP_OPERAND_TYPE_RAW }  ,
    { "minnum"  , QRE_VM_OP_OPERAND_TYPE_RAW }  ,
    { "maxnum"  , QRE_VM_OP_OPERAND_TYPE_RAW }  ,
    { "maxinf"  , QRE_VM_OP_OPERAND_TYPE_NONE } ,
    { "repbp"   , QRE_VM_OP_OPERAND_TYPE_NONE } ,
    { "repbpn"  , QRE_VM_OP_OPERAND_TYPE_NONE } ,
    { "reptest" , QRE_VM_OP_OPERAND_TYPE_RAW }  ,
    { "repjmp"  , QRE_VM_OP_OPERAND_TYPE_RAW }  ,
    { "capnbeg" , QRE_VM_OP_OPERAND_TYPE_RAW }  ,
    { "caplbeg" , QRE_VM_OP_OPERAND_TYPE_RAW }  ,
    { "capnend" , QRE_VM_OP_OPERAND_TYPE_RAW }  ,
    { "caplend" , QRE_VM_OP_OPERAND_TYPE_RAW }  ,
};

int qre_vm_op_cvt_to_name(const char **str_out, uint32_t opcode)
{
    *str_out = qre_vm_op_cvt_tbl[opcode].str;

    return 0;
}

int qre_vm_op_cvt_to_type(int *type_out, uint32_t opcode)
{
    *type_out = qre_vm_op_cvt_tbl[opcode].type;

    return 0;
}


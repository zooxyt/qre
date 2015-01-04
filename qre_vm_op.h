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

#ifndef _QRE_VM_OP_H_
#define _QRE_VM_OP_H_

enum
{
    QRE_OP_NOP = 0, /* Do nothing */
    QRE_OP_SUCC,    /* Success */
    QRE_OP_FAIL,    /* Fail */
    QRE_OP_ME,      /* Match equal */
    QRE_OP_ML,      /* Match less */
    QRE_OP_MG,      /* Match greater */
    QRE_OP_MA,      /* Match any (set test result to true) */
    QRE_OP_MSD,     /* Match digit */
    QRE_OP_MSND,    /* Match nondigit */
    QRE_OP_MSF,     /* Match form-feed */
    QRE_OP_MSNL,    /* Match newline */
    QRE_OP_MSCR,    /* Match carriage return */
    QRE_OP_MSS,     /* Match whitespace */
    QRE_OP_MSNS,    /* Match non-whitespace */
    QRE_OP_MST,     /* Match tab */
    QRE_OP_MSV,     /* Match vertical tab */
    QRE_OP_MSW,     /* Match word */
    QRE_OP_MSNW,    /* Match nonword */
    QRE_OP_TEST,    /* Test, if false, goto to previous stack frame */
    QRE_OP_PASS,    /* Pass */
    QRE_OP_JMP,     /* Jump */
    QRE_OP_JMPT,    /* Jump if true */
    QRE_OP_JMPF,    /* Jump if false */
    QRE_OP_CALL,    /* Call */
    QRE_OP_MINNUM,  /* Min Number */
    QRE_OP_MAXNUM,  /* Max Number */
    QRE_OP_MAXINF,  /* Max Infinite */
    QRE_OP_REPBP,   /* Repeat Break Point */
    QRE_OP_REPBPN,  /* Repeat Break Point (Non-Greedy) */
    QRE_OP_REPTEST, /* Repeat Test */
    QRE_OP_REPJMP,  /* Repeat Jump */
    QRE_OP_CAPNBEG, /* Start Capturing Number */
    QRE_OP_CAPLBEG, /* Start Capturing Label */
    QRE_OP_CAPNEND,  /* End Capturing Number */
    QRE_OP_CAPLEND,  /* End Capturing Label */
};

#define QRE_VM_OP_OPERAND_TYPE_RAW 0
#define QRE_VM_OP_OPERAND_TYPE_CHAR 1
#define QRE_VM_OP_OPERAND_TYPE_NONE 2

int qre_vm_op_cvt_to_name(const char **str_out, uint32_t opcode);
int qre_vm_op_cvt_to_type(int *type_out, uint32_t opcode);

#endif


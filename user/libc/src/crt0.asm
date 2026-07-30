;
; SPDX-License-Identifier: GPL-3.0-or-later
;
; Copyright (c) 2026 sulfurLabs
;
; PROJECT: sulfurOS
; FILE: printf.c
;

[BITS 64]

global _start
extern main
extern _exit

_start:
    xor rbp, rbp
    and rsp, -16
    call main

    mov edi, eax
    call _exit

.hang:
    cli
    hlt
    jmp .hang
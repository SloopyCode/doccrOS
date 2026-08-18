;
; SPDX-License-Identifier: GPL-3.0-or-later
;
; Copyright (c) 2026 sulfurLabs
;
; PROJECT: sulfurOS
; FILE: crt0.asm
;

[BITS 64]

global _start
global __sig_trampoline

extern main
extern _exit

_start:
    xor rbp, rbp
    and rsp, -16

    mov rax, 36
    lea rdi, [rel __sig_trampoline]
    xor rsi, rsi
    xor rdx, rdx
    xor r10, r10
    xor r8, r8
    syscall

    call main

    mov edi, eax
    call _exit

.hang:
    cli
    hlt
    jmp .hang

__sig_trampoline:
    pop rdi
    mov rax, 35
    syscall

.hang_sigreturn:
    hlt
    jmp .hang_sigreturn
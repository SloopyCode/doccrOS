;
; SPDX-License-Identifier: GPL-3.0-or-later
;
; Copyright (c) 2026 sulfurLabs
;
; PROJECT: sulfurOS
; FILE: idt.asm
;
;

[BITS 64]

global idt_flush

; void idt_flush(u64 idt_ptr)
idt_flush:
    lidt [rdi]      ; IDT from RDI
    ret

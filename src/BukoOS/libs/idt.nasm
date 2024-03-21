[BITS 64]
section .text
extern BukoOS_idt_exception_handler
global _bukoos_idt_exception_division
global idt_sprocious_int

align   0x08,   db  0x00
idt_sprocious_int:
   iretq
align	0x08,	db	0x00
_bukoos_idt_exception_division:
   push 0
   push 0
   jmp _bukoos_idt_exception_base


align	0x08,	db	0x00
_bukoos_idt_exception_base:
    push	rax
	push	rbx
	push	rcx
	push	rdx
	push	rsi
	push	rdi
	push	rbp
	push	r8
	push	r9
	push	r10
	push	r11
	push	r12
	push	r13
	push	r14
	push	r15
	mov	rax, cr2
	push	rax
    cld
    mov rdi, rsp
    call BukoOS_idt_exception_handler
    add rsp, 8
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	pop	r10
	pop	r9
	pop	r8
	pop	rbp
	pop	rdi
	pop	rsi
	pop	rdx
	pop	rcx
	pop	rbx
	pop	rax
    add rsp, 0x10
    iretq

[BITS 64]
section .text
extern _driver_ps2_keyboard
global _base_driver_ps2_keyboard



align	0x08,	db	0x00
_base_driver_ps2_keyboard:
   cli
   push rax
   push rbx
   push rcx
   push rdx
   push rsi
   push rdi
   push rbp
   push r8
   push r9
   push r10
   push r11
   push r12
   push r13
   push r14
   push r15
   call _driver_ps2_keyboard
   pop r15
   pop r14
   pop r13
   pop r12
   pop r11
   pop r10
   pop r9
   pop r8
   pop rbp
   pop rdi
   pop rsi
   pop rdx
   pop rcx
   pop rbx
   pop rax
	   sti
	   iretq
	   
	
	
	
	
	
	
	
	
	
	
	

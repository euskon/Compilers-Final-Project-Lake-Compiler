.text
.globl _start
_start:
	callq fun_main
	movq $60, %rax
	movq $0, %rdi
	syscall

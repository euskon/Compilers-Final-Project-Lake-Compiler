.data
gbl_x:
	.quad 0
gbl_myBool:
	.quad 0
str_0:
	.asciz "poo"
.align 8
fun_foo:
subq $8, %rsp
movq %rbp, (%rsp)
movq %rsp, %rbp
addq $16, %rbp
subq $0, %rsp
movq (str_0), %rdi
callq printString
lbl_0: 
addq $0, %rsp
movq (%rsp), %rbp
addq $8, %rsp
retq
fun_main:
subq $8, %rsp
movq %rbp, (%rsp)
movq %rsp, %rbp
addq $16, %rbp
subq $8, %rsp
callq foo
movq $5, %rax
movq %rax, (gbl_x)
movq $1, %rax
movq %rax, -16(%rbp)
lbl_1: 
movq $60, %rax
movq $0, %rdi
syscall

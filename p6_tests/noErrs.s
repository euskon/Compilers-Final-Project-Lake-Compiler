.data
.align 8
fun_main:
subq $8, %rsp
movq %rbp, (%rsp)
movq %rsp, %rbp
addq $16, %rbp
subq $8, %rsp
movq $4, %rax
movq %rax, -16(%rbp)
movq -16(%rbp), %rdi
callq printInt
lbl_0: 
movq $60, %rax
movq $0, %rdi
syscall

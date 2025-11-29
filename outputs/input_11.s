.data
print_fmt_num: .string "%ld \n"
print_fmt_float: .string "%f\n"
print_fmt_str: .string "%s\n"
.text
.global main
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $32, %rsp
 movl $10, %eax
 movl %eax, -8(%rbp)
 movabsq $4626463454704697344, %rax
 movq %rax, -16(%rbp)
 movl $30, %eax
 movl %eax, -24(%rbp)
 movl -8(%rbp), %eax
 cvtsi2sdq %rax, %xmm0
 movq %xmm0, %rax
 pushq %rax
 movq -16(%rbp), %rax
 movq %rax, %xmm1
 popq %rax
 movq %rax, %xmm0
 addsd %xmm1, %xmm0
 movq %xmm0, %rax
 pushq %rax
 movl -24(%rbp), %eax
 cvtsi2sdq %rax, %xmm0
 movq %xmm0, %rax
 movq %rax, %xmm1
 popq %rax
 movq %rax, %xmm0
 addsd %xmm1, %xmm0
 movq %xmm0, %rax
 movq %rax, %xmm0
 leaq print_fmt_float(%rip), %rdi
 movl $1, %eax
 call printf@PLT
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits

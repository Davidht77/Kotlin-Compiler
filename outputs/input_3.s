.data
print_fmt_num: .string "%ld \n"
print_fmt_str: .string "%s\n"
.text
.global main
.globl sumarDos
sumarDos:
 pushq %rbp
 movq %rsp, %rbp
 movq %rdi,-8(%rbp)
 subq $128, %rsp
 movq -8(%rbp), %rax
 pushq %rax
 movq $2, %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 leave
 ret
.end_sumarDos:
leave
ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $128, %rsp
 movq $7, %rax
 movq %rax, -8(%rbp)
 movq -8(%rbp), %rax
 mov %rax, %rdi
 movl $0, %eax
call sumarDos
 movq %rax, -16(%rbp)
 movq -16(%rbp), %rax
 movq %rax, %rsi
 leaq print_fmt_num(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits

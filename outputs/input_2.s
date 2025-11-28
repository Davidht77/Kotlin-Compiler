.data
print_fmt_num: .string "%ld \n"
print_fmt_float: .string "%f\n"
print_fmt_str: .string "%s\n"
a: .quad 5
b: .quad 5
.text
.global main
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $0, %rsp
 movl a(%rip), %eax
 pushq %rax
 movl b(%rip), %eax
 movq %rax, %rcx
 popq %rax
 addl %ecx, %eax
 movslq %eax, %rsi
 leaq print_fmt_num(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits

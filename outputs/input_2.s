.data
print_fmt_num: .string "%ld \n"
print_fmt_str: .string "%s\n"
a: .quad 5
b: .quad 5
.text
.global main
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $128, %rsp
 movq a(%rip), %rax
 pushq %rax
 movq b(%rip), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 movq %rax, %rsi
 leaq print_fmt_num(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits

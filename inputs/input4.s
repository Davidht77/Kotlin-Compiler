.data
print_fmt_num: .string "%ld \n"
print_fmt_str: .string "%s\n"
MAX: .quad 10
count: .quad 0
.text
.global main
.globl factorial
factorial:
 pushq %rbp
 movq %rsp, %rbp
 movq %rdi,-8(%rbp)
 subq $128, %rsp
 movq -8(%rbp), %rax
 pushq %rax
 movq $1, %rax
 movq %rax, %rcx
 popq %rax
 cmpq %rcx, %rax
 movl $0, %eax
 setle %al
 movzbq %al, %rax
 cmpq $0, %rax
 je else_0
 movq $1, %rax
 jmp .end_factorial
 jmp endif_0
else_0:
endif_0:
 movq -8(%rbp), %rax
 pushq %rax
 movq -8(%rbp), %rax
 pushq %rax
 movq $1, %rax
 movq %rax, %rcx
 popq %rax
 subq %rcx, %rax
 mov %rax, %rdi
 movl $0, %eax
call factorial
 movq %rax, %rcx
 popq %rax
 imulq %rcx, %rax
 jmp .end_factorial
.end_factorial:
leave
ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $128, %rsp
 movq $5, %rax
 mov %rax, %rdi
 movl $0, %eax
call factorial
 movq %rax, -8(%rbp)
 movq -8(%rbp), %rax
 movq %rax, %rsi
 leaq print_fmt_num(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq 0(%rbp), %rax
 pushq %rax
 movq $2, %rax
 movq %rax, %rcx
 popq %rax
 imulq %rcx, %rax
 movq %rax, %rsi
 leaq print_fmt_num(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits

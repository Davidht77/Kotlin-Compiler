.data
print_fmt_num: .string "%ld \n"
print_fmt_str: .string "%s\n"
.text
.global main
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $128, %rsp
 leaq str_0(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt_str(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq $1, %rax
 movq %rax, -8(%rbp)
 movq $5, %rax
 movq %rax, -16(%rbp)
 movq $1, %rax
 movq %rax, -24(%rbp)
 movq -8(%rbp), %rax
 movq %rax, -32(%rbp)
loop_0:
 movq -32(%rbp), %rax
 movq -16(%rbp), %rcx
 cmpq %rcx, %rax
 jg endloop_0
 movq -32(%rbp), %rax
 movq %rax, %rsi
 leaq print_fmt_num(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -32(%rbp), %rax
 movq -24(%rbp), %rcx
 addq %rcx, %rax
 movq %rax, -32(%rbp)
 jmp loop_0
endloop_0:
 leaq str_1(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt_str(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq $1, %rax
 movq %rax, -8(%rbp)
 movq $10, %rax
 movq %rax, -16(%rbp)
 movq $2, %rax
 movq %rax, -24(%rbp)
 movq -8(%rbp), %rax
 movq %rax, -32(%rbp)
loop_1:
 movq -32(%rbp), %rax
 movq -16(%rbp), %rcx
 cmpq %rcx, %rax
 jg endloop_1
 movq -32(%rbp), %rax
 movq %rax, %rsi
 leaq print_fmt_num(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -32(%rbp), %rax
 movq -24(%rbp), %rcx
 addq %rcx, %rax
 movq %rax, -32(%rbp)
 jmp loop_1
endloop_1:
 leaq str_2(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt_str(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq $5, %rax
 movq %rax, -8(%rbp)
 movq $1, %rax
 movq %rax, -16(%rbp)
 movq $1, %rax
 movq %rax, -24(%rbp)
 movq -8(%rbp), %rax
 movq %rax, -32(%rbp)
loop_2:
 movq -32(%rbp), %rax
 movq -16(%rbp), %rcx
 cmpq %rcx, %rax
 jl endloop_2
 movq -32(%rbp), %rax
 movq %rax, %rsi
 leaq print_fmt_num(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -32(%rbp), %rax
 movq -24(%rbp), %rcx
 subq %rcx, %rax
 movq %rax, -32(%rbp)
 jmp loop_2
endloop_2:
 leaq str_3(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt_str(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq $10, %rax
 movq %rax, -8(%rbp)
 movq $1, %rax
 movq %rax, -16(%rbp)
 movq $2, %rax
 movq %rax, -24(%rbp)
 movq -8(%rbp), %rax
 movq %rax, -32(%rbp)
loop_3:
 movq -32(%rbp), %rax
 movq -16(%rbp), %rcx
 cmpq %rcx, %rax
 jl endloop_3
 movq -32(%rbp), %rax
 movq %rax, %rsi
 leaq print_fmt_num(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -32(%rbp), %rax
 movq -24(%rbp), %rcx
 subq %rcx, %rax
 movq %rax, -32(%rbp)
 jmp loop_3
endloop_3:
.end_main:
leave
ret
.data
str_3: .string "Test 4: 10 downTo 1 step 2"
str_2: .string "Test 3: 5 downTo 1"
str_1: .string "Test 2: 1..10 step 2"
str_0: .string "Test 1: 1..5"
.section .note.GNU-stack,"",@progbits

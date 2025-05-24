section .text
extern my_func

global _start

_start:
    push rbp
    mov rbp, rsp

    ; Allocating result at [rbp-8]
    sub rsp, 8

    mov rax, 20
    push rax

    mov rax, 10
    push rax

    pop rdi
    pop rsi
    call my_func

    mov rax, rax
    mov [rbp-8], rax

    ; Exit program with the value of the last variable as exit code
    mov rdi, [rbp-8]
    mov rsp, rbp
    pop rbp
    mov rax, 60
    syscall

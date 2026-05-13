.section .data
path:
    .asciz "/dev/mem"
ok:
    .ascii "ok\n"
ok_len = . - ok
nao:
    .ascii "nao\n"
nao_len = . - nao

.equ BASE,    0xFF200000
.equ SPAN,    0x1000
.equ DATA_IN,    0x00000000
.equ SIGNALS,    0x00000010
.equ DATA_OUT,   0x00000020

.section .bss
fd_val: .skip 4

.section .text
.global main

main:
    // int open(const char *pathname, int flags, mode_t mode);
    ldr r0, =path
    mov r1, #2
    mov r7, #5
    svc #0
    cmp r0, #0
    blt erro
    ldr r1, =fd_val
    str r0, [r1]

    // void *mmap2(void *addr, size_t length, int prot, int flags, int fd, off_t pgoffset)
    mov r0, #0              
    mov r1, #SPAN        
    mov r2, #3              
    mov r3, #1              
    ldr r4, =fd_val
    ldr r4, [r4]            
    ldr r5, =0xFF200        
    mov r7, #192            
    svc #0
    cmn r0, #1
    beq erro
    mov r9, r0

    // bit 2 de signal = 1, assim dando reset, dps desce o valor
    mov r1, #4
    str r1, [r9, #SIGNALS]
    mov r1, #0
    str r1, [r9, #SIGNALS]

    /*
    mov r1, #1
    str r1, [r9, #SIGNALS]
    mov r1, #0
    str r1, [r9, #SIGNALS]
     */

    // ssize_t write(int fd, const void *buf, size_t count);
    mov r0, #1
    ldr r1, =ok
    mov r2, #ok_len
    mov r7, #4
    svc #0
    b done

erro:
    // ssize_t write(int fd, const void *buf, size_t count);
    mov r0, #1
    ldr r1, =nao
    mov r2, #nao_len
    mov r7, #4
    svc #0

done:
    // exit(int status);
    mov r7, #1
    mov r0, #0
    svc #0

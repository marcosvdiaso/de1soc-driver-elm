.section .data
path:
    .asciz "/dev/mem"
ok_msg:
    .ascii "ok\n"
ok_len = . - ok_msg
fail_msg:
    .ascii "fail\n"
fail_len = . - fail_msg

.equ LW_BASE,    0xFF200000
.equ LW_SPAN,    0x1000
.equ DATA_IN,    0x00000000
.equ SIGNALS,    0x00000010
.equ DATA_OUT,   0x00000020

.section .bss
fd_val: .skip 4

.section .text
.global main

main:
    @ open("/dev/mem", O_RDWR|O_SYNC)
    ldr r0, =path
    mov r1, #2
    mov r7, #5
    svc #0

    cmp r0, #0
    blt fail

    @ salva fd
    ldr r1, =fd_val
    str r0, [r1]

    @ mmap(NULL, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0xFF200000>>12)
    mov r0, #0              @ NULL
    mov r1, #LW_SPAN        @ tamanho
    mov r2, #3              @ PROT_READ|PROT_WRITE
    mov r3, #1              @ MAP_SHARED
    ldr r4, =fd_val
    ldr r4, [r4]            @ fd
    ldr r5, =0xFF200        @ offset em páginas
    mov r7, #192            @ mmap2
    svc #0

    @ verifica MAP_FAILED (-1)
    cmn r0, #1
    beq fail

    @ salva ponteiro virtual base
    mov r9, r0

    @ envia RST ao CoProcessor (signals[2]=1 → writedata=0b100=4)
    mov r1, #4
    str r1, [r9, #SIGNALS]

    @ desce RST (signals=0)
    mov r1, #0
    str r1, [r9, #SIGNALS]

    @ imprime ok
    mov r0, #1
    ldr r1, =ok_msg
    mov r2, #ok_len
    mov r7, #4
    svc #0
    b done

fail:
    mov r0, #1
    ldr r1, =fail_msg
    mov r2, #fail_len
    mov r7, #4
    svc #0

done:
    mov r7, #1
    mov r0, #0
    svc #0

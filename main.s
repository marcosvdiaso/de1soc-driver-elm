.include "fibonacci.s"

.text
.global main

main:
    LDR r2, =0xFF200040
    LDR r3, =0xFF200000

loop:
    LDR r0, [r2]
    BL  fibonacci
    STR r0, [r3]

    B loop
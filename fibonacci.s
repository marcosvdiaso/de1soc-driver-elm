.section .data

@ reserva bytes
@ ou seja, array de 1023 ints, todos com -1 (Exceto os 2 primeiros), que é a flag de não salvo
memo:   .word 0, 1
        .fill 1021, 4, -1    

.section .text
.global fibonacci

@ fibonacci(x)->  pra inicializar memo e chamar pd
fibonacci:
    PUSH    {r4, lr} @ salva r4 (usado p/ guardar x) e lr (end de retorno) na pilha
    MOV     r4, r0 @ salva x em r0, onde será realizada a operação
    LDR     r1, =memo @ carrega o endereço base de memo em r1

    MOV     r0, r4              
    BL      pd @ chama pd(x) resultado fica em r0

    POP     {r4, pc} @ retorna o valor de r4 e o endereço de quem chamou a função

@ pd(x)-> fibonacci recursivo
pd:
    PUSH    {r4, r5, lr} @r4 salva o x atual, r5 salva o array e lr o retorno
    MOV     r4, r0
    LDR     r5, =memo

    LDR     r1, [r5, r4, LSL #2] @ r1 = endbase + (x*4)
    CMN     r1, #1 
    BNE     .return @ compara r1 com -1, se nao for igual, chama return

    SUB     r0, r4, #1
    BL      pd
    PUSH {r0}

    SUB     r0, r4, #2
    BL      pd @ chama a função recursiva pra x-2
    POP {r1}

    ADD     r0, r0, r1 @ r0 = pd(x-1) + pd(x-2)
    STR     r0, [r5, r4, LSL #2] @ salva o valor em memo[x]

    POP     {r4, r5, pc} @ retorna

.return:
    MOV     r0, r1 @ passa o memo[x] que tava em r1 pro registrador de retorno
    POP     {r4, r5, pc} @ retorna os valores
    
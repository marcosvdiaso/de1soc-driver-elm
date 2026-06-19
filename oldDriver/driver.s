/*
-------------------------------------------------------------------------------
ENDS. REGISTRADORES:
  * BASE -> 0xFF200000
  * OFFSETS:
    * DATA_IN  -> 0x0000
    * SIGNALS  -> 0x0010
    * DATA_OUT -> 0x0020

AAPCS:
  * r0-r3  -> args/retorno
  * r4-r7 -> callee-saved
  * r9     -> mmap pointer

BITS DATA_OUT:
  * 4 -> DONE
	* 5 -> BUSY
	* 6 -> ERROR

OPCODES INSTS.:
	* 000 -> STORE_IMG
	* 001 -> STORE_WEIGHTS_ADDR
	* 010 -> STORE_WEIGHTS_VAL
	* 011 -> STORE_BIAS
	* 100 -> STORE_BETA
	* 101 -> START
	* 110 -> STATUS
	* 111 -> NOP

TEMPO DE PULSO PARA SIGNALS:
	* 50MHz (FPGA)
	* 800MHz (HPS)
	* /= 16, mas vou assumir 32 para margem de segurança
	* https://www.manualslib.com/manual/1485906/Terasic-De1-Soc.html

image: .incbin "archives/images/image.bin"
-------------------------------------------------------------------------------
*/

.section .data

path_devmem:
    .asciz "/dev/mem"

.section .bss
fd_val: .skip 4      

.equ SPAN,    0x1000
.equ DATA_IN,  0x00
.equ SIGNALS,  0x10
.equ DATA_OUT, 0x20

.equ BIT_DONE,  4
.equ BIT_BUSY,  5
.equ BIT_ERROR, 6

.equ OP_STORE_IMG,          0b000
.equ OP_STORE_WEIGHTS_ADDR, 0b001
.equ OP_STORE_WEIGHTS_VAL,  0b010
.equ OP_STORE_BIAS,         0b011
.equ OP_STORE_BETA,         0b100
.equ OP_START,              0b101
.equ OP_STATUS,             0b110
.equ OP_NOP,                0b111
.equ PULSE_WAIT, 32

.section .rodata
W_in: .incbin "archives/W_in.bin"
bbin:    .incbin "archives/b.bin"
beta: .incbin "archives/beta.bin"

.section .text

.global elm_open
.global elm_close
.global elm_reset
.global elm_store_img
.global elm_store_bias
.global elm_store_beta
.global elm_store_weights
.global elm_start
.global elm_result
.global elm_load
.global elm_mmap

/*
elm_load:
	-> Responsável por carregar os arquivos de pesos, bias e betas
 */

elm_load:
    push {r4, lr}

    ldr  r0, =bbin
    bl   elm_store_bias
    cmp  r0, #0
    blt  .elm_load_err

    ldr  r0, =beta
    bl   elm_store_beta
    cmp  r0, #0
    blt  .elm_load_err

    ldr  r0, =W_in
    bl   elm_store_weights
    cmp  r0, #0
    blt  .elm_load_err

    mov  r0, #0
    pop  {r4, pc}

.elm_load_err:
    mvn  r0, #0
    pop  {r4, pc}

/*
elm_open:
	-> Responsável por mapear os endereços virtuais
	-> Salvar mmap em r9

Retorno:
	-> r0: 0 se ok, -1 se erro
 */
elm_open:
    push {r4, r5, r7, lr}

    ldr  r0, =path_devmem
    mov  r1, #2
    mov  r7, #5
    svc  #0
    cmp  r0, #0
    blt  .elm_open_err
    ldr  r1, =fd_val
    str  r0, [r1]

    mov  r4, r0
    mov  r0, #0
    mov  r1, #SPAN
    mov  r2, #3
    mov  r3, #1
    ldr  r5, =0xFF200
    mov  r7, #192
    svc  #0

    cmn  r0, #1
    beq  .elm_open_err

    mov  r9, r0
    mov  r0, #0
    pop  {r4, r5, r7, pc}

.elm_open_err:
    mvn  r0, #0
    pop  {r4, r5, r7, pc}


/*
elm_close:
	* rpimeiro fecha o mmap e depois fecha o file descriptor
 */
elm_close:
    push {r7, lr}

    mov  r0, r9
    mov  r1, #SPAN
    mov  r7, #215
    svc  #0

    ldr  r0, =fd_val
    ldr  r0, [r0]
    mov  r7, #6
    svc  #0

    pop  {r7, pc}

/*
elm_reset:
	-> Sobe 0b100 em SINGALS, a fim de dar reset
 */
elm_reset:
    push {r1, r2, r7, lr}
    mov  r1, #4
    str  r1, [r9, #SIGNALS]

    mov  r7, #PULSE_WAIT
1:  subs r7, r7, #1
    bne  1b

    mov  r1, #0
    str  r1, [r9, #SIGNALS]
    pop  {r1, r2, r7, pc}

/*
send_instruction:
	* Responsável por enviar as instruções
	* Primeiro envia para DATA_IN a instrução guardada no r2
	* Após isso sobe enable, e desce quando busy sobe
	* r3 segura handhsake
 */
send_instruction:
    str  r2, [r9, #DATA_IN]

    mov  r1, #1
    str  r1, [r9, #SIGNALS]

.wb_high:
    ldr  r3, [r9, #DATA_OUT]
    tst  r3, #(1 << BIT_BUSY)
    beq  .wb_high

    mov  r1, #0
    str  r1, [r9, #SIGNALS]

.wb_low:
    ldr  r3, [r9, #DATA_OUT]
    tst  r3, #(1 << BIT_BUSY)
    bne  .wb_low

    bx   lr

/*
build_instruction:
	* Responsável por montar as instruções
	* Recebe opcode, endereço e valor, além dos shifts necessários com base na instrução
 */
build_instruction:
    push {r4, r5, lr}

    mov  r5, r0

    lsl  r1, r1, r3
    lsl  r2, r2, r4
    orr  r2, r2, r1
    orr  r2, r2, r5

    bl   send_instruction

    pop  {r4, r5, pc}

/*
elm_store_img:
	* Envia os 784pxs em loop
 */
elm_store_img:
    push {r4, r5, r6, r7, lr}

    mov  r6, r0
    mov  r5, #0

.si_loop:
    ldrb r2, [r6], #1

    mov  r0, #OP_STORE_IMG
    mov  r1, r5
    mov  r3, #3
    mov  r4, #13
    bl   build_instruction

    tst  r3, #(1 << BIT_ERROR)
    bne  .si_err

    add  r5, r5, #1
    cmp  r5, #784
    blt  .si_loop

    mov  r0, #0
    pop  {r4, r5, r6, r7, pc}

.si_err:
    mvn  r0, #0
    pop  {r4, r5, r6, r7, pc}


/*
elm_store_bias:
	* Envia os 128 bias em loop
 */
elm_store_bias:
    push {r4, r5, r6, r7, lr}

    mov  r6, r0
    mov  r5, #0

.ss_loop:
    ldrh r2, [r6], #2

    mov  r0, #OP_STORE_BIAS
    mov  r1, r5
    mov  r3, #3
    mov  r4, #10
    bl   build_instruction

    tst  r3, #(1 << BIT_ERROR)
    bne  .ss_err

    add  r5, r5, #1
    cmp  r5, #128
    blt  .ss_loop

    mov  r0, #0
    pop  {r4, r5, r6, r7, pc}

.ss_err:
    mvn  r0, #0
    pop  {r4, r5, r6, r7, pc}


/*
elm_store_beta:
	* Envia os 1280 betas por loop
 */
elm_store_beta:
    push {r4, r5, r6, r7, lr}

    mov  r6, r0
    mov  r5, #0

.sb_loop:
    ldrh r2, [r6], #2

    mov  r0, #OP_STORE_BETA
    mov  r1, r5
    mov  r3, #3
    mov  r4, #14
    bl   build_instruction

    tst  r3, #(1 << BIT_ERROR)
    bne  .sb_err

    add  r5, r5, #1
    cmp  r5, #1280
    blt  .sb_loop

    mov  r0, #0
    pop  {r4, r5, r6, r7, pc}

.sb_err:
    mvn  r0, #0
    pop  {r4, r5, r6, r7, pc}


/*
elm_store_weights:
	* Ambas as instruções de pesos juntas
	* Primeiro ele envia o endereço e depois ele já envia o valor, com dois branchs pra build_instruction
	* como em send já tem o handshake com espera de busy, é feito sequencialmente end -> val em um único loop
 */
elm_store_weights:
    push {r4, r5, r6, r7, lr}

    mov  r6, r0
    mov  r5, #0

.sw_loop:
    ldrh r7, [r6], #2

    mov  r0, #OP_STORE_WEIGHTS_ADDR
    mov  r1, r5
    mov  r2, #0
    mov  r3, #3
    mov  r4, #0
    bl   build_instruction

    tst  r3, #(1 << BIT_ERROR)
    bne  .sw_err

    mov  r0, #OP_STORE_WEIGHTS_VAL
    mov  r1, #0
    mov  r2, r7
    mov  r3, #0
    mov  r4, #3
    bl   build_instruction

    tst  r3, #(1 << BIT_ERROR)
    bne  .sw_err

    add  r5, r5, #1
    cmp  r5, #100352
    blt  .sw_loop

    mov  r0, #0
    pop  {r4, r5, r6, r7, pc}

.sw_err:
    mvn  r0, #0
    pop  {r4, r5, r6, r7, pc}

/*
elm_start:
	* Pulsa clear e então inicia a inferência
	* Depois fica verificando se terá erro ou done
 */
elm_start:
    push {r4, r5, r7, lr}

    bl   elm_store_img
    cmp  r0, #0
    blt  .elm_load_err

    mov  r1, #2
    str  r1, [r9, #SIGNALS]

    mov  r7, #PULSE_WAIT
3:  subs r7, r7, #1
    bne  3b

    mov  r1, #0
    str  r1, [r9, #SIGNALS]

    mov  r0, #OP_START
    mov  r1, #0
    mov  r2, #0
    mov  r3, #0
    mov  r4, #0
    bl   build_instruction

.es_poll:
    ldr  r3, [r9, #DATA_OUT]

    tst  r3, #(1 << BIT_ERROR)
    bne  .es_err

    tst  r3, #(1 << BIT_DONE)
    beq  .es_poll

    mov  r0, r3
    pop  {r4, r5, r7, pc}

.es_err:
    mvn  r0, #0
    pop  {r4, r5, r7, pc}


/*
elm_result:
	* Pega o digito predito em DATA_OUT, ou erro
	* Não é necessário uma instrução de status pelo fato de que DATA_OUT é atualizado e deixado explicito frequentemente
 */
elm_result:
    ldr  r3, [r9, #DATA_OUT]

    tst  r3, #(1 << BIT_ERROR)
    bne  .er_err

    and  r0, r3, #0xF
    bx   lr

.er_err:
    mvn  r0, #0
    bx   lr

/* */
elm_mmap:
    mov r0, r9
    bx lr
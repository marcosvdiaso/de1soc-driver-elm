/*
1 - > open e reset
2 - > fluxo de imagens
3 - > fluxo de bias
4 - > fluxo de betas
5 - > fluxo de weights
6 - > fluxo completo
7 - > loop 10x
 */

.equ TEST, 6

.section .data
msg_open_ok: .ascii "Open funcionando\n"
msg_open_ok_len = . - msg_open_ok
msg_open_err: .ascii "Open ruim\n"
msg_open_err_len = . - msg_open_err

msg_reset_ok: .ascii "Reset funcionando\n"
msg_reset_ok_len = . - msg_reset_ok

msg_img_ok: .ascii "Imagens guardadas\n"
msg_img_ok_len  = . - msg_img_ok
msg_img_err: .ascii "Imagens naõ guardadas\n"
msg_img_err_len = . - msg_img_err

msg_bias_ok: .ascii "Bias guardados\n"
msg_bias_ok_len = . - msg_bias_ok
msg_bias_err: .ascii "Bias não guardados\n"
msg_bias_err_len = . - msg_bias_err

msg_beta_ok: .ascii "Betas guardadosK\n"
msg_beta_ok_len = . - msg_beta_ok
msg_beta_err: .ascii "Sobrou nada pro beta\n"
msg_beta_err_len = . - msg_beta_err

msg_wei_ok: .ascii "Pesos guardados\n"
msg_wei_ok_len  = . - msg_wei_ok
msg_wei_err: .ascii "Pesos não guardadosO\n"
msg_wei_err_len = . - msg_wei_err

msg_start_ok: .ascii "Start ok\n"
msg_start_ok_len = . - msg_start_ok
msg_start_err: .ascii "Start com erro\n"
msg_start_err_len = . - msg_start_err

msg_loop: .ascii "Loop ok\n"
msg_loop_len = . - msg_loop

msg_resultado: .ascii "Resultado: "
msg_resultado_len = . - msg_resultado
newline: .ascii "\n"

.section .rodata
.if TEST >= 2
image: .incbin "image.bin"
.endif

.if TEST >= 3
bbin:     .incbin "b.bin"
.endif

.if TEST >= 4
beta:  .incbin "beta.bin"
.endif

.if TEST >= 5
W_in:  .incbin "W_in.bin"
.endif

.section .text
.global main

main:
		.if TEST >= 7
		mov r8, #0
		.endif 

    push {r4, lr}

    bl   elm_open
    cmp  r0, #0
    blt  .err_open

    mov  r0, #1
    ldr  r1, =msg_open_ok
    mov  r2, #msg_open_ok_len
    mov  r7, #4
    svc  #0

    bl   elm_reset

    mov  r0, #1
    ldr  r1, =msg_reset_ok
    mov  r2, #msg_reset_ok_len
    mov  r7, #4
    svc  #0

.if TEST == 1
    b .main_done
.endif

.if TEST >= 2
		ldr  r0, =image
		bl   elm_store_img
		cmp  r0, #0
		blt  .err_img

		mov  r0, #1
		ldr  r1, =msg_img_ok
		mov  r2, #msg_img_ok_len
		mov  r7, #4
		svc  #0
.endif

.if TEST == 2
		b .main_done
.endif

.if TEST >= 3
		ldr  r0, =bbin
		bl   elm_store_bias
		cmp  r0, #0
		blt  .err_bias

		mov  r0, #1
		ldr  r1, =msg_bias_ok
		mov  r2, #msg_bias_ok_len
		mov  r7, #4
		svc  #0
.endif

.if TEST == 3
		b .main_done
.endif

.if TEST >= 4
		ldr  r0, =beta
		bl   elm_store_beta
		cmp  r0, #0
		blt  .err_beta

		mov  r0, #1
		ldr  r1, =msg_beta_ok
		mov  r2, #msg_beta_ok_len
		mov  r7, #4
		svc  #0
.endif

.if TEST == 4
		b .main_done
.endif

.if TEST >= 5
		ldr  r0, =W_in
		bl   elm_store_weights
		cmp  r0, #0
		blt  .err_wei

		mov  r0, #1
		ldr  r1, =msg_wei_ok
		mov  r2, #msg_wei_ok_len
		mov  r7, #4
		svc  #0
.endif

.if TEST == 5
		b .main_done
.endif

.loop_main:
	.if TEST >= 6
			bl   elm_start
			cmp  r0, #0
			blt  .err_start

			mov  r0, #1
			ldr  r1, =msg_start_ok
			mov  r2, #msg_start_ok_len
			mov  r7, #4
			svc  #0

			bl   elm_result
			cmp  r0, #0
			blt  .err_start

			mov  r4, r0                     

			mov  r0, #1
			ldr  r1, =msg_resultado
			mov  r2, #msg_resultado_len
			mov  r7, #4
			svc  #0

			add  r4, r4, #'0'
			sub  sp, sp, #8
			strb r4, [sp]
			mov  r0, #1
			mov  r1, sp
			mov  r2, #1
			mov  r7, #4
			svc  #0
			add  sp, sp, #8

			mov  r0, #1
			ldr  r1, =newline
			mov  r2, #1
			mov  r7, #4
			svc  #0
	.endif

			b .main_done

.err_open:
    mov  r0, #1
    ldr  r1, =msg_open_err
    mov  r2, #msg_open_err_len
    mov  r7, #4
    svc  #0
    b .main_done

.err_img:
    mov  r0, #1
    ldr  r1, =msg_img_err
    mov  r2, #msg_img_err_len
    mov  r7, #4
    svc  #0
    b .main_done

.err_bias:
    mov  r0, #1
    ldr  r1, =msg_bias_err
    mov  r2, #msg_bias_err_len
    mov  r7, #4
    svc  #0
    b .main_done

.err_beta:
    mov  r0, #1
    ldr  r1, =msg_beta_err
    mov  r2, #msg_beta_err_len
    mov  r7, #4
    svc  #0
    b .main_done

.err_wei:
    mov  r0, #1
    ldr  r1, =msg_wei_err
    mov  r2, #msg_wei_err_len
    mov  r7, #4
    svc  #0
    b .main_done

.err_start:
    mov  r0, #1
    ldr  r1, =msg_start_err
    mov  r2, #msg_start_err_len
    mov  r7, #4
    svc  #0

.main_done:
		.if TEST >= 7
		add r8, r8, #1
		cmp r8, #10
		blt .loop_main
		mov  r0, #1
		ldr  r1, =msg_loop
		mov  r2, #msg_loop_len
		mov  r7, #4
		svc  #0
		.endif 

    bl elm_close

    mov  r7, #1
    mov  r0, #0
    svc  #0
    
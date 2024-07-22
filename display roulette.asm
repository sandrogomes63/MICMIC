
; Tr1_Func4_23102023.asm
;
; Created: 23/10/2023 20:58:25
; Author :	Sandro Gomes



	.include <m128def.inc>
	
/////defini��o de variaveis em posi��es
	.def temp = r16
	.def cnt_int = r17
	.def temp_int = r18
	.def dado = r25
	.def cnt = r19
	.def cnt_piscar = r21
	
/////defini�oes de varoaveis em nomes
	.equ timerConfig = 0b00001101
	.equ off = 0xFF
	
	.cseg
	.org 0					;vector reset
		jmp main			;saltar para main

	.org 0x001E
		jmp int_timer0

	.cseg					;deixa espa�o para vectores de interrup��o
	.org 0x0046

digits:
	.db  0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82 ,0xF8

init:
		ldi temp,0xff
		out DDRC,temp		;defini��o das sa�das porto C display
		out PORTC,temp
		
/////defeni�oes entrada do switches
	ldi temp,0b11000000
	out DDRD,temp			;defini��o entradas
	out PORTD,temp			;ativa resistencias de pullups das entradas para os switches n�o utilizados, valor logico 1
		
	;defeni��o ocr0 para 249
	ldi temp,249			;OCR0 = 249
	out OCR0,temp

	ldi temp,0b00001101		;TC0 em modo CTC, prescaler 128, OC0 off
	out TCCR0,temp
		
	in temp,TIMSK			;Enable da interrup�ao do TC0 (CTC)
	ori temp,0b00000010
	out TIMSK,temp

	sei						;Enable global das interrup�oes

	ldi cnt,10				;contador de 10 vezes para o temporizador de 2ms
	ldi cnt_int,10			;contador de interrup��o = 10
	ldi dado,0				;valor dado = 0, posi��o 0 do "vetor"
	ldi cnt_piscar,10		;contador piscar 10 vezes 0,5s liga/desliga = 1Hz

	ret

main:						
	ldi temp,0xff			;inic stack, dividir os 16bits do endere�o em dois endere�os de 8bits e colocar na stack
	out spl,temp			;LOW
	ldi temp,0x10
	out sph,temp			;HIGH

	call init

ciclo:
	sbic PIND,0				;ciclo � espera de SW1 pressionado
	rjmp ciclo

display: 
	ldi temp,0xF9		
	out PORTC,temp			;display = 1, ap�s SW1 pressionado
	ldi temp,timerConfig
	out TCCR0,temp			;inicar timer para incrementa��o do valor do display

display_cont:
	sbis PIND,5				;ver se bot�o sw6 precionado
	rjmp piscar				;se SW6 pressionado, salta para label piscar, sen�o continua o c�digo
    brtc display_cont		;salta se flag t ativa
    		
	inc dado				;incrementa 1 ao valor de dado
	cpi dado,7				;compara se valor de dado = 7
	breq dado1				;se dado = 7, ativa flag XXX , salta para a label dado1 para colocar o valor de dado novamente a 1

	call updatedisp			;ap�s dado=1, atualiza o valor de display de 1 a 6
	clt						;faz clear a flag t
	rjmp display_cont

/////Atualiza o valor dado = 0, para reiniciar a contagem de 1 a 6
Dado1:
	ldi dado,0				;dado = 0
	clt						;clear flag t
	rjmp display_cont

/////Coloca o digito a piscar ap�s SW6 pressionado
piscar:
	in r26,pinc				;guarda o valor do display em r26, ou seja, guarda o digito quando SW6 � pressionado	
	ldi cnt_int,250			;contadores para conseguir 1Hz
	ldi cnt,250
/////Desliga o display de estiver ligado
ligado:
	brtc ligado				;salta se flag t ativa
	clt
	ldi temp,off
	out PORTC,temp
/////Liga o display se estiver desligado
desligado:
	brtc desligado			;salta se flag t ativa
	clt
	out portc,r26			;coloca o digito previamente guardado em r26 no display
	
	dec cnt_piscar			;decrementa o contador piscar para se conseguir 5+5 ciclos de liga/desliga
	brne ligado

	out portc,r26
	rjmp ciclo


/////Interrup��es
int_timer0:
	in temp_int,SREG
	dec cnt_int
	brne end_timer0
	mov cnt_int,cnt
	out SREG,temp_int
	set
	reti

end_timer0:
	out SREG,temp_int
	reti

/////Atualiza��o do display, valor de dado
updatedisp:
	ldi zh, high(digits<<1)	;bits higher
	ldi zl, low(digits<<1)	;bits low
	ldi r20,0				;coloca r20=0		
	add zl,dado				;bits low 
	adc zh,r20				;
	lpm temp,z
	out PORTC,temp
	ret	


; Tr1_Func4_23102023.asm
;
; Created: 23/10/2023 20:58:25
; Author :	Sandro Gomes



	.include <m128def.inc>
	
/////definição de variaveis em posições
	.def temp = r16
	.def cnt_int = r17
	.def temp_int = r18
	.def cnt = r19
	.def cnt_piscar = r21
	.def display1 = r25
	.def display2 = r24
	
	
	
/////definiçoes de varoaveis em nomes
	.equ timerConfig = 0b00001101
	.equ off = 0xFF
	
	.cseg
	.org 0					;vector reset
		jmp main			;saltar para main

	.org 0x001E
		jmp int_timer0

	.cseg					;deixa espaço para vectores de interrupção
	.org 0x0046

digits:
	.db  0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82 ,0xF8

init:
	ldi temp,0xff
	out DDRC,temp		;definição das saídas porto C display
	out PORTC,temp
		
/////defeniçoes entrada do switches
	ldi temp,0b11000000
	out DDRD,temp			;definição entradas
	out PORTD,temp			;ativa resistencias de pullups das entradas para os switches não utilizados, valor logico 1
		
	;defenição ocr0 para 249
	ldi temp,249			;OCR0 = 249
	out OCR0,temp

	ldi temp,0b00001101		;TC0 em modo CTC, prescaler 128, OC0 off
	out TCCR0,temp
		
	in temp,TIMSK			;Enable da interrupçao do TC0 (CTC)
	ori temp,0b00000010
	out TIMSK,temp

	sei						;Enable global das interrupçoes

	ldi cnt,10				;contador de 10 vezes para o temporizador de 2ms
	ldi cnt_int,10			;contador de interrupção = 10
	ldi display1,0			;valor dado = 0, posição 0 do "vetor"
	ldi display2,8
	ldi cnt_piscar,10		;contador piscar 10 vezes 0,5s liga/desliga = 1Hz

	ret

main:						
	ldi temp,0xff			;inic stack, dividir os 16bits do endereço em dois endereços de 8bits e colocar na stack
	out spl,temp			;LOW
	ldi temp,0x10
	out sph,temp			;HIGH

	call init

ciclo:
	sbic PIND,0				;ciclo à espera de SW1 pressionado
	rjmp ciclo

display: 
	ldi temp,0xF9		
	out PORTC,temp			;display = 1, após SW1 pressionado
	sbi PORTD,6
	ldi temp,0x82
	out PORTD,temp
	ldi temp,timerConfig
	out TCCR0,temp			;inicar timer para incrementação do valor do display

display_cont:
	;sbis PIND,5				;ver se botão sw6 precionado
	;rjmp piscar				;se SW6 pressionado, salta para label piscar, senão continua o código
    brtc display_cont		;salta se flag t ativa
    		
	inc display1			;incrementa 1 ao valor de display 1
	dec display2			;decrementa display 2
	cpi display1,7				;compara se valor de dado = 7
	breq dado1				;se dado = 7, ativa flag XXX , salta para a label dado1 para colocar o valor de dado novamente a 1

	call updatedisp			;após dado=1, atualiza o valor de display de 1 a 6
	clt						;faz clear a flag t
	rjmp display_cont

/////Atualiza o valor dado = 0, para reiniciar a contagem de 1 a 6
Dado1:
	ldi display1,0				;dado = 0
	ldi display2,6
	clt						;clear flag t
	rjmp display_cont

/////Coloca o digito a piscar após SW6 pressionado
piscar:
	in r27,pinc				;guarda o valor do display em r26, ou seja, guarda o digito quando SW6 é pressionado	
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
	out portc,r27			;coloca o digito previamente guardado em r26 no display
	
	dec cnt_piscar			;decrementa o contador piscar para se conseguir 5+5 ciclos de liga/desliga
	brne ligado

	out portc,r27
	rjmp ciclo


/////Interrupções
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

/////Atualização do display, valor de dado
updatedisp:
	ldi zh, high(digits<<1)	;bits higher
	ldi zl, low(digits<<1)	;bits low
	ldi temp,0				;coloca r20=0		
	add zl,display1				;bits low 
	adc zh,temp				;para não mexer em bit mais sigificativo
	lpm temp,z
	cbi PORTD,6
	out PORTC,temp

	ldi zh, high(digits<<1)	;bits higher
	ldi zl, low(digits<<1)	;bits low
	ldi temp,0				;coloca r20=0		
	add zl,display2			;bits low 
	adc zh,temp				;para não mexer em bit mais sigificativo
	lpm temp,z
	sbi PORTD,6
	out PORTC,temp

	ret	

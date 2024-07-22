;
; Tr1_Func3_09102023.asm
;
; Created: 09/10/2023 
; Authors :
;	Sandro Gomes	1221717
;

//VALORES PARA ROTINA DE DELAY:
	//Para 1ms:
	.equ x1ms = 238
	.equ y1ms = 20
	.equ z1ms = 6

//TABELA DISPLAY
	.equ n0 = 0xC0
	.equ n1 = 0xF9
	.equ n2 = 0xA4
	.equ n3 = 0xB0
	.equ n4 = 0x99
	.equ n5 = 0x92
	.equ n6 = 0x82
	.equ n7 = 0xF8
	.equ n8 = 0x80
	.equ n9 = 0x90

//Registo de display 7 segementos
	.def valorDisplay = r22

	.include <m128def.inc>

	.cseg
	.org 0				;vector reset
	jmp main			;saltar para main

	.cseg				;deixa espaço para vectores de interrupção
	.org 0x46

;
init:
		ldi r16,0xff
		out DDRC,r16		;definição das saídas porto C display
		ldi r16,n9
		out PORTC,r16		;coloca display a 9

		ldi r16,0xff
		out DDRA,r16		;definição das saídas porto A leds
		out PORTA,r16

		ldi r16,0b11000000
		out DDRD,r16		;definição entradas
		ldi r16,0b11000000	
		out PORTD,r16		;ativa resistencias de pullups das entradas para os switches não utilizados, valor logico 1
		
		ldi valorDisplay,9
		ret

main:						;inic stack
		ldi r16,0xff		;dividir os 16bits do endereço em dois endereços de 8bits e coloca na stack
		out spl,r16
		ldi r16,0x10
		out sph,r16

		call init

ciclo:						;ciclo leitura SW1 e SW6
		
		sbis PIND,0			;se SW1 do porto D = 0, executa a proxima linha, SW1 pressionado, faz rjmp
		rjmp cicloentrada	
		sbis PIND,5			;se SW6  do porto D = 0, executa a proxima linha, SW6 pressionado, faz rjmp
		rjmp ciclosaida

		rjmp ciclo

cicloentrada:
		call timer
		sbic PIND,0			;2a leitura do SW1 pressionado
		rjmp ciclo
		sbic PIND,5			;1a leitura do SW6 pressionado
		rjmp ciclo
		call timer
		sbic PIND,5			;2a leitura do SW6 pressionado
		rjmp ciclo

entrada_sw1_N:				;leitura da subida estado logico SW1, SW1 deixa de estar pressionado
		in r16,PIND
		cpi r16, 0b11011111
		brne entrada_sw1_N

entrada_sw6_N:				;leitura da subida estado logico SW6, SW6 deixa de estar pressionado
		in r16,PIND
		cpi r16, 0b11111111
		brne entrada_sw6_N
/// Atualiza o valor do display
		cpi valorDisplay,0
		breq ciclo
		dec valorDisplay
		cbi	PORTA,6			;???????????????????
		call comparar
		cpi valorDisplay,0
		breq trancaPorta	;sala cheia, tranca porta
		rjmp ciclo
			
ciclosaida:
		call timer
		sbic PIND,5			;2a leitura do SW6 pressionado
		rjmp ciclo
		sbic PIND,0			;1a leitura do SW1 pressionado
		rjmp ciclo
		call timer
		sbic PIND,0			;2a leitura do SW1 pressionado
		rjmp ciclo
saida_sw6_N:
		in r16,PIND
		cpi r16, 0b11111110	;leitura da subida estado logico SW6, SW6 deixa de estar pressionado
		brne saida_sw6_N
saida_sw1_N:
		in r16,PIND
		cpi r16, 0b11111111	;leitura da subida estado logico SW1, SW1 deixa de estar pressionado
		brne saida_sw6_N
/// Atualiza o valor do display
		cpi valorDisplay,9
		breq ciclo
		inc valorDisplay
		sbi	PORTA,7			;??????????????????????
		call comparar
		cpi valorDisplay,9
		breq desligarLuz	;sala vazia, desliga luz
		rjmp ciclo
		
desligarLuz:
		sbi	PORTA,6
		rjmp ciclo
trancaPorta:
		cbi PORTA,7
		rjmp ciclo
		

comparar:
		cpi valorDisplay,9
		breq display9
		cpi valorDisplay,8
		breq display8
		cpi valorDisplay,7
		breq display7
		cpi valorDisplay,6
		breq display6
		cpi valorDisplay,5
		breq display5
		cpi valorDisplay,4
		breq display4
		cpi valorDisplay,3
		breq display3
		cpi valorDisplay,2
		breq display2
		cpi valorDisplay,1
		breq display1
		cpi valorDisplay,0
		breq display0
		ret

display0:
		ldi r16,n0
		out PORTC,r16
		ret
display1:
		ldi r16,n1
		out PORTC,r16
		ret
display2:
		ldi r16,n2
		out PORTC,r16
		ret
display3:
		ldi r16,n3
		out PORTC,r16
		ret
display4:
		ldi r16,n4
		out PORTC,r16
		ret
display5:
		ldi r16,n5
		out PORTC,r16
		ret
display6:
		ldi r16,n6
		out PORTC,r16
		ret
display7:
		ldi r16,n7
		out PORTC,r16
		ret
display8:
		ldi r16,n8
		out PORTC,r16
		ret
display9:
		ldi r16,n9
		out PORTC,r16
		ret


timer:
		push r18
		push r19
		push r20

		ldi r20,z1ms			;copia valor de z
	ciclo0:	
		ldi r19,y1ms			;copia valor de y
	ciclo1:	
		ldi r18,x1ms			;cpoia valor de x
	ciclo2:	
		dec r18
		brne ciclo2

		dec r19
		brne ciclo1

		dec r20
		brne ciclo0
		
		pop r20
		pop r19
		pop	r18

		ret



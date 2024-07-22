;
; Demo3.asm
;
; Created: 25/09/2023 21:19:43
; Author : DEE
;

inic:
	ldi r16,0b11111111
	out DDRC,r16		;porto C como saídas(leds)
	out PORTC,r16		;leds todos desligados

	ldi r16,0b00000000
	out DDRA,r16		;porto A como entradas  (switch)
	ldi r16,0b11011111
	out PORTA,r16		;ativa as resistencias de pullups das entradas -> boa pratica, forçar o estado logico a 1(nao pressionado) das entradas que nao estao a ser utilizadas

	ldi r17,0b11111111	;D1 desligado
	ldi r18,0b11111110	;D1 ligado
	
ciclo:
	in r16,PINA			;load registo de entradas do porto A em r16
	ori r16,0b11011111	;anula outras entradas acionadas
	cpi r16,0b11011111	;compara com constante r16
	brne apaga			;salta para a label se a comparação falha

	out PORTC,r18		;liga led do pino 0 do porto C
	rjmp ciclo

apaga:
	out PORTC,r17		;desliga led do pino 0 do porto C
	rjmp ciclo
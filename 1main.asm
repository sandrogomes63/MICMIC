;
; AssemblerApplication1.asm
;
; Created: 18/09/2023 22:29:58
; Author : DEE
;

inic:
	ldi r16, 0b11111111
	out DDRC,r16
	out PORTC,r16
	ldi r16,0b00000000
	out DDRA,r16
ciclo:
	sbic PINA,0		// pino associado ao SW6, se tiver ligado, apaga
	rjmp apaga
	sbic PINA,5
	rjmp apaga

	cbi PORTC,0		// liga led pino5
	rjmp ciclo
apaga:
	sbi PORTC,0		// desliga led pino5
	rjmp ciclo


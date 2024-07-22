/*
 * Tr2_Func1_18112023.c
 *
 * Created: 20/11/2023
 * Author : Sandro Gomes 1221717
			Gonçalo Melo 1211710
 */

#include <avr/interrupt.h>
#include <avr/io.h>

const unsigned char display[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0xff};
volatile unsigned char flag5ms, flag500ms;
unsigned char dispselect, veldisp = 0;
unsigned char sw5, sw6, noSw;
unsigned char rol1, rol2;
unsigned char conta500ms = 100;
unsigned char velocidade,tempvelocidade = 50;
unsigned char sentido = 0;

// Parametros iniciais
void init(void)
{

	DDRB = 0b11100000; // motor
	PORTB = 0b01111110;

	DDRC = 0xFF; // display
	PORTC = 0xFF;

	DDRA = 0b11000000; // switches
	PORTA = 0b11000000;

	OCR0 = 77; // TEMPORIZADOR 5ms
	TCCR0 = 0b00001111;
	TIMSK |= 0b00000010;

	OCR2 = 128;			// DutyCycle 50%
	TCCR2 = 0b01100011; // phase correct PWM,prescaler 64, fpwm=490hz

	sei();
}

void updateDisplay(void)
{ // atualização displays
	

	if (velocidade >= 95)
	{
		veldisp = 99;
	}
	else
		veldisp = velocidade;

	if (dispselect == 0) // display 0
	{
		PORTA = 0b11000000;
		PORTC = display[veldisp % 10];
	}
	if (dispselect == 1) // display 1
	{
		PORTA = 0b10000000;
		PORTC = display[veldisp / 10];
	}
	if (dispselect == 2 ) // display 2
	{
		if (sentido == 0)
		{ // sentido do motor
			PORTA = 0b01000000;
			PORTC = 0b10111111; // sinal menos
		}
		else
		{
			PORTA = 0b01000000;
			PORTC = 0xff; // display apagado
		}
	}

	dispselect++;
	if (dispselect == 3)
		dispselect = 0;
}

unsigned char dutyCycle(void)
{ // função para calculo de OCR2

	unsigned char veloc;
	veloc = (255 * velocidade) / 100;
	return veloc;
}

void mudar_rot(void)
{ // inverte sentido de rotação do motor
	if (sw6 == 0 && sw5 == 1)
	{
		if (sentido == 1 && flag500ms == 1)
		{
			PORTB &= ~(1 << 6); // coloca bit 5 = 0, invertendo o sentido
			sw5 = 0;
			velocidade = tempvelocidade;
		}
		if (sentido == 0 && flag500ms == 1)
		{
			PORTB &= ~(1 << 5); // coloca bit 6 = 0, invertendo o sentido
			sw5 = 0;
			velocidade = tempvelocidade;
		}
		
	}
}

void motor(void)
{
	OCR2 = dutyCycle();
}

int main(void)
{
	// inicialização de variaveis
	unsigned char switches = 0;
	unsigned char noSw = 0;
	sentido = 1;
	velocidade = 50;
	sw6 = 0;		// variavel de motor a andar
	init();

	PORTB &= ~(1 << 6); // inicia rotação do motor
				
	while (1)
	{

		switches = PINA & 0b00110011; // verifica apenas os switches que pretendemos que altere o funcionamento

		switch (switches)
		{
		case 0b00110010: // sw1
			if (noSw == 0)
			{
				noSw = 1;
				if (velocidade < 100) // limita velocidade a 100%
				{
					velocidade += 5; // aumento de 5% velocidade
				}
			}
			break;
		case 0b00110001: // sw2
			if (noSw == 0)
			{
				noSw = 1;
				if (velocidade >= 5)
				{					 // verifica velocidade maior do que 5 para que nao ultrapasse negativamente a escala de 0 a 100
					velocidade -= 5; // diminui 5% velocidade
				}
			}
			break;
		case 0b00100011: // sw5
			if (noSw == 0)
			{
				noSw = 1;
				if (sw6 == 0 && sw5 == 0)
				{
					sw5 = 1;
					if (PORTB & (1 << 5))
					{				 // verifica o sentido de rotação bit 5 = 1
						sentido = 0; // variável do display 2
					}
					else
					{
						sentido = 1; // variável do display 2, sinal menos
					}
					PORTB |= (1 << 5) | (1 << 6);
					tempvelocidade = velocidade;
					velocidade = 0;
					conta500ms = 100;
					flag500ms = 0;
				}
			}
			break;
		case 0b00010011: // sw6
			if (noSw == 0)
			{
				noSw = 1;
				if (sw6 == 0)
				{
					sw6 = 1;
					velocidade = 0;
				}
				else
				{
					velocidade = 50;
					sw6 = 0;
				}
			}
			break;
		default:
			noSw = 0;
		}

		if (flag5ms == 1)
		{
			flag5ms = 0;
			updateDisplay();
			motor();
			mudar_rot();
		}

	} // while(1)
}

ISR(TIMER0_COMP_vect)
{
	flag5ms = 1;

	conta500ms--;
	if (conta500ms == 0){
		flag500ms = 1;
	}
}

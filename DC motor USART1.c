/*
 * Tr2_Func1_18112023.c
 *
 * Created: 20/11/2023
 * Author : Sandro Gomes 1221717
			Goncalo Melo 1211710
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>

typedef struct USARTRX
{
	char receiver_buffer;
	unsigned char status;
	unsigned char receive : 1; // reserva 1 bit
	unsigned char error : 1;

} USARTRX_st;

volatile USARTRX_st rxUsart = {0, 0, 0, 0};
char transmit_buffer[50];

//////////// declaracao de variaveis ///////////////////
const unsigned char display[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0xff};
volatile unsigned char flag5ms, flag500ms;
unsigned char dispselect, veldisp = 0;
unsigned char sw5, sw6, noSw = 0;
unsigned char rol1, rol2;
unsigned char conta500ms = 100;
unsigned char velocidade, tempVelocidade = 50;
unsigned char sentido = 0;
unsigned char mode = 'D';

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
	TCCR2 = 0b01100011; // phase correct PWM, prescaler 64, fpwm=490hz

	UBRR1H = 0;			  // usart1
	UBRR1L = 103;		  // baud rate =19200
	UCSR1A = (1 << U2X1); // dobro da velocidade
	UCSR1B = (1 << RXCIE1) | (1 << RXEN1) | (1 << TXEN1);
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);

	sei();
}

void updateDisplay(void)
{ // atualizacao displays

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
	if (dispselect == 2) // display 2
	{
		if (sentido == 1)
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
	if (dispselect == 3) // display 2
	{
		if (mode == 'S')
		{
			PORTA = 0b00000000;
			PORTC = 0b10010010;
		}
		if (mode == 'D')
		{
			PORTA = 0b00000000;
			PORTC = 0b10100001;
		}
	}

	dispselect++;
	if (dispselect == 4)
		dispselect = 0;
}

unsigned char dutyCycle(void)
{ // funcao para calculo de OCR2

	unsigned char veloc;
	veloc = (255 * velocidade) / 100;
	return veloc;
}

void mudar_rot(void)
{ // inverte sentido de rotacao do motor
	if (sw6 == 0 && sw5 == 1 && flag500ms == 1)
	{
		if (sentido == 0 && flag500ms == 1)
		{
			PORTB &= ~(1 << 6); // coloca bit 5 = 0, invertendo o sentido
			sw5 = 0;
			velocidade = tempVelocidade;
			sentido = 1;
		}
		else
		{
			PORTB &= ~(1 << 5); // coloca bit 6 = 0, invertendo o sentido
			sw5 = 0;
			velocidade = tempVelocidade;
			sentido = 0;
		}
	}
}

void motor(void)
{
	OCR2 = dutyCycle();
}

void send_message(char *buffer)
{
	unsigned char i = 0;
	while (buffer[i] != '\0') // verifica fim de string
	{
		while ((UCSR1A & 1 << UDRE1) == 0)
			;			  // verifica se buffer transmissao esta vazio
		UDR1 = buffer[i]; // coloca 1 byte no registo de transmissao
		i++;
	}
}
// não e preciso ta feito a cima
void send_messageD(char data)
{
	while ((UCSR1A & 1 << UDRE1) == 0)
		; // ACRESCENTADO teoricas
	UDR1 = data;
}
//
unsigned char USART_RX(void)
{
	while ((UCSR1A & (1 << RXC1)) == 0)
		; // ACRESCENTADO teoricas
	return UDR1;
}

void switch_mode(void)
{
	unsigned char switches = 0;

	switches = PINA & 0b00110011; // verifica apenas os switches que pretendemos que altere o funcionamento

	switch (switches)
	{
	case 0b00110010: // sw1
		if (noSw == 0)
		{
			noSw = 1;
			sw6 = 0;
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
				if (PORTB & (1 << 6))
				{				 // verifica o sentido de rotacao bit 5 = 1
					sentido = 0; // variavel do display 2
				}
				else
				{
					sentido = 1; // variavel do display 2, sinal menos
				}
				tempVelocidade = velocidade;
				PORTB |= (1 << 5) | (1 << 6);
				conta500ms = 100;
				velocidade = 0;
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
}

void console_mode(char *buffer)
{
	if (rxUsart.receive == 1) // verificar se novos dados foram recebidos
	{
		if (rxUsart.error == 1) // verificar se houve um erro
		{
			rxUsart.error = 0;
		}
		else
		{
			*buffer = rxUsart.receiver_buffer;
			switch (*buffer)
			{
			case '+':
				if (mode == 'D')
				{
					sprintf(transmit_buffer, "Tecla: + \r\n");
					send_message(transmit_buffer);
					if (velocidade < 100) // limita velocidade a 100%
					{
						sw6 = 0;
						velocidade += 5; // aumento de 5% velocidade
					}
				}
				break;
			case '-':
				if (mode == 'D')
				{
					sprintf(transmit_buffer, "Tecla: - \r\n");
					send_message(transmit_buffer);
					if (velocidade >= 5)
					{					 // verifica velocidade maior do que 5 para que nao ultrapasse negativamente a escala de 0 a 100
						velocidade -= 5; // diminui 5% velocidade
					}
				}
				break;
			case 'I':
			case 'i':
				if (mode == 'D')
				{
					sprintf(transmit_buffer, "Tecla: I \r\n");
					send_message(transmit_buffer);
					if (sw6 == 0 && sw5 == 0)
					{
						sw5 = 1;
						if (PORTB & (1 << 6))
						{				 // verifica o sentido de rotacao bit 5 = 1
							sentido = 0; // variavel do display 2
						}
						else
						{
							sentido = 1; // variavel do display 2, sinal menos
						}
						tempVelocidade = velocidade;
						PORTB |= (1 << 5) | (1 << 6);
						conta500ms = 100;
						velocidade = 0;
						flag500ms = 0;
					}
				}
				break;
			case 'P':
			case 'p':
				if (mode == 'D')
				{
					sprintf(transmit_buffer, "Tecla: P \r\n");
					send_message(transmit_buffer);
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
			case 'S':
			case 's':

				sprintf(transmit_buffer, "Tecla: S \r\n");
				send_message(transmit_buffer);
				mode = 'S';

				break;
			case 'D':
			case 'd':

				sprintf(transmit_buffer, "Tecla: D \r\n");
				send_message(transmit_buffer);
				mode = 'D';

				break;
			case 'B':
			case 'b':
				tempVelocidade = velocidade;
				if (sentido == 0)
				{
					sprintf(transmit_buffer, "Velocidade: %i  e  Sentido: Horario\r\n", tempVelocidade);
				}
				else
				{
					sprintf(transmit_buffer, "Velocidade: %i e  Sentido: Anti-horario\r\n", tempVelocidade);
				}
				send_message(transmit_buffer);
				
				break;
			
			default:
				break;
			}
		}
		rxUsart.receive = 0;
		rxUsart.receiver_buffer = 0;
	}
}

int main(void)
{
	// inicializacao de variaveis

	velocidade = 50;
	mode = 'D';
	init();

	PORTB &= ~(1 << 5); // inicia rotacao do motor

	while (1)
	{
		console_mode(transmit_buffer);

		if (mode == 'S')
			switch_mode();

		if (flag5ms == 1)
		{
			flag5ms = 0;
			updateDisplay();
			motor();
			mudar_rot();
		}
	}
}

ISR(TIMER0_COMP_vect)
{
	flag5ms = 1;

	conta500ms--;
	if (conta500ms == 0)
		flag500ms = 1;
}

ISR(USART1_RX_vect)
{

	rxUsart.status = UCSR1A;

	if (rxUsart.status & ((1 << FE1) | (1 << DOR1) | (1 >> UPE1))) // verificar erros na recepcao
		rxUsart.error = 1;

	rxUsart.receiver_buffer = UDR1;
	rxUsart.receive = 1;
}

/*
 * Tr2_Func4_10122023.c
 *
 * Created: 20/11/2023
 * Author : Sandro Gomes 1221717

 */

#define F_CPU 16000000UL //clock 16MHz
#include <util/delay.h>
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
unsigned char mode = 'D';
unsigned char passo = 0;
unsigned char contapasso = 0;
////// Tabela de passos /////////////////

const unsigned char M_passo[] = {0b00000001, 0b00001001, 0b00001000, 0b00001100, 0b00000100, 0b00000110, 0b00000010, 0b00000011}; // sentido R



// Parametros iniciais
void init(void)
{

	DDRE = 0b00001111; // motor
	PORTE = 0b11110000;

	OCR2 = 128;			// DutyCycle 50%
	TCCR2 = 0b01100011; // phase correct PWM, prescaler 64, fpwm=490hz

	// usart
	UBRR1H = 0;			  // usart1
	UBRR1L = 103;		  // baud rate =19200
	UCSR1A = (1 << U2X1); // dobro da velocidade
	UCSR1B = (1 << RXCIE1) | (1 << RXEN1) | (1 << TXEN1);
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);

	sei();
	
}

void send_message(char *buffer)
{
	unsigned char i = 0;
	while (buffer[i] != '\0')		// verifica fim de string
	{
		while ((UCSR1A & 1 << UDRE1) == 0);			  // verifica se buffer transmissao esta vazio
		UDR1 = buffer[i];						// coloca 1 byte no registo de transmissao
		i++;
	}
}
//
void send_messageD(char data)
{
	while ((UCSR1A & 1 << UDRE1) == 0); 
	UDR1 = data;
}
//
unsigned char USART_RX(void)
{
	while ((UCSR1A & (1 << RXC1)) == 0); 
	return UDR1;
}


// conta passos sentido R
void decpasso(int n)
{
	while (n > 0)
	{
		unsigned char i = 0;
		while (i < 8)
		{
			PORTE = M_passo[i];
			_delay_ms(25);
			i++;
		}
		n--;
	}
}


// conta passos sentido L
void incpasso(int n)
{ 	
	while (n > 0)
	{
		unsigned char i = 7;
		while (i > 0)
		{
			PORTE = M_passo[i-1];
			_delay_ms(25);
			i--;
			n--;
		}
	}
}

// Controlo por consola
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
			case 'R':
			case 'r':
				sprintf(transmit_buffer, "Tecla: R \r\n");
				send_message(transmit_buffer);
				passo = 1;
				decpasso(passo);
				break;

			case 'L':
			case 'l':
				sprintf(transmit_buffer, "Tecla: L \r\n");
				send_message(transmit_buffer);
				passo = 1;
				incpasso(passo);
				break;

			case 'Z':
			case 'z':
				passo = - (contapasso);
				break;

			case '0':
				// posição inicial 0º
				break;

			case '9':
				decpasso(5); // 90º
				break;

			case '1':
				decpasso(10); // 180º
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
	init();

	PORTE = M_passo[0];
	while (1)
	{
		console_mode(transmit_buffer);

	}

}


ISR(USART1_RX_vect)
{

	rxUsart.status = UCSR1A;

	if (rxUsart.status & ((1 << FE1) | (1 << DOR1) | (1 >> UPE1))) // verificar erros na recepcao
		rxUsart.error = 1;

	rxUsart.receiver_buffer = UDR1;
	rxUsart.receive = 1;
}

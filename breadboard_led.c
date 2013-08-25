#include <msp430.h>
#include <stdint.h>

#define CS_PIN BIT4

#define DIGIT1()	\
		P2OUT &= ~BIT0;	\
		P1OUT |= BIT0

#define DIGIT2()	\
		P1OUT &= ~BIT0;	\
		P2OUT |= BIT0

void delay() {
	uint16_t i = 0;
	uint16_t j = 0;

	for(i = 0; i<150; i++) {
		for(j = 0; j<100; j++) {
			;
		}
	}
}

enum spi_transfer_state {
	START,
	STOP
} SPI_TRANSFER_STATE;

/*const uint16_t num[10] = {
	(~(0x7D))<<1,
	(~(0x44))<<1,
	(~(0x5B))<<1,
	(~(0x5E))<<1,
	(~(0x74))<<1,
	(~(0x3E))<<1,
	(~(0x3F))<<1,
	(~(0x4C))<<1,
	(~(0x7F))<<1,
	(~(0xFE))<<1,
};*/


const uint16_t num[16] = {
	(~(0x5F))<<1,
	(~(0x0A))<<1,
	(~(0x79))<<1,
	(~(0x7A))<<1,
	(~(0x2E))<<1,
	(~(0x76))<<1,
	(~(0x77))<<1,
	(~(0x4A))<<1,
	(~(0x7F))<<1,
	(~(0x7E))<<1,
	(~(0x6F))<<1,
	(~(0x37))<<1,
	(~(0x55))<<1,
	(~(0x3B))<<1,
	(~(0x75))<<1,
	(~(0x65))<<1,
};
/*const uint16_t num[7] = {
	(~((0x01)<<0))<<1,
	(~((0x01)<<1))<<1,
	(~((0x01)<<2))<<1,
	(~((0x01)<<3))<<1,
	(~((0x01)<<4))<<1,
	(~((0x01)<<5))<<1,
	(~((0x01)<<6))<<1,
};*/

void configure_timer() {
	TACCTL0 |= CCIE; //Enable capture compare interrupt flag.
	TACCR0 = 0x0FFF;
	TACTL = TASSEL_2|MC_3|ID_1|TAIE;
}

void configure_spi() {
	SPI_TRANSFER_STATE = STOP;
	UCB0CTL1 = UCSWRST; //Set to reset state.
	UCB0CTL0 = UCMODE_0; //3 wire spi
	UCB0CTL0 &= ~UCMSB;

	UCB0CTL0 &= ~UCMST; //Set to slave.
	
	/* P1SEL and P1SEL2 == 1 for SPI function. */
	P1SEL |= (BIT5|BIT6|BIT7); 
	P1SEL2 |= (BIT5|BIT6|BIT7); 

	UCB0CTL1 &= ~UCSWRST;

	IE2 &= ~(UCB0RXIE|UCB0TXIE); //Stop the rx interrupt.
}

/*
 * P1.1 Red LED.
 * P1.2 Green LED.
 */

uint8_t units = 0;
uint8_t tens = 0;

uint8_t rxbuf = 0;

uint8_t flag = 0;

int main(void) {
	uint16_t i = 8;

	WDTCTL = WDTPW | WDTHOLD;

	/*P2SEL &= ~(BIT6|BIT7);
	P1OUT = (BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7);
	P2OUT = (BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7);


	P2OUT &= (BIT5);

	P1DIR = (BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7);
	P2DIR = (BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7);
	

	while(1) {
		P1OUT ^= BIT1;
		delay();
	}*/

	P2SEL &= ~(BIT6|BIT7);
	P2OUT &= ~BIT0; //Preload.
	P2DIR |= (BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7);
	P2OUT |= (BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7);

	P1OUT &= ~BIT0; //Preload.
	P1DIR |= (BIT0|BIT1|BIT2);
/* Chip select */
	P1OUT |= CS_PIN;	//Set to pull up.
	P1REN |= CS_PIN;
	P1DIR &= ~CS_PIN; //Chip select Set to input.

	P1IES |= CS_PIN;//High to low transition.

	P1IFG &= ~CS_PIN; //Clear the interrupt flag.
/**************/

	P1OUT &= ~BIT2;
	P1OUT &= ~BIT1;
	
	configure_timer();
	configure_spi();

	_BIS_SR(GIE);

	P1IE |= CS_PIN; //Interrupt enable.

	for(i = 0; i<10; i++) {
		delay();
	}
	
	i = 0;
	units = 0;
	tens = 0;

	while(1) {
//		P1OUT ^= BIT2;
#ifdef LEDTEST
		if(units == 15) {

			if(tens == 15) {
				tens = 0;
			} else {
				tens++;
			}

			units = 0;

			delay();
			delay();
			continue;
		}

		units++;
#endif
		delay();
		delay();
	}
}

__attribute__((interrupt(TIMER0_A0_VECTOR)))
void TIMER0_A0_ISR(void) {
	P2OUT = num[tens];
	DIGIT1();
}

__attribute__((interrupt(TIMER0_A1_VECTOR)))
void TIMER0_A1_ISR(void) {
	switch(TA0IV) {
		case 0x00:
			break;
		case 0x02:
			break;
		case 0x04:
			break;
		case 0x06:
			break;
		case 0x08:
			break;
		case 0x0A:
			P2OUT = num[units];
			DIGIT2();
			break;
		case 0x0C:
			break;
		case 0x0E:
			break;
	}
}

uint8_t ch = 0;

__attribute__((interrupt(USCIAB0TX_VECTOR)))
void spi_tx_isr(void) {
	if(IFG2 & UCB0TXIFG) {
		UCB0TXBUF = ch++;
		IE2 &= ~UCB0TXIE;
	}
}

__attribute__((interrupt(USCIAB0RX_VECTOR)))
void spi_rx_isr(void) {
	if(IFG2 & UCB0RXIFG) {
		rxbuf = UCB0RXBUF;
		units = (rxbuf & 0x0F);
		tens = (rxbuf & 0xF0)>>4;
		P1OUT ^= BIT2;
	}
}

__attribute__((interrupt(PORT1_VECTOR)))
void port_isr(void) {
	if(P1IFG & CS_PIN) {
		if(SPI_TRANSFER_STATE == STOP) {
			IE2 |= UCB0RXIE; //Enable receive interrupt.
			P1IES |= CS_PIN; //Low to high transition.
			SPI_TRANSFER_STATE = START;
			P1OUT ^= BIT1;
		} else if(SPI_TRANSFER_STATE == START) {
			P1IES &= ~CS_PIN; //high to low transition.
			IE2 &= ~UCB0RXIE; //Stop the rx interrupt.
			SPI_TRANSFER_STATE = STOP;
		}
		P1IFG &= ~CS_PIN; //Clear the interrupt flag.
	}
}

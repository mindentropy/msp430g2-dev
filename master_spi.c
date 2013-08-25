#include <msp430.h>
#include <stdint.h>

#define CS_PIN 	BIT4
#define SOMI	BIT6
#define SIMO 	BIT7

#define cs_enable()	\
		P1OUT &= ~CS_PIN

#define cs_disable()	\
		P1OUT |= CS_PIN

	
void delay() {
	uint16_t i = 0;
	uint16_t j = 0;

	for(i = 0; i<150; i++) {
		for(j = 0; j<100; j++) {
			;
		}
	}
}


/*
 * P1.0 Red LED.
 * P1.6 Green LED.
 */


/*
 * P1.5 -> CLK
 * P1.6 -> SOMI
 * P1.7 -> SIMO
 */
void configure_spi() {
	UCB0CTL1 = UCSWRST; //Set to reset state.

	UCB0CTL1 |= UCSSEL_3;  //Set to SMCLK.
	UCB0CTL0 = UCMODE_0|UCMST; //3 wire spi, master
	UCB0CTL0 &= ~UCMSB;
	//UCB0CTL0 &= ~UCSYNC;

	
	/* P1SEL and P1SEL2 == 1 for SPI function. */
	P1SEL |= (BIT5|BIT6|BIT7); 
	P1SEL2 |= (BIT5|BIT6|BIT7);

	UCB0BR0 = 0x4C;
	UCB0BR1 = 0x04;

	//UCB0STAT |= UCLISTEN; //Enable listen loopback mode.

	UCB0CTL1 &= ~UCSWRST;

	cs_enable();
	IE2 |= UCB0TXIE;
}

uint8_t flag = 0;
uint8_t rxbuf = 0;

uint8_t ch = 0;

#define is_spi_busy() \
	(UCB0STAT & UCBUSY)
	
int main(void) {

	WDTCTL = WDTPW | WDTHOLD;

//	P1OUT = BIT0;

	P1OUT |= BIT0|CS_PIN;
	P1DIR |= BIT0|CS_PIN;


	configure_spi();
	
	_BIS_SR(GIE);

	while(1) {
		IE2 |= UCB0TXIE;
		while(is_spi_busy())
			;
		cs_disable();
		delay();
	}
}


__attribute__((interrupt(USCIAB0TX_VECTOR)))
void spi_tx_isr(void) {
	if(IFG2 & UCB0TXIFG) {
		cs_enable();
		IE2 &= ~UCB0TXIE;
		UCB0TXBUF = ch;
		ch++;
		P1OUT ^= BIT0;
	}
}

__attribute__((interrupt(USCIAB0RX_VECTOR)))
void spi_rx_isr(void) {
	if(IFG2 & UCB0RXIFG) {
		rxbuf = UCB0RXBUF;
		if(rxbuf == 'A')
			P1OUT ^= BIT0;
	}
}

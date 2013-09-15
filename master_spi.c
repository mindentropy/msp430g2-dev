#include <msp430.h>
#include <stdint.h>

#define S2_PIN	BIT3 //Pin pulled high.
#define CS_PIN 	BIT4
#define SOMI	BIT6
#define SIMO 	BIT7


#define cs_enable()	\
	P1OUT &= ~CS_PIN

#define cs_disable()	\
	P1OUT |= CS_PIN

#define disable_spi_tx_pending_interrupt()	\
	IFG2 &= ~UCB0TXIFG

#define disable_spi_tx_interrupt()	\
	IE2 &= ~UCB0TXIE

#define enable_spi_tx_interrupt()	\
	IE2 |= UCB0TXIE

#define led1_off()	\
	P1OUT &= ~BIT0

#define led1_on()	\
	P1OUT |= BIT0

#define toggle_led1() \
	P1OUT ^= BIT0;

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
	UCB0CTL0 = UCMODE_0|UCMST|UCSYNC; //3 wire spi, master
	UCB0CTL0 &= ~UCMSB;

	
	/* P1SEL and P1SEL2 == 1 for SPI function. */
	P1SEL |= (BIT5|BIT6|BIT7); 
	P1SEL2 |= (BIT5|BIT6|BIT7);

	UCB0BR0 = 0x4C;
	UCB0BR1 = 0x04;

	//UCB0STAT |= UCLISTEN; //Enable listen loopback mode.
	UCB0CTL1 &= ~UCSWRST;
	
	disable_spi_tx_interrupt();

//	cs_enable();
//	IE2 |= UCB0TXIE;
}

uint8_t flag = 0;
uint8_t rxbuf = 0;

uint8_t spi_counter = 0;

#define is_spi_busy() \
	(UCB0STAT & UCBUSY)
	
int main(void) {
	uint16_t i = 0;
	uint16_t j = 0;

	WDTCTL = WDTPW | WDTHOLD;

//	P1OUT = BIT0;

	
	P1OUT |= CS_PIN|BIT0;
	P1DIR |= BIT0|CS_PIN;

	P1OUT |= S2_PIN;
	P1REN |= S2_PIN;
	P1DIR &= ~S2_PIN;

	configure_spi();
	
	_BIS_SR(GIE);
	
	led1_off();
	
/*	while((P1IN & S2_PIN)) //While pulled high.
			;*/


	while(1) {
		cs_enable();
		spi_counter++;
		enable_spi_tx_interrupt();

		while(is_spi_busy())
			;

	/*	while((P1IN & S2_PIN)) //While pulled high.
			;

		while(!(P1IN & S2_PIN)) //While pulled high.
			;*/


		cs_disable();
		delay();
	}
}


__attribute__((interrupt(USCIAB0TX_VECTOR)))
void spi_tx_isr(void) {
	if(IFG2 & UCB0TXIFG) {
		toggle_led1();
		UCB0TXBUF = spi_counter;
		disable_spi_tx_interrupt();
	}
}

__attribute__((interrupt(USCIAB0RX_VECTOR)))
void spi_rx_isr(void) {
	if(IFG2 & UCB0RXIFG) {
		rxbuf = UCB0RXBUF;
	}
}

#include <msp430.h>

#define UART_TXD (1<<1)  //TXD -> P1.1
#define UART_RXD (1<<2)  //RXD -> P1.2


#define UART_TBIT	(1000000/9600)

unsigned int tx_data; /* Buffer large enough to hold the start and stop bit */
unsigned char rx_data;

void TimerA_UART_init(void);
void TimerA_UART_tx(unsigned char byte);
void TimerA_UART_print(char *string);
void delay_long();

void main(void) {
	unsigned char ch = 33;
	WDTCTL = WDTPW | WDTHOLD;

	__delay_cycles(50000);
	__delay_cycles(50000);
	__delay_cycles(50000);
	__delay_cycles(50000);

	DCOCTL = 0x00;
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;


	P1OUT = 0x00|BIT3;

	P1SEL = UART_TXD | UART_RXD;
	P1DIR = 0xFF & ~(UART_RXD|BIT3);

	//P1DIR = 0xFF & ~(BIT3); //Set P1.3 as Input.
	P1REN |= BIT3;

	P2OUT = 0x00;
	P2SEL = 0x00;
	P2DIR = 0xFF;


	while((P1IN & BIT3)) 
		;
	
	P1OUT ^= 0x01;

	__enable_interrupt();

	TimerA_UART_init();

	for(;;) {
//		delay_long();
		TimerA_UART_tx(ch);
		ch++;
		
		if(ch == 126) ch = 33;

		P1OUT ^= 0x01;
	}
		
}

void delay_long() {
	unsigned int i = 0;

	__delay_cycles(50000);
	__delay_cycles(50000);
	__delay_cycles(50000);
}

void TimerA_UART_init(void) {
	TACCTL0 = OUT; 
	TACCTL1 = SCS + CM1 + CAP + CCIE;
	TACTL = TASSEL_2 + MC_2;
}

void TimerA_UART_tx(unsigned char byte) {
  /* 
   * Loop until the char is TX'd. Once a char is 
   * completely transfered the interrupt enable flag
   * is cleared 
   */
   	while(TACCTL0 & CCIE)
   		;

	TACCR0 = TAR;
	TACCR0 += UART_TBIT;
	TACCTL0 = OUTMOD0 + CCIE;

	tx_data = byte;
	tx_data |= 0x100; /* Adding stop bit */
	tx_data <<= 1;	  /* Adding start bit */
}

void TimerA_UART_print(char *string) {
	while(*string) {
		TimerA_UART_tx(*string++);
	}
}

__attribute__((interrupt(TIMER0_A0_VECTOR)))
void Timer0_A0(void) 
{
	static unsigned char tx_bit_cnt = 10;
	TACCR0 += UART_TBIT;

	if(tx_bit_cnt == 0) {
		TACCTL0 &= ~CCIE;
		tx_bit_cnt = 10;
	} else {
		if(tx_data & 0x01) {
			//TACCTL0 |= OUTMOD_3;
			TACCTL0 &= ~OUTMOD2; // Set the value to OUT value which is 1.
		} else {
			//TACCTL0 |= OUTMOD_7;
			TACCTL0 |= OUTMOD2;	// Set the value to 'SET' ie. 0
		}
		tx_data >>= 1;
		tx_bit_cnt--;
	}
}

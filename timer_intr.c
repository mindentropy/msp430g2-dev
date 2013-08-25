#include <msp430.h>

unsigned int x = 0;

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;
	
	P1DIR = BIT0;

/***  Setting the clocks to 16Mhz ***/
	DCOCTL |= (DCO2|DCO1|DCO0);
	BCSCTL1 = (RSEL3|RSEL1);
	BCSCTL2 = (DIVS_1);
/************************************/

	TACCTL1 = CCIE; /* Enable interrupt */
	TACCR0 = 0xFFFF;

	TACTL = TASSEL_2 | MC_2; 

	_BIS_SR(GIE);

	while(1)
		x++;
}

//#pragma vector=TIMER_A0_VECTOR
/*
__attribute__((interrupt(TIMER0_A0_VECTOR)))
void Timer0_A0(void)
{
	P1OUT ^= 0x01;
	TACCR0 |= 0xFFFF;
}
*/

__attribute__((interrupt(TIMER0_A1_VECTOR)))
void Timer0_A1(void)
{
	switch(TA0IV) {
		case TA0IV_TACCR1:
			P1OUT ^= BIT0;
			TACCR0 = 0xFFFF;
			break;
		case TA0IV_TACCR2:
			break;
		case TA0IV_TAIFG:
			break;
	}
}

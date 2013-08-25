//#include <msp430.h>
#include <msp430g2452.h>

#define DELAY 0x6000

void delay() {
   int i = 0;
   for (i = 0; i < 10000; i++);
}


void pattern1() {
	unsigned char i = 0;
	unsigned j = BIT4;

	while(1) {
		if(i == 4) {
			P1OUT ^= BIT0|BIT3|BIT4|BIT5|BIT6|BIT7;
			i = 0;
			j = BIT4;
		}
		P1OUT ^= j;
		j = j<<1;
		i++;
		delay();
		P1OUT ^= (j>>1);
	}
}

void pattern2() {
	unsigned char i = 0;
	unsigned j = BIT4;

	while(1) {
		if(i == 4) {
			P1OUT ^= BIT0|BIT3|BIT4|BIT5|BIT6|BIT7;
			i = 0;
			if(P1OUT & BIT0)
				j = BIT7;
			else
				j = BIT4;
		}

		P1OUT ^= j;

		if(P1OUT & BIT0)
			j = j>>1;
		else
			j = j<<1;

		i++;
		delay();

		if(P1OUT & BIT0)
			P1OUT ^= (j<<1);
		else
			P1OUT ^= (j>>1);
	}
}

void pattern3() {
	unsigned char i = 0;
	unsigned j = BIT4;

	while(1) {
		if(i == 4) {
			i = 0;
			j = BIT4;
		}
		P1OUT ^= j;
		delay();
		P1OUT ^= BIT0;
		P1OUT ^= j;

		P1OUT ^= BIT3;

		P1OUT ^= BIT4|BIT5|BIT6|BIT7;

		P1OUT ^= j;
		delay();
		P1OUT ^= BIT0;
		P1OUT ^= j;
		
		P1OUT ^= BIT3;
		P1OUT ^= BIT4|BIT5|BIT6|BIT7;

		j = j << 1;

		i++;
	}
}

int main(void) {
  volatile int i;

  // stop watchdog timer
  WDTCTL = WDTPW | WDTHOLD;

  P1DIR = BIT0 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7;

  P1OUT = 0x00;

  // loop forever
  for (;;) {
  	//P1OUT ^= BIT0;
    //P1OUT ^= 0x01;
/*	P1OUT ^= BIT4;
	P1OUT ^= BIT5;
	P1OUT ^= BIT6;
	P1OUT ^= BIT7;*/

	pattern3();
	while(1) {
		;
	}
  }
}

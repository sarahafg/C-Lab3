/*--------------------------------------------------------
GEORGE MASON UNIVERSITY
ECE 447 - Lab3 Button Test
  P2.1 button
  P2.2 button
  P2.3 button
  P2.4 button

  P2.6 Row Clock
  P2.7 Row Initialization

  P9.0 LED column 0
  ...
  P9.3 LED column 3

Date:   Fall 2020
Author: Jens-Peter Kaps

Change Log:
20200913 Initial Version, Jens-Peter Kaps
--------------------------------------------------------*/

#include <msp430.h> 

void delay(void) {
    volatile unsigned loops = 2500; // Start the delay counter at 25,000
    while (--loops > 0);             // Count down until the delay counter reaches 0
}


int main(void)
{
    unsigned char rowcnt;     // row counter

    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer

    P2DIR &= ~(BIT1 | BIT2 | BIT3 |BIT4); // buttons as inputs
    P2REN |= (BIT1 | BIT2 | BIT3 |BIT4);  // enable resistor
    P2OUT |= (BIT1 | BIT2 | BIT3 |BIT4);  // pull up resistor

    P2DIR |= (BIT6 | BIT7);   // Make Port 2.6 and 2.7 outputs
    P2OUT &= ~(BIT6 | BIT7);  // Set ports to 0

    P9DIR |= (BIT3 | BIT2 | BIT1 | BIT0);  // Outputs for LED Matrix
    P9OUT &= ~(BIT3 | BIT2 | BIT1 | BIT0); // columns 5-8

    PM5CTL0 &= ~LOCKLPM5;     // Unlock ports from power manager

    rowcnt = 0;

    while(1)                    // continuous loop
    {
        delay();
        if(P2OUT & BIT6)                 // If row clock 1 -> place breakpoint here
            P2OUT &= ~BIT6;              //   Set row clock 0
        else {
            if(rowcnt == 7) {            // if on row 7
                P2OUT |= (BIT6 | BIT7);  //   Set row clock and row init 1
                rowcnt = 0;              //   set row counter back to 0
            } else {                     // for all other rows
                P2OUT |= BIT6;           //   only set row clock 1
                P2OUT &= ~BIT7;          //   set row init to 0
                rowcnt++;                //   increment row counter
            }
            P9OUT = ~(P2IN >> 1) & 0x0F;
        }
    }
	
	return 0;
}

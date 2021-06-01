/*--------------------------------------------------------
GEORGE MASON UNIVERSITY
ECE 447 - Lab3 In-Lab Exercise
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
20200917 Initial Version, Jens-Peter Kaps
--------------------------------------------------------*/

// create global variable to "remember" which columns are on and which are off

#include <msp430.h> 
unsigned char col;      // LED column state on or off
unsigned char row;      // LED row state on or off
unsigned int rowcnt;    // row counter
unsigned char matrix[8] = {0,
                           0,
                           0,
                           0,
                           0x08,
                           0,
                           0,
                           0};

void delay(void) {
    volatile unsigned loops = 100;   // Start the delay counter at 100
    while (--loops > 0);             // Count down until the delay counter reaches 0
}


void main(void)
{
    row = 5;    // Initializing row to 5
    col = 5;    // Initializing col to 5

    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer

    P2DIR &= ~(BIT1 | BIT2 | BIT3 | BIT4); // buttons as inputs
    P2REN |= (BIT1 | BIT2 | BIT3 | BIT4);  // enable resistor
    P2OUT |= (BIT1 | BIT2 | BIT3 | BIT4);  // pull up resistor

    P2DIR |= (BIT6 | BIT7);   // Make Port 2.6 and 2.7 outputs
    P2OUT &= ~(BIT6 | BIT7);  // Set ports to 0

    P8DIR |= (BIT7);                                            // Make Port 8.7 an output
    P9DIR |= (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6);  // Make Ports 9.0-9.6 outputs
    P9OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6); // Make Ports 9.0-9.6 outputs
    P8OUT &= ~(BIT7);                                           // Sets port to 0

    PM5CTL0 &= ~LOCKLPM5;     // Unlock ports from power manager

    // Insert Interrupt configuration for buttons here
    P2IES |= (BIT1 | BIT2 | BIT3 | BIT4);  //  select  falling  edge
    P2IFG &= ~(BIT1 | BIT2 | BIT3 | BIT4); //  clear  interrupt  flag
    P2IE |= (BIT1 | BIT2 | BIT3 | BIT4);   //  enable  interrupt
    // Enable interrupts
    __enable_interrupt();
    while(1)                             // continuous loop
    {
        delay();
        if(P2OUT & BIT6)                 // If row clock 1 -> place breakpoint here
            P2OUT &= ~BIT6;              // Set row clock 0
        else {
            if(rowcnt == 5) {            // if on row 5
                P2OUT |= (BIT6 | BIT7);  // Set row clock and row init 1
            } else {                     // for all other rows
                P2OUT |= BIT6;           // only set row clock 1
                P2OUT &= ~BIT7;          // set row init to 0
            }
            rowcnt++;                    // Incrementing row counter
            if (rowcnt == 8) {           // if on row 8
                rowcnt = 0;              // Resetting clock
                }
            P9OUT = matrix[rowcnt];      // Sending data of matrix to port
            P8OUT = matrix[rowcnt];      // Sending data of matrix to port
        }
    }
}

// Add code to the ISR below to set and reset which columns should be on
// and which should be off.
// More than on column can be on at the same time.

// P2 interrupt service routine
#pragma vector = PORT2_VECTOR      // associate funct. w/ interrupt vector
__interrupt void Port_2(void)      // name of ISR
{
    // accessing P2IV automatically clears the pending interrupt flag with
    // the highest priority with P2IFG.0 being the highest and P2IFG.7 the lowest.
    switch(__even_in_range(P2IV,P2IV_P2IFG7))
    {
    case P2IV_NONE:   break;   // Vector  0: no interrupt
    case P2IV_P2IFG0: break;   // Vector  2: P2.0
    case P2IV_P2IFG1:   // LEFT
        if (col != 8) {                             // if not on column 8
            matrix[rowcnt] = matrix[rowcnt] << 1;   // shift 1 space to the left
            col++;                                  // incrementing column
        }
        break;   // Vector  4: P2.1 CORRECT
    case P2IV_P2IFG2:   // UP
        if (row != 0) {                             // if not on top row
            matrix[rowcnt - 1] = matrix[rowcnt];      // move up in matrix
            matrix[rowcnt] = 0;
            row--;                                  // decrementing row
        }
        break;   // Vector  6: P2.2
    case P2IV_P2IFG3:   // DOWN
        if (row < 8) {                              // if on any row above row 8
            matrix[rowcnt + 1] = matrix[rowcnt];      // move down in matrix
            matrix[rowcnt] = 0;
            row++;                                  // inrementing row
        }
        break;   // Vector  8: P2.3
    case P2IV_P2IFG4:   // RIGHT
        if (col != 1) {                             // if not on column 1
            matrix[rowcnt] = matrix[rowcnt] >> 1;   // shift 1 space to the right
            col++;                                  // incrementing column
        }
        break;   // Vector 10: P2.4 CORRECT
    case P2IV_P2IFG5: break;   // Vector 12: P2.5
    case P2IV_P2IFG6: break;   // Vector 14: P2.6
    case P2IV_P2IFG7: break;   // Vector 16: P2.7
    default: break;
    }
}

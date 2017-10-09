//***************************************************************************************
// LAB 4: TIMERS AND PWM
//
// MSP430FR6989 Debouncing
//
// Description: Debounced LED Button Blink. LED1 is toggled by Port 1 interrupt routine
//              Timer B ISR adjusts timing to nullify bouncing.
//
// ACLK = n/a, MCLK = SMCLK = default DCO
//
//           MSP430FR6989
//           -----------
//        /|\|      XIN|-
//         | |         |
//         --|RST  XOUT|-
//           |         |
// BUTTON -->|P1.1 P1.0|-->LED1
//
// Damon Boorstein
// Rowan University
// Date Created: October 9, 2017
// Date Updated: October 9, 2017
//
// Built with Code Composer Studio v7.2.0
//***************************************************************************************

#include <msp430fr6989.h>

#define LED1    BIT0
#define BUTTON  BIT1

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // Disable GPIO default mode

    P1DIR |= LED1; // Set P1.0 and P1.1 to output
    P1REN |= BUTTON; // Enable puller resistor for button-pins
    P1OUT |= BUTTON; // Sets resistor to pull up for buttons

    P1IES &= ~BUTTON; // Interrupts when button is released
    P1IE |= BUTTON; // Interrupt enable
    P1IFG &= ~BUTTON; // Clear interrupt flag

    TB0CCTL0 = CCIE; // CCR0 interrupt enabled
    TB0CCR0 = 60000;
    TB0CTL = TBSSEL_2 + MC_0 + TBCLR; // SMCLK, Stop, clear TB0R

    __enable_interrupt();

    __bis_SR_register(LPM0 + GIE); // LPM0 w/ general interrupts enabled

    for(;;){}

}

// Port 1 interrupt routine
#pragma vector = PORT1_VECTOR
__interrupt void Port_5(void){
    P1OUT ^= LED1; // Toggle LED
    P1IE &= ~BUTTON; // Clear interrupt
    TB0CTL = TBSSEL_2 + MC_1 + TBCLR; // SMCLK, Up, clear TB0R
    P1IFG &= ~BUTTON; // Clear interrupt flag
}

// Timer B0 ISR
#pragma vector=TIMER0_B0_VECTOR
__interrupt void TIMER0_B0_ISR(void){
    TB0CTL = MC_0; // Stop timer
    P1IE |= BUTTON; // Enable interrupt
    P1IFG &= ~BUTTON; // Clear interrupt flag
}

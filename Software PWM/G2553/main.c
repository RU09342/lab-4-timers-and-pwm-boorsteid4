//***************************************************************************************
// LAB 4: TIMERS AND PWM
//
// MSP430G2553 Software PWM
//
// Description: Debounced LED Button Blink with software PWM. LED1 is at 50% duty cycle,
//              LED2 is at 100% (relative to CCR0).
//
// ACLK = n/a, MCLK = SMCLK = default DCO
//
//           MSP430G2553
//           -----------
//        /|\|      XIN|-
//         | |         |
//         --|RST  XOUT|-
//           |     P1.0|-->LED1
// BUTTON -->|P1.3 P1.6|-->LED2
//
// Damon Boorstein
// Rowan University
// Date Created: October 9, 2017
// Date Updated: October 9, 2017
//
// Built with Code Composer Studio v7.2.0
//***************************************************************************************

#include <msp430g2553.h>

#define LED1    BIT0
#define LED2    BIT6
#define BUTTON  BIT3

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    P1DIR |= LED2 + LED1; // Set P1.0 and P1.6 to output
    P1OUT &= ~(LED2 + LED1); // Turn LEDs off
    P1REN |= BUTTON; // Enable puller resistor for button-pin
    P1OUT |= BUTTON; // Sets resistor to pull up for button

    P1IES &= ~BUTTON; // Interrupts when button is released
    P1IE |= BUTTON; // Interrupt enable
    P1IFG &= ~BUTTON; // Clear interrupt flag

    TA0CCTL0 = CCIE; // CCR0 interrupt enabled
    TA0CCR0 = 1000; // Duty cycle
    TA0CTL = TASSEL_2 + MC_0 + TACLR; // SMCLK, Stop, clear TA0R

    TA0CCTL1 = CCIE; // CCR1 interrupt enabled
    TA0CCR1 = 500; // 50% duty cycle
    TA0CTL = TASSEL_2 + MC_1 + TACLR; // SMCLK, Up, clear TA0R

    TA1CCTL0 = CCIE;
    TA1CCR0 = 60000;
    TA1CTL = TASSEL_2 + MC_1 + TACLR;


    __enable_interrupt();

    __bis_SR_register(LPM0 + GIE); // LPM0 w/ general interrupts enabled

    for(;;){}

}

// Port 1 interrupt routine
#pragma vector = PORT1_VECTOR
__interrupt void Port_1(void){
    P1OUT ^= LED1; // Toggle LED
    P1IE &= ~BUTTON; // Clear interrupt
    TA0CTL = TASSEL_2 + MC_1 + TACLR; // SMCLK, Up, clear TA0R
    P1IFG &= ~BUTTON; //Clears interupt flag register needed otherwise will be stuck in infinite loop
}

// Timer1 A0 ISR
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer1_A0(void){
    TA0CTL = MC_0; // Stop timer
    P1IE |= BUTTON; // Enable interrupt
    P1IFG &= ~BUTTON; // Clear interrupt flag
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer0_A1_ISR (void)
{
    if(TA0CCR1 != 1000)
    {
       P1OUT &= ~LED1;
    }
    TA0CCTL1 &= ~CCIFG;    // Clear CCR interrupt flag
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0_ISR (void)
{
    P1OUT |= LED2;
    TA0CCTL0 &= ~CCIFG;    // Clear CCR interrupt flag
}

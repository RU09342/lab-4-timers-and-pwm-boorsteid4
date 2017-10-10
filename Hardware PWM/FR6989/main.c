//***************************************************************************************
// LAB 4: TIMERS AND PWM
//
// MSP430FR6989 Hardware PWM
//
// Description: Debounced LED Button Blink with hardware PWM. LED1 is at 50% duty cycle,
//              LED2 is at 100% (relative to CCR0) and used to acknowledge button press.
//
// ACLK = n/a, MCLK = SMCLK = default DCO
//
//           MSP430FR2311
//           -----------
//        /|\|      XIN|-
//         | |         |
//         --|RST  XOUT|-
// BUTTON1-->|P1.1 P1.0|-->LED1
//           |     P9.7|-->LED2
//
// Damon Boorstein
// Rowan University
// Date Created: October 9, 2017
// Date Updated: October 9, 2017
//
// Built with Code Composer Studio v7.2.0
//***************************************************************************************

#include <msp430.h>

#define BUTTON  BIT1
#define LED1    BIT0
#define LED2    BIT7

unsigned int x = 625;
unsigned int buttonDown = 0;
unsigned int count = 0;

void initTimer();

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;  // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // Disable GPIO default high-impedance mode

    //P1SEL1 + P1SEL0 = 01 ==> Primary peripheral func.
    P1SEL0 |= LED1; // Select primary peripheral module function on P1.0
    P1DIR |= LED1; // Set P1.0 to output
    P9DIR |= LED2; // Set P9.7 to output
    P9OUT &= ~LED2; // Turn LED2 off
    P1OUT &= ~LED1; // Turn LED1 off
    P1DIR &= ~BUTTON; // Set P2.1 as input
    P1REN |= BUTTON; // Enable puller resistor on button-pin
    P1OUT |= BUTTON; // Tell resistor to pull up

    P1IES |= BUTTON; // Interrupts when button is released
    P1IE |= BUTTON; // Enable interrupt on button-pin
    P1IFG &= ~BUTTON;//clear interrupt flag

    initTimer();

    __enable_interrupt();

   __bis_SR_register(LPM0 + GIE); // Enable LPM0 with general interrupts

    while(1) {}
}

void initTimer(void)
{
    TA0CCR0 = 1250 - 1; // Period
    TA0CCTL1 = OUTMOD_7; // Output = Reset/Set
    TA0CCR1 = 625 - 1; // 50% Duty Cycle
    TA0CTL = TBSSEL__SMCLK + MC__UP; // SMCLK, Up mode

    TA1CCR0 = 4000; // Period for debouncing
    TA1CCTL0 |= CCIE; // Compare mode
    TA1CTL |= TBSSEL__SMCLK + MC__UP; // SMCLK, Up mode

}

// Timer0 B0 interrupt routine (used for debouncing)
#pragma vector = TIMER1_A0_VECTOR
__interrupt void debounce(void)
{
    if (buttonDown == 1) // if we just pressed the button, debounce
    {
        if (count == 100)
        {
            buttonDown = 0;
            count = 0;
        }

        else
            count++;
    }
}

#pragma vector = PORT1_VECTOR
__interrupt void PORT_1(void)
{
    P9OUT ^= LED2;  // Toggle LED2
    if (!(P1IN & BUTTON)) // If button has not been released yet...
    {
        if (buttonDown == 0) // and if buttonDown is false
        {
            buttonDown = 1; // Debounce

            if (x >= 1250)
                x = 1; // Reset TA0CCR1 (TA0CCR1 = 1 - 1 = 0)

            else
                x += 125; // Increment Duty Cycle

            TA0CCR1 = x - 1; // Set value for CCR1
        }
    }

    P1IES ^= BUTTON; // Toggle to allow for redo LED toggle when button is released
    P1IFG &= ~BUTTON; // Clear P2 interrupt flag

}

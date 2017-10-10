//***************************************************************************************
// LAB 4: TIMERS AND PWM
//
// MSP430FR5994 Hardware PWM
//
// Description: Debounced LED Button Blink with hardware PWM. LED1 is at 50% duty cycle,
//              LED2 is at 100% (relative to CCR0) and used to acknowledge button press.
//
// ACLK = n/a, MCLK = SMCLK = default DCO
//
//           MSP430FR5994
//           -----------
//        /|\|      XIN|-
//         | |         |
//         --|RST  XOUT|-
// BUTTON1-->|P5.6 P1.0|-->LED1
//           |     P1.1|-->LED2
//
// Damon Boorstein
// Rowan University
// Date Created: October 9, 2017
// Date Updated: October 9, 2017
//
// Built with Code Composer Studio v7.2.0
//***************************************************************************************

#include <msp430.h>

#define BUTTON  BIT6
#define LED1    BIT0
#define LED2    BIT1

unsigned int x = 625;
unsigned int buttonDown = 0;
unsigned int count = 0;

void initTimer();

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;  // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // Disable GPIO default high-impedance mode

    //P1SEL1 + P1SEL0 = 01 ==> Primary peripheral func.
    P1SEL1 &= ~LED1; // Select primary peripheral module function on P1.0
    P1SEL0 |= LED1;
    P1DIR |= LED2 + LED1; // Set P1.0 and P1.1 to output
    P1OUT &= ~(LED2 + LED1); // Turn LED1 and LED2 off
    P5DIR &= ~BUTTON; // Set P5.6 as input
    P5REN |= BUTTON; // Enable puller resistor on button-pin
    P5OUT |= BUTTON; // Tell resistor to pull up

    P5IES |= BUTTON; // Interrupts when button is released
    P5IE |= BUTTON; // Enable interrupt on button-pin
    P5IFG &= ~BUTTON;//clear interrupt flag

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
    if (buttonDown == 1) // Button is down, debounce
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

#pragma vector = PORT5_VECTOR
__interrupt void PORT_5(void)
{
    P1OUT ^= LED2;  // Toggle LED2
    if (!(P5IN & BUTTON)) // If button has not been released yet...
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

    P5IES ^= BUTTON; // Toggle to allow for redo LED toggle when button is released
    P5IFG &= ~BUTTON; // Clear P5 interrupt flag

}

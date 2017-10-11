# Software PWM
A debounced LED Button Blink with software PWM. LED1 is at 50% duty cycle, LED2 is at 100% (relative to CCR0).

## Implementation
To implement PWM you need to utilize, at most, two CCRs among two timers. You will most likely need to use TA0 and TA1, depending on the microprocessor, and both CCR0 and CCR1. Timer A0 is used primarily as the PWM timer. Setting up all registers as usual, you will need to instantiate a port interrupt vector for however many buttons you intend to use. You will also need to create a timer interrupt routine for each timer that you use.

In my current implementation, I have TA0CCR0 set to 1000 and TA0CCR1 set to 500. TA0 is intially stopped, then after intializing TA0CCR1, it is switched into Up mode. TA1 is used as the debounce timer, and as such, its CCR0 is set to 60000, and its count mode Up. LED2 is set to 100% duty cycle to compare to the PWM'ed LED1.

The current program is faulty and does not work as intended. The button blinks for a moment at 50% duty cycle once the button is pressed. 
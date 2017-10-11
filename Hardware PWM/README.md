# Hardware PWM
Debounced LED Button Blink with hardware PWM. LED1 is at 50% duty cycle, LED2 is at 100% (relative to CCR0) and used to acknowledge button press.

## Implementation

Different to software PWM, hardware PWM uses the internal timer output modules to control PWM. By writing `TAxCCTLn = OUTMOD_7` you change the output mode to 'reset/set'. In human tongue, this means to _reset_ the output at TAxCCRn and _set_ the output at TAxCCR0. With this logic, we can produce PWM without using additional software. The only thing in this exercise that uses software is the debouncing code.

### Setting the PxSEL Register Correctly

To use the hardware functions needed for this exercise, the PxSEL registers must be set up correctly. In most of the cases here, we must activate what is known as peripheral mode 1 for the LED pin. This can be achieved by writing `PxSEL |= BITn` in your initialization code. Check the datasheet relevant to the board you are using for information on how to set the PxSEL bits to activate peripheral mode 1.

### G2553 and F5529

For the F5529, you cannot do hardware PWM directly on the LED pin for some odd reason. So, set a different GPIO pin as your output pin and then jump that pin with P1.0 (located by default inside a jumper; remove the jumper JP8 on the lower part of the board near the LEDs, P1.0 is the bottom-most pin). This should work, although my code doesn't. The G2553 doesn't work either for some reason.

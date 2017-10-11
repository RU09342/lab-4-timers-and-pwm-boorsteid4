# Software Debouncing
Using software, a button is debounced. Port interrupt vectors are used in conjunction with Timer A interrupts to delay cycles, leaving enough time for button to stop bouncing.

## Implementation
A very simple solution to stop button bouncing is to wait for an input to achieve a steady-state before making any decisions. Button bouncing occurs for a short period of time, about .5 microseconds. This can be seen in this oscilloscope screenshot: ![BOUNCING BUTTONS!!!](https://cdn.discordapp.com/attachments/315504933426167809/366992553632858112/scope_2.png). 

Therefore, by setting CCR0 to 60000 (equivalent to a 60 ms delay), you delay for more than enough time so that the button can finish bouncing. At this point, the interrupt will fire and the code will be executed.

## Ultra-Low Power Mode
This code:

```c
__bis_SR_register(LPM0 + GIE); // LPM0 w/ general interrupts enabled
```

sends the microprocessor into Low Power Mode 0 with general interrupts enabled. In LPM0, the CPU is shut off, and only general interrupts can wake the chip up. This saves a lot of energy that would have been expended by the processor just sitting and doing nothing. By adding this, you assure that your processor is not using any more energy or power than it should on the tasks you need it to do (such as blinking LEDs).

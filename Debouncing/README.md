# Software Debouncing
Using software, a button is debounced. Port interrupt vectors are used in conjunction with Timer A interrupts to delay cycles, leaving enough time for button to stop bouncing.

## Implementation
A very simple solution to stop button bouncing is to wait for an input to achieve a steady-state before making any decisions. Button bouncing occurs for a short period of time, about .5 microseconds. Therefore, by setting CCR0 to 60000 (equivalent to a 60 ms delay) is more than enough time for the button to finish debouncing. At this point, the interrupt will fire and the code will be executed.




# ATTiny 4313 USART Keypad



> An ATMEL ATTINY 4313-based very low power keypad -> USART bridge. 

When building a project it can sometimes be annoying to receive user input from button presses. Typically you have to deal with things like debouncing your button to prevent spurious input, finding enough free GPIO pins to support the number of buttons you want, and setting up an efficient way to poll the buttons for state change (either through interrupts or a polling loop). These problems can become even worse when dealing with a RTOS-based project, as you need to ensure that your button implementation does not interfere with threads on the system (for example, by temporarily disabling interrupts). This project helps solve these problems by providing a complete solution for up to 12 buttons on a self-contained very low-power uController. You are able to simply connect your buttons to the uController, and then connect the chip to your project on a free UART port. You'll then receive a nice clean stream of button presses  without much setup headache!
. 

### Main Features ###
- Very low idle current consumption <img src="https://raw.githubusercontent.com/dretay/tiny_uart_keypad_controller/master/pics/idle.png" align="right" width="208">
  - Entire solution is interrupt driven, so uController only wakes when a key has been pressed 
  - Built-in timer-based button debouncing, so no need for supporting passives to filter inputs
- uControlled has been clocked down to 1Mhz, so current consumption when running is only 0.5mA.  <img src="https://raw.githubusercontent.com/dretay/tiny_uart_keypad_controller/master/pics/button_press.png" align="right" width="208">
  - As shown to the right power saving is so aggressive even the "idle" time between pressing and releasing a button is accounted for. 
  - Uses ATYINY's internal RC oscillator, so no need to install a separate crystal for proper functionality. 

### Build Instructions ###
#### Atmel Studio 7.0
 -  You should just be able to check the project out and open it without issue. 
 - Only real value to be edited is the number of ticks used when [debouncing]([https://github.com/dretay/tiny_uart_keypad_controller/blob/master/tiny_usart_keypad_controller/main.c#L22] (https://github.com/dretay/tiny_uart_keypad_controller/blob/master/tiny_usart_keypad_controller/main.c#L22)) an input.
#### Breadboard
 - Setting up the circuit is very straightforward. PD1 is UART TX (9600 8n1). Almost all other pins are available as inputs, and use the 4313's internal pullups to reduce supporting components. The pins map to the output message as follows 
  - <img src="https://raw.githubusercontent.com/dretay/tiny_uart_keypad_controller/master/pics/attiny2313a_4313a.png"  height="308">

 | Pin| Message Index | Pin |Message Index
 | -- | -- | -- | -- |
 | B0| 0|D3 | 6|
 | B1| 1|D4 | 7|
 | B2 | 2|D5 | 8|
 | B3 | 3|D6 | 9|
 | B4 | 4|A0 | 10 |
 | D2 | 5|A1 | 11 |
 <img src="https://raw.githubusercontent.com/dretay/tiny_uart_keypad_controller/master/pics/IMG_5061.jpg" align="right">
 
## References
> Software tools, hardware, and useful articles

### Software ###
- [Atmel Studio](https://www.microchip.com/mplab/avr-support/atmel-studio-7)
- [AVR Sleep](http://www.nongnu.org/avr-libc/user-manual/group__avr__sleep.html)
- [Hackaday Button Debouncing](https://hackaday.com/2010/11/09/debounce-code-one-post-to-rule-them-all/) 

### Hardware ###
- [ATMEL ATTINY 4313](https://www.microchip.com/wwwproducts/en/ATtiny4313)
- [ATMEL ICE](https://www.microchip.com/DevelopmentTools/ProductDetails/ATATMEL-ICE)

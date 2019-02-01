Simple example how to use HD44780 with STM32F0xx
For output information used 4  LCD pins D4..D8
Pin description : Pb4...Pb7 - LCD D4..D7, Pb3 - LCD Rs, Pa10 - LCD E, Pb0 - Blinking Led
in this Example i'm not use recommended microseconds delays for HD44780 , 
if you need it  - change TIM17 configuration

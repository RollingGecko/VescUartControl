VescUartControl library

Library for arduino to interface over UART with the Vesc BLDC controler (http://vedder.se/2015/01/vesc-open-source-esc/)

The files libraries

crc

datatypes

buffer

are directly forked from https://github.com/vedderb/bldc

datatypes.h was adapted.

All available UART handlers the VESC can deal with can be found in the file commands.c (https://github.com/vedderb/bldc.)
in the function commands_process_packet. You can write easily own handler functions. Use converting functions in 
the library buffer.c.


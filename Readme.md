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


Some details to the UART port used in the VESC

It is a uint8_t byte stream. 
First byte 
0x02 for payload length of 256 byte
next byte is for the payload length 

0x03 for >256 byte payload length
next 2 byte for the payload length

The follwing 2 bytes after teh payload are the checksum. (see crc.h)

The byte stream it terminated with a 0x03.

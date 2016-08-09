#VescUartControl library

Library for arduino to interface over UART with the Vesc BLDC controler (http://vedder.se/2015/01/vesc-open-source-esc/)
It is used in the ArduBoardControl. Refer here: https://github.com/RollingGecko/ArduBoardControler

The files libraries

crc

datatypes

buffer

are directly forked from https://github.com/vedderb/bldc


All available UART handlers the VESC can deal with can be found in the file commands.c (https://github.com/vedderb/bldc)
in the function commands_process_packet. You can write easily own handler functions. Use converting functions in 
the library buffer.c.

The rest shut be comment sufficient in the VescUart.h. Take also a look to the RX-Site of the ArduBoardControler (https://github.com/RollingGecko/ArduBoardControler)

##Requirements to use this library on bldc FW

The needed changes where already merged by Vedder to the FW. :)

In bldc-tool please activate UART and if needed the nunchuk application. 



##Some details to the UART port used in the VESC

It is a uint8_t byte stream. 

First byte: 

0x02 for payload length of 256 byte >> next byte is for the payload length 

0x03 for >256 byte payload length  >> next 2 byte for the payload length

The follwing 2 bytes after the payload are the checksum. (see crc.h)

The byte stream it terminated with a 0x03.

For more details please refer also to http://vedder.se/2015/10/communicating-with-the-vesc-using-uart/




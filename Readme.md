VescUartControl library
Copyright 2015 - 2017 Andreas Chaitidis Andreas.Chaitidis@gmail.com

Copyright 2015 - 2017 Andreas Chaitidis Andreas.Chaitidis@gmail.com

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.

Library for arduino to interface over UART with the Vesc BLDC controler (http://vedder.se/2015/01/vesc-open-source-esc/)

The files libraries

crc
datatypes
buffer

are directly forked from https://github.com/vedderb/bldc

datatypes.h was adapted.

All available UART handlers the VESC can deal with can be found in the file commands.c ()https://github.com/vedderb/bldc.)
in the function commands_process_packet. You can write easily own handler functions. Use converting functions in 
the library buffer.c.


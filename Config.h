/*
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
*/

//The Config.h is a file, that I use in other programs usually. So I define also the serial ports there. If you don't want to
//use it, just comment the include statement in the VescUart.h out.

#ifndef _CONFIG_h
#define _CONFIG_h

  #define SERIALIO Serial1  
  #define DEBUGSERIAL Serial

//Toggle in, when you have multiple SErial Ports with multiple VESCs attached. In this implementation you can can use 1 Serial port or 4 Serial ports 
//(Serial0-3)(Mega2560). When toggled out, you use the HardwareSerial Port Serial.
#define MULTI_SERIAL

#endif

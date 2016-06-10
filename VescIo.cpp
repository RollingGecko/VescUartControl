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

#include "VescIo.h"

void VescIo::setMaxMessageSize(int value)
{
	maxMessageSize = value;
}

void VescIo::flushReceivedMessage(uint8_t* message, int length)
{
	for (int i = 0; i < length; i++)
	{
		message[i] = NULL;
	}
}

VescUartIo::VescUartIo(HardwareSerial* port)
{
	setSerialPort(port);
}

void VescUartIo::setSerialPort(HardwareSerial* usedSerial) {
	_Serial = usedSerial;
}

boolean VescUartIo::sendMessage(uint8_t * message, int lengthMessage)
{

	if (lengthMessage > maxMessageSize)
	{
		return false;
	}
	int lengthSend = _Serial->write(message, lengthMessage);

	if (lengthSend == lengthMessage)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int VescUartIo::receiveMessage(uint8_t* message, int lengthMessage)
{
	int index = 0;
	int endMessage = 256;

	if (lengthMessage > maxMessageSize)
	{
		return 0;
	}

	if (_Serial->available())
	{
		flushReceivedMessage(message, lengthMessage);
	}

	//while (_Serial->available())
	while (true)
	{
		int i = 0;
		while (!_Serial->available()&&++i<1000) //In Data stream are breaks. This Filters them out; ++i<1000 to avoid endless loops
		{
			}
		message[index++] = _Serial->read();
	
		if (index == 2) //case if state of 'counter' with last read 1
		{
			switch (message[0])
			{
			case 2:
				endMessage = message[1] + 5; //Payload size + 2 for sice + 3 for SRC and End.
				break;
			case 3:
				//ToDo: Add Message Handling > 255 (starting with 3)
				break;
			default:
				break;
			}
		}

		//Only defined package size allowed

		if (index > lengthMessage)
		{
			flushReceivedMessage(message, lengthMessage);
			return 0;
		}

		if (index == endMessage && message[endMessage - 1] == 3) {//+1: Because of counter++ state of 'counter' with last read = "endMessage"
			message[endMessage] = 0;

			break; //Exit if end of message is reached, even if there is still more data in buffer. 
		}

	}
	return index;
}

boolean VescUartIo::isMessageAvailable()
{
	return _Serial->available();
}

void VescUartIo::begin(unsigned int baud)
{
	_Serial->begin(baud);
}



int VescNrfIo::receiveMessage(uint8_t* message, int lengthMessage)
{
	return 0;
}

boolean VescNrfIo::isMessageAvailable()
{
	return boolean();
}


boolean VescNrfIo::sendMessage(uint8_t * message, int lengthMessage)
{
	return boolean();
}


// VescIo.h

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

#ifndef _VESCIO_h
#define _VESCIO_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

//Interface class

class VescIo
{
public:
	VescIo()
	{
	}
	~VescIo()
	{
	}

	virtual boolean sendMessage(uint8_t *message, int lengthMessage) = 0;
	virtual int receiveMessage(uint8_t* message, int lengthMessage) = 0;
	virtual boolean isMessageAvailable() = 0;
	void setMaxMessageSize(int value);

protected:
	int maxMessageSize = 31;
	void flushReceivedMessage(uint8_t* message, int length);

private:


};

class VescUartIo : public VescIo
{
public:
	VescUartIo(HardwareSerial* port);
	~VescUartIo() {};



	// Inherited via VescIo
	virtual boolean sendMessage(uint8_t * message, int lengthMessage) override;

	virtual int receiveMessage(uint8_t* message, int lengthMessage) override;

	virtual boolean isMessageAvailable() override;

	void setSerialPort(HardwareSerial* usedSerial);

	void begin(unsigned int baud);  //Do not use. Produces error. (ToDo)


private:
	HardwareSerial* _Serial;


};

class VescNrfIo : VescIo
{
public:

	// Inherited via VescIo
	virtual int receiveMessage(uint8_t* message, int lengthMessage) override;

	virtual boolean isMessageAvailable() override;

	virtual boolean sendMessage(uint8_t * message, int lengthMessage) override;
private:


};

#endif


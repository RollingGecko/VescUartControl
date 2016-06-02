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
#ifndef _VESCUART_H
#define _VESCUART_H
#pragma once

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "local_datatypes.h"
#include "datatypes.h"
class VescUart
{
public:
	VescUart(HardwareSerial *usedSerial);
	~VescUart();

	void begin(unsigned int baud);

	void SetUartSerial(HardwareSerial *usedSerial);

	///Help Function to print struct bldcMeasure over Serial for Debug
	///Define in a Config.h the DEBUGSERIAL you want to use

	
	void SerialPrint(HardwareSerial *debugSerial);

	///Help Function to print uint8_t array over Serial for Debug
	///@parm Serialport you want to use, data array and length of the array

	void SerialPrint(HardwareSerial *debugSerial, uint8_t* data, int len);

	///Sends a command to VESC to send telemetry valzes and stores the returned data
	///@param void; Data is stored in Class
	//@return true if sucess
	bool VescUartGetValue(void);

	///Sends a command to VESC to control the motor current
	///@param current as float with the current for the motor

	void VescUartSetCurrent(float current);

	///Sends a command to VESC to control the motor brake
	///@param breakCurrent as float with the current for the brake

	void VescUartSetCurrentBrake(float brakeCurrent);

	///Sends values of a joystick and 2 buttons to VESC to control the nunchuk app

	void VescUartSetNunchukValues(remotePackage& data);

	///Contains all measured Values of Vesc

	struct bldcMeasure vescMeasuredValues;

private:
	///SendMessage sends the data over the UART port
	///
	///@parm message is the message to be send and lenMessage it'S size
	
	SendMessage(uint8_t* message, int lenMessage);
	
	///Packs the payload in a message as preperation for sending the data to VESC
	///@parm payload, length of payload and Message array
	///@return the number of bytes packed
	int PackPayload(uint8_t* payload, int lenPay, uint8_t* messageSend);

	
	///PackSendPayload Packs the payload and sends it over Serial.
	///@param: payload as the payload [unit8_t Array] with length of int lenPayload
	///@return the number of bytes send
	void inline  PackSendPayload(uint8_t* payloadToSend, int lengthPayload);

	///ReceiveUartMessage receives the from VESC message over Serial
	///@parm the payload as the payload [unit8_t Array]
	///@return the number of bytes receeived within the payload

	int ReceiveMessage(uint8_t* payloadReceived);

	///Unpacks a message from VESC and returns the payload in the referenced array
	///
	///@parm: Message from VESC and its length, payload and its length
	///@return true if success

	bool UnpackPayload(uint8_t* message, int lenMes, uint8_t* payload, int lenPa)


	///Handler for the payload from VESC
	///
	///@parm: Message payload and its length
	///@retun true if payload could be handeled

	bool ProcessReadPayload(uint8_t* message, int len);

	HardwareSerial *_Serial;

	
	
};

#endif
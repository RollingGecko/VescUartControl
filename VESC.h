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

#ifndef _VESC_h
#define _VESC_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "datatypes.h"
#include "VescIo.h"

class Vesc {

	VescIo* IOInterface; //used communication interface
	
	//Data declaration 

	//Define remote Package
	struct remotePackage {

		int		valXJoy;
		int		valYJoy;
		boolean	valUpperButton;
		boolean	valLowerButton;
	};

	//store measured values
	struct typeTelemetryData {
		//7 Values int16_t not read(14 byte)
		float avgMotorCurrent;
		float avgInputCurrent;
		float dutyCycleNow;
		long rpm;
		float inpVoltage;
		float ampHours;
		float ampHoursCharged;
		//2 values int32_t not read (8 byte)
		long tachometer;
		long tachometerAbs;
	};

public:
	Vesc(VescIo* Interface) { //declare corresponding IO-Interface to VESC when initializing instance
		this->IOInterface = Interface;
	}

	~Vesc() {};

	//Methods to get or set data from/to VESC
	boolean GetTelemetryValues(void);
	void SetCurrent(float current);
	void SetCurrentBrake(float brakeCurrent);
	void SetNunchukValues(remotePackage& data);

	COMM_PACKET_ID ReceiveHandleMessage(); //get and handle messages from VESC

    struct typeTelemetryData telemetryDataFromVesc; //received telemetry data is stored here in direct access

	///Help Functions: 
	void SerialPrint(HardwareSerial &debugSerial); //to print struct bldcMeasure over Serial for Debug
		
		 
	void SerialPrint(HardwareSerial& debugSerial, uint8_t* data, int len); //to print uint8_t array over Serial for Debug

private:

	void inline  PackSendPayload(uint8_t* payloadToSend, int lengthPayload);
	//uses:
	

	int PackPayload(uint8_t* payload, int lenPay, uint8_t* messageSend); 

	//uses:
	int UnpackPayload(uint8_t* message, int lenMes, uint8_t* payload, int lenPa);

	COMM_PACKET_ID HandlePayload(uint8_t* payload, int len);
};



#endif


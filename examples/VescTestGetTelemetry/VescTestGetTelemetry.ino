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
#include "VESC.h"
#include "datatypes.h"

#define DEBUGSERIAL Serial
#define IOSERIAL	Serial1

VescUartIo UartIO(&IOSERIAL); //dependency Injection of Serial Port

Vesc vesc(&UartIO); // dependency injection

boolean waitForAnswer = false;
boolean sendNextRequest = true;
COMM_PACKET_ID receivedMessageType;
unsigned long lastRequest = 0;
#define WAIT_FOR_ANSWER 1000


void setup() {

	//UartIO.begin(115200); //does not work!!
	IOSERIAL.begin(115200); //ToDo
	DEBUGSERIAL.begin(115200);
	UartIO.setMaxMessageSize(100);

}

//struct bldcMeasure measuredValues;

void loop() {

	if (sendNextRequest) {
		
		if (vesc.GetTelemetryValues()) {

			DEBUGSERIAL.print("GetTelemetryValues request send at "); DEBUGSERIAL.println(millis());
			waitForAnswer = true;
			sendNextRequest = false;
			lastRequest = millis();
		}
		else
		{
			DEBUGSERIAL.println("Failed to send request!");
		}
	}

	if (waitForAnswer)
	{
		if (millis() - lastRequest >= WAIT_FOR_ANSWER) //to send new request if conection was broken
		{
			waitForAnswer = false;
			sendNextRequest = true;
		}
		receivedMessageType = vesc.ReceiveHandleMessage(); 
		DEBUGSERIAL.print("Received MessageType: "); DEBUGSERIAL.println(receivedMessageType);
		if (receivedMessageType == COMM_GET_VALUES)
		{
			sendNextRequest = true;
			waitForAnswer = false;
					
			DEBUGSERIAL.print("Answer received and handled at "); DEBUGSERIAL.println(millis());
			DEBUGSERIAL.println("ReceivedMessage: ");
			vesc.SerialPrint(DEBUGSERIAL);

			delay(2000);
			receivedMessageType = COMM_NON;
			
		}
		
	}

	

	
}






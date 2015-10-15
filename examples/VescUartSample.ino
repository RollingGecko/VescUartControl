/*
 Name:		VescUartSample.ino
 Created:	9/26/2015 10:12:38 PM
 Author:	AC
*/

// the setup function runs once when you press reset or power the board

//Include libraries copied from VESC
 #include "VescUart.h"
#include "datatypes.h"


#define DEBUG

void setup() {
	
	//Setup UART port
	Serial1.begin(115200);
#ifdef DEBUG
	//SEtup debug port
	Serial.begin(115200);
	#endif
}

struct bldcMeasure measuredValues;
	
// the loop function runs over and over again until power down or reset
void loop() {
	//int len=0;
	//len = ReceiveUartMessage(message);
	//if (len > 0)
	//{
	//	len = PackSendPayload(message, len);
	//	len = 0;
	//}

	if (VescUartGetValue(measuredValues)) {
		SerialPrint(measuredValues);
		
	}
	else
	{
		Serial.println("Failed to get data!");
	}
	
}






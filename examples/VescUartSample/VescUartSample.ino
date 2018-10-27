/*
 Name:		VescUartSample.ino
 Created:	9/26/2015 10:12:38 PM
 Author:	AC
*/

// the setup function runs once when you press reset or power the board
// To use VescUartControl stand alone you need to define a config.h file, that should contain the Serial or you have to comment the line
// #include Config.h out in VescUart.h
// This lib version tested with vesc fw 3.38 and 3.40 on teensy 3.2 and arduino uno

//Include libraries copied from VESC
#include <VescUart.h>
#include <datatypes.h>

//#define DEBUG 
#define SERIALIO Serial1
#define DEBUGSERIAL Serial // usb serial


unsigned long count;

void setup() {
	
	//Setup UART port
  SERIALIO.begin(115200);
	SetSerialPort(&SERIALIO);
	

  DEBUGSERIAL.begin(115200);
  #ifdef DEBUG
  	//SEtup debug port
  	SetDebugSerialPort(&DEBUGSERIAL);
	#endif
}

struct bldcMeasure measuredVal;
	
// the loop function runs over and over again until power down or reset
void loop() {
	//int len=0;
	//len = ReceiveUartMessage(message);
	//if (len > 0)
	//{
	//	len = PackSendPayload(message, len);
	//	len = 0;
	//}
	
	if (VescUartGetValue(measuredVal)) {
		DEBUGSERIAL.print("Loop: "); DEBUGSERIAL.println(count++);
		SerialPrint(measuredVal, &DEBUGSERIAL);
	}
	else
	{
		Serial.println("Failed to get data!");
	}
	delay(250);
}

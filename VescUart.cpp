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

#include "VescUart.h"
#include "buffer.h"
#include "crc.h"

#define SERIALIO Serial
#define DEBUGSERIAL Serial

VescUart::VescUart(HardwareSerial *usedSerial)
{
	_Serial = usedSerial;
}


VescUart::~VescUart()
{
}

void VescUart::begin(unsigned int baud) {
	_Serial->begin(baud);
}

int VescUart::ReceiveUartMessage(uint8_t* messageReceived) {

	//Messages <= 255 start with 2. 2nd byte is length
	//Messages >255 start with 3. 2nd and 3rd byte is length combined with 1st >>8 and then &0xFF

	int counter = 0;
	int endMessage = 256;
	int lenPayload = 0;

	while (_Serial->available()) {

		messageReceived[counter++] = _Serial->read();

		if (counter == 2) {//case if state of 'counter' with last read 1

			switch (messageReceived[0])
			{
			case 2:
				endMessage = messageReceived[1] + 5; //Payload size + 2 for sice + 3 for SRC and End.
				lenPayload = messageReceived[1];
				break;
			case 3:
				//ToDo: Add Message Handling > 255 (starting with 3)
				break;
			default:
				break;
			}

		}
		if (counter >= sizeof(messageReceived))
		{
			break;
		}

		if (counter == endMessage && messageReceived[endMessage - 1] == 3) {//+1: Because of counter++ state of 'counter' with last read = "endMessage"
			messageReceived[endMessage] = 0;
#ifdef DEBUG
			DEBUGSERIAL.println("End of message reached!");
#endif			
			break; //Exit if end of message is reached, even if there is still more data in buffer. 
		}
	}
	return endMessage; //returns the length of the message
}

//int VescUart::ReceiveUartMessageOld(uint8_t* payloadReceived) {
//
//	
//	bool unpacked = false;
//	if (messageRead) {
//		unpacked = UnpackPayload(messageReceived, endMessage, payloadReceived, messageReceived[1]);
//	}
//	if (unpacked)
//	{
//		return lenPayload; //Message was read
//
//	}
//	else {
//		return 0; //No Message Read
//	}
//}

bool VescUart::UnpackPayload(uint8_t* message, int lenMes, uint8_t* payload, int lenPay) {
	uint16_t crcMessage = 0;
	uint16_t crcPayload = 0;
	//Rebuild src:
	crcMessage = message[lenMes - 3] << 8;
	crcMessage &= 0xFF00;
	crcMessage += message[lenMes - 2];
#ifdef DEBUG
	DEBUGSERIAL.print("SRC received: "); DEBUGSERIAL.println(crcMessage);
#endif // DEBUG

	//Extract payload:
	memcpy(payload, &message[2], message[1]);

	crcPayload = crc16(payload, message[1]);
#ifdef DEBUG
	DEBUGSERIAL.print("SRC calc: "); DEBUGSERIAL.println(crcPayload);
#endif
	if (crcPayload == crcMessage)
	{
#ifdef DEBUG
		DEBUGSERIAL.print("Received: "); SerialPrint(message, lenMes); DEBUGSERIAL.println();
		DEBUGSERIAL.print("Payload :      "); SerialPrint(payload, message[1] - 1); DEBUGSERIAL.println();
#endif // DEBUG

		return true;
	}
	else
	{
		return false;
	}
}

int VescUart::PackPayload(uint8_t* payload, int lenPay, uint8_t* messageSend) {
	uint16_t crcPayload = crc16(payload, lenPay);
	int count = 0;
	
	if (lenPay <= 256)
	{
		messageSend[count++] = 2;
		messageSend[count++] = lenPay;
	}
	else
	{
		messageSend[count++] = 3;
		messageSend[count++] = (uint8_t)(lenPay >> 8);
		messageSend[count++] = (uint8_t)(lenPay & 0xFF);
	}
	memcpy(&messageSend[count], payload, lenPay);

	count += lenPay;
	messageSend[count++] = (uint8_t)(crcPayload >> 8);
	messageSend[count++] = (uint8_t)(crcPayload & 0xFF);
	messageSend[count++] = 3;
	messageSend[count] = NULL;

	return count;
}

void VescUart::SendMessage (uint8_t* message, int lenMessage) {
#ifdef DEBUG
	DEBUGSERIAL.print("UART package send: "); SerialPrint(messageSend, count);

#endif // DEBUG

	//Sending package
	_Serial->write(message, lenMessage);

}

//int VescUart::PackSendPayload(uint8_t* payload, int lenPay) {
//	uint16_t crcPayload = crc16(payload, lenPay);
//	int count = 0;
//	uint8_t messageSend[256];
//
//	if (lenPay <= 256)
//	{
//		messageSend[count++] = 2;
//		messageSend[count++] = lenPay;
//	}
//	else
//	{
//		messageSend[count++] = 3;
//		messageSend[count++] = (uint8_t)(lenPay >> 8);
//		messageSend[count++] = (uint8_t)(lenPay & 0xFF);
//	}
//	memcpy(&messageSend[count], payload, lenPay);
//
//	count += lenPay;
//	messageSend[count++] = (uint8_t)(crcPayload >> 8);
//	messageSend[count++] = (uint8_t)(crcPayload & 0xFF);
//	messageSend[count++] = 3;
//	messageSend[count] = NULL;
//
//#ifdef DEBUG
//	DEBUGSERIAL.print("UART package send: "); SerialPrint(messageSend, count);
//
//#endif // DEBUG
//
//	//Sending package
//	SERIALIO.write(messageSend, count);
//
//
//	//Returns number of send bytes
//	return count;
//}


bool VescUart::ProcessReadPayload(uint8_t* message, int len) {
	COMM_PACKET_ID packetId;
	int32_t ind = 0;

	packetId = (COMM_PACKET_ID)message[0];
	message++;//Eliminates the message id
	len--;

	switch (packetId)
	{
	case COMM_GET_VALUES:
		ind = 14; //Skipped the first 14 bit.
		vescMeasuredValues.avgMotorCurrent = buffer_get_float32(message, 100.0, &ind);
		vescMeasuredValues.avgInputCurrent = buffer_get_float32(message, 100.0, &ind);
		vescMeasuredValues.dutyCycleNow = buffer_get_float16(message, 1000.0, &ind);
		vescMeasuredValues.rpm = buffer_get_int32(message, &ind);
		vescMeasuredValues.inpVoltage = buffer_get_float16(message, 10.0, &ind);
		vescMeasuredValues.ampHours = buffer_get_float32(message, 10000.0, &ind);
		vescMeasuredValues.ampHoursCharged = buffer_get_float32(message, 10000.0, &ind);
		ind += 8; //Skip 9 bit
		vescMeasuredValues.tachometer = buffer_get_int32(message, &ind);
		vescMeasuredValues.tachometerAbs = buffer_get_int32(message, &ind);
		return true;
		break;

	default:
		return false;
		break;
	}

}

bool VescUart::VescUartGetValue(void) {
	uint8_t command[1] = { COMM_GET_VALUES };
	uint8_t receivedMessage[256];
	uint8_t payload[256];
	uint8_t messageToSend[256];
	//int lengMessageToSend = PackPayload(command, 1, messageToSend);
	//SendMessage(messageToSend, lengMessageToSend);
	PackSendPayload(payload, 1);
	delay(100); //needed, otherwise data is not read
	int lenMessageReceived = ReceiveUartMessage(receivedMessage);
	
	if (UnpackPayload(receivedMessage, lenMessageReceived, payload, receivedMessage[1])) {
		bool read = ProcessReadPayload(payload, receivedMessage[1]); //returns true if sucessfull
		return read;
	}
	else
	{
		return false;
	}
}

void VescUart::VescUartSetCurrent(float current) {
	int32_t index = 0;
	uint8_t payload[5];
	uint8_t messageToSend[256];

	
	payload[index++] = COMM_SET_CURRENT;
	buffer_append_int32(payload, (int32_t)(current * 1000), &index);

	//int lengMessageToSend = PackPayload(payload, 5, messageToSend);
	//SendMessage(messageToSend, lengMessageToSend);
	PackSendPayload(payload, 5);
}

void VescUart::VescUartSetCurrentBrake(float brakeCurrent) {
	int32_t index = 0;
	uint8_t payload[5];

	payload[index++] = COMM_SET_CURRENT_BRAKE;
	buffer_append_int32(payload, (int32_t)(brakeCurrent * 1000), &index);
		//uint8_t messageToSend[256];
	//int lengMessageToSend = PackPayload(payload, 5, messageToSend);
	//SendMessage(messageToSend, lengMessageToSend);

	PackSendPayload(payload, 5);

}

void VescUart::VescUartSetNunchukValues(remotePackage& data) {
	int32_t ind = 0;
	uint8_t payload[5];
	payload[ind++] = COMM_SET_CHUCK_DATA;
	payload[ind++] = data.valXJoy;
	payload[ind++] = data.valYJoy;
	buffer_append_bool(payload, data.valLowerButton, &ind);
	buffer_append_bool(payload, data.valUpperButton, &ind);

#ifdef DEBUG
	DEBUGSERIAL.println("Data reached at VescUartSetNunchuckValues:");
	DEBUGSERIAL.print("valXJoy = "); DEBUGSERIAL.print(data.valXJoy); DEBUGSERIAL.print(" valYJoy = "); DEBUGSERIAL.println(data.valYJoy);
	DEBUGSERIAL.print("LowerButton = "); DEBUGSERIAL.print(data.valLowerButton); DEBUGSERIAL.print(" UpperButton = "); DEBUGSERIAL.println(data.valUpperButton);
#endif
	//uint8_t messageToSend[256];
	//int lengMessageToSend = PackPayload(payload, 5, messageToSend);
	//SendMessage(messageToSend, lengMessageToSend);
	PackSendPayload(payload, 5);
}

void inline VescUart::PackSendPayload(uint8_t* payloadToSend, int lengthPayload) {
	uint8_t messageToSend[256];
	int lengMessageToSend = PackPayload(payloadToSend, 1, messageToSend);
	SendMessage(messageToSend, lengMessageToSend);
}

void VescUart::SerialPrint(HardwareSerial *debugSerial, uint8_t* data, int len) {

	//	DEBUGSERIAL.print("Data to display: "); DEBUGSERIAL.println(sizeof(data));

	for (int i = 0; i <= len; i++)
	{
		debugSerial->print(data[i]);
		debugSerial->print(" ");
	}
	debugSerial->println("");
}


void VescUart::SerialPrint(HardwareSerial *debugSerial) {
	debugSerial->print("avgMotorCurrent: "); debugSerial->println(vescMeasuredValues.avgMotorCurrent);
	debugSerial->print("avgInputCurrent: "); debugSerial->println(vescMeasuredValues.avgInputCurrent);
	debugSerial->print("dutyCycleNow: "); debugSerial->println(vescMeasuredValues.dutyCycleNow);
	debugSerial->print("rpm: "); debugSerial->println(vescMeasuredValues.rpm);
	debugSerial->print("inputVoltage: "); debugSerial->println(vescMeasuredValues.inpVoltage);
	debugSerial->print("ampHours: "); debugSerial->println(vescMeasuredValues.ampHours);
	debugSerial->print("ampHoursCharges: "); debugSerial->println(vescMeasuredValues.ampHoursCharged);
	debugSerial->print("tachometer: "); debugSerial->println(vescMeasuredValues.tachometer);
	debugSerial->print("tachometerAbs: "); debugSerial->println(vescMeasuredValues.tachometerAbs);
}


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


static HardwareSerial* vesc_io;
static DEBUG_SERIAL_CLASS* debugSerialPort = NULL;

bool UnpackPayload(uint8_t* message, int lenMes, uint8_t* payload, int lenPa);
bool ProcessReadPacket(uint8_t* message, struct bldcMeasure& values, int len);

void SetSerialPort(HardwareSerial* _serialPort)
{
	vesc_io = _serialPort;
}

void SetDebugSerialPort(DEBUG_SERIAL_CLASS* _debugSerialPort)
{
	debugSerialPort = _debugSerialPort;
}

int ReceiveUartMessage(uint8_t* payloadReceived, HardwareSerial* _vescserialPort) {

	//Messages <= 255 start with 2. 2nd byte is length
	//Messages >255 start with 3. 2nd and 3rd byte is length combined with 1st >>8 and then &0xFF

	int counter = 0;
	int endMessage = 256;
	int lenPayload = 0;
	uint8_t messageReceived[256];
	const char terminator = 0x03;
	bool unpacked = false;
	
	_vescserialPort->setTimeout(RX_TIMEOUT); // set timeout for messge recieve

   	counter +=  _vescserialPort->readBytes(messageReceived, 2); // get payload size

		switch (messageReceived[0]){
		case 2:
			lenPayload = messageReceived[1];
			endMessage = lenPayload + 5; //Payload size + 2 for sice + 3 for SRC and End.

			break;
		case 3:   // must be careful that rx buffer does not overflow.
			// counter +=  _vescserialPort->readBytes(&messageReceived[counter], 1); // get additional payload size byte
			// lenPayload = (messageReceived[1]<<8) + messageReceived[2]; // is this the right endian?
			// endMessage = lenPayload + 5; //Payload size + 2 for sice + 3 for SRC and End.
		
			// uint8_t* bigmessageReceived[endMessage];

			// memcpy(&messageReceived, &bigmessageReceived, counter+1);
			// delete [] messageReceived;
			// messageReceived = bigmessageReceived;
			// //ToDo: finish Adding Message Handling > 255 (starting with 3)
			break;
		default:
			break;
		}

	counter +=  _vescserialPort->readBytes(&messageReceived[counter], endMessage-counter); // get payload, crc, endbyte

	if (counter == endMessage && messageReceived[endMessage - 1] == terminator) {
		messageReceived[endMessage] = 0;
		if (debugSerialPort != NULL) {
			debugSerialPort->println("End of message reached!");
		}
		unpacked = UnpackPayload(messageReceived, endMessage, payloadReceived, messageReceived[1]);
	}

	if (unpacked){
		return lenPayload; //Message was read
	}
	else {
		return 0; //No Message Read
	}
}

bool UnpackPayload(uint8_t* message, int lenMes, uint8_t* payload, int lenPay) {
	uint16_t crcMessage = 0;
	uint16_t crcPayload = 0;
	//Rebuild src:
	crcMessage = message[lenMes - 3] << 8;
	crcMessage &= 0xFF00;
	crcMessage += message[lenMes - 2];
	if(debugSerialPort!=NULL){
		debugSerialPort->print("SRC received: "); debugSerialPort->println(crcMessage);
	} // DEBUG

	//Extract payload:
	memcpy(payload, &message[2], message[1]);

	crcPayload = crc16(payload, message[1]);
	if(debugSerialPort!=NULL){
		debugSerialPort->print("SRC calc: "); debugSerialPort->println(crcPayload);
	}
	if (crcPayload == crcMessage){
		if(debugSerialPort!=NULL){
				debugSerialPort->print("Received: "); SerialPrint(message, lenMes); debugSerialPort->println();
				//debugSerialPort->print("Payload :      "); SerialPrint(payload, message[1] - 1); debugSerialPort->println();
		} // DEBUG
		return true;
	}
	else{
		return false;
	}
}

int PackSendPayload(uint8_t* payload, int lenPay, HardwareSerial* _vescserialPort) {
	uint16_t crcPayload = crc16(payload, lenPay);
	int count = 0;
	uint8_t messageSend[256];

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

	if(debugSerialPort!=NULL){
		debugSerialPort->print("UART package send: "); SerialPrint(messageSend, count);
	} // DEBUG

	//Sending package
	_vescserialPort->write(messageSend, count);

	//Returns number of send bytes
	return count;
}


bool ProcessReadPacket(uint8_t* message, struct bldcMeasure& values, int len) {
	COMM_PACKET_ID packetId;
	int32_t ind = 0;

	packetId = (COMM_PACKET_ID)message[0];
	message++;//Eliminates the message id
	len--;

	switch (packetId)
	{

	case COMM_GET_VALUES:
		values.tempFetFiltered		= buffer_get_float16(message, 1e1, &ind);
		values.tempMotorFiltered	= buffer_get_float16(message, 1e1, &ind);
		values.avgMotorCurrent		= buffer_get_float32(message, 100.0, &ind);
		values.avgInputCurrent		= buffer_get_float32(message, 100.0, &ind);
		values.avgId				= buffer_get_float32(message, 1e2, &ind);
		values.avgIq				= buffer_get_float32(message, 1e2, &ind);
		values.dutyNow				= buffer_get_float16(message, 1000.0, &ind);
		values.rpm					= buffer_get_float32(message, 1.0, &ind);
		values.inpVoltage			= buffer_get_float16(message, 10.0, &ind);
		values.ampHours				= buffer_get_float32(message, 10000.0, &ind);
		values.ampHoursCharged		= buffer_get_float32(message, 10000.0, &ind);
		values.wattHours			= buffer_get_float32(message, 1e4, &ind);
		values.watthoursCharged		= buffer_get_float32(message, 1e4, &ind);
		values.tachometer			= buffer_get_int32(message, &ind);
		values.tachometerAbs		= buffer_get_int32(message, &ind);
		values.faultCode			= message[ind];
		return true;
		break;
	default:
		return false;
		break;
	}

}

bool VescUartGetValue(bldcMeasure& values, HardwareSerial* _vescserialPort) {
	uint8_t command[1] = { COMM_GET_VALUES };
	uint8_t payload[256];
	_vescserialPort->flush(); // move to comm function directly?
	PackSendPayload(command, 1, _vescserialPort);

	int lenPayload = ReceiveUartMessage(payload, _vescserialPort);
	if (lenPayload > 1) {
		bool read = ProcessReadPacket(payload, values, lenPayload); //returns true if sucessful
		return read;
	}
	else
	{
		return false;
	}
}
bool VescUartGetValue(bldcMeasure& values) {
	return VescUartGetValue(values, vesc_io);
}

void VescUartSetCurrent(float current, HardwareSerial* _vescserialPort) {
	int32_t index = 0;
	uint8_t payload[5];

	payload[index++] = COMM_SET_CURRENT ;
	buffer_append_int32(payload, (int32_t)(current * 1000), &index);
	PackSendPayload(payload, 5, _vescserialPort);
}
void VescUartSetCurrent(float current){
	VescUartSetCurrent(current, 0);
}

void VescUartSetPosition(float position, HardwareSerial* _vescserialPort) {
	int32_t index = 0;
	uint8_t payload[5];

	payload[index++] = COMM_SET_POS ;
	buffer_append_int32(payload, (int32_t)(position * 1000000.0), &index);
	PackSendPayload(payload, 5, _vescserialPort);
}
void VescUartSetPosition(float position) {
	VescUartSetPosition(position, vesc_io);
}

void VescUartSetDuty(float duty, HardwareSerial* _vescserialPort) {
	int32_t index = 0;
	uint8_t payload[5];

	payload[index++] = COMM_SET_DUTY ;
	buffer_append_int32(payload, (int32_t)(duty * 100000), &index);
	PackSendPayload(payload, 5, _vescserialPort);
}
void VescUartSetDuty(float duty) {
	VescUartSetDuty(duty, vesc_io);
}


void VescUartSetRPM(float rpm, HardwareSerial* _vescserialPort) {
	int32_t index = 0;
	uint8_t payload[5];

	payload[index++] = COMM_SET_RPM ;
	buffer_append_int32(payload, (int32_t)(rpm), &index);
	PackSendPayload(payload, 5, _vescserialPort);
}
void VescUartSetRPM(float rpm) {
	VescUartSetRPM(rpm, vesc_io);
}

void VescUartSetCurrentBrake(float brakeCurrent, HardwareSerial* _vescserialPort) {
	int32_t index = 0;
	uint8_t payload[5];

	payload[index++] = COMM_SET_CURRENT_BRAKE;
	buffer_append_int32(payload, (int32_t)(brakeCurrent * 1000), &index);
	PackSendPayload(payload, 5, _vescserialPort);
}
void VescUartSetCurrentBrake(float brakeCurrent) {
	VescUartSetCurrentBrake(brakeCurrent, vesc_io);
}


void VescUartSetNunchukValues(remotePackage& data, HardwareSerial* _vescserialPort) {
	int32_t ind = 0;
	uint8_t payload[11];
	payload[ind++] = COMM_SET_CHUCK_DATA;
	payload[ind++] = data.valXJoy;
	payload[ind++] = data.valYJoy;
	buffer_append_bool(payload, data.valLowerButton, &ind);
	buffer_append_bool(payload, data.valUpperButton, &ind);
	//Acceleration Data. Not used, Int16 (2 byte)
	payload[ind++] = 0;
	payload[ind++] = 0;
	payload[ind++] = 0;
	payload[ind++] = 0;
	payload[ind++] = 0;
	payload[ind++] = 0;

	if(debugSerialPort!=NULL){
		debugSerialPort->println("Data reached at VescUartSetNunchuckValues:");
		debugSerialPort->print("valXJoy = "); debugSerialPort->print(data.valXJoy); debugSerialPort->print(" valYJoy = "); debugSerialPort->println(data.valYJoy);
		debugSerialPort->print("LowerButton = "); debugSerialPort->print(data.valLowerButton); debugSerialPort->print(" UpperButton = "); debugSerialPort->println(data.valUpperButton);
	}

	PackSendPayload(payload, 11, _vescserialPort);
}
void VescUartSetNunchukValues(remotePackage& data) {
	VescUartSetNunchukValues(data, vesc_io);
}


void SerialPrint(uint8_t* data, int len) {

	debugSerialPort->print("Data to display: "); debugSerialPort->println(len);

	for (int i = 0; i <= len; i++)
	{
		debugSerialPort->print(data[i]);
		debugSerialPort->print(" ");
	}
	debugSerialPort->println("");
}


void SerialPrint(const struct bldcMeasure& values, DEBUG_SERIAL_CLASS*  print_serialPort ) {
	print_serialPort->print("tempFetFiltered:	"); print_serialPort->println(values.tempFetFiltered);
	print_serialPort->print("tempMotorFiltered:"); print_serialPort->println(values.tempMotorFiltered);
	print_serialPort->print("avgMotorCurrent:	"); print_serialPort->println(values.avgMotorCurrent);
	print_serialPort->print("avgInputCurrent:	"); print_serialPort->println(values.avgInputCurrent);
	print_serialPort->print("avgId:			"); print_serialPort->println(values.avgId);
	print_serialPort->print("avgIq:			"); print_serialPort->println(values.avgIq);
	print_serialPort->print("dutyNow:			"); print_serialPort->println(values.dutyNow);
	print_serialPort->print("rpm:				"); print_serialPort->println(values.rpm);
	print_serialPort->print("inpVoltage:		"); print_serialPort->println(values.inpVoltage);
	print_serialPort->print("ampHours:		"); print_serialPort->println(values.ampHours);
	print_serialPort->print("ampHoursCharged:	"); print_serialPort->println(values.ampHoursCharged);
	print_serialPort->print("tachometer:		"); print_serialPort->println(values.tachometer);
	print_serialPort->print("tachometerAbs:	"); print_serialPort->println(values.tachometerAbs);
	print_serialPort->print("faultCode:		"); print_serialPort->println(values.faultCode);
}

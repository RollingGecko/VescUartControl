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

#include "VESC.h"
#include "VescIo.h"
#include "crc.h"
#include "buffer.h"

boolean Vesc::sendRequestGetTelemetryValues(void)
{
	uint8_t payload[1];
	payload[0] = COMM_GET_VALUES;
	boolean sendResult = PackSendPayload(payload, 1);
	if (sendResult)
	{
		memRunningRequests.waitForReceiveGetTelemetryValues = true;
	}

	return sendResult;
}

boolean Vesc::statusRequestGetTelemetryValues(void) {
	
	return memRunningRequests.waitForReceiveGetTelemetryValues;

}



void Vesc::sendSetCurrent(float current)
{
	int32_t index = 0;
	uint8_t payload[5];

	payload[index++] = COMM_SET_CURRENT;
	buffer_append_int32(payload, (int32_t)(current * 1000), &index);
	PackSendPayload(payload, 5);
}

void Vesc::sendSetCurrentBrake(float brakeCurrent)
{
	int32_t index = 0;
	uint8_t payload[5];

	payload[index++] = COMM_SET_CURRENT_BRAKE;
	buffer_append_int32(payload, (int32_t)(brakeCurrent * 1000), &index);
	PackSendPayload(payload, 5);
}

boolean Vesc::SetNunchukValues(remotePackage& data)
{
		int32_t ind = 0;
		uint8_t payload[5];
		payload[ind++] = COMM_SET_CHUCK_DATA;
		payload[ind++] = data.valXJoy;
		payload[ind++] = data.valYJoy;
		buffer_append_bool(payload, data.valLowerButton, &ind);
		buffer_append_bool(payload, data.valUpperButton, &ind);

		return PackSendPayload(payload, 5);
}

int Vesc :: PackPayload(uint8_t * payload, int lenPay, uint8_t * messageSend)
{
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

boolean inline  Vesc::PackSendPayload(uint8_t * payloadToSend, int lengthPayload)
{
	uint8_t messageToSend[256];
	int lengMessageToSend = PackPayload(payloadToSend, lengthPayload, messageToSend);
	return IOInterface->sendMessage(messageToSend, lengMessageToSend);
}

COMM_PACKET_ID Vesc::ReceiveHandleMessage()
{
	uint8_t message[100];
	uint8_t payload[100];
	int lengthMessage = 0;
	int lengthPayload = 0;

	if (IOInterface->isMessageAvailable())
	{
		lengthMessage = IOInterface->receiveMessage(message, 100);
	}
	else
	{
		return COMM_NON;
	}
	lengthPayload = UnpackPayload(message, lengthMessage, payload, 100); 
	COMM_PACKET_ID packetID = HandlePayload(payload, lengthPayload);
	return packetID;
	
}

void Vesc::SerialPrint(HardwareSerial &debugSerial)
{
	debugSerial.print("avgMotorCurrent: "); debugSerial.println(telemetryDataFromVesc.avgMotorCurrent);
	debugSerial.print("avgInputCurrent: "); debugSerial.println(telemetryDataFromVesc.avgInputCurrent);
	debugSerial.print("dutyCycleNow: "); debugSerial.println(telemetryDataFromVesc.dutyCycleNow);
	debugSerial.print("rpm: "); debugSerial.println(telemetryDataFromVesc.rpm);
	debugSerial.print("inputVoltage: "); debugSerial.println(telemetryDataFromVesc.inpVoltage);
	debugSerial.print("ampHours: "); debugSerial.println(telemetryDataFromVesc.ampHours);
	debugSerial.print("ampHoursCharges: "); debugSerial.println(telemetryDataFromVesc.ampHoursCharged);
	debugSerial.print("tachometer: "); debugSerial.println(telemetryDataFromVesc.tachometer);
	debugSerial.print("tachometerAbs: "); debugSerial.println(telemetryDataFromVesc.tachometerAbs);
}

void Vesc::SerialPrint(HardwareSerial& debugSerial, uint8_t * data, int len)
{
	for (int i = 0; i <= len; i++)
	{
		debugSerial.print(data[i]);
		debugSerial.print(" ");
	}
	debugSerial.println("");
}

int Vesc::UnpackPayload(uint8_t * message, int lenMes, uint8_t * payload, int lenPa) 
{
	uint16_t crcMessage = 0;
	uint16_t crcPayload = 0;
	//Rebuild src:
	crcMessage = message[lenMes - 3] << 8;
	crcMessage &= 0xFF00;
	crcMessage += message[lenMes - 2];

	//Extract payload:
	memcpy(payload, &message[2], message[1]); //message[1] has the message size

	crcPayload = crc16(payload, message[1]);

	if (crcPayload == crcMessage)
	{
		return message[1]; //returns the message size
	}
	else
	{
		return 0;
	}
}

COMM_PACKET_ID Vesc::HandlePayload(uint8_t* payload, int len)
{
	COMM_PACKET_ID packetId;
	int32_t ind = 0;
	packetId = (COMM_PACKET_ID)payload[0];
	payload++;//Eliminates the payload id
	len--;
	
	switch (packetId)
	{
	case COMM_GET_VALUES:
		
		ind = 14; //Skipped the first 14 bit.
		telemetryDataFromVesc.avgMotorCurrent = buffer_get_float32(payload, 100.0, &ind);
		telemetryDataFromVesc.avgInputCurrent = buffer_get_float32(payload, 100.0, &ind);
		telemetryDataFromVesc.dutyCycleNow = buffer_get_float16(payload, 1000.0, &ind);
		telemetryDataFromVesc.rpm = buffer_get_int32(payload, &ind);
		telemetryDataFromVesc.inpVoltage = buffer_get_float16(payload, 10.0, &ind);
		telemetryDataFromVesc.ampHours = buffer_get_float32(payload, 10000.0, &ind);
		telemetryDataFromVesc.ampHoursCharged = buffer_get_float32(payload, 10000.0, &ind);
		ind += 8; //Skip 9 bit
		telemetryDataFromVesc.tachometer = buffer_get_int32(payload, &ind);
		telemetryDataFromVesc.tachometerAbs = buffer_get_int32(payload, &ind);

		memRunningRequests.waitForReceiveGetTelemetryValues = false;
		
		return packetId;
		break;

	default:
		return COMM_NON;
		break;
	}

	
}

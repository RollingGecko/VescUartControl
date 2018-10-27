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

// This lib version tested on vesc fw 3.38 and 3.40

#ifndef _VESCUART_h
#define _VESCUART_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "datatypes.h"
#include "local_datatypes.h"

#include <HardwareSerial.h>

#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
  #if defined(USE_TEENSY_HW_SERIAL)
    #define DEBUG_SERIAL_CLASS HardwareSerial // Teensy HW Serial
  #else
    //#include <usb_serial.h>  // Teensy 3.0 and 3.1
    #define DEBUG_SERIAL_CLASS usb_serial_class
    #pragma using usb_serial_class for vesc debug_serial 
  #endif
#elif defined(_SAM3XA_)
  #include <UARTClass.h>  // Arduino Due
  #define DEBUG_SERIAL_CLASS UARTClass
#elif defined(USE_USBCON)
  // Arduino Leonardo USB Serial Port
  #define DEBUG_SERIAL_CLASS Serial_
#else
  #include <HardwareSerial.h>  // Arduino AVR
  #define DEBUG_SERIAL_CLASS HardwareSerial
#endif

#ifndef RX_TIMEOUT
  //SEtup debug port
  #define RX_TIMEOUT 10  // 10 ms default timeout
#endif




///SetSerialPort sets the serial to communicate with the VESC
void SetSerialPort(HardwareSerial* _serialPort);

///SetDebugSerialPort sets the serial for debug information
void SetDebugSerialPort(DEBUG_SERIAL_CLASS* _debugSerialPort);

///PackSendPayload Packs the payload and sends it over Serial.
///Define in a Config.h a SERIAL with the Serial in Arduino Style you want to you
///@param: payload as the payload [unit8_t Array] with length of int lenPayload
///@return the number of bytes send

int PackSendPayload(uint8_t* payload, int lenPay, HardwareSerial* _vescserialPort);

///ReceiveUartMessage receives the a message over Serial
///Define in a Config.h a SERIAL with the Serial in Arduino Style you want to you
///@parm the payload as the payload [unit8_t Array]
///@return the number of bytes receeived within the payload

int ReceiveUartMessage(uint8_t* payloadReceived, HardwareSerial* _vescserialPort);

///Help Function to print struct bldcMeasure over Serial for Debug
///#define DEBUG necessary
///Define in a Config.h the DEBUGSERIAL you want to use

void SerialPrint(const struct bldcMeasure& values, DEBUG_SERIAL_CLASS*  print_serialPort );

///Help Function to print uint8_t array over Serial for Debug
///Define in a Config.h the DEBUGSERIAL you want to use

void SerialPrint(uint8_t* data, int len);

///Sends a command to VESC and stores the returned data
///@param bldcMeasure struct with received data
///@param _vescserialPort, pointer of serial port to use
/// if no pointer uses default port set by SetSerialPort()
//@return true if success
bool VescUartGetValue(struct bldcMeasure& values, HardwareSerial* _vescserialPort);
bool VescUartGetValue(bldcMeasure& values);

///Sends a command to VESC to control the motor current
///@param current as float with the current for the motor
///@param _vescserialPort, pointer of serial port to use
/// if no pointer uses default port set by SetSerialPort()

void VescUartSetCurrent(float current, HardwareSerial* _vescserialPort);
void VescUartSetCurrent(float current);

///Sends a command to VESC to control the motor brake
///@param breakCurrent as float with the current for the brake
///@param _vescserialPort, pointer of serial port to use
/// if no pointer uses default port set by SetSerialPort()

void VescUartSetCurrentBrake(float brakeCurrent, HardwareSerial* _vescserialPort);
void VescUartSetCurrentBrake(float brakeCurrent);

///Sends values of a joystick and 2 buttons to VESC to control the nunchuk app
///@param struct remotePackage with all values
///@param _vescserialPort, pointer of serial port to use
/// if no pointer uses default port set by SetSerialPort()

void VescUartSetNunchukValues(remotePackage& data, HardwareSerial* _vescserialPort);
void VescUartSetNunchukValues(remotePackage& data);

///Sends a command to VESC to control the motor position
///@param position as float with the position in degrees for the motor
///@param _vescserialPort, pointer of serial port to use
/// if no pointer uses default port set by SetSerialPort()

void VescUartSetPosition(float position, HardwareSerial* _vescserialPort) ;
void VescUartSetPosition(float position) ;

///Sends a command to VESC to control the motor duty cycle
///@param duty as float with the duty cycle for the motor
///@param _vescserialPort, pointer of serial port to use
/// if no pointer uses default port set by SetSerialPort()

void VescUartSetDuty(float duty, HardwareSerial* _vescserialPort) ;
void VescUartSetDuty(float duty) ;

///Sends a command to VESC to control the motor rotational speed
///@param rpm as float with the revolutions per second for the motor
///@param _vescserialPort, pointer of serial port to use
/// if no pointer uses default port set by SetSerialPort()

void VescUartSetRPM(float rpm, HardwareSerial* _vescserialPort);
void VescUartSetRPM(float rpm);

#endif
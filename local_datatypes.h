#ifndef LOCAL_DATATYPES_H_
#define LOCAL_DATATYPES_H_

//// Added by AC to store measured values
//struct bldcMeasure {
//	//2 Values float16_t not read(4 byte)
//	float avgMotorCurrent;
//	float avgInputCurrent;
//	//2 Values float32 not read (8 byte)
//	float dutyCycleNow;
//	long rpm;
//	float inpVoltage;
//	float ampHours;
//	float ampHoursCharged;
//	//2 values float32_t not read (8 byte)
//	long tachometer;
//	long tachometerAbs;
//};

//Define remote Package

struct remotePackage {

	int		valXJoy;
	int		valYJoy;
	boolean	valUpperButton;
	boolean	valLowerButton;

};

#endif
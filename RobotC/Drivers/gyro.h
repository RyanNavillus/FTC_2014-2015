// This header provides an API for the HiTechnic gyro sensor.
// Version 0.4, made by Xander Soldaat.
#ifndef __HTGYRO_H__
#define __HTGYRO_H__
#pragma systemFile
#ifndef __COMMON_H__
#include "common.h"
#endif // __COMMON_H__



float HTGYROreadRot(tSensors link);
float HTGYROstartCal(tSensors link);
float HTGYROreadCal(tSensors link);
void HTGYROsetCal(tSensors link, int offset);

#ifdef __HTSMUX_SUPPORT__
float HTGYROreadRot(tMUXSensor muxsensor);
float HTGYROstartCal(tMUXSensor muxsensor);
float HTGYROreadCal(tMUXSensor muxsensor);
void HTGYROsetCal(tMUXSensor muxsensor, int offset);
#endif // __HTSMUX_SUPPORT__

// Array for offset values. Default is 620.
float HTGYRO_offsets[][] =	{{620.0, 620.0, 620.0, 620.0},
							 {620.0, 620.0, 620.0, 620.0},
							 {620.0, 620.0, 620.0, 620.0},
							 {620.0, 620.0, 620.0, 620.0}};



// Reads the value of the gyro.
// `link`:	Port number of gyro.
float HTGYROreadRot(tSensors link) {
	// Make sure the sensor is configured as type sensorRawValue.
	if (SensorType[link] != sensorAnalogInactive) {
		SetSensorType(link, sensorAnalogInactive);
		wait1Msec(100);
	}
	float vel = SensorValue[link]-HTGYRO_offsets[link][0];
	if (abs(vel-0.0)<1.0) {
		vel = 0.0;
	}
	return vel;
}

// Calibrate gyro by calculating the average offset of 25
// readings. Takes about 50*5=250ms, or 1/4th of a second.
// Returns the new offset value for the gyro.
// `link`:	Port number of gyro.
float HTGYROstartCal(tSensors link) {
	const int flush_count = 20;
	const int avg_count = 40;

	//hogCPU(); // TODO: doesn't actually work because `wait`s

	// Make sure the sensor is configured as type sensorRawValue.
	if (SensorType[link] != sensorAnalogInactive) {
		SetSensorType(link, sensorAnalogInactive);
		wait1Msec(200);
	}

	float sum = 0.0;
	wait1Msec(100); // Give the gyro time to initialize.
	for (int i=0; i<flush_count; i++) {
		int temp_reading = SensorValue[link]; // Flush out bad readings.
		wait1Msec(5);
	}

	// Takes a few readings and averages them out.
	for (int i=0; i<avg_count; i++) {
		sum += SensorValue[link];
		wait1Msec(5);
	}

	//releaseCPU(); // TODO: doesn't actually work because `wait`s

	// Store & return new offset value.
	HTGYRO_offsets[link][0] = sum/(float)avg_count;
	return HTGYRO_offsets[link][0];
}

// Manually override the current offset for the gyro.
// `link`:	Port number.
void HTGYROsetCal(tSensors link, int offset) {
	HTGYRO_offsets[link][0] = offset;
}

// Retrieve the current offset for the gyro.
// `link`:	Port number.
float HTGYROreadCal(tSensors link) {
	return HTGYRO_offsets[link][0];
}

#ifdef __HTSMUX_SUPPORT__
// Reads the value of the gyro.
// `muxsensor`:	SMUX sensor port number.
float HTGYROreadRot(tMUXSensor muxsensor) {
	return HTSMUXreadAnalogue(muxsensor) - HTGYRO_offsets[SPORT(muxsensor)][MPORT(muxsensor)];
}

// Calibrate gyro by calculating the avg offset of 50 raw
// readings. Takes about 50*5=250ms, or 1/4th of a second.
// Returns the new offset value for the gyro.
// `muxsensor`:	SMUX sensor port number.
float HTGYROstartCal(tMUXSensor muxsensor) {
	long _avgdata = 0;
	hogCPU();
	wait1Msec(100); // Give the gyro time to initialize.
	int temp_reading = HTSMUXreadAnalogue(muxsensor); // Flush out bad readings.

	// Take 50 readings and average them out.
	// NOTE: When changing limits on `i`, make sure to change
	// the number `_avgdata` is averaged by as well!
	for (int i=0; i<50; i++) {
		_avgdata += HTSMUXreadAnalogue(muxsensor);
		wait1Msec(5);
	}
	releaseCPU();

	// Store new offset value.
	HTGYRO_offsets[SPORT(muxsensor)][MPORT(muxsensor)] = (_avgdata/50.0);
	// Return new offset value.
	return HTGYRO_offsets[SPORT(muxsensor)][MPORT(muxsensor)];
}

// Manually override the current offset for the gyro.
// `muxsensor`:	SMUX sensor port number.
void HTGYROsetCal(tMUXSensor muxsensor, int offset) {
	HTGYRO_offsets[SPORT(muxsensor)][MPORT(muxsensor)] = offset;
}

// Retrieve the current offset for the gyro.
// `muxsensor`:	SMUX sensor port number.
float HTGYROreadCal(tMUXSensor muxsensor) {
	return HTGYRO_offsets[SPORT(muxsensor)][MPORT(muxsensor)];
}
#endif // __HTSMUX_SUPPORT__



#endif // __HTGYRO_H__

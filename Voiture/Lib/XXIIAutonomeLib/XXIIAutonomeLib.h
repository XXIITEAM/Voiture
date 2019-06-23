// XXIIAutonomeLib.h

#ifndef _XXIIAUTONOMELIB_h
#define _XXIIAUTONOMELIB_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
	//Niveaux d'alerte distance
#define NV_ALERTE_1				1
#define NV_ALERTE_2				2
#define NV_ALERTE_3				3
#define NV_ALERTE_4				4
#define NV_ALERTE_5				5

//Bool d'alerte active
#define ALERTE false

#pragma once
class XXIIAutonomeLibClass
{
private:
	//float getCM(int trigPin, int echoPin);
public:
	void init();
	void algoObstacles(float avg, float avc, float avd, float arg, float arc, float ard);
	void decouverte();
	float calibration();
};
extern XXIIAutonomeLibClass XXIIAutonomeLib;
#endif


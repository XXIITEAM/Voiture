// XXIISensorLib.h

#ifndef _XXIISENSORLIB_h
#define _XXIISENSORLIB_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
#include <String.h>
#include <Arduino.h>
//Définition capteurs_US
//US1 AvG
#define US1_TRIGGER_PIN  22
#define US1_ECHO_PIN     23
//US2 AvC
#define US2_TRIGGER_PIN  24
#define US2_ECHO_PIN     25
//US3 AvD
#define US3_TRIGGER_PIN  26
#define US3_ECHO_PIN     27
//US4 ArD
#define US4_TRIGGER_PIN  28
#define US4_ECHO_PIN     29
//US5 ArC
#define US5_TRIGGER_PIN  30
#define US5_ECHO_PIN     31
//US6 ArG
#define US6_TRIGGER_PIN  32
#define US6_ECHO_PIN     33

#define CYCLE_DISTANCE        500
#pragma once
class XXIISensorLib
{
private:
	float getCM(int trigPin, int echoPin);
public:
	XXIISensorLib();
	float ScanAv(float *capteur_av_g_cm, float *capteur_av_c_cm, float *capteur_av_d_cm);
	float ScanAr(float *capteur_ar_d_cm, float *capteur_ar_c_cm, float *capteur_ar_g_cm);

};
#endif


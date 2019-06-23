// XXIIVehiculeLib.h
#ifndef _XXIIVehiculeLib_h
#define _XXIIVehiculeLib_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "../XXIIEEPROMLib/XXIIEEPROMLib.h"
#include <MotorDriver.h>
#include "../XXIISensorLib/XXIISensorLib.h"
#include "../XXIIBluetoothLib/XXIIBluetoothLib.h"
	//Niveaux d'alerte distance
#define NV_ALERTE_1				1
#define NV_ALERTE_2				2
#define NV_ALERTE_3				3
#define NV_ALERTE_4				4
#define NV_ALERTE_5				5

//Bool d'alerte active
#define ALERTE false
#define CMD_DELIM_2 '/'
struct paramStuct {
	int struct_cm_s;
};

#pragma once
class XXIIVehiculeLibClass
{
private:
	//float getCM(int trigPin, int echoPin);
public:
	//Etats

	bool boolForward = false;
	bool boolStop = false;
	bool boolBackward = false;
	bool boolCalib = false;
	String mode;

	unsigned long magic;
	int zone_1_min, zone_2_min, zone_3_min, zone_4_min, zone_4_max;
	// --------------------------------------------------------------------------------
/// <summary>
/// @struct paramStruct
/// @brief Structure int.
/// Stockage des paramètres de configuration des distances.
/// Récupéré et mis à jour par BTArdroid
/// </summary>
// --------------------------------------------------------------------------------
	paramStuct ps_write;
	//Etats
	void XXIIVehiculeLibClass::init();
	void XXIIVehiculeLibClass::ScanUS();
	void XXIIVehiculeLibClass::algoObstacles(float avg, float avc, float avd, float arg, float arc, float ard);
	void XXIIVehiculeLibClass::decouverte();
	float XXIIVehiculeLibClass::calibration();
	void XXIIVehiculeLibClass::leftForward();
	void XXIIVehiculeLibClass::rightForward();
	void XXIIVehiculeLibClass::traitementMessage(char commande_a_traiter);
	void XXIIVehiculeLibClass::optDist();

};
extern XXIIVehiculeLibClass XXIIVehiculeLib;
#endif


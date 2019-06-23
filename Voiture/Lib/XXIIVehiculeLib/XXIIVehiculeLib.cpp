#include "XXIIVehiculeLib.h"

void XXIIVehiculeLibClass::init()
{
	uint8_t speed0 = 100;

}

void XXIIVehiculeLibClass::ScanUS()
{
	float avg, avc, avd, arg, arc, ard;
	XXIISensorLib.ScanAv(&avg, &avc, &avd);
	XXIISensorLib.ScanAr(&arg, &arc, &ard);
	algoObstacles(avg, avc, avd, arg, arc, ard);
}

// ********************************************************************************
/// <summary>
/// @fn void algoObstacles(float avg, float avc, float avd, float arg, float arc, float ard)
/// @brief Fonction comportement si obstacle.
/// </summary>
/// <param name="avg">Distance capteur avant gauche</param>
/// <param name="avc">Distance capteur avant centre</param>
/// <param name="avd">Distance capteur avant droit</param>
/// <param name="arg">Distance capteur arrière gauche</param>
/// <param name="arc">Distance capteur arrière centre</param>
/// <param name="ard">Distance capteur arrière droit</param>
// ********************************************************************************

void XXIIVehiculeLibClass::algoObstacles(float avg, float avc, float avd, float arg, float arc, float ard)
{
	XXIIEEPROMLib.getEEPROMParam(&zone_1_min, &zone_2_min, &zone_3_min, &zone_4_min, &zone_4_max);
	//Tableau distances
	float tabDistances[6] = { avg, avc, avd, arg, arc, ard };
	//Tableau niveaux d'alerte
	int tabAlerte[6];

	//Attribution du niveau d'alerte
	for (int i = 0; i < 6; i++) {
		if ((tabDistances[i] > zone_4_max)) {
			tabAlerte[i] = NV_ALERTE_1;
		}
		if ((tabDistances[i] <= zone_4_max) && (tabDistances[i] > zone_4_min)) {
			tabAlerte[i] = NV_ALERTE_2;
		}
		if ((tabDistances[i] <= zone_4_min) && (tabDistances[i] > zone_3_min)) {
			tabAlerte[i] = NV_ALERTE_3;
		}
		if ((tabDistances[i] <= zone_3_min) && (tabDistances[i] > zone_2_min)) {
			tabAlerte[i] = NV_ALERTE_4;
		}
		if ((tabDistances[i] <= zone_2_min) && (tabDistances[i] > zone_1_min)) {
			tabAlerte[i] = NV_ALERTE_5;
		}
		if ((tabDistances[i] <= zone_1_min)) {
			tabAlerte[i] = 22.00;
		}
		if (tabAlerte[i] > NV_ALERTE_3) {
			Serial.print("Capteur N° ");
			Serial.println(i + 1);
			Serial.print("Alerte level : ");
			Serial.println(tabAlerte[i]);
		}
	}
	//Si distance min d'un des 6 capteurs < 50.00 cm on agit sinon on continue
	if (min(tabDistances[0], min(tabDistances[1], min(tabDistances[2], min(tabDistances[3], min(tabDistances[4], tabDistances[5]))))) < float(50)) {
		Serial.println("Entrée dans -50cm");
		if (tabAlerte[0] > NV_ALERTE_4 || tabAlerte[1] > NV_ALERTE_4 || tabAlerte[2] > NV_ALERTE_4) {
			//Alerte face
			Serial.println("Entrée dans -50cm");
			boolForward = false;
			boolStop = false;
			boolBackward = true;
			motordriver.goBackward();

		}
		else if (tabAlerte[3] > NV_ALERTE_4 || tabAlerte[4] > NV_ALERTE_4 || tabAlerte[5] > NV_ALERTE_4) {
			//Alerte arrière
			Serial.println("Entrée dans -50cm Arrière");
			boolForward = true;
			boolStop = false;
			boolBackward = false;
			motordriver.goForward();
		}
		else {
			if (boolStop == false) {
				boolForward = false;
				boolStop = true;
				boolBackward = false;
				motordriver.stop();
			}
		}
	}
}

// ********************************************************************************
	/// <summary>
	/// @fn void decouverte()
	/// @brief Fonction découverte environnement.
	/// </summary>
	// ********************************************************************************
	void XXIIVehiculeLibClass::decouverte() {
		long departMillis, stopMillis, execTime;
		float avg, avc, avd, arg, arc, ard;
		//Sensor.ScanAv(&avg, &avc, &avd);
		//Sensor.ScanAr(&arg, &arc, &ard);
		do {
			departMillis = millis();
			motordriver.goBackward();
			//Sensor.ScanAr(&arg, &arc, &ard);
		} while (arc > 30.00);
		stopMillis = millis();
		execTime = stopMillis - departMillis;
		motordriver.stop();
		do {
			departMillis = millis();
			motordriver.goForward();
			//Sensor.ScanAv(&avg, &avc, &avd);
		} while (avc > 30.00);
		stopMillis = millis();
		motordriver.stop();

	}


	// ********************************************************************************
	/// <summary>
	/// @fn float calibration()
	/// @brief Fonction calibration vitesse vh (cm/s)
	/// Placer le véhicule face à un mur (1 ou 2 m)
	/// </summary>
	/// <returns>cm_s</returns>
	// ********************************************************************************
	float XXIIVehiculeLibClass::calibration() {
		//bool second passage (cycle mesure > MAV > mesure > MAR > mesure > calcul) x2
		bool second;
		second = false;
		//Test récup echo indirect
		int echo_direct;
		//var temps
		long departMillis, stopMillis, execTime;
		float avg, avc, avd, arg, arc, ard;
		double cm_s, cm_s_av, oavc, oarc, cm_s_ar, cm_s_premier, cm_s_second;
		//Boucle calibration
		for (int i = 0; i < 2; i++) {
			XXIISensorLib.ScanAv(&avg, &avc, &avd);
			oavc = avc;
			departMillis = millis();
			motordriver.goForward();
			do {

			} while (millis() - departMillis <= 5000);
			stopMillis = millis();
			motordriver.stop();
			XXIISensorLib.ScanAv(&avg, &avc, &avd);
			oavc = oavc - avc;
			cm_s_av = oavc / 5;


			XXIISensorLib.ScanAv(&avg, &avc, &avd);
			oarc = avc;
			departMillis = millis();
			motordriver.goBackward();
			do {

			} while (millis() - departMillis <= 5000);
			stopMillis = millis();
			motordriver.stop();
			XXIISensorLib.ScanAv(&avg, &avc, &avd);
			oarc = avc - oarc;
			cm_s_ar = oarc / 5;
			if (second == true) {
				cm_s_second = (cm_s_av + cm_s_ar) / 2;
			}
			else {
				cm_s_premier = (cm_s_av + cm_s_ar) / 2;
			}
			second = true;
		}
		cm_s = (cm_s_premier + cm_s_second) / 2;
		second = false;
		//bool calib OK
		boolCalib = true;
		//Stock la variable en structure
		ps_write.struct_cm_s = cm_s;
		//Enregistrement de la variable sur l'EEPROM
		EEPROM.put(100, ps_write);
		//On renvoi la mesure
		return cm_s;
	}

	void XXIIVehiculeLibClass::rightForward() {
		motordriver.setSpeed(100, MOTORB);
		return;

	}
	

	void XXIIVehiculeLibClass::leftForward() {
		motordriver.setSpeed(100, MOTORA);
		return;

	}

	// ********************************************************************************
	/// <summary>
	/// @fn void optDist()
	/// @brief Envoi des parametres (Distances zones) a BTArduino
	/// </summary>
	// ********************************************************************************
	unsigned long magic = 0;
	int zone_1_min, zone_2_min, zone_3_min, zone_4_min, zone_4_max = 0;
	void XXIIVehiculeLibClass::optDist() {
		XXIIEEPROMLib.chargerParametres(&magic, &zone_1_min, &zone_2_min, &zone_3_min, &zone_4_min, &zone_4_max);
		String str_list_zone;
		int tmp_tab_zone_param[]{ zone_1_min, zone_2_min, zone_3_min, zone_4_min, zone_4_max };
		str_list_zone += "O/";
		for (int i = 0; i < 5; i++) {
			str_list_zone += String(tmp_tab_zone_param[i]) + CMD_DELIM_2;
		}
		str_list_zone += "X";
		XXIIBluetoothLib.writeMsg(str_list_zone);
		return;
	}
	void XXIIVehiculeLibClass::traitementMessage(char commande_a_traiter) {
		switch (commande_a_traiter)
		{

		case CMD_FORWARD:
			boolForward = true;
			boolStop = false;
			boolBackward = false;
			motordriver.goForward();
			break;
			/*case CMD_RIGHT_FORWARD:
				motordriver.goForward();
			Serial.println("Avance Droite");
				rightForward();
				motordriver.setSpeed(speed0, MOTORB);
				break;*/
		/*case CMD_LEFT_FORWARD:
			boolForward = true;
			boolStop = false;
			boolBackward = false;
			motordriver.goForward();
			leftForward();
			motordriver.setSpeed(vehicule.speed0, MOTORA);
			break;
		case CMD_RIGHT_BACK:
			boolForward = false;
			boolStop = false;
			boolBackward = true;
			motordriver.goBackward();
			rightForward();
			motordriver.setSpeed(speed0, MOTORB);
			break;
		case CMD_LEFT_BACK:
			boolForward = false;
			boolStop = false;
			boolBackward = true;
			motordriver.goBackward();
			leftForward();
			motordriver.setSpeed(speed0, MOTORA);
			break;*/
		case CMD_RIGHT_FRONT:
			boolForward = true;
			boolStop = false;
			boolBackward = false;
			motordriver.goRight();
			break;
		case CMD_BACKWARD:
			boolForward = false;
			boolStop = false;
			boolBackward = true;
			motordriver.goBackward();
			break;
		case CMD_LEFT_FRONT:
			boolForward = false;
			boolStop = false;
			boolBackward = false;
			motordriver.goLeft();
			break;
		case CMD_STOP:
			boolForward = false;
			boolStop = true;
			boolBackward = false;
			motordriver.stop();
			break;
		case CMD_OPT_DIST:
			optDist();
			break;
		case CMD_WRITE:
			XXIIBluetoothLib.traitementOptions(commande_a_traiter);
			break;
		case CMD_SAVE:
			XXIIEEPROMLib.sauvegardeParametres(&magic, &zone_1_min, &zone_2_min, &zone_3_min, &zone_4_min, &zone_4_max);
			break;
		case CMD_GETVALUES:
			//listingBT();
			break;
		case CMD_AUTONOME:
			XXIIBluetoothLib.writeMsg("A");
			XXIIVehiculeLib.mode = "A";
			break;
		case CMD_MANUELLE:
			motordriver.stop();
			XXIIVehiculeLib.mode = "M";
			XXIIBluetoothLib.writeMsg("M");
			break;
		case CMD_TEMP:
			//temperature();
			break;
		default:
			break;
		}
		return;
	}
	XXIIVehiculeLibClass XXIIVehiculeLib;
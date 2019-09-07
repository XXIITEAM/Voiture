// ********************************************************************************
/// <summary>
/// @file Voiture.ino
/// @brief Programme voiture arduino
/// @author XXII-TEAM
/// @version 0.1
/// @date 22 juin 2019
/// Programme Arduino voiture commandée par application Android (BTArdroid).
/// </summary>
/// <created>XXII TEAM,22/06/2019</created>
/// <changed>XXII TEAM,23/06/2019</changed>
// ********************************************************************************

#include "Lib/XXIIVehiculeLib/XXIIVehiculeLib.h"
#include "Arduino.h"
#include <String.h>
#include <stdlib.h>
#include <DHT.h>
#include "Lib\XXIIBluetoothLib\XXIIBluetoothLib.h"

#define CAR_STOP 0
#define CAR_FORWARD 1
#define CAR_BACK 2

//unsigned long magic = 0;
//int zone_1_min, zone_2_min, zone_3_min, zone_4_min, zone_4_max = 0;
//Capteur Hydrométrie/Température
#define DHTPIN A0
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
uint8_t car_status = CAR_STOP;
uint8_t new_status = car_status;
uint8_t bt_command;
// Tempo
const unsigned long TEMPO_DIST = 100;

const unsigned long TEMPO_MOVE = 100;
const unsigned long TEMPO_TURN = 200;

unsigned long previousMillisDIST = 0;
unsigned long previousMillisMove = 0;
unsigned long previousMillisTurn = 0;
unsigned long currentMillis;


// ********************************************************************************
/// <summary>
/// @fn void setup()
/// @brief Fonction Setup.
/// </summary>
// ********************************************************************************
void setup() {
	Serial.begin(115200);
	Serial3.begin(115200);
	XXIIBluetoothLib.s_connecting = "CONNECTING";
	XXIIBluetoothLib.s_connected = "CONNECTED";
	XXIIBluetoothLib.status = 0;
	motordriver.init();
	motordriver.setSpeed(100, MOTORA);
	motordriver.setSpeed(100, MOTORB);
	XXIIEEPROMLib.chargerParametres(&XXIIVehiculeLib.magic, &XXIIVehiculeLib.zone_1_min, &XXIIVehiculeLib.zone_2_min, &XXIIVehiculeLib.zone_3_min, &XXIIVehiculeLib.zone_4_min, &XXIIVehiculeLib.zone_4_max);
	//XXIIBluetoothLib.waitPairable();
	//XXIIBluetoothLib.waitConnected();
		
	XXIIVehiculeLib.mode = "M";
	//dht.begin();
}

// ********************************************************************************
/// <summary>
/// @fn void loop()
/// @brief Fonction loop.
/// </summary>
// ********************************************************************************
void loop() {
	float dist_av_g, dist_av_c, dist_av_d, dist_ar_d, dist_ar_c, dist_ar_g;
	XXIIVehiculeLib.autonome();
	/*XXIIBluetoothLib.commande_recue = XXIIBluetoothLib.readByte();
	if (XXIIBluetoothLib.getStatus() == PAIRABLE) {
		motordriver.stop();
		XXIIBluetoothLib.waitConnected();
	}
	currentMillis = millis();
	if (currentMillis - previousMillisDIST >= TEMPO_DIST) {
		XXIISensorLib.ScanAv(&dist_av_g, &dist_av_c, &dist_av_d);
		previousMillisDIST = currentMillis;

	}
	if (XXIIVehiculeLib.mode != "A" || XXIIBluetoothLib.commande_recue == 'M') {
		if (XXIIBluetoothLib.commande_recue != CMD_INVALID || XXIIBluetoothLib.commande_recue != CMD_TEMP || XXIIBluetoothLib.commande_recue != XXIIBluetoothLib.CMD_VIDE) {
			if (XXIIBluetoothLib.commande_recue !=XXIIBluetoothLib.commande_precedente) {
				XXIIBluetoothLib.commande_precedente = XXIIBluetoothLib.commande_recue;
				XXIIVehiculeLib.traitementMessage(XXIIBluetoothLib.commande_recue);
			}
			
		}
	}
	else {
		XXIISensorLib.ScanAv(&dist_av_g, &dist_av_c, &dist_av_d);
		XXIISensorLib.ScanAr(&dist_ar_d, &dist_ar_c, &dist_ar_g);
		XXIIVehiculeLib.algoObstacles(dist_av_g,dist_av_c,dist_av_d, dist_ar_d, dist_ar_c, dist_ar_g);
		delay(500);
	}*/
	/*currentMillis = millis();
	if (currentMillis - previousMillisDIST >= 500) {
<<<<<<< HEAD
		if (boolCalib == false) {
			Serial.println(calibration());
		}
		
		previousMillisDIST = currentMillis;
	}

	
}

void testScan() {
	for (int i = 1; i < 6; i++) {
	sd_write.capteurSource = i;
	Sensor.Scan(&sd_write);
	Serial.print("Capteur source : ");
	Serial.println(sd_write.capteurSource);
	Serial.print("Capteur reception : ");
	Serial.println(sd_write.capteurRecept);
	Serial.print("Distance : ");
	Serial.println(sd_write.distCm);
	}
}
/*..........................................................*/
/*..........................................................*/
/*					BLUETOOTH								*/
/*..........................................................*/
/*..........................................................*/

/*!
 * @fn void waitPairable()
 * @brief Fonction attente BT.
 *
 * @param Aucun.
 * @return Aucun
 */
void waitPairable() {
	char recvChar;
	while (status != PAIRABLE) {
		if (Serial3.available()) {
			recvChar = Serial3.read();
			Serial.write(recvChar);
			if (recvChar == '+') {
				while (Serial3.available() == 0);
				recvChar = Serial3.read();
				Serial.write(recvChar);
				if (recvChar == 'R')continue;
				else if (recvChar == 'P') {
					while (Serial3.available() < 7);
					String recvString;
					recvString += recvChar;
					for (uint8_t i = 0; i < 7; i++) {
						recvChar = Serial3.read();
						recvString += recvChar;
					}
					if (recvString == "PAIRABLE") status = PAIRABLE;
				}
			}
		}
		else {
			if (testAT())status = PAIRABLE;
			else {
				delay(200);
				if (testAT()) status = PAIRABLE;
				else { status = CONNECTED; break; }
			}
		}
	}
}
/*!
 * @fn void waitConnected()
 * @brief Fonction connexion BT.
 *
 * @param Aucun.
 * @return Aucun
 */
void waitConnected() {
	char recvChar;
	while (status != CONNECTED) {
		if (Serial3.available()) {
			recvChar = Serial3.read();
			if (recvChar == '+') {
				while (Serial3.available() < 10);
				String recvString;
				for (uint8_t i = 0; i < 10; i++) {
					recvChar = Serial3.read();
					recvString += recvChar;
				}
				if (recvString == s_connecting) status = CONNECTING;
			}
			if (recvChar == 'C') {
				while (Serial3.available() < 8);
				String recvString;
				recvString += recvChar;
				for (uint8_t i = 0; i < 8; i++) {
					recvChar = Serial3.read();
					recvString += recvChar;
				}
				if (recvString == s_connected) status = CONNECTED;
			}
		}
		else {
			if (testAT()) status = PAIRABLE;
			else {
				if (testAT()) status = PAIRABLE;
				else status = CONNECTED;
			}
		}
	}
}

int8_t getStatus() {
	return status;
}

void clearBuffer() {
	char recvChar;
	while (Serial3.available())recvChar = Serial3.read();
}

/*..........................................................*/
/*..........................................................*/
/*					BLUETOOTH								*/
/*..........................................................*/
/*..........................................................*/

/*!
 * @fn bool writeAT(String cmdAT)
 * @brief Fonction commandes AT BT.
 *
 * @param String cmdAT.
 * @return Trus/False
 */
bool writeAT(String cmdAT) {
	Serial3.println(cmdAT);
	//delay(500);
	if (Serial3.available() > 1) {
		String recvString;
		
		char recvChar;
		recvChar = Serial3.read();
		recvString += recvChar;
		recvChar = Serial3.read();
		recvString += recvChar;
		if (recvString == "OK")return true;
		else return false;
	}
	else return false;
}
bool testAT() {
	clearBuffer();
	return writeAT("AT");
}
/*!
 * @fn bool writeMsg(String cmd)
 * @brief Fonction envoie messages BT.
 *
 * @param String cmd.
 * @return Aucun
 */
bool writeMsg(String cmd) {
	Serial3.println(cmd);
}


char readByte() {
	if (status != CONNECTED) return 0xff;
	if (Serial3.available())
	{
		char recvChar;
		recvChar = Serial3.read();
		if (recvChar == '+')//Char deconnexion BT "+"
		{
			waitPairable();
			return 0xff;//Char no connect
		}
		else return recvChar;
	}
	else return 0xff;
}

/*..........................................................*/
/*..........................................................*/
/*						MODE AUTONOME						*/
/*..........................................................*/
/*..........................................................*/

/*!
 * @fn void ScanUS()
 * @brief Fonction appel scanUS lib XXIISensorLib.
 *
 * @param Aucun
 * @return Lance algoObstacles(avg, avc, avd, arg, arc, ard);
 */
void ScanUS() {
	float avg, avc, avd, arg, arc, ard;
	Sensor.ScanAv(&avg, &avc, &avd);
	Sensor.ScanAr(&arg, &arc, &ard);
	algoObstacles(avg, avc, avd, arg, arc, ard);
}

/*!
 * @fn void algoObstacles(float avg, float avc, float avd, float arg, float arc, float ard)
 * @brief Fonction comportement si obstacle.
 *
 * @param float avg, float avc, float avd, float arg, float arc, float ard
 * @return Aucun
 */
void algoObstacles(float avg, float avc, float avd, float arg, float arc, float ard) {
	//Tableau distances
	float tabDistances[6] =  { avg, avc, avd, arg, arc, ard };
	//Tableau niveaux d'alerte
	int tabAlerte[6];
	//Attribution du niveau d'alerte
	for (int i = 0; i < 6; i++) {
			if ((tabDistances[i] > tab_zone_param[4])) {
				tabAlerte[i] = NV_ALERTE_1;
			}
			if ((tabDistances[i] <= tab_zone_param[4]) && (tabDistances[i] > tab_zone_param[3])) {
				tabAlerte[i] = NV_ALERTE_2;
			}
			if ((tabDistances[i] <= tab_zone_param[3]) && (tabDistances[i] > tab_zone_param[2])) {
				tabAlerte[i] = NV_ALERTE_3;
			}
			if ((tabDistances[i] <= tab_zone_param[2]) && (tabDistances[i] > tab_zone_param[1])) {
				tabAlerte[i] = NV_ALERTE_4;
			}
			if ((tabDistances[i] <= tab_zone_param[1]) && (tabDistances[i] > tab_zone_param[0])) {
				tabAlerte[i] = NV_ALERTE_5;
			}
			if ((tabDistances[i] <= tab_zone_param[0])) {
				tabAlerte[i] = 22.00;
			}
			if (tabAlerte[i] > NV_ALERTE_3) {
				Serial.print("Capteur N° ");
				Serial.println(i+1);
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
			
=======
		if (XXIIVehiculeLib.boolCalib == false) {
			Serial.println(XXIIVehiculeLib.calibration());
>>>>>>> V1_Lib
		}

		previousMillisDIST = currentMillis;
	}*/

	//Serial.println(XXIISensorLib.Scan());
}
/*..........................................................*/

// ********************************************************************************
/// <summary>
/// @fn void listingBT()
/// @brief Envoie des données capteurs a BTArduino
/// </summary>
// ********************************************************************************
void listingBT() {
	float cmMsec = 12.0;
	String distance = String(cmMsec);
	String list = ("Z" "/" "Distance : " + distance + "/" + temperature());
	//writeMsg(list);
}

/*..........................................................*/
/*..........................................................*/
/*					SAUVEGARDE EEPROM						*/
/*..........................................................*/
/*..........................................................*/

	

// ********************************************************************************
/// <summary>
/// @fn String temperature()
/// @brief Récupération temperature et humidite pour envoie vers BTArdroid
/// </summary>
/// <returns>String th = "Température: " + temp + "°C/" + "Hygrométrie: " + hydro +"%/";</returns>
// ********************************************************************************
String temperature() {
	float h = dht.readHumidity();
	float t = dht.readTemperature();
	if (isnan(t) || isnan(h))
	{
		Serial.println("Failed to read from DHT");
	}
	else
	{
		String temp = String(t);
		String hydro = String(h);
		String th = "Température: " + temp + "°C/" + "Hygrométrie: " + hydro + "%/";
		return th;
	}


}

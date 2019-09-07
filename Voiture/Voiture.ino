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
		if (XXIIVehiculeLib.boolCalib == false) {
			Serial.println(XXIIVehiculeLib.calibration());
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

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

#include "Lib/XXIIEEPROMLib/XXIIEEPROMLib.h"
#include "Lib/XXIIAutonomeLib/XXIIAutonomeLib.h"
#include "Arduino.h"
#include "Lib/XXIISensorLib/XXIISensorLib.h"
#include <String.h>
#include <MotorDriver.h>
#include <stdlib.h>
#include <DHT.h>
	/*-------ULTRASONIC CONFIGURATION-------*/
int zone_1_min, int zone_2_min, int zone_3_min, int zone_4_min, int zone_4_max,unsigned long magic;
XXIISensorLibClass Sensor;
XXIIAutonomeLibClass Autonome;
XXIIEEPROMLibClass eeprom;
int alerteCentre;
int alerteGauche;
int alerteDroite;

#define CAR_STOP 0
#define CAR_FORWARD 1
#define CAR_BACK 2
	//Etats bluetooth
#define INITIALIZING 0
#define READY        1
#define INQUIRING    2
#define PAIRABLE     3
#define CONNECTING   4
#define CONNECTED    5

//Capteur Hydrométrie/Température
#define DHTPIN A0
#define DHTTYPE DHT11
	DHT dht(DHTPIN, DHTTYPE);

	//Commandes
#define CMD_INVALID     0xff
#define CMD_FORWARD     'F'
#define CMD_RIGHT_FRONT 'R'
#define CMD_RIGHT_FORWARD	'D'
#define CMD_LEFT_FORWARD	'G'
#define CMD_RIGHT_BACK		'J'
#define CMD_LEFT_BACK		'H'
#define CMD_BACKWARD		'B'
#define CMD_AUTONOME		'A'
#define CMD_LEFT_FRONT		'L'
#define CMD_STOP			'S'
#define CMD_GETVALUES		'Z'
#define CMD_MANUELLE		'M'
#define CMD_OPT_DIST		'O'
#define CMD_WRITE			'W'
#define STOP_CMD			'X'
#define CMD_DELIM			'/'
#define CMD_SAVE			'Q'
#define CMD_TEMP			'T'
#define SPEED_STEPS 20

//Définition vitesse
	uint8_t speed0 = 100;

	//Définition STRUCT_MAGIC (test données options)
	static const unsigned long STRUCT_MAGIC = 22;
	int tab_zone_param[5];

	//Bluetooth
	int8_t status;
	String s_connecting;
	String s_connected;
	String mode;

	//Etats
	boolean boolForward = false;
	boolean boolStop = false;
	boolean boolBackward = false;
	boolean boolCalib = false;

	char commande_precedente = 'I';
	char commande_recue;
	char CMD_VIDE = NULL;
	uint8_t car_status = CAR_STOP;
	uint8_t new_status = car_status;
	uint8_t bt_command;
	// Tempo
	const unsigned long TEMPO_DIST = 100;
	const unsigned long TEMPO_BT = 500;
	const unsigned long TEMPO_MOVE = 100;
	const unsigned long TEMPO_TURN = 200;
	unsigned long previousMillisBT = 0;
	unsigned long previousMillisDIST = 0;
	unsigned long previousMillisMove = 0;
	unsigned long previousMillisTurn = 0;
	unsigned long currentMillis;

	// --------------------------------------------------------------------------------
	/// <summary>
	/// @struct optionStruct
	/// @brief Structure int.
	/// Stockage des paramètres de configuration des distances.
	/// Récupéré et mis à jour par BTArdroid
	/// </summary>
	// --------------------------------------------------------------------------------
	struct optionStruct {
		unsigned long magic;
		int zone_4_max;
		int zone_4_min;
		int zone_3_min;
		int zone_2_min;
		int zone_1_min;
	};
	optionStruct os_write;
	// --------------------------------------------------------------------------------
	/// <summary>
	/// @struct paramStruct
	/// @brief Structure int.
	/// Stockage des paramètres de configuration des distances.
	/// Récupéré et mis à jour par BTArdroid
	/// </summary>
	// --------------------------------------------------------------------------------
	struct paramStuct {
		int struct_cm_s;
	};
	paramStuct ps_write;

	// ********************************************************************************
	/// <summary>
	/// @fn void setup()
	/// @brief Fonction Setup.
	/// </summary>
	// ********************************************************************************
	void setup() {
		Serial.begin(115200);
		Serial3.begin(115200);
		s_connecting = "CONNECTING";
		s_connected = "CONNECTED";
		status = 0;
		motordriver.init();
		motordriver.setSpeed(speed0, MOTORA);
		motordriver.setSpeed(speed0, MOTORB);
		eeprom.chargerParametres(&magic, &zone_1_min, &zone_2_min, &zone_3_min, &zone_4_min, &zone_4_max);
		waitPairable();
		waitConnected();
		mode = "M";
		//dht.begin();
	}

	// ********************************************************************************
	/// <summary>
	/// @fn void loop()
	/// @brief Fonction loop.
	/// </summary>
	// ********************************************************************************
	void loop() {
		/*commande_recue = readByte();
		if (getStatus() == PAIRABLE) {
			motordriver.stop();
			waitConnected();
		}
		currentMillis = millis();
		if (currentMillis - previousMillisDIST >= TEMPO_DIST) {
			Sensor.ScanAv(&dist_av_g, &dist_av_c, &dist_av_d);
			previousMillisDIST = currentMillis;

		}
		if (mode != "A" || commande_recue == 'M') {
			if (commande_recue != CMD_INVALID || commande_recue != CMD_TEMP || commande_recue != CMD_VIDE) {
				if (commande_recue != commande_precedente) {
					commande_precedente = commande_recue;
					traitementMessage(commande_recue);
				}
				commande_precedente = commande_recue;
			}
		}
		else {
			US_scan_Av;
			US_scan_Ar;
			autonome();
			delay(500);
		}*/
		currentMillis = millis();
		if (currentMillis - previousMillisDIST >= 500) {
			if (boolCalib == false) {
				Serial.println(Autonome.calibration());
			}

			previousMillisDIST = currentMillis;
		}


	}
	/*..........................................................*/
	/*..........................................................*/
	/*					BLUETOOTH								*/
	/*..........................................................*/
	/*..........................................................*/

	// ********************************************************************************
	/// <summary>
	/// @fn void waitPairable()
	/// @brief Fonction attente BT.
	/// </summary>
	// ********************************************************************************
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
	// ********************************************************************************
	/// <summary>
	/// @fn void waitConnected()
	/// @brief Fonction connexion BT.
	/// </summary>
	// ********************************************************************************
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

	// ********************************************************************************
	/// <summary>
	/// @fn bool writeAT(String cmdAT)
	/// @brief Fonction commandes AT BT.
	/// </summary>
	/// <param name="cmdAT">Commande AT de test BT</param>
	/// <returns>Trus/False</returns>
	// ********************************************************************************
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
	// ********************************************************************************
	/// <summary>
	/// @fn bool writeMsg(String cmd)
	/// @brief Fonction envoie messages BT.
	/// </summary>
	/// <param name="cmd">Commande envoyée vers Android</param>
	/// <returns>Aucun</returns>
	// ********************************************************************************
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

	// ********************************************************************************
	/// <summary>
	/// @fn void ScanUS()
	/// @brief Fonction appel scanUS lib XXIISensorLib.
	/// </summary>
	// ********************************************************************************
	void ScanUS() {
		float avg, avc, avd, arg, arc, ard;
		Sensor.ScanAv(&avg, &avc, &avd);
		Sensor.ScanAr(&arg, &arc, &ard);
		Autonome.algoObstacles(avg, avc, avd, arg, arc, ard);
	}

	/*..........................................................*/
	/*..........................................................*/
	/*					TRAITEMENT COMMANDE						*/
	/*..........................................................*/
	/*..........................................................*/

	// ********************************************************************************
	/// <summary>
	/// @fn void traitementMessage(char commande_a_traiter)
	/// @brief Fonction traitement du message reçu (BTArduino)
	/// </summary>
	/// <param name="commande_a_traiter">Commande reçue de Android à traiter</param>
	// ********************************************************************************
	void traitementMessage(char commande_a_traiter) {
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
		case CMD_LEFT_FORWARD:
			boolForward = true;
			boolStop = false;
			boolBackward = false;
			motordriver.goForward();
			leftForward();
			motordriver.setSpeed(speed0, MOTORA);
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
			break;
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
			traitementOptions(commande_a_traiter);
			break;
		case CMD_SAVE:
			eeprom.sauvegardeParametres();
			break;
		case CMD_GETVALUES:
			//listingBT();
			break;
		case CMD_AUTONOME:
			writeMsg("A");
			mode = "A";
			break;
		case CMD_MANUELLE:
			motordriver.stop();
			mode = "M";
			writeMsg("M");
			break;
		case CMD_TEMP:
			//temperature();
			break;
		default:
			break;
		}
		return;
	}
	void rightForward() {
		motordriver.setSpeed(100, MOTORB);
		return;

	}
	void leftForward() {
		motordriver.setSpeed(100, MOTORA);
		return;

	}

	/*..........................................................*/
	/*..........................................................*/
	/*					PARAMETRAGE ZONES						*/
	/*..........................................................*/
	/*..........................................................*/

	// ********************************************************************************
	/// <summary>
	/// @fn void optDist()
	/// @brief Envoi des parametres (Distances zones) a BTArduino
	/// </summary>
	// ********************************************************************************
	void optDist() {
		eeprom.chargerParametres(&magic,  &zone_1_min, &zone_2_min, &zone_3_min, &zone_4_min, &zone_4_max);
		String str_list_zone;
		int tmp_tab_zone_param[]  { zone_1_min, zone_2_min, zone_3_min, zone_4_min, zone_4_max };
		str_list_zone += "O/";
		for (int i = 0; i < 5; i++) {
			str_list_zone += String(tmp_tab_zone_param[i]) + CMD_DELIM;
		}
		str_list_zone += "X";
		writeMsg(str_list_zone);
		return;
	}

	// ********************************************************************************
	/// <summary>
	/// @fn void traitementOptions(char cmd)
	/// @brief Traitement du String parametres reçu de BTArduino
	/// </summary>
	/// <param name="cmd">Commande reçue de android</param>
	// ********************************************************************************
	void traitementOptions(char cmd) {
		String cmdStr = "";
		char param = cmd;
		char cmd1[5];
		int icmd;
		int i = 0;
		while (param != STOP_CMD) {
			if (param == CMD_WRITE) {
				param = Serial3.read();
			}
			else if (param == CMD_DELIM) {
				if (cmdStr != "") {
					icmd = cmdStr.toInt();
					tab_zone_param[i] = icmd;
					i++;
					cmdStr = "";
				}
				param = Serial3.read();
			}
			else {
				cmdStr += param;
				param = Serial3.read();
			}
		}
		writeMsg("W");
	}
	/*..........................................................*/
	/*..........................................................*/
	/*						ENVOI DONNEES						*/
	/*..........................................................*/
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

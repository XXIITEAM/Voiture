/*!
 * @file Voiture.ino
 * @brief Programme voiture arduino
 * @author XXII-TEAM
 * @version 0.1
 * @date 22 juin 2019
 *
 * Programme Arduino voiture commandée par application Android (BTArdroid).
 *
 */
#include "Arduino.h"
#include "Lib/XXIISensorLib/XXIISensorLib.h"
#include <String.h>
#include <MotorDriver.h>
#include <stdlib.h>
#include <EEPROM.h>
#include <DHT.h>

/*-------ULTRASONIC CONFIGURATION-------*/

//Niveaux d'alerte distance
#define NV_ALERTE_1				1
#define NV_ALERTE_2				2
#define NV_ALERTE_3				3
#define NV_ALERTE_4				4
#define NV_ALERTE_5				5

//Bool d'alerte active
#define ALERTE false

XXIISensorLib Sensor;
int alerteCentre;
int alerteGauche;
int alerteDroite;

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
#define CAR_STOP 0
#define CAR_FORWARD 1
#define CAR_BACK 2
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

/*!
 * @struct optionStruct
 * @brief Structure int.
 *
 * Stockage des paramètres de configuration des distances.
 * Récupéré et mis à jour par BTArdroid
 *
 */
struct optionStruct {
	unsigned long magic;
	int zone_4_max;
	int zone_4_min;
	int zone_3_min;
	int zone_2_min;
	int zone_1_min;
};
optionStruct os_write;
/*!
 * @struct paramStruct
 * @brief Structure int.
 *
 * Stockage des paramètres de configuration des distances.
 * Récupéré et mis à jour par BTArdroid
 *
 */
struct paramStuct {
	int struct_cm_s;
};
paramStuct ps_write;

/*!
 * @fn void setup()
 * @brief Fonction Setup.
 *
 * @param Aucun.
 * @return Aucun
 */
void setup() {
	Serial.begin(115200);
	Serial3.begin(115200);
	s_connecting = "CONNECTING";
	s_connected = "CONNECTED";
	status = 0;
	motordriver.init();
	motordriver.setSpeed(speed0, MOTORA);
	motordriver.setSpeed(speed0, MOTORB);
	chargerParametres();
	waitPairable();
	waitConnected();
	mode = "M";
	//dht.begin();
}

/*!
 * @fn void loop()
 * @brief Fonction loop.
 *
 * @param Aucun.
 * @return Aucun
 */
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
/*!
 * @fn void decouverte()
 * @brief Fonction découverte environnement.
 *
 * @param Aucun
 * @return Aucun
 */
void decouverte() {
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

/*!
 * @fn float calibration()
 * @brief Fonction calibration vitesse vh (cm/s)
 * Placer le véhicule face à un mur (1 ou 2 m)
 *
 * @param Aucun
 * @return cm_s
 */
float calibration() {
	//bool second passage (cycle mesure > MAV > mesure > MAR > mesure > calcul) x2
	boolean second;
	second = false;
	//Test récup echo indirect
	int echo_direct;
	//var temps
	long departMillis, stopMillis, execTime;
	float avg, avc, avd, arg, arc, ard;
	double cm_s, cm_s_av, oavc, oarc, cm_s_ar, cm_s_premier, cm_s_second;
	//Boucle calibration
	for (int i = 0; i < 2; i++) {
		Sensor.ScanAv(&avg, &avc, &avd);
		oavc = avc;
		departMillis = millis();
		motordriver.goForward();
		do {

		} while (millis() - departMillis <= 5000);
		stopMillis = millis();
		motordriver.stop();
		Sensor.ScanAv(&avg, &avc, &avd);
		oavc = oavc - avc;
		cm_s_av = oavc / 5;


		Sensor.ScanAv(&avg, &avc, &avd);
		oarc = avc;
		departMillis = millis();
		motordriver.goBackward();
		do {

		} while (millis() - departMillis <= 5000);
		stopMillis = millis();
		motordriver.stop();
		Sensor.ScanAv(&avg, &avc, &avd);
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

/*..........................................................*/
/*..........................................................*/
/*					TRAITEMENT COMMANDE						*/
/*..........................................................*/
/*..........................................................*/

/*!
 * @fn void traitementMessage(char commande_a_traiter)
 * @brief Fonction traitement du message reçu (BTArduino)
 *
 * @param char commande_a_traiter
 * @return Aucun
 */
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
		sauvegardeParametres();
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
     default :
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

/*!
 * @fn void optDist()
 * @brief Envoi des parametres (Distances zones) a BTArduino
 *
 * @param Aucun
 * @return Aucun
 */
void optDist() {
	chargerParametres();
	String str_list_zone;
	str_list_zone += "O/";
	for (int i = 0; i < 5; i++) {
		str_list_zone += String(tab_zone_param[i]) + CMD_DELIM;
	}
	str_list_zone += "X";
	writeMsg(str_list_zone);
	return;
}

/*!
 * @fn void traitementOptions(char cmd)
 * @brief Traitement du String parametres reçu de BTArduino
 *
 * @param char cmd
 * @return Aucun
 */
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

/*!
 * @fn void listingBT()
 * @brief Envoie des données capteurs a BTArduino
 *
 * @param Aucun
 * @return Aucun
 */
void listingBT() {
	float cmMsec = 12.0;
	String distance = String(cmMsec);
	String list = ("Z" "/" "Distance : " + distance + "/" + temperature() );
	//writeMsg(list);
}

/*..........................................................*/
/*..........................................................*/
/*					SAUVEGARDE EEPROM						*/
/*..........................................................*/
/*..........................................................*/

/*!
 * @fn void sauvegardeParametres()
 * @brief Sauvegarde des parametres utilisateurs de tableau parametres vers EEPROM
 *
 * @param Aucun
 * @return Aucun
 */
void sauvegardeParametres() {
	os_write.magic = STRUCT_MAGIC;
	os_write.zone_1_min = tab_zone_param[0];
	os_write.zone_2_min = tab_zone_param[1];
	os_write.zone_3_min = tab_zone_param[2];
	os_write.zone_4_min = tab_zone_param[3];
	os_write.zone_4_max = tab_zone_param[4] ;
	EEPROM.put(0, os_write);
	writeMsg("Q");
}

/*!
 * @fn void chargerParametres()
 * @brief Chargement des parametres utilisateurs de EEPROM vers tableau de parametres
 *
 * @param Aucun
 * @return Aucun
 */
void chargerParametres() {
	Serial.println("charger param");
	EEPROM.get(0, os_write);
	// Test initialisation de la m�moire
	byte erreur = os_write.magic != STRUCT_MAGIC;
	
	// Si erreur on attribue des valeurs par d�faut
	if (erreur) {
		Serial.println("Erreur struct");
		os_write.magic = STRUCT_MAGIC;
		os_write.zone_1_min = 10;
		os_write.zone_2_min = 30;
		os_write.zone_3_min = 50;
		os_write.zone_4_min = 70;
		os_write.zone_4_max = 100;
	}
	// Mise � jour du tableau de param�tres
	updateTableauParam();
}

/*!
 * @fn void updateTableauParam()
 * @brief Mise à jour du tableau de parametres a partir de EEPROM
 *
 * @param Aucun
 * @return Aucun
 */
void updateTableauParam() {
	Serial.println("update tab param");
	tab_zone_param[0] = os_write.zone_1_min;
	tab_zone_param[1] = os_write.zone_2_min;
	tab_zone_param[2] = os_write.zone_3_min;
	tab_zone_param[3] = os_write.zone_4_min;
	tab_zone_param[4] = os_write.zone_4_max;
}

/*!
 * @fn String temperature()
 * @brief Récupération temperature et humidite pour envoie vers BTArdroid
 *
 * @param Aucun
 * @return String th
 */
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
		String th = "Température: " + temp + "°C/" + "Hygrométrie: " + hydro +"%/";
		return th;
	}
	
	
}

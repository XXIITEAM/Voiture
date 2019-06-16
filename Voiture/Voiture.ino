//#include <XXIISensorLib.h>
#include "XXIISensorLib.h"
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
//BT
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
uint8_t speed0 = 220;
//Définition STRUCT_MAGIC (test data)
static const unsigned long STRUCT_MAGIC = 22;
int tab_zone_param[5];
int8_t status;
String s_connecting;
String s_connected;
String mode;
struct optionStruct {
	unsigned long magic;
	int zone_4_max;
	int zone_4_min;
	int zone_3_min;
	int zone_2_min;
	int zone_1_min;
};

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
	US_scan_Av();
	US_scan_Ar();
	waitPairable();
	waitConnected();
	mode = "M";
	//dht.begin();
}
char commande_precedente = 'I';
char commande_recue;
char CMD_VIDE = NULL;
#define CAR_STOP 0
#define CAR_FORWARD 1
#define CAR_BACK 2
uint8_t car_status = CAR_STOP;
uint8_t new_status = car_status;

optionStruct os_write;
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

void loop() {
	/*commande_recue = readByte();
	float dist_av_g, dist_av_c, dist_av_d, dist_ar_d, dist_ar_c, dist_ar_g;
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
	if (currentMillis - previousMillisDIST >= TEMPO_DIST) {
		US_scan_Av();
		US_scan_Ar();
		previousMillisDIST = currentMillis;

	}


}
/*..........................................................*/
/*..........................................................*/
/*					BLUETOOTH								*/
/*..........................................................*/
/*..........................................................*/
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
				//delay(200);
				if (testAT()) status = PAIRABLE;
				else status = CONNECTED;
			}
		}
	}
}

int8_t getStatus() {
	return status;
}

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

/*AT commandes*/
bool writeAT(String cmd) {
	Serial3.println(cmd);
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
bool writeMsg(char cmd) {
	Serial3.println(cmd);
}
bool testAT() {
	clearBuffer();
	return writeAT("AT");
}
void clearBuffer() {
	char recvChar;
	while (Serial3.available())recvChar = Serial3.read();
}

char readByte() {
	if (status != CONNECTED) return 0xff;
	if (Serial3.available())
	{
		char recvChar;
		recvChar = Serial3.read();
		if (recvChar == '+')//the remote control disconnect the car
		{
			waitPairable();
			return 0xff;
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
void autonome() {
	float av_g;
	if (av_g >= tab_zone_param[4]) {
			motordriver.goRight();
			delay(TEMPO_MOVE);
	}

	if (av_g < tab_zone_param[4] && av_g >= tab_zone_param[3]) {
			motordriver.goForward();
			delay(TEMPO_MOVE);

	}
	if (av_g < tab_zone_param[3] && av_g >= tab_zone_param[2]) {
			motordriver.goForward();
            Serial.println("Avance");
			delay(TEMPO_MOVE);

	}
	if (av_g < tab_zone_param[2] && av_g >= tab_zone_param[1]) {
			motordriver.goLeft();
			delay(TEMPO_MOVE);

	}
	if (av_g < tab_zone_param[1] && av_g >= tab_zone_param[0]) {
			motordriver.goBackward();
			delay(TEMPO_MOVE);

	}
	if (av_g < tab_zone_param[0]) {
			motordriver.goBackward();
			delay(TEMPO_MOVE);

	}
	if (av_g == 0) {
		Serial.println("Pas de mouvement");
	}
	return;
}
/*..........................................................*/
/*..........................................................*/
/*					TRAITEMENT COMMANDE						*/
/*..........................................................*/
/*..........................................................*/
void traitementMessage(char commande_a_traiter) {
	switch (commande_a_traiter)
	{

	case CMD_FORWARD:
		motordriver.goForward();
		break;
	/*case CMD_RIGHT_FORWARD:
		motordriver.goForward();
    Serial.println("Avance Droite");
		rightForward();
		motordriver.setSpeed(speed0, MOTORB);
		break;*/
	case CMD_LEFT_FORWARD:
		motordriver.goForward();
		leftForward();
		motordriver.setSpeed(speed0, MOTORA);
		break;
	case CMD_RIGHT_BACK:
		motordriver.goBackward();
		rightForward();
		motordriver.setSpeed(speed0, MOTORB);
		break;
	case CMD_LEFT_BACK:
		motordriver.goBackward();
		leftForward();
		motordriver.setSpeed(speed0, MOTORA);
		break;
	case CMD_RIGHT_FRONT:
		motordriver.goRight();
		break;
	case CMD_BACKWARD:
		motordriver.goBackward();
		break;
	case CMD_LEFT_FRONT:
		motordriver.goLeft();
		break;
	case CMD_STOP:
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
		writeAT("A");
		mode = "A";
		break;
	case CMD_MANUELLE:
		motordriver.stop();
		mode = "M";
		writeAT("M");
		break;
	case CMD_TEMP:
		//temperature();
		break;
     default :
 break;
	}
	return;
}
/*..........................................................*/
/*..........................................................*/
/*					SCAN US FRONT							*/
/*..........................................................*/
/*..........................................................*/
float * US_scan_Av() {
	float dist_av_g, dist_av_c, dist_av_d;
	Sensor.ScanAv(&dist_av_g, &dist_av_c, &dist_av_d);
 
  float US_dists_av[] = { dist_av_g, dist_av_c, dist_av_d};
  Serial.println("Distances AV");
  Serial.print("Gauche : ");
  Serial.println(dist_av_g);
  Serial.print("Centre : ");
  Serial.println(dist_av_c);
  Serial.print("Droite : ");
  Serial.println(dist_av_d);
  Serial.println("*--------------------*");
  return;
  //algoObstacles(dist_av_g, dist_av_c, dist_av_d);
}

float * US_scan_Ar() {
	float dist_ar_g, dist_ar_c, dist_ar_d;
	Sensor.ScanAr(&dist_ar_g, &dist_ar_c, &dist_ar_d);

	float US_dists_ar[] = {dist_ar_g, dist_ar_c, dist_ar_d};
	//algoObstacles(dist_ar_g, dist_ar_c, dist_ar_d);
	Serial.println("Distances AR");
	Serial.print("Gauche : ");
	Serial.println(dist_ar_g);
	Serial.print("Centre : ");
	Serial.println(dist_ar_c);
	Serial.print("Droite : ");
	Serial.println(dist_ar_d);
	return;
	//return US_dists_ar;
}
void algoObstacles(float gauche, float centre, float droite) {
	Serial.print(" Gauche : ");
	Serial.println(gauche);
	Serial.print(", Face : ");
	Serial.println(centre);
	Serial.print(", Droite: ");
	Serial.println(droite);

	//CENTRE
	if ((centre > tab_zone_param[4])) {
		alerteCentre = NV_ALERTE_1;
	}
	if ((centre <= tab_zone_param[4]) && (centre > tab_zone_param[3])) {
		alerteCentre = NV_ALERTE_2;
	}
	if ((centre <= tab_zone_param[3]) && (centre > tab_zone_param[2])) {
		alerteCentre = NV_ALERTE_3;
	}
	if ((centre <= tab_zone_param[2]) && (centre > tab_zone_param[1])) {
		alerteCentre = NV_ALERTE_4;
	}
	if ((centre <= tab_zone_param[1]) && (centre > tab_zone_param[0])) {
		alerteCentre = NV_ALERTE_5;
	}



	if ((gauche > tab_zone_param[4])) {
		alerteGauche = NV_ALERTE_1;
	}
	if ((gauche <= tab_zone_param[4]) && (gauche > tab_zone_param[3])) {
		alerteGauche = NV_ALERTE_2;
	}
	if ((gauche <= tab_zone_param[3]) && (gauche > tab_zone_param[2])) {
		alerteGauche = NV_ALERTE_3;

	}
	if ((gauche <= tab_zone_param[2]) && (gauche > tab_zone_param[1])) {
		alerteGauche = NV_ALERTE_4;
	}
	if ((gauche <= tab_zone_param[1]) && (gauche > tab_zone_param[0])) {
		alerteGauche = NV_ALERTE_5;
	}

	//DROITE
	if ((droite > tab_zone_param[4])) {
		alerteDroite = NV_ALERTE_1;
	}
	if ((droite <= tab_zone_param[4]) && (droite > tab_zone_param[3])) {
		alerteDroite = NV_ALERTE_2;
	}
	if ((droite <= tab_zone_param[3]) && (droite > tab_zone_param[2])) {
		alerteDroite = NV_ALERTE_3;
	}
	if ((droite <= tab_zone_param[2]) && (droite > tab_zone_param[1])) {
		alerteDroite = NV_ALERTE_4;
	}
	if ((droite <= tab_zone_param[1]) && (droite > tab_zone_param[0])) {
		alerteDroite = NV_ALERTE_5;
	}

	if ((alerteCentre >= NV_ALERTE_3) || (alerteDroite >= NV_ALERTE_3) || (alerteGauche >= NV_ALERTE_3)) {
		float mface, mdroite, mgauche;
		mface = centre;
		mdroite = droite;
		mgauche = gauche;
		float minTemp = min(mdroite, min(mgauche, mface));
		if (mdroite == minTemp) {
			//Serial.println("alerte Droite niveau " + alerteDroite);
			//Tourner à gauche
		}
		if (mgauche == minTemp) {
			//Serial.println("alerte Gauche niveau " + alerteGauche);
			//Tourner à droite
			if (alerteCentre >= NV_ALERTE_3) {

			}
		}
		if (mface == minTemp) {
			//Serial.println("alerte Face niveau " + alerteCentre);
			minTemp = min(alerteDroite, alerteGauche);
			if (alerteDroite == minTemp) {
				switch (alerteDroite) {
				case NV_ALERTE_3:
					break;
				case NV_ALERTE_2:
					break;
				case NV_ALERTE_1:
					break;
				}
				//reculer en s'écartant de l'obstacle (tourner à gauche)
			}
			else {
				//reculer en s'écartant de l'obstacle (tourner à droite)
				switch (alerteGauche) {
				case NV_ALERTE_3:
					break;
				case NV_ALERTE_2:
					break;
				case NV_ALERTE_1:
					break;
				}
			}

		}
		//Serial.println("Aucun");

	}
	else {
		//Serial.println("Pas d'alerte");
	}


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
//Envoi des param�tres (Distances zones) � l'application android
void optDist() {
	chargerParametres();
	String str_list_zone;
	str_list_zone += "O/";
	for (int i = 0; i < 5; i++) {
		str_list_zone += String(tab_zone_param[i]) + CMD_DELIM;

	}
	str_list_zone += "X";

	Serial3.println(str_list_zone);
	//Serial.println(str_list_zone);
	return;

}

//Traite les param�tres de zones envoy�s par le client
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

	Serial3.println("W");
}
/*..........................................................*/
/*..........................................................*/
/*						ENVOI DONNEES						*/
/*..........................................................*/
/*..........................................................*/
void listingBT() {
	float cmMsec = 12.0;
	String distance = String(cmMsec);
	String list = ("Z" "/" "Distance : " + distance + "/" + temperature() );
	//Serial3.println(list);
}
/*..........................................................*/
/*..........................................................*/
/*					SAUVEGARDE EEPROM						*/
/*..........................................................*/
/*..........................................................*/
void sauvegardeParametres() {
	os_write.magic = STRUCT_MAGIC;
	os_write.zone_1_min = tab_zone_param[0];
	os_write.zone_2_min = tab_zone_param[1];
	os_write.zone_3_min = tab_zone_param[2];
	os_write.zone_4_min = tab_zone_param[3];
	os_write.zone_4_max = tab_zone_param[4] ;
	EEPROM.put(0, os_write);
	writeAT("Q");
}
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
void updateTableauParam() {
	Serial.println("update tab param");
	tab_zone_param[0] = os_write.zone_1_min;
	tab_zone_param[1] = os_write.zone_2_min;
	tab_zone_param[2] = os_write.zone_3_min;
	tab_zone_param[3] = os_write.zone_4_min;
	tab_zone_param[4] = os_write.zone_4_max;
}
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
		String hygro = String(h);
    //float cmMsec = US_scan_Av();
		String th = "Température: " + temp + "°C/" + "Hygrométrie: "+hygro+"%/";
		return th;
	}
	
	
}

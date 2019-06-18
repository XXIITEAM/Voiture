//#include <XXIISensorLib.h>
#include <XXIISensorLib.h>
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
boolean boolFront = false;
boolean boolStop = false;
boolean boolBackward = false;
boolean boolCalib = false;

//Structures
struct optionStruct {
	unsigned long magic;
	int zone_4_max;
	int zone_4_min;
	int zone_3_min;
	int zone_2_min;
	int zone_1_min;
};
struct paramStuct {
	int struct_cm_s;
};
paramStuct ps_write;

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
void ScanUS() {
	float avg, avc, avd, arg, arc, ard;
	Sensor.ScanAv(&avg, &avc, &avd);
	Sensor.ScanAr(&arg, &arc, &ard);
	algoObstacles(avg, avc, avd, arg, arc, ard);
}


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
	//Si distance min dans tout les capteurs < 50.00 cm on agit sinon on continue
	if (min(tabDistances[0], min(tabDistances[1], min(tabDistances[2], min(tabDistances[3], min(tabDistances[4], tabDistances[5]))))) < float(50)) {
		Serial.println("Entrée dans -50cm");
		if (tabAlerte[0] > NV_ALERTE_4 || tabAlerte[1] > NV_ALERTE_4 || tabAlerte[2] > NV_ALERTE_4) {
			//Alerte face
				Serial.println("Entrée dans -50cm");
				boolFront = false;
				boolStop = false;
				boolBackward = true;
				motordriver.goBackward();
			
		}
		else if (tabAlerte[3] > NV_ALERTE_4 || tabAlerte[4] > NV_ALERTE_4 || tabAlerte[5] > NV_ALERTE_4) {
			//Alerte arrière
			Serial.println("Entrée dans -50cm Arrière");
				boolFront = true;
				boolStop = false;
				boolBackward = false;
				motordriver.goForward();
		}
		else {
			if (boolStop == false) {
				boolFront = false;
				boolStop = true;
				boolBackward = false;
				motordriver.stop();
			}
		}
		

	}
	


}

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
//Fonction mesure dist/s
float calibration() {
	boolean second;
	second = false;
	int echo_direct;
	long departMillis, stopMillis, execTime;
	float avg, avc, avd, arg, arc, ard;
	double cm_s, cm_s_av, oavc, oarc, cm_s_ar, cm_s_premier, cm_s_second;
	for (int i = 0; i < 2; i++) {
		Serial.print("bool state : ");
		Serial.println(second);
		Sensor.ScanAv(&avg, &avc, &avd);
		Serial.print("cm_1 MAV : ");
		Serial.println(avc);
		oavc = avc;
		departMillis = millis();
		motordriver.goForward();
		do {

		} while (millis() - departMillis <= 5000);
		stopMillis = millis();
		motordriver.stop();
		Sensor.ScanAv(&avg, &avc, &avd, &echo_direct);
		Serial.print("cm_2 MAV : ");
		Serial.println(avc);
		oavc = oavc - avc;
		Serial.print("Distance parcourue : ");
		Serial.println(oavc);
		cm_s_av = oavc / 5;


		Sensor.ScanAv(&avg, &avc, &avd, &echo_direct);
		Serial.print("cm_1 MAR : ");
		Serial.println(avc);
		oarc = avc;
		departMillis = millis();
		motordriver.goBackward();
		do {

		} while (millis() - departMillis <= 5000);
		stopMillis = millis();
		motordriver.stop();
		Sensor.ScanAv(&avg, &avc, &avd);
		Serial.print("cm_2 MAR : ");
		Serial.println(avc);
		oarc = avc - oarc;
		Serial.print("Distance pacrcourue : ");
		Serial.println(oarc);
		cm_s_ar = oarc / 5;
		if (second == true) {
			cm_s_second = (cm_s_av + cm_s_ar) / 2;
			Serial.print("cm/s second passage : ");
			Serial.println(cm_s_second);
		}
		else {
			cm_s_premier = (cm_s_av + cm_s_ar) / 2;
			Serial.print("cm/s premier passage : ");
			Serial.println(cm_s_premier);
		}
		second = true;

	}
	cm_s = (cm_s_premier + cm_s_second) / 2;
	second = false;
	boolCalib = true;
	ps_write.struct_cm_s = cm_s;
	EEPROM.put(100, ps_write);
	return cm_s;
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
		boolFront = true;
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
		boolFront = true;
		boolStop = false;
		boolBackward = false;
		motordriver.goForward();
		leftForward();
		motordriver.setSpeed(speed0, MOTORA);
		break;
	case CMD_RIGHT_BACK:
		boolFront = false;
		boolStop = false;
		boolBackward = true;
		motordriver.goBackward();
		rightForward();
		motordriver.setSpeed(speed0, MOTORB);
		break;
	case CMD_LEFT_BACK:
		boolFront = false;
		boolStop = false;
		boolBackward = true;
		motordriver.goBackward();
		leftForward();
		motordriver.setSpeed(speed0, MOTORA);
		break;
	case CMD_RIGHT_FRONT:
		boolFront = true;
		boolStop = false;
		boolBackward = false;
		motordriver.goRight();
		break;
	case CMD_BACKWARD:
		boolFront = false;
		boolStop = false;
		boolBackward = true;
		motordriver.goBackward();
		break;
	case CMD_LEFT_FRONT:
		boolFront = false;
		boolStop = false;
		boolBackward = false;
		motordriver.goLeft();
		break;
	case CMD_STOP:
		boolFront = false;
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
		String hydro = String(h);
    //float cmMsec = US_scan_Av();
		String th = "Température: " + temp + "°C/" + "Hygrométrie: " + hydro +"%/";
		return th;
	}
	
	
}

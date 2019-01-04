//test 1:2015.08.15
//#include <car_bluetooth.h>
//#include <SoftwareSerial.h>   //Software Serial Port
#include <String.h>
#include <MotorDriver.h>
#include <stdlib.h>
#include <EEPROM.h>
#include <DHT.h>
#include <Ultrasonic.h>
//Définition capteur_front_center
#define TRIGGER_PIN  5
#define ECHO_PIN     3
#define DHTPIN A0
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

Ultrasonic ultrasonic(TRIGGER_PIN, ECHO_PIN);
//Définition des commandes
//BT
#define INITIALIZING 0
#define READY        1
#define INQUIRING    2
#define PAIRABLE     3
#define CONNECTING   4
#define CONNECTED    5
//CMD
#define CMD_INVALID     0XFF
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
struct optionStruct {
	unsigned long magic;
	int zone_4_max;
	int zone_4_min;
	int zone_3_min;
	int zone_2_min;
	int zone_1_min;
};

void setup() {
	Serial.begin(9600);
	Serial3.begin(9600);
	s_connecting = "CONNECTING";
	s_connected = "CONNECTED";
	status = 0;
	motordriver.init();
	motordriver.setSpeed(180, MOTORA);
	motordriver.setSpeed(180, MOTORB);
	chargerParametres();
	waitPairable();
	waitConnected();
	dht.begin();
}

#define CAR_STOP 0
#define CAR_FORWARD 1
#define CAR_BACK 2
uint8_t car_status = CAR_STOP;
uint8_t new_status = car_status;
String mode = "M";
optionStruct os_write;
uint8_t bt_command;
// Tempo
const unsigned long TEMPO_DIST = 100;
const unsigned long TEMPO_BT = 50;
const unsigned long TEMPO_MOVE = 200;
const unsigned long TEMPO_TURN = 500;
// Pr�c�dente valeur de millis() Tempo
unsigned long previousMillisBT = 0;

unsigned long previousMillisDIST = 0;
unsigned long previousMillisMove = 0;
unsigned long previousMillisTurn = 0;
unsigned long currentMillis;

void loop() {
	char commande_recue = readByte();
	char commande_precedente;
	float cmMsec;
	if (getStatus() == PAIRABLE) {
		motordriver.stop();
		waitConnected();
	}
	currentMillis = millis();
	if (currentMillis - previousMillisDIST >= TEMPO_DIST) {
		previousMillisDIST = currentMillis;
		cmMsec = scanFrontCenter();
	}
	if (commande_recue != CMD_INVALID) {
		if (commande_precedente == NULL) {
			commande_precedente = commande_recue;
			traitementMessage(commande_recue);
		}
		else if (commande_recue != commande_precedente) {
			commande_precedente = commande_recue;
			traitementMessage(commande_recue);

		}
	}
	if (mode == "A") {
		autonome(cmMsec);
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
				delay(200);
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
				//Serial.write(recvChar);
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

/*Write AT command to bluetooth module*/
bool writeAT(String cmd) {
	Serial3.println(cmd);
	delay(500);
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
void autonome(float distance_front) {
	
	if (distance_front >= tab_zone_param[4]) {
			motordriver.goRight();
			delay(TEMPO_MOVE);
	}

	if (distance_front < tab_zone_param[4] && distance_front >= tab_zone_param[3]) {
			motordriver.goForward();
			delay(TEMPO_MOVE);

	}
	if (distance_front < tab_zone_param[3] && distance_front >= tab_zone_param[2]) {
			motordriver.goForward();
			delay(TEMPO_MOVE);

	}
	if (distance_front < tab_zone_param[2] && distance_front >= tab_zone_param[1]) {
			motordriver.goLeft();
			delay(TEMPO_MOVE);

	}
	if (distance_front < tab_zone_param[1] && distance_front >= tab_zone_param[0]) {
			motordriver.goBackward();
			delay(TEMPO_MOVE);

	}
	if (distance_front < tab_zone_param[0]) {
			motordriver.goBackward();
			delay(TEMPO_MOVE);

	}

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
	case CMD_RIGHT_FORWARD:
		motordriver.goForward();
		rightForward();
		motordriver.setSpeed(speed0, MOTORB);
		break;
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
		listingBT();
		break;
	case CMD_INVALID:
		motordriver.stop();		

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
		temperature();
		break;
	default: break;
	}
}
/*..........................................................*/
/*..........................................................*/
/*					SCAN US FRONT							*/
/*..........................................................*/
/*..........................................................*/
float scanFrontCenter() {
  long microsec = ultrasonic.timing();
  float distance = ultrasonic.convert(microsec, Ultrasonic::CM);
  return distance;
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

	writeAT(str_list_zone);
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
	writeAT("W");
}
/*..........................................................*/
/*..........................................................*/
/*						ENVOI DONNEES						*/
/*..........................................................*/
/*..........................................................*/
void listingBT() {
	float cmMsec = scanFrontCenter();
	String distance = String(cmMsec);
	String list = ("Z" "/" "Distance : " + distance + "/" + "Temp�rature : " + "21.02" + "/" + "Temp�rature : " + "21.02" + "/" + "Temp�rature : " + "21.02" + "/" + "Temp�rature : " + "21.02" + "/" + "Temp�rature : " + "21.02");
	writeAT(list);
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
	EEPROM.get(0, os_write);
	// Test initialisation de la m�moire
	byte erreur = os_write.magic != STRUCT_MAGIC;
	
	// Si erreur on attribue des valeurs par d�faut
	if (erreur) {
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
	tab_zone_param[0] = os_write.zone_1_min;
	tab_zone_param[1] = os_write.zone_2_min;
	tab_zone_param[2] = os_write.zone_3_min;
	tab_zone_param[3] = os_write.zone_4_min;
	tab_zone_param[4] = os_write.zone_4_max;
}
void temperature() {
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
		String th = "T" "/" "Température: " + temp + "°C/" + "Hygrométrie: "+hygro+"%";
    Serial.println(th);
		writeAT(th);
	}
}

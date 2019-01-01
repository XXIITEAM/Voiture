//test 1:2015.08.15
//#include <car_bluetooth.h>
//#include <SoftwareSerial.h>   //Software Serial Port
#include <String.h>
#include <MotorDriver.h>
#include <stdlib.h>
#include <EEPROM.h>

#include <Ultrasonic.h>
//Définition capteur_front_center
#define TRIGGER_PIN  5
#define ECHO_PIN     3
Ultrasonic ultrasonic(TRIGGER_PIN, ECHO_PIN);
//Définition des commandes
#define CMD_INVALID     '255'
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
#define SPEED_STEPS 20
//Définition vitesse
uint8_t speed0 = 220;
//Définition STRUCT_MAGIC (test data)
static const unsigned long STRUCT_MAGIC = 22;
int tab_zone_param[5];

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
	motordriver.init();
	motordriver.setSpeed(180, MOTORA);
	motordriver.setSpeed(180, MOTORB);
	chargerParametres();
}
#define CAR_STOP 0
#define CAR_FORWARD 1
#define CAR_BACK 2
uint8_t car_status = CAR_STOP;
uint8_t new_status = car_status;
String mode = "M";
optionStruct os_write;
// Tempo
const unsigned long TEMPO_DIST = 500;
const unsigned long TEMPO_BT = 10;
const unsigned long TEMPO_MOVE = 200;
const unsigned long TEMPO_TURN = 500;
// Pr�c�dente valeur de millis() Tempo
unsigned long previousMillisBT = 0;

unsigned long previousMillisDIST = 0;
unsigned long previousMillisMove = 0;
unsigned long previousMillisTurn = 0;
unsigned long currentMillis;

void loop() {
	
	char commande_recue = getBluetoothMessage();
	char commande_precedente;
	float cmMsec;
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
	
	currentMillis = millis();
	if (currentMillis - previousMillisDIST >= TEMPO_DIST) {
		previousMillisDIST = currentMillis;
		cmMsec = scanFrontCenter();
	}
	
	if (mode == "A") {
		autonome(cmMsec);
	}
}
/*..........................................................*/
/*..........................................................*/
/*					BOUCLE RECEPTION BT						*/
/*..........................................................*/
/*..........................................................*/
char getBluetoothMessage() {
	char bt_command = Serial3.read();
	return bt_command;

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
		previousMillisDIST = millis();
		while (currentMillis - previousMillisDIST >= TEMPO_MOVE) {
			currentMillis = millis();
		}
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
		Serial3.println("A");
		mode = "A";
		break;
	case CMD_MANUELLE:
		motordriver.stop();
		mode = "M";
		Serial3.println("M");

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

	Serial3.println(str_list_zone);
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
		else if (cmd == CMD_DELIM) {
			if (cmdStr != "") {
				icmd = cmdStr.toInt();
				tab_zone_param[i] = icmd;
				i++;
				cmdStr = "";
			}
			param = Serial3.read();
		}
		else {
			cmdStr += cmd;
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
	float cmMsec = scanFrontCenter();
	String distance = String(cmMsec);
	String list = ("Z" "/" "Distance : " + distance + "/" + "Temp�rature : " + "21.02" + "/" + "Temp�rature : " + "21.02" + "/" + "Temp�rature : " + "21.02" + "/" + "Temp�rature : " + "21.02" + "/" + "Temp�rature : " + "21.02");
	Serial3.println(list);
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
	Serial3.println("Q");
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
	

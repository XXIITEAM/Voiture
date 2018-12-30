//test 1:2015.08.15
#include <car_bluetooth.h>
#include <SoftwareSerial.h>   //Software Serial Port
#include <String.h>
#include <MotorDriver.h>
#include <stdlib.h>
#include <eeprom.h>

#include <Ultrasonic.h>

#define TRIGGER_PIN  5//connect Trip of the Ultrasonic Sensor moudle to D5 of Arduino 
//and can be changed to other ports
#define ECHO_PIN     3
Ultrasonic ultrasonic(TRIGGER_PIN, ECHO_PIN);
// Only run 50 time so we can reburn the code easily.
int CYCLES = 200;
int CYCLES2 = 3;
int count = 0;
int count2 = 0;
bool test;
float cmMsec;
#define RxD 2
#define TxD 4
CarBluetooth bluetooth(RxD, TxD);
#define CMD_INVALID     0xFF
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
uint8_t speed0 = 180;
static const unsigned long STRUCT_MAGIC = 22;


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
	motordriver.init();
	motordriver.setSpeed(180, MOTORA);
	motordriver.setSpeed(180, MOTORB);
	chargerParametres();
	bluetooth.waitPairable();
	Serial.println("pairable");
	Serial.println("*------------------------------*");
	bluetooth.waitConnected();
	Serial.println("connecte");
	Serial.println("*------------------------------*");
	

}
uint8_t bt_command;
String distance = "";
#define CAR_STOP 0
#define CAR_FORWARD 1
#define CAR_BACK 2
uint8_t car_status = CAR_STOP;
uint8_t new_status = car_status;
String str = "M";
int tab_zone_param[5];
optionStruct os_write;
// Tempo
const unsigned long TEMPO_DIST = 200;
const unsigned long TEMPO_BT = 50;
const unsigned long TEMPO_MOVE = 100;
const unsigned long TEMPO_TURN = 500;
// Précédente valeur de millis() Tempo
unsigned long previousMillisBT = 0;

unsigned long previousMillisDIST = 0;
unsigned long previousMillisMove = 0;
unsigned long previousMillisTurn = 0;


void loop() {


	unsigned long currentMillis = millis();
	if (currentMillis - previousMillisDIST >= TEMPO_DIST) {

		// Garde en mémoire la valeur actuelle de millis()
		previousMillisDIST = currentMillis;
		scanFrontCenter();

	}
	if (currentMillis - previousMillisBT >= TEMPO_BT) {
		previousMillisBT = currentMillis;
		getBluetoothMessage();
		if (str == "A") {
			autonome();
		}
	}
	
}
/*..........................................................*/
/*..........................................................*/
/*					BOUCLE RECEPTION BT						*/
/*..........................................................*/
/*..........................................................*/
void getBluetoothMessage() {
	bt_command = bluetooth.readByte();
	if (bt_command != CMD_INVALID) {
		traitementMessage(bt_command);
		
	}
	if (bluetooth.getStatus() == PAIRABLE) {
		motordriver.stop();
		bluetooth.waitConnected();

	}
	return;

}

/*..........................................................*/
/*..........................................................*/
/*						MODE AUTONOME						*/
/*..........................................................*/
/*..........................................................*/
void autonome() {
	unsigned long currentMillis = millis();

		if (cmMsec > tab_zone_param[4]) {
			motordriver.goRight();
			while (currentMillis - previousMillisMove < TEMPO_MOVE) {
				currentMillis = millis();
				previousMillisMove = currentMillis;
			}
		}

		if (cmMsec <= tab_zone_param[4] && cmMsec >= tab_zone_param[3]) {
			motordriver.goForward();
			while (currentMillis - previousMillisMove < TEMPO_MOVE) {
				currentMillis = millis();
				previousMillisMove = currentMillis;

			}


		}
		if (cmMsec <= tab_zone_param[3]-1 && cmMsec >= tab_zone_param[2]) {
			motordriver.goForward();
			while (currentMillis - previousMillisMove < TEMPO_MOVE) {
				currentMillis = millis();
				previousMillisMove = currentMillis;

			}


		}
		if (cmMsec <= tab_zone_param[2]-1 && cmMsec >= tab_zone_param[1]) {
			motordriver.goLeft();
			while (currentMillis - previousMillisTurn < TEMPO_TURN) {
				currentMillis = millis();
				previousMillisTurn = currentMillis;

			}

		}
		if (cmMsec <= tab_zone_param[1]-1 && cmMsec >= tab_zone_param[0]) {
			motordriver.goBackward();
			while (currentMillis - previousMillisTurn < TEMPO_TURN) {
				currentMillis = millis();
				previousMillisTurn = currentMillis;
				
			}
		}
		if (cmMsec < tab_zone_param[0]) {
			motordriver.goBackward();
		}

}


/*..........................................................*/
/*..........................................................*/
/*					TRAITEMENT COMMANDE						*/
/*..........................................................*/
/*..........................................................*/
void traitementMessage(uint8_t cmd) {
	switch (cmd)
	{
	case CMD_FORWARD:
		motordriver.goForward();
		bluetooth.writeAT("F");
		break;
	case CMD_RIGHT_FORWARD:
		rightForward();
		motordriver.goForward();
		motordriver.setSpeed(speed0, MOTORB);
		bluetooth.writeAT("D");

		break;
	case CMD_LEFT_FORWARD:
		leftForward();
		motordriver.goForward();
		motordriver.setSpeed(speed0, MOTORA);
		bluetooth.writeAT("G");

		break;
	case CMD_RIGHT_BACK:
		rightForward();
		motordriver.goBackward();
		motordriver.setSpeed(speed0, MOTORB);
		bluetooth.writeAT("F");

		break;
	case CMD_LEFT_BACK:
		leftForward();
		motordriver.goBackward();
		motordriver.setSpeed(speed0, MOTORA);
		bluetooth.writeAT("H");


		break;
	case CMD_RIGHT_FRONT:
		motordriver.goRight();
		bluetooth.writeAT("R");


		break;
	case CMD_BACKWARD:
		motordriver.goBackward();
		bluetooth.writeAT("B");


		break;
	case CMD_LEFT_FRONT:
		motordriver.goLeft();
		bluetooth.writeAT("L");

		break;
	case CMD_STOP:
		motordriver.stop();
		str = "S";
		bluetooth.writeAT("S");

		break;
	case CMD_OPT_DIST:
		optDist();
		break;
	case CMD_WRITE:
		traitementOptions(bt_command);
		break;
	case CMD_SAVE:
		sauvegardeParametres();
		bluetooth.writeAT("Q");
		break;
	case CMD_GETVALUES:
		listingBT();
		break;

	case CMD_INVALID:
		motordriver.stop();		

		break;
	case CMD_AUTONOME:
		bluetooth.writeAT("A");
		str = "A";

		break;
	case CMD_MANUELLE:
		motordriver.stop();
		str = "M";
		bluetooth.writeAT("M");

		break;
	default: break;
	}
	return;

}
/*..........................................................*/
/*..........................................................*/
/*					SCAN US FRONT							*/
/*..........................................................*/
/*..........................................................*/
void scanFrontCenter() {
  long microsec = ultrasonic.timing();
  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
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
//Envoi des paramètres (Distances zones) à l'application android
void optDist() {
	chargerParametres();
	String str_list_zone;
	str_list_zone += "O/";
	for (int i = 0; i < 5; i++) {
		str_list_zone += String(tab_zone_param[i]) + CMD_DELIM;

	}
	str_list_zone += "X";

	bluetooth.writeAT(str_list_zone);
	return;

}
//Traite les paramètres de zones envoyés par le client
void traitementOptions(char cmd) {
	String cmdStr = "";
	char cmd1[5];
	int icmd;
	int i = 0;
	while (cmd != STOP_CMD) {
		if (cmd == CMD_WRITE) {
			cmd = bluetooth.readByte();
		}
		else if (cmd == CMD_DELIM) {
			if (cmdStr != "") {
				icmd = cmdStr.toInt();
				tab_zone_param[i] = icmd;
				i++;
				cmdStr = "";
				/*Serial.println(tab_zone_param[i]);
				Serial.println(icmd);
				Serial.println();*/
			}
			cmd = bluetooth.readByte();
		}
		else {
			cmdStr += cmd;
			cmd = bluetooth.readByte();
		}
	}
	bluetooth.writeAT("W");
	return;
}
/*..........................................................*/
/*..........................................................*/
/*						ENVOI DONNEES						*/
/*..........................................................*/
/*..........................................................*/
void listingBT() {
	distance = String(cmMsec);
	String list = ("Z" "/" "Distance : " + distance + "/" + "Température : " + "21.02" + "/" + "Température : " + "21.02" + "/" + "Température : " + "21.02" + "/" + "Température : " + "21.02" + "/" + "Température : " + "21.02");
	bluetooth.writeAT(list);
	return;
}
/*..........................................................*/
/*..........................................................*/
/*					SAUVEGARDE EEPROM						*/
/*..........................................................*/
/*..........................................................*/
void sauvegardeParametres() {
	os_write.magic = STRUCT_MAGIC;
	os_write.zone_1_min= tab_zone_param[0];
	os_write.zone_2_min = 	tab_zone_param[1];
	os_write.zone_3_min = tab_zone_param[2];
	os_write.zone_4_min = tab_zone_param[3];
	os_write.zone_4_max = tab_zone_param[4] ;
	EEPROM.put(0, os_write);
	return;
}
void chargerParametres() {
	EEPROM.get(0, os_write);
	// Test initialisation de la mémoire
	byte erreur = os_write.magic != STRUCT_MAGIC;
	
	// Si erreur on attribue des valeurs par défaut
	if (erreur) {
		os_write.zone_1_min = 1;
		os_write.zone_2_min = 2;
		os_write.zone_3_min = 3;
		os_write.zone_4_min = 4;
		os_write.zone_4_max = 5;
	}
	// Mise à jour du tableau de paramètres
	updateTableauParam();
	//Sauvegarde en EEPROM des nouveaux paramètres
	sauvegardeParametres();
	return;

}
void updateTableauParam() {
	tab_zone_param[0] = os_write.zone_1_min;
	Serial.println(os_write.zone_1_min);
	tab_zone_param[1] = os_write.zone_2_min;
	tab_zone_param[2] = os_write.zone_3_min;
	tab_zone_param[3] = os_write.zone_4_min;
	tab_zone_param[4] = os_write.zone_4_max;
	return;

}
	
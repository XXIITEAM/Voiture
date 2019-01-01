//test 1:2015.08.15
//#include <car_bluetooth.h>
//#include <SoftwareSerial.h>   //Software Serial Port
#include <String.h>
#include <MotorDriver.h>
#include <stdlib.h>
#include <EEPROM.h>

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
#define RxD 14
#define TxD 15
//CarBluetooth bluetooth(RxD, TxD);

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
//bluetooth = Serial3;

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
	//bluetooth.waitPairable();
	
	Serial.println("pairable");
	Serial.println("*------------------------------*");
	//bluetooth.waitConnected();
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
const unsigned long TEMPO_MOVE = 200;
const unsigned long TEMPO_TURN = 500;
// Pr�c�dente valeur de millis() Tempo
unsigned long previousMillisBT = 0;

unsigned long previousMillisDIST = 0;
unsigned long previousMillisMove = 0;
unsigned long previousMillisTurn = 0;


void loop() {
	unsigned long currentMillis = millis();
	if (currentMillis - previousMillisDIST >= TEMPO_DIST) {

		// Garde en m�moire la valeur actuelle de millis()
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
	bt_command = Serial3.read();
  //Serial.println(  Serial3.read());
	if (bt_command != CMD_INVALID) {
		traitementMessage(bt_command);
    Serial.println(bt_command);
		  
	}
	/*if (bluetooth.getStatus() == PAIRABLE) {
		motordriver.stop();
		bluetooth.waitConnected();

	}*/
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

int8_t readCommand(char cmd) {
	char recvChar;
	char recvChar2;
	recvChar2 = Serial3.read();
	//int8_t cmd = 0;
	while (Serial3.read() == cmd) {
		//recvChar = Serial3.read();
			//traitementMessage(recvChar);
			//return cmd;
		delay(50);
	}
	return;
}
/*..........................................................*/
/*..........................................................*/
/*					TRAITEMENT COMMANDE						*/
/*..........................................................*/
/*..........................................................*/
void traitementMessage(char cmd) {
	switch (cmd)
	{
		//Serial3.println("O");
	case CMD_FORWARD:
		motordriver.goForward();
		readCommand(cmd);
		Serial3.println("F");
		break;
	case CMD_RIGHT_FORWARD:
		rightForward();
		motordriver.goForward();
		readCommand(cmd);
		motordriver.setSpeed(speed0, MOTORB);
		Serial3.println("D");

		break;
	case CMD_LEFT_FORWARD:
		leftForward();
		motordriver.goForward();
		readCommand(cmd);
		motordriver.setSpeed(speed0, MOTORA);
		Serial3.println("G");

		break;
	case CMD_RIGHT_BACK:
		rightForward();
		motordriver.goBackward();
		readCommand(cmd);
		motordriver.setSpeed(speed0, MOTORB);
		Serial3.println("F");

		break;
	case CMD_LEFT_BACK:
		leftForward();
		motordriver.goBackward();
		readCommand(cmd);
		motordriver.setSpeed(speed0, MOTORA);
		Serial3.println("H");


		break;
	case CMD_RIGHT_FRONT:
		motordriver.goRight();
		readCommand(cmd);
		Serial3.println("R");


		break;
	case CMD_BACKWARD:
		motordriver.goBackward();
		readCommand(cmd);
		Serial3.println("B");


		break;
	case CMD_LEFT_FRONT:
		motordriver.goLeft();
		readCommand(cmd);
		Serial3.println("L");

		break;
	case CMD_STOP:
		motordriver.stop();
		str = "S";
		Serial3.println("S");

		break;
	case CMD_OPT_DIST:
		optDist();
		break;
	case CMD_WRITE:
		traitementOptions(bt_command);
		break;
	case CMD_SAVE:
		sauvegardeParametres();
		Serial3.println("Q");
		break;
	case CMD_GETVALUES:
		listingBT();
		break;

	case CMD_INVALID:
		motordriver.stop();		

		break;
	case CMD_AUTONOME:
		Serial3.println("A");
		str = "A";

		break;
	case CMD_MANUELLE:
		motordriver.stop();
		str = "M";
		Serial3.println("M");

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
	char cmd1[5];
	int icmd;
	int i = 0;
	while (cmd != STOP_CMD) {
		if (cmd == CMD_WRITE) {
			cmd = Serial3.read();
		}
		else if (cmd == CMD_DELIM) {
			if (cmdStr != "") {
				icmd = cmdStr.toInt();
				tab_zone_param[i] = icmd;
				i++;
				cmdStr = "";
			}
			cmd = Serial3.read();
		}
		else {
			cmdStr += cmd;
			cmd = Serial3.read();
		}
	}
	Serial3.println("W");
	return;
}
/*..........................................................*/
/*..........................................................*/
/*						ENVOI DONNEES						*/
/*..........................................................*/
/*..........................................................*/
void listingBT() {
	distance = String(cmMsec);
	String list = ("Z" "/" "Distance : " + distance + "/" + "Temp�rature : " + "21.02" + "/" + "Temp�rature : " + "21.02" + "/" + "Temp�rature : " + "21.02" + "/" + "Temp�rature : " + "21.02" + "/" + "Temp�rature : " + "21.02");
	Serial3.println(list);
	return;
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
	Serial.println("****Sauvegarde Parametres****");
	Serial.println("Zone 1");
	Serial.println(tab_zone_param[0]);
	Serial.println("Zone 2");
	Serial.println(tab_zone_param[1]);
	Serial.println("Zone 3");
	Serial.println(tab_zone_param[2]);
	Serial.println("Zone 4");
	Serial.println(tab_zone_param[3]);
	Serial.println("Zone max");
	Serial.println(tab_zone_param[4]);
	Serial.println("****Parametres sauvegard�s****");
	Serial.println();

	return;
}
void chargerParametres() {
	EEPROM.get(0, os_write);
	// Test initialisation de la m�moire
	byte erreur = os_write.magic != STRUCT_MAGIC;
	
	// Si erreur on attribue des valeurs par d�faut
	if (erreur) {
		os_write.zone_1_min = 1;
		os_write.zone_2_min = 2;
		os_write.zone_3_min = 3;
		os_write.zone_4_min = 4;
		os_write.zone_4_max = 5;
	}
	// Mise � jour du tableau de param�tres
	updateTableauParam();
	//Sauvegarde en EEPROM des nouveaux param�tres
	sauvegardeParametres();
	Serial.println("****Chargement Parametres****");
	Serial.println("Zone 1");
	Serial.println(tab_zone_param[0]);
	Serial.println("Zone 2");
	Serial.println(tab_zone_param[1]);
	Serial.println("Zone 3");
	Serial.println(tab_zone_param[2]);
	Serial.println("Zone 4");
	Serial.println(tab_zone_param[3]);
	Serial.println("Zone max");
	Serial.println(tab_zone_param[4]);
	Serial.println("****Parametres charg�s****");
	Serial.println();
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
	

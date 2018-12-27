//test 1:2015.08.15
#include <car_bluetooth.h>
#include <SoftwareSerial.h>   //Software Serial Port
#include <String.h>
#include <MotorDriver.h>
#include <stdlib.h>

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
#define CMD_GETPARAM		'Z'
#define CMD_MANUELLE		'M'
#define SPEED_STEPS 20
uint8_t speed0 = 180;
void setup() {
	Serial.begin(9600);
	motordriver.init();
	motordriver.setSpeed(180, MOTORA);
	motordriver.setSpeed(180, MOTORB);
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
// Nombre de millisecondes entre deux changements d'état de la LED 
const unsigned long TEMPO_DIST = 200;
const unsigned long TEMPO_BT = 50;
const unsigned long TEMPO_MOVE = 100;
const unsigned long TEMPO_TURN = 500;
// Précédente valeur de millis()
unsigned long previousMillisBT = 0;

unsigned long previousMillisDIST = 0;
unsigned long previousMillisMove = 0;
unsigned long previousMillisTurn = 0;


void loop() {
	// Récupére la valeur actuelle de millis()
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
void getBluetoothMessage() {

		bt_command = bluetooth.readByte();
		if (bt_command != CMD_INVALID) {
			traitementMessage(bt_command);
		}
	
	
	if (bluetooth.getStatus() == PAIRABLE) {
		motordriver.stop();
		bluetooth.waitConnected();

	}

}
void autonome() {
	unsigned long currentMillis = millis();
		if (cmMsec > 150) {
			motordriver.goRight();
			

			while (currentMillis - previousMillisMove < TEMPO_MOVE) {
				currentMillis = millis();
				previousMillisMove = currentMillis;
			}


		}
		if (cmMsec <= 150 && cmMsec >= 50) {
			motordriver.goForward();
			while (currentMillis - previousMillisMove < TEMPO_MOVE) {
				currentMillis = millis();
				previousMillisMove = currentMillis;

			}


		}
		if (cmMsec <= 49 && cmMsec >= 30) {
			motordriver.goLeft();
			while (currentMillis - previousMillisTurn < TEMPO_TURN) {
				currentMillis = millis();
				previousMillisTurn = currentMillis;

			}

		}
		if (cmMsec <= 29 && cmMsec >= 1) {
			motordriver.goBackward();
			while (currentMillis - previousMillisTurn < TEMPO_TURN) {
				currentMillis = millis();
				previousMillisTurn = currentMillis;

			}
		}
}

void listingBT() {
	distance = String(cmMsec);
	String list = ("Distance : " + distance + "/" + "Température : " + "21.02" + "/" + "Température : " + "21.02" + "/" + "Température : " + "21.02" + "/" + "Température : " + "21.02" + "/" + "Température : " + "21.02");
	bluetooth.writeAT(list);
}
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
	case CMD_GETPARAM:
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
}

void scanFrontCenter() {
  long microsec = ultrasonic.timing();
  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
}

void rightForward() {
	motordriver.setSpeed(100, MOTORB);
}

void leftForward() {
	motordriver.setSpeed(100, MOTORA);
}

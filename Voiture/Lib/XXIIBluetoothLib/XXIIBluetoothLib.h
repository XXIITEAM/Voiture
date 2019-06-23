// XXIIBluetoothLib.h

#ifndef _XXIIBLUETOOTHLIB_h
#define _XXIIBLUETOOTHLIB_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
	//Etats bluetooth
#define INITIALIZING 0
#define READY        1
#define INQUIRING    2
#define PAIRABLE     3
#define CONNECTING   4
#define CONNECTED    5
	//Bluetooth

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
#define CMD_SAVE			'Q'
#define CMD_TEMP			'T'
#define SPEED_STEPS 20
#define CMD_DELIM			'/'

class XXIIBluetoothLibClass
{
 protected:
	 char commande_precedente = 'I';
	 char commande_recue;
	 char CMD_VIDE = NULL;
	 

 public:
	 
	 const unsigned long TEMPO_BT = 500;
	 unsigned long previousMillisBT = 0;
	 int8_t status;
	 String s_connecting;
	 String s_connected;
	void XXIIBluetoothLibClass::init();
	void XXIIBluetoothLibClass::waitPairable();
	void XXIIBluetoothLibClass::waitConnected();
	int8_t XXIIBluetoothLibClass::getStatus();
	void XXIIBluetoothLibClass::clearBuffer();
	bool XXIIBluetoothLibClass::writeAT(String cmdAT);
	bool XXIIBluetoothLibClass::testAT();
	bool XXIIBluetoothLibClass::writeMsg(String cmd);
	char XXIIBluetoothLibClass::readByte();
	void XXIIBluetoothLibClass::traitementOptions(char cmd);
};

extern XXIIBluetoothLibClass XXIIBluetoothLib;

#endif


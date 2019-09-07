// 
// 
// 

#include "XXIIBluetoothLib.h"

void XXIIBluetoothLibClass::init()
{


}

int tab_zone_param[5];
	// ********************************************************************************
	/// <summary>
	/// @fn void waitPairable()
	/// @brief Fonction attente BT.
	/// </summary>
	// ********************************************************************************
void XXIIBluetoothLibClass::waitPairable() {
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
void XXIIBluetoothLibClass::waitConnected() {
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

int8_t XXIIBluetoothLibClass::getStatus() {
	return status;
}

void XXIIBluetoothLibClass::clearBuffer() {
	char recvChar;
	while (Serial3.available())recvChar = Serial3.read();
}

// ********************************************************************************
/// <summary>
/// @fn bool writeAT(String cmdAT)
/// @brief Fonction commandes AT BT.
/// </summary>
/// <param name="cmdAT">Commande AT de test BT</param>
/// <returns>Trus/False</returns>
// ********************************************************************************
bool XXIIBluetoothLibClass::writeAT(String cmdAT) {
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
bool XXIIBluetoothLibClass::testAT() {
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
bool XXIIBluetoothLibClass::writeMsg(String cmd) {
	Serial3.println(cmd);
}


char XXIIBluetoothLibClass::readByte() {
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

// ********************************************************************************
	/// <summary>
	/// @fn void traitementOptions(char cmd)
	/// @brief Traitement du String parametres reçu de BTArduino
	/// </summary>
	/// <param name="cmd">Commande reçue de android</param>
	// ********************************************************************************
void XXIIBluetoothLibClass::traitementOptions(char cmd) {
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

XXIIBluetoothLibClass XXIIBluetoothLib;


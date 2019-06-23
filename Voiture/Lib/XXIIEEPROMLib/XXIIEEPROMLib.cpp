// 
// 
// 

#include "XXIIEEPROMLib.h"
void XXIIEEPROMLibClass::init()
{


}

//XXIIBluetoothLibClass bluetoothEEPROM;
// ********************************************************************************
	/// <summary>
	/// @fn void sauvegardeParametres()
	/// @brief Sauvegarde des parametres utilisateurs de tableau parametres vers EEPROM
	/// </summary>
	// ********************************************************************************
void XXIIEEPROMLibClass::sauvegardeParametres(unsigned long* magic, int* zone_1_min, int* zone_2_min, int* zone_3_min, int* zone_4_min, int* zone_4_max) {
	os_write.magic = STRUCT_MAGIC;
	os_write.zone_1_min = *zone_1_min;
	os_write.zone_2_min = *zone_2_min;
	os_write.zone_3_min = *zone_3_min;
	os_write.zone_4_min = *zone_4_min;
	os_write.zone_4_max = *zone_4_max;
	EEPROM.put(0, os_write);
	Serial3.println("Q");
}

// ********************************************************************************
/// <summary>
/// @fn void chargerParametres()
/// @brief Chargement des parametres utilisateurs de EEPROM vers tableau de parametres
/// </summary>
// ********************************************************************************
void XXIIEEPROMLibClass::chargerParametres(unsigned long* magic, int* zone_1_min, int* zone_2_min, int* zone_3_min, int* zone_4_min, int* zone_4_max) {
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
		//*magic = STRUCT_MAGIC;
	}
	// Mise � jour du tableau de param�tres
	getEEPROMParam(zone_1_min , zone_2_min, zone_3_min, zone_4_min, zone_4_max);
}

// ********************************************************************************
/// <summary>
/// @fn void updateTableauParam()
/// @brief Mise à jour du tableau de parametres a partir de EEPROM
/// </summary>
// ********************************************************************************
void XXIIEEPROMLibClass::getEEPROMParam(int* zone_1_min, int* zone_2_min, int* zone_3_min, int* zone_4_min, int* zone_4_max) {
	*zone_1_min = os_write.zone_1_min;
	*zone_2_min = os_write.zone_2_min;
	*zone_3_min = os_write.zone_3_min;
	*zone_4_min = os_write.zone_4_min;
	*zone_4_max = os_write.zone_4_max;
}
XXIIEEPROMLibClass XXIIEEPROMLib;
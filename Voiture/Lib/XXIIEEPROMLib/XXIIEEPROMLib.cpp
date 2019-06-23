// 
// 
// 

#include "XXIIEEPROMLib.h"

void XXIIEEPROMLibClass::init()
{


}


XXIIEEPROMLibClass XXIIEEPROMLib;

// ********************************************************************************
	/// <summary>
	/// @fn void sauvegardeParametres()
	/// @brief Sauvegarde des parametres utilisateurs de tableau parametres vers EEPROM
	/// </summary>
	// ********************************************************************************
void XXIIEEPROMLibClass::sauvegardeParametres(unsigned long *magic, int* zone_1_min, int* zone_2_min, int* zone_3_min, int* zone_4_min, int* zone_4_max) {
	os_write.magic = STRUCT_MAGIC;
	os_write.zone_1_min = *zone_1_min;
	os_write.zone_2_min = *zone_2_min;
	os_write.zone_3_min = *zone_3_min;
	os_write.zone_4_min = *zone_4_min;
	os_write.zone_4_max = *zone_4_max;
	EEPROM.put(0, os_write);
	writeMsg("Q");
}

// ********************************************************************************
/// <summary>
/// @fn void chargerParametres()
/// @brief Chargement des parametres utilisateurs de EEPROM vers tableau de parametres
/// </summary>
// ********************************************************************************
void XXIIEEPROMLibClass::chargerParametres(unsigned long *magic, int* zone_1_min, int* zone_2_min, int* zone_3_min, int* zone_4_min, int* zone_4_max) {
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
	//updateTableauParam();
}

// ********************************************************************************
/// <summary>
/// @fn void updateTableauParam()
/// @brief Mise à jour du tableau de parametres a partir de EEPROM
/// </summary>
// ********************************************************************************
/*void XXIIEEPROMLibClass::updateTableauParam() {
	Serial.println("update tab param");
	tab_zone_param[0] = os_write.zone_1_min;
	tab_zone_param[1] = os_write.zone_2_min;
	tab_zone_param[2] = os_write.zone_3_min;
	tab_zone_param[3] = os_write.zone_4_min;
	tab_zone_param[4] = os_write.zone_4_max;
}*/
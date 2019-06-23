// XXIIEEPROMLib.h

#ifndef _XXIIEEPROMLIB_h
#define _XXIIEEPROMLIB_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include <EEPROM.h>
	//D�finition STRUCT_MAGIC (test donn�es options)
static const unsigned long STRUCT_MAGIC = 22;
struct optionStruct {
	unsigned long magic;
	int zone_4_max;
	int zone_4_min;
	int zone_3_min;
	int zone_2_min;
	int zone_1_min;
};
class XXIIEEPROMLibClass
{
protected:


public:
	optionStruct os_write;
	// --------------------------------------------------------------------------------
	/// <summary>
	/// @struct optionStruct
	/// @brief Structure int.
	/// Stockage des param�tres de configuration des distances.
	/// R�cup�r� et mis � jour par BTArdroid
	/// </summary>
	// --------------------------------------------------------------------------------
	void XXIIEEPROMLibClass::init();
	void XXIIEEPROMLibClass::sauvegardeParametres(unsigned long* magic, int *zone_1_min, int *zone_2_min, int *zone_3_min, int *zone_4_min, int *zone_4_max);
	void XXIIEEPROMLibClass::chargerParametres(unsigned long* magic, int *zone_1_min, int *zone_2_min, int *zone_3_min, int *zone_4_min, int *zone_4_max);
	void XXIIEEPROMLibClass::getEEPROMParam(int* zone_1_min, int* zone_2_min, int* zone_3_min, int* zone_4_min, int* zone_4_max);

};

extern XXIIEEPROMLibClass XXIIEEPROMLib;

#endif


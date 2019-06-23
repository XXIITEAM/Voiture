// XXIIEEPROMLib.h

#ifndef _XXIIEEPROMLIB_h
#define _XXIIEEPROMLIB_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include <EEPROM.h>

class XXIIEEPROMLibClass
{
 protected:


 public:
	void init();
	void sauvegardeParametres(unsigned long *magic, int *zone_1_min, int *zone_2_min, int *zone_3_min, int *zone_4_min, int *zone_4_max);
	void chargerParametres(unsigned long *magic, int* zone_1_min, int* zone_2_min, int* zone_3_min, int* zone_4_min, int* zone_4_max);
	//void updateTableauParam();

};

extern XXIIEEPROMLibClass XXIIEEPROMLib;

#endif


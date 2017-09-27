// https://github.com/PaulStoffregen/IntelliScreen
// http://forum.pjrc.com/threads/26305-Highly-optimized-ILI9341-(320x240-TFT-color-display)-library

/***************************************************
  This is our library for the Adafruit  ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#ifndef _INTELLI_H_
#define _INTELLI_H_

#include "WProgram.h"

enum ACTIONS {
  NO_ACTION,
  BUTTON_DOWN,
  BUTTON_UP,
  BUTTON_PRESS,
  ACTIONS_NB
};

enum MODE {
  MODE_SD,
  MODE_GPS,
  MODE_CRS,
  MODE_PAR,
  MODE_HRM,
  MODE_HT,
  MODE_SIMU,
  MODE_MENU
};

class IntelliScreen
{
  public:
	IntelliScreen();
    virtual uint8_t getModeCalcul() {return _mode_calcul;}
	virtual uint8_t getModeAffi() {return _mode_affi;}
	void setModeCalcul(uint8_t mode) {_mode_calcul = mode;}
	void setModeAffi(uint8_t mode) {_mode_affi = mode;}
	void setModeAffiPrec(uint8_t mode) {_mode_affi_prec = mode;}
	void setStoredMode(uint8_t mode) {_stored_mode = mode;}
	uint8_t getStoredMode() {return _stored_mode;}
	uint8_t getSelectionMenu() {return _selectionMenu;}
	uint8_t getNbElemMenu() {return _nb_elem_menu;}
	uint8_t getBackLight() {return _BL_level;}
	uint8_t getPendingAction() {return _pendingAction;}
	const char *getMenuItem(uint8_t indice) {return _items[indice];}
	void addMenuItem(const char *item) {
		_items[_nb_elem_menu] = item;
		_nb_elem_menu++;
	}
	void machineEtat ();
	void menuDescend ();
	void menuMonte ();
	void menuClic ();
	void buttonUpEvent ();
	void buttonDownEvent ();
	void buttonPressEvent ();
	

 protected:
    uint8_t _mode_affi, _mode_affi_prec, _mode_calcul;
	uint8_t _stored_mode;
    uint8_t _selectionMenu, _pendingAction;
    uint8_t _nb_elem_menu;
	uint8_t _BL_level;
    
	const char *_items[10];
};


#endif

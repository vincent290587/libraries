
#include "IntelliScreen.h"
#include <WProgram.h>


IntelliScreen::IntelliScreen() {
	
  _mode_calcul = MODE_GPS;
  _mode_affi = MODE_SD;
  _BL_level = 0;
  _nb_elem_menu = 0;
  
  addMenuItem(" Retour");
  addMenuItem(" BackLight");
}


void IntelliScreen::machineEtat () {

  if (_pendingAction > NO_ACTION) {
    if (_mode_affi != MODE_MENU) {
      // mode course ou HT / HRM
      // -> on va au menu
	  _mode_affi_prec = _mode_affi;
	  _mode_affi = MODE_MENU;
    } else {
      // mode menu
      if (_pendingAction == BUTTON_DOWN) {
        menuDescend ();
      } else if (_pendingAction == BUTTON_UP) {
        menuMonte ();
      } else {
        menuClic ();
      }
    }
    _pendingAction = NO_ACTION;
  }
  
}

void IntelliScreen::menuDescend () {
  _selectionMenu++;
  _selectionMenu = _selectionMenu % _nb_elem_menu;
}

void IntelliScreen::menuMonte () {
  _selectionMenu+=_nb_elem_menu - 1;
  _selectionMenu = _selectionMenu % _nb_elem_menu;
}


void IntelliScreen::menuClic () {
  
  _selectionMenu+=_nb_elem_menu;
  _selectionMenu = _selectionMenu % _nb_elem_menu;
  
  if (_selectionMenu == 0) {
	  _mode_affi = _mode_affi_prec;
  } else if (_selectionMenu == 1) {
	  _BL_level != _BL_level;
  } else {
	  _mode_affi = _selectionMenu;
  }

}

void IntelliScreen::buttonDownEvent () {

  if (!_pendingAction) {
    _pendingAction = BUTTON_DOWN;
  }

}

void IntelliScreen::buttonUpEvent () {

  if (!_pendingAction) {
    _pendingAction = BUTTON_UP;
  }

}


void IntelliScreen::buttonPressEvent () {

  if (!_pendingAction) {
    _pendingAction = BUTTON_PRESS;
  }

}


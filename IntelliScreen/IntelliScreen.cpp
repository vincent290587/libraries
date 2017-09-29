
#include "IntelliScreen.h"
#include <WProgram.h>

#define NB_ELEM_MENU   menu.nb_elem


IntelliScreen* IntelliScreen::pIntelliScreen = nullptr;


static void callbackMENU(int entier) {
	// empty callback
}

IntelliScreen::IntelliScreen() {

	_mode_calcul = 0;
	_mode_affi = 0;
	_is_menu_active = 0;

	pIntelliScreen = this;

	memset(&menu, 0, sizeof(sIntelliMenu));

	// init all the menu items
	sIntelliMenuItem item;

	item.name = "Menu";
	item.p_func = callbackMENU;
	this->addMenuItem(&item);

}


uint8_t IntelliScreen::getModeAffi() {

	if (_is_menu_active) {
		return I_MODE_MENU;
	} else {
		return _mode_affi;
	}

}

void IntelliScreen::menuDescend () {
	if (_is_menu_active) {
		_selectionMenu++;
		_selectionMenu = _selectionMenu % NB_ELEM_MENU;
	}
}

void IntelliScreen::menuMonte () {
	if (_is_menu_active) {
		_selectionMenu+=NB_ELEM_MENU - 1;
		_selectionMenu = _selectionMenu % NB_ELEM_MENU;
	}
}


void IntelliScreen::menuClic () {

	if (_is_menu_active) {
		_is_menu_active = 0;

		_selectionMenu = _selectionMenu % NB_ELEM_MENU;

		// if item was clicked: call the function
		if (_selectionMenu > I_MODE_MENU) {
			// an item other than menu was clicked
			// -> call its handler
			(*menu.item[_selectionMenu].p_func)(_selectionMenu);
		}

	} else {
		// reset selected item
		//_selectionMenu = 0;
		// activate menu
		_is_menu_active = 1;
	}

}


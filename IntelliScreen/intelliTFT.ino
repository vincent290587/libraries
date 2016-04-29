

void setAffichage (int affi_) {
  affichageCourant = affi_;
}

int getAffichage () {
  return affichageCourant;
}

void machineEtat () {

  if (pendingAction>0) {
    if (affichageCourant - 20 >= 0 || affichageCourant <= 1) {
      // mode course ou indoor
      // -> on va au menu
      if (pendingAction==3) {
        // pression
        avantMenu = affichageCourant;
        affichageCourant = 10;
      } else if (affichageCourant - 20 >= 0) {
        // on bascule la vue de l'affichage
        cycleAffichageCourse ();
      }
      aRafraichir = 1;
    } else if (affichageCourant - 10 >= 0) {
      // mode menu
      if (pendingAction == 1) {
        menuDescend ();
      } else if (pendingAction == 2) {
        menuMonte ();
      } else {
        menuClic ();
      }
    }
    pendingAction = -1;
  }
  
}

void menuDescend () {
  selectionMenu++;
}

void menuMonte () {
  selectionMenu--;
}


void menuClic () {
  
  selectionMenu+=5;
  selectionMenu = selectionMenu % 5;
  
  switch (selectionMenu) {
    case 0:
      // retour
      affichageCourant = avantMenu;
      break;
    case 1:
      // mode course
      affichageCourant = 30;
      break;
    case 2:
      // mode indoor
      affichageCourant = 20;
      break;
    case 3:
      // mode indoor
      affichageCourant = 25;
      break;
    case 4:
      // mode BL
      BL_level += 20;
      BL_level = BL_level % 240;
      analogWrite(tft_bl, BL_level);
      break;
  }

}

void buttonDownEvent () {

  if (!pendingAction) {
    desactiverNavigateur();
    pendingAction = 1;
  }

}

void buttonUpEvent () {

  if (!pendingAction) {
    desactiverNavigateur();
    pendingAction = 2;
  }

}


void buttonPressEvent () {

  if (!pendingAction) {
    desactiverNavigateur();
    pendingAction = 3;
  }

}

void checkPendingPress() {
  if (pendingAction > 0) {
    desactiverNavigateur();
    
    delay(200);
    affichage();
    activerNavigateur();
  }
}

void setSDAvailable () {
  if (affichageCourant==0) {
    affichagePrecedent = 0;
    affichageCourant = 1;
  }
}

void setGPSAvailable () {
  if (affichageCourant==1) {
    affichagePrecedent = 1;
    affichageCourant = 30;
    clearScreen();
    initScreen(); 
    lignesNormal();
  }
}

void affichage () {

  // on met a jour affichageCourant avec les actions
  machineEtat ();

  if (affichagePrecedent != affichageCourant || aRafraichir) {
    // clear
    clearScreen();
    if (affichageCourant>=20 || affichagePrecedent<=1) {
      estScreenInit=0;
    }
  }
  
  //Serial.println(affichageCourant);

  switch (affichageCourant) {
    case 0:
      // SD
      //sd_lec_tft();
      //delay(200);
      break;
    case 1:
      // no GPS
      //activerNavigateur();
      //no_gps_tft();
      //boucle_outdoor ();
      //delay(500);
      break;
    case 10:
      //menu
      if (pendingAction < 0) {
        pendingAction=0;
      }
      boucleMenu();
      break;
    case 20:
      // indoor
      if (pendingAction < 0) {
        pendingAction=0;
      }
      boucle_indoor ();
      break;
    case 25:
      // HT
      if (pendingAction < 0) {
        pendingAction=0;
      }
      boucle_HT ();
      break;
    case 30:
    case 31:
    case 32:
      // course
      if (pendingAction < 0) {
        pendingAction=0;
      }
      boucle_outdoor ();
      break;
  }

  if (affichageCourant!=10) pendingAction = 0;
  affichagePrecedent = affichageCourant;
  aRafraichir = 0;
}

int isOutdoor () {
  
  if (affichageCourant >= 30 || (affichageCourant<=10 && avantMenu >= 30)) {
    return 1;
  }
  return 0;
}

int isIndoor () {
  
  if (affichageCourant == 20 || (affichageCourant<=10 && avantMenu == 20)) {
    return 1;
  }
  return 0;
}

int isHT () {
  
  if (affichageCourant == 25 || (affichageCourant<=10 && avantMenu == 25)) {
    return 1;
  }
  return 0;
}

void cycleAffichageCourse () {
  
  if (affichageCourant>=30) {
    affichageCourant = (affichageCourant-30+1) % 3 + 30;
  }
  
}

void affichageMenu () {

  selectionMenu+=5;
  selectionMenu = selectionMenu % 5;

  tft.setCursor(0, 50);
  tft.setTextSize(3);
  tft.setTextColor(CLR_FRGND, CLR_BCKGND);

  switch (selectionMenu) {
    case 0:
      // retour
      tft.setTextColor(CLR_FRGND, CLR_GREEN);
      tft.println(F(" Retour"));
      tft.setTextColor(CLR_FRGND, CLR_BCKGND);
      tft.println(F(" Mode course"));
      tft.println(F(" Mode indoor"));
      tft.println(F(" Mode HT"));
      tft.println(F(" Toggle BL"));
      break;
    case 1:
      // mode course
      tft.println(F(" Retour"));
      tft.setTextColor(CLR_FRGND, CLR_GREEN);
      tft.println(F(" Mode course"));
      tft.setTextColor(CLR_FRGND, CLR_BCKGND);
      tft.println(F(" Mode indoor"));
      tft.println(F(" Mode HT"));
      tft.println(F(" Toggle BL"));
      break;
    case 2:
      // mode indoor
      tft.println(F(" Retour"));
      tft.println(F(" Mode course"));
      tft.setTextColor(CLR_FRGND, CLR_GREEN);
      tft.println(F(" Mode indoor"));
      tft.setTextColor(CLR_FRGND, CLR_BCKGND);
      tft.println(F(" Mode HT"));
      tft.println(F(" Toggle BL"));
      break;
    case 3:
      // mode HT
      tft.println(F(" Retour"));
      tft.println(F(" Mode course"));
      tft.println(F(" Mode indoor"));
      tft.setTextColor(CLR_FRGND, CLR_GREEN);
      tft.println(F(" Mode HT"));
      tft.setTextColor(CLR_FRGND, CLR_BCKGND);
      tft.println(F(" Toggle BL"));
      break;
    case 4:
      // mode BL
      tft.println(F(" Retour"));
      tft.println(F(" Mode course"));
      tft.println(F(" Mode indoor"));
      tft.println(F(" Mode HT"));
      tft.setTextColor(CLR_FRGND, CLR_GREEN);
      tft.println(F(" Toggle BL"));
      tft.setTextColor(CLR_FRGND, CLR_BCKGND);
      break;
  }

}

void clearScreen() {
  tft.setCursor(0, 0);
  tft.fillScreen(CLR_BCKGND);
}



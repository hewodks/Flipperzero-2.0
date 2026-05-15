#include "joystick_module.h"

#define JOY_UP    12  // Pin conectado a "UP"
#define JOY_DOWN  33  // Pin conectado a "DWN" 
#define JOY_SW    32  // Pin conectado a "MID"

void setupJoystick() {
  pinMode(JOY_UP, INPUT_PULLUP);
  pinMode(JOY_DOWN, INPUT_PULLUP);
  pinMode(JOY_SW, INPUT_PULLUP);}

void actualizarJoystick(int &menuIndex, bool &enSubMenu, int numItems) {
  if (!enSubMenu) {
    // Si se presiona hacia ARRIBA (el pin se va a LOW)
    if (digitalRead(JOY_UP) == LOW) { 
      if (menuIndex > 0) menuIndex--;
      delay(200); // Antirrebote
    }
    
    // Si se presiona hacia ABAJO (el pin se va a LOW)
    if (digitalRead(JOY_DOWN) == LOW) { 
      if (menuIndex < numItems - 1) menuIndex++;
      delay(200); 
    }

    // Si se presiona CENTRO
    if (digitalRead(JOY_SW) == LOW) {
      enSubMenu = true;
      delay(300); 
    }
  } else {
    // Para salir del submenú
    if (digitalRead(JOY_SW) == LOW) {
      enSubMenu = false;
      delay(300); 
    }
  }
}


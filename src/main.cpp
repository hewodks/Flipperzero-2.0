#include <Arduino.h>
#include "interface.h"
#include "joystick_module.h"
#include "sd_module.h"
#include "wifi_module.h"
#include "ir_module.h"
#include "nfc_module.h"
#include "bt_module.h"
#include "conexion_api.h"

const int NUM_ITEMS = 6; // Subimos a 6 opciones
int indiceActual = 0;
bool dentroDeOpcion = false;

// Añadimos el Gestor SD al menú
String menuItems[NUM_ITEMS] = {"FID/NFC", "Infrarrojo IR", "Wifi", "Bluetooth", "Sub-GHz CC1101", "Gestor SD"};


void setup() {
  Serial.begin(115200); // Super importante para que el volcado USB funcione
  Wire.begin(25, 26);
  
  //setupSleep(); // NUEVO: 2. Inicializamos el pin del switch
  
  //setupLED(); 
  inicializarPantalla();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_tr);
  //u8g2.drawBox
  u8g2.drawStr(0, 20, "FHAAAAAAA");
  u8g2.sendBuffer();
  delay(500);
  setupSD(); 
  setupJoystick();
 
 
  setupNFC();
  setupIR(); 
  setupWiFi(); 
  setupBluetooth(); 
  //iniciarWiFi();
  
  //setupCC1101();
}

void loop() {
  //checkSleepMode(); // NUEVO: 3. El vigilante que revisa si activaste el switch para dormir

  //parpadearLED(500);
  actualizarJoystick(indiceActual, dentroDeOpcion, NUM_ITEMS);
  if (dentroDeOpcion && indiceActual == 0) flujoCapturaRFID(dentroDeOpcion);
  else if(dentroDeOpcion && indiceActual == 1) flujoInfrarrojo(dentroDeOpcion);
  else if(dentroDeOpcion && indiceActual == 2) flujoWiFi(dentroDeOpcion);
  else if(dentroDeOpcion && indiceActual == 3) flujoBluetooth(dentroDeOpcion);
  //else if(dentroDeOpcion && indiceActual == 4) flujoCC1101(dentroDeOpcion); 
  else if(dentroDeOpcion && indiceActual == 5) flujoSDManager(dentroDeOpcion); // NUEVO
  else dibujarPantalla(indiceActual, dentroDeOpcion, menuItems, NUM_ITEMS); 
}
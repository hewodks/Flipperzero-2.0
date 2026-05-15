#include <Arduino.h>
#include <interface.h>
#include "logos.h" // <--- IMPORTANTE: Incluir tus logos

// ... Tu definición de u8g2 se queda igual 
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 26, /* data=*/ 25);

void inicializarPantalla() {
  u8g2.begin();
}

void dibujarPantalla(int menuIndex, bool enSubMenu, String items[], int numItems){
  u8g2.clearBuffer();

  // --- 1. DIBUJAR LA CABECERA ---
  u8g2.setFont(u8g2_font_ncenB08_tr);
  if(!enSubMenu){
    u8g2.drawStr(15, 10, "-- FLIPPER HUB --");
  } else {
    u8g2.drawStr(25, 10, "-- SUBMENU --");
  }
  
  // --- NUEVO: DIBUJAR LOGO DINÁMICO EN LA ESQUINA ---
  // Detectamos qué hay en la opción actual para poner un dibujo
  String opcionActual = items[menuIndex];
  
  if (opcionActual.indexOf("TV") != -1) {
    u8g2.drawXBMP(110, 0, 16, 16, logo_samsung);    // TV si es algo de tele
  }

  u8g2.drawLine(0, 13, 128, 13);

  // --- 2. LÓGICA DE SCROLLING (Se queda igual) ---
  int maxOpcionesVisibles = 4; 
  int indiceInicio = 0;
  if (menuIndex >= maxOpcionesVisibles) {
    indiceInicio = menuIndex - maxOpcionesVisibles + 1;
  }

  // --- 3. DIBUJAR LAS OPCIONES (Con un pequeño ajuste de espacio) ---
  u8g2.setFont(u8g2_font_6x12_tr); 

  for (int i = 0; i < maxOpcionesVisibles; i++) {
    int itemReal = indiceInicio + i;
    if (itemReal >= numItems) break; 

    int yPos = 25 + (i * 12); 

    if (itemReal == menuIndex) {
      u8g2.drawStr(0, yPos, ">"); 
      // Opcional: Invertir el color de la línea seleccionada para que se vea más pro
      u8g2.drawBox(10, yPos-9, 100, 11); 
      u8g2.setDrawColor(0); // Texto en negro sobre cuadro blanco
    } else {
      u8g2.setDrawColor(1); // Texto normal
    }
    
    u8g2.setCursor(12, yPos);
    u8g2.print(items[itemReal]);
    u8g2.setDrawColor(1); // Resetear color por si acaso
  }

  // --- 4. INDICADORES (Se quedan igual) ---
  if (indiceInicio > 0) u8g2.drawStr(120, 22, "^"); 
  if (indiceInicio + maxOpcionesVisibles < numItems) u8g2.drawStr(120, 62, "v"); 

  u8g2.sendBuffer();
}
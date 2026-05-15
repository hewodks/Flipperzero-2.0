#include "ir_module.h"
#include "sd_module.h" 
#include "interface.h"
#include "joystick_module.h"
#include "joystick_module.h"

#include <ir_Gree.h>    
#include <ir_Midea.h>   
#include <ir_LG.h>      
#include <ir_Samsung.h> 
#include <ir_Daikin.h>
#include <ir_Mitsubishi.h>
#include <ir_Toshiba.h>



IRrecv irrecv(IR_RX_PIN);
IRsend irsend(IR_TX_PIN);
decode_results results;

decode_type_t tipoProtocolo = UNKNOWN;

#define JOY_DOWN  33
#define JOY_SW 32
#define JOY_UP 12

// Variable global para mantener la temperatura en la memoria
int tempActualAC = 22;

void modoClonarIR(bool &dentroDeOpcion);
void ataqueTVUniversal();
void menuTVMarcas(bool &dentroDeOpcion); 
void menuAccionTV(int marcaID);          
void ejecutarAtaqueTV(int marcaID, int accionID); 
void menuACMarcas(bool &dentroDeOpcion); 
void menuAccionAC(int marcaID);          
void ejecutarAtaqueAC(int marcaID, int accionAC);
void  cargarYEnviarGuardado();
void mostrarOpcionesIR(String codigoHex, uint32_t codigoCrudo, bool &dentroDeOpcion);
bool esperarO_Salir(int tiempoMs); 
void enviarCodigoIR(uint32_t codigo); 

void setupIR() { pinMode(IR_RX_PIN, INPUT_PULLUP); irrecv.enableIRIn(); irsend.begin(); }

bool esperarO_Salir(int tiempoMs) {
    unsigned long inicio = millis();
    while (millis() - inicio < (unsigned long)tiempoMs) {
        if (digitalRead(JOY_SW) == LOW) { delay(200); return true; }
        delay(5);
    }
    return false; 
}

void flujoInfrarrojo(bool &dentroDeOpcion) {
    bool enMenuIR = true; int subIndice = 0; const int SUB_ITEMS = 6; 
    String subMenu[SUB_ITEMS] = {"1. Clonar/Sniffer", "2. Emular Guardados", "3. TV Rafaga", "4. TV x Marcas", "5. AC x Marcas", "6. Regresar"};

    while (enMenuIR) {
        dibujarPantalla(subIndice, false, subMenu, SUB_ITEMS);
            
        // NAVEGACIÓN DIGITAL
        if (digitalRead(JOY_UP) == LOW) { if (subIndice > 0) subIndice--; delay(200); }
        if (digitalRead(JOY_DOWN) == LOW) { if (subIndice < SUB_ITEMS - 1) subIndice++; delay(200); }

        if (digitalRead(JOY_SW) == LOW) {
            delay(200); 
            if (subIndice == 0) { bool enClonador = true; modoClonarIR(enClonador); } 
            else if (subIndice == 1) cargarYEnviarGuardado();
            else if (subIndice == 2) ataqueTVUniversal();
            else if (subIndice == 3) { bool enMarcasTV = true; menuTVMarcas(enMarcasTV); }
            else if (subIndice == 4) { bool enMarcasAC = true; menuACMarcas(enMarcasAC); }
            else if (subIndice == 5) enMenuIR = false; 
        }
    }
    dentroDeOpcion = false;
}

void menuTVMarcas(bool &dentroDeOpcion) {
    bool enMenuMarcas = true; int indiceMarca = 0; const int NUM_MARCAS = 8;
    String listaMarcas[NUM_MARCAS] = {"1. LG", "2. Samsung", "3. Sony", "4. Panasonic", "5. Hisense", "6. TCL", "7. Vizio", "8. Regresar"};
    while(digitalRead(JOY_SW) == LOW) delay(10); 

    while (enMenuMarcas) {
        dibujarPantalla(indiceMarca, true, listaMarcas, NUM_MARCAS);
        
        if (digitalRead(JOY_UP) == LOW) { if (indiceMarca > 0) indiceMarca--; delay(180); }
        if (digitalRead(JOY_DOWN) == LOW) { if (indiceMarca < NUM_MARCAS - 1) indiceMarca++; delay(180); }
        
        if (digitalRead(JOY_SW) == LOW) {
            delay(200);
            if (indiceMarca == 7) enMenuMarcas = false; 
            else menuAccionTV(indiceMarca); 
        }
    }
}

void menuAccionTV(int marcaID) {
    bool enAccion = true; int indiceAccion = 0; const int NUM_ACCIONES = 5;
    String listaAcciones[NUM_ACCIONES] = {"1. Power", "2. Vol +", "3. Vol -", "4. Mute", "5. Regresar"};
    while(digitalRead(JOY_SW) == LOW) delay(10); 
    while (enAccion) {
        dibujarPantalla(indiceAccion, true, listaAcciones, NUM_ACCIONES);

        if (digitalRead(JOY_UP) == LOW) { if (indiceAccion > 0) indiceAccion--; delay(180); }
        if (digitalRead(JOY_DOWN) == LOW) { if (indiceAccion < NUM_ACCIONES - 1) indiceAccion++; delay(180); }
        
        if (digitalRead(JOY_SW) == LOW) {
            delay(200);
            if (indiceAccion == 4) enAccion = false;
            else ejecutarAtaqueTV(marcaID, indiceAccion); 
        }
    }
}

void ejecutarAtaqueTV(int marcaID, int accionID) {
    u8g2.clearBuffer(); u8g2.setFont(u8g2_font_ncenB08_tr); u8g2.drawStr(0, 15, "DISPARANDO..."); u8g2.sendBuffer();
    for(int i = 0; i < 3; i++) {
        if (marcaID == 0) { 
            if (accionID == 0) irsend.sendNEC(0x20DF10EF, 32); else if (accionID == 1) irsend.sendNEC(0x20DF40BF, 32); 
            else if (accionID == 2) irsend.sendNEC(0x20DFC03F, 32); else if (accionID == 3) irsend.sendNEC(0x20DF906F, 32); 
        }
        else if (marcaID == 1) { 
            if (accionID == 0) irsend.sendSAMSUNG(0xE0E040BF, 32); else if (accionID == 1) irsend.sendSAMSUNG(0xE0E0E01F, 32); 
            else if (accionID == 2) irsend.sendSAMSUNG(0xE0E0D02F, 32); else if (accionID == 3) irsend.sendSAMSUNG(0xE0E0F00F, 32); 
        }
        else if (marcaID == 2) { 
            if (accionID == 0) irsend.sendSony(0xA90, 12, 2); else if (accionID == 1) irsend.sendSony(0x490, 12, 2); 
            else if (accionID == 2) irsend.sendSony(0xC90, 12, 2); else if (accionID == 3) irsend.sendSony(0x290, 12, 2); 
        }
        else if (marcaID == 3) { 
            if (accionID == 0) irsend.sendPanasonic(0x4004, 0x100BCBD); else if (accionID == 1) irsend.sendPanasonic(0x4004, 0x1000405); 
            else if (accionID == 2) irsend.sendPanasonic(0x4004, 0x1008485); else if (accionID == 3) irsend.sendPanasonic(0x4004, 0x1004C4D); 
        }
        else if (marcaID == 4) { 
            if (accionID == 0) irsend.sendNEC(0x4FB4AB5, 32); else if (accionID == 1) irsend.sendNEC(0x4FB0AF5, 32); 
            else if (accionID == 2) irsend.sendNEC(0x4FB8A75, 32); else if (accionID == 3) irsend.sendNEC(0x4FBCA35, 32); 
        }
        else if (marcaID == 5) { 
            if (accionID == 0) irsend.sendNEC(0x00E41BA4, 32); else if (accionID == 1) irsend.sendNEC(0x00E45AA5, 32); 
            else if (accionID == 2) irsend.sendNEC(0x00E40BF4, 32); else if (accionID == 3) irsend.sendNEC(0x00E4639C, 32); 
        }
        else if (marcaID == 6) { 
            if (accionID == 0) irsend.sendNEC(0x20DF10EF, 32); else if (accionID == 1) irsend.sendNEC(0x20DF40BF, 32); 
            else if (accionID == 2) irsend.sendNEC(0x20DFC03F, 32); else if (accionID == 3) irsend.sendNEC(0x20DF906F, 32); 
        }
        delay(40);
    }
}

void menuACMarcas(bool &dentroDeOpcion) {
    bool enMenuMarcas = true; int indiceMarca = 0; const int NUM_MARCAS = 8;
    String listaMarcas[NUM_MARCAS] = {"1. Trane/Gree", "2. Mirage/Midea", "3. LG", "4. Samsung", "5. Daikin", "6. Mitsubishi", "7. Toshiba", "8. Regresar"};
    while(digitalRead(JOY_SW) == LOW) delay(10); 
    while (enMenuMarcas) {
        dibujarPantalla(indiceMarca, true, listaMarcas, NUM_MARCAS);
  
        if (digitalRead(JOY_UP) == LOW) { if (indiceMarca > 0) indiceMarca--; delay(180); }
        if (digitalRead(JOY_DOWN) == LOW) { if (indiceMarca < NUM_MARCAS - 1) indiceMarca++; delay(180); }
  
        if (digitalRead(JOY_SW) == LOW) { delay(200); if (indiceMarca == 7) enMenuMarcas = false; else menuAccionAC(indiceMarca); }
    }
}

void menuAccionAC(int marcaID) {
    bool enAccion = true; int indiceAccion = 0; const int NUM_ACCIONES = 5;
    
    while(digitalRead(JOY_SW) == LOW) delay(10); 
    while (enAccion) {
        String listaAcciones[NUM_ACCIONES] = {
            "1. Encender", 
            "2. Apagar", 
            "3. Temp + (" + String(tempActualAC) + "C)", 
            "4. Temp - (" + String(tempActualAC) + "C)", 
            "5. Regresar"
        };
        
        dibujarPantalla(indiceAccion, true, listaAcciones, NUM_ACCIONES);
        
        if (digitalRead(JOY_UP) == LOW) { if (indiceAccion > 0) indiceAccion--; delay(180); }
        if (digitalRead(JOY_DOWN) == LOW) { if (indiceAccion < NUM_ACCIONES - 1) indiceAccion++; delay(180); }

        if (digitalRead(JOY_SW) == LOW) { 
            delay(200); 
            if (indiceAccion == 4) enAccion = false; 
            else ejecutarAtaqueAC(marcaID, indiceAccion); 
        }
    }
}

void ejecutarAtaqueAC(int marcaID, int accionAC) {
    bool encender = true;

    if (accionAC == 1) {
        encender = false; 
    } else if (accionAC == 2) {
        if (tempActualAC < 30) tempActualAC++; 
    } else if (accionAC == 3) {
        if (tempActualAC > 16) tempActualAC--; 
    }

    u8g2.clearBuffer(); 
    u8g2.setFont(u8g2_font_ncenB08_tr); 
    
    if (encender) {
        u8g2.drawStr(0, 15, "ENCENDIENDO..."); 
        u8g2.setCursor(0, 35);
        u8g2.print("TEMP: " + String(tempActualAC) + " C");
    } else {
        u8g2.drawStr(0, 15, "APAGANDO...");
    }
    u8g2.sendBuffer();

    if (marcaID == 0) { 
        IRGreeAC ac(IR_TX_PIN); ac.begin();
        if(encender) { ac.on(); ac.setTemp(tempActualAC); ac.setMode(kGreeCool); ac.setFan(kGreeFanMax); } else ac.off();
        ac.send();
    } 
    else if (marcaID == 1) { 
        IRMideaAC ac(IR_TX_PIN); ac.begin();
        if(encender) { ac.on(); ac.setTemp(tempActualAC); ac.setMode(kMideaACCool); ac.setFan(kMideaACFanHigh); } else ac.off();
        ac.send();
    } 
    else if (marcaID == 2) { 
        IRLgAc ac(IR_TX_PIN); ac.begin();
        if(encender) { ac.on(); ac.setTemp(tempActualAC); ac.setMode(kLgAcCool); ac.setFan(kLgAcFanHigh); } else ac.off();
        ac.send();
    } 
    else if (marcaID == 3) { 
        IRSamsungAc ac(IR_TX_PIN); ac.begin();
        if(encender) { ac.on(); ac.setTemp(tempActualAC); ac.setMode(kSamsungAcCool); ac.setFan(kSamsungAcFanHigh); } else ac.off();
        ac.send();
    }
    else if (marcaID == 4) { 
        IRDaikinESP ac(IR_TX_PIN); ac.begin();
        if(encender) { ac.on(); ac.setTemp(tempActualAC); ac.setMode(kDaikinCool); ac.setFan(kDaikinFanAuto); } else ac.off();
        ac.send();
    }
    else if (marcaID == 5) { 
        IRMitsubishiAC ac(IR_TX_PIN); ac.begin();
        if(encender) { ac.on(); ac.setTemp(tempActualAC); ac.setMode(kMitsubishiAcCool); ac.setFan(kMitsubishiAcFanMax); } else ac.off();
        ac.send();
    }
    else if (marcaID == 6) { 
        IRToshibaAC ac(IR_TX_PIN); ac.begin();
        if(encender) { ac.on(); ac.setTemp(tempActualAC); ac.setMode(kToshibaAcCool); ac.setFan(kToshibaAcFanAuto); } else ac.off();
        ac.send();
    }
    delay(1000);
}

bool recibirCodigoIR(String &resultadoHex, uint32_t &codigoCrudo) {
    if (irrecv.decode(&results)) {
        resultadoHex = resultToHexidecimal(&results);
        codigoCrudo = (uint32_t)results.value;
        tipoProtocolo = results.decode_type;
        irrecv.resume(); 
        return true;
    }
    return false;
}

//by hk
void enviarCodigoIR(uint32_t codigo) {
    irrecv.disableIRIn(); // Apagamos receptor para no causar ruido
    delay(50);

    // En lugar de irsend.sendNEC, usamos send() que detecta el protocolo
    // Si el protocolo es desconocido, por defecto intentamos NEC
    if (tipoProtocolo != UNKNOWN) {
        irsend.send(tipoProtocolo, codigo, 32); 
    } else {
        irsend.sendNEC(codigo, 32); 
    }

    delay(50);
    irrecv.enableIRIn(); // Reencendemos el receptor
}


void mostrarOpcionesIR(String codigoHex, uint32_t codigoCrudo, bool &dentroDeOpcion) {
    int indexOpcion = 0; String opcionesIR[3] = {"1. Replicar", "2. Guardar", "3. Regresar"}; bool enOpcionesIR = true;

    while (enOpcionesIR) {
        u8g2.clearBuffer(); u8g2.setFont(u8g2_font_6x12_tr); u8g2.setCursor(0, 10); u8g2.print("Hex: " + codigoHex);
        for (int i = 0; i < 3; i++) {
            int yPos = 30 + (i * 12);
            if (i == indexOpcion) u8g2.drawStr(0, yPos, ">"); 
            u8g2.setCursor(10, yPos); u8g2.print(opcionesIR[i]);
        }
        u8g2.sendBuffer();

        if (digitalRead(JOY_UP) == LOW) { if (indexOpcion > 0) indexOpcion--; delay(200); }
        if (digitalRead(JOY_DOWN) == LOW) { if (indexOpcion < 2) indexOpcion++; delay(200); }
        
        if (digitalRead(JOY_SW) == LOW) {
            delay(300);
            if (indexOpcion == 0){
                enviarCodigoIR(codigoCrudo);
                u8g2.drawStr(80, 30, "!!!"); 
                u8g2.sendBuffer();
                
                enviarCodigoIR(codigoCrudo); 
                
                delay(500); // Pausa para ver el feedback
            }

            else if (indexOpcion == 1) {
                String nombreUser = tecladoNombre("IR");
                guardarDato("IR", nombreUser, "Hex: " + codigoHex);
                u8g2.clearBuffer(); u8g2.drawStr(10, 30, "Guardado!"); u8g2.sendBuffer(); delay(1000);
            }

            else if (indexOpcion == 2) enOpcionesIR = false;

            unsigned long timeout = millis();
            while(digitalRead(JOY_SW) == LOW && (millis() - timeout < 1000));
        }
    }
}

void modoClonarIR(bool &dentroDeOpcion) {
    String codigoHex = ""; uint32_t codigoCrudo = 0;
    while (true) {
        u8g2.clearBuffer(); u8g2.setFont(u8g2_font_6x12_tr); u8g2.drawStr(10, 20, "Esperando IR..."); u8g2.sendBuffer();
        if (recibirCodigoIR(codigoHex, codigoCrudo)) { mostrarOpcionesIR(codigoHex, codigoCrudo, dentroDeOpcion); break; }
        if (digitalRead(JOY_SW) == LOW) { delay(200); break; }
    }
}

//||BY:hk
//||      Cargar codigos de SD para el disparo
//||
// Función para cargar un código guardado y dispararlo
void cargarYEnviarGuardado() {
    String archivo = seleccionarArchivoSD("IR");
    if (archivo == "") return; // El usuario canceló o no hay archivos

    File f = SD.open("/IR/" + archivo);
    if (f) {
        String hexStr = "";
        
        // Leemos el archivo línea por línea
        while (f.available()) {
            String linea = f.readStringUntil('\n');
            linea.trim(); // Limpiamos espacios o saltos de línea extra
            
            // Buscamos la línea que contiene el dato real
            if (linea.startsWith("Dato: Hex: ")) {
                // Extraemos lo que está después de "Dato: Hex: "
                // El prefijo tiene 11 caracteres
                hexStr = linea.substring(11); 
                break; // Ya encontramos lo que buscábamos, salimos del bucle
            }
        }
        f.close();

        if (hexStr != "") {
            // Convertimos el String (ej: "0xE0E040BF") a un número real
            uint32_t codigoVal = strtoul(hexStr.c_str(), NULL, 16);

            // Feedback visual
            u8g2.clearBuffer();
            u8g2.setFont(u8g2_font_6x12_tr);
            u8g2.drawStr(0, 15, "ENVIANDO...");
            u8g2.setCursor(0, 35);
            u8g2.print(archivo);
            u8g2.setCursor(0, 50);
            u8g2.print("ID: " + hexStr);
            u8g2.sendBuffer();

            // Disparamos el LED IR
            enviarCodigoIR(codigoVal);
            delay(1500);
        } else {
            // Si el archivo está corrupto o no tiene el formato
            u8g2.clearBuffer();
            u8g2.drawStr(10, 30, "Error de formato");
            u8g2.sendBuffer();
            delay(1000);
        }
    }
}


void ataqueTVUniversal() {
    u8g2.clearBuffer(); u8g2.setFont(u8g2_font_ncenB08_tr); u8g2.drawStr(0, 15, "TV RÁFAGA"); u8g2.sendBuffer();
    while(digitalRead(JOY_SW) == LOW) delay(10);
    bool atacando = true;
    while (atacando) {
        irsend.sendNEC(0x20DF10EF, 32); if(esperarO_Salir(50)) break; 
        irsend.sendSAMSUNG(0xE0E040BF, 32); if(esperarO_Salir(50)) break; 
        irsend.sendSony(0xA90, 12, 2); if(esperarO_Salir(50)) break; 
        irsend.sendPanasonic(0x4004, 0x100BCBD); if(esperarO_Salir(50)) break; 
        irsend.sendNEC(0x4FB4AB5, 32); if(esperarO_Salir(50)) break; 
        irsend.sendNEC(0x00E41BA4, 32); if(esperarO_Salir(50)) break; 
        irsend.sendRC5(0x0C, 12); if(esperarO_Salir(50)) break; 
        if(esperarO_Salir(500)) break; 
    }
}
#include "sd_module.h"
#include "interface.h"
#include "joystick_module.h"
#include "wifi_module.h"
#include <U8g2lib.h> 

// Nuevas librerías para el servidor
#include <WiFi.h>
#include <SimpleFTPServer.h>

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2; 

#define SPI_SCK  18
#define SPI_MISO 19
#define SPI_MOSI 23

#define JOY_UP    12  // Pin conectado a "UP"
#define JOY_DOWN  33  // Pin conectado a "DWN" 
#define JOY_SW    32  // Pin conectado a "MID"

// Instancia global del servidor FTP
FtpServer ftpSrv;

bool setupSD() {
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SD_CS); 
    if (!SD.begin(SD_CS)) return false;

    // Crear carpetas organizadas by:hk
    if (!SD.exists("/NFC")) SD.mkdir("/NFC");
    if (!SD.exists("/IR")) SD.mkdir("/IR");
    if (!SD.exists("/CC1101")) SD.mkdir("/CC1101");
    if (!SD.exists("/WIFI")) SD.mkdir("/WIFI");
    if (!SD.exists("/BLUT")) SD.mkdir("/BLUT");

    return true;
}


//by:hk - AM
void guardarDato(String categoria, String nombre, String contenido) {
    String path = "/" + categoria + "/" + nombre + ".txt";
    File file = SD.open(path, FILE_WRITE); 
    
    if (!SD.begin(SD_CS)) {
        Serial.println("SD perdió conexión!");
        return; 
    }
    
    if (!SD.exists("/" + categoria)) SD.mkdir("/" + categoria);
    
    File archivo = SD.open("/" + categoria + "/" + nombre + ".txt", FILE_APPEND);
    if (archivo) {
        archivo.println(contenido);
        archivo.close();
        Serial.println("¡Escritura exitosa!");
        }
    
    if (file) {
        file.println("--- LOG DE AUDITORIA ---");
        file.println("Timestamp: " + String(millis())); // Ayuda a ordenar en el Dashboard
        file.println("Tipo: " + categoria);
        file.println("Dato: " + contenido);
        file.close();
    }
}

// Función exclusiva para que el EvilTwin guarde rápido sin pedir nombre by:hk
void guardarCredencialesWiFi(String datos) {
    File file = SD.open("/WIFI/logs_eviltwin.txt", FILE_APPEND);
    if (file) {
        file.println(datos);
        file.close();
        Serial.println("Credencial guardada en SD!");
    } else {
        Serial.println("Error: No se pudo abrir logs_eviltwin.txt");
    }
}

void volcarSDALaptop() {
    String carpetas[] = {"/NFC", "/IR", "/CC1101", "/WIFI"};
    Serial.println("\n>>> INICIANDO VOLCADO SD <<<");
    
    for (int i=0; i<4; i++) {
        File root = SD.open(carpetas[i]);
        if(!root) continue;
        File file = root.openNextFile();
        while (file) {
            Serial.println("\n--- ARCHIVO: " + carpetas[i] + "/" + String(file.name()) + " ---");
            while (file.available()) Serial.write(file.read());
            file = root.openNextFile();
        }
    }
    Serial.println("\n>>> VOLCADO FINALIZADO <<<");
}

String tecladoNombre(String categoria) {
    String nombre = "";
    const char caracteres[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
    int idx = 0;
    bool listo = false;

    while(digitalRead(32) == LOW) delay(10); // Anti-rebote

    while (!listo) {
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_6x12_tr);
        u8g2.drawStr(0, 12, ("Nombre " + categoria + ":").c_str());
        u8g2.setCursor(0, 35);
        u8g2.print(nombre + caracteres[idx] + "_");
        u8g2.drawStr(0, 60, "Click:Add  Hold:OK");
        u8g2.sendBuffer();

        if (digitalRead(JOY_UP) == LOW) { 
            idx = (idx - 1 + 37) % 37; 
            delay(150); 
        }
        if (digitalRead(JOY_DOWN) == LOW) { 
            idx = (idx + 1) % 37; 
            delay(150); 
        }

        if (digitalRead(JOY_SW) == LOW) {
            unsigned long t = millis();
            while (digitalRead(JOY_SW) == LOW); 
            if (millis() - t > 800) listo = true; // Hold para finalizar
            else nombre += caracteres[idx];      // Click para añadir letra
        }
    }
    return nombre;
}

//||BY:hk                                                                    ||
//||     Listado de codigos/archivos de modulos para ejecucion posterior     ||
//||                                                                         ||
// Función para listar archivos y devolver el nombre del seleccionado
String seleccionarArchivoSD(String carpeta) {
    File root = SD.open("/" + carpeta);
    if (!root || !root.isDirectory()) return "";

    String archivos[10]; // Ajustar según memoria, máximo 10 archivos por carpeta
    int contador = 0;
    
    File file = root.openNextFile();
    while (file && contador < 10) {
        if (!file.isDirectory()) {
            archivos[contador] = String(file.name());
            contador++;
        }
        file = root.openNextFile();
    }
    root.close();

    if (contador == 0) return "";

    int sel = 0;
    while (true) {
        dibujarPantalla(sel, true, archivos, contador);
        if (digitalRead(JOY_UP) == LOW) { if (sel > 0) sel--; delay(200); }
        if (digitalRead(JOY_DOWN) == LOW) { if (sel < contador - 1) sel++; delay(200); }
        if (digitalRead(JOY_SW) == LOW) {
            delay(250);
            return archivos[sel];
        }
    }
}

void guardarLogSniffer(String mac, String ssid) {
    // Abre (o crea) un archivo exclusivo para el Sniffer de Probes
    File file = SD.open("/WIFI/sniffer_probes.txt", FILE_APPEND);
    if (file) {
        file.print("["); file.print(millis() / 1000); file.print("s] ");
        file.print("MAC: "); file.print(mac);
        file.print(" -> Buscando: "); file.println(ssid);
        file.close();
    }
}

//
///    BY:HK
//
void guardarLogBLE(String mac, int rssi, String nombre) {
    // Abre o crea el archivo exclusivo de logs de Bluetooth
    File file = SD.open("/BLUT/sniffer_ble.txt", FILE_APPEND);
    if (file) {
        file.print("["); file.print(millis() / 1000); file.print("s] ");
        file.print("MAC: "); file.print(mac);
        file.print(" | RSSI: "); file.print(rssi);
        file.print(" dBm | Nombre: "); file.println(nombre);
        file.close();
    }
}

// === EL MENÚ ACTUALIZADO ===
void flujoSDManager(bool &dentroDeOpcion) {
    int subIndice = 0;
    const int ITEMS = 4;
    // Añadimos el FTP como la primera opción
    String menuSD[ITEMS] = {"1. Servidor FTP", "2. Volcar USB", "3. Borrar SD", "4. Regresar"};

    while(digitalRead(32) == LOW) delay(10);

    while (dentroDeOpcion) {
        dibujarPantalla(subIndice, true, menuSD, ITEMS);

        if (digitalRead(JOY_UP) == LOW) { 
            if (subIndice > 0) subIndice--; 
            delay(200); 
        }
        if (digitalRead(JOY_DOWN) == LOW) { 
            if (subIndice < ITEMS - 1) subIndice++; 
            delay(200); 
        }

        if (digitalRead(JOY_SW) == LOW) {
            delay(250);
            
            // --- OPCIÓN 1: SERVIDOR FTP INALÁMBRICO ---
            if (subIndice == 0) {
                u8g2.clearBuffer();
                u8g2.drawStr(10, 30, "Iniciando WiFi...");
                u8g2.sendBuffer();

                WiFi.mode(WIFI_AP);
                WiFi.softAP("FLIPPER_DIY", "flipper123");
                IPAddress IP = WiFi.softAPIP();

                ftpSrv.begin("admin", "admin");

                u8g2.clearBuffer();
                u8g2.drawStr(0, 10, "== GESTOR FTP ==");
                u8g2.setCursor(0, 25); u8g2.print("Red : FLIPPER_DIY");
                u8g2.setCursor(0, 35); u8g2.print("Pass: flipper123");
                u8g2.setCursor(0, 45); u8g2.print("IP  : " + IP.toString());
                u8g2.drawStr(0, 60, "(Click para salir)");
                u8g2.sendBuffer();

                // El servidor se queda vivo hasta que presiones el botón
                while (digitalRead(32) == HIGH) {
                    ftpSrv.handleFTP();
                    delay(10);
                }

                // Rutina de apagado seguro al salir
                u8g2.clearBuffer();
                u8g2.drawStr(10, 30, "Apagando WiFi...");
                u8g2.sendBuffer();
                
                WiFi.softAPdisconnect(true);
                WiFi.mode(WIFI_OFF);
                delay(500);
            } 
            // --- OPCIÓN 2: VOLCADO POR CABLE USB ---
            else if (subIndice == 1) {
                u8g2.clearBuffer(); u8g2.drawStr(0, 30, "Enviando x USB..."); u8g2.sendBuffer();
                volcarSDALaptop();
                delay(2000);
            } 
            // --- OPCIÓN 3: BORRAR TARJETA ---
            else if (subIndice == 2) {
                u8g2.clearBuffer(); u8g2.drawStr(0, 30, "Borrando..."); u8g2.sendBuffer();
                SD.rmdir("/NFC"); SD.rmdir("/IR"); SD.rmdir("/CC1101"); SD.rmdir("/WIFI");
                setupSD(); 
                delay(1000);
            } 
            // --- OPCIÓN 4: SALIR ---
            else if (subIndice == 3) {
                dentroDeOpcion = false;
            }
        }
    }
}
#ifndef SD_MODULE_H
#define SD_MODULE_H

#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>

#define SD_CS 15 

bool setupSD();

// --- LÓGICA DE GUARDADO CON CARPETAS ---
void guardarDato(String categoria, String nombre, String contenido);
void guardarCredencialesWiFi(String datos); 
void guardarLogBLE(String mac, int rssi, String nombre);

// --- DASHBOARD Y GESTIÓN ---
void flujoSDManager(bool &dentroDeOpcion);
void volcarSDALaptop();
String tecladoNombre(String categoria);
String seleccionarArchivoSD(String carpeta);
#endif
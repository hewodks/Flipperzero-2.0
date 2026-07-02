#ifndef CONEXION_API
#define CONEXION_API

#include <WiFi.h>
#include <HTTPClient.h>

void iniciarWiFi();
void enviarLogAPI(String modulo, String funcion, unsigned long duracion);

#endif
#include <WiFi.h>
#include <HTTPClient.h>
#include "conexion_api.h"

// --- Configuración de Red ---
const char* ssid = "CUGDL_ALUMNOS_L";
const char* password = "";

// --- Configuración de la API ---
// REEMPLAZA con la IP de tu laptop. Mantén el puerto 8000 y la ruta /api/logs
const char* server_url = "http://10.216.12.255:8000/api/logs"; 

void iniciarWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n¡Conectado exitosamente al WiFi!");
}

// Función para enviar los logs a tu API de Python
void enviarLogAPI(String modulo, String funcion, unsigned long duracion) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // Configurar la petición HTTP POST
    http.begin(server_url);
    http.addHeader("Content-Type", "application/json");
    
    // Construimos el string JSON manualmente de forma ligera para el ESP32
    // Formato esperado: {"modulo": "X", "funcion": "Y", "duracion_ms": Z}
    String jsonPayload = "{\"modulo\":\"" + modulo + "\",\"funcion\":\"" + funcion + "\",\"duracion_ms\":" + String(duracion) + "}";
    
    // Enviar el POST con el JSON
    int httpResponseCode = http.POST(jsonPayload);
    
    // Monitoreo por puerto serie
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("Log enviado. Respuesta del servidor: ");
      Serial.println(httpResponseCode); // Debería devolver 200
    } else {
      Serial.print("Error al enviar POST. Código de error: ");
      Serial.println(httpResponseCode);
    }
    
    http.end(); // Liberar recursos de la conexión
  } else {
    Serial.println("No se pudo enviar el log: ESP32 desconectado del WiFi");
  }
}

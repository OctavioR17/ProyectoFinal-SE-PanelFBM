#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define ONE_WIRE_BUS 4  // Pin para el DS18B20
#define REPORTING_PERIOD_MS 1000

PulseOximeter pox;
uint32_t tsLastReport = 0;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const char* WIFI_SSID = "TP-Link_B67E"; // cambiar
const char* WIFI_PASSWORD = "95930848"; // cambiar

const char* SERVER = "http://192.168.0.108:3000/"; // cambiar

void wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 
  Serial.println("Connecting to Wi-Fi");
 
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  //Serial.println();
  Serial.print("\nSuccessfully connected to : ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("-------------");
}

void iniciarSensores() {
  if (!pox.begin()) {
    Serial.println("No se pudo encontrar el sensor MAX30100, verifique las conexiones.");
    for (;;);
  }

  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  sensors.begin();
}

void leerSensores() {
  pox.update();

  sensors.requestTemperatures();
  float temperatura = sensors.getTempCByIndex(0);

  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    tsLastReport = millis();

    float oxigenacion = pox.getSpO2();
    float pulso = pox.getHeartRate();

    Serial.print("Oxigenacion (%): ");
    Serial.print(oxigenacion);
    Serial.print("\tPulso (BPM): ");
    Serial.print(pulso);
    Serial.print("\tTemperatura (°C): ");
    Serial.println(temperatura);
  }
}

void enviarDatos() {
  Serial.println("\nEnviando datos...");

  StaticJsonDocument<200> doc;

  doc["humidity"] = 117; // reemplazar por la lectura del sensor
  doc["temperature"] = 343; //// reemplazar por la lectura del sensor
  
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);

  HTTPClient http;
  http.begin(SERVER);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(jsonBuffer);

  Serial.println(jsonBuffer);

  
  if(httpResponseCode>0){
    String response = http.getString();
    Serial.println(httpResponseCode);
    Serial.println(response);
  }else{
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }

  http.end();  //Free resources
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  
  //iniciarSensores();

  wifi();
  
}

void loop() {
  //leerSensores();

  enviarDatos();
  delay(5000);
}

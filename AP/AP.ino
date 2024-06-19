#include <WiFi.h>
#include <WebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "index.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

/* Put your SSID & Password */
const char* ssid = "ESP32";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

const char* SERVER = "http://192.168.1.2:3000/"; // cambiar

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);

// azul
int painLED = 4;
// verde
int tissueLED = 5;
// amarillo
int hairLED = 19;

int sensor = 2;
OneWire oneWire(sensor);
DallasTemperature sensors(&oneWire);
float temperatura = 117.0;

void setup() {
  Serial.begin(115200);

  pinMode(painLED, OUTPUT);
  pinMode(tissueLED, OUTPUT);
  pinMode(hairLED, OUTPUT);

  sensors.requestTemperatures();
  temperatura = sensors.getTempCByIndex(0);

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  server.on("/", handle_OnConnect);
  server.on("/pain", painTreatment);
  server.on("/tissue", tissueTreatment);
  server.on("/hair", hairTreatment);
  server.on("/cancel", cancelTreatment);

  server.on("/button5", enviarDatos);

  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  sensors.requestTemperatures();
  temperatura = sensors.getTempCByIndex(0);
  Serial.println(temperatura);
  if(temperatura > 30){
    cancelTreatment();
  }
  server.handleClient();
}

void handle_OnConnect() {
  Serial.println("\nIniciado\n");
  server.send(200, "text/html", SendHTML()); 
}

void painTreatment() {
  Serial.println("\nPain\n");
  analogWrite(painLED, 30);
  server.send(200, "text/html", SendHTML()); 
}

void tissueTreatment() {
  Serial.println("\ntissue\n");
  for(int i = 0; i < 5; i++){
    analogWrite(tissueLED, 255);
    delay(1000);
    analogWrite(tissueLED, 125);
    delay(1000);
  }
  server.send(200, "text/html", SendHTML()); 
}

void hairTreatment() {
  Serial.println("\nhair\n");
  analogWrite(hairLED, 255);
  server.send(200, "text/html", SendHTML()); 
}

void cancelTreatment() {
  Serial.println("\ncanceling\n");
  analogWrite(painLED, 0);
  analogWrite(tissueLED, 0);
  analogWrite(hairLED, 0);
  server.send(200, "text/html", SendHTML()); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

void enviarDatos() {
  Serial.println("\nEnviando datos...");

  StaticJsonDocument<200> doc;

  doc["nombre"] = "Oktavio"; // reemplazar por el nombre del paciente
  doc["intensidadled"] = 11; // reemplazar por la lectura del sensor
  doc["oxigenacion"] = 0.00; //// reemplazar por la lectura del sensor
  doc["pulso"] = 5.72; // reemplazar por la lectura del sensor
  doc["temperatura"] = 26.00; // reemplazar por la lectura del sensor
  
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

  server.send(200, "text/html", SendHTML()); 
}
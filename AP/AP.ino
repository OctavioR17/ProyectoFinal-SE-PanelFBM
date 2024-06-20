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
int led1 = 4;
// verde
int led2 = 5;
// amarillo
int led3 = 19;

int led4 = 10;

int led5 = 19;

int activeLed = 0;

int sensor = 2;
OneWire oneWire(sensor);
DallasTemperature sensors(&oneWire);
float temperatura = 117.0;

void setup() {
  Serial.begin(115200);

  iniciarSensores();

  iniciarServidor();
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

void iniciarSensores() {
  Serial.println("Iniciando sensores...");
  
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);

  sensors.requestTemperatures();
  temperatura = sensors.getTempCByIndex(0);

  Serial.println("Sensores iniciados");
}

void iniciarServidor() {
  Serial.println("Iniciando servidor...");
  
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  server.on("/", handle_OnConnect);
  server.on("/button1", opcion1);
  server.on("/button2", opcion2);
  server.on("/button3", opcion3);
  server.on("/button4", opcion4);
  server.on("/button5", opcion5);
  server.on("/slider", slider);
  server.on("/update", update);
  server.on("/cancel", cancelTreatment);

  //server.on("/button5", enviarDatos);

  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");

}

void handle_OnConnect() {
  Serial.println("\nIniciado\n");
  server.send(200, "text/html", SendHTML()); 
}

void opcion1() {
  activeLed = led1;
  Serial.println("\nOpcion 1\n");

  //adaptar al tratamiento adecuado
  analogWrite(led1, 30);

  server.send(200, "text/html", SendHTML()); 
}

void opcion2() {
  activeLed = led2;
  Serial.println("\nOpcion 2\n");
  
  analogWrite(led2, 255);
  
  server.send(200, "text/html", SendHTML()); 
}

void opcion3() {
  activeLed = led3;
  Serial.println("\nOpcion 3\n");
  analogWrite(led3, 255);
  server.send(200, "text/html", SendHTML()); 
}

void opcion4() {
  activeLed = led4;
  Serial.println("\nOpcion 4\n");
  analogWrite(led4, 255);
  server.send(200, "text/html", SendHTML()); 
}

void opcion5() {
  activeLed = led5;
  Serial.println("\nOpcion 5\n");
  analogWrite(led5, 255);
  server.send(200, "text/html", SendHTML()); 
}

void slider() {
  Serial.println("\nSlider\n");

  int intensity = server.arg("intensity").toInt();
  int duration = server.arg("duration").toInt();
  int frequency = server.arg("frequency").toInt();

  Serial.println(intensity);
  Serial.println(duration);
  Serial.println(frequency);

  analogWrite(activeLed, intensity);

  Serial.print("Led: "); Serial.print(activeLed); 
  Serial.print(", intensity: "); Serial.println(intensity);

  server.send(200, "text/html", SendHTML()); 
}

void cancelTreatment() {
  Serial.println("\ncanceling\n");

  activeLed = 0;

  analogWrite(led1, 0);
  analogWrite(led2, 0);
  analogWrite(led3, 0);
  analogWrite(led4, 0);
  analogWrite(led5, 0);

  server.send(200, "text/html", SendHTML()); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

void update() {
  Serial.println("\nEnviando datos...");

  StaticJsonDocument<200> doc;

  doc["nombre"] = "Oktavio"; // reemplazar por el nombre del paciente
  doc["intensidadled"] = 11; // reemplazar por la lectura del sensor
  doc["oxigenacion"] = 56.00; //// reemplazar por la lectura del sensor
  doc["pulso"] = 5.72; // reemplazar por la lectura del sensor
  doc["temperatura"] = 26.00; // reemplazar por la lectura del sensor
  
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);

  server.send(200, "application/json", jsonBuffer); 
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
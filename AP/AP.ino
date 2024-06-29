#include <WiFi.h>
#include <WebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "index.h"
#include "variables.h"

void setup() {
  Serial.begin(115200);

  iniciarSensores();

  iniciarServidor();
}

void loop() {
  sensors.requestTemperatures();
  temperatura = sensors.getTempCByIndex(0);
  //Serial.println(temperatura);
  if(temperatura > 30 || digitalRead(pinEmergencia) == HIGH){
    //Serial.println("Temperatura alta");
    Serial.println("*******\n Emergencia \n*******");
    terminarTratamiento();
  }

  server.handleClient();
}

void iniciarSensores() {
  Serial.println("\nIniciando sensores...");
  /*
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  */

  pinMode(ledInfrarrojo, OUTPUT);
  pinMode(pinEmergencia, INPUT);
  sensors.requestTemperatures();
  temperatura = sensors.getTempCByIndex(0);

  Serial.println("Sensores iniciados");
}

void iniciarServidor() {
  Serial.println("\nIniciando servidor...");
  
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  server.on("/", handle_OnConnect);
  server.on("/tratamiento1", tratamiento1);
  server.on("/tratamiento2", tratamiento2);
  server.on("/tratamiento3", tratamiento3);
  server.on("/tratamiento4", tratamiento4);
  server.on("/tratamiento5", tratamiento5);
  server.on("/slider", slider);
  server.on("/updateSensors", updateSensors);
  server.on("/terminarTratamiento", terminarTratamiento);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("Servidor iniciado\n");
}

void handle_OnConnect() {
  Serial.println("\nIniciado\n");
  server.send(200, "text/html", SendHTML()); 
}

void tratamiento1() {
  tratamiento = "tratamiento1";
  //activeLed = led1;
  Serial.println("\ tratamiento 1\n");
  intensidad = 50;
  //adaptar al tratamiento adecuado
  analogWrite(ledInfrarrojo, intensidad);

  server.send(200, "text/html", SendHTML()); 
}

void tratamiento2() {
  tratamiento = "tratamiento2";
  //activeLed = led2;
  Serial.println("\ tratamiento 2\n");
  intensidad = 100;
  analogWrite(ledInfrarrojo, intensidad);
  
  server.send(200, "text/html", SendHTML()); 
}

void tratamiento3() {
  tratamiento = "tratamiento3";
  //activeLed = led3;

  Serial.println("\ tratamiento 3\n");
  intensidad = 150;
  analogWrite(ledInfrarrojo, intensidad);
  server.send(200, "text/html", SendHTML()); 
}

void tratamiento4() {
  tratamiento = "tratamiento4";
  //activeLed = led4;

  Serial.println("\ tratamiento 4\n");
  intensidad = 200;
  analogWrite(ledInfrarrojo, intensidad);
  server.send(200, "text/html", SendHTML()); 
}

void tratamiento5() {
  tratamiento = "tratamiento5";
  //activeLed = led5;

  Serial.println("\ tratamiento 5\n");
  intensidad = 250;
  analogWrite(ledInfrarrojo, intensidad);
  server.send(200, "text/html", SendHTML()); 
}

void slider() {
  Serial.println("\nSlider\n");

  intensidad = server.arg("intensity").toInt();
  //int duration = server.arg("duration").toInt();
  frecuencia = server.arg("frequency").toInt();

  Serial.println(intensidad);
  //Serial.println(duration);
  Serial.println(frecuencia);

  analogWrite(ledInfrarrojo, intensidad);

  Serial.print("Led: "); Serial.print(ledInfrarrojo); 
  Serial.print(", intensidad: "); Serial.println(intensidad);

  server.send(200, "text/html", SendHTML()); 
}

void terminarTratamiento() {
  Serial.println("\nTerminando\n");

  //activeLed = 0;
  tratamiento = "";
  intensidad = 0;
  analogWrite(ledInfrarrojo, intensidad);

  /*
  analogWrite(led1, 0);
  analogWrite(led2, 0);
  analogWrite(led3, 0);
  analogWrite(led4, 0);
  analogWrite(led5, 0);
  */

  server.send(200, "text/html", SendHTML()); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

void updateSensors() {
  Serial.println("\nEnviando datos...");

  String tratamiento = server.arg("tratamiento");
  StaticJsonDocument<200> doc;

  doc["intensidad"] = intensidad; // reemplazar por la lectura del sensor
  doc["frecuencia"] = 117;
  doc["oxigenacion"] = 56.00; //// reemplazar por la lectura del sensor
  doc["pulso"] = 5.72; // reemplazar por la lectura del sensor
  doc["temperatura"] = 26.00; // reemplazar por la lectura del sensor
  doc["tratamiento"] = tratamiento;
  
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);

  // para insertar en la bd ---------------
  if(tratamiento.length() > 0){
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
  // para insertar en la bd ---------------

  server.send(200, "application/json", jsonBuffer);
}
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4  // Pin para el DS18B20
#define REPORTING_PERIOD_MS 1000

PulseOximeter pox;
uint32_t tsLastReport = 0;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(115200);

  if (!pox.begin()) {
    Serial.println("No se pudo encontrar el sensor MAX30100, verifique las conexiones.");
    for (;;);
  }

  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  sensors.begin();
}

void loop() {
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

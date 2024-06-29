/* Put your SSID & Password */
const char* ssid = "ESP32";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

const char* SERVER = "http://192.168.1.2:3000/"; // cambiar

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);

String tratamiento = "";

// sensore de temperatura
int pinSensorTemperatura = 0;
// sensor de oxigenacion
int pinSensorOxigenacion = 0;
// sensor de ritmo cardiaco
int pinSensorRitmo = 0;
// led infrarrojo
int ledInfrarrojo = 4;
// intensidad
int intensidad = 0;
// frecuencia
float frecuencia = 0;
// boton de emergencia
int pinEmergencia = 19;

int activeLed = 0;

int sensor = 2;
OneWire oneWire(sensor);
DallasTemperature sensors(&oneWire);
float temperatura = 117.0;
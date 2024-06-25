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
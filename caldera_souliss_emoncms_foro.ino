
// Let the IDE point to the Souliss framework
#include "SoulissFramework.h"
// Configure the framework
#include "bconf/MCU_ESP8266.h"              // Load the code directly on the ESP8266
#include "conf/DynamicAddressing.h"
#include "conf/IPBroadcast.h"

// Include framework code and libraries
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include "Souliss.h"

#include <ArduinoOTA.h>

// This identify the number of the SLOT logic
#define DALLAS              0
#define DALLAS2             2
#define DALLAS3             4
#define DALLAS4             6
#define DALLASi             8
#define DALLASe            10
// This identify the number of the LED logic
#define MYreleLOGIC          12

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS D5  // on pin 2 (a 4.7K resistor is necessary)
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensores(&oneWire);

// **** Define here the right pin for your ESP module RELE ****
#define OUTPUTPIN   D6


// Contador gas
const int PinReed = D7; //pin donde conecto el reed
// Variables will change:
boolean  ReedState = 0;         // current state of the button
boolean lastReedState = 1; // previous state of the button
int contador = 0;

//long previousMillis = 0;
//long interval = 10000; //El intervalo que determina cada cuanto se entra al if
String cadena = "";
String pulso;

// Temperatura
float tempC = 0;
float temp2 = 0;
float temp3 = 0;
float temp4 = 0;
float tempe = 0;
float tempi = 0;
DeviceAddress S1 = {0x28, 0xFF, 0x91, 0x2A, 0xB5, 0x16, 0x05, 0xA9};  //Sensor superior
DeviceAddress S2 = {0x28, 0xFF, 0x5F, 0x23, 0xB5, 0x16, 0x05, 0xBA};  //Sensor
DeviceAddress S3 = {0x28, 0xFF, 0xE1, 0xE5, 0xB5, 0x16, 0x03, 0xBA};  //Sensor
DeviceAddress S4 = {0x28, 0xAA, 0x5F, 0x19, 0x06, 0x00, 0x00, 0xD};  //Sensor inferior
DeviceAddress Se = {0x28, 0xFF, 0x32, 0x2B, 0x93, 0x16, 0x5, 0x1E}; // Sensor Exterior
DeviceAddress Si = {0x28, 0xFF, 0x61, 0xE3, 0xB5, 0x16, 0x3, 0x78}; // Sensor Interior


String floatToString( float, int = 3, int = 2, boolean = false);

//<--- Conexion wifi ESP8266
#include <ESP8266WiFi.h>
#define WIFICONF_INSKETCH
#define WiFi_SSID               "xxxxx"
#define WiFi_Password       "xxxxxx"


const char* host = "80.243.190.58";
const char* streamId   = "....................";
const char* apikey = "5963b833ed48130be8102e4e52fc681e";


//>--- fin configuracion wifi


// Convierte un float en una cadena.
// n -> número a convertir.
// l -> longitud total de la cadena, por defecto 8.
// d -> decimales, por defecto 2.
// z -> si se desea rellenar con ceros a la izquierda, por defecto true.
String floatToString( float n, int l, int d, boolean z) {
  char c[l + 1];
  String s;

  dtostrf(n, l, d, c);
  s = String(c);

  if (z) {
    s.replace(" ", "0");
  }
  return s;
}

boolean antiRebote (boolean eAnterior)
{
  boolean eActual = digitalRead(PinReed);
  if (eAnterior != eActual)
  {
    delay(5);
    eActual = digitalRead(PinReed);
  }
  return eActual;
}

void setup(void) {
   Serial.begin(115200);
  //pinMode (BUILTIN_LED , OUTPUT);
  //digitalWrite(BUILTIN_LED, HIGH);
  Initialize();
  ReadIPConfiguration();
  SetDynamicAddressing();
  GetAddress();
 

  // We start by connecting to a WiFi network

 
  Serial.print("Connecting to ");
  Serial.println(WiFi_SSID);
 

  // initialize the button pin as a input:
  pinMode(PinReed, INPUT_PULLUP); //Resistencia de pullup interna

  //  while (WiFi.status() != WL_CONNECTED) {
  //    delay(500);
  //    Serial.print(".");
  //  }


  sensores.begin(); //Se inician los sensores DS18B20

  Set_Temperature(DALLAS);
  Set_Temperature(DALLAS2);
  Set_Temperature(DALLAS3);
  Set_Temperature(DALLAS4);
  Set_Temperature(DALLASe);
  Set_Temperature(DALLASi);


  Set_SimpleLight(MYreleLOGIC);        // Define a simple LED light logic

  pinMode(OUTPUTPIN, OUTPUT);         // Use pin as output Rele
//lastReedState = digitalRead(PinReed);
ArduinoOTA.begin();
}
void loop(void)
{
 ArduinoOTA.handle();
  EXECUTEFAST() {
    UPDATEFAST();

     FAST_10ms() {
           // Read temperature and humidity from DHT every 110 seconds
      // sensores.requestTemperatures();  //Enviamos el comando para obtener los datos de los sensores
      //      float tempC = sensores.getTempC(S1);
      //      float temp2 = sensores.getTempC(S2);
      //      float temp3 = sensores.getTempC(S3);
      //      float temp4 = sensores.getTempC(S4);
      Logic_SimpleLight(MYreleLOGIC);
      DigOut(OUTPUTPIN, Souliss_T1n_Coil, MYreleLOGIC);
     }

    FAST_90ms() {
     // ArduinoOTA.handle();
      // Read temperature and humidity from DHT every 110 seconds
      // sensores.requestTemperatures();  //Enviamos el comando para obtener los datos de los sensores
      //      float tempC = sensores.getTempC(S1);
      //      float temp2 = sensores.getTempC(S2);
      //      float temp3 = sensores.getTempC(S3);
      //      float temp4 = sensores.getTempC(S4);
     // Logic_SimpleLight(MYreleLOGIC);
     // DigOut(OUTPUTPIN, Souliss_T1n_Coil, MYreleLOGIC);

      ReedState = antiRebote(lastReedState);
      // ReedState = digitalRead(PinReed);
      if (ReedState != lastReedState) {
        //   if the state has changed, increment the counter
        if (ReedState == 1) {
          contador++;
          // if the current state is HIGH then the button
          // wend from off to on:
          // buttonPushCounter++;
          pulso = ",GasPulso:0.0" + (String(contador));
          //Serial.print("segundo if reedstate: ");
          //Serial.println(ReedState);
          Serial.print("======= contador: ");
          Serial.println(contador);
        }
      }
      lastReedState = ReedState ;
    }


    FAST_910ms()    {
      sensores.requestTemperatures();  //Enviamos el comando para obtener los datos de los sensores
      tempC = sensores.getTempC(S1);
      temp2 = sensores.getTempC(S2);
      temp3 = sensores.getTempC(S3);
      temp4 = sensores.getTempC(S4);
      tempe = sensores.getTempC(Se);
      tempi = sensores.getTempC(Si);

      Souliss_ImportAnalog(memory_map, DALLAS, &tempC);
      Souliss_ImportAnalog(memory_map, DALLAS2, &temp2);
      Souliss_ImportAnalog(memory_map, DALLAS3, &temp3);
      Souliss_ImportAnalog(memory_map, DALLAS4, &temp4);
      Souliss_ImportAnalog(memory_map, DALLASe, &tempe);
      Souliss_ImportAnalog(memory_map, DALLASi, &tempi);
    }


    FAST_2110ms()
    {
      Logic_Temperature(DALLAS);
      Logic_Temperature(DALLAS2);
      Logic_Temperature(DALLAS3);
      Logic_Temperature(DALLAS4);
      Logic_Temperature(DALLASe);
      Logic_Temperature(DALLASi);

    }
    // Here we handle here the communication with Android
    FAST_PeerComms();
  }
  EXECUTESLOW() {
    UPDATESLOW();



    SLOW_10s() {


     // unsigned long currentMillis = millis();

     // if (currentMillis - previousMillis > interval) //Actualmente llevamos 950ms y la ultima vez fue a los 0ms, eso es mayor a 1000? No, entonces no pasa nada..
    //  {
      //  previousMillis = currentMillis; //"La ultima vez fue AHORA"
        // Serial.println(pulso);
        cadena = ("{temp1:" + floatToString(tempC, 5) + ",temp2:" + floatToString(temp2, 5) + ",temp3:" + floatToString(temp3, 5) + ",temp4:" + floatToString(temp4, 5) + ",tempInterior:" + floatToString(tempi, 5) + ",tempExterior:" + floatToString(tempe, 5)  + pulso + "}");
        pulso = "";
        contador = 0;
        Serial.print(cadena);
        Serial.println("") ;
        Serial.print("connecting to ");
        Serial.println(host);

        // Use WiFiClient class to create TCP connections
        WiFiClient client;
        const int httpPort = 80;
        if (!client.connect(host, httpPort)) {
          Serial.println("connection failed");
          return;
        }

        String url = "/emoncms/input/post.json?node=Cocina&json=" + cadena + "&apikey=" + apikey;

        Serial.print("Requesting URL: ");
        Serial.println(url);

        // This will send the request to the server
        client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" +
                     "Connection: close\r\n\r\n");

        //delay(1000); //delay(2000);

        // Read all the lines of the reply from server and print them to Serial
        while (client.available()) {
          String line = client.readStringUntil('\r');
          Serial.print(line);
        }

        Serial.println();
        Serial.println("closing connection");
    //  }
    }
    SLOW_PeerJoin();
  }
}

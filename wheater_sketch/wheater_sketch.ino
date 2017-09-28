

// Bibliotecas do sensor DS18B20
// Sensor de temperatura a prova dagua
#include <OneWire.h> 
#include <DallasTemperature.h>

// Biblioteca AM2302
#include "DHT.h"

// BMP 180
// https://www.sparkfun.com/products/retired/11824
#include <SFE_BMP180.h>
#include <Wire.h>

#define PINO_DS18B20 2 // Temperatura do solo
#define PINO_AM2302 3
#define PINO_MOLHAMENTO_FOLIAR 0
#define PINO_LUMINOSIDADE_SOLAR 1
#define PINO_HIGROMETRO 2

OneWire oneWire(PINO_DS18B20);

DallasTemperature sensorsDallas(&oneWire);

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(PINO_AM2302, DHTTYPE);

// You will need to create an SFE_BMP180 object, here called "pressure":
SFE_BMP180 pressure;
#define ALTITUDE 362.0 // Altitude of SparkFun's HQ in Boulder, CO. in meters

 // include the SD library:
#include <SD.h>
File myFile;
const int chipSelect = 4;

// Leitura de temperatura com LM35DZ
#define PINO_LM35DZ 3
// Variaveis que armazenam a temperatura em Celsius e Fahrenheit
int tempc = 0,tempf=0; 
int samples[8]; // Array para precisão na medição
// Variáveis que guardam a temperatura máxima e mínima
// int maxtemp = -100, mintemp = 100;

int contador = 1;
float luminosidadeSolar;
float molhamentoFoliar;
float pressaoAtmosferica;
float temperaturaAr;
float temperaturaAr2;
float temperaturaSolo;
float umidadeAr;
float umidadeSolo;

void setup(void) 
{ 
  // start serial port 
  Serial.begin(9600);

  // Start up the library 
  sensorsDallas.begin();
  
  dht.begin();
  
  if (!pressure.begin())
  {
    Serial.println("BMP180 init fail\n\n");
    while(1)
    {
      delay(60000);
    }
  }
  
  //if (SD.begin(chipSelect))
  //{
  //  Serial.println("SD init success");
  //}
  //else
  //{
  //  Serial.println("initialization failed!");
  //  while(1)
  //  {
  //    delay(60000);
  //  }
  //}
}

void loop(void) 
{
  
  luminosidadeSolar = lerLuminosidadeSolar();
  molhamentoFoliar = lerMolhamentoFoliar();
  pressaoAtmosferica = lerPressaoAtmosferica();
  temperaturaAr = lerTemperaturaAr();
  temperaturaAr2 = lerTemperaturaAr2();
  temperaturaSolo = lerTemperaturaSolo();
  umidadeAr = lerUmidadeAr();
  umidadeSolo = lerUmidadeSolo();

  //Serial.print("Contador = ");
  //Serial.println(contador);
  
  /* Serial.print("Luminosidade Solar = ");
  Serial.println(luminosidadeSolar);
  
  Serial.print("Molhamento Foliar = ");
  Serial.println(molhamentoFoliar);
  
  Serial.print("Temperatura do ar = ");
  Serial.println(temperaturaAr);
  
  Serial.print("Temperatura do ar2 = ");
  Serial.println(temperaturaAr2);
  
  Serial.print("Temperatura do solo = ");
  Serial.println(temperaturaSolo);
  
  Serial.print("Umidade do ar = ");
  Serial.println(umidadeAr);
  
  Serial.print("Umidade do solo = ");
  Serial.println(umidadeSolo); */
  
  char buffer[10];
  String sep = ";";
  String dataString = "";
  
  dataString += contador;
  dataString += sep;
  dataString += dtostrf(luminosidadeSolar, 4, 2, buffer);
  dataString += sep;
  dataString += dtostrf(molhamentoFoliar, 4, 2, buffer);
  dataString += sep;
  dataString += dtostrf(temperaturaAr, 4, 2, buffer);
  dataString += sep;
  //dataString += dtostrf(temperaturaAr2, 4, 2, buffer);
  //dataString += sep;
  dataString += dtostrf(temperaturaSolo, 4, 2, buffer);
  dataString += sep;
  dataString += dtostrf(umidadeAr, 4, 2, buffer);
  dataString += sep;
  dataString += dtostrf(umidadeSolo, 4, 2, buffer);
  
  Serial.println(dataString);
  Serial.println("=========================");
  // Serial.println("");
  
  //myFile = SD.open("wheater.csv", FILE_WRITE);
  //myFile.println(dataString);
  //myFile.close();
  
  contador++;
  
  delay(1500);
}

float lerLuminosidadeSolar()
{
  // 1023 = Sem luz / 0 = Com luz
  // 79 = Com laterna do cel
  // 1018 = Com fita isolante por cima
  return analogRead(PINO_LUMINOSIDADE_SOLAR);
}

float lerMolhamentoFoliar()
{
  // max 577/877
  // min 49/15
  return analogRead(PINO_MOLHAMENTO_FOLIAR);
}

float lerTemperaturaAr()
{
  return dht.readTemperature();
}

// http://blog.vidadesilicio.com.br/arduino/basico/lm35-medindo-temperatura-com-arduino/
float lerTemperaturaAr2() {
  float temp;
  // return float(analogRead(PINO_LM35DZ));
  temp = (analogRead(PINO_LM35DZ) * 5.0 / 1024.0) / 0.01;
  // return ( 5.0 * analogRead(PINO_LM35DZ) * 100.0) / 1024.0;
  
  // Ajuste
  // temp *= 16.8 / 19.04;
  
  return temp;
}

float lerTemperaturaSolo()
{
  // Send the command to get temperature readings 
  sensorsDallas.requestTemperatures();
  return sensorsDallas.getTempCByIndex(0);
}

float lerPressaoAtmosferica() {
  char status;
  double T,P,p0,a;
  
  status = pressure.startTemperature();
  
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      // Print out the measurement:
      //Serial.print("temperature: ");
      //Serial.print(T,2);
      //Serial.println("C");
      
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.

      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        T = 4.0;
        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          // Print out the measurement:
          //Serial.print("absolute pressure: ");
          //Serial.print(P,2);
          //Serial.println(" mb");

          // The pressure sensor returns abolute pressure, which varies with altitude.
          // To remove the effects of altitude, use the sealevel function and your current altitude.
          // This number is commonly used in weather reports.
          // Parameters: P = absolute pressure in mb, ALTITUDE = current altitude in m.
          // Result: p0 = sea-level compensated pressure in mb

          p0 = pressure.sealevel(P,ALTITUDE);
          pressaoAtmosferica = p0;
          //Serial.print("relative (sea-level) pressure: ");
          //Serial.print(p0,2);
          //Serial.println(" mb");

          // On the other hand, if you want to determine your altitude from the pressure reading,
          // use the altitude function along with a baseline pressure (sea-level or other).
          // Parameters: P = absolute pressure in mb, p0 = baseline pressure in mb.
          // Result: a = altitude in m.

          //a = pressure.altitude(P,p0);
          //Serial.print("computed altitude: ");
          //Serial.print(a,0);
          //Serial.println(" meters");
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");
  
  return 0;
}

float lerUmidadeAr()
{
  return dht.readHumidity();
}

float lerUmidadeSolo()
{
  // 0 = 100% umido
  // 702 = 0% umido
  return analogRead(PINO_HIGROMETRO);
}

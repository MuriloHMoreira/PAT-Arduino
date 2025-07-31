

/**
 * @file MultiThermometer.ino
 * Read temperatures from multiple Termopar Nanoshields, with different thermocouple types.
 * 
 * Copyright (c) 2015 Circuitar
 * This software is released under the MIT license. See the attached LICENSE file for details.
 */
#include <SPI.h>
#include <SD.h>
#include <Nanoshield_Termopar.h>

int sensorPin = A0;
int sensorRead = 0;
float V_read = 0.0;

#define FILENAME "datalog.txt" // Name of the file to use in the SD card (max 8+3 characters)
#define SAMPLE_TIME 10000      // Sample time, in milliseconds


// Create an array of four type J thermocouples
Nanoshield_Termopar tc[] = {
  Nanoshield_Termopar(7, TC_TYPE_J),  // Jumper on D7
  Nanoshield_Termopar(8, TC_TYPE_J),  // Jumper on D8
  Nanoshield_Termopar(10, TC_TYPE_J), // Jumper on D10
  Nanoshield_Termopar(A3, TC_TYPE_J)  // Jumper on A3
};
const int numModules = sizeof(tc) / sizeof(Nanoshield_Termopar);

// Use pin D4 as the SD card CS pin
const int cardChipSelect = 4;

void setup()
{
  Serial.begin(9600);
  Serial.println("------------------");
  Serial.println(" Analog Input & Thermocouples");
  Serial.println("------------------");
  Serial.println();
  
  // Initialize each Termopar Nanoshield
  for(int i = 0; i < numModules; i++) {
    tc[i].begin();
  }

  // Initialize the SD card
  if (!SD.begin(cardChipSelect)) {
    Serial.println("Unable to find SD card, aborting");
    while (true);
  }
  Serial.println("SD card initialized");

}

void printErrors(Nanoshield_Termopar tc) {
  if (tc.isOpen()) {
    Serial.print("(Open circuit) ");
  } else if (tc.isOverUnderVoltage()) {
    Serial.print("(Overvoltage/Undervotage) ");
  } else if (tc.isInternalOutOfRange()) {
    Serial.print("(Internal temperature out of range) ");
  } else if (tc.isExternalOutOfRange()) {
    Serial.print("(External temperature out of range) ");
  }
}

float convertVoltToVolFlow(float V_read) {
  float q_0 = -8.6;
  float q_max = 111.6;
  float q;
  float V_0 = 2.69;
  float V_max = 3.63;
  q = (V_read - V_0) / (V_max - V_0) * (q_max - q_0);
  return q;
}

void loop()
{
  File dataFile = SD.open(FILENAME, FILE_WRITE);

  if (dataFile) {
    // Write current time to the file, in milliseconds
    unsigned long t = millis();
    dataFile.print(t);
    Serial.print("Time [s]: ");
    Serial.print(t / 1000);
    dataFile.print(',');
    Serial.print(", Voltage [V]: ");
    sensorRead = analogRead(sensorPin);
    float V_read = sensorRead * (5.0 / 1023.0);
    Serial.print(V_read);
    dataFile.print(V_read);
    dataFile.print(',');
    Serial.print(", Mass Flow [mL/min]:");
    Serial.print(convertVoltToVolFlow(V_read));
    dataFile.print(convertVoltToVolFlow(V_read));
    //Serial.println();

    for(int i = 0; i < numModules; i++) {
      // Read thermocouple data
      tc[i].read();
      
      // Write temperature data to the file
      dataFile.print(',');
      if (tc[i].hasError()) {
        //printErrors(tc[i]);
        ;
      } else {
        dataFile.print(tc[i].getExternal());
        Serial.print(", T_");
        Serial.print(i+1);
        Serial.print(" [°C] : ");
//        Serial.print(tc[i].getInternal());
        Serial.print(tc[i].getExternal());
//        Serial.print(",");
      }
    }
    dataFile.println();
    dataFile.close();
    dataFile.flush();

    //Serial.print("\t Data logged at ");
    //Serial.print(t);
    //Serial.println("ms");
    Serial.println();
  } else {
    Serial.println("Unable to open file");
  }


  // Wait for next second
  delay(1000);
}

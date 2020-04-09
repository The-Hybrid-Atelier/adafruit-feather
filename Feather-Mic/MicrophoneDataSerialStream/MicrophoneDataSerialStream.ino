/****************************************
Sound Level from
Adafruit Microphone Amplifier

Contributors: Cesar Torres, Hedieh Moradi
****************************************/
#include "Adafruit_ZeroFFT.h"
#include "Adafruit_CPlay_Mic.h"

#define SKETCH_NAME "Microphone Grinder"
#define SKETCH_VERSION 2.0
#define BAUD 115200 // Serial Communication buadrate
#define MIC_PIN A4

// MICROPHONE DEFINES
#define DATA_SIZE 256 // the sample rate; power of 2
#define FS 22000
#define FREQ_MIN 600 // the lowest frequency that will register on the meter
#define FREQ_MAX 3000 // the highest frequency that will register on the meter
#define MIN_INDEX FFT_INDEX(FREQ_MIN, FS, DATA_SIZE)
#define MAX_INDEX FFT_INDEX(FREQ_MAX, FS, DATA_SIZE)
#define SCALE_FACTOR 32

int16_t inputData[DATA_SIZE];
Adafruit_CPlay_Mic mic = Adafruit_CPlay_Mic();
void setup() 
{
   Serial.begin(BAUD);
   Serial.print(SKETCH_NAME);
   Serial.println(" ");
   Serial.print(SKETCH_VERSION);
   Serial.println(" @ ");
   Serial.print(BAUD);   
}

void capture(){
  for(int i = 0; i< DATA_SIZE; i++){
    inputData[i] = analogRead(MIC_PIN);
  }
}

void loop() 
{
  capture();
  for(int i = 0; i< DATA_SIZE; i++){
    Serial.print("d:");
    Serial.println(inputData[i]);
  }
}


#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include "Adafruit_MPR121.h"

#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

/* NEOPIXEL CODE */
#define LED_PIN    6
#define LED_COUNT 12
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


void neopixel_setup() {
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif
  strip.begin();           
  strip.show();           
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
}


/* END OF NEOPIXEL CODE */
bool state_record = true;
bool state_feedback = false;
uint32_t black = strip.Color(  0, 0,   0);
uint32_t green = strip.Color(  0, 255,   0);
uint32_t red = strip.Color(  225, 0,   0);
 
/* API CODE */
void print_state(){
  if(state_record){
    Serial.println("RECORD ON");
    strip.setPixelColor(0, red);
  }else{
    Serial.println("OFF");
    strip.setPixelColor(0, black);
  }
  if(state_feedback){
    Serial.println("FEEDBACK ON");
    strip.setPixelColor(1, green);
  }else{
    Serial.println("OFF");
    strip.setPixelColor(1, black);
  }
  strip.show();
}
/* CAPTOUCH CODE */
Adafruit_MPR121 cap = Adafruit_MPR121();
uint16_t lasttouched = 0;
uint16_t currtouched = 0;

void captouch_setup(){
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");
}
void captouch_loop(){
  currtouched = cap.touched();
  for (uint8_t i=0; i<12; i++) {
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
      if(i == 0){
        state_record = !state_record;
      }
      if(i == 1){
        state_feedback = !state_feedback;
      }
      Serial.print(i); Serial.println(" touched");
      print_state();
    }
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
      Serial.print(i); Serial.println(" released");
    }
  }
  lasttouched = currtouched;
}
/* END OF CAPTOUCH CODE */


void setup() {
  Serial.begin(9600);
  while (!Serial) { delay(10);}
  Serial.println("Controller API");   
  captouch_setup();
  neopixel_setup();
}

void loop() {
  captouch_loop();
  neopixel_loop();
}

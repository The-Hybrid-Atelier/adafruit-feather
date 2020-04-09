/****************************************
Sound Level from
Adafruit Microphone Amplifier

Contributors: Cesar Torres, Hedieh Moradi
****************************************/
#define SKETCH_NAME "Microphone Grinder"
#define SKETCH_VERSION 2.0
#define WINDOW_SIZE 50 // Sample window width in mS (50 mS = 20Hz)
#define BAUD 115200 // Serial Communication parameter
#define MIC_PIN A0

const int sampleWindow = WINDOW_SIZE; 
unsigned int sample;

void setup() 
{
   Serial.begin(BAUD);

   Serial.print(SKETCH_NAME);
   Serial.println(" ");
   Serial.print(SKETCH_VERSION);
   Serial.println(" @ ");
   Serial.print(BAUD);
   
   pinMode(MIC_PIN, INPUT);
}


void loop() 
{
   unsigned long startMillis= millis();  // Start of sample window
   unsigned int peakToPeak = 0;   // peak-to-peak level

   unsigned int signalMax = 0;
   unsigned int signalMin = 1024;

   // collect data for 50 mS
   while (millis() - startMillis < sampleWindow)
   {
      sample = analogRead(A0);
      if (sample < 1024)  // toss out spurious readings
      {
         if (sample > signalMax)
         {
            signalMax = sample;  // save just the max levels
         }
         else if (sample < signalMin)
         {
            signalMin = sample;  // save just the min levels
         }
      }
   }
   peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
   double volts = (peakToPeak * 5.0) / 1024;  // convert to volts
   Serial.print("f:");
   Serial.println(volts);
}

//#define DEBUG true; 

#include <Adafruit_NeoPixel.h>
//Define number of LED strips and arduino data pin the strip output is connected to
int numStrips = 7;
int ledDataPin = 8;
//Number of leds per strip
int ledsPerChannel = 8;
int numLeds = ledsPerChannel * numStrips;

//.000782
double divideBy1023 = 1.0 / 1023.0;
double divideTimesLeds = divideBy1023 * (ledsPerChannel);

//Create an LED object
Adafruit_NeoPixel leds = Adafruit_NeoPixel(70, ledDataPin, NEO_GRB + NEO_KHZ800);

uint32_t black = leds.Color(0, 0, 0);

//Array of colors, each representing an LED on each strip. Ranges from blue->green->yellow->red with varying intensity
uint32_t colors[8] = {
  leds.Color(30, 0, 255),
  leds.Color(30, 0, 255),
  leds.Color(0, 255, 30),
  leds.Color(0, 255, 30),
  leds.Color(0, 255, 30),
  leds.Color(222, 255, 0),
  leds.Color(255, 0, 0),
  leds.Color(255, 0, 0),
};

//Different pins on MSGEQ7. Strobe pin is channel selection pin. Each channel is a different frequency band
int readPin = 0;
int strobePin = 2;
int resetPin = 3;

int spectrum[7] = {0, 0, 0, 0, 0, 0, 0};

//Initial setup of arduino
void setup() {
  //#if defined DEBUG
  Serial.begin(2000000);
  //#endif

  //Define Arduino pins
  pinMode(readPin, INPUT);
  pinMode(strobePin, OUTPUT);
  pinMode(resetPin, OUTPUT);

  //Write to arduino pins
  digitalWrite(resetPin, LOW);
  digitalWrite(strobePin, HIGH);

  //Neopixel library functions 
  leds.begin();
  leds.setBrightness(70);
}

//Main program loop
void loop() {
  #if defined DEBUG
  unsigned long start = micros();
  #endif

  readSpectrum();
  displayAll();

  #if defined DEBUG
  unsigned long endTime = micros();
  Serial.println(endTime - start);
  #endif
}

//Reads LED frequency spectrum
void readSpectrum() {
 digitalWrite(resetPin, HIGH);
 digitalWrite(resetPin, LOW);

  delayMicroseconds(50);

  //Loop over each LED strip, reading from auxillary input on analogRead
  for (int i = 0; i < 7; i++) {
    digitalWrite(strobePin, LOW);
    delayMicroseconds(50);
    //Frequency spectrum is written to array indices
    spectrum[i] = analogRead(readPin);
    digitalWrite(strobePin, HIGH);
    delayMicroseconds(50);
  }
}

void displayAll() {
  //Clear current LED values
  leds.clear();
  
  for (int i = 0; i < 7; i++) {
    //Frequency spectrum and channel index passed here
    displaySpectrum(spectrum[i], i);
  }

  //Display colors on LEDs
  leds.show();
}

void displaySpectrum(int value, int channel) {
  //
  int level = value * divideTimesLeds;
  
  if (value > 100) {
    level++;
  }

  //Gauging the colors to display on LED strip. For example: beginningIndex = 8*1; endingIndex = 8+level, where level is based off a frequency. 
  int beginningIndex = ledsPerChannel * channel;
  int endingIndex = beginningIndex + level;

  //Sets each LED color along the string.
  int ledIndex = 0;
  for(int i = beginningIndex; i < endingIndex; i++){
    leds.setPixelColor(i, colors[ledIndex++]);
  }
}


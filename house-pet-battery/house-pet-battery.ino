#include <Adafruit_NeoPixel.h>
#include <Ewma.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>

#include "arduino_secrets.h"
#include "thingProperties.h"

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        6 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 8 // Popular NeoPixel ring size

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels

Ewma battVoltageFilter(0.1); 
Ewma chargeCurrentFilter(0.2); 
Ewma loadCurrentFilter(0.2); 

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  
  analogReadResolution(12);

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
  setDebugMessageLevel(4);
  ArduinoCloud.printDebugInfo();

  ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  ads.begin();
}

void loop() {
  int blueVal = 0;
  int greenVal = 0;
  int redVal = 0;
  
  ArduinoCloud.update();

  int16_t adc0, adc1, adc2;

  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);

  float batt_volts = adc0 * 0.1875;
  float charge_volts = adc1 * 0.1875;
  float load_volts = adc2 * 0.1875;

  if (charge_volts > 1500)
  {
    charge_volts = 0;
  }

  if (load_volts > 1500)
  {
    load_volts = 0;
  }

  float only_load_mv = load_volts - charge_volts;

  float charge_amps = ((charge_volts/1000) * 35.12) - 0.13;
  if (charge_amps < 0 )
  {
    charge_amps = 0;
  }
  
  float load_amps = ((only_load_mv /1000) * 43.30) - 1.71;

  chargeCurrent = chargeCurrentFilter.filter(charge_amps);
  loadCurrent = loadCurrentFilter.filter(load_amps);
  batteryvoltage = battVoltageFilter.filter(batt_volts/1000);

  Serial.print("Charge A: "); Serial.println(chargeCurrent);
  Serial.print("Load: "); Serial.println(loadCurrent);
  Serial.print("Batt: "); Serial.println(batteryvoltage);

  // Calculate Power Ratio
  currentRatio = chargeCurrent - loadCurrent;
  

  if (currentRatio > 0)
  {
    redVal = 0;
    greenVal = 50;
    blueVal = 0;
  }
  else
  {
    redVal = 50;
    greenVal = 0;
    blueVal = 0;
  }

    // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
  for(int i=0; i<NUMPIXELS; i++) { // For each pixel...

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(redVal, greenVal, blueVal));
    pixels.show();   // Send the updated pixel colors to the hardware.
  }

  // a little delay to not hog Serial Monitor
  delay(1000);
  
}

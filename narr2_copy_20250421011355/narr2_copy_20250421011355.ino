#include <Wire.h>
#include <SPI.h>
#include <Adafruit_CAP1188.h>


//     Organic theramin   ---------   gblekkenhorst for experiences & interfaces 20254
//     Capp code credit from Examples/ Adafruit CAP1188 Library / cap1188test
//     Light sensor calibration from Nick Puckett https://github.com/DigitalFuturesOCADU/CC2024/blob/main/experiment4/Arduino/Sensors/Light/ligh_raw_smoothed/ligh_raw_smoothed.ino





float knockvolume;
bool isClosed = false;
float openThreshold = 600;
float closeThreshold = 0;

int allOn;
int isOpen;



const int lightAverageWindow = 10; // Number of samples to average
int lightValue = 0;          // Raw value
int smoothedLightValue = 0;  // Filtered value
int startupLightValue = 0;   // Calibration value from startup

// Rolling average variables
int lightReadings[lightAverageWindow];
int lightReadIndex = 0;
long lightTotalValue = 0;


// Light sensor.
#define PHOTO_PIN 1

// Reset Pin is used for I2C or SPI
#define CAP1188_RESET  9

// CS pin is used for software or hardware SPI
#define CAP1188_CS  10

// These are defined for software SPI, for hardware SPI, check your 
// board's SPI pins in the Arduino documentation
#define CAP1188_MOSI  11
#define CAP1188_MISO  12
#define CAP1188_CLK  13




Adafruit_CAP1188 cap = Adafruit_CAP1188();


void setup() {
  // distance, light, button 
  Serial.begin(9600);
  delay(150);  // Delay to brute force issue with printing a bunch of rectangles and crashing. 
  Serial.println("Starting..."); 

  
  // Init light sensor.
  pinMode(PHOTO_PIN, INPUT_PULLUP);
  calibrateSensor();

  // Init Capacitive sensor.
    if (!cap.begin()) {
    Serial.println("CAP1188 not found");
    while (1);
  }
  Serial.println("CAP1188 found!");
  
}

void loop() {

// Get light sensor data and convert it into something Touch can use for volume.
knockvolume = map(analogRead(PHOTO_PIN),40,startupLightValue,100,0);

knockvolume = knockvolume/100;
knockvolume = constrain(knockvolume,0,1);

// Create 1/0 bools about box to send to Touch - I realize this is silly but I could not figure out how to do an IF NOT statement in Touch. 
 if(analogRead(PHOTO_PIN)> openThreshold)
  {isClosed = true;
  isOpen = false;
  }

else{
  isClosed = false;
  isOpen = true;
    knockvolume =1; // constraints knock volume from peaking. 
  }

  // Cuts out knock volume completely if closed.
 if(analogRead(PHOTO_PIN)< closeThreshold)
  knockvolume =0;



capp1();

// Print the previously calculated 1/0 bools. 
Serial.print(isClosed);
Serial.print(",");

Serial.print(isOpen);
Serial.print(",");

Serial.print(knockvolume);
Serial.print(",");

Serial.print(analogRead(PHOTO_PIN));
Serial.println();
delay(50);
}


// Called from loop() to read capacitive sensor. 
void capp1() {
  uint8_t touched = cap.touched();

  int howmany = 0;

  // Check if each cap sensors is active, and print results as 1/0
  for (uint8_t i=0; i<4; i++) {
    if (touched & (1 << i)) {

      Serial.print("1,");
      howmany++;

    } else {
      Serial.print("0,");
    }
    
  }
 

  // Check if all cap sensors are active, and print results as 1/0
  if(howmany ==4)
    allOn = 1;
  else
    allOn = 0;

    Serial.print(allOn);
  Serial.print(",");


  delay(50);
}

// Calibrate the light sensor.
void calibrateSensor() {

  // Take multiple readings and average them for startup value
  long total = 0;
  for (int i = 0; i < lightAverageWindow; i++) {
    total += analogRead(PHOTO_PIN);
    delay(50); // Short delay between readings
  }
  startupLightValue = total / lightAverageWindow;
}
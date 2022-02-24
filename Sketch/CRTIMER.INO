/********************************************************************************************************************************************************
*                                                                                                                                                       *
   Project:         CR Timer
   Target Platform: ARduino Nano
*                                                                                                                                                       *
   Version: 1.0
   Hardware setup: See github

*                                                                                                                                                       *
   Mark Donners
   The Electronic Engineer
   Website:   www.theelectronicengineer.nl
   facebook:  https://www.facebook.com/TheelectronicEngineer
   youtube:   https://www.youtube.com/channel/UCm5wy-2RoXGjG2F9wpDFF3w
   github:    https://github.com/donnersm
*                                                                                                                                                       *
*********************************************************************************************************************************************************
  Version History
  1.0
*/

//Libraries and stuff
#include <SD.h>                      // need to include the SD library

//TMRpcm PCM/WAV audio playback V1.2.3
//https://github.com/TMRh20/TMRpcm/wiki
#include <TMRpcm.h>                   

//EasyButton by Evert Arias Version 2.01
//https://easybtn.earias.me/
#include <EasyButton.h>

//Adafruit NeoPixel V1.7.0
//https://github.com/adafruit/Adafruit_NeoPixel
#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif


// Buttons
#define BUTTON_PIN 6 //Define d6 as button.
EasyButton button(BUTTON_PIN, 35, false, false); // Instance of the button.

// SD CARD
#define SD_ChipSelectPin 8  //using digital pin 4 on arduino nano 328

// Led String Neo Pixels
#define PIN        7 // Which pin on the Arduino is connected to the NeoPixels?
#define NUMPIXELS 5 //  How many NeoPixels are attached to the Arduino?
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//Speaker
#define Volume  5   // 1 to 7 but 6 and 7 doesn't give clear sound
TMRpcm tmrpcm;   // create an object for use in this sketch

// Interval Time
#define DELAYVAL 180000 //120000 // mSeconds it takes for a alertlevel to go down automatically to previous level

// some variables no need to change
int AlertLevel = 0;
int MaxLevel = 4; // this is the highest level ( 0  to 4)
long int t1 = millis();



void setup() {
  // First define pins for IO and activate the switch override
  pinMode(5, OUTPUT); // D5 is used to drive an external mosfet to override the on switch
  pinMode(6, INPUT); // D6 is used to read back the status of that switch
  digitalWrite(5, HIGH);   // activate power override so user can let go of button

  Serial.begin(9600); // Set serial port baudrate

  // Initialize SD card for reading
  if (!SD.begin(SD_ChipSelectPin)) {  // see if the card is present and can be initialized:
    Serial.println("SD fail, All sound playback is disables");
  }
  else Serial.println("SD ok");


  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  Activate_LED(0); // lednumber, r,g,b
  pixels.show();   // Send the updated pixel colors to the hardware.

  // initialize speaker and play welcome sound
  tmrpcm.speakerPin = 9; // The speaker output pin
  tmrpcm.setVolume(Volume);
  tmrpcm.play("0.wav"); //the sound file "1" will play each time the arduino powers up, or is reset

}

void loop() {
  delay(2000);
  // Initialize the button.
  button.begin();


  button.onPressed(onPressed); // Add the callback function to be called when the button is pressed.
  button.onPressedFor(3000, UnitOff); // call function to switch off unit if pressed for 3 seconds
  while (1) {
    button.read(); // Continuously read the status of the button.

    if ((long int)millis() - t1 > DELAYVAL) {

      t1 = millis();
      if (AlertLevel > 0) {
        AlertLevel--;
        Activate_LED(AlertLevel);
        if (AlertLevel == 3) {}
        else if (AlertLevel == 2) {}
        else if (AlertLevel == 1) {
          tmrpcm.play("1b.wav");
        }
        else if (AlertLevel == 0) {
          tmrpcm.play("0b.wav");
        }
      }
      else digitalWrite(5, LOW); // switch off unit
    }


  }
}
// Callback function to be called when the button is pressed.
void onPressed() {
  t1 = millis();
  if (AlertLevel < MaxLevel)AlertLevel++;
  Activate_LED(AlertLevel);
  if (AlertLevel == 1) {
    tmrpcm.play("1.wav");
  }
  else if (AlertLevel == 2) {
    tmrpcm.play("2.wav");
  }
  else if (AlertLevel == 3) {
    tmrpcm.play("3.wav");
  }
  else if (AlertLevel == 4) {
    tmrpcm.play("4.wav");
  }
}

void Activate_LED(int LED) {
  pixels.clear();
  if (LED == 0) {
    pixels.setPixelColor(LED, pixels.Color(0, 255, 0));
  }
  else if (LED == 1) {
    pixels.setPixelColor(LED, pixels.Color(255, 255, 0));
  }
  else if (LED == 2) {
    pixels.setPixelColor(LED, pixels.Color(204, 102, 0));
  }
  else if (LED == 3) {
    pixels.setPixelColor(LED, pixels.Color(255, 0, 255));
  }
  else {
    pixels.setPixelColor(LED, pixels.Color(255, 0, 0));
  }

  pixels.setBrightness(30);  // set the overall brightness of the leds
  pixels.show();

}

void UnitOff() {
  tmrpcm.disable();
  digitalWrite(5, LOW);
}

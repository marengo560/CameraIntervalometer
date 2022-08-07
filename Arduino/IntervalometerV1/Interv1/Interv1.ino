/*
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x32 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 */

#include "settings.h"
#include "icons.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Encoder.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels

#define OLED_RESET 4         // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES 4  // Number of snowflakes in the animation example

//4 "X" positions for icons, pos[0] is centered, and and the rest are [1 2 3 4]
int posX[] = { 55, 3, 36, 69, 102 };
int posY = 6;

//Option selection
int option = 1;

/*Menu
0-> Initial menu with start, restart, settings, battery, 
1-> Screen while running
2-> Settings screen with Time of capture, time of final video, FPS and return to main button
21->Set Time of capture
22->Set Time of final video
23->Set FPS
*/
int menu = 0;

//Rotary encoder
Encoder rotary(PIN_ROTA, PIN_ROTB);
int rotaryPosition = 1;  //Last position to keep track of changes


bool pressed = false;
uint32_t swDebounce = 0L;

long timeRecord = 0;
long timeVideo = 0;
long fps = 0;
long delayPhoto = 0;

long i = 0;
long nphotos = 0;
int relay = 14;  


void setup() {
  
  Serial.begin(9600);
  pinMode(relay, OUTPUT);
  pinMode(PIN_SW1,INPUT_PULLUP);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    //Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  // Clear the buffer
  display.setRotation(2);
  display.clearDisplay();
  delay(100);
}

void loop() {
  calcDelay();
  checkKeysAndReportChanges();
  //int rotaryPosition = checkRotaryEncoderAndReportChanges();
  testdrawbitmap();  // Draw a small bitmap image
  
  changeMenu(option, pressed);
  // delay(50);
}


void testdrawbitmap() {

  int rotaryNew = rotary.read();
  if (rotaryNew != rotaryPosition) {
    rotaryPosition = rotaryNew / 4;
    eraseLine();
    if ((rotaryPosition < 1) || (rotaryPosition > 4)) {
      rotary.write(1);
      rotaryPosition = 1;
    }

    switch (rotaryPosition) {

      case 1:
        option = 1;


        display.drawLine(posX[1], 31, posX[1] + 25, 31, WHITE);

        //display.display();
        break;

      case 2:
        option = 2;
        //eraseLine();
        display.drawLine(posX[2], 31, posX[2] + 25, 31, WHITE);

        //display.display();
        break;

      case 3:
        option = 3;
        //eraseLine();
        display.drawLine(posX[3], 31, posX[3] + 25, 31, WHITE);


        break;

      case 4:
        option = 4;
        //eraseLine();
        display.drawLine(posX[4], 31, posX[4] + 25, 31, WHITE);

        break;

      default:
        option = 1;

        //eraseLine();
        display.drawLine(posX[1], 31, posX[1] + 25, 31, WHITE);

        //display.display();
        break;
    }
    display.display();
    delay(150);
  }
}

void changeMenu(int opt, bool butPress) {
  int butflag = 0;
  if (butflag == 0) {
    if ((butPress == true) && (opt == 3) && (menu == 0)) {
      butflag = 1;
      menu = 3;
    } else if ((butPress == true) && (opt == 1) && (menu == 0)) {
      butflag = 1;
      menu = 11;
    } else if ((butPress == true) && (opt == 2) && (menu == 0)) {
      butflag = 1;
      menu = 12;
    } else if ((butPress == true) && (opt == 1) && (menu == 3)) {
      butflag = 1;
      menu = 31;
    } else if ((butPress == true) && (opt == 2) && (menu == 3)) {
      butflag = 1;
      menu = 32;
    } else if ((butPress == true) && (opt == 3) && (menu == 3)) {
      butflag = 1;
      menu = 33;
    } else if ((butPress == true) && (opt == 4) && (menu == 3)) {
      butflag = 1;
      menu = 0;
    }
  }

  switch (menu) {

    case 0:
      display.clearDisplay();

      display.drawBitmap(posX[1], posY, play_ico, sizeIco[0], sizeIco[0], WHITE);
      display.drawBitmap(posX[2], posY, repeat, sizeIco[0], sizeIco[0], WHITE);
      display.drawBitmap(posX[3], posY, settings_ico, sizeIco[0], sizeIco[0], WHITE);
      display.drawBitmap(posX[4], posY, return_ico, sizeIco[0], sizeIco[0], WHITE);
      display.display();


      butflag = 0;
      break;

    case 3:
      display.clearDisplay();

      display.drawBitmap(posX[1], posY, timerecord_ico, sizeIco[0], sizeIco[0], WHITE);
      display.drawBitmap(posX[2], posY, timevideo_ico, sizeIco[0], sizeIco[0], WHITE);
      display.drawBitmap(posX[3], posY, fps_ico, sizeIco[0], sizeIco[0], WHITE);
      display.drawBitmap(posX[4], posY, return_ico, sizeIco[0], sizeIco[0], WHITE);
      display.display();
      butflag = 0;
      break;
      
    case 11:
      i=0;
      
      Serial.println(delayPhoto);
      //Serial.println(timeRecord);
     if(nphotos > 0){       
     // Serial.println(j);
      while ((i < nphotos) && (digitalRead(PIN_SW1) == HIGH)) {

        display.clearDisplay();

        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.println(F("Pics left:"));
        display.setCursor(75, 0);
        display.print(nphotos-i);
        display.setCursor(0, 8);
        display.println(F("Time left:"));
        display.setCursor(75, 8);
        display.print(nphotos-i);
        display.drawBitmap(55, posY+5, camera_ico, sizeIco[0], sizeIco[0], WHITE);
        digitalWrite(relay,1);       
        delay(delayPhoto / 2);
        display.display();
        display.drawBitmap(55, posY+5, camera_ico, sizeIco[0], sizeIco[0], BLACK);
        digitalWrite(relay,0); 
        delay(delayPhoto / 2);
        display.display();
        i++;
      }
delay(1000);      
      } 
else{
        display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println(F("Invalid inputs check settings!"));
display.display();
}      menu = 0;
      butflag = 0;
      break;

      case 12:
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(posX[1], 12);
        display.println(F("Capt:"));
        display.setCursor(posX[1], 24);
        display.println(timeRecord);
        display.setCursor(posX[2], 12);
        display.println(F("Vid:"));
        display.setCursor(posX[2], 24);
        display.println(timeVideo);
        display.setCursor(posX[3], 12);
        display.println(F("FPS:"));
        display.setCursor(posX[3], 24);
        display.println(fps);
        display.setCursor(posX[4], 12);
        display.println(F("Phs:"));
        display.setCursor(posX[4], 24);
        display.println(nphotos);
        display.display();
        delay(2000);
        menu = 0;
        butflag = 0;
      break;
      
    case 31:
      while (digitalRead(PIN_SW1) == HIGH) {


        display.clearDisplay();
        display.drawBitmap(posX[1], posY, timerecord_ico, sizeIco[0], sizeIco[0], WHITE);
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(posX[2], 12);
        timeRecord = rotary.read() / 4;
        display.println(timeRecord);
              display.setCursor(posX[3]+10, 12);
              display.println(F("(m)"));
        display.display();
      }
      delay(300);
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(posX[1], 12);
      display.println(F("Capture time set to: "));
      display.setCursor(posX[0], 24);
      display.println(timeRecord);
      display.display();
      delay(1000);
      //rotary.write(1);
      menu = 3;
      butflag = 0;
      break;



    case 32:
      while (digitalRead(PIN_SW1) == HIGH) {
        display.clearDisplay();
        display.drawBitmap(posX[1], posY, timevideo_ico, sizeIco[0], sizeIco[0], WHITE);
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(posX[2], 12);
        timeVideo = rotary.read() / 4;
        display.println(timeVideo);
        display.setCursor(posX[3]+10, 12);
        display.println(F("(s)"));
        display.display();
      }
      delay(300);
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(posX[1], 12);
      display.println(F("Video time set to: "));
      display.setCursor(posX[0], 24);
      display.println(timeVideo);
      display.display();
      delay(1000);
      //rotary.write(1);
      menu = 3;
      butflag = 0;
      break;

    case 33:
      while (digitalRead(PIN_SW1) == HIGH) {
        display.clearDisplay();
        display.drawBitmap(posX[1], posY, fps_ico, sizeIco[0], sizeIco[0], WHITE);
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(posX[2], 12);
        fps = rotary.read() / 4;
        display.println(fps);
        display.display();
      }
      delay(300);
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(posX[1], 12);
      display.println(F("fps set to: "));
      display.setCursor(posX[0], 24);
      display.println(fps);
      display.display();
      delay(1000);
      //rotary.write(1);
      menu = 3;
      butflag = 0;
      break;

 
  }

}


void eraseLine() {
  display.drawLine(0, 31, 128, 31, BLACK);
  display.display();
}

//Called when state of key has changed. Checks debounce time
//and returns false if event should be filtered. Otherwise it


//Check if keys have changed and report any changes
void checkKeysAndReportChanges() {
  int state = digitalRead(PIN_SW1);
  if (state == LOW) {
    pressed = true;

  } else if (state == HIGH) {
    pressed = false;
  }
}

void calcDelay() {
nphotos = 0;
nphotos = fps * timeVideo * 60;
delayPhoto = (timeRecord * 60)/nphotos;
delayPhoto = delayPhoto * 1000;
}




//               display.setTextSize(1);
// display.setTextColor(SSD1306_WHITE);
// display.setCursor(24,0);
// display.println(F("Set final time"));

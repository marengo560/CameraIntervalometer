/**************************************************************************
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
 **************************************************************************/

#include "settings.h"
#include "icons.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Encoder.h>     

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     4 // Number of snowflakes in the animation example

//4 "X" positions for icons, pos[0] is centered, and and the rest are [1 2 3 4]
int posX[] = {55,3,36,69,102};
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


void setup() {

  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    //Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();
  delay(100);
}

void loop() {
    checkKeysAndReportChanges();
    //int rotaryPosition = checkRotaryEncoderAndReportChanges();
    testdrawbitmap();    // Draw a small bitmap image
    changeMenu(option,pressed);
   // delay(50);
}


void testdrawbitmap() {

  int rotaryNew = rotary.read();
  if (rotaryNew != rotaryPosition) {
    rotaryPosition = rotaryNew/4;
    if((rotaryPosition < 1)||(rotaryPosition >4)){
      rotary.write(1);
      rotaryPosition=1;
} 


          


  // display.setTextSize(1);            
  // display.setTextColor(SSD1306_WHITE);       
  // display.setCursor(24,0);             
  // display.println(F("Settings"));
  // display.setCursor(75,0);            
  // display.println(rotaryPosition);
    switch (rotaryPosition){
      
      case 1:
        option = 1;
        
        eraseLine();
        display.drawLine(posX[1], 31, posX[1]+25,31, WHITE); 

        //display.display();
      break;

      case 2:
        option = 2;
        eraseLine();
        display.drawLine(posX[2], 31, posX[2]+25,31, WHITE); 

        //display.display();
      break;

      case 3:
        option = 3;
                eraseLine();
        display.drawLine(posX[3], 31, posX[3]+25,31, WHITE); 


      break;
      
      case 4:
        option = 4;
        eraseLine();
        display.drawLine(posX[4], 31, posX[4]+25,31, WHITE); 

      break;  

      
    }
    display.display();    
  }

  
    
}

void changeMenu(int opt , bool butPress){
  int butflag = 0;
if(butflag==0){  
  if((butPress == true) && (opt == 3) && (menu == 0)){
    butflag=1;
    menu=3;
  }
  else if((butPress == true) && (opt == 1) && (menu == 3)){
    butflag=1;
    menu=31;
  }
  else if((butPress == true) && (opt == 2) && (menu == 3)){
    butflag=1;
    menu=32;
  }
}

  switch (menu){

    case 0:
      display.clearDisplay();
      display.drawBitmap(posX[1], posY, play_ico, sizeIco[0], sizeIco[0], WHITE);
      display.drawBitmap(posX[2], posY, pause_ico, sizeIco[0], sizeIco[0], WHITE);
      display.drawBitmap(posX[3], posY, settings_ico, sizeIco[0], sizeIco[0], WHITE);
      display.drawBitmap(posX[4], posY, return_ico, sizeIco[0], sizeIco[0], WHITE);
      display.display();
      butflag=0; 
    break;  
        
    case 1:
    break;

    case 3:
      display.clearDisplay();
      display.drawBitmap(posX[1], posY, timerecord_ico, sizeIco[0], sizeIco[0], WHITE);
      display.drawBitmap(posX[2], posY, timevideo_ico, sizeIco[0], sizeIco[0], WHITE);
      display.drawBitmap(posX[3], posY, fps_ico, sizeIco[0], sizeIco[0], WHITE);
      display.drawBitmap(posX[4], posY, return_ico, sizeIco[0], sizeIco[0], WHITE);
      display.display(); 
      butflag=0; 
    break;

    case 31:

      display.clearDisplay();
      display.drawBitmap(posX[1], posY, timerecord_ico, sizeIco[0], sizeIco[0], WHITE);
      display.display(); 
      if(pressed == true){
        menu = 3;
      }      
      butflag=0; 
    break;

    case 32:
      display.clearDisplay();
      display.drawBitmap(posX[1], posY, timevideo_ico, sizeIco[0], sizeIco[0], WHITE);
      display.display(); 
      butflag=0; 
    break;  
    
    case 33:
      display.clearDisplay();
      display.drawBitmap(posX[1], posY, fps_ico, sizeIco[0], sizeIco[0], WHITE);
      display.display(); 
      butflag=0; 
    break;      
  }
 
}


void eraseLine(){
  display.drawLine(0, 31, 128,31, BLACK);
  display.display();
}

//Called when state of key has changed. Checks debounce time
//and returns false if event should be filtered. Otherwise it


//Check if keys have changed and report any changes
void checkKeysAndReportChanges() {
    int state = digitalRead(PIN_SW1);
    if (state == LOW) {
          pressed = true;
     
    } 
    else if (state == HIGH) {
        pressed = false;
   }
}








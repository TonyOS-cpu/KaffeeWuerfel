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

 //AKTUELL auf dem ATMEGA zum testen
 // OLED DISPLAY FUER die kaffeemaschine MIT 2 Taster

#include <Time.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
//#include <DS3231.h>


RTC_DS1307 RTC; 

const int buttonPin = 7; //switchPin
int buttonState = 0; //switch status

//relais:
const int powerPin =  10;      // the number of the power pin
const int cupPin =  11;      // the number of the cup pin



#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels


//RTClib RTC;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16


const int TasterDOWN = 8;
const int TasterUP = 9;

  unsigned long time;

  int editMode = 0;
  int TempI = 24;
  int Timer1I =5*60; //stunden * 60
  int Timer2I = 7*60;   // stunden * 60
  int Timer1Zustand = 0;
  int Timer2Zustand = 0;
  int kaffeeNr = 0;
  int kaffeeStep = 0;
  int kaffeeProzess = 0;

  int zeitBruehen = 5*1000;   //sekunden * 1000 standard 90 sek
  int zeitTasse = 15*1000;    //sekunden * 1000 standard 90 sek
  int zeitFinish = 15*1000;   //sekunden * 1000 standard 90 sek
  
  String Temp;
  String Timer1Zeit ;
  String Timer2Zeit ;

  int relaisPower = 0;
  int relaisCup1 = 0;
  int relaisCup2 = 0;
  
  int mode = 0;
  int menue1 = 0; //up
  int menue2 = 0; //down

//funktionen:________________________________________

//anzeige minuten/stunden aus minuten(0-1439)
String minuten(int minutenTag,char c){

  String zusatzM = "";
  String zusatzS = "";

   int stunden = minutenTag/60;
   int minuten = minutenTag%60;
   
 if(stunden < 10){ zusatzS = "0";};
 if(minuten < 10){ zusatzM = "0";};


 String ausgabeS = zusatzS+String(stunden);
 String ausgabeM = zusatzM+String(minuten);
   if( c == 's'){return ausgabeS;}
   if( c == 'm'){return ausgabeM;}
}
String doppelnull(int Zahl){
    if(Zahl == 9) {return "0";} else {return "";}
}

  
//setup:_________________________________________________ 
void setup() {
  Serial.begin(9600);
    Wire.begin();
    pinMode(buttonPin, INPUT); //switchInput
    pinMode(cupPin, OUTPUT);
  pinMode(powerPin, OUTPUT);
      digitalWrite(powerPin, HIGH);
          digitalWrite(cupPin, HIGH);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  pinMode(TasterDOWN, INPUT); // setzt 'pin' als Eingang
  digitalWrite(TasterDOWN, HIGH); //Pullup einschalten
  delay(1000); 
  pinMode(TasterUP, INPUT); // setzt 'pin' als Eingang
  digitalWrite(TasterUP, HIGH); //Pullup einschalten
  delay(1000); 

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  //display.drawPixel(10, 10, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  //delay(2000);
  // display.display() is NOT necessary after every single drawing command,
  // unless that's what you want...rather, you can batch up a bunch of
  // drawing operations and then update the screen all at once by calling
  // display.display(). These examples demonstrate both approaches...

}
//loop:______________________________________________________________________________
void loop() {
      display.clearDisplay();
    DateTime now = RTC.now();
    byte stundenX = now.hour();
    byte minutenX = now.minute();
    byte sekundenX = now.second();

//    stundenT = now.toString("hh");
//    minutenT = now.toString("mm");
//   sekundenT = now.toString("ss");

    if(Timer1I/60 == stundenX && Timer1I%60 == minutenX && Timer1Zustand > 0){
      kaffeeNr = 1;
      }     // Timernummer
    if(Timer2I/60 == stundenX && Timer2I%60 == minutenX && Timer2Zustand > 0){
      kaffeeNr = 2;
      }    // Timernummer
    
   if(sekundenX < 4 && kaffeeProzess == 0 && kaffeeNr > 0){
    time = millis();
    relaisPower = 1;
    relaisCup1 = 1;
    relaisCup2 = 1;
    kaffeeProzess = 1; //wecker in arbeit
    kaffeeStep = 1;   //prozessphase
    mode = 4;         //displayanzeige 4

    if(relaisPower == 1){
    digitalWrite(powerPin, LOW); //Kaffeemaschine einschalten
    delay(800);
    digitalWrite(powerPin, HIGH);
    relaisPower = 0;
    }

   }
   
    if(time <= (millis()-zeitBruehen) && kaffeeProzess == 1) {

    if(kaffeeNr == 1){
    if(Timer1Zustand == 1){kaffeeStep = 2;};
    if(Timer1Zustand == 2){kaffeeStep = 3;};
    }else{
    if(Timer2Zustand == 1){kaffeeStep = 2;};
    if(Timer2Zustand == 2){kaffeeStep = 3;};  
    }
    
    if(relaisCup1 == 1){
    digitalWrite(cupPin, LOW); //Knopf für eine Tasse drücken
    delay(800);
    digitalWrite(cupPin, HIGH);
    relaisCup1 = 0;
    }
    }
    
    if(time <= (millis()-zeitBruehen-zeitTasse) && kaffeeProzess == 1) {
    
    if(kaffeeNr == 1){
    if(Timer1Zustand == 2){
    kaffeeStep = 4;
    if(relaisCup2 == 1){
    digitalWrite(cupPin, LOW); //Knopf für eine Tasse drücken
    delay(800);
    digitalWrite(cupPin, HIGH);
    relaisCup2 = 0;
    }
    }else{
    kaffeeStep = 5;
    relaisCup2 = 0;
    };
    }else{
    if(Timer2Zustand == 2){
    kaffeeStep = 4;
    if(relaisCup2 == 1){
    digitalWrite(cupPin, LOW); //Knopf für eine Tasse drücken
    delay(800);
    digitalWrite(cupPin, HIGH);
    relaisCup2 = 0;
    }
    }else{
    kaffeeStep = 5;
    relaisCup2 = 0;
    };    
    }
    }
    
    if(time <= (millis()-zeitBruehen-zeitTasse-zeitTasse) && kaffeeProzess == 1){
    if(kaffeeNr == 1){
    if(Timer1Zustand == 2){kaffeeStep = 5;
    }else{
    kaffeeProzess = 0;
    Timer1Zustand = 0;
    kaffeeStep = 0;
    kaffeeNr = 0;
    mode = 0;
    };
    }else{
    if(Timer2Zustand == 2){kaffeeStep = 5;
    }else{
    kaffeeProzess = 0;
    Timer2Zustand = 0;
    kaffeeStep = 0;
    kaffeeNr = 0;
    mode = 0;
    };    
    }
    }

    if(time <= (millis()-zeitBruehen-zeitTasse-zeitTasse-zeitFinish) && kaffeeProzess == 1){

    if(kaffeeNr == 1){Timer1Zustand = 0;}
    if(kaffeeNr == 2){Timer2Zustand = 0;}
    kaffeeProzess = 0;
    kaffeeStep = 0;
    kaffeeNr = 0;
    mode = 0;
    
   }

    
    buttonState = digitalRead(buttonPin);
    if (buttonState == HIGH) {editMode = 1;}
    if (buttonState == LOW) {editMode = 0;}
    
 if(editMode == 1){  
  if (digitalRead(TasterUP) == HIGH && menue1 == 0 && menue2 == 0 ){
    menue1 = 1;
    mode = 1;
    time = millis();
}
  if (digitalRead(TasterUP) == HIGH && menue1 == 1){
    if(Timer1I < 1440){Timer1I = Timer1I +1;}else{Timer1I =0;}
    time = millis();
}
  if (digitalRead(TasterUP) == HIGH && menue2 == 1){
    if((Timer2I)<1440){Timer2I = Timer2I+1;}else{Timer2I = 0;}
    time = millis();
}
  if (digitalRead(TasterDOWN) == HIGH && menue1 == 0 && menue2 == 0 ){
    menue2 = 1;
    mode = 2;
    time = millis();
  }
  if (digitalRead(TasterDOWN) == HIGH && menue1 == 1){
    if(Timer1I>0){Timer1I = Timer1I-1;}else{Timer1I = 1439;}
    time = millis();
}
  if (digitalRead(TasterDOWN) == HIGH && menue2 == 1){
    if(Timer2I>0){Timer2I = Timer2I-1;}else{Timer2I = 1439;}
    time = millis();
  }
if(time <= (millis()-5000) && mode!=0){
      menue1 = 0;
      menue2 = 0;
    mode = 0;
}
 }else{// relai aktivierungen anzahl in timerZustand1 bzw 2
  if (digitalRead(TasterUP) == HIGH){
 if(Timer1Zustand < 2){Timer1Zustand =Timer1Zustand+1;}else{Timer1Zustand = 0;}
  };
  if (digitalRead(TasterDOWN) == HIGH){
 if(Timer2Zustand < 2){Timer2Zustand =Timer2Zustand+1;}else{Timer2Zustand = 0;}
  } 
 }
  

    display.clearDisplay();
//    String minuten = (now.minute, DEC)(INT);
//Temp = String(minuten);

//Timer1Zeit = String(doppelnull(minuten(Timer1I,'s')))+String(minuten(Timer1I,'s'))+String(":")+String(doppelnull(minuten(Timer1I,'m')))+String(minuten(Timer1I,'m'));
//Timer2Zeit = String(doppelnull(minuten(Timer1I,'s')))+String(minuten(Timer2I,'s'))+String(":")+String(doppelnull(minuten(Timer1I,'m')))+String(minuten(Timer2I,'m'));

Timer1Zeit = String(minuten(Timer1I,'s'))+String(":")+String(minuten(Timer1I,'m'));
Timer2Zeit = String(minuten(Timer2I,'s'))+String(":")+String(minuten(Timer2I,'m'));

//if(einI == 42) ein = "ROBIN";
//if(ausI == 42) aus = "ROBIN"; 

    switch (mode) {
  case 0:
  //edit mode: 
  
  //aktuelle Uhrzeit
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
 if(stundenX<10){display.print(F("0")); };
  display.print(stundenX);
  display.print(':');
  
 if(minutenX<10){display.print(F("0")); };
 display.print(minutenX);
 display.print(':');
 
   if(sekundenX<10){display.print(F("0")); };
 display.print(sekundenX);

if(sekundenX == 16) {display.print(F("--")); }

   //Timer 1
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 16);
  display.print(F("Kaffee1: "));
  display.print(Timer1Zeit);
  display.print(F(" "));
  display.setCursor(90, 16);
  if(editMode == 1){display.println(F(" EDIT"));}
  if(editMode == 0 && Timer1Zustand >0){
    display.print(Timer1Zustand);
    display.println(F("x"));
    };
  
  //Timer 2
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 25);
  display.print(F("Kaffee2: "));
  display.print(Timer2Zeit);
  display.print(F(" "));
  display.setCursor(90, 25);
  if(editMode == 1){display.println(F(" MODE"));}
  if(editMode == 0 && Timer2Zustand >0){
    display.print(Timer2Zustand);
    display.println(F("x"));
    };;
  
  display.display();      // Show initial text
  //99+mode = 1;

    break;
    
  case 1:
  //only High Temperatur
          //Heizung AUS bei:
    display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 0);
  display.println(F("Timer1:"));
  display.setTextSize(2);
  display.setCursor(20, 15);
  display.print(Timer1Zeit);
  display.println(F(" "));

  display.display();      // Show initial text
  //mode = 2;
    break;
    
   case 2:
   //only LOW Temperatur
      //Heizung Ein bei:
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 0);
  display.println(F("Timer2:"));
  display.setTextSize(2);
  display.setCursor(20, 15);
  display.print(Timer2Zeit);
  display.println(F(" "));
  
  display.display();      // Show initial text
  //mode = 0;
  break;

  case 3:
   //aktivate mode
  
  //aktuelle Uhrzeit
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 0);
//  if(now.hour() < 10 ){display.print("0");}
  display.print(now.hour(), DEC);
  display.print(':');
 //  if(now.minute() < 10 ){display.print("0");}
  display.print(now.minute(), DEC);
  display.print(':');
  display.print(now.second(), DEC);
//  display.println(F(" Grad"));

   //Timer 1
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 16);
  display.print(F("Kaffee1: "));
  display.print(Timer1Zeit);
  display.println(F(" "));
  
  //Timer 2
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 25);
    display.print(F("Kaffee2: "));
  display.print(Timer2Zeit);
  display.println(F(" "));
  
  display.display();      // Show initial text
  //99+mode = 1;
    break;

       case 4: //Kaffee kochen
      //aktuelle Uhrzeit
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
 if(stundenX<10){display.print(F("0")); };
  display.print(stundenX);
  display.print(':');
  
 if(minutenX<10){display.print(F("0")); };
 display.print(minutenX);
 display.print(':');
 
   if(sekundenX<10){display.print(F("0")); };
 display.print(sekundenX);

//if(sekundenX == 16) {display.print(F("--")); }
display.setTextSize(1); // Draw 2X-scale text
  display.setCursor(0, 16);
  if(kaffeeNr == 1){display.print(F("Kaffee1.. "));}
  if(kaffeeNr == 2){display.print(F("Kaffee2.. "));}
    display.setCursor(0, 25);
    if(kaffeeStep == 1){display.print(F("Wasser aufkochen.."));}
    if(kaffeeStep == 2){display.print(F("Tasse 1/1 ..."));}
    if(kaffeeStep == 3){display.print(F("Tasse 1/2 ..."));}
    if(kaffeeStep == 4){display.print(F("Tasse 2/2 ..."));}
    if(kaffeeStep == 5){display.print(F("Kaffee ist fertig :)"));}
  display.display();
    break;

    
    }
  display.display();  
 delay(150);
}

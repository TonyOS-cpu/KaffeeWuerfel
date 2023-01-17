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


 // OLED DISPLAY FUER die kaffeemaschine MIT 2 Taster
 //Arduino KafeeCube V1.01
 // Komponenten:
 //             Elegoo Uno
 //             Relais 2er
 //             2x Taster
 //             Switch
 //             LCD-Display 32x128
 //             Ultraschallsensor
 //             RTC-Modul (RealTimeClock)

 //

#include <Time.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
//#include <DS3231.h>


//rtc kalibrieren: beispiele - RTClib - ds3231

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
  unsigned long timeUS;
  unsigned long timeCup;

  int editMode = 0;
  int TempI = 24;
  int Timer1I =4*60+50; //stunden * 60 + minuten
  int Timer2I = 6*60+50;   // stunden * 60 + minuten
  int Timer1Zustand = 0;
  int Timer2Zustand = 0;
  int kaffeeNr = 0;
  int kaffeeStep = 0;
  int kaffeeProzess = 0;
  int kaffeeStepX = 0;

  unsigned long zeitBruehen = 90000;   //sekunden * 1000 standard 90 sek
  unsigned long zeitTasse = 90000;    //sekunden * 1000 standard 90 sek
  unsigned long zeitFinish = 90000;   //sekunden * 1000 standard 90 sek
  
  String Temp;
  String Timer1Zeit ;
  String Timer2Zeit ;

  int relaisPower = 0;
  int relaisCup1 = 0;
  int relaisCup2 = 0;
  
  int mode = 0;
  int menue1 = 0; //up
  int menue2 = 0; //down

//0 24 29

  //ultraschallsensor
int trigger=5; //Trigger-Pin des Ultraschallsensors an Pin7 des Arduino-Boards 
int echo=6; // Echo-Pim des Ultraschallsensors an Pin6 des Arduino-Boards 
long dauer=0; // Das Wort dauer ist jetzt eine Variable, unter der die Zeit gespeichert wird, die eine Schallwelle bis zur Reflektion und zurück benötigt. Startwert ist hier 0.
long entfernung=0; // Das Wort „entfernung“ ist jetzt die variable, unter der die berechnete Entfernung gespeichert wird. Info: Anstelle von „int“ steht hier vor den beiden Variablen „long“. Das hat den Vorteil, dass eine größere Zahl gespeichert werden kann. Nachteil: Die Variable benötigt mehr Platz im Speicher.
byte  UScup = 0; //Tasse vorhanden: 0 nein, 1 ja

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
  
//ultraschallsensor
pinMode(trigger, OUTPUT); // Trigger-Pin ist ein Ausgang
pinMode(echo, INPUT); // Echo-Pin ist ein Eingang

}
//loop:______________________________________________________________________________
void loop() {
      display.clearDisplay();
    DateTime now = RTC.now();
    byte stundenX = now.hour();
    byte minutenX = now.minute();
    byte sekundenX = now.second();
    
//___________________________________________________________________________________
if(timeUS <= (millis()-2000)){
timeUS = millis();
digitalWrite(trigger, LOW); //Hier nimmt man die Spannung für kurze Zeit vom Trigger-Pin, damit man später beim senden des Trigger-Signals ein rauschfreies Signal hat.
delay(5); //Dauer: 5 Millisekunden
digitalWrite(trigger, HIGH); //Jetzt sendet man eine Ultraschallwelle los.
delay(10); //Dieser „Ton“ erklingt für 10 Millisekunden.
digitalWrite(trigger, LOW);//Dann wird der „Ton“ abgeschaltet.
dauer = pulseIn(echo, HIGH); //Mit dem Befehl „pulseIn“ zählt der Mikrokontroller die Zeit in Mikrosekunden, bis der Schall zum Ultraschallsensor zurückkehrt.
entfernung = (dauer/2) * 0.03432; //Nun berechnet man die Entfernung in Zentimetern. Man teilt zunächst die Zeit durch zwei (Weil man ja nur eine Strecke berechnen möchte und nicht die Strecke hin- und zurück). Den Wert multipliziert man mit der Schallgeschwindigkeit in der Einheit Zentimeter/Mikrosekunde und erhält dann den Wert in Zentimetern.
if (entfernung >= 500 || entfernung <= 0) //Wenn die gemessene Entfernung über 500cm oder unter 0cm liegt,…
{
Serial.println("Kein Messwert"); //dann soll der serial monitor ausgeben „Kein Messwert“, weil Messwerte in diesen Bereichen falsch oder ungenau sind.
}
else //  Ansonsten…
{
Serial.print(entfernung); //…soll der Wert der Entfernung an den serial monitor hier ausgegeben werden.
Serial.println(" cm"); // Hinter dem Wert der Entfernung soll auch am Serial Monitor die Einheit "cm" angegeben werden.
}
if(entfernung >=5 && entfernung <=15){ 
  UScup = 1; 
  }else {
    UScup = 0;
    }
 //Das delay von einer Sekunde sorgt in ca. jeder neuen Sekunde für einen neuen Messwert.
}

//____________________________________________________________
//switchschalter

    buttonState = digitalRead(buttonPin);
    if (buttonState == HIGH) {editMode = 1;}
    if (buttonState == LOW) {editMode = 0;}


//________________________________________


    if(Timer1I/60 == stundenX && Timer1I%60 == minutenX && Timer1Zustand > 0){
      kaffeeNr = 1;
      }     // Timernummer
    if(Timer2I/60 == stundenX && Timer2I%60 == minutenX && Timer2Zustand > 0){
      kaffeeNr = 2;
      }    // Timernummer
    
   if(sekundenX < 4 && kaffeeProzess == 0 && kaffeeNr > 0 && UScup == 1 && editMode == 0 && kaffeeStepX == 0){
    timeCup = millis();
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
    timeCup = timeCup+zeitBruehen;
    kaffeeStepX = 1;
   }
   
 if(timeCup <= millis() && kaffeeProzess == 1 && UScup == 1 && relaisPower == 0  && kaffeeStepX == 1) {
    timeCup = timeCup+zeitTasse;
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
    kaffeeStepX = 2;
    }
    
    if(timeCup <= millis() && kaffeeProzess == 1 && UScup == 1 && relaisCup1 == 0 && kaffeeStepX == 2 &&  relaisCup1 == 0) {
    kaffeeStepX = 3;
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
    kaffeeStepX = 4;
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
    kaffeeStepX = 4;
    kaffeeStep = 5;
    relaisCup2 = 0;
    };    
    }
    timeCup = timeCup+zeitTasse;
    }
    
    if(timeCup <= millis() && kaffeeProzess == 1 && kaffeeStepX == 3){
    kaffeeStepX = 4;
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
    timeCup = timeCup+zeitFinish;
    }

    if(((timeCup) <= millis()) && kaffeeProzess == 1 && kaffeeStepX == 4){

    if(kaffeeNr == 1){Timer1Zustand = 0;}
    if(kaffeeNr == 2){Timer2Zustand = 0;}
    kaffeeStepX = 0;
    kaffeeProzess = 0;
    kaffeeStep = 0;
    kaffeeNr = 0;
    mode = 0;
    
   }
//____________________________________________________________________________
    
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
if(time <= (millis()-5000) && mode!=0 || editMode == 0){
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

Timer1Zeit = String(minuten(Timer1I,'s'))+String(":")+String(minuten(Timer1I,'m'));
Timer2Zeit = String(minuten(Timer2I,'s'))+String(":")+String(minuten(Timer2I,'m'));

//if(einI == 42) ein = "ROBIN";
//if(ausI == 42) aus = "ROBIN"; 

//____________________________________________________

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
 
display.setTextSize(1);
 display.setCursor(100, 4);
 //display.print(entfernung);
if(UScup == 1) {display.print(F("c(_)")); }

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

display.setTextSize(1);
 display.setCursor(100, 4);
 //display.print(entfernung);
if(UScup == 1) {display.print(F("c(_)")); }

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

display.setTextSize(1);
 display.setCursor(100, 4);
 //display.print(entfernung);
if(UScup == 1) {display.print(F("c(_)")); }

//if(sekundenX == 16) {display.print(F("--")); }
display.setTextSize(1); // Draw 2X-scale text
  display.setCursor(0, 16);
if(kaffeeNr == 1){display.print(F("Kaffee1.. "));}
/*  if(kaffeeNr == 1){display.print(zeitTasse);}
  if(kaffeeNr == 1){display.print(F(" "));} */
  
  if(kaffeeNr == 2){display.print(F("Kaffee2.. "));}
if(kaffeeNr >=1){display.print((timeCup-millis())/1000);}
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

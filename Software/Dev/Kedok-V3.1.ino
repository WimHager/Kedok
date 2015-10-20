/*
    Kedok (audio aiming device), Copyright 2015 Wim Hager
    Kedok is distributed under the terms of the GNU GPL
    
    Kedok is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Kedok is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    12/6/2015 This software is started by Wim Hager email: wim@mowbot.nl
              CAD and 3D printworks Jan Stinissen

To Do:
 Battery level check?
 ShowLCD bug. no clear
 Reset all if version updated
 Update min max only if changed Static!
 Mode to use pull down resistor on phototransistor
 //Bug. while adjusting the MIN or UP parameter while shooting screen dos not return to running if Display is off.
 Always Sound. Added not tested yet!!
 //Menu option to set auto adjust window, Benjamin prefers a window of 100 and a gain of -3
 //Test if gain is reverse. Negative etting gives a positve gain. 
 //Sound during auto adjust wait.
 //More info if display readings. Sensor Min Max Lowest
 //Auto adjust auto stop if no lower reading  within 12 sec.
 
 //Loops free running 4150 with sound 1300
 
 
 V2.00 18-4-2015
 Compiled with 1.0.5
 New: Using NewTone Library
 25-4-2015 Changed defaults for high low tone
 V2.10 27-4-2015
 27-4-2015 Added shift sensor window while running Up/Down key
 29-4-2015 Speed up the main loop by one readkey.
 V3.00 3-5-2015
 Added Auto adjust
 New Beep and Keypressed function
 23-5-2015 Added some text in Auto adjust
 V3.1 26-6-2015
 Gain is replaced by Curve for better understanding.
 Added Auto adjust window size. Default 200
 28-7-2015 Made SensorPin a variable
 30-7-2015 Added option to enable Always sound
 1-9-2015  Always sound bug, was not set in menu
 */

//Note Audio pin 3, 82 Ohm and 470N in serie
//Opto resistor 68K

#include <LiquidCrystal.h>
#include <NewTone.h>
#include <EEPROM.h>
#include <LcdBarGraph.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
LcdBarGraph   lbg(&lcd, 16, 0, 0);
int Melody[] = { 
  262, 196, 196, 220, 196, 0, 247, 262 };
int NoteDurations[] = { 
  4, 8, 8, 4, 4, 4, 4, 4 };

const   char      Version[5]= "3.11";
const   byte      None=           0; 
const   byte      Select=         1;
const   byte      Left=           2; 
const   byte      Down=           3;
const   byte      Up=             4; 
const   byte      Right=          5;
const   byte      RightLong=     15;
const   byte      Value=          1;
const   byte      Bar=            2;
const   boolean   Disable=        true;
const   boolean   Enable=         false;
const   char      EmptyLine[17]=  "                ";

word    MinValue=         100;
word    MaxValue=         800;
word    LowTone=          100; 
word    HighTone=         1750;
word    Curve=            0;
word    AutoAdjustWindow= 200;
word    UpdTime=          1000;
byte    Display=          1;
byte    AlwaysSound=      0;
byte    ResetAll=         0;
char    *DisplayType[]  = {"None", "Value",  "Bar"};
char    *FactoryReset[] = {"No", "Yes"};
byte    AudioPin=         3;
byte    SensorPin=        A1;
int     LowFreq=          1500; // rename to a better to understand variable name

word    LoopCounter;

long    PrevTime;
word    Reading;
word    AudioTone;
word    LowestReading;
byte    KeyPressed;

float fscale( float originalMin, float originalMax, float newBegin, float newEnd, float inputValue, float Curve){
  float   OriginalRange=    0;
  float   NewRange=         0;
  float   zeroRefCurVal=    0;
  float   normalizedCurVal= 0;
  float   rangedValue=      0;
  boolean invFlag=          0;

  Curve= 0-Curve; //Make Curve negative
  Curve= (Curve * -.1) ; 
  Curve= pow(10, Curve);
  if (inputValue < originalMin) inputValue= originalMin;
  if (inputValue > originalMax) inputValue = originalMax;
  OriginalRange = originalMax - originalMin;
  if (newEnd > newBegin) NewRange= newEnd - newBegin;
  else{
    NewRange= newBegin - newEnd; 
    invFlag= 1;
  }
  zeroRefCurVal= inputValue - originalMin;
  normalizedCurVal=  zeroRefCurVal / OriginalRange;  
  if (originalMin > originalMax ) return 0;
  if (invFlag == 0) rangedValue=  (pow(normalizedCurVal, Curve) * NewRange) + newBegin;
  else rangedValue=  newBegin - (pow(normalizedCurVal, Curve) * NewRange); 
  return rangedValue;
}

void Reset() {
  asm volatile ("  jmp 0");
}  

word ReadValue() {
  return analogRead(SensorPin);
}

void Beep(byte Beeps, word Tone) {
  for (byte X=0; X<Beeps; X++) {
    NewTone(AudioPin,Tone,200);
    delay(400);
  }
}

boolean InRange() {
  if (AlwaysSound) return true;
    else return ((Reading > MinValue) && (Reading < MaxValue)); 
}

String WordFormat(word Inp, byte Size) {
  String Str;
  if (Size==4) Str= "0000"; 
  else Str= "000";
  String WordStr= (String)Inp;
  byte Len= WordStr.length();
  for (byte C= 0; C<Len; C++) Str[C+Str.length()-Len]= WordStr[C];
  return Str; 
}  

void PlayMelody() {
  for (int ThisNote= 0; ThisNote < 8; ThisNote++) {
    int NoteDuration = 1000/NoteDurations[ThisNote];
    NewTone(AudioPin, Melody[ThisNote], NoteDuration); // Play thisNote at full volume for noteDuration in the background.
    delay(NoteDuration * 4 / 3); // Wait while the tone plays in the background, plus another 33% delay between notes.
  }
}

void MoveSensorWindow(int Val) {
  MinValue= MinValue + Val;
  MaxValue= MaxValue + Val;
  Beep(2,300);
  ShowLCD("Settings saved..",0, true);
  ShowLCD("Min "+(String)MinValue+" Max "+(String)MaxValue, 1, true);
  WriteConfig();
  delay(3000);
  if (!Display) ShowLCD("Running..",0, false);
}  

void ShowLCD(String Text, byte Line, boolean Clear) {
  lcd.setCursor(0, Line);
  lcd.print(EmptyLine);
  lcd.setCursor(0, Line);
  lcd.print(Text);
}

void ShowBar(byte Len) {
  lcd.clear();
  lbg.drawValue(Len, 160);
  ShowLCD((String)map(Len,0,160,0,10),2,true);
} 

void Screen(boolean Dis) {
  if (Dis) {
    Display= None;
    lcd.clear();
    ShowLCD("Running..",0, false);
  }
  else Display= 1;
  delay(500); 
}

void ShowValues() {
  ShowLCD("Sen:"+WordFormat(Reading,4)+ " Low:"+WordFormat(LowestReading,3),0, true);
  //ShowLCD("Sen:"+WordFormat(Reading,4)+ " L:"+WordFormat(LoopCounter,4),0, true);
  ShowLCD("Max:"+WordFormat(MaxValue,4)+" Min:"+WordFormat(MinValue,3),1, true);
}  

void WriteConfig() {
  EEPROM.write(1,lowByte(MinValue)); 
  EEPROM.write(2,highByte(MinValue));
  EEPROM.write(3,lowByte(MaxValue)); 
  EEPROM.write(4,highByte(MaxValue));
  EEPROM.write(5,lowByte(Curve));     
  EEPROM.write(6,highByte(Curve));
  
  EEPROM.write(7,lowByte(AutoAdjustWindow));     
  EEPROM.write(8,highByte(AutoAdjustWindow));  
    
  EEPROM.write(9,lowByte(LowTone));  
  EEPROM.write(10,highByte(LowTone));
  EEPROM.write(11,lowByte(HighTone)); 
  EEPROM.write(12,highByte(HighTone));
  EEPROM.write(13,Display);
  EEPROM.write(14,AlwaysSound);
  EEPROM.write(0,1);
} 

void ReadConfig() {
  ShowLCD("Read Config...",0, true);
  MinValue=         word(EEPROM.read(2),EEPROM.read(1));
  MaxValue=         word(EEPROM.read(4),EEPROM.read(3));
  Curve=            word(EEPROM.read(6),EEPROM.read(5));
  
  AutoAdjustWindow= word(EEPROM.read(8),EEPROM.read(7)); 
  
  LowTone=          word(EEPROM.read(10),EEPROM.read(9));
  HighTone=         word(EEPROM.read(12),EEPROM.read(11));  
  Display=          EEPROM.read(13);
  AlwaysSound=      EEPROM.read(14);
}

void EEPromClear() {
  for (int I = 0; I < 512; I++) EEPROM.write(I, 0);
}

void AutoAdjust() {
  word Reading;
  word LowestReading= 1023;
  word TimeOutCounter=   0;
  lcd.clear();
  ShowLCD("Auto Adjust.", 0, true);
  ShowLCD("Get ready.", 1, true);
  //ShowLCD((String)TimeOutCounter, 1, true);
  while (TimeOutCounter < 2000) {
    Reading= ReadValue();
    AudioTone= fscale(100,900,HighTone,LowTone,Reading,Curve);
    NewTone(AudioPin, AudioTone);
    TimeOutCounter++;
    delay(10);
  }
  delay(300); 
  Beep(1,2000); 
  delay(300);
  ShowLCD("Aim at target.", 1, true);
  TimeOutCounter= 0;
  while (ReadKey() == None) {
    Reading= ReadValue();
    AudioTone= fscale(100,900,HighTone,LowTone,Reading,Curve);
    NewTone(AudioPin, AudioTone);
    if (Reading < LowestReading) {
      LowestReading= Reading;
      TimeOutCounter= 0;
    }  
    TimeOutCounter++;
    if (TimeOutCounter > 2000) break;
    delay(10);
  }
  if (LowestReading < 820) {
    Beep(3,2000);
    MinValue= LowestReading-20;
    MaxValue= MinValue+AutoAdjustWindow;
    ShowLCD("Config saved", 0, true);
    ShowLCD("Min "+(String)MinValue+" Max "+(String)MaxValue, 1, true);
    WriteConfig();
    delay(3000);
  }
  else Beep(6,100);
  lcd.clear();
  if (!Display) ShowLCD("Running..",0, false); 
} 

byte KeyVal() {
  word KeyVal= analogRead(0);
  if (KeyVal > 900)  return 0; //None
  if (KeyVal > 600)  return 1; //Select
  if (KeyVal > 400)  return 2; //Left
  if (KeyVal > 200)  return 3; //Down
  if (KeyVal > 80 )  return 4; //Up
  return                    5; //Right
}  

byte ReadKey() {
  byte LongPressed= 0;
  word KeyPressCounter= 0;
  byte Key= KeyVal();
  if (Key) {
    while (Key == KeyVal()) {
      KeyPressCounter++;
      if (KeyPressCounter > 15000) {
        Beep(2,600);
        Key= Key+10;
        break;  
      }
    }
  }
  return Key;
}  

void Menu() {
  noNewTone(AudioPin);
  ShowLCD("Settings",0, true);
  boolean Esc= false;
  while (!Esc) {
    ShowLCD("MIN: "+(String)MinValue, 1, true);
    if (KeyVal() == Down)   if (MinValue > 10) MinValue-= 5;
    if (KeyVal() == Up)     if (MinValue < (MaxValue-20)) MinValue+= 5;
    if (KeyVal() == Select) Esc= true;
    delay(300);
  } 
  Esc= false; 
  while (!Esc) {
    ShowLCD("MAX: "+(String)MaxValue, 1, true);
    delay(300);
    if (KeyVal() == Down)   if (MaxValue > (MinValue+20)) MaxValue-= 5;
    if (KeyVal() == Up)     if (MaxValue < 990) MaxValue+= 5;
    if (KeyVal() == Select) Esc= true;
  }
  Esc= false;   
  while (!Esc) {
    ShowLCD("Curve: "+(String)Curve, 1, true);
    delay(300);
    if (KeyVal() == Down)   if (Curve > 0) Curve--;
    if (KeyVal() == Up)     if (Curve < 5) Curve++;
    if (KeyVal() == Select) Esc= true;
  }
  Esc= false;   
  while (!Esc) {
    ShowLCD("AutoWindow: "+(String)AutoAdjustWindow, 1, true);
    delay(300);
    if (KeyVal() == Down)   if (AutoAdjustWindow > 50)   AutoAdjustWindow-= 10;
    if (KeyVal() == Up)     if (AutoAdjustWindow < 300)  AutoAdjustWindow+= 10;
    if (KeyVal() == Select) Esc= true;
  }  
  Esc= false;
  while (!Esc) {
    ShowLCD("LowTone: "+(String)LowTone, 1, true);
    NewTone(AudioPin, LowTone);
    delay(300);
    if (KeyVal() == Down)   if (LowTone > 50) LowTone-= 50;
    if (KeyVal() == Up)     if (LowTone < (HighTone-100)) LowTone+= 50;
    if (KeyVal() == Select) Esc= true;
  } 
  Esc= false; 
  noNewTone(AudioPin);
  while (!Esc) {
    ShowLCD("HighTone: "+(String)HighTone, 1, true);
    NewTone(AudioPin, HighTone);
    delay(300);
    if (KeyVal() == Down)   if (HighTone > (LowTone+100)) HighTone-= 50;
    if (KeyVal() == Up)     if (HighTone < 9000) HighTone+= 50;
    if (KeyVal() == Select) Esc= true;
  }
  Esc= false;
  noNewTone(AudioPin);
  Display=  EEPROM.read(13); //read value from rom setting instead of global  
  while (!Esc) {
    ShowLCD("Display: "+(String)DisplayType[Display], 1, true);
    delay(300);
    if (KeyVal() == Down)   if (Display > 0) Display--;
    if (KeyVal() == Up)     if (Display < 2) Display++;
    if (KeyVal() == Select) Esc= true;
  }
  Esc= false;   
  while (!Esc) {
    ShowLCD("Always Sound: "+(String)AlwaysSound, 1, true);
    delay(300);
    if (KeyVal() == Down)   if (AlwaysSound > 0) AlwaysSound--;
    if (KeyVal() == Up)     if (AlwaysSound < 1) AlwaysSound++;
    if (KeyVal() == Select) Esc= true;
  }
  Esc= false;   
  while (!Esc) {
    ShowLCD("Reset ALL: "+(String)FactoryReset[ResetAll], 1, true);
    delay(300);
    if (KeyVal() == Down)   if (ResetAll > 0) ResetAll--;
    if (KeyVal() == Up)     if (ResetAll < 1) ResetAll++;
    if (KeyVal() == Select) Esc= true;
  }
  if (ResetAll) { 
    EEPROM.write(0,0); 
    Reset(); 
  }
  WriteConfig();
  ShowLCD("Saved......",1,true);
  delay(3000);
  lcd.clear();
  if (!Display) ShowLCD("Running..",0, false); 
} 

void setup() {
  pinMode(A1, INPUT);
  lcd.begin(16, 2);
  ShowLCD("Version "+(String)Version,0, true);
  delay(800);
  ShowLCD("Starting...",0, true);
  delay(500);
  if (EEPROM.read(0)==1) ReadConfig();
  else WriteConfig();
  delay(1000);
  PrevTime= millis();
  LowestReading= MaxValue;
  if (!Display) ShowLCD("Running..",0, false);
  PlayMelody();
}

void loop() {
  Reading= ReadValue();
  if (Display) {
    if ((millis()-PrevTime) > UpdTime) {
      if (Display == Value) ShowValues();
      if (Display == Bar) if (InRange()) ShowBar(map(Reading,MinValue,MaxValue,160,0));
      else ShowBar(2);
      PrevTime= millis();
    }
  }  
  if (Reading < MinValue) {
    lcd.clear();
    ShowLCD("Lower Min Value!",0,true);
    Beep(3,300);
    if (!Display) ShowLCD("Running..",0, false);
  }
  if (InRange()) {
    AudioTone= fscale(MinValue,MaxValue,HighTone,LowTone,Reading,Curve);
    NewTone(AudioPin, AudioTone);
    //delay(100);
  }
  else noNewTone(AudioPin); // Turn off the tone. 
  if (Reading < LowestReading) LowestReading= Reading; 
  KeyPressed= ReadKey();
  if (KeyPressed) {  
    if (KeyPressed == Select) Menu();
    if (KeyPressed == Right)  LowestReading= MaxValue;
    if (KeyPressed == Left)   if (Display) Screen(Disable); 
    else Screen(Enable);
    if (KeyPressed == Down)   MoveSensorWindow(-5);
    if (KeyPressed == Up)     MoveSensorWindow(+5);
    if (KeyPressed == RightLong) AutoAdjust();
  }
}


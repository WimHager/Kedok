#if 1                     //Needed for sketch to respect preprocessor directives
__asm volatile ("nop");
#endif

//====================================Compiler options=============================================================
//#define DEBUG-LCD     //Show Debug on LCD screen. Enables debug info on LCD screen, only with LCD 
//#define DDS9833       //Enable for a Kedok version with a DDS module. Disable for TTL output (DDS only with LCD)
#define SPEECH        //Enable if you compile a Kedok speech version. Disable for LCD
#define OLED          //Enable O-LED i2c 128x64 only used for Speech
#define DEBUG-SPEECH  //Give debug info over serial port if you compile a Kedok speech version
#define MEMBRANE-1X5 //Enable if  1 x 5 membrane keypad is used. Disabled if you use 1 x 4 keypad. Only for Speech!!


const   char      Version[5]="6.00";
const   char      Owner[16]= "DEMO";
const   char      SerialNr[23]=  "";

#ifdef SPEECH
   word    MP3Language=       0X0100; //0X0100 English 0X0200 Dutch
#endif
//=================================================================================================================

/*
    Kedok (audio aiming device), Copyright 2016 Wim Hager
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
 Start to add a i2c ADS1015 12-Bit ADC - 4 Channel 
 ShowLCD bug. no clear
 Reset all if version updated
 Better Auto adjust
 Full test of Curve parameter
 Triangle wave still gives a beep when fequency is set to zero,
          Buggy at B28 Setting, needs a full test.
 Full test if without DDS module
 Always Sound does not work if DDS connected.
 Add an option to escape the menu without saving
 Add option to save users presets
 Change delay's in speech, some are utterly slow alo improve saying values (remove "value is" in some cases) 
 Beginners mode, wide window
 Test PitchStep feature
 Set Hightone default 300 lower if Pitchstep is enabled
 Remove Log option, it is never used.
 Add change owner name with buttons.
 Smoothing filter, Exponential Filter. filter.h
 Bug LowReadWarning()  must say lower sensor value
 Adding new 1 x 5 membrane Keypad speech version
 //Changed MoveSensorWindowStepSize from 10 to 5 if shooter presses UP Or Down while shooting 
 //Added PitchStep feature
 //UP and Down buttons, Speedup
 //Rename Treshhold to Lead in
 //Test Factory settings with speech
 //Change text for 055-Sensor Level To High.mp3. Say the value.
 //Add parameter to enable/disable  say value is. 
 //Add speech say freqency at save
 //Add English Help files
 //Add smooth tone 
 //Sound menu options for average and loop speed
 //Auto calibrate does not start
 //Option to set loop speed
 //Add AutoAdjustGetReadyTime to menu as option
 //Say calibratie value
 //MP3 Beep if keypressed       
 //Rollback from 3v3 mod. Does not work well with LCD keypad output on A0
 //No need to save logmode in eeprom
 //Logmode in display Only in No Display mode
 //Reverse Pitch
 //Save Setting as structure???
 //Always Sound. Added not tested yet!!
 //Removed "Always sound" no-one liked it.
 //new inrange for bar reading
 //Better way to find the target card. Idea:  if sensor-read < Min + 2 x Windowsize set sensor window twice the size
 //SetSensorWindowToLowestRead()
 //Make WordToStr better so it fits all sizes
 //Bug. while adjusting the MIN or UP parameter while shooting screen dos not return to running if Display is off.
 //Menu option to set auto adjust window, Benjamin prefers a window of 100 and a gain of -3
 //Test if gain is reverse. Negative etting gives a positve gain. 
 //Sound during auto adjust wait.
 //More info if display readings. Sensor Min Max Lowest
 //Auto adjust auto stop if no lower reading  within 12 sec.
 //Show settings instead of Running ... if displaymode is off
 //Logging
 
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
 28-7-2015  Made SensorPin a variable
 30-7-2015  Added option to enable Always sound
 1-9-2015   Always sound bug, was not set in menu
 24-10-2015 Screen bug update in no Dispaly mode
 26-10-2015 Removed cracking sound aiming near bulls-eye (removed inrange low value check)
 28-10-2015 Window Dec Inc set to 10
 28-10-2015 Redo of show status screen if display is disabled.
 31-10-2015 Added Owner name, shown when booting.
 02-11-2015 Added auto set Min value with a warning by pressing 3 sec down key
 V3.20 04-11-2015
 06-11-2015 New Kernel for better target card find
 06-11-2015 Removed Always sound, no-one liked it.
 14-11-2015 Added Logging.
 14-11-2015 Added Reverse Pitch
 14-11-2015 EE-Prom settings are saved as a Struct obj now
 25-11-2015 Compiled with 1.6.5 now
 28-11-2015 Roll back:  Added option to use 3.3V ref for optosensor. Enabled if data pin 0 is grounded.
 V5.00 01-01-2016 
 01-01-2016 Adding an AD8933 DDS module
 04-02-2016 Merged in Speech
 07-02-2016 Added Average Sensor
 07-04-2016 Added Sample rate
 09-04-2016 Added Time settings for wait time Get Ready auto adjust
 10-04-2016 Added Dutch help files
 22-05-2016 moved to Stable branch
 */

//Note Audio pin 3 or 10. 82 Ohm and 470N in serie
//Opto resistor 68K
//Loops DDS FAST:     1200
//      DDS MEDIUM:    173
//      DDS SLOW:       93
//      DDS SLOWEST     45
//      TTL FAST:
//      TTL MEDIUM:
//      TTL SLOW: 
//      TTL SLOWEST: 

/*  Nano speech =====================================================================================
 
  To Pot volume
       ^
       |
      ---              To Keyboard
      | | 5K6               ^
      | |                   |
      ---              -----------
       |              |           |
       |              |           |                    
[0][0][X][0][0][0][X][X][X][X][X][X][0][0][0]
      D10          D6 D5 D4 D3 D2 GND

                                GND <---[X][0]
                                 TX <---[X][0]         To MP3 board
                                VCC <---[X][X]---> RX


                  A3             VCC   GND
[0][0][0][0][0][0][X][0][0][0][0][X][0][X][0]
                   |              |     |              |--> To Charger GND
                   |             ---    ---------------|
                   v             | |                   |--> To Sensor Jack
                                 v v
              To Sensor jack     To Charger To Sensor jack
*/

/* Lenoardo LCD =====================================================================================

                                        To Pot volume
                                             ^
                                             |
                                            ---             
                                            | | 5K6           
                                            | |               
                                            ---         
                                             |             
                                             |    
[0][0][0][0][0][0][0][0][0][0]  [0][0][0][0][X][0][0][0]
                                            D3


                                             GND <---[0][0]
                                                     [0][0]       
                                             VCC <---[0][0]



                       +5  Gnd        A0 A1 A2 A3
           [0][0][0][0][X][X][X][0]   [0][X][0][0][0][0]
                        |                 |
                        |                 |
                        v To Charger      v To Sensor jack

*/


////////////////////////////////////////////

#ifdef OLED
  #define I2C_ADDRESS 0x3C
  #include "SSD1306Ascii.h"
  #include "SSD1306AsciiAvrI2c.h"
  SSD1306AsciiAvrI2c oled;
#endif
#ifdef SPEECH
  #include <SoftwareSerial.h>
#else
  #include <LiquidCrystal.h>
  #include <LcdBarGraph.h>
#endif
#ifdef DDS9833
  #include <SPI.h>
#else
  #include <NewTone.h>
#endif
#include <EEPROM.h>

#ifdef SPEECH
   #define ARDUINO_RX 11 //should connect to TX of the Serial MP3 Player module
   #define ARDUINO_TX 12 //connect to RX of the module
   #define CMD_SET_VOLUME 0X06
   #define CMD_PLAY_W_INDEX 0X08
   #define CMD_SEL_DEV 0X09
   #define DEV_TF 0X02
   #define CMD_PLAY 0X0D
   #define CMD_PAUSE 0X0E
   #define CMD_SINGLE_CYCLE 0X19
   #define SINGLE_CYCLE_ON 0X00
   #define SINGLE_CYCLE_OFF 0X01
   #define CMD_PLAY_W_VOL 0X22
   #define CMD_PLAY_FOLDER_FILE 0X0F
   #define ZeroMP3                              0
   #define OneMP3                               1
   #define TwoMP3                               2
   #define ThreeMP3                             3
   #define FourMP3                              4
   #define FiveMP3                              5
   #define SixMP3                               6
   #define SevenMP3                             7
   #define EightMP3                             8
   #define NineMP3                              9
   #define YesMP3                              10
   #define NoMP3                               16
   #define EnableMP3                           14
   #define DisableMP3                          13
   #define WelcomeMP3                          31
   #define SettingsMenuMP3                     28
   #define UseArrowKeysMP3                     30
   #define SetMinimalSensorValueMP3            23  
   #define SetMaximalSensorValueMP3            22  
   #define SetSensorThresholdValueMP3          25  
   #define SetSoundCurveValueMP3               26  
   #define SetPitchReverseMP3                  24  
   #define SetLowestPitchMP3                   21  
   #define SetHigestPitchMP3                   20  
   #define SetAutoAdjustWindowMP3              19
   #define SetTimeToGetReadyMP3                61 
   #define SetSampleSpeedMP3                   59
   #define SetVolumeMP3                        27 
   #define SetAlwaysSoundMP3                   43
   #define SetSensorAverageCountMP3            51         
   #define RestoreFactorySettingsMP3           18
   #define LowerMinimalSettingMP3              15
   #define PrepareWeaponForAimingMP3           17
   #define StartToAimNowMP3                    29
   #define AutoAdjustFinishedMP3               12
   #define CountDownFrom20MP3                  11
   #define SelectTheOptionMP3                  32
   #define ExitOptionsMenuMP3                  33
   #define DataSettingsSavedMP3                34
   #define TheValueIsMP3                       35
   #define PitchReverseDisabledMP3             37
   #define PitchReverseEnabledMP3              36
   #define AlwaysSoundEnabledMP3               38
   #define AlwaysSoundDisabledMP3              39
   #define AllSettingsResetToDefaultsMP3       40
   #define RestoreFactoryDefaultsDisabledMP3   41
   #define RestoreFactoryDefaultsEnabledMP3    42
   #define SettingsAreSavedMP3                 44
   #define LoggingStoppedMP3                   45 
   #define AutoAdjustStartedMP3                46
   #define ReadConfigMP3                       47
   #define HaveFunShootingMP3                  48
   #define SensorReducedMP3                    49
   #define SensorIncreasedMP3                  50 
   #define LowMP3                              52
   #define AverageMP3                          53
   #define HighMP3                             54
   #define CalibrationLevelToHighMP3           55
   #define CalibratedAtMP3                     56
   #define AutoAdjustGetReadyMP3               57
   #define MaximalMP3                          58
   #define FastMP3                             60
   #define SlowMP3                             62
   #define MediumMP3                           63
   #define SlowestMP3                          64
   #define LowPitchSetAtMP3                    65
   #define HighPitchSetAtMP3                   66
   #define HertzMP3                            67
   #define SetPitchStepMP3                     68 
   #define PitchStepEnabledMP3                 69
   #define PitchStepDisabledMP3                70  

   #define HelpSetMinimalSensorValueMP3       123  
   #define HelpSetMaximalSensorValueMP3       122  
   #define HelpSetSensorThresholdValueMP3     125  
   #define HelpSetSoundCurveValueMP3          126  
   #define HelpSetPitchReverseMP3             124  
   #define HelpSetLowestPitchMP3              121  
   #define HelpSetHigestPitchMP3              120  
   #define HelpSetAutoAdjustWindowMP3         119
   #define HelpSetTimeToGetReadyMP3           161 
   #define HelpSetSampleSpeedMP3              159
   #define HelpSetVolumeMP3                   127 
   #define HelpSetSensorAverageCountMP3       151  
   #define HelpSetAlwaysSoundMP3              143  
   #define HelpRestoreFactorySettingsMP3      118
   #define HelpSetPitchStepMP3                168 
   #ifdef MEMBRANE-1X5 // 1 x 5 Membrane Keypad
     #define Key1Pin                            2
     #define Key2Pin                            3
     #define Key3Pin                            4
     #define Key4Pin                            6
     #define Key5Pin                            5   
   #else  // 1 x 4 Membrane Keypad
     #define Key1Pin                            4
     #define Key2Pin                            2
     #define Key3Pin                            5
     #define Key4Pin                            3
     #define Key5Pin                            6
   #endif  
   SoftwareSerial mySerial(ARDUINO_RX, ARDUINO_TX);
   static int8_t     Send_buf[8]= {0};
#else
   LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
   LcdBarGraph   lbg(&lcd, 16, 0, 0);
#endif   

#ifdef SPEECH
  byte    AudioPin=               10;
  byte    SensorPin=              A3;
#else
  byte    AudioPin=                3;
  byte    SensorPin=              A1;
#endif 
   
int Melody[] = { 
  262, 196, 196, 220, 196, 0, 247, 262 };
int NoteDurations[] = { 
  4, 8, 8, 4, 4, 4, 4, 4 };
  
#ifdef SPEECH
  const  byte None=                 0;
  const  byte Select=               1;
  const  byte Down=                 3;
  const  byte Up=                   2;
  const  byte Right=                4;
  const  byte Left=                 5;
#else
  const   byte      None=           0; 
  const   byte      Select=         1;
  const   byte      Left=           2;
  const   byte      Down=           3;
  const   byte      DownLong=      13;
  const   byte      Up=             4; 
  const   byte      Right=          5;
  const   byte      RightLong=     15;
#endif 
const   byte      Value=            1;
const   byte      Bar=              2;
const   boolean   Disable=       true;
const   boolean   Enable=       false;
const   char      EmptyLine[17]=  "                ";

#ifdef DDS9833
  const   int       FSyncPin=       2;            // AD9833 Chip select Pin
  const   float     XTALFreq=  25.0E6;            // On-board X-TAL reference frequency.
#endif  

word    MinValue=                 100;
word    MaxValue=                 800;
#ifdef DDS9833
  word    LowTone=                150; 
  word    HighTone=              2000;
#else
  word    LowTone=                100; 
  word    HighTone=              1750;
#endif

byte    Curve=                      0;
byte    WaveShape=                  0;
byte    PitchRev=               false;
#ifdef SPEECH
  byte    AlwaysSound=           true; 
#else
  byte    AlwaysSound=          false; 
#endif  
word    AutoAdjustWindow=         200; // Normal card size
byte    ThresholdWindow=          150; 
byte    GetReadyTime=              20; // 20 Seconds
word    PitchStepValue=           300; // Around the 8 on target card
word    MoveSensorWindowStepSize=   5; // In/Dec MoveWindow steps if Up or Down is pressed  //Make constant?? !!!
byte    LogMode=                    0;
word    LogCounter=                 0;
byte    MP3Volume=                 25;
byte    LogBufferStart=            50; 
word    LogUpdTime=               250; //4 times a second for 1000 values about 4 Min. logging 
byte    AverageValue=               1; //Read 0 values to average, Default none. Steps 0,5,20,85
byte    SampleSpeed=                0; //Loop delay 0,5,10,20
byte    PitchStep=                  0; //Disables or enables Pitch step feature
byte    Display=                    0;

struct SettingsObj {
  word MinValue;
  word MaxValue;
  byte ThresholdWindow;
  byte Curve;
  byte WaveShape;
  byte PitchRev;
  byte AlwaysSound;  
  word AutoAdjustWindow;
  byte GetReadyTime;        //Timeout for get ready with auto adjust
  word LowTone;
  word HighTone;
  byte MP3Volume;
  byte Display;
  byte AverageValue;
  byte SampleSpeed;
  byte PitchStep;
};  

//#ifndef SPEECH  
  word    DispUpdTime=            1000; //1 sec Screen update 
  char    *DisplayType[]=       {"None", "Value",  "Bar"};
  char    *WaveShapes[]=        {"Sine", "Triangle", "Square"};
  char    *YesNoArr[]=          {"N", "Y"};
  char    *LoggingModes[]=      {"Off", "On", "Play"};
  char    *AvgModes[]=          {"Disable", "Low", "Medium", "Maximal"};
  char    *SampleModes[]=       {"Fast", "Medium", "Slow", "Slowest"};
  char    *EnableDisableArr[]=  {"Disable","Enable"};
  long    PrevDispTime;
//#endif

long    PrevLogTime;
word    Reading;
word    AudioTone;
word    LowestReading;
word    WarningReading;
byte    KeyPressed;

//Debug params
#ifdef DEBUG
word    LoopCounter; 
#endif  

void WriteConfig() {
  EEPROM.write(0,1);
  SettingsObj CurSettings= {
    MinValue,
    MaxValue,
    ThresholdWindow,
    Curve,
    WaveShape,
    PitchRev,
    AlwaysSound,
    AutoAdjustWindow,
    GetReadyTime,
    LowTone,
    HighTone,
    MP3Volume,
    Display,
    AverageValue,
    SampleSpeed,
    PitchStep,
  };  
  EEPROM.put(1, CurSettings);
  #ifdef SPEECH
    ShowOLED("Saving config..", 0,4,1);
    PlaySound(DataSettingsSavedMP3,3);
  #endif  
} 

void ReadConfig() {
  SettingsObj CurSettings;
  EEPROM.get(1, CurSettings); //Read all settings
  MinValue= CurSettings.MinValue;
  MaxValue= CurSettings.MaxValue;
  ThresholdWindow= CurSettings.ThresholdWindow;
  GetReadyTime= CurSettings.GetReadyTime;
  Curve= CurSettings.Curve;
  WaveShape= CurSettings.WaveShape;
  PitchRev= CurSettings.PitchRev;
  AlwaysSound= CurSettings.AlwaysSound;
  AutoAdjustWindow= CurSettings.AutoAdjustWindow;
  LowTone= CurSettings.LowTone;
  HighTone= CurSettings.HighTone;
  MP3Volume= CurSettings.MP3Volume;
  Display= CurSettings.Display;
  AverageValue= CurSettings.AverageValue;
  SampleSpeed= CurSettings.SampleSpeed;
  PitchStep= CurSettings.PitchStep;
  #ifdef SPEECH
     ShowOLED("Reading config..", 0,4,1);
     PlaySound(ReadConfigMP3,4);
  #else   
    ShowLCD("Read Config...",0, true);
  #endif 
}

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

word ReadValue(word AvgVal) { 
  if (!AverageValue) return analogRead(SensorPin); // For the sake of speed
  unsigned long ValueSum= 0;
  AvgVal= pow(AvgVal,4)+4; //Creates avg. steps of 5, 20, 85
  for (long X=0; X<AverageValue; X++) ValueSum+= analogRead(SensorPin);
  return ValueSum/AverageValue;
}

void PlayTone(word Tone, word Duration) {
   #ifdef DDS9833
     SetFrequency(Tone);
     if (Duration != 0) {
       delay(Duration);
       SetFrequency(0);
     }
   #else
     if (Tone == 0) noNewTone(AudioPin);
     else NewTone(AudioPin, Tone, Duration);
   #endif    
}

void Beep(byte Beeps, word Pitch) {
  for (byte X=0; X<Beeps; X++) {
    #ifdef DDS8933
        PlayTone(AudioPin,Pitch,400);
    #else
        PlayTone(Pitch,200);
    #endif        
    delay(400);
  }
}

void PlayMelody() {
  for (int ThisNote= 0; ThisNote < 8; ThisNote++) {
    int NoteDuration = 1000/NoteDurations[ThisNote];
    #ifdef DDS9833
        PlayTone(Melody[ThisNote]*5, NoteDuration/2); // Play thisNote at full volume for noteDuration in the background.
    #else
        PlayTone(Melody[ThisNote], NoteDuration);   // Play thisNote at full volume for noteDuration in the background. 
    #endif       
    delay(NoteDuration * 4 / 3); // Wait while the tone plays in the background, plus another 33% delay between notes.
  }
}

#ifdef DDS9833
  void AD9833reset() {
    WriteToDDS(0x100);   // Write '1' to AD9833 Control register bit D8.
    delay(50);
  }

  void SetFrequency(long Frequency) {   // Set AD8933 frequency and wave shape registers.
    int Shape= 0x2000;                  // Sinus
    if (WaveShape == 1) Shape= 0x2002;  // Triangle
    if (WaveShape == 2) Shape= 0x2020;  // Square  
    long FreqWord = (Frequency * pow(2, 28)) / XTALFreq;
    int MSB = (int)((FreqWord & 0xFFFC000) >> 14);    //Only lower 14 bits are used for data
    int LSB = (int)( FreqWord & 0x3FFF);
    //Set control bits 15 ande 14 to 0 and 1, respectively, for frequency register 0
    LSB |= 0x4000;
    MSB |= 0x4000; 
    if (!Shape)  WriteToDDS(1 << 13); // B28 for 16 bits updates if Sinus
    WriteToDDS(LSB);                  // Write lower 16 bits to AD9833 registers
    WriteToDDS(MSB);                  // Write upper 16 bits to AD9833 registers.
    WriteToDDS(0xC000);               // Phase register
    WriteToDDS(Shape);                // Exit & Reset to SINE, SQUARE or TRIANGLE
  }

  void WriteToDDS(int Data) {  
    digitalWrite(FSyncPin, LOW);       // Set FSyncPinPin low before writing to AD9833 registers
    delayMicroseconds(5);              // Give AD9833 time to get ready to receive data.
    SPI.transfer(highByte(Data));      // Each AD9833 register is 32 bits wide and each 16
    SPI.transfer(lowByte (Data));      // bits has to be transferred as 2 x 8-bit bytes.
    digitalWrite(FSyncPin, HIGH);      // Write done. Set FSyncPinPin high
  }
#endif

void LowReadWarning() {
    PlayTone(0,0); // Turn off the tone.
    WarningReading= Reading;
    #ifndef SPEECH
      lcd.clear();
      ShowLCD("Lower Min Value!",0,true);
      Beep(3,300);
      if (!Display) ShowStatusLCD();
    #else  
      Beep(3,300);
      //Say play sound 15
    #endif  
}  

String WordToStr(word Inp, byte Size) {
  String Str;
  for (byte C= 0; C<Size; C++) Str+= "0";
  String WordStr= (String)Inp;
  byte Len= WordStr.length();
  for (byte C= 0; C<Len; C++) Str[C+Str.length()-Len]= WordStr[C];
  return Str; 
}  

boolean InRange() {
  return ((Reading > MinValue) && (Reading < MaxValue));
}

void StopLogging() {
   LogMode= 0;
   LogCounter= 0;
   #ifdef SPEECH
     PlaySound(LoggingStoppedMP3,4);
   #else
     lcd.clear();
     ShowLCD("Logging stopped.",0,true);
   #endif  
   Beep(2,300);
   #ifndef SPEECH
      if (!Display) ShowStatusLCD();
   #endif   
}  

void WriteLog(word Value) {
  if ((millis()-PrevLogTime) > LogUpdTime) {
    if (LogCounter < 999) {
       EEPROM.update(LogCounter+LogBufferStart, map(Value, MinValue, MaxValue, 0, 255));
       LogCounter++;
       PrevLogTime= millis();
    }else StopLogging();
  }  
 }  

void OutputLog() {
  for (int C= LogBufferStart; C<999; C++) { 
    Serial.println(EEPROM.read(C));
  }
  StopLogging();
}  

void MoveSensorWindow(int Val) {
  MinValue= MinValue + Val;
  MaxValue= MaxValue + Val;
  #ifdef SPEECH
     if (Val < 0) PlaySound(SensorReducedMP3,3);
     else  PlaySound(SensorIncreasedMP3,3);
  #else
     Beep(1,300);
     ShowLCD("Settings saved..",0, true);
     ShowLCD("Min "+(String)MinValue+" Max "+(String)MaxValue, 1, true);
  #endif   
  WriteConfig();
  delay(500);
  #ifndef SPEECH
     if (!Display) ShowStatusLCD();
  #endif   
}  

void MoveSensorWindowToLowestRead() {
  word SensorWindow= MaxValue - MinValue;
  MinValue= WarningReading-10;
  MaxValue= MinValue+SensorWindow;
  Beep(2,300);
  #ifdef SPEECH
    PlaySound(SettingsAreSavedMP3,4);
    //Maybe say settings  
  #else
    ShowLCD("Settings saved..",0, true);
    ShowLCD("Min "+(String)MinValue+" Max "+(String)MaxValue, 1, true);
  #endif  
  WriteConfig();
  delay(3000);
  #ifndef SPEECH
    if (!Display) ShowStatusLCD();
  #endif  
}  

void EEPromClear() {
  for (int I = 0; I < 999; I++) EEPROM.write(I, 0); //reserve 1000 to 1023 for serial number
}

void AutoAdjust() {
  word Reading;
  word LowestReading= 1023;
  word TimeOutCounter=   0; 
  #ifdef SPEECH
    ClearOLED();
    ShowOLED("Prepare for aiming..", 0,4,1);
    PlaySound(AutoAdjustStartedMP3,4);
    PlaySound(PrepareWeaponForAimingMP3,4);
  #else  
    lcd.clear();
    ShowLCD("Auto Adjust.", 0, true);
    ShowLCD("Get ready.", 1, true);
    //ShowLCD((String)TimeOutCounter, 1, true);
  #endif  
  while (TimeOutCounter < (GetReadyTime*90)) {
    Reading= ReadValue(0);
    AudioTone= fscale(100,900,HighTone,LowTone,Reading,Curve);
    PlayTone(AudioTone,0);
    TimeOutCounter++;
    delay(10);
  }
  delay(300); 
  Beep(1,2000); 
  delay(300);
  #ifdef SPEECH
     ShowOLED("Start to aim now!", 0,4,1);
     PlaySound(StartToAimNowMP3,5);
  #else   
     ShowLCD("Aim at target.", 1, true);
  #endif   
  TimeOutCounter= 0;
  while (ReadKey() == None) {
    Reading= ReadValue(0);
    AudioTone= fscale(100,900,HighTone,LowTone,Reading,Curve);
    PlayTone(AudioTone,0);
    if (Reading < LowestReading) {
      LowestReading= Reading;
      TimeOutCounter= 0;
    }  
    TimeOutCounter++;
    if (TimeOutCounter > 2000) break;
    delay(10);
  }
  if (LowestReading < (1023-(AutoAdjustWindow+ThresholdWindow))) { //Calculate lowest value within AD range
    Beep(3,2000);
    MinValue= LowestReading-20;
    MaxValue= MinValue+AutoAdjustWindow;
    #ifndef SPEECH
      ShowLCD("Config saved", 0, true);
      ShowLCD("Min "+(String)MinValue+" Max "+(String)MaxValue, 1, true);
    #endif  
    WriteConfig();
    delay(3000);
  }else{
     #ifdef SPEECH
        ShowOLED("Warning level to high!", 0,4,1);
        PlaySound(CalibrationLevelToHighMP3,10);
        PlayNumber(LowestReading,1);
     #else
        lcd.clear();
        ShowLCD("Warning!!", 0, true);
        ShowLCD("Sensor Error, ", 1, false);
        Beep(6,100);
     #endif 
  }  
  #ifdef SPEECH
     ShowOLED("Calibrated at: "+(String)MinValue, 0,4,1);
     PlaySound(CalibratedAtMP3,3);
     PlayNumber(MinValue, 0);
     ShowOLED("Ajusting finished", 0,4,1);
     PlaySound(AutoAdjustFinishedMP3,4);
     ShowOLED("Enjoy shooting!", 0,4,1);
     PlaySound(HaveFunShootingMP3,4);
     ShowValuesOLED();
  #else
    lcd.clear();
    if (!Display)ShowStatusLCD();
  #endif   
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

#ifdef SPEECH
  void InitKeyPad() {
    pinMode(Key1Pin,INPUT_PULLUP);
    pinMode(Key2Pin,INPUT_PULLUP);
    pinMode(Key3Pin,INPUT_PULLUP);
    pinMode(Key4Pin,INPUT_PULLUP);
    pinMode(Key5Pin,INPUT_PULLUP);
  }
 
  byte DelaySecIntr(word Time, boolean Intr) {
    byte KeyPressed;
    while (KeyVal()) delay(100);  Intr= Intr; //Wait till key is released
      if (Intr) {
         Time= Time*800;
         for (word X=0; X<Time; X++) {
            KeyPressed= KeyVal();
            if (KeyPressed) return KeyPressed;
            delay(1);
         }  
      }else{
        delay(Time);
        return None; 
      }
      return None;
  }  
  
  #ifdef OLED
     void ShowOLED(String Text, byte X, byte Y, byte Font) {
       if (Font==1) oled.setFont(X11fixed7x14); 
       if (Font==2) oled.setFont(TimesNewRoman16_bold);
       if (Font==3) oled.setFont(System5x7);
       if (Font==4) oled.setFont(Wendy3x5);
       if (Font==5) oled.setFont(lcdnums14x24);
       oled.setCol(X); oled.setRow(Y);
       oled.clearToEOL();
       oled.print(Text);
     }
     
     void ShowValuesOLED() {
        ShowOLED("Pitch     : "+(String)LowTone+".."+(String)HighTone,0,2,3);
        ShowOLED("Sensor    : "+(String)MinValue+".."+(String)MaxValue,0,3,3);
        ShowOLED("Curve     : "+(String)Curve,0,4,3); 
        ShowOLED("Average   : "+(String)AvgModes[AverageValue],0,5,3);
        ShowOLED("Samples   : "+(String)SampleModes[SampleSpeed],0,6,3);
        ShowOLED("Pitch Step: "+(String)YesNoArr[PitchStep],0,7,3);
     }

     void ShowSensorOLED(word Value) {
        ShowOLED((String)Value,45,3,5);
     }

     void ClearOLED() {
        oled.clear();
        ShowOLED("Kedok "+(String)Version+"           (c)", 0,0,2);
     }
     
  #endif   

  void SendMP3Command(int8_t Command, int16_t Data) {
    #ifdef DEBUG-SPEECH
      Serial.println("MP3 COMMAND");   
      Serial.println("CMD: "+(String)Command);
      Serial.println("Data: "+(String)Data);
    #endif   
    delay(20);
    Send_buf[0] = 0x7e; //starting byte
    Send_buf[1] = 0xff; //version
    Send_buf[2] = 0x06; //the number of bytes of the command without starting byte and ending byte
    Send_buf[3] = Command; //
    Send_buf[4] = 0x00;//0x00 = no feedback, 0x01 = feedback
    Send_buf[5] = (int8_t)(Data >> 8);//datah
    Send_buf[6] = (int8_t)(Data); //datal
    Send_buf[7] = 0xef; //ending byte
    for(uint8_t I=0; I<8; I++) mySerial.write(Send_buf[I]) ;
  }

  void SetVolume(word Vol) {
    SendMP3Command(CMD_SET_VOLUME, 0X0000+Vol);
  } 

  void PlaySound(word Index, word Time) {
    #ifdef DEBUG-SPEECH
      Serial.println("Play sound nr: "+(String)Index);
    #endif
    SendMP3Command(CMD_PLAY_FOLDER_FILE, MP3Language+Index);
    DelaySecIntr(Time,true);
  }

  void PlayNumber(word Nr, byte EnableTheValueText) {
    if (EnableTheValueText) PlaySound(TheValueIsMP3,2);
    if (Nr > 999) PlaySound(( Nr/1000),1);
    if (Nr > 99)  PlaySound(((Nr/100)%10),1);
    if (Nr > 9)   PlaySound(((Nr/10)%10),1);
    PlaySound((Nr%10),3);
  }

  void PlayHelp(byte Option) {
    switch(Option) {
      case  0: PlaySound(HelpSetVolumeMP3,8); break;
      case  1: PlaySound(HelpSetMinimalSensorValueMP3,13); break;
      case  2: PlaySound(HelpSetMaximalSensorValueMP3,13); break; 
      case  3: PlaySound(HelpSetSensorThresholdValueMP3,10); break;
      case  4: PlaySound(HelpSetSoundCurveValueMP3,11); break;
      case  5: PlaySound(HelpSetPitchReverseMP3,9); break;
      case  6: PlaySound(HelpSetAlwaysSoundMP3,8); break;
      case  7: PlaySound(HelpSetLowestPitchMP3,7); break;  
      case  8: PlaySound(HelpSetHigestPitchMP3,7); break;
      case  9: PlaySound(HelpSetAutoAdjustWindowMP3,11); break;
      case 10: PlaySound(HelpSetTimeToGetReadyMP3,10); break;
      case 11: PlaySound(HelpSetSensorAverageCountMP3 ,9); break;
      case 12: PlaySound(HelpSetSampleSpeedMP3 ,8); break;
      case 13: PlaySound(HelpSetPitchStepMP3,12); break;
      case 14: PlaySound(HelpRestoreFactorySettingsMP3,10); break;
    }  
  }

  byte KeyVal() {
    #ifdef DEBUG-SPEECH
       if (!digitalRead(Key1Pin)) Serial.println("Key: Select"); 
       if (!digitalRead(Key2Pin)) Serial.println("Key: Down");
       if (!digitalRead(Key3Pin)) Serial.println("Key: Up");
       if (!digitalRead(Key4Pin)) Serial.println("Key: Right");
       if (!digitalRead(Key5Pin)) Serial.println("Key: Left");
    #endif
    if (!digitalRead(Key1Pin)) {PlayTone(1000,20); return Select;}
    if (!digitalRead(Key2Pin)) {PlayTone(1000,20); return Down;}
    if (!digitalRead(Key3Pin)) {PlayTone(1000,20); return Up;}
    if (!digitalRead(Key4Pin)) {PlayTone(1000,20); return Right;}
    if (!digitalRead(Key5Pin)) {PlayTone(1000,20); return Left;}
    return None; //No key
  }

  void Menu() {
  ClearOLED();
  ShowOLED("Settings menu", 0,0,2);
  ShowOLED("Please wait...", 0,4,1);
  PlayTone(0,0);
  byte OptionNr= 0;
  PlaySound(SettingsMenuMP3,3);
  PlaySound(SelectTheOptionMP3,5);
  boolean Esc= false;
  while(!Esc) {
      switch (OptionNr) {
        case  0: ShowOLED("[Voice volume]", 0,4,1); PlaySound(SetVolumeMP3,8); break;
        case  1: ShowOLED("[Minimal sensor]", 0,4,1); PlaySound(SetMinimalSensorValueMP3,8); break; 
        case  2: ShowOLED("[Maximal sensor]", 0,4,1); PlaySound(SetMaximalSensorValueMP3,8); break;
        case  3: ShowOLED("[Sensor threshold]", 0,4,1); PlaySound(SetSensorThresholdValueMP3,8); break;  
        case  4: ShowOLED("[Sound curve]", 0,4,1); PlaySound(SetSoundCurveValueMP3,8); break;
        case  5: ShowOLED("[Pitch reverse]", 0,4,1); PlaySound(SetPitchReverseMP3,8); break;
        case  6: ShowOLED("[Always sound]", 0,4,1); PlaySound(SetAlwaysSoundMP3,8); break;    
        case  7: ShowOLED("[Lowest pitch]", 0,4,1); PlaySound(SetLowestPitchMP3,8); break;
        case  8: ShowOLED("[Higest pitch]", 0,4,1); PlaySound(SetHigestPitchMP3,8); break;
        case  9: ShowOLED("[Auto adj. window]", 0,4,1); PlaySound(SetAutoAdjustWindowMP3,8); break;
        case 10: ShowOLED("[Get ready time]", 0,4,1); PlaySound(SetTimeToGetReadyMP3,8); break;
        case 11: ShowOLED("[Average]", 0,4,1); PlaySound(SetSensorAverageCountMP3 ,8); break;
        case 12: ShowOLED("[Sample speed]", 0,4,1); PlaySound(SetSampleSpeedMP3,8); break;
        case 13: ShowOLED("[Pitch step]", 0,4,1); PlaySound(SetPitchStepMP3,8); break;
        case 14: ShowOLED("[Factory settings]", 0,4,1); PlaySound(RestoreFactorySettingsMP3,8); break;                
      } 
      //Serial.println("Option Nr: "+String(OptionNr)); 
      if (KeyVal() == Up)       if (OptionNr < 14) OptionNr+= 1;
      if (KeyVal() == Down)     if (OptionNr >  0) OptionNr-= 1;
      if (KeyVal() == Select)   Esc= true; 
      if (KeyVal() == Right)    PlayHelp(OptionNr);  
      if (KeyVal() == Left)     return;    
    }
    ShowOLED("Please wait...", 0,4,1);
    PlaySound(UseArrowKeysMP3,6);
    switch(OptionNr) {
      case 0: Esc= false;
              while (!Esc) {
                 #ifdef OLED
                    ShowOLED("Volume: "+(String)MP3Volume, 0,4,1);
                 #endif
                 PlayNumber(MP3Volume, 1);
                 if (KeyVal() == Up)       if (MP3Volume < 30) MP3Volume+= 1;
                 if (KeyVal() == Down)     if (MP3Volume >  5) MP3Volume-= 1;
                 if (KeyVal() == Right)    PlayHelp(OptionNr); 
                 if (KeyVal() == Left)     Esc= true;
                 if (KeyVal() == Select)   WriteConfig();
                 SetVolume(MP3Volume);
              }
              break;
      case 1: Esc= false;
              while (!Esc) {
                 #ifdef OLED
                    ShowOLED("Min. Sensor: "+(String)MinValue, 0,4,1);
                 #endif
                 PlayNumber(MinValue, 1);
                 if (KeyVal() == Up)       if (MinValue < (MaxValue-20)) MinValue+= 5;
                 if (KeyVal() == Down)     if (MinValue > 10) MinValue-= 5;
                 if (KeyVal() == Right)    PlayHelp(OptionNr); 
                 if (KeyVal() == Left)     Esc= true;
                 if (KeyVal() == Select)   WriteConfig();
              }
              break;
      case 2: Esc= false;
              while (!Esc) {
                 #ifdef OLED
                    ShowOLED("Max. Sensor: "+(String)MaxValue, 0,4,1);
                 #endif
                 PlayNumber(MaxValue, 1);
                 if (KeyVal() == Up)       if (MaxValue < 990) MaxValue+= 5;
                 if (KeyVal() == Down)     if (MaxValue > (MinValue+20)) MaxValue-= 5;
                 if (KeyVal() == Right)    PlayHelp(OptionNr); 
                 if (KeyVal() == Left)     Esc= true;
                 if (KeyVal() == Select)   WriteConfig();
              }
              break;
      case 3: Esc= false;
              while (!Esc) {
                 #ifdef OLED
                    ShowOLED("Threshold: "+(String)ThresholdWindow, 0,4,1);
                 #endif
                 PlayNumber(ThresholdWindow, 1);
                 if (KeyVal() == Up)       if (ThresholdWindow < 190) ThresholdWindow+= 10;
                 if (KeyVal() == Down)     if (ThresholdWindow > 10)  ThresholdWindow-= 10;
                 if (KeyVal() == Right)    PlayHelp(OptionNr); 
                 if (KeyVal() == Left)     Esc= true;
                 if (KeyVal() == Select)   WriteConfig();
              }
              break;     
      case 4: Esc= false;
              while (!Esc) {
                 #ifdef OLED
                    ShowOLED("Pitch Curve: "+(String)Curve, 0,4,1);
                 #endif
                 PlayNumber(Curve, 1);
                 if (KeyVal() == Up)       if (Curve < 5) Curve++;
                 if (KeyVal() == Down)     if (Curve > 0) Curve--;
                 if (KeyVal() == Right)    PlayHelp(OptionNr); 
                 if (KeyVal() == Left)     Esc= true;
                 if (KeyVal() == Select)   WriteConfig();
              }
              break;  
      case 5: Esc= false;
              while (!Esc) {
                 if (PitchRev) PlaySound(PitchReverseEnabledMP3,3); else PlaySound(PitchReverseDisabledMP3,3); 
                 if (KeyVal() == Up)       if (PitchRev < 1) PitchRev++;
                 if (KeyVal() == Down)     if (PitchRev > 0) PitchRev--;
                 if (KeyVal() == Right)    PlayHelp(OptionNr); 
                 if (KeyVal() == Left)     Esc= true;
                 if (KeyVal() == Select)   WriteConfig();
              }
              break;
      case 6: Esc= false;
              while (!Esc) {
                 if (AlwaysSound) PlaySound(AlwaysSoundEnabledMP3,3); else PlaySound(AlwaysSoundDisabledMP3,3); 
                 if (KeyVal() == Up)       if (AlwaysSound < 1) AlwaysSound++;
                 if (KeyVal() == Down)     if (AlwaysSound > 0) AlwaysSound--;
                 if (KeyVal() == Right)    PlayHelp(OptionNr); 
                 if (KeyVal() == Left)     Esc= true;
                 if (KeyVal() == Select)   WriteConfig();
              }
              break;
      case 7: Esc= false;
              while (!Esc) {
                 #ifdef OLED
                    ShowOLED("Lo Pitch: "+(String)LowTone+" Hz", 0,4,1);
                 #endif
                 PlayTone(LowTone,0);
                 delay(300);
                 if (KeyVal() == Down)     if (LowTone > 50) LowTone-= 50;
                 if (KeyVal() == Up)       if (LowTone < (HighTone-100)) LowTone+= 50;
                 if (KeyVal() == Right)    PlayHelp(OptionNr); 
                 if (KeyVal() == Left)     Esc= true;
                 if (KeyVal() == Select)   WriteConfig();
              }
              PlaySound(LowPitchSetAtMP3,4); PlayNumber(LowTone, 0); PlaySound(HertzMP3,2);
              break;    
      case 8: Esc= false;
              while (!Esc) {
                 #ifdef OLED
                    ShowOLED("Hi Pitch: "+(String)HighTone+" Hz", 0,4,1);
                 #endif
                 PlayTone(HighTone,0);
                 delay(300);
                 if (KeyVal() == Down)     if (HighTone > (LowTone+100)) HighTone-= 50;
                 if (KeyVal() == Up)       if (HighTone < 9000) HighTone+= 50;
                 if (KeyVal() == Right)    PlayHelp(OptionNr); 
                 if (KeyVal() == Left)     Esc= true;
                 if (KeyVal() == Select)   WriteConfig();
              }
              PlaySound(HighPitchSetAtMP3,4); PlayNumber(HighTone, 0); PlaySound(HertzMP3,2);
              break;  
      case 9: Esc= false;
              while (!Esc) {
                #ifdef OLED
                    ShowOLED("Auto adj. window: "+(String)AutoAdjustWindow, 0,4,1);
                 #endif
                 PlayNumber(AutoAdjustWindow, 1);
                 if (KeyVal() == Down)     if (AutoAdjustWindow > 50)   AutoAdjustWindow-= 10;
                 if (KeyVal() == Up)       if (AutoAdjustWindow < 300)  AutoAdjustWindow+= 10;
                 if (KeyVal() == Right)    PlayHelp(OptionNr); 
                 if (KeyVal() == Left)     Esc= true;
                 if (KeyVal() == Select)   WriteConfig();
              }
              break;
     case 10: Esc= false;
              while (!Esc) {
                 #ifdef OLED
                    ShowOLED("Get ready time: "+(String)GetReadyTime+" s.", 0,4,1);
                 #endif
                 PlayNumber(GetReadyTime, 1);
                 if (KeyVal() == Down)     if (GetReadyTime > 2)       GetReadyTime-= 1;
                 if (KeyVal() == Up)       if (GetReadyTime < 20)      GetReadyTime+= 1;
                 if (KeyVal() == Right)    PlayHelp(OptionNr); 
                 if (KeyVal() == Left)     Esc= true;
                 if (KeyVal() == Select)   WriteConfig();
              }
              break;             
     case 11: Esc= false;
              while (!Esc) {
                #ifdef OLED
                    ShowOLED("Averaging: "+(String)AvgModes[AverageValue], 0,4,1);
                 #endif
                 if (AverageValue == 0)    PlaySound(DisableMP3,4); //beter uitleg? disabled als niet actief?
                 if (AverageValue == 1)    PlaySound(LowMP3,4);
                 if (AverageValue == 2)    PlaySound(MediumMP3,4); 
                 if (AverageValue == 3)    PlaySound(HighMP3,4);
                 if (KeyVal() == Down)     if (AverageValue > 0)       AverageValue-= 1;
                 if (KeyVal() == Up)       if (AverageValue < 3)       AverageValue+= 1;
                 if (KeyVal() == Right)    PlayHelp(OptionNr); 
                 if (KeyVal() == Left)     Esc= true;
                 if (KeyVal() == Select)   WriteConfig();
              }
              break;
     case 12: Esc= false;
              while (!Esc) {
                 #ifdef OLED
                    ShowOLED("Speed: "+(String)SampleModes[SampleSpeed], 0,4,1);
                 #endif
                 if (SampleSpeed == 0)     PlaySound(FastMP3,4);
                 if (SampleSpeed == 1)     PlaySound(MediumMP3,4); 
                 if (SampleSpeed == 2)     PlaySound(SlowMP3,4);
                 if (SampleSpeed == 3)     PlaySound(SlowestMP3,4);
                 if (KeyVal() == Down)     if (SampleSpeed > 0)       SampleSpeed-= 1;
                 if (KeyVal() == Up)       if (SampleSpeed < 3)       SampleSpeed+= 1;
                 if (KeyVal() == Right)    PlayHelp(OptionNr); 
                 if (KeyVal() == Left)     Esc= true;
                 if (KeyVal() == Select)   WriteConfig();
              }
              break; 
     case 13: Esc= false;
              while (!Esc) {
                #ifdef OLED
                    ShowOLED("Pitch step: "+(String)YesNoArr[PitchStep], 0,4,1);
                 #endif
                 if (PitchStep) PlaySound(PitchStepEnabledMP3,4); else PlaySound(PitchStepDisabledMP3,4); 
                 if (KeyVal() == Up)       if (PitchStep < 1) PitchStep++;
                 if (KeyVal() == Down)     if (PitchStep > 0) PitchStep--;
                 if (KeyVal() == Right)    PlayHelp(OptionNr); 
                 if (KeyVal() == Left)     Esc= true;
                 if (KeyVal() == Select)   WriteConfig();
              }
              break;          
     case 14: Esc= false;
              boolean SetToDefaults= false; 
              while (!Esc) {
                 #ifdef OLED
                    ShowOLED("Factory reset: "+(String)YesNoArr[SetToDefaults], 0,4,1);
                 #endif
                 if (SetToDefaults) PlaySound(RestoreFactoryDefaultsEnabledMP3,4); else PlaySound(RestoreFactoryDefaultsDisabledMP3,4); 
                 if (KeyVal() == Up)        if (SetToDefaults < 1) SetToDefaults= true;
                 if (KeyVal() == Down)      if (SetToDefaults > 0) SetToDefaults= false;
                 if (KeyVal() == Right)     PlayHelp(OptionNr); 
                 if (KeyVal() == Left)     Esc= true;
                 if (KeyVal() == Select)   WriteConfig();
              }
              if (SetToDefaults) {
                  PlaySound(AllSettingsResetToDefaultsMP3,4);
                  EEPROM.write(0,0); 
                  Reset(); 
              }
              break;                        
    }
    ShowOLED("Leaving menu", 0,4,1);
    PlayTone(0,0);  
    PlaySound(ExitOptionsMenuMP3,4);
    if (LogMode==2) OutputLog();
    if (LogMode==1) LogCounter= 0;  
    ClearOLED();
    ShowValuesOLED();
  }
#else
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
      ShowStatusLCD();
    }
    else Display= 1;
    delay(500); 
  }

  void ShowValues() {
    #ifdef DEBUG-LCD
      ShowLCD("Sen:"+WordToStr(Reading,4)+ " L:"+WordToStr(LoopCounter,5),0, true);
      LoopCounter= 0;
    #else  
      ShowLCD("Sen:"+WordToStr(Reading,4)+ " Low:"+WordToStr(LowestReading,3),0, true);
    #endif  
    ShowLCD("Min:"+WordToStr(MinValue,4)+" Max:"+WordToStr(MaxValue,3),1, true);
  } 

  void ShowStatusLCD() {
    if (LogMode) ShowLCD("Running..      *",0, false);
    else ShowLCD("Running..",0, false);
    ShowLCD("L:"+WordToStr(MinValue,3)+" H:"+WordToStr(MaxValue,3)+" C:"+WordToStr(Curve,2),1, true);
  } 

  byte KeyVal() {                     
    word Val= analogRead(0);
    if (Val > 900)  return 0; //None     todo change code to map function
    if (Val > 600)  return 1; //Select
    if (Val > 400)  return 2; //Left
    if (Val > 200)  return 3; //Down
    if (Val > 80 )  return 4; //Up
    return                 5; //Right
  }
  
  void Menu() {
    PlayTone(0,0);
    delay(500);
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
      ShowLCD("Lead in: "+(String)ThresholdWindow, 1, true);
      delay(300);
      if (KeyVal() == Down)   if (ThresholdWindow > 10)  ThresholdWindow-= 10;
      if (KeyVal() == Up)     if (ThresholdWindow < 190) ThresholdWindow+= 10;
      if (KeyVal() == Select) Esc= true;
    }
    Esc= false;  
    while (!Esc) {
      ShowLCD("GetReadyTime: "+(String)GetReadyTime, 1, true);
      delay(300);
      if (KeyVal() == Down)   if (GetReadyTime > 2)    GetReadyTime--;
      if (KeyVal() == Up)     if (GetReadyTime < 20)   GetReadyTime++;
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
    #ifdef DDS9833   
      while (!Esc) {
        ShowLCD("Shape: "+(String)WaveShapes[WaveShape], 1, true);
        delay(300);
        if (KeyVal() == Down)   if (WaveShape > 0) WaveShape--;
        if (KeyVal() == Up)     if (WaveShape < 2) WaveShape++;
        if (KeyVal() == Select) Esc= true;
      }
      Esc= false; 
    #endif 
    while (!Esc) {
      ShowLCD("Pitch rev: "+(String)YesNoArr[PitchRev], 1, true);
      delay(300);
      if (KeyVal() == Down)   if (PitchRev > 0) PitchRev--;
      if (KeyVal() == Up)     if (PitchRev < 1) PitchRev++;
      if (KeyVal() == Select) Esc= true;
    }
    Esc= false;   
    while (!Esc) {
      ShowLCD("Always tone: "+(String)YesNoArr[AlwaysSound], 1, true);
      delay(300);
      if (KeyVal() == Down)   if (AlwaysSound > 0) AlwaysSound--;
      if (KeyVal() == Up)     if (AlwaysSound < 1) AlwaysSound++;
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
      PlayTone(LowTone,0);
      delay(300);
      if (KeyVal() == Down)   if (LowTone > 50) LowTone-= 50;
      if (KeyVal() == Up)     if (LowTone < (HighTone-100)) LowTone+= 50;
      if (KeyVal() == Select) Esc= true;
    } 
    Esc= false; 
    PlayTone(0,0);
    while (!Esc) {
      ShowLCD("HighTone: "+(String)HighTone, 1, true);
      PlayTone(HighTone,0);
      delay(300);
      if (KeyVal() == Down)   if (HighTone > (LowTone+100)) HighTone-= 50;
      if (KeyVal() == Up)     if (HighTone < 9000) HighTone+= 50;
      if (KeyVal() == Select) Esc= true;
    }
    Esc= false;
    PlayTone(0,0);
    Display=  EEPROM.read(18); //read value from rom setting instead of global  
    while (!Esc) {
      ShowLCD("Display: "+(String)DisplayType[Display], 1, true);
      delay(300);
      if (KeyVal() == Down)   if (Display > 0) Display--;
      if (KeyVal() == Up)     if (Display < 2) Display++;
      if (KeyVal() == Select) Esc= true;
    }
    Esc= false;
    while (!Esc) {
      ShowLCD("Average: "+(String)AvgModes[AverageValue], 1, true);
      delay(300);
      if (KeyVal() == Down)   if (AverageValue > 0) AverageValue--;
      if (KeyVal() == Up)     if (AverageValue < 3) AverageValue++;
      if (KeyVal() == Select) Esc= true;
    }
    Esc= false;
    while (!Esc) {
      ShowLCD("Samples: "+(String)SampleModes[SampleSpeed], 1, true);
      delay(300);
      if (KeyVal() == Down)   if (SampleSpeed > 0) SampleSpeed--;
      if (KeyVal() == Up)     if (SampleSpeed < 3) SampleSpeed++;
      if (KeyVal() == Select) Esc= true;
    }
    Esc= false;
    while (!Esc) {
      ShowLCD("Pitch Step: "+(String)YesNoArr[PitchStep], 1, true);
      delay(300);
      if (KeyVal() == Down)   if (PitchStep > 0) PitchStep--;
      if (KeyVal() == Up)     if (PitchStep < 1) PitchStep++;
      if (KeyVal() == Select) Esc= true;
    }    
    Esc= false;  
    while (!Esc) {
      ShowLCD("Logging: "+(String)LoggingModes[LogMode], 1, true);
      delay(300);
      if (KeyVal() == Down)   if (LogMode > 0) LogMode--;
      if (KeyVal() == Up)     if (LogMode < 2) LogMode++;
      if (KeyVal() == Select) Esc= true;
    }
    Esc= false;
    boolean SetToDefaults= false;   
    while (!Esc) {
      ShowLCD("Reset ALL: "+(String)YesNoArr[SetToDefaults], 1, true);
      delay(300); 
      if (KeyVal() == Down)   if (SetToDefaults > 0) SetToDefaults= false;
      if (KeyVal() == Up)     if (SetToDefaults < 1) SetToDefaults= true;
      if (KeyVal() == Select) Esc= true;
    }
    if (SetToDefaults) { 
      EEPROM.write(0,0); 
      Reset(); 
    }
    if (LogMode==2) OutputLog();
    if (LogMode==1) LogCounter= 0;  
    WriteConfig();
    ShowLCD("Saved......",1,true);
    delay(3000);
    lcd.clear();
    if (!Display) ShowStatusLCD(); 
  }
#endif

void setup() {
  //Pinmode for audio pin????  
  //pinMode(AudioPin, OUTPUT);
  pinMode(SensorPin, INPUT);
  #ifdef SPEECH
    InitKeyPad();
    #ifdef OLED
       oled.begin(&Adafruit128x64, I2C_ADDRESS);
       ClearOLED();
       ShowOLED("Initialize...", 0,4,2);
    #endif
  #else  
    lcd.begin(16, 2);
    lcd.clear();
    ShowLCD("Starting...",0, true);
    delay(1000);
  #endif
  Serial.begin(9600);
  if (EEPROM.read(0)==1) ReadConfig();
  else WriteConfig();
  PrevLogTime= millis();
  LowestReading= MaxValue;
  WarningReading= MinValue;
  #ifdef SPEECH
    mySerial.begin(9600);
    delay(500);//Wait chip initialization is complete
    SendMP3Command(CMD_SEL_DEV, DEV_TF);//select the TF card  
    delay(500);
    SetVolume(MP3Volume);
    PlaySound(WelcomeMP3,4);
    ShowValuesOLED();
  #endif 
  #ifdef DDS9833
    pinMode(FSyncPin, OUTPUT); //CS for DDS module
    SPI.begin(); //Start SPI for DDS module
    SPI.setDataMode(SPI_MODE2);  delay(50); 
    AD9833reset();     // Reset AD9833.
    SetFrequency(0);   // Set the frequency
  #endif  
  #ifndef SPEECH
    PlayMelody();
    #ifdef DDS9833
      ShowLCD("Kedok "+(String)Version+" DDS",0, true);
    #else  
      ShowLCD("Kedok "+(String)Version,0, true);
    #endif  
    ShowLCD(Owner,1, false);
    delay(2000);
    if (!Display) ShowStatusLCD();
    PrevDispTime= millis();
  #endif  
}

void loop() {
  Reading= ReadValue(AverageValue);
  if (Display) {
     if ((millis()-PrevDispTime) > DispUpdTime) {
        ShowSensorOLED(Reading);
        PrevDispTime= millis();
     }
  } 
   
 //--------Kernel part-------- 
  if (Reading < MinValue) {
     LowReadWarning(); 
  }else if (Reading < MaxValue) {
     if (PitchRev) AudioTone= fscale(MinValue,MaxValue,LowTone,HighTone,Reading,Curve);
     else AudioTone= fscale(MinValue,MaxValue,HighTone,LowTone,Reading,Curve);
     if (PitchStep) { if (AudioTone > (HighTone-PitchStepValue)) AudioTone= AudioTone+PitchStepValue; }//PitchStep around the 8 on the target card.
     PlayTone(AudioTone,0);
     if (LogMode) WriteLog(Reading);
  }else if (Reading < (MaxValue+ThresholdWindow)) {
     if (PitchRev) PlayTone(HighTone + 200,0);
     else PlayTone(LowTone-30,0);  
  }else if (AlwaysSound) PlayTone(1,0); // 1 Hz if not on card. 
        else PlayTone(0,0); // Turn off the tone. 
//-----------------

  if (Reading < LowestReading) LowestReading= Reading; 
  #ifdef SPEECH
    KeyPressed= KeyVal();
    if (KeyPressed) {  
      if (KeyPressed == Select)    Menu();
      if (KeyPressed == Right)     AutoAdjust();
      if (KeyPressed == Left)      {ClearOLED(); delay(300); Display= !Display;}
      if (KeyPressed == Down)      MoveSensorWindow(0-MoveSensorWindowStepSize);
      if (KeyPressed == Up)        MoveSensorWindow(MoveSensorWindowStepSize);
      //ToDo add more
    }
  #else
    KeyPressed= ReadKey();
    if (KeyPressed) {  
      if (KeyPressed == Select)    Menu();
      if (KeyPressed == Right)     LowestReading= MaxValue;
      if (KeyPressed == RightLong) AutoAdjust();
      if (KeyPressed == Left)      if (Display) Screen(Disable); else Screen(Enable);
      if (KeyPressed == Down)      MoveSensorWindow(0-MoveSensorWindowStepSize);
      if (KeyPressed == DownLong)  MoveSensorWindowToLowestRead();
      if (KeyPressed == Up)        MoveSensorWindow(MoveSensorWindowStepSize);
    }  
  #endif
  if (SampleSpeed) delay((1 << SampleSpeed-1)*5); //0,5,10,20  
  #ifdef DEBUG-LCD
    LoopCounter++;
  #endif  
}



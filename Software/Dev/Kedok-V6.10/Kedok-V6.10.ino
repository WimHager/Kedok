#if 1                     //Needed for sketch to respect preprocessor directives
__asm volatile ("nop");
#endif

//====================================Compiler options=============================================================
//#define SENSOR_TEST   //Enable test mode for test

const   char      Version[5]="6.10";
const   char      Owner[16]= "DEMO";
const   char      SerialNr[23]=  "";

word    MP3Language=       0X0200; //0X0100 English 0X0200 Dutch

//=================================================================================================================

/*
    Kedok (audio aiming device), Copyright 2018 Wim Hager
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
 Pitch divider default is window size 200. option to divide it is 200..100 20 was to extreme.  

 Bug, if kalibration is done on a realy bright light source weid value and sound give lowest pich is with Min sensor value
 Batt check funtction
 Add standing position trainer. Play sound, no sound for 5 sec than sound again in a repeat mode. (Nanne Jonker)
 add compile date maybe in eeprom as serial number Serial.println( "Compiled: " __DATE__ ", " __TIME__ ", " __VERSION__);
 if sensor read is < 50 try to switch off. Probably headset connected to the unit.
 Fonts naming
 Language selection in menu
 Pitch reverse needed?
 Better English voice
 Had a bug with filter, not sure. was in maximal, gave lower minimal settings, Benjamin mentioned something like occilating in filter
 Pre calculate average value and don't calculate every time
 Reduce fonts to save memory
 Threshold tests, Doing tests at the range what the settings really do
 Start to add a i2c ADS1015 12-Bit ADC - 4 Channel 
 Reset all if version updated
 Better Auto adjust
 Change delay's in speech, some are utterly slow alo improve saying values (remove "value is" in some cases) 
 Beginners mode, wide window
 Add change owner name with buttons.
 Writeconfig with or without voice?
 Think about new aiming system, running window.
 Optimize keyboard read code.
 
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
 V6.00 04-06-2017
 04-06-2017 New Kedok, Voice and O-Led display
 05-06-2017 Removed all LCD, DDS code for planning to only to support Voice and OLED in the near future
 06-06-2017 Huge code cleaning, added Sensor value showing
 15-06-2017 Remove all logging modes.
 15-06-2017 New EMA filter
 24-09-2017 Presets for novice or experienced shooter
 V6.10 24-12-2017
 25-12-2017 Added tone jump setting, you can move the threshold of the jump from 0 to the 9 ring
 31-12-2017 Removed Pitch revers, no-one use it.
 31-12-2017 Added Stance trainer
*/

//Note Audio pin 10. 
//Opto resistor 68K
//Loops FAST:     1600
//      MEDIUM:    180
//      SLOW:      100
//      SLOWEST:    50

/*  Nano speech =====================================================================================
 
  To Pot volume
       ^
       |
      ---             To Keyboard
      | | 1K2              ^
      | |                  |
      ---           ---------------
       |            |             |
       |            |             |                    
[0][0][X][0][0][0][X][X][X][X][X][X][0][0][0]
      D10          D6 D5 D4 D3 D2 GND

                                GND <---[X][0]
                                 TX <---[X][0]         To MP3 board
                                VCC <---[X][X]---> RX


                  A3             VCC   GND
[0][0][0][0][0][0][X][X][X][0][0][X][0][X][0]
                   |  |  |        |     |              |--> To Charger GND
  Sensor jack <----   |  |       ---    ---------------|
                      |  |       | |                   |--> To Sensor Jack
                      v  v       v v
            Display SDA SDC      To Charger, Sensor jack , Display
                                
*/

////////////////////////////////////////////

#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include <SoftwareSerial.h>
#include <NewTone.h>
#include <EEPROM.h>

#define I2C_ADDRESS 0x3C
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
//#define SetPitchReverseMP3                  24  
#define SetLowestPitchMP3                   21  
#define SetHigestPitchMP3                   20  
#define SetAutoAdjustWindowMP3              19
#define SetTimeToGetReadyMP3                61 
#define SetSampleSpeedMP3                   59
#define SetVolumeMP3                        27 
#define SetAlwaysSoundMP3                   43
#define SetSensorAverageCountMP3            51
#define SetSensorAverageFilterMP3           71        
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
//#define PitchReverseDisabledMP3             37 //removed option since 6.10
//#define PitchReverseEnabledMP3              36
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
#define KedokUsageMP3                       72
#define LoadShooterTypeMP3                  73 
#define LoadNoviceUserMP3                   74
#define LoadAdvancedUserMP3                 75
#define SavedNoviceUserMP3                  76
#define SavedAdvancedUserMP3                77
#define SecondsMP3                          78
#define SetPitchStepValueMP3                79
#define StartPositionTrainerMP3             80
#define LayRifleOnStandTakeAimMP3           81
#define AimingTrainerStartedMP3             82
#define PressMenuToStartMP3                 83

#define HelpSetMinimalSensorValueMP3       123  
#define HelpSetMaximalSensorValueMP3       122  
#define HelpSetSensorThresholdValueMP3     125  
#define HelpSetSoundCurveValueMP3          126  
//#define HelpSetPitchReverseMP3             124  //removed option since 6.10
#define HelpSetLowestPitchMP3              121  
#define HelpSetHigestPitchMP3              120  
#define HelpSetAutoAdjustWindowMP3         119
#define HelpSetTimeToGetReadyMP3           161 
#define HelpSetSampleSpeedMP3              159
#define HelpSetVolumeMP3                   127 
#define HelpSetSensorAverageCountMP3       151
#define HelpSetSensorAverageFilterMP3      171
#define HelpSetAlwaysSoundMP3              143  
#define HelpRestoreFactorySettingsMP3      118
#define HelpSetPitchStepMP3                168
#define HelpKedokUsageMP3                  172 
#define HelpKedokExtHelpMP3                173
#define HelpLoadShooterTypeMP3             174
#define HelpSetPitchStepValueMP3           179
#define HelpStanceTrainerMP3               180


#define Key1Pin                              2
#define Key2Pin                              3
#define Key3Pin                              4
#define Key4Pin                              6
#define Key5Pin                              5   

SoftwareSerial mySerial(ARDUINO_RX, ARDUINO_TX);
static int8_t     Send_buf[8]= {0};

SSD1306AsciiAvrI2c oled;

byte    AudioPin=                    10;
byte    SensorPin=                   A3;

const  byte None=                     0;
const  byte Select=                   1;
const  byte Down=                     3;
const  byte Up=                       2;
const  byte Right=                    4;
const  byte Left=                     5;

const  byte       Value=              1;
const  byte       Bar=                2;
const  boolean    Disable=         true;
const  boolean    Enable=         false;

word    MinValue=                   100;
word    MaxValue=                   800;

word    LowTone=                    100; 
word    HighTone=                  1750;

byte    Curve=                        0;
byte    PitchRev=                 false;
byte    AlwaysSound=              false; 
byte    NoviceUser=                true;
  
word    AutoAdjustWindow=           200; // Normal pistol card size
byte    ThresholdWindow=            150; 
byte    GetReadyTime=                15; // 15 Seconds Also update SetNoviceMode !!!
word    PitchStepValue=               0; // Default off. Range 9 div sensor range
word    PitchStepThreshold=           0; // Threshold for the tone jump in the output.
word    MoveSensorWindowStepSize=    10; // In/Dec MoveWindow steps if Up or Down is pressed Also update SetNoviceMode !!!

long    LoopCounter=                  0;
byte    MP3Volume=                   25;
byte    AverageValue=                 0; //Read 0 values to average, Default Minimal
byte    SampleSpeed=                  0; //Loop delay 0,5,10,20
byte    DisplaySensorReadings=        0; 
word    CalibrationTime=           2000; //Calibration timeout if there are no better readings readby the sensor Also update SetNoviceMode !!

struct SettingsObj {
  word MinValue;
  word MaxValue;
  byte ThresholdWindow;
  byte Curve;
  byte PitchRev;
  byte AlwaysSound;  
  word AutoAdjustWindow;
  byte GetReadyTime;        //Timeout for get ready with auto adjust
  word LowTone;
  word HighTone;
  byte MP3Volume;
  byte AverageValue;
  byte SampleSpeed;
  byte PitchStepValue;
  byte NoviceUser;
};  

word    DispUpdTime=          1000; //1 sec Screen update 
const char    *YesNoArr[]=          {"N", "Y"};
const char    *AvgModes[]=          {"None", "Low", "Medium", "Maximal"};
const char    *SampleModes[]=       {"Fast", "Medium", "Slow", "Slowest"};
const char    *EnableDisableArr[]=  {"Disable","Enable"};
long    PrevDispTime;

word    Reading;
word    AudioTone;
word    LowestReading;
word    WarningReading;
byte    KeyPressed;

void Reset() {
  asm volatile ("  jmp 0");
} 

#ifdef SENSOR_TEST
  #include "QuickStats.h"  
  QuickStats stats;
  float Readings[100];
  int   NumReadings= 100;
  
  void SensorTest() {
    ClearOLED();   
    ShowOLED("Test mode!!", 0,3,1);
    PlayTone(440,0);
    delay(3000);
    Serial.begin(9600);
    while (!KeyVal()) {
       for(int X= 0; X<NumReadings; X++) {
         Readings[X]= ReadValue(0); //0 is no filter
         delay(10); //One sec sample
       }
       ShowOLED("Average: "+(String)stats.average(Readings,NumReadings), 0,2,3);
       ShowOLED("Minimum: "+(String)stats.minimum(Readings,NumReadings), 0,3,3);
       ShowOLED("Maximum: "+(String)stats.maximum(Readings,NumReadings), 0,4,3);
       ShowOLED("Std dev: "+(String)stats.stdev(Readings,NumReadings), 0,5,3);
       ShowOLED("Std err: "+(String)stats.stderror(Readings,NumReadings), 0,6,3);
       ShowOLED("Co Vari: "+(String)stats.CV(Readings,NumReadings), 0,7,3);
       DelaySecIntr(5, true);
    }
    ClearOLED();
    ShowOLED("Sensor to USB", 0,3,1);       
    while (true) {
      Serial.println((String)ReadValue(0));
    }
}
#endif

void WriteConfig() {
  EEPROM.write(0,1);
  SettingsObj CurSettings= {
    MinValue,
    MaxValue,
    ThresholdWindow,
    Curve,
    PitchRev,
    AlwaysSound,
    AutoAdjustWindow,
    GetReadyTime,
    LowTone,
    HighTone,
    MP3Volume,
    AverageValue,
    SampleSpeed,
    PitchStepValue,
    NoviceUser,
  };  
  EEPROM.put(1, CurSettings);
  ShowOLED("Saving config..", 0,4,1);
  PlaySound(DataSettingsSavedMP3,3,0);
}

void ReadWriteDate(byte Write, byte Install) {
  if (Install) if (Write) EEPROM.put(1000, __DATE__); else EEPROM.get(1000, __DATE__);
  else if (Write) EEPROM.put(1020, __DATE__); else EEPROM.get(1020, __DATE__);
}

void ReadConfig(byte OnlyRead) {
  SettingsObj CurSettings;
  EEPROM.get(1, CurSettings); //Read all settings
  MinValue= CurSettings.MinValue;
  MaxValue= CurSettings.MaxValue;
  ThresholdWindow= CurSettings.ThresholdWindow;
  GetReadyTime= CurSettings.GetReadyTime;
  Curve= CurSettings.Curve;
  PitchRev= CurSettings.PitchRev;
  AlwaysSound= CurSettings.AlwaysSound;
  AutoAdjustWindow= CurSettings.AutoAdjustWindow;
  LowTone= CurSettings.LowTone;
  HighTone= CurSettings.HighTone;
  MP3Volume= CurSettings.MP3Volume;
  AverageValue= CurSettings.AverageValue;
  SampleSpeed= CurSettings.SampleSpeed;
  PitchStepValue= CurSettings.PitchStepValue;
  NoviceUser= CurSettings.NoviceUser;
  if (!OnlyRead) {
    ShowOLED("Reading config..", 0,4,1);
    PlaySound(ReadConfigMP3,4,0);
  }  
}

long ReadVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring
  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both
  long result = (high<<8) | low;
  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
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

//When α is close to 1, dampening is quick and when α is close to 0, dampening is slow. in Benjamins test, 0.2 was nice for him
word EmaFilter(word Inp, float Alpha) { //Alpha in range of 9..1  
  static word EmaVal;
  Alpha= Alpha/10;
  EmaVal= (Alpha*Inp) +((1-Alpha)*EmaVal);
  return EmaVal;
}

word ReadValue(word AvgVal) { 
  if (!AverageValue) return analogRead(SensorPin); // For the sake of speed
  return EmaFilter(analogRead(SensorPin),10-(AvgVal*3)); //Creates 1=7 Low,2=6 Medium, 3=1 Maximal //it maybe can be pre calculated!!
}

void PlayTone(word Tone, word Duration) {
  if (Tone == 0) noNewTone(AudioPin);
  else NewTone(AudioPin, Tone, Duration);
}

void Beep(byte Beeps, word Pitch) {
  for (byte X=0; X<Beeps; X++) {
     PlayTone(Pitch,200);
     delay(400);
  }
}

void LowReadWarning() {
  PlayTone(0,0); // Turn off the tone.
  WarningReading= Reading;
  Beep(3,300);
  ClearOLED(); 
  ShowOLED("Lower settings", 0,4,1);
  if (NoviceUser) PlaySound(LowerMinimalSettingMP3,3,0);
  ClearOLED();
  UpdateDisplay();
}  

String WordToStr(word Inp, byte Size) {
  String Str;
  for (byte C= 0; C<Size; C++) Str+= "0";
  String WordStr= (String)Inp;
  byte Len= WordStr.length();
  for (byte C= 0; C<Len; C++) Str[C+Str.length()-Len]= WordStr[C];
  return Str; 
}  

void MoveSensorWindow(int Val) {
  MinValue= MinValue + Val;
  MaxValue= MaxValue + Val;
  if (NoviceUser) {
       if (Val < 0) PlaySound(SensorReducedMP3,3,0);
       else  PlaySound(SensorIncreasedMP3,3,0);
  }   
  ClearOLED();
  WriteConfig();
  DisplaySensorReadings= false;
  UpdateDisplay();
}  


void StanceTrainer() {
  word TimeOutCounter=   0; 
  ClearOLED();
  ShowOLED("Stance training..", 0,4,1);
  //Aiming trainer started, take aim.
  PlaySound(AimingTrainerStartedMP3, 9,0); //to stop here
  while (!KeyVal()) {
     Beep(1,440); 
     delay(300);
     while (TimeOutCounter < (6*90)) { //5 seconds
       Reading= ReadValue(0);
       AudioTone= fscale(100,900,HighTone,LowTone,Reading,0);
       PlayTone(AudioTone,0);
       TimeOutCounter++;
       delay(10);
    }
    PlayTone(0,0); // Turn off the tone. 
    //Lay rifle on the stand, reposition and start to take aim again, press any key to stop.    
    PlaySound(LayRifleOnStandTakeAimMP3,14,0);
    TimeOutCounter=   0;
  }  
}

void AutoAdjust() {
  word Reading;
  word LowestReading= 1023;
  word TimeOutCounter=   0; 

  ClearOLED();
  ShowOLED("Prepare for aiming..", 0,4,1);
  PlaySound(AutoAdjustStartedMP3,4,0);
  PlaySound(PrepareWeaponForAimingMP3,4,0);
 
  while (TimeOutCounter < (GetReadyTime*90)) {
     Reading= ReadValue(0);
     AudioTone= fscale(100,900,HighTone,LowTone,Reading,0);
     PlayTone(AudioTone,0);
     TimeOutCounter++;
     delay(10);
  }
  delay(300); 
  Beep(1,2000); 
  delay(300);

  ShowOLED("Start to aim now!", 0,4,1);
  PlaySound(StartToAimNowMP3,5,0);
  TimeOutCounter= 0;
  while (KeyVal() == None) {
    Reading= ReadValue(0);
    AudioTone= fscale(100,900,HighTone,LowTone,Reading,0);
    PlayTone(AudioTone,0);
    if (Reading < LowestReading) {
      if (Reading > 50) LowestReading= Reading; //Very bright light came in, avoid negative value overload
      TimeOutCounter= 0;
    }  
    TimeOutCounter++;
    if (TimeOutCounter > CalibrationTime) break;
    delay(10);
  }
  if (LowestReading < (1023-(AutoAdjustWindow+ThresholdWindow))) { //Calculate lowest value within AD range
    Beep(3,2000);
    MinValue= LowestReading-20;
    MaxValue= MinValue+AutoAdjustWindow;
    WriteConfig();
    delay(300);
  }else{
    ShowOLED("Level to high!", 0,4,1);
    PlaySound(CalibrationLevelToHighMP3,10,0);
    PlayNumber(LowestReading,1);
  }  

   ShowOLED("Calibrated at: "+(String)MinValue, 0,4,1);
   PlaySound(CalibratedAtMP3,3,0);
   PlayNumber(MinValue, 0);
   ShowOLED("Adjusting finished", 0,4,1);
   PlaySound(AutoAdjustFinishedMP3,4,0);
   ShowOLED("Enjoy shooting!", 0,4,1);
   PlaySound(HaveFunShootingMP3,4,0);
   DisplaySensorReadings= false;
   UpdateDisplay();
} 

void InitKeyPad() {
  pinMode(Key1Pin,INPUT_PULLUP);
  pinMode(Key2Pin,INPUT_PULLUP);
  pinMode(Key3Pin,INPUT_PULLUP);
  pinMode(Key4Pin,INPUT_PULLUP);
  pinMode(Key5Pin,INPUT_PULLUP);
}

byte DelaySecIntr(long Time, boolean Intr) {  //optimize this
  byte KeyPressed;
  while (KeyVal()) delay(100);  Intr= Intr; //Wait till key is released
  if (Intr) {
     Time= Time*800;
     for (long X=0; X<Time; X++) {
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

void ClearOLED() {
   oled.clear();
   ShowOLED("Kedok "+(String)Version+"           (c)", 0,0,2);
}

void ShowOLED(String Text, byte X, byte Y, byte Font) {
   if (Font==1) oled.setFont(X11fixed7x14); 
   if (Font==2) oled.setFont(TimesNewRoman16_bold);
   if (Font==3) oled.setFont(System5x7);
   //if (Font==4) oled.setFont(Wendy3x5);
   if (Font==5) oled.setFont(lcdnums14x24);
   oled.setCol(X); oled.setRow(Y); //set cursor?
   oled.clearToEOL();
   oled.print(Text);
}

void UpdateDisplay() {
  if (DisplaySensorReadings) {
    byte Score;
    if (Reading <= MaxValue) Score= map(Reading,MinValue,MaxValue,10,0); else Score= 0; //Avoid oveflow 
    ShowOLED((String)Reading,43,3,5);
    //ShowOLED((String)PitchStepThreshold,0,2,2);
    ShowOLED("L:"+(String)LowestReading+"   S:"+(String)Score+"   C:"+(String)LoopCounter,0,7,3);
    LoopCounter= 0;
  }else{
    ClearOLED();
    ShowOLED("Pitch     : "+(String)LowTone+".."+(String)HighTone,0,2,3);
    ShowOLED("Sensor    : "+(String)MinValue+".."+(String)MaxValue,0,3,3);
    ShowOLED("Curve     : "+(String)Curve,0,4,3); 
    ShowOLED("Average   : "+(String)AvgModes[AverageValue],0,5,3);
    ShowOLED("Samples   : "+(String)SampleModes[SampleSpeed],0,6,3);
    ShowOLED("Pitch Jump: "+(String)PitchStepValue,0,7,3);
  }  
}

void SetNoviceMode(byte Novice) {
   if (Novice) {
     GetReadyTime= 15;
     MoveSensorWindowStepSize= 10;
     CalibrationTime= 2000;
     PlaySound(SavedNoviceUserMP3,4,0);
   }else{
     GetReadyTime= 5;
     MoveSensorWindowStepSize= 8;
     CalibrationTime= 800;
     PlaySound(SavedAdvancedUserMP3,4,0);
   }
   NoviceUser= Novice;
}

void SendMP3Command(int8_t Command, int16_t Data) {
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

void PlaySound(word Index, word Time, byte ScrnMsg) {
  SetVolume(MP3Volume);
  if (ScrnMsg) ShowOLED("Please wait...", 0,4,1);
  SendMP3Command(CMD_PLAY_FOLDER_FILE, MP3Language+Index);
  DelaySecIntr(Time,true);
}

void PlayNumber(word Nr, byte EnableTheValueText) {
  if (EnableTheValueText) PlaySound(TheValueIsMP3,2,0);
  if (Nr > 999) PlaySound(( Nr/1000),1,0);
  if (Nr > 99)  PlaySound(((Nr/100)%10),1,0);
  if (Nr > 9)   PlaySound(((Nr/10)%10),1,0);
  PlaySound((Nr%10),3,0);
}

void PlayHelp(byte Option) {
  ShowOLED("Help playing..",0,4,1);
  switch(Option) {
    case  0: PlaySound(HelpKedokUsageMP3,80,0); break;
    case  1: PlaySound(HelpSetLowestPitchMP3,7,0); break;  
    case  2: PlaySound(HelpSetHigestPitchMP3,7,0); break;
    case  3: PlaySound(HelpSetSensorAverageFilterMP3,14,0); break;
    case  4: PlaySound(HelpSetSampleSpeedMP3,8,0); break;
    case  5: PlaySound(HelpSetSoundCurveValueMP3,11,0); break;
    case  6: PlaySound(HelpSetSensorThresholdValueMP3,10,0); break;
    case  7: PlaySound(HelpSetAutoAdjustWindowMP3,11,0); break;
    case  8: PlaySound(HelpSetMinimalSensorValueMP3,13,0); break;
    case  9: PlaySound(HelpSetMaximalSensorValueMP3,13,0); break;
    case 10: PlaySound(HelpSetPitchStepValueMP3,20,0); break; 
    case 11: PlaySound(HelpStanceTrainerMP3 ,20,0); break;
    case 12: PlaySound(HelpSetTimeToGetReadyMP3,10,0); break; 
    case 13: PlaySound(HelpSetAlwaysSoundMP3,8,0); break;                                
    case 14: PlaySound(HelpSetVolumeMP3,8,0); break;
    case 15: PlaySound(HelpLoadShooterTypeMP3,8,0); break;
    case 16: PlaySound(HelpRestoreFactorySettingsMP3,10,0); break;
  }  
}

byte KeyVal() {  //optimize!!
  if (!digitalRead(Key1Pin)) {PlayTone(1000,20); return Select;}
  if (!digitalRead(Key2Pin)) {PlayTone(1000,20); return Down;}
  if (!digitalRead(Key3Pin)) {PlayTone(1000,20); return Up;}
  if (!digitalRead(Key4Pin)) {PlayTone(1000,20); return Right;}
  if (!digitalRead(Key5Pin)) {PlayTone(1000,20); return Left;}
  return None; //No key
}

byte MainMenuSelection() {
    byte OptionNr= 0;
    ClearOLED();
    ShowOLED("Settings menu", 0,0,2);
    PlaySound(SettingsMenuMP3,3,1);
    PlaySound(SelectTheOptionMP3,5,0);
    byte Esc= false;
    while(!Esc) {
      switch (OptionNr) {
          case  0: ShowOLED("[How to use]", 0,4,1); PlaySound(KedokUsageMP3,6,0); break;
          case  1: ShowOLED("[Lowest pitch]", 0,4,1); PlaySound(SetLowestPitchMP3,8,0); break;
          case  2: ShowOLED("[Higest pitch]", 0,4,1); PlaySound(SetHigestPitchMP3,8,0); break;
          case  3: ShowOLED("[Average]", 0,4,1); PlaySound(SetSensorAverageFilterMP3,8,0); break;
          case  4: ShowOLED("[Sample speed]", 0,4,1); PlaySound(SetSampleSpeedMP3,8,0); break;
          case  5: ShowOLED("[Sound curve]", 0,4,1); PlaySound(SetSoundCurveValueMP3,8,0); break;
          case  6: ShowOLED("[Sensor threshold]", 0,4,1); PlaySound(SetSensorThresholdValueMP3,8,0); break; 
          case  7: ShowOLED("[Auto adj. window]", 0,4,1); PlaySound(SetAutoAdjustWindowMP3,8,0); break;                                                  
          case  8: ShowOLED("[Minimal sensor]", 0,4,1); PlaySound(SetMinimalSensorValueMP3,8,0); break; 
          case  9: ShowOLED("[Maximal sensor]", 0,4,1); PlaySound(SetMaximalSensorValueMP3,8,0); break;
          case 10: ShowOLED("[Pitch jump]", 0,4,1); PlaySound(SetPitchStepValueMP3,8,0); break;
          case 11: ShowOLED("[Stance Trainer]", 0,4,1); PlaySound(StartPositionTrainerMP3,8,0); break; //doto
          case 12: ShowOLED("[Get ready time]", 0,4,1); PlaySound(SetTimeToGetReadyMP3,8,0); break;          
          case 13: ShowOLED("[Always sound]", 0,4,1); PlaySound(SetAlwaysSoundMP3,8,0); break;    
          case 14: ShowOLED("[Voice volume]", 0,4,1); PlaySound(SetVolumeMP3,8,0); break;
          case 15: ShowOLED("[Shooter type]", 0,4,1); PlaySound(LoadShooterTypeMP3,8,0); break;                        
          case 16: ShowOLED("[Factory settings]", 0,4,1); PlaySound(RestoreFactorySettingsMP3,6,0); break; 
           
     } 
     if (KeyVal() == Up)       if (OptionNr < 16) OptionNr+= 1;
     if (KeyVal() == Down)     if (OptionNr >  0) OptionNr-= 1;
     if (KeyVal() == Select)   Esc= true; 
     if (KeyVal() == Right)    PlayHelp(OptionNr);  
     if (KeyVal() == Left)     return 255; 
  }
  return OptionNr;
}

void OptionsMenu(byte Option) {
    byte Esc= false;
    if ((Option != 0) && (Option != 11) ) PlaySound(UseArrowKeysMP3,6,1);  //do not play if help or trainer is selected, bit dirty
    switch(Option) {
        case  0: Esc= false;
                 while (!Esc) {
                    ShowOLED("Help playing..", 0,4,1);
                    PlayHelp(Option);
                    if (KeyVal() == Right) {ShowOLED("Reading manual..", 0,4,1); PlaySound(HelpKedokExtHelpMP3,308,0); break;}  
                    if (KeyVal() == Left)  Esc= true;                   
                 }   
                 break;  
        case  1: Esc= false;
                 while (!Esc) {
                    ShowOLED("Lo Pitch: "+(String)LowTone+" Hz", 0,4,1);
                    PlayTone(LowTone,0);
                    delay(300);
                    if (KeyVal() == Down)     if (LowTone > 50) LowTone-= 50;
                    if (KeyVal() == Up)       if (LowTone < (HighTone-100)) LowTone+= 50;
                    if (KeyVal() == Right)    PlayHelp(Option); 
                    if (KeyVal() == Left)     Esc= true;
                    if (KeyVal() == Select)   {WriteConfig(); PlaySound(LowPitchSetAtMP3,4,0); PlayNumber(LowTone, 0); PlaySound(HertzMP3,2,0); Esc= true;}
                 }   
                 break;    
        case  2: Esc= false;
                 while (!Esc) {
                    ShowOLED("Hi Pitch: "+(String)HighTone+" Hz", 0,4,1);
                    PlayTone(HighTone,0);
                    delay(300);
                    if (KeyVal() == Down)     if (HighTone > (LowTone+100)) HighTone-= 50;
                    if (KeyVal() == Up)       if (HighTone < 9000) HighTone+= 50;
                    if (KeyVal() == Right)    PlayHelp(Option); 
                    if (KeyVal() == Left)     Esc= true;
                    if (KeyVal() == Select)   {WriteConfig(); PlaySound(HighPitchSetAtMP3,4,0); PlayNumber(HighTone, 0); PlaySound(HertzMP3,2,0); Esc= true;}
                 }
                 break; 
       case   3: Esc= false;
                 while (!Esc) {
                    ShowOLED("Averaging: "+(String)AvgModes[AverageValue], 0,4,1);
                    if (AverageValue == 0)    PlaySound(DisableMP3,4,0); 
                    if (AverageValue == 1)    PlaySound(LowMP3,4,0);
                    if (AverageValue == 2)    PlaySound(MediumMP3,4,0); 
                    if (AverageValue == 3)    PlaySound(HighMP3,4,0);
                    if (KeyVal() == Down)     if (AverageValue > 0)       AverageValue-= 1;
                    if (KeyVal() == Up)       if (AverageValue < 3)       AverageValue+= 1;
                    if (KeyVal() == Right)    PlayHelp(Option); 
                    if (KeyVal() == Left)     Esc= true;
                    if (KeyVal() == Select)   {WriteConfig(); Esc= true;}
                 }
                 break;  
       case   4: Esc= false;
                 while (!Esc) {
                   ShowOLED("Speed: "+(String)SampleModes[SampleSpeed], 0,4,1);
                   if (SampleSpeed == 0)     PlaySound(FastMP3,4,0);
                   if (SampleSpeed == 1)     PlaySound(MediumMP3,4,0); 
                   if (SampleSpeed == 2)     PlaySound(SlowMP3,4,0);
                   if (SampleSpeed == 3)     PlaySound(SlowestMP3,4,0);
                   if (KeyVal() == Down)     if (SampleSpeed > 0)       SampleSpeed-= 1;
                   if (KeyVal() == Up)       if (SampleSpeed < 3)       SampleSpeed+= 1;
                   if (KeyVal() == Right)    PlayHelp(Option); 
                   if (KeyVal() == Left)     Esc= true;
                   if (KeyVal() == Select)   {WriteConfig(); Esc= true;}
                 }
                 break;
       case   5: Esc= false;
                 while (!Esc) {
                      ShowOLED("Pitch Curve: "+(String)Curve, 0,4,1);
                      PlayNumber(Curve, 1);
                      if (KeyVal() == Up)       if (Curve < 5) Curve++;
                      if (KeyVal() == Down)     if (Curve > 0) Curve--;
                      if (KeyVal() == Right)    PlayHelp(Option); 
                      if (KeyVal() == Left)     Esc= true;
                      if (KeyVal() == Select)   {WriteConfig(); Esc= true;}
                 }
                 break;                  
       case   6: Esc= false;
                 while (!Esc) {
                    ShowOLED("Threshold: "+(String)ThresholdWindow, 0,4,1);
                    PlayNumber(ThresholdWindow, 1);
                    if (KeyVal() == Up)       if (ThresholdWindow < 240) ThresholdWindow+= 10;
                    if (KeyVal() == Down)     if (ThresholdWindow > 10)  ThresholdWindow-= 10;
                    if (KeyVal() == Right)    PlayHelp(Option); 
                    if (KeyVal() == Left)     Esc= true;
                    if (KeyVal() == Select)   {WriteConfig(); Esc= true;}
                 }
                 break; 
       case   7: Esc= false;
                 while (!Esc) {
                     ShowOLED("Auto window: "+(String)AutoAdjustWindow, 0,4,1);
                     PlayNumber(AutoAdjustWindow, 1);
                     if (KeyVal() == Down)     if (AutoAdjustWindow > 50)   AutoAdjustWindow-= 10;
                     if (KeyVal() == Up)       if (AutoAdjustWindow < 300)  AutoAdjustWindow+= 10;
                     if (KeyVal() == Right)    PlayHelp(Option); 
                     if (KeyVal() == Left)     Esc= true;
                     if (KeyVal() == Select)   {WriteConfig(); Esc= true;}
                 }
                 break;   
       case   8: Esc= false;
                 while (!Esc) {
                     ShowOLED("Min. Sensor: "+(String)MinValue, 0,4,1);
                     PlayNumber(MinValue, 1);
                     if (KeyVal() == Up)       if (MinValue < (MaxValue-20)) MinValue+= 5;
                     if (KeyVal() == Down)     if (MinValue > 10) MinValue-= 5;
                     if (KeyVal() == Right)    PlayHelp(Option); 
                     if (KeyVal() == Left)     Esc= true;
                     if (KeyVal() == Select)   {WriteConfig(); Esc= true;}
                 }
                 break;
      case    9: Esc= false;
                 while (!Esc) {
                    ShowOLED("Max. Sensor: "+(String)MaxValue, 0,4,1);
                    PlayNumber(MaxValue, 1);
                    if (KeyVal() == Up)       if (MaxValue < 990) MaxValue+= 5;
                    if (KeyVal() == Down)     if (MaxValue > (MinValue+20)) MaxValue-= 5;
                    if (KeyVal() == Right)    PlayHelp(Option); 
                    if (KeyVal() == Left)     Esc= true;
                    if (KeyVal() == Select)   {WriteConfig(); Esc= true;}
                 }
                 break;
      case   10: Esc= false;
                 while (!Esc) {
                     ShowOLED("Pitch jump: "+(String)PitchStepValue, 0,4,1);
                     PlayNumber(PitchStepValue, 1);
                     if (KeyVal() == Up)       if (PitchStepValue < 9) PitchStepValue++;
                     if (KeyVal() == Down)     if (PitchStepValue > 0) PitchStepValue--;
                     if (KeyVal() == Right)    PlayHelp(Option); 
                     if (KeyVal() == Left)     Esc= true;
                     if (KeyVal() == Select)   {WriteConfig(); Esc= true;}
                 }
                 break; 
       case  11: Esc= false;
                 while (!Esc) {
                    ShowOLED("Menu to start: ",0,4,1);
                    PlaySound(PressMenuToStartMP3,6,1);
                    if (KeyVal() == Select)    {StanceTrainer(); Esc= true;} 
                    if (KeyVal() == Right)     PlayHelp(Option); 
                    if (KeyVal() == Left)      Esc= true;                   
                 }
                 break;
       case  12: Esc= false;
                 while (!Esc) {
                   ShowOLED("Timer: "+(String)GetReadyTime+" sec.", 0,4,1);
                   PlayNumber(GetReadyTime, 1); PlaySound(SecondsMP3,2,0);
                   if (KeyVal() == Down)     if (GetReadyTime > 2)       GetReadyTime-= 1;
                   if (KeyVal() == Up)       if (GetReadyTime < 20)      GetReadyTime+= 1;
                   if (KeyVal() == Right)    PlayHelp(Option); 
                   if (KeyVal() == Left)     Esc= true;
                   if (KeyVal() == Select)   {WriteConfig(); Esc= true;}
                }
                break; 
       case 13: Esc= false;
                while (!Esc) {
                    ShowOLED("Always Sound: "+(String)YesNoArr[AlwaysSound], 0,4,1);
                    if (AlwaysSound) PlaySound(AlwaysSoundEnabledMP3,3,0); else PlaySound(AlwaysSoundDisabledMP3,3,0); 
                    if (KeyVal() == Up)       if (AlwaysSound < 1) AlwaysSound++;
                    if (KeyVal() == Down)     if (AlwaysSound > 0) AlwaysSound--;
                    if (KeyVal() == Right)    PlayHelp(Option); 
                    if (KeyVal() == Left)     Esc= true;
                    if (KeyVal() == Select)   {WriteConfig(); Esc= true;}
                }
                break;                                                                                                                                                     
       case 14: Esc= false;
                while (!Esc) {
                    ShowOLED("Volume: "+(String)MP3Volume, 0,4,1);
                    PlayNumber(MP3Volume, 1);
                    if (KeyVal() == Up)       if (MP3Volume < 30) MP3Volume+= 1;
                    if (KeyVal() == Down)     if (MP3Volume >  5) MP3Volume-= 1;
                    if (KeyVal() == Right)    PlayHelp(Option); 
                    if (KeyVal() == Left)     Esc= true;
                    if (KeyVal() == Select)   {WriteConfig(); Esc= true;}
                }
                break;
      case  15: { Esc= false;
                  boolean AdvancedUser= false; 
                  while (!Esc) {
                    ShowOLED("Advanced Usr: "+(String)YesNoArr[AdvancedUser], 0,4,1);
                    if (AdvancedUser) PlaySound(LoadAdvancedUserMP3,6,0); else PlaySound(LoadNoviceUserMP3,6,0); 
                    if (KeyVal() == Up)       if (!AdvancedUser) AdvancedUser= true;
                    if (KeyVal() == Down)     if (AdvancedUser)  AdvancedUser= false;
                    if (KeyVal() == Right)    PlayHelp(Option); 
                    if (KeyVal() == Left)     Esc= true;
                    if (KeyVal() == Select)   {SetNoviceMode(!AdvancedUser); WriteConfig(); Esc= true;}
                  }
                }  
                break;                
      case 16: { Esc= false;
                 boolean SetToDefaults= false; 
                 while (!Esc) {
                    ShowOLED("Factory reset: "+(String)YesNoArr[SetToDefaults], 0,4,1);
                    if (SetToDefaults) PlaySound(RestoreFactoryDefaultsEnabledMP3,4,0); else PlaySound(RestoreFactoryDefaultsDisabledMP3,4,0); 
                    if (KeyVal() == Up)        if (!SetToDefaults) SetToDefaults= true; 
                    if (KeyVal() == Down)      if (SetToDefaults)  SetToDefaults= false;
                    if (KeyVal() == Right)     PlayHelp(Option); 
                    if (KeyVal() == Left)      Esc= true;
                    if (KeyVal() == Select)    if (SetToDefaults) {
                                                   ShowOLED("Reset to defaults.", 0,4,1);
                                                   PlaySound(AllSettingsResetToDefaultsMP3,4,0);
                                                   EEPROM.write(0,0); 
                                                   Reset(); 
                                               }
                 }
              }
              break;
    }  
}
  
void Menu() {
  byte OptionSelect;  //maybe static???
  PlayTone(0,0);
  while(OptionSelect != 255) {
    OptionSelect= MainMenuSelection();
    if (OptionSelect != 255) OptionsMenu(OptionSelect); //dirty !!!!! use do while?
    ReadConfig(true); //Only use stored settings, no disp or sound
    PlayTone(0,0); //needed?
  }
  ShowOLED("Leaving menu", 0,4,1);
  PlaySound(ExitOptionsMenuMP3,4,0);
  DisplaySensorReadings= false;
  UpdateDisplay();
}

void setup() {
  Beep(1,440); //Let user know we started
  //Pinmode for audio pin????  
  //pinMode(AudioPin, OUTPUT);
  pinMode(SensorPin, INPUT);
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  InitKeyPad();
  ClearOLED();
  ShowOLED("Batt: " +(String)ReadVcc()+"mV",0,6,3); //make init screen must be better!!!
  ShowOLED("Build: "+(String)__DATE__,0,7,3);
  //WriteConfig(); //enable to overwrite settings with defaults
  if (EEPROM.read(0)==1) ReadConfig(false); //Check if eeprom is empty (new)
  else WriteConfig();
  LowestReading= MaxValue;
  WarningReading= MinValue;
  mySerial.begin(9600);
  delay(500);//Wait audio chip initialization is complete
  SendMP3Command(CMD_SEL_DEV, DEV_TF);//select the TF card  
  delay(500);
  SetVolume(MP3Volume);
  PlaySound(WelcomeMP3,4,0);
  #ifdef SENSOR_TEST
      SensorTest();
  #endif
  for (byte X=0; X<255; X++) ReadValue(AverageValue); //clear EMA filter needed???
  UpdateDisplay();
}

void loop() {
  Reading= ReadValue(AverageValue);
  if (DisplaySensorReadings) {
     if ((millis()-PrevDispTime) > DispUpdTime) {
        UpdateDisplay();
        PrevDispTime= millis();
     }
  } 
   
 //--------Kernel part-------- 
  if (Reading < MinValue) {
     LowReadWarning(); 
  }else if (Reading < MaxValue) {
     AudioTone= fscale(MinValue,MaxValue,HighTone,LowTone,Reading,Curve);
     PitchStepThreshold= MaxValue-(((MaxValue-MinValue)/10)*PitchStepValue); //Try to get this out the loop for speed
     if (Reading < PitchStepThreshold) AudioTone= AudioTone+300;
     PlayTone(AudioTone,0);
  }else if (Reading < (MaxValue+ThresholdWindow)) {
     PlayTone(LowTone-30,0);  
  }else if (AlwaysSound) PlayTone(1,0); // 1 Hz if not on card. 
        else PlayTone(0,0); // Turn off the tone. 
//-----------------

  if (Reading < LowestReading) LowestReading= analogRead(SensorPin); //Dirty, but avoids storing an average reading

  KeyPressed= KeyVal();
  if (KeyPressed) {  
    if (KeyPressed == Select)    Menu();
    if (KeyPressed == Right)     AutoAdjust();
    if (KeyPressed == Left)      { 
                                   ClearOLED();
                                   LowestReading= MaxValue; //Reset low read.
                                   delay(300);
                                   DisplaySensorReadings= !DisplaySensorReadings;
                                   UpdateDisplay();
                                 }
    if (KeyPressed == Down)      MoveSensorWindow(0-MoveSensorWindowStepSize);
    if (KeyPressed == Up)        MoveSensorWindow(MoveSensorWindowStepSize);
    //ToDo add more
  }

  if (SampleSpeed) delay((1 << (SampleSpeed-1))*5); //0,5,10,20  

  LoopCounter++;
}



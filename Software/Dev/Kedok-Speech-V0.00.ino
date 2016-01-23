
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
#define EnableMP3                           13
#define DisableMP3                          14
#define WelcomeMP3                          31
#define SettingsMenuMP3                     28
#define UseArrowKeysMP3                     30
#define SetMinimalSensorValueMP3            23
#define SetMaximalSensorValueMP3            22
#define SetSensorThresholdVlaueMP3          25
#define SetSoundCurveValueMP3               26
#define SetPichReverseMP3                   24
#define SetLowestPitchMP3                   21
#define SetHigestPitchMP3                   20
#define SetAutoAdjustWindowMP3              19
#define RestoreFactorySettingsMP3           18
#define LowerMinimalSettingMP3              15
#define PrepareWeaponForAimingMP3           17
#define StartToAimNowMP3                    29
#define AutoAdjustFinishedMP3               12
#define CountDownFrom20MP3                  11
#define SetVolumeMP3                        27
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


#include <SoftwareSerial.h>

#define Key1Pin 3
#define Key2Pin 2
#define Key3Pin 5
#define Key4Pin 4

#define NoKey   0
#define Select  1
#define Down    2
#define Up      3
#define Enter   4

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

SoftwareSerial mySerial(ARDUINO_RX, ARDUINO_TX);

word    MP3Language=          0X0200; //01 English 02 Dutch
byte    MP3Volume=                25;
word    MinValue=                100;
word    MaxValue=                800;
word    LowTone=                 100; 
word    HighTone=               1750;
byte    Curve=                     0;
byte    PitchRev=              false;  
byte    AlwaysSound=           false;
word    AutoAdjustWindow=        200;
byte    ThresholdWindow=         150;    

static int8_t Send_buf[8]= {0} ;

void InitKeyPad() {
  pinMode(Key1Pin,INPUT_PULLUP);
  pinMode(Key2Pin,INPUT_PULLUP);
  pinMode(Key3Pin,INPUT_PULLUP);
  pinMode(Key4Pin,INPUT_PULLUP);
}

byte ReadKey() {
  if (!digitalRead(Key1Pin)) return Select;
  if (!digitalRead(Key2Pin)) return Up;
  if (!digitalRead(Key3Pin)) return Down;
  if (!digitalRead(Key4Pin)) return Enter;
  return 0;
}

void DelaySecIntr(word Time, boolean Intr) {
  while (ReadKey()) Intr= Intr; //Wait till key is released
  if (Intr) {
     Time= Time*800;
     for (word X=0; X<Time; X++) {
        if (ReadKey()) X= Time;
        delay(1);
     }  
  }else delay(Time); 
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

void PlaySound(word Index, word Time) {
  Serial.println("Play sound nr: "+(String)Index);
  SendMP3Command(CMD_PLAY_FOLDER_FILE, MP3Language+Index);
  DelaySecIntr(Time,true);
}

void PlayNumber(word Nr, byte Digits) {
  PlaySound(TheValueIsMP3,2);
  if (Digits>3) PlaySound((Nr/1000),1);
  if (Digits>2) PlaySound(((Nr/100)%10),1);
  if (Digits>1) PlaySound(((Nr/10)%10),1);
  PlaySound((Nr%10),2);
}

String WordToStr(word Inp, byte Size) {
  String Str;
  for (byte C= 0; C<Size; C++) Str+= "0";
  String WordStr= (String)Inp;
  byte Len= WordStr.length();
  for (byte C= 0; C<Len; C++) Str[C+Str.length()-Len]= WordStr[C];
  return Str; 
} 


void Menu() {
  byte OptionNr= 0;
  PlaySound(SettingsMenuMP3,3);
  PlaySound(SelectTheOptionMP3,6);
  boolean Esc= false;
  while(!Esc) {
    switch (OptionNr) {
      case  0: PlaySound(SetVolumeMP3,8); break;
      case  1: PlaySound(SetMinimalSensorValueMP3,8); break; 
      case  2: PlaySound(SetMaximalSensorValueMP3,8); break;
      case  3: PlaySound(SetSensorThresholdVlaueMP3,8); break;  
      case  4: PlaySound(SetSoundCurveValueMP3,8); break;
      case  5: PlaySound(SetPichReverseMP3,8); break;
      case  6: PlaySound(SetLowestPitchMP3,8); break;
      case  7: PlaySound(SetHigestPitchMP3,8); break;
      case  8: PlaySound(SetAutoAdjustWindowMP3,8); break;
      case  9: PlaySound(RestoreFactorySettingsMP3,8); break;                
    } 
    Serial.println("Option Nr: "+String(OptionNr)); 
    if (ReadKey() == Up)       if (OptionNr < 10) OptionNr+= 1;
    if (ReadKey() == Down)     if (OptionNr >  0) OptionNr-= 1;
    if (ReadKey() == Enter)   Esc= true;      
  }
  switch(OptionNr) {
    case 0: Esc= false;
            while (!Esc) {
               PlayNumber(MP3Volume,2);
               if (ReadKey() == Up)       if (MP3Volume < 30) MP3Volume+= 1;
               if (ReadKey() == Down)     if (MP3Volume >  5) MP3Volume-= 1;
               if (ReadKey() == Enter)    Esc= true;
               SetVolume(MP3Volume);
            }
            break;
    case 1: Esc= false;
            while (!Esc) {
               PlayNumber(MinValue,4);
               if (ReadKey() == Up)       if (MinValue < (MaxValue-20)) MinValue+= 5;
               if (ReadKey() == Down)     if (MinValue > 10) MinValue-= 5;
               if (ReadKey() == Enter)    Esc= true;
            }
            break;
    case 2: Esc= false;
            while (!Esc) {
               PlayNumber(MaxValue,4);
               if (ReadKey() == Up)       if (MaxValue < 990) MaxValue+= 5;
               if (ReadKey() == Down)     if (MaxValue > (MinValue+20)) MaxValue-= 5;
               if (ReadKey() == Enter)    Esc= true;
            }
            break;
    case 3: Esc= false;
            while (!Esc) {
               PlayNumber(ThresholdWindow,3);
               if (ReadKey() == Up)       if (ThresholdWindow < 190) ThresholdWindow+= 10;
               if (ReadKey() == Down)     if (ThresholdWindow > 10)  ThresholdWindow-= 10;
               if (ReadKey() == Enter)    Esc= true;
            }
            break;     
    case 4: Esc= false;
            while (!Esc) {
               PlayNumber(Curve,1);
               if (ReadKey() == Up)       if (Curve < 5) Curve++;
               if (ReadKey() == Down)     if (Curve > 0) Curve--;
               if (ReadKey() == Enter)    Esc= true;
            }
            break;  
    case 5: Esc= false;
            while (!Esc) {
               if (PitchRev) PlaySound(PitchReverseEnabledMP3,3); else PlaySound(PitchReverseDisabledMP3,3); 
               if (ReadKey() == Up)       if (PitchRev < 1) PitchRev++;
               if (ReadKey() == Down)     if (PitchRev > 0) PitchRev--;
               if (ReadKey() == Enter)    Esc= true;
            }
            break;
    case 6: Esc= false;
            while (!Esc) {
               if (PitchRev) PlaySound(AlwaysSoundEnabledMP3,3); else PlaySound(AlwaysSoundDisabledMP3,3); 
               if (ReadKey() == Up)       if (AlwaysSound < 1) AlwaysSound++;
               if (ReadKey() == Down)     if (AlwaysSound > 0) AlwaysSound--;
               if (ReadKey() == Enter)    Esc= true;
            }
            break;
    case 9: Esc= false;
            boolean SetToDefaults= false; 
            while (!Esc) {
               if (SetToDefaults) PlaySound(RestoreFactoryDefaultsEnabledMP3,4); else PlaySound(RestoreFactoryDefaultsDisabledMP3,4); 
               if (ReadKey() == Up)       if (SetToDefaults < 1) SetToDefaults= true;
               if (ReadKey() == Down)     if (SetToDefaults > 0) SetToDefaults= false;
               if (ReadKey() == Enter)    Esc= true;
            }
            if (SetToDefaults) {
               PlaySound(AllSettingsResetToDefaultsMP3,4);
               //Resore factory settings
            }
           break;                        
  }  
  PlaySound(ExitOptionsMenuMP3,4);
  //PlaySound(DataSettingsSavedMP3,3);
} 

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  delay(500);//Wait chip initialization is complete
  SendMP3Command(CMD_SEL_DEV, DEV_TF);//select the TF card  
  delay(200);//wait for 200ms
  SetVolume(MP3Volume);
  PlaySound(WelcomeMP3,3);//play the intro
  InitKeyPad();
}

void loop() {
  if (ReadKey()==Select) Menu();
  delay(10); 
}

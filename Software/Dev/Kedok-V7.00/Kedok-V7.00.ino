#include <SD.h>
#include <SPI.h>
#include <AudioZero.h>

void setup()
{
  // debug output at 115200 baud
  Serial.begin(115200);

  // setup SD-card
  Serial.print("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println(" failed!");
    while(true);
  }
  Serial.println(" done.");

  // 44100kHz stereo => 88200 sample rate
  AudioZero.begin(2*44100);
}

void loop()
{
  int count = 0;

  // open wave file from sdcard
  File myFile = SD.open("test.wav");
  if (!myFile) {
    // if the file didn't open, print an error and stop
    Serial.println("error opening test.wav");
    while (true);
  }

  Serial.print("Playing");
 
  // until the file is not finished  
  AudioZero.play(myFile);

  Serial.println("End of file. Thank you for listening!");
  while (true) ;
}void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

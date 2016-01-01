#include <SPI.h>

const int SINE     = 0x2000;                // Define AD9833's WaveShapes 
const int SQUARE   = 0x2020;                
const int TRIANGLE = 0x2002;                  

const float CrystalFreq = 25000000.0;       // On-board X-TAL reference frequency.
const int   FSYNC = 10;                     // AD9833 Chipselect Pin

void setup() { 
  pinMode(FSYNC, OUTPUT);
  SPI.begin();
  SPI.setDataMode(SPI_MODE2); 
  delay(50); 
  AD9833reset();                             // Reset AD9833.
  delay(50);
  SetFrequency(0, SINE);                     // Set the frequency and Sine Wave output
}

void loop() {
  for (int x=50; x<4000; x+=10) {
    SetFrequency(x, SINE);
    delay(10);
  }
}

void AD9833reset() {
  WriteToDDS(0x100);   // Write '1' to AD9833 Control register bit D8.
  delay(10);
}

// Set AD8933 frequency and wave shape registers.
void SetFrequency(long Frequency, int WaveShape) {
  
  long FreqWord = (Frequency * pow(2, 28)) / CrystalFreq;

  int MSB = (int)((FreqWord & 0xFFFC000) >> 14);    //Only lower 14 bits are used for data
  int LSB = (int)( FreqWord & 0x3FFF);
  
  //Set control bits 15 ande 14 to 0 and 1, respectively, for frequency register 0
  LSB |= 0x4000;
  MSB |= 0x4000; 
  
  WriteToDDS(1 << 13);              // B28 for 16 bits updates
  WriteToDDS(LSB);                  // Write lower 16 bits to AD9833 registers
  WriteToDDS(MSB);                  // Write upper 16 bits to AD9833 registers.
  WriteToDDS(0xC000);               // Phase register
  WriteToDDS(WaveShape);            // Exit & Reset to SINE, SQUARE or TRIANGLE
}

void WriteToDDS(int Data) { 
  
  digitalWrite(FSYNC, LOW);           // Set FSYNC low before writing to AD9833 registers
  delayMicroseconds(5);               // Give AD9833 time to get ready to receive data.
  
  SPI.transfer(highByte(Data));       // Each AD9833 register is 32 bits wide and each 16
  SPI.transfer(lowByte (Data));       // bits has to be transferred as 2 x 8-bit bytes.

  digitalWrite(FSYNC, HIGH);          //Write done. Set FSYNC high
}

